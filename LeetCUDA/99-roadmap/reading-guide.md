# 阅读路线

- 文档目的：解释 99-roadmap/reading-guide.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：`main` / `4513b31`。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-10
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 99-roadmap/reading-guide.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。

## 按目标选择

| 目标 | 路线 |
|---|---|
| 初学 CUDA | M01 → M02 → M03 → M05 |
| 写 PyTorch extension | M08 → M02 → M10 → M06 |
| 学矩阵性能 | M05 → M06 → M11 |
| 学 attention | M03 → M06 → M07 → M09 |
| 面试复习 | M09：common → base → GEMV/GEMM → HGEMM → FlashAttention |
| 调试竞态 | M08 → M10 → error boundaries → sanitizer |

## 每个模块的固定阅读法

1. README：职责和边界。
2. source-map/interfaces：入口和契约。
3. call-chains/line-level：控制流和同步。
4. testing/risks：验证和已知债务。
5. 对照源码，不把推断当确认。

## 证据等级

- **已确认**：源码、构建脚本或可复现命令直接支持。
- **推断**：由目录/命名/调用关系归纳，需补源码或实验。
- **未知**：当前没有足够证据，不能作为 API 承诺。

## 相关入口

[README](../README.md)、[module-registry](../01-modules/module-registry.md)、[analysis-state](../00-overview/analysis-state.md)。

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
