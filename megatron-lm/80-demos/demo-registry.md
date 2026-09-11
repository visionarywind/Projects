# D01 最小 Core GPT 训练

- 文档目的：用仓库自带最小示例串联 M01-M05。
- 对应源码版本：`8190837c2b6ce176a431bc2a6ffd3439507648a7`
- 证据状态：静态源码分析；命令未运行。

## 入口

`examples/run_simple_mcore_train_loop.py` 创建 2 层小 GPT、MockGPTDataset、Core DDP、Adam、forward/backward schedule，并保存/加载 distributed checkpoint。[`examples/run_simple_mcore_train_loop.py:28-283`]

## 运行命令（未验证）

```bash
torchrun --nproc_per_node=2 examples/run_simple_mcore_train_loop.py
```

需要 CUDA、NCCL、两张 GPU 和上游依赖环境。

## 阶段

| 阶段 | 源码 | 模块 |
|---|---|---|
| distributed init | `initialize_distributed` | M02 |
| model provider | `model_provider` | M01 |
| dataset iterator | `get_train_data_iterator` | M04 |
| forward step | `forward_step_func` | M01/M03 |
| schedule | `get_forward_backward_func` | M02/M03 |
| grad finalize/update | `finalize_model_grads` + Adam | M05 |
| checkpoint | `dist_checkpointing.save/load` | M05 |

## 价值

D01 不覆盖正式 `pretrain` 参数系统和完整 checkpoint tracker，但它最小化外部依赖，适合作为理解 Core API 的第一条可读链。