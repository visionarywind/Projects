# K08：跨卡/跨节点通信、集合通信与 RDMA

## 边界

本知识域讨论多个 rank 之间的数据搬运与协同：跨卡、跨节点的集合通信（AllReduce、Broadcast、AllGather 等）以及承载它们的 P2P、RDMA、SDMA 和 fabric 映射。

讨论对象是“多参与者如何共同开始、共同完成、共同清理”，不是单条链路的代价（那是 K07），也不是错误分类与恢复策略（那是 K09）。每个 PE 的虚拟地址窗口、`MemSlice`、`export/import` fabric handle、`HalMemMap` 以及 collective gate 的判定语义，是本域的事实边界。

特定厂商的集合算法选择、RDMA 拥塞控制、NCCL/HCCL 类库实现和真实网络带宽必须绑定目标版本与拓扑。文档只确认了资源保留、映射和集体判定的骨架，性能与容错行为属于 `[待验证]`。

## 核心机制

### 1. 每 PE 固定虚拟窗口

`[已确认]` SHMEM 的 VMM 实现按 `rankCnt * aligned segment size` 保留全局 VA，每个 PE 获得一个固定窗口；`AllocLocalMemory` 在大页和容量约束通过后调用 `HalMemCreate`，创建 `MemSlice`、导出 fabric handle 并 `HalMemMap` 到本地 VA；释放按 `Unmap -> HalMemRelease -> erase slice` 顺序执行。[cann/90-cross-module/pooling-and-resource-management.md:87-93]

窗口地址可以写成：

```text
window(r) = global_va_base + r * aligned_segment_size,  r ∈ [0, rankCnt)
```

这个公式说明两件事：**窗口数量由 rank 数决定**，任何 rank 数变化都需要重新保留 VA；**窗口对齐**决定单次可注册的 segment 大小。

### 2. 后端 heap 与 fabric handle

`[已确认]` SHMEM init backend 为 P2P / RDMA / SDMA 建立 host/device heap base 指针数组；`remove_heap` 释放这些数组并 unmap，`release_heap` 先 unreserve VA 再 destroy entity；任一步失败都可能留下部分状态，调用方依赖 `firstError` 和 collective status gate 保持各 rank 一致。[cann/90-cross-module/pooling-and-resource-management.md:87-93]

`[已确认]` Driver 侧 Ascend910B 额外包含 RoCE 组件，提供主机/设备通信、队列、事件调度、SVM、RoCE 等硬件接入能力。[cann/README.md:23] [cann/01-modules/M04-driver/source-map.md:36]

因此远端可见不是“直接解引用远端指针”，而是：本地创建 `MemSlice` → 导出 fabric handle → 对端 import → `HalMemMap` 到本地固定窗口。handle 和映射共享同一生命周期，拆掉映射前不能释放 backing。

### 3. 集合通信的状态机

集合操作是一组 rank 的联合动作，不能退化成单 rank 的本地返回：

```text
ALIGNED --all ranks ready-------> RUNNING
RUNNING --collective complete----> DONE
RUNNING --any rank local error---> FAILED  (向全体广播 abort/free)
ALIGNED --timeout----------------> UNKNOWN (不得单方面释放)
FAILED  --all ranks ack abort----> ABORTED (统一清理)
```

`[推断]` 该状态机由 `firstError + collective status gate` 的语义和四时刻规则推出。[cann/90-cross-module/pooling-and-resource-management.md:87-93] [cann/90-cross-module/memory-and-resource-lifecycle.md:24-25]

关键点：`RUNNING -> FAILED` 不是“某个 rank 自己失败”，而是“某个 rank 发现失败后，所有 rank 看到同一个结论”。没有广播的局部失败会退化成其余 rank 永久等待。

### 4. 对称性与集体完成的伪代码

```python
def collective(ranks, op, buffers):
    # 1. 对齐阶段：保证所有 rank 的窗口、buffer 布局、调用顺序一致
    barrier_all(ranks)                      # 任何不对称都在这里暴露
    for r in ranks:
        assert window(r) is valid
        assert buffer_layout(r) == expected_layout(op)
    # 2. 执行阶段：本地提交只是进入队列
    local = op_submit(op, buffers)
    # 3. 完成阶段：集体判定，而不是各 rank 各自返回
    status = gate_all(ranks, local, reducer=first_error)
    if status is not OK:
        abort_all(ranks)                    # 全体一致失败，不局部继续
        raise CollectiveFailure(status)
    return status

def teardown(ranks, slices):
    # 释放顺序固定，且需获得集体结果
    for s in slices:
        unmap(s)                            # 先解除映射
        hal_mem_release(s)                  # 再释放 backing
        erase_slice(s)                      # 最后移除 slice 记录
    release_heap(ranks)                     # unreserve VA -> destroy entity
```

