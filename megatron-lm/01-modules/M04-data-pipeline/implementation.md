# M04 实现与数据生命周期

## Dataset index

`GPTDataset` 通过 document index、sample index 和 shuffle index 将文档级存储映射为训练 sample；builder 可按 train/valid/test split 和 blended dataset 配置分配样本。[`gpt_dataset.py:734-800`; `blended_megatron_dataset_builder.py:30+`]

## masks/positions

`_get_ltor_masks_and_position_ids` 按配置创建 causal attention mask、loss mask 和 position ids。跨文档 masking 时额外生成每个 packed segment 的起止位置和最大长度，供 varlen attention 使用。[`gpt_dataset.py:800+`]

## sampler

普通 sampler、random sampler、hybrid CP sampler 和 full validation sampler 将 dataset sample 变为 rank-local batch；`build_pretraining_data_loader` 负责选择 loader。随机数据集包装器用于 epoch/seed 可复现。[`training/datasets/data_samplers.py:19-380`]

## get_batch

`pretrain_gpt.get_batch` 从 iterator 取字典，按 TP source rank 进行 CUDA 搬运和广播，再根据 packed sequence、CP 和 MTP 分支整理为固定 `BATCH_KEYS`。它是 M04 到 M01/M03 的主要边界。