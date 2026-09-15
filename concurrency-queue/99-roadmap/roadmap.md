# 路线图与后续批次

- 文档目的：安排知识库和工程验证的增量维护。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/concurrency-queue HEAD 683b9e3（2026-09-15 只读确认）。
- 证据状态：优先级是分析建议，不是仓库已承诺计划。
- 最后更新：2026-09-10
- 前置阅读：[风险与技术债务](risks-and-debt.md)
- 后续阅读：[分析状态](../00-overview/analysis-state.md)
## 结论摘要

本页聚焦 99-roadmap/roadmap.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。

## P0：证据闭环

- 执行 native `make tests` 和 unit smoke。
- 运行核心单项、threaded、blocking、C API 测试并记录真实名称/输出。
- 检查所有新增 Markdown 相对链接和 evidence 行号。

## P1：核心协议验证

- 执行 fuzz/stress。
- 按环境运行 Relacy/CDSChecker。
- 对 memory-order、block recycle 和异常路径补充模型/压力证据。

## P2：交付矩阵

- 执行 CMake configure/build/install。
- 验证 RISC-V cross/QEMU（若工具链可用）。
- 记录 C ABI 编译和链接边界。

## P3：性能基线

- 固定机器和编译环境运行 benchmark。
- 保存原始 log、CSV 和场景参数。
- 只在重复测量后建立回归阈值。

每完成一批，更新 `analysis-state.md`；未执行或工具不可用都应保留为明确状态。

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
