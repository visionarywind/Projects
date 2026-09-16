# DeepSeek 源码证据与调试地图

- 文档目的：把 DeepSeek 的配置、并行组、模型构造和 forward 关键点映射到源码，便于逐步调试。
- 对应源码版本：`source/sglang` HEAD `78be4b50af`。
- 证据状态：路径和行号按当前 checkout 静态确认；没有运行时日志。

## 1. 配置与模型识别

| 目的 | 关键位置 | 应观察的值 |
|---|---|---|
| 识别 DeepSeek 架构 | `source/sglang/python/sglang/srt/server_args.py:1764-1773` | `model_arch`、`is_deepseek_nsa(hf_config)` |
| 选择 DSA backend | `server_args.py:1773-1796` | `attention_backend=nsa`、`index_topk` threshold |
| 设置 DSA CP 限制 | `server_args.py:1798-1853` | `enable_nsa_prefill_context_parallel`、`attn_cp_size`、`dp_size`、`ep_size` |
| 设置 DP Attention | `server_args.py:3012-3028` | `enable_dp_attention`、chunked prefill、整除约束 |
| 设置 A2A EP | `server_args.py:3138-3201` | `moe_a2a_backend`、自动改写 `ep_size` |
| 设置 PP | `server_args.py:3259-3264` | `pp_size`、overlap schedule 是否关闭 |

## 2. 并行 group 和 rank

| 目的 | 关键位置 | 结论 |
|---|---|---|
| attention rank 公式 | `source/sglang/python/sglang/srt/layers/dp_attention.py:240-272` | `(dp, cp, tp)` 布局及 local info |
| DP 初始化 | `dp_attention.py:275-316` | global/local DP rank、buffer metadata |
| attention TP group | `source/sglang/python/sglang/srt/distributed/parallel_state.py:1906-1938` | 宽度不足全 TP 时创建独立 group |
| MoE DP/EP/TP | `parallel_state.py:1940-2024` | 根据 EP/DP/TP 宽度建组或复用 `_TP`/`_ATTN_CP` |
| PP group | `parallel_state.py:2026-2044` | rank 按步长构成 PP stage group |
| custom group | `parallel_state.py:2047-2093` | all-gather 配置、排序后所有 rank 一致 `new_group` |

## 3. 模型构造和 weight ownership

| 目的 | 关键位置 | 调试问题 |
|---|---|---|
| PP 层范围 | `deepseek_v2.py:2047-2107` | 当前 rank 是否只创建自己的层，first/last rank 是否正确 |
| MLA local heads | `deepseek_v2.py:1312-1324` | `attn_tp_rank/size`、`num_local_heads` 是否可整除 |
| MLA projections | `deepseek_v2.py:1333-1433` | column/row parallel 的 rank 和 reduce_results |
| MoE expert layout | `deepseek_v2.py:398-428` | `moe_ep_size`、shared fusion、expert slots/top-k |
| MoE shared expert | `deepseek_v2.py:533-565` | A2A backend 下是否 replicated 或 fused |
| V3/V3.2 entry class | `deepseek_v2.py:2299-2366,2529-2555` | architecture 是否映射到预期 class |

## 4. Forward 断点顺序

```text
1. ServerArgs resolved config
2. initialize_model_parallel / initialize_dp_attention
3. DeepseekV2Model.__init__
4. DeepseekV2AttentionMLA.__init__
5. DeepseekV2MoE.__init__
6. DeepseekV2Model.forward
7. DeepseekV2AttentionMLA.dispatch_attn_forward_method
8. DeepseekV2MoE.forward
9. attention/MoE collective
10. PP proxy 或 last-rank norm/logits
```

对应位置：

- PP/CP 输入和输出：`deepseek_v2.py:2160-2296`；
- attention backend dispatch：`deepseek_v2.py:1512-1532`、`1573-1697`；
- MLA CP KV rebuild：`deepseek_v2.py:1716-1728`；
- MoE normal/A2A 分支：`deepseek_v2.py:644-697`、`756-858`。

## 5. 常见症状到检查点

| 症状 | 首先检查 | 可能原因 |
|---|---|---|
| `num_heads` 不可整除 | `dp_attention.py:240-255`、`deepseek_v2.py:1321-1324` | DP/CP 后 attention TP 宽度不合法 |
| 启动时 ep_size 被改写 | `server_args.py:3138-3201` | A2A backend 当前强制 `ep_size=tp_size` |
| DSA 不走 NSA | `server_args.py:1773-1796` | architecture/config 未识别、backend 被显式覆盖 |
| CP 启动断言 | `server_args.py:1798-1853,2974-3010` | 单机、dp、moe_dp、ep、kv dtype 或 PP 限制不满足 |
| MoE 输出数值重复/缩放异常 | `deepseek_v2.py:459-471,741-754,845-858` | shared expert scaling、A2A combine 或 TP all-reduce 重复 |
| PP rank 缺少 embedding/lm head | `deepseek_v2.py:2047-2107,2322-2346` | first/last rank 识别或 PP layer partition 错误 |
| DP batch hang/shape 错位 | `dp_attention.py:388-420,464-...` | global token count、padding mode、DP rank slice 不一致 |
| MTP draft 与 target group 不匹配 | `parallel_state.py:2137-2159`、`dp_attention.py:367-385` | 临时 patch/disable DP 状态未恢复 |

## 6. 运行前静态检查清单

```text
[ ] architecture 是 DeepseekV3ForCausalLM / DeepseekV32ForCausalLM 或预期变体
[ ] tp_size % dp_size == 0
[ ] tp_size % attn_cp_size == 0（启用 CP 时）
[ ] ep_size、moe_dp_size、moe_tp_size 满足 group 约束
[ ] `num_heads % attention_tp_size == 0`
[ ] A2A backend 与 ep_size/runner/quantization 兼容
[ ] DSA 的 attention backend、KV dtype、page size 已由最终配置确认
[ ] PP layer range、first/last rank 和权重过滤一致
[ ] MTP 的 draft model/worker 不误用 target 的 DP/TP group
```

## 7. 测试入口（仅登记，不代表已运行）

- DeepSeek 基础：`source/sglang/test/registered/8-gpu-models/test_deepseek_v3_mtp.py`、`test_deepseek_v32.py`；
- CP：`source/sglang/test/registered/cp/test_deepseek_v32_cp_single_node.py`；
- DP Attention：`source/sglang/test/registered/distributed/test_dp_attention.py`；
- 并行状态：`source/sglang/test/registered/distributed/test_parallel_state.py`；
- EP/MoE：`source/sglang/test/registered/ep/`、`test/registered/moe/`；
- 手工 MLA：`source/sglang/test/manual/mla/test_mla_deepseek_v3.py`。

## 相关文档
- [M19 入口](README.md)
- [模型家族与 Forward](model-family.md)
- [并行策略与拓扑](parallel-strategies.md)

## 源码证据摘要
本文以具体函数区间作为断点和结论证据；`...` 仅表示同一文件中尚未逐行展开的后续函数，不表示已执行。

## 未解决问题
测试文件存在性已确认，但当前工作区未运行 checkpoint、GPU、NCCL、DeepEP 或多节点测试。

## 下一步阅读建议
将启动日志中的 resolved `ServerArgs`、每个 rank 的 group membership、attention TP size、MoE EP size 和 PP layer range 记录为一行，再对照本页表格定位问题。
