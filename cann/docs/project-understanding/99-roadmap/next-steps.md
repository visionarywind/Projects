# 后续分析路线

- 文档目的：解释 99-roadmap/next-steps.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cann HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 99-roadmap/next-steps.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


- 当前版本基线：GE `8ee1b040a`、ACL `50be4c922`、Runtime `50be4c922`、Driver `977336913`

## P0：补齐跨仓关键路径

1. 追踪 Runtime `Api::SetDevice`、Context、Stream、Memory 和 Task 的实现。
2. 追踪 Runtime 到 HAL 的实际导出符号、动态库和错误码。
3. 追踪 Driver HAL 到 SDK-driver、ioctl、内核模块和设备节点。

## P1：补齐模块深度

- GE Compiler Pass、EnginePartitioner、Memory Planner、V1/V2 差异。
- ACL Model、Single-op、Tensor/DataTransfer 和 callback。
- Runtime scheduler、DFX、内存和多设备并发。
- Driver HDC、Queue、esched、SVM、TRS、DMS、RoCE。

## P2：验证和工具化

- 完成链接断链检查和证据索引自动生成。
- 在无硬件环境跑可运行的静态检查/UT，并保存结果。
- 在匹配 NPU 上执行样例、集成、故障注入和性能测试。
- 用 GDB、trace、profiling 和日志确认异步生命周期与错误传播。

完成每项后更新 `analysis-state.md` 的覆盖率、断点和证据状态。

## 相关文档
- [项目入口](../README.md)
- [分析状态](../00-overview/analysis-state.md)
- [源码证据索引](../00-overview/evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](../00-overview/analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
