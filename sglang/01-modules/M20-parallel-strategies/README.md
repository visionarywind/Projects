# M20 SGLang 并行策略源码分析

- 文档目的：从配置解析、rank 拓扑、并行组、collective 到模型 forward，解释 SGLang 中 TP、PP、DP Attention、CP/DCP、EP 及 MoE DP/TP 如何协同工作。
- 适用范围：`server_args.py`、`bootstrap.py`、`distributed/parallel_state.py`、`layers/dp_attention.py`、通信层、MoE dispatcher、模型实现及相关测试。
- 对应源码版本：`source/sglang` HEAD `78be4b50af`（2026-09-15）。
- 证据状态：静态源码和仓库文档已确认；真实 GPU、多进程、跨节点通信、权重加载和性能未验证。

## 1. 术语核验：SPTD/CPTD 与真实源码名称

当前 checkout 中没有精确匹配的 `SPTD`/`sptd`，也没有名为 SPTD 的 CLI flag、类、函数或 parallel group。此前核验的 `CPTD` 同样没有精确匹配。因此不能把 SPTD/CPTD 写成 SGLang 内置策略。

本文将“SPTD 等并行策略”拆解为源码中实际存在的维度：

- **TP**：Tensor Parallel，切分线性层和 attention head/hidden 维度；
- **PP**：Pipeline Parallel，按 Transformer layer range 切分；
- **DP**：Data Parallel，以及 attention 专用的 DP Attention；
- **CP**：Context Parallel，切分长序列 attention/KV 或 prefill token；
- **DCP**：Decode Context Parallel，服务 decode 阶段的 context-parallel group；
- **EP**：Expert Parallel，切分 routed experts 和 token dispatch；
- **MoE DP/TP**：分别处理专家副本协作和专家内部 tensor 切分；
- **A2A**：All-to-All dispatcher/backend，是通信执行方式，不是额外的切分维度；
- **MTP/NextN**：推测解码模型/算法，不是新的 model-parallel group。

如果 SPTD 是外部论文、其他推理框架或内部部署方案的缩写，应补充全称和出处，才能建立精确映射。

## 2. 统一心智模型：四层而不是一个“并行开关”

```text
ServerArgs / ModelConfig
  -> 并行宽度与合法性校验
  -> WORLD、TP、PP、attention CP/TP/DP、MoE DP/EP/TP groups
  -> collective / dispatcher / token-KV buffer
  -> linear、attention、MoE、logits、PP proxy 的实际消费
```

应始终分开观察：

```text
拓扑：哪些 global rank 在一起
  -> torch ProcessGroup / GroupCoordinator

张量布局：每个 rank 持有什么 weight、head、token、KV 或 expert
  -> parallel linear / local heads / local expert map

通信执行：如何交换和聚合
  -> all-gather、all-reduce、reduce-scatter、all-to-all、A2A

调度优化：何时切 batch、padding、capture 或 overlap
  -> DP padding、chunked prefill、CUDA Graph、PP schedule
```

关闭 custom all-reduce 会改变通信实现，不会改变 TP 成员；启用 DP Attention 也不意味着所有模型层都改用同一组 DP rank。

## 3. 配置入口：宽度先被解析和约束

`ServerArgs` 保存 `tp_size`、`pp_size`、`dp_size`、`attn_cp_size`、`moe_dp_size`、`ep_size` 和 DP/CP/A2A backend 选项。初始化阶段依次执行模型特化、数据并行、context parallel、EPLB/dispatch、pipeline parallel 和 speculative decoding 处理。[`python/sglang/srt/server_args.py:438-442,516-522,715-716,757-766,909-947`]

关键约束包括：

- 基础 WORLD 要满足 `world_size = tp_size * pp_size`；
- attention CP 启用时，`tp_size` 要能被 `attn_cp_size` 以及 `dp_size * attn_cp_size` 整除；
- MoE DP 启用时，`tp_size` 要能被 `moe_dp_size`，且 `ep_size * moe_dp_size <= tp_size`；
- `attn_cp_size != moe_dp_size` 只有在 `moe_dp_size == 1` 时支持；
- DP Attention 需要 `tp_size % dp_size == 0`，并会调整 chunked prefill 和调度参数；
- `pp_size > 1` 会影响 overlap schedule；
- 部分 A2A backend 会将 `ep_size` 调整为 `tp_size`，并改变 CUDA Graph 或 shared-expert 约束。[`python/sglang/srt/server_args.py:2974-3028,3138-3201,3259-3264`]

