# M03 Runtime 行级分析

- 文档目的：解释 01-modules/M03-runtime/line-level-analysis.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cann HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：关键函数已确认
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M03-runtime/line-level-analysis.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。

## `rtSetDevice`

- 先等待全局状态锁，避免与相关全局状态操作冲突 `[runtime/src/runtime/api/api_c_device.cc:74-78]`。
- 获取 `Api::Instance()` 并委托 `SetDevice(devId)` `[79-81]`。
- 用统一宏/路径把内部错误转换为公开 Runtime 返回值 `[82-84]`。

## `rtGetDeviceCount`

获取单例、调用 `GetDeviceCount` 并返回统一转换结果 `[runtime/src/runtime/api/api_c_device.cc:49-55]`。

## ACL Runtime 状态

`aclSocVersionMutex` 保护 SoC 缓存；`aclInitMutex` 和 `aclInitRefCount` 管理初始化引用计数 `[runtime/src/acl/aclrt_impl/acl_rt_impl_base.cpp:25-32]`。`InitSocVersion` 在锁内读取并缓存 SoC 版本 `[120-135]`。平台更新进一步读取 Core 数量并更新资源信息 `[41-117]`。

## 资源限制

`rtsSetDeviceResLimit`、`rtsResetDeviceResLimit` 和 `rtsGetDeviceResLimit` 对设备资源限制提供 C API 门面 `[runtime/src/runtime/api/api_c_device.cc:165-205]`；策略和硬件实现需查内部类。

## 内存池相关行级核对

- KernelMemoryPool 的 `MemoryList::GetBlock` 是链表首个适配块；不要因名称 `pool` 推断 Best-Fit `[runtime/src/runtime/core/src/pool/memory_list.cc:14-122]`。
- SOMA `SegmentAlloc` 先尝试 `TryToReuse`，当前实现返回空指针，随后从 `freeSegs_` 按序查找并切分；隐式 trim 入口当前直接成功 `[runtime/src/runtime/feature/soma/stream_mem_pool.cc:140-195,345-361]` `[runtime/src/runtime/feature/soma/soma.cc:328-333]`。
- 普通 `DevMalloc` 完成对齐和策略映射后进入 Driver；是否进入 Driver cache 不能在 Runtime 入口处单独确定 `[runtime/src/runtime/api/impl/api_impl_memory.cc:765-842]` `[runtime/src/runtime/driver/npu_driver_mem.cc:1026-1114,1302-1326]`。

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
