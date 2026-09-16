# DeepSeek 并行策略：TP、DP Attention、EP、CP、PP

- 文档目的：用 rank 拓扑、张量布局和执行时机解释 DeepSeek 的并行策略，不把所有 `--*p` 参数混成同一种“切卡”。
- 对应源码版本：`source/sglang` HEAD `78be4b50af`。
- 证据状态：拓扑和配置约束为静态确认；collective 的实际通信与性能未验证。

## 1. 先澄清三个层次

```text
WORLD/进程布局：tp_size、pp_size、global rank
  -> GroupCoordinator：TP/PP/attention CP/attention TP/MoE DP/EP/TP
    -> 模型层：哪些 weight/hidden/token/KV 在哪个 group 上切分
      -> collective/backend：all-reduce、all-gather、reduce-scatter、A2A
```

`tp_size` 是模型并行 WORLD 的宽度；开启 DP Attention 后，attention 的实际 TP 宽度由 `tp_size // dp_size // attn_cp_size` 推导，不能把 `--tp 8 --dp 8` 解释成“attention 仍然 TP8”。[ `source/sglang/python/sglang/srt/layers/dp_attention.py:240-255` ]

## 2. TP：默认路径和 DP Attention 下的变化

### 2.1 默认 TP

纯 TP 时，`DeepseekV2AttentionMLA` 使用全 TP group：

- Q/QKV/KV-B 等 column-parallel 权重按 TP rank 切分；
- O projection 使用 row-parallel 权重；
- MoE 的非 A2A 路径在 expert GEMM 后按条件 TP all-reduce；
- 每个 PP stage 内的 rank 共同完成该 stage 的 layer。[`source/sglang/python/sglang/srt/models/deepseek_v2.py:1333-1432`][`source/sglang/python/sglang/srt/models/deepseek_v2.py:748-754`]

纯 TP 更适合低延迟、小 batch；DeepSeek V3 文档将 `--tp 8` 作为多卡单副本的基本部署方式，但文档命令本身不是本地成功证据。[`source/sglang/docs/basic_usage/deepseek_v3.md:138-152`]

### 2.2 DP Attention 的坐标公式

当 `enable_dp_attention=True`：

```python
attn_tp_size  = tp_size // dp_size // attn_cp_size
attn_tp_rank  = tp_rank % attn_tp_size
attn_dp_rank  = tp_rank // (attn_tp_size * attn_cp_size)
```

代码注释规定 rank 逻辑布局为 `(dp, cp, tp)`，TP 是最快变化维度。[`source/sglang/python/sglang/srt/layers/dp_attention.py:240-255`]

例如 `tp_size=8, dp_size=2, attn_cp_size=1`：

```text
全局 tp_rank:       0 1 2 3 | 4 5 6 7
attention-dp rank:  0 0 0 0 | 1 1 1 1
attention-tp rank:  0 1 2 3 | 0 1 2 3
```

每个 DP replica 只需维护自己负责的请求/KV 工作集；attention weight 按 4-way attention TP，而不是按 8-way 全 TP。MoE 可以仍按另一套 EP/DP/TP group 运行。

### 2.3 DP gather/scatter 的代价

`initialize_dp_attention` 保存 global/local DP rank 和 size，并为 DP buffer 记录 hidden size、dtype、device。[`source/sglang/python/sglang/srt/layers/dp_attention.py:275-316`]

batch 执行时需要根据每个 DP rank 的 token 数计算 local start/length；非 graph 模式按前序 token 数求偏移，graph 模式使用固定 batch slot。[`source/sglang/python/sglang/srt/layers/dp_attention.py:388-420`]

padding mode 的选择根据 prefill/decode、最大 token 数、总 token 数和 DP size 选择 `MAX_LEN` 或 `SUM_LEN`；这直接影响 gather buffer、通信量和 CUDA Graph/symmetric memory 兼容性。[`source/sglang/python/sglang/srt/layers/dp_attention.py:53-95`]

