# 技术债务

- 文档目的：解释 99-roadmap/technical-debt.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：`main` / `4513b31`。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-10
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 99-roadmap/technical-debt.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。

## 已确认债务

1. 没有统一全仓库构建/测试编排和依赖 lockfile。
2. 没有统一 device/stream/contiguous/launch-error 检查规范。
3. 普通 extension、wheel、standalone binary 的接口和资源模型不同。
4. 深层 Tensor Core/CuTe 模板难以只靠 README 维护。
5. benchmark 历史数据缺少统一机器/版本元数据。
6. 没有 GPU CI，因此架构回归只能在目标环境发现。

## 建议顺序

P0：固定版本/环境记录，补最小 correctness 和 NMS regression。
P1：统一输入检查、错误检查和 benchmark metadata。
P2：建立按 SM 的编译 smoke matrix 和 sanitizer job。
P3：再考虑共享 launcher、统一 CMake 或测试 harness；不能先假设这些重构不会破坏教学边界。

证据：`[00-overview/analysis-state.md:27-44]`、`[source/LeetCUDA/.pre-commit-config.yaml:1-41]`。

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
