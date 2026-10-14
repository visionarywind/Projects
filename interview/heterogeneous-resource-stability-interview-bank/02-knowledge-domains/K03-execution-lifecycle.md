# K03：Context、Stream、Event 与 Queue 生命周期

## 边界

本知识域处理执行对象的生命周期：Context、Stream、Event、Model/Kernel、Callback data 以及 Driver 侧的 Queue/HDC session。它回答“谁在什么时候被创建、绑定、使用、停止和销毁”，以及销毁时必须满足哪些引用与完成条件。

本域解决：

1. 执行对象层次与归属关系；
2. Context 的 Setup/ACTIVE/Teardown 状态机；
3. 依赖逆拓扑序的销毁顺序与引用归零条件；
4. teardown 失败后的可重试状态与残留风险。

本域不负责：

- 提交后设备是否真正完成（K05）；
- 错误分类与恢复策略（K09）；
- 内存池内部算法（K04）；
- 调度和优先级（K06）。

与相邻域的接口：K02 输出准入结果后由 K03 创建并绑定对象；K05 消费 Event/Stream 的完成语义；K04 的内存对象被任务引用，必须在 K03 的完成边界之后释放；K09 处理 teardown 失败后的恢复。

## 核心机制

### 1. 对象层次

Runtime 的对象层次如下。[已确认] 来源：`cann/00-overview/runtime-model.md:14-28`：

```text
进程
 ├─ GE 全局状态 / SessionManager
 │   └─ Session → Graph → Executor → Model
 ├─ ACL 全局初始化引用计数
 └─ Runtime Api::Instance()
     ├─ Device
     ├─ Context
     ├─ Stream / Event
     ├─ Memory / DataBuffer
     └─ Model / Kernel / Task
         └─ Driver client / queue / HDC session
```

关键事实：C API 不直接暴露内部 C++ 对象，而通过 `RT_VALIDATE_AND_UNWRAP_OBJECT` 等宏校验和解包。[已确认] 来源：`cann/01-modules/M03-runtime/data-structures.md:42`。因此“句柄非空”不等于“对象可用”。

### 2. Context 创建（Setup）

显式 Context 创建时，Runtime 校验 device ID，retain Device，构造 Context 并执行 `Context::Setup`。Setup 初始化 module allocator，申请 overflow address，主 Context 复用设备 Primary Stream，非主 Context 创建并 Setup default Stream，满足 feature/run-mode 时再初始化 online fast CQ，最后才把 Context 状态设为 `ACTIVE`。[已确认] 来源：`cann/90-cross-module/memory-and-resource-lifecycle.md:43`。

```text
rtsCtxCreate
  -> 校验 devId
  -> Runtime::DeviceRetain(deviceId, tsId)
  -> new Context(dev, false)
  -> Context::Setup
       -> Init (module allocator)
       -> SetOverflowAddr (DevMemAlloc)
       -> 创建/取得 default Stream
       -> Stream::Setup
       -> OnlineStreamInit（条件满足时 fast CQ）
       -> state = ACTIVE
  -> ContextManage::InsertContext
  -> ContextSetCurrent
```

Setup 失败时 `NewContext` 调用 `TearDown` 后 delete，部分初始化分支有局部清理。[已确认] 来源：`cann/90-cross-module/memory-and-resource-lifecycle.md:107`。

### 3. Context 状态机

```text
NEW -> SETTING_UP -> ACTIVE -> FINALIZING -> DEINITIALIZING -> DESTROYED
             \-> FAILED
```

- `ACTIVE` 才允许正常提交；
- `FINALIZING` 拒绝新访问但允许清理；
- `FAILED` 的局部资源必须按已成功初始化的子集回收；
- Primary Context 不能被显式 destroy。[已确认] 来源：`cann/90-cross-module/memory-and-resource-lifecycle.md:31,49`。

采用状态机可以避免把“句柄非空”误认为“对象可用”。

### 4. Context 销毁（依赖逆拓扑序）

销毁先做有效性和访问模式检查，Primary Context 显式 destroy 被拒绝；显式 Context 要取得唯一 teardown 执行权并切换 `ACTIVE→FINALIZING`。TearDown 删除 Context 上的 Model，清理 owner Streams，清理 online/default Stream，最后清除线程绑定。[已确认] 来源：`cann/90-cross-module/memory-and-resource-lifecycle.md:49`。

官方顺序：

```text
停止新访问 -> 清理 Model -> flush/清理 Stream -> 清除线程绑定
-> 等待引用耗尽 -> 移除 Context Registry -> 删除 Context
```

