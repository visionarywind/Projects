# D01 数据与状态轨迹

- 文档目的：解释 80-demos/D01-simple-mcore-training/data-and-state-trace.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/megatron-lm HEAD 3703d4e33（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 80-demos/D01-simple-mcore-training/data-and-state-trace.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


```text
MockGPTDataset
  -> DataLoader batch
  -> tokens/attention_mask/position_ids/labels/loss_mask
  -> CUDA device
  -> GPTModel forward(labels=labels)
  -> per-token losses
  -> loss_func(loss_mask)
  -> backward gradients
  -> finalize_model_grads
  -> Adam parameter update
```

## 状态对象

| 对象 | 所有者 | 说明 |
|---|---|---|
| dataset config | demo | null tokenizer、sequence length 64 |
| GPTModel | demo/DDP | 2 层、hidden 12、4 heads |
| losses | schedule/loss func | mask 后均值 |
| gradients | autograd/DDP | finalize 后给 optimizer |
| checkpoint state | dist_checkpointing | 来自 model.sharded_state_dict |

D01 的 checkpoint 只演示模型 sharded state；不覆盖正式训练中的 optimizer、scheduler、RNG tracker 全量状态。

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
