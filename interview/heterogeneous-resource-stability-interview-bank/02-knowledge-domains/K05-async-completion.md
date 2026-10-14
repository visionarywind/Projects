# K05：异步提交、完成语义与内存可见性

## 边界

本知识域处理“任务什么时候算完成、结果什么时候可读”的判定。它连接提交路径（K03）与内存消费（K04），回答完成证据有哪些、覆盖范围多大、内存可见性如何建立、以及超时时资源如何处理。

本域解决：

1. 四时刻模型在完成语义上的具体判定；
2. 同步、Stream synchronize、Event wait、callback 四类完成证据的边界；
3. happens-before 与内存可见性；
4. 三态结果中的 `UNKNOWN` 处理；
5. SOMA 本地状态与设备侧完成的差异。

本域不负责：

- 对象创建/销毁顺序（K03）；
- 内存池分配算法（K04）；
- 错误分类与恢复（K09）；
- 性能优化（K11）。

与相邻域的接口：K03 提供 Stream/Event 的合法状态；K04 提供可释放的内存量；K05 输出“完成证据是否被观察”的判定，供 K09 决定重试或隔离、供 K11 评估同步开销。

## 核心机制

### 1. 四时刻模型

CANN 文档把“主机 API 返回成功、Runtime 元数据改变、Driver 接受请求、设备任务完成”视为四个不同时刻。尤其普通异步任务、SOMA AICPU 操作和模型执行，不能把提交成功当成设备已消费完 buffer。[已确认] 来源：`cann/90-cross-module/memory-and-resource-lifecycle.md:25`。

```text
submit(t) -> ... -> completion_evidence(t)  =>  result(t) is readable
completion_evidence(t) must be observed, not assumed from submit return
```

### 2. 完成证据的覆盖范围

| 完成手段 | 覆盖范围 | 典型用途 | 代价/边界 |
|---|---|---|---|
| `rtDeviceSynchronize` | 设备侧整体 | 收尾、退出前 | 粒度粗、阻塞 |
| Stream synchronize | 该 Stream 前序任务 | 顺序消费结果 | 只覆盖该 Stream |
| Event wait | 指定完成标记 | 跨 Stream 依赖 | 需正确 record/wait |
| Callback | 异步通知 | 流水线 | 须确认回调完成并读错误 |

选择依据：需要多细的边界、是否需要跨 Stream 建立依赖、是否愿意阻塞。Event 适合细粒度/跨 Stream；Stream synchronize 适合该 Stream 顺序消费；`rtDeviceSynchronize` 范围最大但最粗。

### 3. 同步释放与非同步释放

`rtFreeWithDevSync` 和 `rtFreeHostWithDevSync` 显式先同步设备再释放；它们不是普通 free 的别名。[已确认] 来源：`cann/90-cross-module/memory-and-resource-lifecycle.md:101`、`cann/01-modules/M03-runtime/call-chains.md:112`。

```text
rtFreeWithDevSync:
  rtDeviceSynchronize() -> DevFree

普通 rtFree:
  DevFree   // 不隐含等待设备完成
```

这说明普通 `rtFree` 与“先确认设备任务完成再释放”是两个不同契约。不能把普通 free 的成功理解为所有异步消费者已经完成。

### 4. 内存可见性

“结果可读、内存可见”要同时满足两个条件：

1. **完成**：设备任务已经执行结束，并且失败信息已经被观察到；
2. **可见**：写入对下一个观察者（CPU 或另一设备任务）可见，缓存/一致性已被正确同步。

```text
CPU 读结果:  submit -> [同步边界] -> read
设备间依赖:  stream A 写 -> event record -> stream B wait -> 读A结果
错误路径:    submit -> 无完成证据 -> 标 UNKNOWN -> 隔离, 不释放
```

CPU 要读取设备写出的结果，通常需要一次同步边界收尾设备一致性状态；设备任务之间要看到彼此写入，则用同一 Stream 的顺序语义或 Event/依赖建立 happens-before。设备侧的缓存一致性与 event 完成时序在目标硬件验证前属于 `[待验证]`。

### 5. 三态状态机

```text
SUBMITTED --(completion evidence)--> DONE
SUBMITTED --(error observed)-------> FAILED
SUBMITTED --(window expired)--------> UNKNOWN
DONE  => buffer/result 可消费
UNKNOWN => 不得释放、不得重试，先隔离观察
```

