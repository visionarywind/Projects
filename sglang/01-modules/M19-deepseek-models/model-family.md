# DeepSeek 模型家族与 Forward 实现

- 文档目的：解释 DeepSeek 架构在 SGLang 中如何从配置映射到模型类、MLA/DSA attention、MoE、PP 和输出。
- 对应源码版本：`source/sglang` HEAD `78be4b50af`。
- 证据状态：静态源码已确认；实际权重加载和 GPU forward 未验证。

## 1. 架构映射：V2/V3/V3.2 共用骨架

`deepseek_v2.py` 定义 `DeepseekV2ForCausalLM`，并把 `DeepseekV3ForCausalLM`、`DeepseekV32ForCausalLM` 声明为其派生类。模型 registry 根据 HF architecture 选择 entry class；因此“V3 使用 V2 文件”不是版本混淆，而是实现复用。[`source/sglang/python/sglang/srt/models/deepseek_v2.py:2299-2366`][`source/sglang/python/sglang/srt/models/deepseek_v2.py:2529-2555`]

实现层次为：

```text
DeepseekV{2,3,32}ForCausalLM
  -> DeepseekV2Model
      -> DeepseekV2DecoderLayer * local PP layer range
          -> DeepseekV2AttentionMLA
          -> DeepseekV2MoE 或 DeepseekV2MLP
      -> RMSNorm / ParallelLMHead (仅 PP last rank)
```

V4 使用独立 `deepseek_v4.py`，但仍复用 `DeepseekV2MoE` 和部分 MLA/并行抽象；V4 NextN 则在 `deepseek_v4_nextn.py` 中复用 decoder/model 组件。本文主体以 V2/V3/V3.2 为主，不把 V4 的专用 sparse memory pool 混入普通 V3 结论。

## 2. DeepseekV2Model：PP 层范围和 CP token 重排

构造阶段先获取 PP coordinator。只有 first PP rank 创建 `VocabParallelEmbedding`，其他 rank 用 `PPMissingLayer`；`make_layers` 依据 `pp_rank`/`pp_size` 只创建本 stage 的 layer range；last rank 才创建 RMSNorm。[`source/sglang/python/sglang/srt/models/deepseek_v2.py:2027-2107`]

forward 阶段：

1. first rank 从 embedding/input embeds 得到 hidden states，非 first rank 从 `PPProxyTensors` 接收上游 activation；
2. 如果启用 NSA prefill CP，first rank 对 hidden states、所有 rank 对 positions 做 CP split/rebuild；
3. 顺序执行本 PP stage 的 decoder layers；
4. 非 last rank 返回 hidden/residual proxy；last rank 做 norm；
5. CP prefill 模式下，last rank `cp_all_gather_rerange_output` 恢复全局 token 顺序。[`source/sglang/python/sglang/srt/models/deepseek_v2.py:2160-2296`]

这说明 PP 和 CP 的作用点不同：PP 切 layer，CP 重排同一批序列的 token/KV；二者不能用一个 rank 公式替代。

## 3. MLA：权重吸收与 attention-TP

`DeepseekV2AttentionMLA.__init__` 从 `get_attention_tp_rank/size()` 获取 attention 专用坐标，并把 `num_heads` 除以 attention-TP size 得到 `num_local_heads`。Q/KV 的投影使用 `ColumnParallelLinear`，输出投影使用 `RowParallelLinear`；输入的 latent/Q/KV 部分可 replicated，head 维度则按 attention-TP 切分。[`source/sglang/python/sglang/srt/models/deepseek_v2.py:1273-1433`]

关键不变量：

- `num_heads % attn_tp_size == 0`；
- 开启 DP Attention 后，`attn_tp_size` 不必等于全局 `tp_size`；
- `o_proj` 是否 reduce results 由 layer communicator/forward mode 决定，不能简单假设每层都立即 all-reduce；
- `attn_mqa` 使用 latent KV cache，`attn_mha` 是普通 MHA/短 prefill 等 fallback 的另一条路径。[`source/sglang/python/sglang/srt/models/deepseek_v2.py:1423-1473`]

attention forward 先按 batch mode 和配置选择 backend，再在 `MHA`、`MHA_CHUNKED_KV`、`MHA_ONE_SHOT`、`MLA`、ROCm/CPU fused MLA、NPU DSA 等方法之间分派。[`source/sglang/python/sglang/srt/models/deepseek_v2.py:1512-1532`][`source/sglang/python/sglang/srt/models/deepseek_v2.py:1573-1697`]

## 4. V3/V3.2 的 MoE

`DeepseekV2MoE` 构造时同时保存全局 TP size 和 MoE EP size。它根据 A2A backend 决定 shared expert 是否融合进本地 MoE kernel；DeepEP fusion 会把 routed experts 扩展为 `routed + EP_size` slots，并将 top-k 加一个 shared slot。[`source/sglang/python/sglang/srt/models/deepseek_v2.py:386-428`]

