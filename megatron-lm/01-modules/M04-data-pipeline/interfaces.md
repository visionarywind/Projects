# M04 接口与测试边界

| 接口 | 输入 | 输出 |
|---|---|---|
| `GPTDatasetConfig` | tokenizer、sequence length、mask/packing flags | 不可变/配置化 dataset 行为 |
| `GPTDataset.__getitem__` | sample index | tokens、labels、loss mask、position ids 和可选 metadata |
| `BlendedMegatronDatasetBuilder.build` | dataset class、split sizes、config | train/valid/test datasets |
| `build_pretraining_data_loader` | dataset、batch/DP 参数 | rank-local DataLoader |
| `get_batch` | data iterator | device-side batch 字典 |

源码测试重点在 `tests/unit_tests/data/` 和 dataset builder/sampler 测试。真实 mmap/index 文件、tokenizer 和 packed attention 需在具备数据依赖的环境单独验证。