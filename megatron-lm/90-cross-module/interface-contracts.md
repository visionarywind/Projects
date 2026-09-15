# 跨模块接口契约

- 文档目的：解释 90-cross-module/interface-contracts.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/megatron-lm HEAD 3703d4e33（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 90-cross-module/interface-contracts.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


| 契约 | 生产方 | 消费方 | 关键约束 |
|---|---|---|---|
| `ProcessGroupCollection`/parallel_state | M02 | M01/M03/M05/M06 | 初始化后才能使用；global fallback 与显式 PG 并存 |
| `forward_step_func` | 入口/M03 | M02 schedule | 返回 output tensor 与 loss function |
| batch dict | M04 | M03/M01 | keys、device、shape 与模型配置一致 |
| `GPTModel.forward` | M01 | M02/M03/M06 | stage flags、labels、inference context 匹配 |
| optimizer step result | M05 | M03 | update successful 才推进 scheduler |
| sharded state dict | M01/M05 | checkpoint writer | shard metadata 必须被 consumer 理解 |
| inference request | M06 | M06 engine/controller | sampling params、KV capacity、streaming 状态一致 |

接口风险主要来自隐式全局状态：args、parallel_state、random seed 和 distributed group 的生命周期应在文档/代码中显式标注。

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