`server_args` 开启 DP Attention 后会：

- 要求 `tp_size % dp_size == 0`；
- 将 chunked prefill size 除以 `dp_size`；
- 将 schedule conservativeness 乘以 `0.3`；
- 若启用 DP LM head，强制要求 DP Attention。[`source/sglang/python/sglang/srt/server_args.py:3012-3028`]

因此 DP Attention 是调度、buffer、attention、MoE 边界共同参与的模式，不是只替换一个 attention kernel。

## 3. EP：专家权重和 token 路由

### 3.1 group 构造

`initialize_model_parallel` 先计算：

```text
moe_ep_size = expert_model_parallel_size
moe_dp_size = moe_data_model_parallel_size
moe_tp_size = tensor_model_parallel_size // moe_ep_size // moe_dp_size
```

随后分别创建/复用 `_MOE_DP`、`_MOE_EP`、`_MOE_TP`。当 EP 宽度等于 TP 时，`_MOE_EP = _TP`；当 MoE TP 宽度等于 TP 时，`_MOE_TP = _TP`；这意味着 group 名称不保证对象独立，清理和调试必须检查对象复用。[`source/sglang/python/sglang/srt/distributed/parallel_state.py:1940-2024`]

### 3.2 DeepSeek MoE forward

`DeepseekV2MoE` 读取 `get_moe_expert_parallel_world_size()`，根据 A2A backend 决定 `_enable_a2a_moe`。非 A2A 走 local expert runner + optional TP all-reduce；A2A 走 `forward_deepep` 等 dispatcher 路径。[`source/sglang/python/sglang/srt/models/deepseek_v2.py:398-428`][`source/sglang/python/sglang/srt/models/deepseek_v2.py:622-697`]

DeepEP shared-expert fusion 的语义是：shared expert 被放进其 home EP rank 的 local slot，expert 数和 top-k 视图相应扩展；这改变的是物理 dispatch/layout，不是 router 的逻辑专家语义。[`source/sglang/python/sglang/srt/models/deepseek_v2.py:412-428`]

### 3.3 A2A backend 对 ep_size 的影响

当前配置处理会将 DeepEP、Mooncake、NIXL、Ascend fused EP、FlashInfer、MORI 的 `ep_size` 设为 `tp_size`；DeepEP normal mode 还会禁用 CUDA Graph。该限制来自当前实现，不能推广为所有未来 EP backend 的理论要求。[`source/sglang/python/sglang/srt/server_args.py:3138-3201`]

## 4. CP：长上下文和 DSA prefill

### 4.1 普通 context parallel 与 DSA prefill CP

当前 checkout 有 `attn_cp_size`/attention CP group，也有 DeepSeek DSA 专用 `enable_nsa_prefill_context_parallel`。后者在模型特化配置中：

- 设置 `enable_dp_attention=True`、`moe_dense_tp_size=1`；
- in-seq-split 模式强制 DeepEP、`ep_size=tp_size`，并要求 batch size 1（源码注释）；
- round-robin 模式要求 `dp_size==1`；
- 只支持单机 `tp_size<=8`，且 PD decode 不允许该 CP。[`source/sglang/python/sglang/srt/server_args.py:1798-1853`]

这解释了为什么 DSA CP 不是任意 CP×DP×EP 的自由笛卡尔积：模型的 indexer、latent KV 重建、MoE token ownership 和 backend buffer 必须同时满足约束。

### 4.2 forward 中的 CP 数据生命周期

```text
输入 hidden/position
  -> cp_split_and_rebuild_data/position
  -> 每个 CP rank 执行 MLA/NSA
  -> latent KV + rope KV 写入局部 cache
  -> cp_all_gather_rerange_output
  -> 恢复全局 token 顺序
```