`barrier_all`、`gate_all`、`abort_all` 是可读模型，不代表具体厂商 API。

### 5. 完成语义：为什么本地返回不等于集合完成

`[已确认]` 集合与异步路径遵守同一规则：提交成功、Runtime 元数据改变、Driver 接收请求、设备任务完成是四个不同时刻。[cann/90-cross-module/memory-and-resource-lifecycle.md:24-25]

`[已确认]` GE V2 执行包含 `SpecifyInputs/Outputs`、`OccupyStreamResource`、`ExecuteGraph`，随后才跨越到 Runtime 和 Driver。[cann/90-cross-module/cross-module-call-chains.md:28-40]

所以集合通信里，“本地 `submit` 返回成功”只代表本 rank 的任务进了队列；它既不代表本 rank 完成，更不代表全体完成。完成判定必须由统一的 gate 产生。

### 6. 固定槽资源与通信

`[已确认]` Runtime 的 SPM/SQ address、copy/event 等固定槽资源使用 `BufferAllocator` + `Bitmap` 分配 item id，`LINEAR/EXPONENTIAL` 只控制扩容节奏，不是块选择策略。[cann/00-overview/evidence-index.md:27] [cann/90-cross-module/pooling-and-resource-management.md:45-49]

集合通信会同时占用这些固定槽（拷贝、事件、SQ 地址）。槽位耗尽时，即使带宽空闲也会阻塞——这属于资源耗尽，而不是链路故障。

### 7. 失败模式

- rank 不对称：某 rank 的窗口或 buffer 布局与预期不符，出现局部成功、局部错误数据。
- 单 rank 提前返回/提前释放：其他 rank 卡在等待或读到失效内存。
- 无广播的局部失败：其余 rank 永久等待，形成集合死锁。
- handle 生命周期错乱：重建时地址或 handle 冲突。
- 超时即重试：集合超时是 `UNKNOWN`，重试可能重复副作用或加剧拥塞。

### 8. 集合类型与参与者契约

`[推断]` 不同集合操作对“谁提供数据、谁接收结果、缓冲区大小关系”有不同的对称性契约：

| 操作 | 输入关系 | 输出关系 | 语义要点 |
|---|---|---|---|
| Broadcast | root 有数据，其余接收 | 所有 rank 相同 | 一个源，多个副本 |
| AllGather | 每 rank 贡献一段 | 所有 rank 得到全量 | 拼装顺序必须一致 |
| AllReduce | 每 rank 贡献一段 | 所有 rank 得到归约值 | 归约顺序/结合律敏感 |
| ReduceScatter | 每 rank 贡献一段 | 每 rank 得到一部分 | 归约与散播合并 |
| AllToAll | 每 rank 给每人一段 | 每 rank 收每人一段 | 最容易布局不一致 |

契约的关键不是“算法怎么算”，而是“所有 rank 对 buffer 布局、分片边界和调用顺序的认知一致”。任何一处不一致都会让 `barrier_all` 之后的执行阶段读到错位数据，而单 rank 视角看不出问题。

### 9. 算法家族：ring / tree / 递归倍增

`[推断]` 通用集合算法家族（具体实现属厂商库，需绑定版本）：

```text
ring        : 每个 rank 只与左右邻居通信，带宽友好、步数随 rank 数线性增长
tree        : 分层汇聚/分发，步数与 log(rankCnt) 相关，适合延迟敏感的小消息
recursive   : 每轮距离翻倍，用于 AllGather/AllReduce 的倍增传播
```

算法选择是带宽与延迟的取舍：大消息偏向 ring 类以摊薄带宽，小消息偏向 tree 类以降低步数。`[已确认]` 文档只确认了每 PE 固定窗口和集体判定，没有给出算法选择策略或性能结论。[cann/90-cross-module/pooling-and-resource-management.md:87-93]

### 10. 拥塞与流控

`[推断]` RDMA 类传输有接收侧缓冲与完成队列（CQ）约束。如果某个 rank 消费慢，发送侧可能先遇到 CQ 或缓冲反压，而不是链路带宽不足；此时表现是“局部 rank 变慢导致全体变慢”，与集合死锁在现象上相似但根因不同。区分方法：

```text
全体同时变慢 + CQ/缓冲逼近上限 -> 拥塞/反压
单个 rank 长期无完成 + 其余在等   -> 该 rank 失败或阻塞
所有 rank 都 ready 但无人推进     -> 调用顺序/对称性不一致
```

这条分类把“超时”进一步拆成拥塞、单点阻塞和对称性错误，避免一律归因到网络。

### 11. 验证 oracle 与实验设计

`[待验证]` 多 rank 集合通信模拟：