这些检查发生在建组之前；不能只看命令行参数推断最终拓扑，必须以 resolved `ServerArgs` 为准。

## 4. TP 与 PP：基础模型并行坐标

### 4.1 TP group 与 linear 消费

`initialize_model_parallel` 先按连续 global rank 创建 `_TP`。例如 `world_size=8,tp_size=4` 时，TP groups 为 `[0,1,2,3]`、`[4,5,6,7]`。[`python/sglang/srt/distributed/parallel_state.py:1820-1840`]

模型层通过 parallel linear 消费该 group：column-parallel 权重保留输出分片，row-parallel 权重在输入分片后按需要 all-reduce；attention 专用 TP 可能是 `_ATTN_TP` 而非全 `_TP`。[`python/sglang/srt/layers/linear.py:1510-1548`]

### 4.2 PP group 与 layer ownership

PP group 按跨区间步长构造。例如 `tp=2,pp=4,world=8` 时，PP group 是 `[0,2,4,6]` 和 `[1,3,5,7]`，不是连续的 `[0,1,2,3]`。[`python/sglang/srt/distributed/parallel_state.py:2026-2044`]

PP 的消费不在 collective 名称本身，而在模型构造：`get_pp_indices`/`make_layers` 只实例化当前 stage 的 layer range；first rank 持有 embedding，last rank 持有 norm/lm head，中间 rank 通过 proxy tensors 传递 hidden/residual。DeepSeek 的具体实例见 [M19 模型家族](../M19-deepseek-models/model-family.md)。

PP 是 layer parallel，不是 token DP；改变 PP size 必须同时检查 layer partition、P2P/PP proxy、权重过滤、overlap schedule 和 pipeline bubble。

## 5. DP 与 DP Attention：数据坐标改变 attention 宽度

### 5.1 attention rank 公式

DP Attention 开启时，代码采用 `(dp, cp, tp)` 布局，TP 是最快变化维度：

```python
attn_dp_size = dp_size if enable_dp_attention else 1
attn_tp_size = tp_size // attn_dp_size // attn_cp_size
attn_tp_rank = tp_rank % attn_tp_size
attn_dp_rank = tp_rank // (attn_tp_size * attn_cp_size)
```

该逻辑由 `compute_dp_attention_world_info` 和 `initialize_dp_attention` 建立；因此 `tp=8,dp=2,cp=1` 时，每个 attention DP replica 使用 4-way attention TP，而不是继续使用 8-way attention TP。[`python/sglang/srt/layers/dp_attention.py:240-316`]

### 5.2 token buffer 与 collective

`initialize_dp_attention` 保存 global/local DP rank、hidden size、dtype 和 device。执行时根据每个 DP rank 的 token 数计算 local slice；非 graph 模式按前序 token 数定位，graph 模式使用固定 batch slot。padding mode 在 `MAX_LEN` 与 `SUM_LEN` 之间选择，影响 gather buffer 和 graph/symmetric-memory 兼容性。[`python/sglang/srt/layers/dp_attention.py:275-316,388-420`]

collective 路径可能包含 TP all-gather、attention-TP reduce-scatter 以及 WORLD/TP all-gather；其作用是形成正确的 global token buffer，而非单纯减少通信。[`python/sglang/srt/layers/dp_attention.py:568-602`]

### 5.3 DP Attention 与模型消费

模型 attention 初始化使用 `get_attention_tp_rank/size`，并以派生后的宽度计算 local heads；MLA/DSA、其他支持 DP Attention 的模型和 logits processor 都必须匹配同一坐标。DP LM head 还依赖 DP Attention。[`python/sglang/srt/models/deepseek_v2.py:1273-1433`][`python/sglang/srt/server_args.py:3012-3028`]

## 6. CP 与 DCP：序列维度和 decode 维度的不同路径

普通 attention CP、DeepSeek DSA prefill CP 和 DCP 不能混称：

- attention CP (`attn_cp_size`) 参与 attention `(dp,cp,tp)` 坐标，通常处理 prefill 长序列/上下文；
- DSA prefill CP 还要 split hidden/position、重建 latent/rope KV 并按全局顺序 rerange；
- DCP (`decode_context_parallel_size`) 是 decode 阶段的独立 context-parallel group，建组和平台限制不同。

