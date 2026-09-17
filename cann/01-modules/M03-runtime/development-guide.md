# M03 Runtime 开发指南

- 文档目的：解释 01-modules/M03-runtime/development-guide.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cann HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M03-runtime/development-guide.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


## 修改前

- 找到公开 C API、对应 `Api` 方法和内部对象的完整链路。
- 确定错误码是否需要扩展映射，保留底层错误上下文。
- 明确资源的创建线程、Context 归属、异步完成和销毁顺序。
- 判断变更属于 KernelMemoryPool、SOMA 还是普通 device/Host memory；三条路径不能用同一套生命周期假设。
- 对 SOMA 变更核对 `SegmentManager` 状态、PoolRegistry ownership、AICPU 参数和 HAL/SDK-driver ABI；特别确认 `TryToReuse` 与 `MemPoolTrimImplicit` 的当前实现状态。
- 检查 CMake 目标、安装库和 ACL 导出表是否需要同步修改。

## 修改中

- 复用现有验证和错误宏，不在单个 API 中引入另一套返回规则。
- 不直接暴露内部对象布局；保持 C ABI 稳定。
- 所有锁都要检查异常路径、锁顺序和潜在回调重入。
- 变更调度或内存时，同时考虑 DFX、profiling、dump 和 Driver 协议。
- 不把 Segment 的本地状态变化写成设备侧已完成；区分提交成功、callback/event 完成和同步完成。
- 若启用或恢复 SOMA 重用/隐式 trim，必须补足策略选择、失败回滚、统计一致性和跨 Stream/Event 测试，不能只修改辅助函数。

## 修改后

先跑无设备 UT，再在匹配 Toolkit/Driver/Firmware/NPU 上跑集成和压力测试；记录版本、配置、设备和测试结果。对内存变更至少覆盖重复 free、越界、异步提前释放、trim 失败、Context/Stream 销毁失败以及普通 free 与同步 free 的差异。

## 内存改动的判定清单

- 先判断请求属于 KernelMemoryPool、SOMA 还是 ordinary `rtMalloc`；不要复用另一条路径的 Fit、锁或 free 假设。
- 修改 SOMA 时同时核对 `SegmentManager`、`PoolRegistry`、AICPU 参数和 Driver VMM；明确 `TryToReuse`/implicit trim 是当前 no-op 还是目标行为。
- 修改普通内存 flags、align 或 policy 时，继续追踪 `NpuDriver` 到 Driver V2/V3；产品 `ascend910B`/`ascend910_93` 与 `ascend950` 的实现不同。
- 修改 free/trim 时分别验证本地 metadata、Driver backing、异步任务完成和错误回滚。

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