`UNKNOWN` 不是失败，也不是成功。超时只说明在观察窗口内没有拿到完成证据，任务可能仍在设备上运行。此时复用 buffer 或释放资源都可能造成数据损坏或悬空引用。

### 6. Stream 与实际设备消费的区别

Stream 只是提交顺序和依赖的 Runtime 对象；真正的设备消费发生在 Driver queue/HDC/esched 和设备侧调度链中。除非调用 `rtDeviceSynchronize`、stream synchronize、Event wait 或已确认 callback 完成，否则应保留被任务引用的 memory、model、stream、event 和 callback data。[已确认] 来源：`cann/90-cross-module/memory-and-resource-lifecycle.md:88-92`。

这解释了为什么“同一条 Stream 提交顺序正确”不等于“设备已经消费完”：顺序只保证依赖关系，不保证时间。

### 7. SOMA：本地状态 ≠ 设备完成

SOMA async malloc 的本地 Segment 分配发生在 AICPU kernel 提交之前；提交失败时 Runtime 尝试 free，但默认非 force free，可能进入 CACHED。Driver V3 malloc 配置先建立本地 VMM segment，再发送 `SVM_SOMA_MEM_ALLOC`；free 配置先删除 segment，再发送 `SVM_SOMA_MEM_FREE`，源码明确标注 ioctl 失败“不回滚”。[已确认] 来源：`cann/90-cross-module/memory-and-resource-lifecycle.md:95`。

这形成故障窗口：

```text
SOMA free: 删除本地 VMM segment --(ioctl 失败)--> 设备侧未回滚
=> 上层 Runtime 元数据 / Driver VMM segment / 远端 pool client / AICPU 操作
   可能短暂处于不同状态
```

因此“free 返回成功”只表示 Runtime 元数据已接受释放请求，物理 backing 和设备侧配置仍由后续异步协议处理。

### 8. 异步错误传播

错误沿“设备/驱动 → HAL → Runtime `rtError_t` → ACL/GE → 同步返回/异步回调/日志”向上传播。[已确认] 来源：`cann/90-cross-module/error-boundaries.md:16-24`。诊断原则明确要求不以 `ACL_SUCCESS` 或 `GRAPH_SUCCESS` 推断设备任务已完成，异步路径必须同步或等待事件。[已确认] 来源：`cann/00-overview/global-error-model.md:39-43`。

异步错误的关键特征：错误可能只在同步或 callback 处才暴露，因此“提交成功但业务结果错误”必须通过完成证据和结果 oracle 才能发现。

### 9. 等待策略取舍

轮询与阻塞是两种等待策略：

- 阻塞（同步/Event wait）：语义清晰，但占用线程、可能拖长尾延迟；
- 轮询：可做进度观测，但消耗 CPU，且轮询到“看似完成”不等于完成证据。

两种策略都不能替代完成证据本身。sleep 只能用于探测，不能用于正确性判定。

### 10. happens-before 形式化

把可见性写成偏序关系，能避免“看起来跑完了”的直觉错误：

```text
enqueue(A, streamS) < enqueue(B, streamS)   // 同一 Stream 内顺序
  => completion(A) happens-before start(B)

record(eventE, streamS) then wait(eventE, streamT)
  => completion(streamS before record) happens-before start(streamT after wait)

synchronize(device) => 观察点能看到此前所有已提交任务的写入
```

不成立的推论：

```text
submit(A) returns -> A completion      // 错
sleep(dt) -> A completion              // 错
same process, different streams -> order  // 错，除非有 event/依赖
```

happens-before 只由 Stream 顺序、Event 依赖和同步边界建立，不由时间、线程或进程关系建立。

### 11. callback 上下文与完成确认

callback 是异步通知，不是同步点。使用 callback 时要注意：

- callback 可能在不同于提交点的线程/上下文执行；
- callback 被触发不等于 callback 已执行完、也不等于错误已被读取；
- callback 里访问的对象（buffer、model、stream、callback data）必须在回调期间保持有效；
- callback 中抛出的错误需要被读取，否则 `SUBMITTED` 可能永远停在无终态。

```text
正确: submit -> callback 确认完成 + 读错误 -> 标记终态 -> 释放
错误: submit -> callback 已排队 -> 立即释放对象
```

### 12. 一个流水线示例

把多个任务串成流水线时，每一步的输入是上一步的输出，完成证据必须沿着流水线传递：

