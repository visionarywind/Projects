# 跨模块调用链

## 训练主链

```text
pretrain_gpt.forward_step
  -> get_batch
  -> model(tokens, position_ids, attention_mask, labels)
  -> GPTModel.forward
  -> TransformerBlock.forward
  -> loss_func
```

## schedule 主链

```text
train_step
  -> forward_backward_func
  -> forward_step
  -> backward_step
  -> finalize_model_grads
  -> optimizer.step
```

## checkpoint 主链

```text
train loop
  -> save_checkpoint
  -> generate_state_dict
  -> model.sharded_state_dict
  -> dist_checkpointing.save
```

## inference 主链

```text
server/offline example
  -> get_inference_engine
  -> TextGenerationController
  -> DynamicInferenceEngine.generate
  -> model wrapper forward
  -> sampling
```

这些链路以 Python 静态入口为准；Transformer Engine kernel、NCCL collective 和 torch autograd engine 的内部调用不在本仓库源码中完整展开。