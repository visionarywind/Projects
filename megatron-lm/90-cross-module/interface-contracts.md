# 跨模块接口契约

| 契约 | 生产方 | 消费方 | 关键约束 |
|---|---|---|---|
| `ProcessGroupCollection`/parallel_state | M02 | M01/M03/M05/M06 | 初始化后才能使用；global fallback 与显式 PG 并存 |
| `forward_step_func` | 入口/M03 | M02 schedule | 返回 output tensor 与 loss function |
| batch dict | M04 | M03/M01 | keys、device、shape 与模型配置一致 |
| `GPTModel.forward` | M01 | M02/M03/M06 | stage flags、labels、inference context 匹配 |
| optimizer step result | M05 | M03 | update successful 才推进 scheduler |
| sharded state dict | M01/M05 | checkpoint writer | shard metadata 必须被 consumer 理解 |
| inference request | M06 | M06 engine/controller | sampling params、KV capacity、streaming 状态一致 |

接口风险主要来自隐式全局状态：args、parallel_state、random seed 和 distributed group 的生命周期应在文档/代码中显式标注。