| 注入 | 期望 | oracle |
|---|---|---|
| 正常对齐执行 | 全体共同完成 | 结果经 gate 后一致，资源计数归零 |
| 单 rank 失败 | 全体一致失败（广播 abort） | 无 rank 永久等待 |
| 单 rank 超时 | 进入 `UNKNOWN`，不单方释放 | 资源保留，等对账收敛 |
| 释放乱序 | 被拒绝或按固定顺序执行 | `Unmap→Release→erase` 顺序可验证 |
| rank 数变化 | 重建 VA 窗口 | 窗口数 == `rankCnt` |

失败判据：出现部分 rank 永久等待、或释放后仍有未映射资源，即测试失败。真实多卡/多节点网络、RDMA/SDMA 与 fabric 行为需在目标环境执行。

## 常见失效条件

- 只在单卡测试通信：无法覆盖对称性、共享 VA 窗口和 collective gate。
- 对 collective 超时无限重试：没有区分 `FAILED` 与 `UNKNOWN`，放大拥塞。
- 用本地返回代表集合完成：把单 rank 提交成功当作全体完成。
- 释放顺序错误：先释放 backing 再 unmap，或 `release_heap` 未先 unreserve VA。
- 忽略固定槽耗尽：只盯带宽，不看 copy/event/SQ 槽位。
- rank 数变化未重建窗口：仍按旧 `rankCnt` 使用 VA 窗口。

## 关键不变量

```text
I1  ∀ rank r: window(r) valid ∧ buffer_layout(r) == expected
I2  collective_result == reduce(rank local results via firstError/gate)
I3  任意 rank 的失败最终被全体观察到同一终态（无永久等待）
I4  释放严格按 Unmap -> HalMemRelease -> erase slice；
    release_heap 先 unreserve VA 再 destroy entity
I5  fabric handle 在其映射存在期间不得释放
I6  集合超时进入 UNKNOWN，不可单方面释放或无限重试
I7  固定槽资源（copy/event/SQ）与带宽是两条独立的容量约束
```

## 与其他知识域的连接

- **K07 传输与拓扑**：P2P、RDMA、SDMA 是集合通信的物理后端；pin/map 与完成语义在该域有更细的机制。
- **K05 异步完成与可见性**：集合完成是异步完成语义的“多参与者”版本，四时刻规则仍然适用。
- **K09 错误与恢复**：集合超时/取消必须按 `FAILED`/`UNKNOWN` 三态处理，失败要广播并统一清理。
- **K06 调度与隔离**：参与的 rank 集合、优先级和配额影响集合操作能否按时对齐。
- **K11 性能与容量**：算法（ring/tree/递归倍增）与拓扑共同决定带宽利用率和尾延迟。
- **K12 生产演进**：rank 数、拓扑和 fabric 后端的变化需要重新对齐与灰度。

## 题目索引

本域对应题目（按 `知识域：` 字段确认）：`HRS-B009`、`HRS-I008`、`HRS-A008`、`HRS-E008`。

- `HRS-B009` 概念题：跨卡/跨节点集合通信需要保证哪些资源与完成语义。
- `HRS-I008` 分析题：如何设计跨卡/跨节点集合通信与 RDMA 的稳定性验证。
- `HRS-A008` 设计题：集合通信如何选择算法并处理 RDMA 拥塞。
- `HRS-E008` 高级题：跨卡/跨节点集合通信的可扩展性与容错设计。

## 证据入口

- `[已确认]` `cann/90-cross-module/pooling-and-resource-management.md:87-93`：每 PE VA 窗口、`MemSlice`、export/import、map/unmap 顺序、`firstError + collective gate`。
- `[已确认]` `cann/90-cross-module/memory-and-resource-lifecycle.md:24-25`：四时刻规则同样适用于集合/异步路径。
- `[已确认]` `cann/90-cross-module/cross-module-call-chains.md:28-40`：GE V2 执行的 IO 指定、资源占用与 ExecuteGraph 顺序。
- `[已确认]` `cann/00-overview/evidence-index.md:27` 和 `cann/90-cross-module/pooling-and-resource-management.md:45-49`：固定槽资源用 `BufferAllocator` + `Bitmap`。
- `[已确认]` `cann/README.md:23`、`cann/01-modules/M04-driver/source-map.md:36`：Driver 提供 RoCE / HDC / 队列等硬件接入能力（Ascend910B 含 RoCE）。
- `[推断]`：集合状态机、对称性不变量与伪代码由上述证据推导；`[待验证]`：集合算法性能、RDMA 拥塞与真实网络容错。

## 学习提示

先用 `HRS-B009` 建立“对称性 + 集体完成 + 生命周期”三要素，再做 `HRS-I008` 的稳定性验证设计，重点覆盖单 rank 失败、单 rank 超时和释放乱序。进阶读 `HRS-A008`：算法选择如何改变流量分布、拥塞如何反馈到超时判定。最后用 `HRS-E008` 检验可扩展性：rank 数增大时窗口保留、gate 开销和容错重建是否仍成立。