普通 grouped top-k 覆盖 V3/V3.2/GLM-5 等配置；随后 `get_moe_impl_class` 创建量化/设备相关的 MoE runner。共享 expert 未融合时可以用 `tp_size=1` 的 replicated MLP，以避免在 A2A 模式下重复进行不匹配的 TP 切分。[`source/sglang/python/sglang/srt/models/deepseek_v2.py:450-565`]

forward 有三条主路径：

```text
should_use_mega_moe -> mega MoE
_enable_a2a_moe = false -> forward_normal / dual_stream
_enable_a2a_moe = true  -> forward_deepep
```

`forward_normal` 的核心顺序是 shared expert（若未融合）→ router gate → top-k → `self.experts`（dispatch/runner/combine）→ shared output 合并 → TP all-reduce；如果启用了 reduce-scatter 或 fusion，post-expert all-reduce 可被跳过。[`source/sglang/python/sglang/srt/models/deepseek_v2.py:644-697`][`source/sglang/python/sglang/srt/models/deepseek_v2.py:756-858`]

因此 EP 的真正边界在 `self.experts` 的 dispatcher/runner，而不是 `DeepseekV2MoE` 类名本身；修改 A2A backend 时要同时检查 top-k output format、shared expert ownership、combine 后的 scaling 和 all-reduce 条件。

## 5. DSA/V3.2：NSA Indexer 与 sparse attention

`server_args._handle_model_specific_adjustments` 通过 `is_deepseek_nsa(hf_config)` 识别 DSA。未手工指定时选择 `nsa` attention backend，并以模型 `index_topk` 设置 dense-attention KV threshold；GPU 上进一步设置 page size、KV cache dtype 和 NSA prefill/decode backend。[`source/sglang/python/sglang/srt/server_args.py:1764-1848`]

MLA 初始化中 `self.use_nsa` 控制是否创建 `Indexer`。Indexer 保存 `index_topk`、rope/index head 参数；`skip_topk`/`next_skip_topk` 允许按频率或 pattern 复用上一层 indexer top-k，从而减少索引计算。[`source/sglang/python/sglang/srt/models/deepseek_v2.py:1312-1411`]

在 prefill CP 模式，`rebuild_cp_kv_cache` 将 latent KV 和 rope KV 写回 cache，再执行 CP all-gather/reorder；这解释了为什么 DSA CP 不只是“把序列均分”，还必须恢复 attention 所需的全局 KV 顺序。[`source/sglang/python/sglang/srt/models/deepseek_v2.py:1716-1728`]

## 6. MTP/NextN 与并行边界

`deepseek_nextn.py` 定义 `DeepseekModelNextN`/`DeepseekV3ForCausalLMNextN`，复用 `DeepseekV2DecoderLayer` 和 target 模型结构，并显式读取 DP Attention 与 CP helper。MTP 是 speculative decoding 的 draft/verify 机制，不是新的 TP/DP 拓扑；draft worker 可能用临时 patched TP/DP 状态运行。[`source/sglang/python/sglang/srt/models/deepseek_nextn.py:70-234`][`source/sglang/python/sglang/srt/distributed/parallel_state.py:2137-2159`]

## 7. 配置到执行的完整链路

```text
HF architecture/config
  -> ServerArgs._handle_model_specific_adjustments
      -> attention_backend / quantization / MoE backend / CP constraints
  -> initialize_dp_attention + initialize_model_parallel
      -> attention TP/DP/CP + MoE DP/EP/TP + PP groups
  -> DeepseekV3ForCausalLM constructor
      -> local PP layers + local attention heads + expert runner
  -> forward batch
      -> optional CP split
      -> MLA/DSA backend
      -> MoE dispatch/experts/combine
      -> TP/DP result synchronization
      -> PP proxy or logits
```

## 相关文档
- [M19 入口](README.md)
- [并行策略与拓扑](parallel-strategies.md)
- [源码证据与调试地图](source-map.md)

## 源码证据摘要
关键证据集中在 `deepseek_v2.py` 的模型、MLA、MoE、forward 区间，以及 `server_args.py` 的模型特化分支。

## 未解决问题
具体 DeepSeek V4、NSA sparse kernel 的所有平台分支、MTP 实际 acceptance rate 与多卡性能尚未在本地运行验证。

## 下一步阅读建议
先用 `server_args` 打印最终配置，再在 `DeepseekV2Model.__init__` 检查 PP layer range，在 `DeepseekV2AttentionMLA.__init__` 检查 attention-TP，在 `DeepseekV2MoE.forward` 检查 A2A/normal 分支。
