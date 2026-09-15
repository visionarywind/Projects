# M03 训练运行时

- 文档目的：追踪从 `pretrain` 到单次迭代、优化器更新、日志和退出的控制流。
- 对应源码版本：`3703d4e33a3a2b2d11ebcc8e41f45af7ce7d1eda`
- 证据状态：静态源码分析；未运行 GPU 训练。
- 最后更新：2026-09-15

## 边界

M03 位于 `megatron/training`，负责作业初始化、模型/优化器装配、数据 iterator 连接、训练/验证迭代、动态 microbatch、日志、checkpoint 和退出条件。模型计算属于 M01，schedule 属于 M02，参数更新实现属于 M05。

## 主控制流

```text
pretrain
  -> initialize_megatron
  -> setup_model_and_optimizer
  -> build_train_valid_test_datasets/iterators
  -> train
    -> get_forward_backward_func
    -> train_step
      -> forward_backward_func
      -> optimizer.step
      -> scheduler.step
    -> training_log / save_checkpoint / validation
```

## 关键状态

- `args`/config：启动参数和 Transformer 配置
- model list：PP/VP chunk 集合
- optimizer/scheduler：参数更新和学习率状态
- data iterators：train/valid/test
- iteration/sample tokens：进度计数
- rerun/fault/profiling/CUDA graph 状态：控制特殊执行路径

## 证据

- `pretrain`：[megatron/training/training.py:1530-约 1630]
- `get_model`：[training.py:2435-约 2630]
- optimizer setup：[training.py:2632-约 2860]
- `train_step`：[training.py:3092-约 3390]
- `train`：[training.py:4309-约 5620]
- dataset iterator：[training.py:5627-约 5790]

## 当前限制

异步 checkpoint finalize、fault injection、FSDP 和 RL 扩展仅有静态入口证据；CUDA Graph 的 shared pool/static buffer/reset、optimizer replay、NCCL MemPool 注册和实际互斥条件仍未通过运行验证。详见 [CUDA Graph 与显存池生命周期](../../90-cross-module/cuda-graph-resource-lifecycle.md)。
