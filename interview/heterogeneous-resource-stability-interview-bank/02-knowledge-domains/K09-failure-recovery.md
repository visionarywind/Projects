# K09：错误传播、超时、取消、恢复与故障注入

## 边界

本知识域讨论错误如何从设备/驱动一路上报，以及超时、取消、重试、回滚和故障注入如何共同决定一次请求的最终状态。

讨论对象是“错误的生命周期和恢复决策”，不是可观测性工具本身（那是 K10）、也不是容量与成本（那是 K11）。分层错误码、错误类别、`SUCCESS/FAILURE/UNKNOWN` 三态、故障窗口和回滚矩阵，是本域的事实边界。

特定厂商的取消语义、设备 reset 行为和驱动重试策略必须绑定目标版本。文档确认了错误分层模式和几处“明确不回滚”的窗口，但没有在真实设备上验证取消与 reset 的语义，因此这些结论按 `[待验证]` 处理。

## 核心机制

### 1. 错误分层链

`[已确认]` 错误沿分层结构向上传播并逐层转换：

```text
Driver drvError_t / errno / ioctl result
  -> HAL return
  -> Runtime rtError_t / extended error
  -> ACL aclError 或 GE Status/graphStatus
  -> synchronous return / async callback / log
```

转换点包括 Runtime 统一错误门面和 ACL 设备包装；GE 异步回调在失败时记录错误并转换 Tensor。[cann/90-cross-module/error-boundaries.md:16-24]

`[已确认]` 每层如果只保留映射后的顶码，就会丢掉第一个失败边界；诊断应优先保存最底层错误码、设备 ID、Context/Stream、模型/Graph 标识和调用阶段。[cann/00-overview/global-error-model.md:39-43]

### 2. 错误类别与调用方动作

`[已确认]` 错误按类别区分，不同类别对应不同动作：[cann/00-overview/global-error-model.md:29-37]

| 类别 | 示例 | 调用方动作 |
|---|---|---|
| 参数错误 | 空指针、数量不匹配、非法设备 ID | 修正参数，不重试同一请求 |
| 状态错误 | 未初始化、Executor 非 Loaded、活动 session 存在 | 按生命周期补齐或释放 |
| 资源错误 | 内存、Stream、Queue、HDC 资源不足 | 记录上下文，按策略清理/重试 |
| 设备/驱动错误 | 设备不可用、IOCTL/HDC 失败 | 检查驱动、固件、设备日志 |
| 编译/模型错误 | 图不合法、shape/算子不支持 | 回到模型和编译配置排查 |
| 异步错误 | callback 返回失败、任务执行失败 | 等完成通知并保留原始错误上下文 |

### 3. 三态判定：SUCCESS / FAILURE / UNKNOWN

超时、错误传播和取消是三条不同路径，但汇合到同一个“未知”风险。把每个请求的结果分成三态：

```text
SUCCESS: 完成证据 + 结果校验通过  -> 释放资源
FAILURE: 观察到明确错误           -> 按幂等性重试 / 降级
UNKNOWN: 超时 / 取消未确认         -> 隔离，不释放，不盲重试
```

`[推断]` 该三态由“超时只表示窗口内没有完成证据、取消不等于已回收”推出。

必须区分两个概念：

```text
timeout  ⇒ 观察窗口内 no_completion_evidence  (任务可能仍在设备上)
cancel_ok ≠ resource_reclaimed               (取消成功不等于资源已回收)
```

### 4. 一个必须测试的故障窗口

`[已确认]` SOMA async free 先删除本地 VMM segment，再发送 `SVM_SOMA_MEM_FREE`，源码明确标注 ioctl 失败“不回滚”；上层 Runtime 元数据、Driver 用户态 VMM segment、远端设备 pool client 和 AICPU 操作可能短暂处于不同状态。[cann/90-cross-module/memory-and-resource-lifecycle.md:91-97]

```text
SOMA free: 删除本地 VMM segment --(ioctl 失败)--> 设备侧未回滚
=> 本地元数据与设备侧状态可能不一致（故障窗口）
```

这类窗口是故障注入的重点：上层以为已释放，底层仍持有；或相反。处理它的原则是把不一致隔离、保留证据、按对账/幂等策略收敛，而不是假设“调用链返回即一致”。

### 5. 回滚矩阵（已确认部分）

`[已确认]` 文档汇总了若干故障点的清理与残余状态：[cann/90-cross-module/memory-and-resource-lifecycle.md:103-114]

