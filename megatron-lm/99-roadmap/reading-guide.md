# 阅读路线

- 文档目的：解释 99-roadmap/reading-guide.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/megatron-lm HEAD 3703d4e33（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 99-roadmap/reading-guide.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


## 初学者

README → project-overview → runtime-model → D01 → M02 README → M03 README。

## 模型开发

M01 design/implementation → TransformerBlock forward → M02 schedule → M05 checkpoint。

## 训练系统开发

M03 implementation → M02 execution-flows → M05 optimizer/checkpoint → error-boundaries。

## 数据开发

M04 README → GPTDataset → sampler → `pretrain_gpt.get_batch` → shared-data-and-types。

## 推理开发

M06 README → StaticInferenceEngine → DynamicInferenceEngine → GPTInferenceWrapper → server 入口。

每条路线都应回到 `evidence-index.md` 核对源码版本和行号。

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
