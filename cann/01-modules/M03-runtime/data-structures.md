# M03 Runtime 数据结构与资源

- 文档目的：解释 01-modules/M03-runtime/data-structures.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cann HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：Device/Context/Stream/SOMA/KernelMemoryPool 关键字段和所有权已确认；完整全量布局仍待补
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M03-runtime/data-structures.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。

## 资源对象

| 对象 | 作用 | 关键不变量 |
|---|---|---|
| Runtime `Api` singleton | C API 到内部实现的门面 | 初始化前不能使用内部能力 |
| Device | 设备选择、属性和 reset | device ID 必须有效；Context 通过 Device 访问 Driver |
| Context | 当前执行环境和资源 owner | Stream/Event/Model/任务需匹配 Context；状态必须可访问 |
| Stream | 异步任务队列和顺序 | 提交后保持有效；销毁前 flush/teardown |
| Event | 完成和跨队列依赖 | 等待关系和 Registry sequence 记录需先于释放维护 |
| KernelMemoryPool | Runtime 内部 Kernel/Program/Binary 小块分配 | 固定 2 MiB backing；按 read-only 分池；非 SOMA |
| SOMA `SegmentManager` | `rtMemPool_t` 的区间、Segment 和统计管理 | `allocedMap_`、`cachedSegs_`、`freeSegs_` 与双向链一致 |
| SOMA `Segment` | 单个虚拟地址区间状态 | FREE/BUSY/CACHED 状态和 stream/seq 元数据一致 |
| Device memory | 输入、输出、workspace 和内部 buffer | 地址/容量/对齐满足 API 和模型约束 |
| Model/Kernel handle | 可执行对象 | 释放前不可有未完成任务 |
| Queue/HDC handle | 向 Driver 提交工作 | Driver session 生命周期先于 queue 使用 |

## SOMA 关键字段

`SegmentManager` 使用 `allocedMap_` 记录 BUSY 基地址，`cachedSegs_` 记录逻辑释放段，`freeSegs_` 记录可立即分配段；`busySize_` 是已分配大小，`reserveSize_` 是已分配加缓存大小。`state_` 保存 Stream/Event 依赖开关和 watermark，`canDelete_`、`isIPCPool_` 影响销毁/分配边界 `[runtime/src/runtime/feature/soma/stream_mem_pool.hpp:107-171]`。

`PoolRegistry` 以按池基址排序的 `entries_` 定位池，以 `poolOwnership_` 保存 shared ownership，并维护 Event/Stream sequence map。查询返回 `shared_ptr`，因此查询期间 manager 不会仅因 Registry 移除而立刻析构 `[runtime/src/runtime/feature/soma/stream_mem_pool.hpp:180-230]`。

## 全局缓存

SoC 版本和平台资源信息由 ACL Runtime 基础实现缓存，包含 AICore、Vector Core、Cube Core 数量 `[runtime/src/acl/aclrt_impl/acl_rt_impl_base.cpp:41-151]`。

## 句柄安全

C API 不直接暴露内部 C++ 对象，而通过 `RT_VALIDATE_AND_UNWRAP_OBJECT` 等宏校验和解包 `[runtime/src/runtime/api/api_c.cc:118-153]`。SOMA 的 `rtMemPool_t` 在当前实现中承载 `SegmentManager*`，但调用者仍必须通过公开 API 使用，不能依赖内部布局 `[runtime/src/runtime/feature/soma/soma.cc:245-264]`。

## 内存池选择矩阵

| 对象 | 索引/容器 | 选择策略 | 回收条件 |
|---|---|---|---|
| KernelMemoryPool 空闲块 | `MemoryList` 链表 | First-Fit | 池管理器按空闲池数量和状态回收 |
| SOMA FREE Segment | `(size, basePtr)` 排序集合 | `lower_bound`，Best-Fit | force-free/trim 或 manager 生命周期 |
| Driver V2 node | VA、size、mapped cache 多棵树 | exact/upper-bound；小请求先 mapped cache | threshold + 完整 mapped backing |
| Driver V3 area | range 地址树 + 全局 size tree | exact/upper-bound；Best-Fit 行为 | 完整 idle range + shrink threshold |

这里的 Driver 对象不属于 Runtime 的 C++ `SegmentManager`；跨层只通过 HAL/Driver ABI 联系。

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