| 故障点 | 已确认的清理/回滚 | 需验证的残余 |
|---|---|---|
| Context Setup 失败 | `NewContext` 调 `TearDown` 后 delete | Device retain 引用与 feature-specific 资源 |
| Stream Setup 失败 | 删除新建 default Stream | Driver SQ/CQ 是否完全回收 |
| SOMA HAL create 失败 | 释放已保留 VA / global VA / client 中间资源 | 各后端 destroy 语义 |
| SOMA AICPU launch 失败 | 尝试本地 `FreeToMemPool` | cached / 设备配置 / sequence 一致性 |
| SOMA V3 free ioctl 失败 | 已删除 VMM segment，源码标注不回滚 | 重试、泄漏与地址可见性 |
| ordinary `halMemAlloc` 失败 | 映射 drv error 并记录 flags/module/device | Driver 内部部分分配残留 |
| Context/Stream teardown 失败 | 恢复 owner stream 或保留对象以便重试 | 设备 pending task 与 callback 状态 |

### 6. 恢复决策伪代码

```python
def handle_result(req, result):
    if result is COMPLETED and verify(req) == OK:
        return STATE_SUCCESS              # 只有一个终态
    if result is ERROR and result.kind is not UNKNOWN:
        return STATE_FAILURE
    return STATE_UNKNOWN                  # timeout / cancel unconfirmed

def recover(req, state):
    if state is STATE_SUCCESS:
        release(req)                      # 完成证据齐备才释放
    elif state is STATE_FAILURE:
        if idempotent(req) and retry_budget(req) > 0:
            backoff_then_retry(req)
        else:
            degrade_or_reject(req)
    else:  # UNKNOWN
        isolate_fault_domain(req)         # 停新提交、限流、标记故障域
        capture_evidence(req)             # 保留 memory/model/stream/callback
        # 不释放、不盲重试；等完成证据或按对账收敛
```

`isolate_fault_domain`、`capture_evidence` 是可读模型；真实设备需绑定驱动的 reset/查询能力。

### 7. 故障注入点

`[已确认]` 测试配方把异步路径列为必测：callback/event、超时、错误、资源提前销毁；跨模块要覆盖 ACL→Runtime 错误映射、GE→Runtime 执行、Runtime→HAL。[cann/99-roadmap/testing-recipes.md:14-24]

`[已确认]` 风险登记列出“异步资源提前释放”“错误跨层映射丢上下文”“ioctl/DMA 用户参数校验不足”等静态风险。[cann/99-roadmap/risk-register.md:15-25]

```text
注入点: submit | driver_accept | device_exec | wait | callback | cleanup
注入内容: 错误码 | 延迟 | 取消 | 重复回调 | ioctl 失败不回滚
期望: 每请求唯一终态；UNKNOWN 不进成功/重试分支；未证明完成前引用不归零
```

### 8. 失败模式

- 丢弃原始错误：顶层 `ACL_ERROR` 掩盖 Driver/HAL 的第一失败边界。
- 超时当失败：重复执行副作用，放大拥塞。
- 取消当回收：取消成功但仍持有内存/句柄，随后被覆盖。
- 盲目补偿：对非幂等操作重试，产生重复副作用。
- 假设原子：忽略“不回滚”窗口，元数据与设备状态长期不一致。

### 9. 超时预算、退避与熔断

`[推断]` 超时不是孤立参数，而是预算：一次端到端请求由多层超时组成，级联超时必须逐层收敛，否则上层很早放弃、下层仍在长期占用资源。

```text
t_total  >= t_submit + t_driver + t_device + t_wait
backoff  : 重试间隔随失败次数指数增长，并设上限
circuit  : 某故障域连续失败达阈值即熔断，停止新提交，避免放大
```

熔断的作用域应与故障域一致（见 K06）：设备级故障熔断该设备，链路级故障熔断该链路，而不是熔断整个集群。

### 10. 恢复时间的分解

`[推断]` 恢复不能只说“多久恢复”，要拆成可观测的分段：

```text
t_detect    : 从故障发生到被观测到
t_isolate   : 从检测到完成故障域隔离（停新提交、限流）
t_recover   : 从隔离到服务恢复（重试/迁移/降级/对账）
t_verify    : 从恢复到确认结果正确并清理残留
```

