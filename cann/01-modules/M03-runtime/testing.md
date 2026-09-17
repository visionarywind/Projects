# M03 Runtime 测试

- 文档目的：解释 01-modules/M03-runtime/testing.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cann HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：测试能力和入口已确认；结果未执行
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M03-runtime/testing.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。

## 分层测试

- C API 参数、句柄魔数和错误宏 UT。
- Device/Context/Stream/Event/Memory 生命周期 UT。
- KernelMemoryPool 的 0 字节、>2 MiB、read-only 分池、碎片和空闲池回收测试。
- SOMA 的 pool create/destroy、Segment split、重复 free、越界、BUSY/CACHED/FREE 状态、trim 和 AICPU 配置失败测试。
- scheduler、queue、异步同步和超时测试。
- DFX 日志、trace、profiling、dump 和错误上下文测试。
- 与 ACL、GE、Driver、固件和真实 NPU 的集成测试。

## 回归重点

1. `rtSetDevice` 的全局锁、Primary Context retain 和多线程初始化/Finalize。
2. 无效 handle、重复释放和跨 Context 使用。
3. 异步任务完成前释放 buffer/stream/event；对比普通 free 与 `rtFreeWithDevSync`。
4. SOMA `TryToReuse` 当前 no-op、显式 trim 和隐式 trim no-op 的行为回归，避免把辅助策略误当成已启用功能。
5. AICPU `SomaMemMng` launch 失败、本地 free 回滚和 Driver V3 free ioctl“不回滚”窗口。
6. SoC 缓存与多设备切换。
7. 资源限制 API 的设置、重置、查询一致性。

## 内存分层专项矩阵

- KernelMemoryPool：2 MiB 边界、First-Fit、read-only 分池、释放尾插、5 个空闲池阈值和碎片。
- SOMA：FREE Best-Fit、`TryToReuse` no-op 回归、同/跨 Stream、Event sequence、AICPU launch 失败、显式 trim、隐式 trim no-op、destroy busy。
- ordinary `rtMalloc`：V2 mapped/unmapped node 命中、V3 range/area split/merge、cache bypass、shrink、`DRV_ERROR_BUSY` 和产品构建映射。

这些测试项是待执行矩阵，不代表当前已经通过；没有 NPU、匹配 Driver/Firmware 和 Toolkit 时，设备侧完成语义仍未验证。

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
