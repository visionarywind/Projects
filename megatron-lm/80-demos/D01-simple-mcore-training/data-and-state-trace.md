# D01 数据与状态轨迹

```text
MockGPTDataset
  -> DataLoader batch
  -> tokens/attention_mask/position_ids/labels/loss_mask
  -> CUDA device
  -> GPTModel forward(labels=labels)
  -> per-token losses
  -> loss_func(loss_mask)
  -> backward gradients
  -> finalize_model_grads
  -> Adam parameter update
```

## 状态对象

| 对象 | 所有者 | 说明 |
|---|---|---|
| dataset config | demo | null tokenizer、sequence length 64 |
| GPTModel | demo/DDP | 2 层、hidden 12、4 heads |
| losses | schedule/loss func | mask 后均值 |
| gradients | autograd/DDP | finalize 后给 optimizer |
| checkpoint state | dist_checkpointing | 来自 model.sharded_state_dict |

D01 的 checkpoint 只演示模型 sharded state；不覆盖正式训练中的 optimizer、scheduler、RNG tracker 全量状态。