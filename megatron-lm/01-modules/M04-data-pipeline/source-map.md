# M04 源码地图

| 文件 | 重点 |
|---|---|
| `megatron/core/datasets/gpt_dataset.py:25+` | `GPTDatasetConfig` |
| `gpt_dataset.py:140+` | `GPTDataset` |
| `gpt_dataset.py:264+` | sample 到 token/label/mask |
| `gpt_dataset.py:734-877` | index、mask 和 mock low-level dataset |
| `gpt_dataset.py:933+` | `MockGPTDataset` |
| `core/datasets/blended_megatron_dataset_builder.py` | blended split 构造 |
| `training/datasets/data_samplers.py` | DP/random/hybrid sampler |
| `pretrain_gpt.py:111+` | device batch 整理 |
| `tests/unit_tests/data/` | 数据回归测试 |

本分析没有运行数据 builder；命令和性能结论均不能视为已验证。