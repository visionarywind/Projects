# M03 数据结构

- 文档目的：解释 01-modules/M03-training-runtime/data-structures.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/megatron-lm HEAD 3703d4e33（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M03-training-runtime/data-structures.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


| 对象 | 生命周期 | 说明 |
|---|---|---|
| `PretrainConfigContainer` | pretrain 入参 | 汇总配置和 provider |
| model list | setup→train→save | VP 时包含多个 chunk |
| optimizer/scheduler | setup→每步→save | 更新和恢复训练状态 |
| data iterator | dataset build→消费 | 提供 batch 给 forward step |
| `losses_reduced` | 单步产生→日志/聚合 | 不等于完整训练状态 |
| iteration/token counters | loop 持有 | checkpoint 和退出依据 |

不能把 model list 中的 wrapper、底层 module 和 checkpoint state dict 混为同一所有权对象；D01 显式 unwrap DDP 后生成 model shard。

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
