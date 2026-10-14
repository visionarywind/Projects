# K07：Host-device 传输、NUMA、IOMMU 与 P2P

## 边界

本知识域讨论一份数据在主机内存、设备内存和多设备之间移动时的路径与代价：H2D、D2H、同卡 D2D、跨卡 P2P，以及支撑这些路径的 pin/map、DMA、IOMMU 地址转换、NUMA 节点和 PCIe / peer 链路选择。

讨论对象是“搬运路径和它的资源前置条件”，不是分配器内部的碎片策略（那是 K04）、也不是异步完成语义（那是 K05）。一次 `memcpy` 的返回、`halMemAlloc` 的 flags、`rtMallocHost`/host register 的软件表，都是本域的事实边界。特定厂商的链路带宽、IOMMU 缺页行为、P2P 可用性必须绑定目标 SoC、Driver、固件和拓扑，不能从另一代产品外推。

本域的证据只支持“路径和属性如何被选择”，不含设备实测。任何带宽、延迟、首帧成本的数字都属于 `[待验证]`。

## 核心机制

### 1. 传输链路由四个互相独立的变量决定

把“同一份数据的传输代价”拆成四个正交维度，而不是一个拷贝时间：

1. **位置**：数据落在主机哪个 NUMA 节点、目标设备挂在哪个节点，决定跨越哪些互连；
2. **映射**：主机内存是否 pin、是否建立设备/IOMMU 映射，决定首次访问的固定成本；
3. **方向**：H2D、D2H、同卡 D2D、跨卡 P2P，四者走的物理链路不同；
4. **后端**：经普通 DMA、P2P 直连、RDMA 还是 SDMA，语义和成本都不同。

`[推断]` 工程上把“首帧固定成本”和“稳态带宽”分开：pin/map、页固定、allocator 首次建 backing 属于一次性成本；稳态受链路带宽、对齐和并发影响。

### 2. 分配属性如何进入 backing 选择

`[已确认]` Runtime 普通 `rtMalloc` 先解析 memory type、policy、advise、module/device config 并做 32-byte 对齐；Driver 随后把页类型、HBM/DDR/P2P、节点和 module ID 编进 HAL flags，再调用 `halMemAlloc`。[cann/90-cross-module/memory-and-resource-lifecycle.md:80-82]

这意味着 NUMA 节点与 P2P 不是传输时的临时选项，而是分配时就被写进 backing 的属性。属性一旦不一致，后续传输可能落到非预期路径，甚至根本不可用。

`[已确认]` Driver V3 `cache_init` 根据 host/device、P2P、huge-page 的组合创建多类 allocator，并按 `(devid, flag)` 建立全局 allocator 表；产品反初始化前先 `svm_cache_shrink` 再销毁 allocator。[cann/90-cross-module/pooling-and-resource-management.md:67-71]

`[已确认]` SHMEM 初始化后端为 P2P / RDMA / SDMA 建立 host/device heap base 指针数组；`remove_heap` 释放这些数组并 unmap，`release_heap` 先 unreserve VA 再 destroy entity。[cann/90-cross-module/pooling-and-resource-management.md:87-93]

也就是说，“能用 P2P”必须先有对应 flag 的 allocator / heap 后端存在，而不是运行到传输时才决定。

### 3. Host 内存的 pin/map 与 Runtime 软件表

`[已确认]` Host allocation 按 32-byte 对齐；host register 在 Driver pin/map 能力不足时，由 Runtime 维护 mapped / pinned 软件表，并在 unregister 时分别清理；重复注册的 Driver BUSY 映射为 `RT_ERROR_HOST_MEMORY_ALREADY_REGISTERED`。[cann/01-modules/M03-runtime/memory-pool-analysis.md:174]

`[已确认]` Host free / unregister 的顺序必须与 location、map、pin 状态相符。[cann/90-cross-module/memory-and-resource-lifecycle.md:78-82]

