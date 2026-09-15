# D02-S06 修改练习

- 文档目的：解释 80-demos/D02-interview-binary/modification-exercises.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/LeetCUDA HEAD 4513b31（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 80-demos/D02-interview-binary/modification-exercises.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


1. 为 HGEMM warmup 同步补 `check()`，比较错误传播一致性。
2. 在 `check_smem_feasible` 旁记录每个 stage/swizzle 配置的 skip 原因和 shared-memory bytes。
3. 增加一个小矩阵 `M/N/K` 的 correctness-only 路径，分离 benchmark 与验证。
4. 为不同 `--arch` 输出编译宏和实际 binary 名称，避免运行错架构。
5. 给 host/device/event/stream/TMA map 资源引入统一清理 helper 或 RAII，并对照现有显式释放。
6. 对 FA 非整 tile、非对齐 sequence、D=64/128 运行 sanitizer，再更新 M07 风险。

所有练习未执行；修改应同时更新源码证据、testing、risk-register 和本 Demo 映射。

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
