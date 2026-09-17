# M03 Runtime：模块入口

- 文档目的：解释 01-modules/M03-runtime/README.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：`50be4c922`
- 证据状态：C API、构建分层和 ACL Runtime 映射已确认
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M03-runtime/README.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


Runtime 是设备运行时和维测基础设施，负责 Device、Context、Stream、Event、内存、任务调度、模型/Kernel 执行以及 log、profiling、dump、error manager 等 `[runtime/README.md:7-14]`。

## 关键入口

- `rtSetDevice`、`rtGetDeviceCount`、`rtDeviceReset`、`rtDeviceSynchronize`：`src/runtime/api/api_c_device.cc` `[49-119,208-215]`。
- 通用 C API 门面：`src/runtime/api/api_c.cc`，通过 `Api::Instance()`、句柄校验宏和错误宏委托内部对象 `[118-153]`。
- `src/acl/aclrt/acl_rt.cpp`：ACL Runtime 导出映射和 Hook `[runtime/src/acl/aclrt/acl_rt.cpp:17-104]`。
- `src/acl/aclrt_impl/acl_rt_impl_base.cpp`：SoC、平台信息和 ACL 初始化全局状态 `[25-199]`。
- [内存池源码级分析](memory-pool-analysis.md)：KernelMemoryPool、SOMA 与普通设备/Host 内存的分层、状态、锁、异步 ABI 和验证边界。

## 已确认的深层边界

- `rtSetDevice` 并非只更新一个线程变量：`ApiImpl::SetDevice` 取得 `PrimaryContextRetain` 返回的引用对象，写入线程局部 current reference，校验默认 Stream，设置设备 SatMode，清除显式 current Context，并发出设备状态回调 `[runtime/src/runtime/api/impl/api_impl.cc:2914-2940]`。
- 普通 `rtMalloc` 按 policy、module/device 配置和 32-byte 对齐后进入 `NpuDriver::DevMemAlloc`；该 Driver 再按 online/offline/AICPU-scheduler 选择 `halMemAlloc` flags `[runtime/src/runtime/api/impl/api_impl_memory.cc:765-842]` `[runtime/src/runtime/driver/npu_driver_mem.cc:1026-1114,1302-1326]`。
- `rtMemPool*` 的 Segment 复用选择入口当前是 no-op；显式 trim 有 Driver→本地两阶段，而隐式 trim 当前直接成功返回。详见专题，不将设计意图写成当前行为。

## 进一步阅读：内存分层

普通 `rtMalloc` 不等于 Runtime 内部池分配：KernelMemoryPool 服务 Kernel/Program/Binary，SOMA 服务 `rtMemPool*` Stream-ordered API，普通设备内存经 `NpuDriver::DevMemAlloc` 后还可能进入 Driver V2/V3 ordinary cache。三条路径的对象、锁、回收和异步语义不同，详见 [memory-pool-analysis.md](memory-pool-analysis.md)。

已确认的实现差异：KernelMemoryPool 是 2 MiB backing 上的 `MemoryList` First-Fit；SOMA FREE Segment 使用 `lower_bound` 的按大小 Best-Fit，但当前 `TryToReuse` 和隐式 trim 为 no-op；Driver ordinary cache 的 V2/V3 选择由 SoC 产品构建决定，而非本文档层面的运行时猜测。

## 文档元数据（规范补充）

- 文档目的：说明 `01-modules/M03-runtime/README.md` 的源码分析范围、结论和维护入口。
- 适用范围：当前项目对应模块/入口的静态源码与测试分析。
- 对应源码版本：以本项目 `00-overview/analysis-state.md` 或同页版本字段为准。
- 证据状态：静态源码证据；未执行的构建、测试、GPU、网络或多进程行为保持“未验证”。
- 最后更新：2026-09-15
- 前置阅读：本项目根 README 与 `00-overview/analysis-state.md`。
- 后续阅读：本模块/示例的实现、测试和风险页面。

## 深度审计

| 分析对象 | 入口落地 | 正常路径 | 分支 | 异常 | 清理 | 数据生命周期 | 执行上下文 | 行级证据 | Demo 映射 | 状态/缺口 |
|---|---|---|---|---|---|---|---|---|---|---|
| `01-modules/M03-runtime/README.md` | 已完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 已映射或不适用 | 部分完成：动态行为、边界或专用变体仍需验证 |

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