这条软件表是“pin 相关资源要能闭合归零”的证据来源：如果 pin 能力由 Runtime 自己记账，那么 unregister 漏掉一步，pin 计数就不会归零。

### 4. NUMA 门控：什么时候请求会绕过 cache 走普通路径

`[已确认]` Driver V3 的 `go_malloc_cache` 不是只看大小，而是要求 `numa_id == SVM_MALLOC_NUMA_NO_NODE`、flag 支持 cache、对应 `(devid, cache_flag)` 已有 allocator、`align == alloc_gran`，并且对齐后大小不超过请求阈值；否则请求落到 `malloc_normal(devid, numa_id, flag, align, size)`。[cann/01-modules/M04-driver/driver-memory-pool-analysis.md:1128-1156]

这是一个真实的分支门控：**带明确 NUMA 节点的请求默认不进 cache**。所以“显存还有空闲却分配慢/失败”有时不是容量问题，而是请求属性把路径切到了 normal allocator。NUMA 属性同时决定 backing 落在哪个节点，以及是否复用 cache。

### 5. 传输缓冲区的状态机

把一次传输建模成显式的资源状态机，避免“指针非空就等于可 DMA”：

```text
ALLOCATED
  --register/pin (host)--> PINNED
  --map (IOMMU/device)--> MAPPED
  --submit transfer-----> IN_FLIGHT
  --completion evidence-> COMPLETED
  --unmap---------------> UNMAPPED
  --unregister/unpin----> UNPINNED
  --free----------------> FREED

任意状态 --错误/超时--> UNKNOWN（保留映射，不得直接释放）
```

`[推断]` 该状态机由 host register 软件表、`Unmap -> HalMemRelease -> erase slice` 释放顺序和四时刻规则共同推出。[cann/90-cross-module/pooling-and-resource-management.md:87-93] [cann/90-cross-module/memory-and-resource-lifecycle.md:24-25]

### 6. 传输准备与拆卸的伪代码

```python
def prepare_transfer(host_buf, dev_buf, direction, node_hint):
    # 1. 对齐与属性检查：location / flag / node 必须和目标路径一致
    assert aligned(host_buf) and aligned(dev_buf)
    # 2. 主机内存需要设备访问时必须 pin + map（首次成本）
    if direction in ("H2D", "D2H") and not is_pinned(host_buf):
        pin(host_buf)                      # Driver 不支持时记入 Runtime 软件表
        map_for_device(host_buf, dev_buf)
    # 3. 选择后端：同卡 DMA / 跨卡 P2P / RDMA / SDMA，取决于拓扑与 flag
    backend = choose_backend(host_buf, dev_buf, node_hint)
    # 4. 提交仅代表进入队列，不是完成
    handle = submit(direction, backend, host_buf, dev_buf)
    return handle

def teardown_after_complete(handle, host_buf):
    # 只有拿到完成证据才能拆
    assert wait_completion(handle) is COMPLETED
    unmap(host_buf)                        # 先解除映射
    unpin(host_buf)                        # 再解除 pin，软件表计数归零
    # unmapped/unpinned 之后才允许 free
```

上面的 `choose_backend`、`pin`、`map` 都是可读模型，不代表任一厂商 API 的完整实现。

### 7. 成本模型

```text
cost = pin_map_setup(once, if host)
     + route_penalty(src_node, dst_node, link)
     + size / effective_bandwidth(link, align, concurrency)
     + visibility_sync(completion_evidence)
```

一致性与可用性条件：

```text
transfer(src, dst) feasible →
      flag(src) compatible flag(dst)
  and topology(src, dst) supports chosen backend (P2P / RDMA / SDMA / DMA)
  and mapping(src) established before submit
```

其中 `route_penalty`、`effective_bandwidth`、`topology` 是否成立不由文档给出，必须在目标环境测量 `[待验证]`。

### 8. 失败模式

