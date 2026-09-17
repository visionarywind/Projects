# M19 DeepSeek 系列模型与并行策略

- 文档目的：基于当前 SGLang checkout，解释 DeepSeek V2/V3/V3.1/R1/V3.2/DSA 及 MTP 相关模型代码，并把 TP、DP Attention、EP、CP、PP 等并行策略串成可追踪的执行模型。
- 适用范围：`source/sglang/python/sglang/srt/models/deepseek*.py`、分布式状态、`server_args.py`、DP Attention、MoE dispatcher 及 DeepSeek 使用文档。
- 对应源码版本：`source/sglang` HEAD `78be4b50af`（2026-09-15）。
- 证据状态：静态源码与仓库文档已确认；真实多 GPU、通信、模型权重和性能结果未在本工作区验证。
- 术语说明：当前 checkout 中未搜索到名为 `CPTD` 的配置、类或函数。本文不臆造该缩写；将用户所说的“cptd 等并行策略”按 DeepSeek 部署中实际存在的 CP/TP/DP/EP/PP 组合分析。如果 `CPTD` 指某个外部项目或论文术语，应补充其全称后再建立一一对应关系。

## 结论摘要

1. DeepSeek V2/V3/V3.2 在 SGLang 中主要复用 `deepseek_v2.py` 的模型骨架：MLA/DSA Attention、稀疏 MoE、共享专家、PP layer slicing 和统一权重加载；V3/V3.2 通过不同 architecture/config 分支进入同一套实现。[`source/sglang/python/sglang/srt/models/deepseek_v2.py:2027-2107`][`source/sglang/python/sglang/srt/models/deepseek_v2.py:2529-2555`]
2. TP 不是“所有子模块都按同一组切分”。开启 DP Attention 后，attention 使用由 `tp_size / dp_size / attn_cp_size` 推导的 attention-TP/DP/CP 坐标，而 MoE 仍使用独立的 MoE DP/EP/TP 拓扑。[`source/sglang/python/sglang/srt/layers/dp_attention.py:240-316`][`source/sglang/python/sglang/srt/distributed/parallel_state.py:1910-2024`]
3. DP Attention 的核心收益是让不同 DP rank 持有不同请求 token/KV 工作集；其代价是 attention 前后的 gather/scatter、padding 和 MoE 前后的 token 协调。`server_args` 会在启用后缩小 chunked prefill，并放宽调度保守度。[`source/sglang/python/sglang/srt/server_args.py:3012-3028`]
4. EP 主要改变 MoE expert 的位置和 token dispatch，不等价于 attention DP。DeepEP/Mooncake/NIXL/FlashInfer 等 A2A 路径当前会把 `ep_size` 调整为 `tp_size`；非 A2A 的 hybrid EP/TP 仍由独立 group 和 runner 处理。[`source/sglang/python/sglang/srt/server_args.py:3138-3201`][`source/sglang/python/sglang/srt/models/deepseek_v2.py:386-628`]
5. DSA（DeepSeek V3.2）在模型代码中表现为 NSA/Indexer、稀疏 attention backend 和可选 prefill CP，而不是一个新的模型类；`server_args` 会自动选择 `nsa` backend、设置 page size/KV dtype/backend，并对 CP 组合施加限制。[`source/sglang/python/sglang/srt/server_args.py:1764-1853`][`source/sglang/python/sglang/srt/models/deepseek_v2.py:1312-1411`]

## 推荐阅读

1. [模型家族与 forward](model-family.md)
2. [并行策略与拓扑](parallel-strategies.md)
3. [源码证据与调试地图](source-map.md)
4. [M07 分布式并行](../M07-分布式并行.md)
5. [M14 MoE、量化与 LoRA](../M14-moe-quantization-lora/README.md)
6. [DeepSeek V3/V3.1/R1 使用文档](../../../source/sglang/docs/basic_usage/deepseek_v3.md)
7. [DeepSeek V3.2/GLM-5 使用文档](../../../source/sglang/docs/basic_usage/deepseek_v32.md)

## 一条 DeepSeek 请求的实现主线

```text
ServerArgs.resolve
  -> model_arch / HF config / quantization / backend defaults
  -> WORLD + TP/attention-TP/DP/CP + MoE DP/EP/TP + PP groups
  -> DeepseekV2ForCausalLM
       -> DeepseekV2Model (PP 层范围)
       -> DeepseekV2DecoderLayer
            -> DeepseekV2AttentionMLA (MLA 或 DSA/NSA)
            -> DeepseekV2MoE (router -> dispatch -> expert GEMM -> combine)
  -> logits / sampling
```

### 并行策略速查

| 策略 | 切分对象 | 主要 group | DeepSeek 中的消费点 | 典型目标 |
|---|---|---|---|---|
| TP | attention/线性层及部分 MoE tensor 维度 | `_TP`、`_ATTN_TP`、`_MOE_TP` | `ColumnParallelLinear`、`RowParallelLinear`、all-reduce | 低延迟、单节点或跨节点模型驻留 |
| DP Attention | 请求/token/KV 工作集，attention 副本化 | attention DP + attention TP | `dp_attention.py` gather/scatter、MLA local heads | 高并发、KV 容量和吞吐 |
| EP | routed experts 及 token 路由 | `_MOE_EP`、A2A dispatcher | `FusedMoE`、DeepEP 等 | 降低每卡 expert 权重，扩大 MoE |
| MoE DP | expert 副本之间的 token/负载协调 | `_MOE_DP` | `get_moe_dp_group`、dispatch/combine | 与 EP/CP 组合扩大吞吐 |
| CP | 长序列 attention/KV 或 DSA prefill token | `_ATTN_CP` | `cp_split_and_rebuild_*`、KV all-gather | 长上下文 prefill |
| PP | Transformer layer range | `_PP` | `make_layers`、`PPMissingLayer`、proxy tensors | 跨节点驻留、长 prefill |

## 当前边界

- 本文解释“代码如何实现”和“参数如何投影”，不把官方 benchmark 的数字当作本地验证结果。
- DeepSeek V3/V3.1/R1 的 MLA、V3.2 的 DSA/NSA、MTP/NextN 和量化 backend 是相互叠加的维度；不能把某个 attention backend 或 MoE backend 直接当作一种并行策略。
- `--tp` 在 SGLang 的多个语境里是 WORLD 中的模型并行宽度；启用 DP/CP 后，真正的 attention-TP 宽度由派生公式决定。

## 相关文档
- [项目入口](../../README.md)
- [分析状态](../../00-overview/analysis-state.md)
- [源码证据索引](../../00-overview/evidence-index.md)
- [M20 通用并行策略源码分析](../M20-parallel-strategies/README.md)

## 源码证据摘要
本文引用以 `source/sglang` 当前 checkout 为准；行号随上游变更需要复核。

## 未解决问题
未在本环境启动 DeepSeek checkpoint，也未验证 NCCL/RCCL/HCCL、DeepEP、FlashInfer、CUDA Graph、跨节点 CP/EP 或实际 KV 节省比例。

## 下一步阅读建议
先读模型家族，再读并行拓扑；排障时按 `server_args` 的最终 resolved 配置、group rank、model constructor、attention/MoE dispatcher 顺序设置断点。