如果 Stream 清理失败且仍有合法 child handle，代码会恢复 owner stream 以支持重试。最终删除需要非 Primary、delete 标志和线程引用计数为 0，随后切换 `DEINITIALIZING`、从 Registry 移除并 `delete this`。[已确认] 来源：`cann/90-cross-module/memory-and-resource-lifecycle.md:51`。

释放条件：

```text
destroy(context) only when pending_tasks == 0
                         and child_handles == 0
                         and owner_refs == 0
```

实际实现可能选择同步等待、异步延迟删除或返回 busy；具体策略需查目标 Runtime。CANN 文档确认某些 teardown 失败时会恢复 owner stream 以支持重试，但设备 pending task 的具体行为未在硬件上验证。

### 5. Stream / Event / Model

- **Stream**：提交顺序和依赖的 Runtime 对象；真正的设备消费发生在 Driver queue/HDC/esched 和设备侧调度链中。[已确认] 来源：`cann/90-cross-module/memory-and-resource-lifecycle.md:91`。
- **Event**：完成和跨队列依赖；等待关系和 Registry sequence 记录需先于释放维护。[已确认] 来源：`cann/01-modules/M03-runtime/data-structures.md:21`。
- **Model/Kernel**：设备仍运行时不得提前销毁。[已确认] 来源：`cann/90-cross-module/memory-and-resource-lifecycle.md:37`。

依赖图决定释放顺序：Context 是生命周期的上层边界，Stream 和任务依赖它；任务又可能引用 Model、Memory、Event 和 callback data。因此销毁通常是依赖图的逆拓扑序，而不是简单的创建逆序。

### 6. Driver 侧 Queue / HDC / Model 状态

Driver 侧生命周期有两个已确认约束：

- HDC client 创建时分配 client/session 数组并初始化 magic、服务类型、超时、session 状态和 mutex；销毁前加锁检查活动 session，有活动 session 返回 `DRV_ERROR_CLIENT_BUSY`，否则关闭底层 client 并释放内存。[已确认] 来源：`cann/01-modules/M04-driver/design.md:24`、`cann/01-modules/M04-driver/call-chains.md:36-42`。
- Queue 打开时按当前进程 TGID 创建 context 并放入 `file->private_data`，release 时删除进程信息并释放 context。[已确认] 来源：`cann/01-modules/M04-driver/design.md:26`。这说明 queue 资源与文件描述符/进程生命周期绑定。

GE 侧：V2 只有 `Loaded` 状态可执行；Load/Execute/UnLoad 有明确状态路径。[已确认] 来源：`cann/00-overview/design-principles.md:19`、`cann/00-overview/runtime-model.md:36`。

### 7. 同步陷阱与死锁

同步与死锁问题（见 `HRS-I014`）通常来自：

- 在持有某把锁或绑定状态下等待另一个执行流的完成事件；
- 跨 Stream 循环依赖（A 等 B，B 等 A）；
- teardown 路径与提交路径互相等待；
- 用固定 sleep 代替完成证据，掩盖了真正的依赖环。

生命周期正确性要求：先停止新访问，再等待/处理未完成任务，最后销毁被依赖对象。若 Stream 清理失败，不能无条件 delete Context。

### 8. 引用计数与 RAII

生命周期安全在多线程下不能只靠“调用顺序看起来合理”。可用引用计数和 RAII 类思维护住不变量：

```text
持有引用:  retain(obj)  -> refcount += 1
释放引用:  release(obj) -> refcount -= 1; if refcount == 0 -> finalize
绑定引用:  线程局部 current context 是访问路径，不是所有权
```

CANN 的 Context 最终删除需要非 Primary、delete 标志和线程引用计数为 0。[已确认] 来源：`cann/90-cross-module/memory-and-resource-lifecycle.md:51`。SOMA 的 `PoolRegistry` 用 `shared_ptr` 保存 ownership，查询会延长 manager 生命周期。[已确认] 来源：`cann/90-cross-module/memory-and-resource-lifecycle.md:33`。这两处都说明“谁持有引用”决定了“何时能销”。

需要区分的三种引用：

- **所有权引用**：决定对象能否最终删除（如 Context 的 owner、Registry 的 shared_ptr）；
- **child handle 引用**：子对象持父对象，父对象销毁前必须 child handle 归零；
- **线程绑定引用**：线程局部访问路径，清理绑定是销前置步骤，但不等价于所有权归零。

### 9. teardown 失败与重试状态

TearDown 失败不一定等于对象进入终态。CANN 文档确认：若 Stream 清理失败且仍有合法 child handle，代码会恢复 owner stream 以支持重试。[已确认] 来源：`cann/90-cross-module/memory-and-resource-lifecycle.md:51`。

