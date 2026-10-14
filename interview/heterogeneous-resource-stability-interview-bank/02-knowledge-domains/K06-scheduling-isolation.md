# K06：调度、并发、隔离、优先级与配额

## 边界

本知识域处理共享异构设备上的竞争问题：多个 Stream、多个线程、多个进程/租户如何排队、隔离、获得优先级和受限配额。它回答“资源竞争在哪里、隔离点有哪些、优先级和配额是否真的生效”。

本域解决：

1. “提交到执行”三段模型（主机提交 / Runtime 对象 / Driver 设备调度）；
2. 隔离维度的分层（地址、顺序、资源、时间、租户）；
3. 配额水位不变量；
4. 固定槽资源的扩容策略；
5. 优先级与抢占的证明边界。

本域不负责：

- 单任务的完成语义（K05）；
- 内存池内部算法（K04，但会引用固定槽和 stream-ordered 语义）；
- 跨节点通信的调度（K08）；
- SLO 治理（K10/K11）。

与相邻域的接口：K03 提供 Context/Stream 的绑定与顺序；K04 提供池与槽资源；K05 提供完成边界；K06 把这些组合成多租户并发控制，并输出隔离与配额不变量供 K10/K11 度量。

## 核心机制

### 1. 提交到执行的三段模型

```text
tenant -> Context(地址/隔离)
       -> Stream(顺序/依赖)
       -> Runtime 对象/槽池(限资源)
       -> Driver queue/HDC/esched(设备侧调度)
       -> 设备执行
```

- **主机段**：Context 与线程绑定、Stream 提交顺序、API 是否阻塞；
- **Runtime 段**：Stream/Event/队列对象、固定槽资源池、上下文隔离；
- **Driver/设备段**：queue/HDC/esched 的设备侧调度，以及可能的优先级通道；
- **租户段**：不同进程/Context 之间的资源竞争和配额。

其中前两段在文档中有可核对的控制流；优先级和配额的具体语义在闭源设备侧，需标为 `[推断]` 或 `[待验证]`。

### 2. 隔离先回答“隔离什么”

不同隔离目标对应不同机制：

| 隔离维度 | 机制（文档可核对） | 需验证部分 |
|---|---|---|
| 地址域 | Context / Primary Context | 跨进程隔离细节 |
| 顺序依赖 | Stream、Event | 跨 Stream 依赖聚合 |
| 固定槽资源 | BufferAllocator Bitmap | 高并发扩容行为 |
| 时间/优先级 | 设备侧 queue/esched | 优先级是否硬保证 |
| 租户配额 | 上层配额 + 池所有权 | 设备侧强制能力 |

隔离是分层的：地址和上下文隔离靠 Context，让不同租户或任务不共享可写状态；队列和提交顺序隔离靠 Stream；内存和槽资源隔离靠各自的池和池所有权；时间维度的隔离和优先级通常落在设备调度侧。

### 3. Context 作为隔离与所有权边界

- 显式 Context 创建会初始化 module allocator、申请 overflow address；主 Context 复用设备 Primary Stream，非主 Context 创建并 Setup default Stream，满足 feature/run-mode 时初始化 online fast CQ，最后才把状态设为 `ACTIVE`。[已确认] 来源：`cann/90-cross-module/memory-and-resource-lifecycle.md:43`。
- Runtime Context 创建时 retain Device；`rtSetDevice` 把引用对象写入线程局部容器并管理 Primary Context；Primary Context 不能被显式 destroy。[已确认] 来源：`cann/90-cross-module/memory-and-resource-lifecycle.md:31,45`。
- SOMA 的 `PoolRegistry` 用 `poolOwnership_` 保存 `shared_ptr<SegmentManager>`，查询 shared_ptr 会延长 manager 生命周期；删除 stream/event 时清理依赖 metadata。[已确认] 来源：`cann/90-cross-module/memory-and-resource-lifecycle.md:31-37`。

所有权会影响隔离：查询返回的 shared ownership 会延长生命周期，隔离策略必须尊重所有权，不能在仍有引用时强行回收。

### 4. 设备边界与资源分表

Driver V3 `cache_allocator` 按 `(devid, flag)` 建立全局 allocator 表，`cache_init` 根据 host/device、P2P、huge-page 组合创建多类 allocator。[已确认] 来源：`cann/90-cross-module/pooling-and-resource-management.md:67-71`。这说明资源天然按设备和属性分开，跨设备租户不能只在一个设备上记账。

