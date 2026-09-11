# M05 优化器与分布式检查点

- 对应源码版本：`8190837c2b6ce176a431bc2a6ffd3439507648a7`
- 证据状态：静态源码分析；未运行保存/恢复实验。

## 主链

```text
model parameters
  -> _get_param_groups / buffers
  -> get_megatron_optimizer
    -> FP32 / mixed precision / distributed / chained optimizer
  -> train_step optimizer.step
    -> update_successful, grad_norm, zeros
  -> scheduler.step

model.sharded_state_dict + optimizer state
  -> save_checkpoint/generate_state_dict
  -> dist_checkpointing.save
  -> load_checkpoint / dist_checkpointing.load
```

## optimizer factory

`get_megatron_optimizer` 根据参数组、混合精度、distributed optimizer、FSDP、CPU offload、MuP、TE/Apex/PyTorch fallback 等配置选择实现。`ChainedOptimizer` 可组合多个 optimizer。关键实现位于 `core/optimizer/__init__.py` 和 `optimizer.py`。[`core/optimizer/__init__.py:13-约 1100`]

## 更新不变量

`train_step` 只有在 `optimizer.step()` 报告 update successful 时推进 scheduler；梯度 norm 和 zero count 用于日志/诊断。distributed optimizer 的 shard 所有权不能与普通 model state 混淆。

## checkpoint

传统 checkpoint 按 iteration、TP/PP/EP rank 和 distributed optimizer 形成路径；distributed checkpoint 使用 sharded state dict 与 metadata。save/load 同时涉及模型、optimizer、scheduler、RNG、tokenizer、数据加载状态和 tracker。[`training/checkpointing.py:176-230,611+,1567+,2469+`]

## 风险

checkpoint 兼容性取决于模型结构和并行配置；`check_checkpoint_args` 会拒绝/警告关键结构参数不匹配。异步保存和 non-persistent/local checkpoint 的清理及恢复语义尚未实测。