| teardown 步骤 | 失败表现 | 已确认的恢复 | 需验证 |
|---|---|---|---|
| 清理 Model | delete 失败或仍有任务 | 保留对象 | 设备侧任务状态 |
| flush/清理 Stream | child handle 未归零 | 恢复 owner stream 以重试 | SQ/CQ 是否完全回收 |
| 清除线程绑定 | 其他线程仍持有 | 延迟删除 | 跨线程引用时序 |
| Registry 移除 | 仍有查询 | shared ownership 延长生命周期 | 并发查询行为 |

策略上，teardown 返回 busy 时保留对象和所有权，记录 pending task/child handle，等待明确完成或按策略重试；不要直接 free 底层句柄。具体选择同步等待、异步延迟删除还是返回 busy 取决于目标 Runtime。

## 常见失效条件

- **先销毁 Context 再销毁 Stream。** Stream 依赖 Context，先销毁产生悬空句柄和未定义行为。
- **看到指针非空就调用 destroy。** 对象可能处于 `FINALIZING`、`FAILED` 或已有 child handle。
- **用 sleep 代替完成确认。** 固定等待不能证明长任务、异常任务或不同负载下已经完成。
- **teardown 返回 busy 时直接 free 底层句柄。** 应保留对象和所有权，记录 pending/child，等待或按策略重试。
- **线程绑定当所有权。** 线程局部 current context 只是访问路径之一，不能代替全局引用计数。
- **忽视 HDC session。** 有活动 session 时销毁 client 返回 BUSY，忽视它会导致资源残留。
- **循环依赖同步。** 跨 Stream 互相等待会死锁，且用超时无法可靠区分“未完成”和“已死锁”。

## 关键不变量

1. **唯一 teardown 执行权**：同一 Context 同时只有一个 teardown 流程，避免并发销毁。
2. **逆拓扑序释放**：Model/Kernel → Stream → 绑定 → Context，末尾等待引用归零。[已确认]
3. **引用与 child 归零**：`pending_tasks == 0 ∧ child_handles == 0 ∧ owner_refs == 0` 才允许最终删除。
4. **Primary Context 不可显式销毁**：只能 retain/release，不能 destroy。[已确认]
5. **失败可重试**：teardown 部分失败时对象保持可重试状态，恢复 owner stream 或保留对象。[已确认]
6. **句柄 ≠ 可用**：对象可用性由状态决定，不由句柄非空决定。
7. **Queue 绑定进程**：queue context 与 TGID/fd 生命周期绑定；HDC client 先于 session 存在、后于 session 销毁。[已确认]

## 与其他知识域的连接

- ← **K01**：唯一终态与所有权不变量在此具体化为对象状态机。
- ← **K02**：准入通过后由本域创建并绑定 Context/Stream。
- → **K04**：任务引用的内存对象必须在完成后按逆拓扑序释放。
- → **K05**：Event/Stream 是完成证据的载体，本域提供它们的合法状态。
- → **K06**：Context 与线程绑定、Stream 顺序是隔离与并发的基础。
- → **K09**：teardown 失败与残留状态进入恢复流程。
- → **K12**：排空（drain）是发布/迁移的前提，本质是生命周期收敛。

## 题目索引

主域题目（`01-question-bank` 中 `知识域：K03`）：

- 入门：`HRS-B003`（Context/Stream/任务生命周期顺序）、`HRS-B004`（异步异常状态机）
- 进阶：`HRS-I003`（生命周期状态机与销毁顺序）、`HRS-I014`（Stream/Event 同步陷阱与死锁）
- 高级：`HRS-A003`（执行生命周期状态一致性与安全回收）
- 专家：`HRS-E003`（可验证的形式化生命周期状态机）

## 证据入口

- `cann/90-cross-module/memory-and-resource-lifecycle.md:31-55,107,114`：所有权分层、创建/销毁顺序、teardown 失败恢复。
- `cann/01-modules/M03-runtime/call-chains.md:14-71`：设备选择、Context 创建/销毁完整控制流。
- `cann/01-modules/M03-runtime/data-structures.md:14-42`：执行对象与关键不变量、句柄安全。
- `cann/00-overview/runtime-model.md:14-46`：对象层次与资源所有权表。
- `cann/01-modules/M04-driver/design.md:22-26`：HDC client 与 queue context 生命周期。
- `cann/00-overview/design-principles.md:19`：显式生命周期拒绝不安全顺序调用。
- `cann/99-roadmap/debugging-guide.md:20`：检查异步完成、handle 生命周期和清理顺序。

## 学习提示

把生命周期当成依赖图，而不是调用清单。先画对象归属（Context 拥有什么、任务引用什么），再把销毁顺序写成逆拓扑序，最后补上“停止新访问 → 等待完成 → 引用归零”的前置条件。遇到同步问题时，先确认是否存在循环依赖和“用 sleep 代替证据”，再回 K05 找正确的完成机制。
