# 配置影响图

| 配置 | 影响模块 | 主要影响 |
|---|---|---|
| `tensor_model_parallel_size` | M02/M01/M05 | group、参数分片、logits 输出 |
| `pipeline_model_parallel_size` | M02/M03/M01 | stage flags、schedule、model list |
| `num_layers_per_virtual_pipeline_stage` | M02/M03 | interleaved schedule 和 chunk |
| `context_parallel_size` | M02/M04/M05 | sequence split、batch 处理、grad reduce |
| `num_experts`/EP | M01/M02/M05 | MoE layer spec、expert groups、optimizer shards |
| `transformer_impl` | M01 | local/TE/inference spec |
| `sequence_parallel` | M01/M02/M05 | RNG、layernorm grads、参数通信 |
| `use_distributed_optimizer` | M05/M02 | optimizer shard 和 DP group |
| `fp16/bf16/fp8/fp4` | M01/M03/M05 | autocast/quant context、optimizer wrapper |
| inference max requests/sequence | M06 | context capacity 和 scheduler admission |

修改配置默认值时，应同步检查 builder、parallel_state、schedule、checkpoint args 校验和对应测试。