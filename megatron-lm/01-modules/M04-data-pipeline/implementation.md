# M04 实现与数据生命周期

- 文档目的：解释 01-modules/M04-data-pipeline/implementation.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/megatron-lm HEAD 3703d4e33（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M04-data-pipeline/implementation.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


## Dataset index

`GPTDataset` 通过 document index、sample index 和 shuffle index 将文档级存储映射为训练 sample；builder 可按 train/valid/test split 和 blended dataset 配置分配样本。[`gpt_dataset.py:734-800`; `blended_megatron_dataset_builder.py:30+`]

## masks/positions

`_get_ltor_masks_and_position_ids` 按配置创建 causal attention mask、loss mask 和 position ids。跨文档 masking 时额外生成每个 packed segment 的起止位置和最大长度，供 varlen attention 使用。[`gpt_dataset.py:800+`]

## sampler

普通 sampler、random sampler、hybrid CP sampler 和 full validation sampler 将 dataset sample 变为 rank-local batch；`build_pretraining_data_loader` 负责选择 loader。随机数据集包装器用于 epoch/seed 可复现。[`training/datasets/data_samplers.py:19-380`]

## get_batch

`pretrain_gpt.get_batch` 从 iterator 取字典，按 TP source rank 进行 CUDA 搬运和广播，再根据 packed sequence、CP 和 MTP 分支整理为固定 `BATCH_KEYS`。它是 M04 到 M01/M03 的主要边界。

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