证据位于 `DeepseekV2Model.forward` 和 `DeepseekV2AttentionMLA.rebuild_cp_kv_cache`。[`source/sglang/python/sglang/srt/models/deepseek_v2.py:2201-2204`][`source/sglang/python/sglang/srt/models/deepseek_v2.py:2286-2293`][`source/sglang/python/sglang/srt/models/deepseek_v2.py:1716-1728`]

## 5. PP：layer slicing，不是 token DP

`make_layers` 按 `pp_rank` 和 `pp_size` 创建本 stage 层；first rank 才有 embedding，last rank 才有 norm/lm head。中间 stage 用 `PPProxyTensors` 传递 hidden/residual。[`source/sglang/python/sglang/srt/models/deepseek_v2.py:2047-2107`][`source/sglang/python/sglang/srt/models/deepseek_v2.py:2175-2178`][`source/sglang/python/sglang/srt/models/deepseek_v2.py:2272-2278`]

PP 与 DP Attention 的组合受到配置约束和调度实现影响；改变 PP size 必须同时核对 layer partition、PP communication、CUDA Graph/overlap scheduler、KV/CP 以及模型加载的 PP weight filter。

## 6. 推荐组合与适用场景

| 场景 | 推荐起点 | 原因 | 主要风险 |
|---|---|---|---|
| 低延迟、小 batch V3/R1 | 纯 TP：`--tp N` | 没有 DP gather/scatter，attention TP 直观 | KV 在 TP ranks 上重复/容量压力 |
| 高并发、KV 容量受限 | `--tp N --dp N --enable-dp-attention`（需满足整除） | DP rank 分担 attention/KV 工作集 | 通信、padding、调度和 graph shape |
| V3/V3.2 大规模 MoE | TP + EP，A2A backend 视硬件选择 | expert 权重和 token 在 EP rank 间分布 | A2A buffer、ep_size 限制、shared fusion |
| V3.2 长上下文 prefill | DSA/NSA + 合法的 prefill CP | indexer 稀疏 attention + CP 分摊长序列 | 单机/设备/DP/EP 限制，精度与 kernel 依赖 |
| 多节点长 prefill | TP + PP + chunked prefill | PP 边界通信少于超大 TP | pipeline bubble、调度与异步 P2P |

DeepSeek 官方使用文档也明确把 TP+DP Attention、EP+DP Attention 和纯 TP 作为 V3.2 的不同启动模式；这些命令应理解为配置入口，不是本地运行验证。[`source/sglang/docs/basic_usage/deepseek_v32.md:40-63`]

## 7. 关于 “CPTD”

在当前 `source/sglang` 源码和现有 `sglang` 知识库中没有 `cptd` 精确匹配，也没有对应 CLI flag、类或 group。可确认的相近概念是：

- `CP`：context parallel；
- `TP`：tensor parallel；
- `DP`：data parallel / DP Attention；
- `EP`：expert parallel；
- `PP`：pipeline parallel；
- DSA 的 prefill CP 与 MoE token dispatch。

所以本文将 “cptd 等” 标成待澄清，而不是把它错误扩展成一个 SGLang 内置策略。

## 相关文档
- [M19 入口](README.md)
- [模型家族与 Forward](model-family.md)
- [源码证据与调试地图](source-map.md)
- [M07 分布式并行](../M07-分布式并行.md)

## 源码证据摘要
拓扑证据来自 `parallel_state.py`，DP 公式和 buffer 来自 `dp_attention.py`，DeepSeek 消费点来自 `deepseek_v2.py`，参数约束来自 `server_args.py`。

## 未解决问题
没有真实多 GPU 执行，因此通信正确性、KV 副本数、吞吐/延迟曲线、CP 精度影响和跨节点 EP 均未验证。

## 下一步阅读建议
先画出 `tp_size/dp_size/attn_cp_size/ep_size/moe_dp_size/pp_size`，再按 group 构造顺序检查每个 rank 的 attention 与 MoE 坐标，最后核对 forward 中 gather/scatter/A2A/all-reduce 的 tensor shape。