SHMEM 按每 PE 固定虚拟窗口保留地址，说明多设备/多参与者下存在按设备或 rank 划分的资源边界。[已确认] 来源：`cann/90-cross-module/pooling-and-resource-management.md:91`。

### 5. 固定槽资源与扩容策略

Runtime `BufferAllocator` 用 Bitmap 管理 item id 到连续 backing 数组的占用状态。`LINEAR` 按 `initCount` 增长，`EXPONENTIAL` 按 2 倍增长。[已确认] 来源：`cann/90-cross-module/pooling-and-resource-management.md:45-49`。

关键点：策略只决定扩容节奏，不是地址区间的 best-fit。修改 `initCount/maxCount` 会改变资源上限、扩容次数和 host/device backing 分配压力。并发突发时，扩容点可能成为抖动来源。

```text
扩容策略决定:
  资源上限  maxCount
  扩容次数  与并发突发形态相关
  backing 分配压力  每次扩容触发 host/device 分配
```

### 6. 设备侧调度 ABI

Driver `esched_ioctl.h` 定义 CPU ID、进程组、事件优先级、等待/提交事件和表项增删等结构，是跨用户态/内核态的数据契约。[已确认] 来源：`cann/01-modules/M04-driver/data-structures.md:42`。

这表明设备侧确实存在优先级和进程组概念，但其调度语义、是否硬抢占、优先级反转行为属于闭源设备行为，需在目标版本上验证。工程上可确认的是“存在优先级通道”，不能确认“优先级是硬保证”。

### 7. 多进程/多租户隔离

Queue 打开时按当前进程 TGID 创建 context 并放入 `file->private_data`，release 时删除进程信息并释放 context。[已确认] 来源：`cann/01-modules/M04-driver/design.md:26`。这说明 queue 资源与进程生命周期绑定，进程级隔离有明确的所有权边界。

HDC client 必须无活动 session 才能销毁。[已确认] 来源：`cann/90-cross-module/cross-module-call-chains.md:52`。这为租户排空提供了检查点：迁移或清理前要确认没有活动 session。

### 8. 配额水位不变量

```text
for each class r:  used(r) + reserved(r) <= quota(r)
admit(task) requires  forall r: used(r) + demand(task, r) <= quota(r)
```

`r` 要绑定到具体维度：设备、内存属性、Stream、槽位类别。忽略预留、碎片和扩容点会导致瞬时击穿。配额必须与 K04 的分层水位和 K02 的准入判定联合使用。

### 9. 优先级与公平性

优先级更多是“影响排队和抢占”的建议，而不是可以随意假设的硬保证。工程上常用显式优先级通道、独立 Stream、限额和背压来近似，并用队列等待时间、尾延迟和资源水位来验证是否真的隔离住了。公平性、饥饿、队头阻塞和优先级反转都是这类系统的常见问题。

### 10. 背压与限流

多租户共享设备时，排队深度必须受控，否则尾延迟会随负载非线性上升。常用手段：

- **准入限流**：在 K02 的准入链路上按配额拒绝或排队；
- **背压**：当队列深度或资源水位超过阈值时，向上游返回可重试或阻塞信号；
- **限次重试 + 退避**：避免重试放大拥塞（与 K09 共享）；
- **隔离池**：为关键路径预留独立 Stream/槽/内存，避免被噪声邻居拖垮。

背压设计要点是“可观测 + 可解释”：每个被拒绝或延迟的请求都要能归因到具体资源类别，而不是笼统的 `busy`。

### 11. 公平性与饥饿

公平性不等于均分。常见权衡：

```text
max-min fair:   每个租户至少拿到 min(需求, 均分份额)
weighted fair:  按权重分配, 高权重租户获得更多
priority:       高优先级优先, 可能饥饿低优先级
```

风险：

- **饥饿**：低优先级请求长期得不到执行；
- **优先级反转**：低优先级任务持有关键资源（锁、池、队列），高优先级任务被迫等待；
- **队头阻塞**：一个大任务阻塞同一队列里的后续小任务；
- **噪声邻居**：一个租户的突发流量拖高其他租户的尾延迟。

设备侧是否提供公平调度、优先级是否硬保证，属于 `[待验证]`；工程上必须用队列等待时间和尾延迟分布验证隔离效果，而不是相信配置项。

### 12. 一个队列模型

把提交到执行建模成多级队列，便于推理配额和等待时间：

