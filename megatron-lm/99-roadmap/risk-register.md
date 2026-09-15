# 风险登记

- 文档目的：解释 99-roadmap/risk-register.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/megatron-lm HEAD 3703d4e33（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 99-roadmap/risk-register.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


| 风险 | 影响 | 缓解 |
|---|---|---|
| 静态分析替代动态验证 | 错误归因 | 明确标注未验证，补 smoke test |
| 并行配置组合爆炸 | 漏掉分支 | 以配置影响图逐项覆盖 |
| checkpoint 兼容性复杂 | 恢复失败 | round-trip 测试和 args 校验 |
| optional backend 差异 | TE/FlashInfer/ModelOpt 路径不同 | 按 backend 建立测试矩阵 |
| 隐式全局状态 | 调用顺序 bug | 记录初始化/销毁边界 |
| 推理 dynamic/static 兼容层 | 实际 engine 与名称不一致 | 明确记录 fallback 条件 |

当前最大风险是文档覆盖速度高于实测覆盖；后续应优先补 D01 和 focused unit tests。

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