`[已确认]` 风险登记把“异步资源提前释放”“错误跨层映射丢上下文”列为静态风险，说明 `t_verify` 阶段必须以资源计数归零和错误上下文完整为判据。[cann/99-roadmap/risk-register.md:15-25]

## 常见失效条件

- 丢弃原始错误码、只记录顶层错误，无法定位第一失败边界。
- 无限重试或未经幂等性判断就执行补偿。
- 把超时判定为“未执行”并直接重试同一个非幂等请求。
- 把取消成功等同于资源已回收，提前释放被设备引用的对象。
- 在 `UNKNOWN` 状态下释放 memory/model/stream/callback。
- 忽略跨层不一致窗口，把它当成“调用返回即一致”。

## 关键不变量

```text
I1  每个请求恰好一个终态（complete / fail / cancelled），不重复、不缺失
I2  UNKNOWN 状态下不得释放被任务引用的资源
I3  每个错误保留最底层错误码、阶段与资源上下文
I4  UNKNOWN 不得进入 SUCCESS 分支，也不得无限重试
I5  重试/补偿仅在操作幂等且预算允许时执行
I6  跨层不一致窗口（如 ioctl 失败不回滚）必须被显式识别和对账
I7  资源引用计数在完成证据齐备后才允许归零
```

## 与其他知识域的连接

- **K05 异步完成与可见性**：三态判定建立在异步完成语义之上；“完成证据”的定义来自该域。
- **K03 Context/Stream 生命周期**：回滚顺序与 teardown 状态机直接依赖生命周期规则。
- **K07 / K08 传输与集合**：超时、取消、部分失败在传输和集合通信里表现为 `UNKNOWN` 与死锁。
- **K10 可观测性**：错误上下文、证据链和 SLO 是恢复决策的输入；K09 定义“保留什么证据”，K10 定义“如何采集与关联”。
- **K12 生产演进**：驱动/固件升级、reset 和灾备切换都是 K09 恢复路径的放大版。
- **K06 调度与隔离**：故障隔离通过调度侧的故障域、限流和移除受影响资源落地。

## 题目索引

本域对应题目（按 `知识域：` 字段确认）：`HRS-B010`、`HRS-I009`、`HRS-A009`、`HRS-A013`、`HRS-E009`、`HRS-E013`；并与 `HRS-B004` 关联（K03 主域，标注关联 K09）。

- `HRS-B010` 概念题：异步任务经超时、错误传播和取消后可能落到哪些状态。
- `HRS-I009` 分析题：构建跨层错误传播、超时、取消与故障注入闭环。
- `HRS-A009` 设计题：故障传播控制与恢复策略。
- `HRS-A013` 设计题：设备级故障的隔离与自愈。
- `HRS-E009` 高级题：故障域划分与爆炸半径治理。
- `HRS-E013` 高级题：整数级规模下的可扩展稳定性验证框架。

## 证据入口

- `[已确认]` `cann/90-cross-module/error-boundaries.md:16-24`：Driver→HAL→Runtime→ACL/GE 的分层转换链。
- `[已确认]` `cann/90-cross-module/error-boundaries.md:26-45`：内存路径的错误边界与按类别处理的调用方动作。
- `[已确认]` `cann/00-overview/global-error-model.md:29-37`：错误类别表；`:39-43` 诊断原则。
- `[已确认]` `cann/90-cross-module/memory-and-resource-lifecycle.md:91-97`：SOMA free ioctl 失败“不回滚”的故障窗口。
- `[已确认]` `cann/90-cross-module/memory-and-resource-lifecycle.md:103-114`：错误回滚矩阵。
- `[已确认]` `cann/99-roadmap/testing-recipes.md:14-24`：异步/跨模块/ABI 最小回归矩阵。
- `[已确认]` `cann/99-roadmap/risk-register.md:15-25`：R1–R7 静态风险（异步提前释放、错误丢上下文、ioctl/DMA 校验等）。
- `[推断]`：三态状态机、恢复伪代码与不变量由上述证据推导；`[待验证]`：设备取消、reset 与驱动重试语义。

## 学习提示

先读 `HRS-B010` 建立三态判定和时间线，再用 `HRS-I009` 把错误分层、超时、取消和故障注入串成闭环。进阶用 `HRS-A009`/`HRS-A013` 学故障传播控制与设备级自愈，最后用 `HRS-E009`/`HRS-E013` 检验故障域划分和规模化验证：重点不是“能不能重试”，而是“在 UNKNOWN 与不一致窗口下如何既不误释放也不盲重试”。