`parallel_state` 先在 TP 内构造 DCP/attention CP 派生组；当 CP 覆盖整个 TP 时可能复用 `_TP`，否则创建专用 `_ATTN_CP`/`_ATTN_TP`。[`python/sglang/srt/distributed/parallel_state.py:1869-1938`]

DeepSeek forward 的典型生命周期是：

```text
hidden/position
  -> CP split/rebuild
  -> local MLA/NSA + local KV write
  -> CP all-gather/rerange
  -> 恢复全局 token 顺序
```

[`python/sglang/srt/models/deepseek_v2.py:2160-2296,1716-1728`]

DCP 需要额外检查设备、`tp_size % decode_context_parallel_size` 以及 decode attention backend；相关配置验证位于 `server_args.py` 的 DCP 处理逻辑。[`python/sglang/srt/server_args.py:1342-1344,486-492`]

## 7. EP、MoE DP 与 MoE TP：专家维度的三套坐标

`initialize_model_parallel` 派生：

```text
moe_ep_size = expert_model_parallel_size
moe_dp_size = moe_data_model_parallel_size
moe_tp_size = tensor_model_parallel_size // moe_ep_size // moe_dp_size
```

然后构造或复用 `_MOE_DP`、`_MOE_EP`、`_MOE_TP`。满足宽度条件时 group 可能直接复用 `_TP` 或 `_ATTN_CP`；group 名称不代表对象一定独立。[`python/sglang/srt/distributed/parallel_state.py:1940-2024`]

MoE forward 的实际边界是 dispatcher/runner：

```text
router/top-k
  -> logical expert id
  -> local/physical expert mapping
  -> TP gather 或 A2A dispatch
  -> local expert GEMM
  -> combine / shared expert
  -> 必要时 TP all-reduce
```

standard dispatcher 负责 top-k、hidden payload 和 local expert mapping；A2A backend（例如 DeepEP、Mooncake、NIXL、FlashInfer 变体）改变 token 交换方式，不改变 router 的逻辑专家语义。[`python/sglang/srt/layers/moe/token_dispatcher/standard.py:145-224`][`python/sglang/srt/models/deepseek_v2.py:386-428,622-697,748-858`]

MoE DP 与 attention DP 的 token ownership 不一定一致。当 `attn_cp_size > moe_dp_size` 时，MoE 前需要跨 CP rank 共享 token；这也是配置中对 `attn_cp_size != moe_dp_size` 施加限制的原因。[`python/sglang/srt/layers/dp_attention.py:605-622`]

## 8. collective 与通信实现

`GroupCoordinator` 根据 tensor 所在设备和注册的 communicator 执行 collective：GPU 可能走 device group/custom op，CPU 可能走 shared-memory 或 PyTorch distributed，最后按调用者期望 reshape/movedim。[`python/sglang/srt/distributed/parallel_state.py:1281-1353`]

典型消费点：

- attention TP hidden-state gather，并用 `is_pre_gathered` 防止重复聚合；[`python/sglang/srt/layers/communicator.py:247-292`]
- DP Attention gather/reduce-scatter；[`python/sglang/srt/layers/dp_attention.py:568-602`]
- vocab-parallel logits 的 TP all-to-all 和 row 重组；[`python/sglang/srt/layers/logits_processor.py:1016-1023`]
- MoE token dispatcher 的 TP all-gatherv 和 expert mapping；[`python/sglang/srt/layers/moe/token_dispatcher/standard.py:145-224`]
- PP proxy/P2P 的 stage 间 activation 传输；[`python/sglang/srt/distributed/parallel_state.py:2026-2044`]

因此排查 hang、shape mismatch 或数值重复时，要同时记录 group members、rank-in-group、输入/输出 shape、padding mode 和 collective 次序；只确认 world size 不足以证明拓扑正确。

## 9. 一组具体 rank 示例

### 9.1 TP + PP

```text
world_size=8, tp_size=2, pp_size=4
TP: [0,1] [2,3] [4,5] [6,7]
PP: [0,2,4,6] [1,3,5,7]
```

对于 rank 2：

```text
global rank = 2
TP group = [2,3], rank_in_tp = 0
PP group = [0,2,4,6], rank_in_pp = 1
```

