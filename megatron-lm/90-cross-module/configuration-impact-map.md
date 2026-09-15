# 配置影响图

- 文档目的：解释 90-cross-module/configuration-impact-map.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/megatron-lm HEAD 3703d4e33（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 90-cross-module/configuration-impact-map.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


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

## 相关文档
- [项目入口](../README.md)
- [分析状态](../00-overview/analysis-state.md)
- [源码证据索引](../00-overview/evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](../00-overview/analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