- 属性不一致：分配用 P2P/huge/node flag，传输却按普通页假设，路径与预期不符。
- 映射缺失：主机指针“能访问”只说明 CPU 能访问，不代表设备 DMA 可达；缺 map 会得到 IOMMU fault 或错误数据。
- 释放早于完成：在完成证据之前 unmap/unpin，DMA 读到的内存被回收或重映射。
- pin 泄漏：软件表记账未随 unregister 归零，长期耗尽固定能力。
- 单次外推：用一次大拷贝的耗时外推稳态带宽，混淆首帧与稳态。

### 9. IOMMU 与地址转换的边界

`[推断]` 设备发起的 DMA 使用设备可见地址，需要经过 IOMMU/IOTLB 转换到物理页。这个翻译项在 map 时建立；如果主机页没有 pin/map，设备访问该地址会在地址转换处失败，表现为设备访问错误而不是普通的段错误。因此“CPU 能解引用”和“设备能 DMA”是两个不同的能力，前者不蕴含后者。

IOMMU 的存在还意味着：释放主机页之前必须先拆除映射，否则 IOTLB 中可能仍留有指向已回收物理页的翻译项。这与不变量 I2 一致，但设备是否立即失效、是否需要显式 TLB 失效，必须绑定目标 SoC/Driver 验证 `[待验证]`。

### 10. 对齐、分块与拷贝效率

`[已确认]` Host allocation 按 32-byte 对齐。[cann/90-cross-module/memory-and-resource-lifecycle.md:78-82]

`[已确认]` “Host-device 拷贝、未对齐 buffer 或不必要的内存分配”被列为潜在瓶颈。[cann/90-cross-module/performance-critical-paths.md:25-33]

`[推断]` 未对齐会迫使 DMA 产生额外的首尾残段拷贝，或退化为更慢的路径；这与链路带宽无关，属于对齐成本。工程上应保证地址、长度、页粒度一致，并尽量把多次小传输合并成对齐的批量传输，而不是盲目增加并发。

### 11. 验证 oracle 与实验设计

`[待验证]` 一个不依赖具体数值、只验证结构和不变量的实验：

| 维度 | 输入变化 | 期望（方向性，非倍数） | oracle |
|---|---|---|---|
| 位置 | 本地节点 vs 远端节点 | 远端更慢 | 数据逐字节一致，pin/map 计数归零 |
| 映射 | 已 pin/map vs 未 pin/map | 未映射首帧更慢或失败 | 映射存在时才允许 submit |
| 方向 | H2D / D2H / 同卡 D2D / 跨卡 P2P | 链路不同，代价不同 | 完成证据齐备后才读结果 |
| 对齐 | 对齐 vs 非对齐 | 非对齐更慢 | 结果一致，无越界 |
| 后端 | P2P 可用 vs 不可用 | 不可用时退化或不支持 | 属性与后端一致，否则结构化报错 |

判定准则：首帧成本与稳态带宽可分离；释放后 pin/map 计数不归零即失败。真实链路带宽、P2P 可用性和 IOMMU 行为必须由目标环境产生，本文不给数字。

## 常见失效条件

- 把“CPU 能访问的指针”当作“设备可高效 DMA”：忽略 pin/map，首次访问失败或极慢。
- 把设备可见当作传输已完成：`submit`/`copydata` 返回后立即读目标 buffer，读到旧数据。
- 忽略 NUMA：数据在远端节点，跨节点互连抬高延迟；或带节点的请求绕过 cache 走 normal 路径。
- pin/map 不一致：释放顺序与 location/map/pin 状态不符，留下映射或 pin 泄漏。
- 用单次大拷贝外推稳态带宽：没有把一次性固定成本和稳态带宽分开。
- 假设 P2P 一定可用：拓扑、属性和后端任一不支持都退化为经主机中转或不支持。

## 关键不变量