rank 2 只持有 PP stage 1 的 layer range，并与 rank 3 协同完成该 stage 的 TP 计算。

### 9.2 TP + DP Attention

```text
tp_size=8, dp_size=2, attn_cp_size=1
全局 tp_rank:       0 1 2 3 | 4 5 6 7
attention-dp rank:  0 0 0 0 | 1 1 1 1
attention-tp rank:  0 1 2 3 | 0 1 2 3
```

attention weights/head 按 4-way TP 分片；两组 DP replica 负责不同 token/KV 工作集。MoE 是否同样按两组划分，要由 `moe_dp_size/ep_size/moe_tp_size` 重新计算，不能直接套用上表。

## 10. 生命周期、错误和清理

```text
init_process_group
  -> init_torch_distributed
  -> initialize_model_parallel
  -> model construction / weight loading
  -> request forward collectives
  -> destroy_model_parallel
  -> destroy_distributed_environment
```

启动时会校验 WORLD 与 TP×PP、DCP 整除和 backend；某 rank 在模型加载或 collective 中失败，可能经 barrier、worker/scheduler 异常路径传播到父进程。清理时必须注意 `_MOE_DP` 可能复用 `_ATTN_CP` 或 `_TP`，按对象身份避免重复销毁。[`python/sglang/srt/distributed/bootstrap.py:70-172,250-308`][`python/sglang/srt/distributed/parallel_state.py:1-2417`]

## 11. 推荐断点

1. `server_args.py:909-947`：确认各并行维度的处理顺序；
2. `server_args.py:2974-3028`：确认整除和 DP/CP/MoE 约束；
3. `bootstrap.py:250-308`：确认 world/rank 和 group 初始化入口；
4. `parallel_state.py:1820-2044`：逐个打印 TP、DCP、attention、MoE、PP group members；
5. `dp_attention.py:240-316`：确认 attention rank 坐标；
6. `dp_attention.py:388-420,568-602`：确认 token slice、padding 和 gather/reduce-scatter shape；
7. `communicator.py:247-292`：确认 hidden-state 是否已预聚合；
8. `standard.py:145-224`：确认 MoE token dispatch 与 local expert mapping；
9. 模型的 attention/MoE constructor 和 forward：确认派生 group 真正被消费；
10. `parallel_state.py` 的 destroy 路径：确认 alias 不重复销毁。

## 12. 测试入口与未验证边界

静态登记的测试入口包括：

- `test/registered/distributed/test_parallel_state.py`；
- `test/registered/distributed/test_dp_attention.py`；
- `test/registered/dcp/`；
- `test/registered/ep/`、`test/registered/moe/`；
- `test/registered/pp/`；
- `test/registered/8-gpu-models/test_deepseek_v3_mtp.py`；
- `test/registered/cp/test_deepseek_v32_cp_single_node.py`。

本地未运行这些测试，也未执行 NCCL/RCCL/HCCL、跨节点 TCPStore/RDMA、DeepEP/A2A、CUDA Graph、checkpoint 加载或性能 benchmark。测试文件存在只证明测试入口存在，不证明策略在目标硬件上成功。

## 相关文档

- [M07 分布式并行](../M07-分布式并行.md)
- [M19 DeepSeek 系列模型与并行策略](../M19-deepseek-models/README.md)
- [模型加载与并行初始化](../../03-model-loading/01-模型加载与并行初始化.md)
- [M14 MoE、量化与 LoRA](../M14-moe-quantization-lora/README.md)

## 源码证据摘要

本文以 `source/sglang` HEAD `78be4b50af` 为证据锚点。源码引用用于说明静态调用关系和字段消费；动态通信、硬件 backend、模型加载、性能和端到端服务均未验证。

## 未解决问题

- SPTD/CPTD 的外部全称和语境未知；
- 不同硬件 backend 的 collective 实际实现和性能未验证；
- TP/DP/CP/EP/PP 组合在真实 checkpoint 上的数值正确性和容量收益未验证；
- A2A、DCP、CUDA Graph 与 speculative decoding 叠加后的边界仍需运行测试。

## 下一步阅读建议

先读本页建立统一 rank/数据/通信坐标，再读 [M07](../M07-分布式并行.md) 的通用建组实现，最后读 [M19](../M19-deepseek-models/README.md) 观察 DeepSeek MLA/DSA/MoE 如何消费这些 group。
