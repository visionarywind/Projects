# D01 执行轨迹

| 步骤 | 代码 | 状态变化 |
|---|---|---|
| 1 | `initialize_distributed` | 设置 CUDA device、init process group、初始化 model parallel |
| 2 | `model_parallel_cuda_manual_seed` | 建立并行 RNG seed |
| 3 | `model_provider` | 创建 `TransformerConfig` 与 `GPTModel` |
| 4 | `DistributedDataParallel` | 包装模型以支持 grad sync |
| 5 | `get_train_data_iterator` | 编译 helper、构造 MockGPTDataset 和 DataLoader |
| 6 | `get_forward_backward_func` | 根据 PP=1 选择无 pipeline schedule |
| 7 | iteration loop | zero grad、forward/backward、finalize、Adam step |
| 8 | checkpoint save/load | 通过 sharded_state_dict 和 dist_checkpointing round-trip |

D01 的 schedule 参数固定为 `num_microbatches=1`、`seq_length=64`、`micro_batch_size=8`，因此不会展示复杂 pipeline warmup/1F1B。