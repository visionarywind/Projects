# M01 GE 风险与技术债务

- 文档目的：解释 01-modules/M01-ge/risks-and-debt.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cann HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：风险评估；不是已验证缺陷
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M01-ge/risks-and-debt.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。

## 高风险

- Graph/Model/Runtime/算子包版本不匹配会导致编译、加载或执行失败。
- V2 Executor 状态顺序、init/de-init 图失败和 Stream/Event/allocator 生命周期错误可能泄漏资源或产生异步故障 `[ge/runtime/v2/core/model_v2_executor.cc:201-318]`。
- GE 到 Runtime 的跨仓 ABI 和设备错误码转换未完全静态追踪。

## 中风险

- V1/V2 自动选择、Sink/SuperKernel 启用条件和动态 shape 行为需要按场景确认。
- Compiler Pass 顺序、EnginePartitioner、Memory Planner 和流分配细节尚未形成完整结构图。

## 待补

- 完成 V1/V2、Compiler、Parser、MetaDef 的符号级调用图。
- 建立模型格式、Graph/Tensor ABI 和算子包兼容矩阵。
- 增加 ASAN、异步故障注入、重复生命周期和无设备 mock 回归。

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
