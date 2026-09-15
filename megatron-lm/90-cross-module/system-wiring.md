# 跨模块系统串联

- 对应源码版本：`3703d4e33a3a2b2d11ebcc8e41f45af7ce7d1eda`
- 证据状态：静态源码分析。

```text
entry/pretrain_gpt
  -> M03 initialize/pretrain/train
  -> M02 distributed/process groups/schedule
  -> M04 dataset/get_batch
  -> M01 model forward
  -> M05 optimizer/checkpoint
```

## 责任矩阵

| 责任 | 所属模块 | 说明 |
|---|---|---|
| 参数与作业编排 | M03 | 控制生命周期和训练循环 |
| 并行拓扑 | M02 | 提供 TP/PP/DP/CP/EP group |
| microbatch 调度 | M02/M03 | schedule 被 train_step 调用 |
| 模型计算 | M01 | embedding、decoder、output/loss |
| batch 生成 | M04 | dataset/sampler/get_batch |
| 参数更新 | M05 | optimizer 和 scheduler |
| 状态保存 | M05/M03 | checkpoint 命名、state dict、tracker |
| 推理服务 | M06 | 复用 M01/M02 并独立管理 request/KV |

跨模块边界的核心原则：M03 调度调用时机，M02 管通信和 microbatch，M01 只做 tensor 计算，M05 管训练状态持久化。