```text
tenant -> admit(配额检查)
       -> Stream queue(顺序, 依赖)
       -> Runtime 槽池(slot 分配)
       -> Driver queue/HDC/esched(设备侧优先级)
       -> device

每级都有容量上限和等待队列;
积压在第 i 级的任务会抬高第 i+1 级的进入延迟;
端到端等待 ≈ Σ 各级排队时间 + 执行时间
```

验证隔离时，构造两个租户、多个 Stream、长短任务混合和优先级反转场景，观察每个租户的队列等待和尾延迟是否按预期方向变化。若某租户超过配额，或优先级设置后等待时间毫无变化且无设备证据，则隔离结论不成立。

## 常见失效条件

- **把 Stream 数量当并行度。** 提交很多 Stream 也可能在 Driver/设备处排队。
- **假设优先级是硬保证。** 缺少设备证据时会把“建议优先级”当契约。
- **配额只算瞬时用量。** 忽略预留、碎片和扩容点，突发时仍会击穿。
- **隔离只放到进程级。** 共享设备、内存池、队列和设备侧调度仍可能互相影响。
- **跨设备只在一台记账。** 租户跨设备时配额应在每个设备分别成立。
- **加并发不看扩容策略。** `EXPONENTIAL` 扩容点可能带来 backing 分配抖动。
- **噪声邻居。** 一个租户的突发流量拖高其他租户的尾延迟。
- **优先级反转。** 低优先级任务持有关键资源，阻塞高优先级任务。

## 关键不变量

1. **配额不漏维度**：每个资源类别在每台设备上 satisfied `used + reserved <= quota`。
2. **准入联动**：任务准入同时检查静态能力、拓扑、动态预算和配额。
3. **所有权尊重**：查询返回的 shared ownership 会延长生命周期，回收不得破坏仍被持有的对象。
4. **进程绑定**：queue context 与 TGID/fd 绑定；HDC session 先于 client 销毁。[已确认]
5. **设备分表**：资源按 `(devid, flag)` 分离，跨设备分别记账。[已确认]
6. **扩容有代价**：固定槽扩容策略影响上限、次数和 backing 压力，不是纯扩容。[已确认]
7. **优先级需证据**：设备侧优先级/抢占语义在目标版本上验证前按 `[待验证]` 处理。

## 与其他知识域的连接

- ← **K01**：可用性与故障域边界是本域的验收口径。
- ← **K02**：准入判定与队列/并发能力限制进入隔离设计。
- ← **K03**：Context 绑定、Stream 顺序是主机段隔离的基础。
- ← **K04**：池与固定槽资源是多租户资源隔离的载体。
- ← **K05**：完成边界决定任务何时释放资源、让出配额。
- → **K08**：多 rank 集合通信的调度与对称性。
- → **K09**：隔离是恢复的前提，故障域与 bulkhead 在此定义。
- → **K10/K11**：队列等待、尾延迟和资源水位是隔离效果的度量 SLO。

## 题目索引

主域题目（`01-question-bank` 中 `知识域：K06`）：

- 入门：`HRS-B007`（多 Stream/多租户的调度、隔离、优先级）、`HRS-B015`（多设备多租户资源隔离与配额水位）
- 进阶：`HRS-I006`（调度、并发、隔离与配额边界）
- 高级：`HRS-A006`（共享设备调度隔离与 QoS）
- 专家：`HRS-E006`（调度隔离、公平性与准入控制）

## 证据入口

- `cann/90-cross-module/memory-and-resource-lifecycle.md:31-45,89-91`：Context/所有权、Primary Context、Stream 与设备调度。
- `cann/90-cross-module/pooling-and-resource-management.md:45-49,67-71,87-103`：BufferAllocator 扩容、`(devid, flag)` 分表、SHMEM 资源边界。
- `cann/01-modules/M04-driver/data-structures.md:18-42`：queue context、esched ioctl ABI、资源回收不变量。
- `cann/01-modules/M04-driver/design.md:22-26`：queue 与 TGID 绑定。
- `cann/90-cross-module/cross-module-call-chains.md:42-52`：HDC client 无活动 session 才可销毁。
- `cann/00-overview/design-principles.md:19`：显式生命周期拒绝不安全顺序调用。
- `cann/00-overview/runtime-model.md:49-51`：并发模型与状态协议。

## 学习提示

先分清三段（主机提交、Runtime 对象、Driver 设备调度），再对每一段问“隔离什么、用什么隔离”。配额写成带维度索引的不变量，不要写成单个数字。凡是涉及设备侧优先级、抢占和公平性的结论，先确认是否有目标版本证据，否则保持 `[待验证]`。
