# M04 Driver 数据结构与资源

- 文档目的：解释 01-modules/M04-driver/data-structures.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cann HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：关键资源布局和所有权已确认；完整内部结构未知
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M04-driver/data-structures.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。

## HDC client

创建函数分配 client 和 session 数组，并记录 magic、服务类型、超时、session 状态、锁等字段 `[driver/src/ascend_hal/hdc/common/hdc_client.c:49-111]`。client 是 session 的拥有者；销毁必须晚于所有 session。

## Queue context

`queue_drv_open` 使用当前进程 TGID 建立上下文，保存于 `file->private_data`；`queue_drv_release` 负责移除进程记录和释放上下文 `[driver/src/sdk_driver/queue/host/queue_fops.c:62-95]`。其典型所有权是 file descriptor → queue context → 进程/设备队列。

## Vector/DMA 参数

`queue_check_vector` 验证 vector 数量、每项地址与长度的一致性 `[queue_fops.c:149-175]`。这类参数可能进一步用于 DMA 或 HDC 传输，必须避免溢出、越界和生命周期失效；具体 pin/map/unmap 代码待继续追踪。

## SVM ordinary cache

| 版本 | 核心对象 | 索引 | 空间状态 |
|---|---|---|---|
| V2 | heap、`devmm_rbtree_node` | alloced、idle VA、idle size、idle mapped cache trees | mapped/unmapped、allocated/idle |
| V3 | `cache_allocator`、`ga_inst`、`ga_range`、`ga_area` | range 地址树、area 地址树、全局 size tree | backing range 与其中 free area |

V2 node 的 `va/size/total/flag/advise` 描述 VA、当前片段、原始 backing 和内存属性；V3 的 `range` 是 backing 所有权边界，`area` 是可复用的空闲片段。一个 area 同时进入地址索引和大小索引，支持按大小分配、按地址合并。完整字段与生命周期见 [driver-memory-pool-analysis.md](driver-memory-pool-analysis.md)。

## 资源回收不变量

- V2 只有完整 mapped node（`total == size`）才适合作为独立 shrink 单位；底层释放失败时需恢复 cache tree 和统计。
- V3 只有 `idle_area_size == range->size` 的完整 idle range 才能从 gen allocator 移除；底层 BUSY 时进入延迟 recycle。
- cache 元数据不能与不同 flag、page type 或设备的 backing 混用。


`esched_ioctl.h` 定义 CPU ID、进程组、事件优先级、等待/提交事件和表项增删等结构，是跨用户态/内核态的数据契约。字段布局、兼容版本和安全拷贝规则应与实现配套审查。

## 相关文档
- [项目入口](../../README.md)
- [分析状态](../../00-overview/analysis-state.md)
- [源码证据索引](../../00-overview/evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](../../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](../../00-overview/analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
