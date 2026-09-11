# M05 执行流程与接口

## 单步

```text
zero_grad
  -> schedule backward/finalize grads
  -> optimizer.step
  -> aggregate result across relevant groups
  -> if successful: scheduler.step
```

## 保存

```text
train loop checkpoint condition
  -> save_checkpoint
  -> generate model/optimizer/training state
  -> dist checkpoint writer
  -> tracker/metadata finalize
```

## 恢复

```text
load_checkpoint
  -> resolve iteration/path
  -> check_checkpoint_args
  -> load model + optimizer + scheduler/RNG/data state
  -> return iteration and consumed samples
```

| 接口 | 约束 |
|---|---|
| `get_megatron_optimizer` | model 参数属性和 precision/config 一致 |
| `optimizer.step` | 调用方处理 skip/update result |
| `model.sharded_state_dict` | consumer 理解 shard metadata |
| `save_checkpoint/load_checkpoint` | 并行拓扑和结构参数兼容 |

当前没有运行 checkpoint round-trip。