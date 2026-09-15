# D01 最小 Core GPT 训练

- 文档目的：记录 `examples/run_simple_mcore_train_loop.py` 的端到端静态轨迹。
- 对应源码版本：`3703d4e33a3a2b2d11ebcc8e41f45af7ce7d1eda`
- 状态：已完成静态阅读，未运行。

## 一句话

D01 是一个绕过完整 training 参数系统的 Core API 示例：直接初始化分布式、构造小 GPT、用 MockGPTDataset 训练 5 步，并保存/恢复 distributed checkpoint。

## 控制流

```text
initialize_distributed(TP=2, PP=1)
  -> model_parallel_cuda_manual_seed
  -> model_provider
  -> DDP wrap
  -> Adam
  -> get_train_data_iterator
  -> get_forward_backward_func
  -> 5 iterations
    -> zero_grad
    -> schedule forward/backward
    -> finalize_model_grads
    -> Adam.step
  -> save_distributed_checkpoint
  -> load_distributed_checkpoint
```

## 重点源码

- 初始化：[examples/run_simple_mcore_train_loop.py:28-53]
- 模型：[examples/run_simple_mcore_train_loop.py:56-78]
- 数据：[examples/run_simple_mcore_train_loop.py:81-120]
- forward/loss：[examples/run_simple_mcore_train_loop.py:123-165]
- checkpoint：[examples/run_simple_mcore_train_loop.py:168-217]
- 主循环：[examples/run_simple_mcore_train_loop.py:220-283]

## 未验证

尚未确认本机 GPU、NCCL、依赖和 helper 编译状态，因此不记录 loss 数值或 checkpoint 文件结果。