```text
I1  device_readable(ptr) ⇒ mapped(ptr)
I2  unmap(ptr) 仅在 in_flight_dma_count(ptr) == 0 后允许
I3  pin_count(ptr) 与 Runtime 软件表记账在 unregister 后归零
I4  传输完成后数据逐字节一致（oracle），仅凭 submit 返回不成立
I5  backing 的 node/P2P/page 属性与目标传输后端一致
I6  UNKNOWN（超时/取消未确认）状态下不得 unmap/unpin/free
```

## 与其他知识域的连接

- **K04 内存与地址空间**：`rtMalloc`/`rtMallocHost`/host register 是传输的前置；backing 的 flag、对齐和池路径决定可用的传输后端。
- **K05 异步完成与可见性**：传输是异步的，`cost` 模型里的 `visibility_sync` 与四时刻规则直接来自完成语义。
- **K06 调度与隔离**：拓扑感知放置、NUMA 亲和和带宽配额影响多个租户的传输公平性。
- **K08 集合通信与 RDMA**：跨节点传输的后端（RDMA/SDMA/fabric handle）是集合通信的物理基础。
- **K11 性能与容量**：首帧成本、稳态带宽、对齐和并发是容量与降级模型的输入。
- **K02 设备发现与拓扑**：peer 能力、NUMA 距离和链路类型属于拓扑快照，是传输路径选择的输入。

## 题目索引

本域对应题目（按 `知识域：` 字段确认）：`HRS-B008`、`HRS-I007`、`HRS-A007`、`HRS-E007`。

- `HRS-B008` 概念题：同一份数据在主机/设备内存，传输代价与 NUMA、IOMMU、P2P 差异。
- `HRS-I007` 分析题：如何分析传输、NUMA、IOMMU 与 P2P 的稳定性。
- `HRS-A007` 设计题：带宽-延迟取舍与数据搬运规划。
- `HRS-E007` 高级题：拓扑感知传输路径优化，并保证正确性不被优化牺牲。

## 证据入口

- `[已确认]` `cann/90-cross-module/memory-and-resource-lifecycle.md:78-82`：普通 device/Host memory 的 flags、对齐、pin/map 软件表与释放顺序。
- `[已确认]` `cann/90-cross-module/pooling-and-resource-management.md:67-71`：Driver V3 `cache_init` 按 host/device、P2P、huge-page 组合建 allocator。
- `[已确认]` `cann/90-cross-module/pooling-and-resource-management.md:87-93`：SHMEM 的 P2P/RDMA/SDMA heap 后端与 `Unmap -> HalMemRelease -> erase slice` 顺序。
- `[已确认]` `cann/01-modules/M03-runtime/memory-pool-analysis.md:174`：host register 软件表与重复注册错误映射。
- `[已确认]` `cann/01-modules/M04-driver/driver-memory-pool-analysis.md:1128-1156`：NUMA/flag/align/size 对是否进入 V3 cache 的门控。
- `[已确认]` `cann/90-cross-module/performance-critical-paths.md:25-33`：Host-device 拷贝、未对齐 buffer 属于潜在瓶颈；「可能瓶颈」不等于已测收益。
- `[已确认]` `cann/90-cross-module/memory-and-resource-lifecycle.md:24-25`：主机 API 成功、元数据变化、Driver 接收、设备完成是四个不同时刻。
- `[推断]`：状态机、成本模型与不变量由上述证据推导；`[待验证]`：链路带宽、延迟、P2P 可用性与 IOMMU 行为。

## 学习提示

先读 `HRS-B008` 建立“位置—映射—方向—后端”四变量模型，再做 `HRS-I007` 的稳定性分析，把 pin/map 生命周期和完成边界的失败点列全。进阶用 `HRS-A007` 做带宽-延迟取舍，最后用 `HRS-E007` 检验：当优化改变了传输路径和后端时，如何用不变量 I1–I6 保证数据正确性和资源可回收，而不是只看吞吐。