```text
stage1: submit(load)  -> sync/event e1
stage2: wait(e1); submit(compute) -> event e2
stage3: wait(e2); submit(store)  -> sync
最后:   读取结果前需要 stage3 的完成证据
```

如果用同一条 Stream，中间可以省略显式 event（Stream 顺序保证依赖）；如果跨 Stream，必须用 event 建立 happens-before，否则 stage2 可能在 stage1 完成前读取未就绪的数据。释放 buffer 的时机是“所有消费者都拿到完成证据之后”，而不是“提交之后”。

## 常见失效条件

- **提交成功即读结果。** 可能读到旧数据或半成品。
- **用固定睡眠代替同步边界。** 负载变化或长任务时会误判。
- **超时后立刻释放。** 任务可能仍在设备上，释放会破坏数据并使错误更难定位。
- **把 `UNKNOWN` 当 `DONE`。** 未确认完成就复用 buffer。
- **把 `UNKNOWN` 当 `FAILURE` 重试。** 任务可能已产生副作用。
- **只等 Stream synchronize 却需要跨 Stream 依赖。** 顺序语义不覆盖其他 Stream。
- **把 callback 已触发当 callback 已处理。** callback 内部错误仍需读取。
- **混淆本地 SOMA 状态与设备完成。** 本地 BUSY→CACHED 不代表 backing 已回收。

## 关键不变量

1. **证据必须被观察**：完成状态由可观察证据决定，不由提交返回值推出。
2. **保留被引用对象**：在完成证据获得前，保留 memory、model、stream、event、callback data。
3. **未知保守**：`UNKNOWN` 不释放、不重试，先隔离观察。
4. **可见性需边界**：CPU 读需同步边界；设备间依赖需 Stream 顺序或 Event。
5. **同步释放显式化**：需要释放前完成时使用 `rtFreeWithDevSync`/`rtFreeHostWithDevSync`，普通 free 无隐式同步。[已确认]
6. **本地与设备分离**：SOMA 本地段状态变化不等于设备侧操作完成。[已确认]
7. **观测旁路**：诊断设施不改变完成语义和资源所有权。[已确认]

## 与其他知识域的连接

- ← **K01**：完成证据实现“完成证明”不变量，三态模型在此具体化。
- ← **K03**：Event/Stream 的合法状态与销毁顺序决定完成证据的可获得性。
- ← **K04**：`pending_tasks == 0` 的复用/释放条件依赖本域判定。
- → **K07**：传输完成后仍需完成证据和可见性同步才能读主机内存。
- → **K08**：集合通信的整体完成不能用单 rank 本地返回代表。
- → **K09**：`UNKNOWN`、超时、取消和异步错误传播在此进入恢复流程。
- → **K10**：完成证据与原始错误是诊断证据链的关键字段。
- → **K11**：同步粒度和阻塞范围影响尾延迟与吞吐。

## 题目索引

主域题目（`01-question-bank` 中 `知识域：K05`）：

- 入门：`HRS-B006`（判断结果可读、内存可见）
- 进阶：`HRS-I005`（证明异步完成并保证内存可见性）
- 高级：`HRS-A005`（可见性与一致性代价取舍）
- 专家：`HRS-E005`（可推理的异步一致性模型）

## 证据入口

- `cann/90-cross-module/memory-and-resource-lifecycle.md:24-25,88-101`：四时刻模型、异步保留、同步释放、SOMA ioctl 窗口。
- `cann/00-overview/global-error-model.md:14-43`：错误流与“不以 SUCCESS 推断完成”的诊断原则。
- `cann/90-cross-module/error-boundaries.md:14-24,43-49`：错误分层与异步完成边界。
- `cann/01-modules/M03-runtime/call-chains.md:88-117`：SOMA 异步控制流与同步释放边界。
- `cann/01-modules/M03-runtime/memory-pool-analysis.md:109-113,176-178`：SegmentFree 语义与 `rtFreeWithDevSync`。
- `cann/00-overview/runtime-model.md:32-37`：同步/异步执行模型。
- `cann/90-cross-module/end-to-end-flows.md:45-98`：队列任务与内存路径的完成边界。

## 学习提示

把“完成”从动词变成名词：完成是一个可观察证据（E），不是一次调用返回。写代码时给每个 buffer 标注它的 E 来源，并规定“E 获得前不释放”。遇到超时，先把请求标 `UNKNOWN`，再决定隔离、等待还是重试，不要默认它已经失败或已经成功。
