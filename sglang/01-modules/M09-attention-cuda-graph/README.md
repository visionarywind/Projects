# M09 Attention 与 CUDA Graph

- 文档目的：解释 model runner 如何选择 attention backend、准备 forward metadata，并在 eager、prefill graph 和 decode graph 之间选择执行路径。
- 适用范围：`AttentionBackend` 合约、backend registry、`build_attention_backends`、`ModelRunner._forward_raw`、decode/prefill graph eligibility。
- 对应源码版本：`f1a512c51c73ab660cf41e1af3110c7c11e3b600`
- 证据状态：部分完成
- 最后更新：2026-09-10
- 前置阅读：[M05 模型执行](../M05-model-execution/README.md)、[M08 KV Cache 与 Radix Cache](../M08-kv-cache/README.md)
- 后续阅读：[M07 分布式并行](../M07-分布式并行.md)、[性能关键路径](../../90-cross-module/performance-critical-paths.md)

## 1. 先建立直觉

**通俗解释**：attention backend 不是模型本身，而是“如何用当前硬件读取 Q/K/V 和 KV cache”的执行策略。CUDA Graph 也不是另一种模型，它是把形状稳定的一段 GPU 操作预先录制，后续用固定 buffer 重放。

因此同一轮 forward 需要回答两个问题：

1. 选择哪一个 prefill/decode attention backend；
2. 当前 batch 的形状、metadata、配置是否允许 graph replay，否则回退 eager。

## 2. `AttentionBackend` 的 metadata 合约

基础类将 forward metadata 初始化分成三段：eager 入口 `init_forward_metadata`、graph 外的动态准备 `init_forward_metadata_out_graph`，以及 graph 内可记录的静态 shape GPU 操作 `init_forward_metadata_in_graph`。[`python/sglang/srt/layers/attention/base_attn_backend.py:36-129`]

这一区分解决了一个实际问题：`.cpu()`、`.item()`、动态 shape tensor 创建等 host/dynamic 操作不能安全地录进 CUDA Graph，因此必须留在 graph 外；真正可记录的 device 操作才放入 graph 内。[`python/sglang/srt/layers/attention/base_attn_backend.py:96-129`]

backend 还声明 `SharedReadEnds`，描述 scheduler 共享数据的读取何时结束；默认 decode/verify 读到 replay 内，其他模式采用保守的 unknown fence。[`python/sglang/srt/layers/attention/base_attn_backend.py:22-35`][`python/sglang/srt/layers/attention/base_attn_backend.py:147-159`]

**开发含义**：attention metadata 不只是临时 Python 字段。overlap 和 graph 代码必须知道它是否引用 scheduler 仍会修改的 tensor；错误的生命周期判断可能形成 WAR race。

## 3. backend 选择和实例化

backend registry 使用装饰器把名字映射到创建函数；例如 `flashinfer` 根据是否 MLA 选择普通或 MLA backend，并在特定 speculative 配置下创建 planning stream。[`python/sglang/srt/layers/attention/attention_registry.py:40-75`]

`build_attention_backends` 先读取 runner 已解析的 prefill/decode backend 名称，再按部署模式选择：

- PDMUX：创建一个主 backend 和多个 decode backend；
- two-batch overlap：用 `TboAttnBackend` 包装子 backend；
- 普通路径：创建单一 resolved backend。[`python/sglang/srt/model_executor/model_runner_components/attention_backend_setup.py:69-115`]

完成后，resolved 名称被写回 backend，形成后续模型 dispatch 可使用的 per-mode 记录。[`python/sglang/srt/model_executor/model_runner_components/attention_backend_setup.py:133-143`]

**已确认**：backend 名称不是“导入哪个 Python 文件”这么简单；设备、MLA、speculative、PDMUX 和 overlap 都可能改变最终对象拓扑。

## 4. eager、prefill graph、decode graph

`ModelRunner._forward_raw` 先判断当前 `ForwardMode` 是否允许 graph，以及对应 runner 是否存在、`can_run_graph(forward_batch)` 是否为真。满足时直接执行 decode graph；不满足时先做 eager batch preparation。[`python/sglang/srt/model_executor/model_runner.py:1763-1805`]

之后：

```text
split prefill
  -> forward_split_prefill
else if extend and prefill graph eligible
  -> prefill_cuda_graph_runner.execute
else
  -> eager_runner.execute
```

对应分支在 `1811-1847`：split prefill 由 `ModelRunner` 自己处理；extend 只有在 prefill runner、graph eligibility 和 context-parallel 条件都满足时才 replay；其余 decode/extend/idle 走 eager runner。[`python/sglang/srt/model_executor/model_runner.py:1811-1847`]

**关键边界**：prefill graph 和 decode graph 不是同一个 runner，也不是所有 batch 都能使用。没有满足静态 shape 或 backend 条件时回退 eager 是设计行为，不是必然错误。

## 5. graph eligibility 的具体例子

decode runner 的 `can_run_graph` 会拒绝动态 token embedding override；在 ragged verify、speculative request width 不匹配、batch size 超过支持范围或 DP/MLP sync 条件不满足时也可能拒绝。[`python/sglang/srt/model_executor/runner/decode_cuda_graph_runner.py:664-718`]

例如：

```text
batch A: 固定 decode width、batch size 在 capture bucket 内
  -> 可以尝试 graph replay

batch B: 某请求有 replace_embeds
  -> can_run_graph = false
  -> eager

batch C: speculative num_tokens_per_req 与 capture width 不同
  -> can_run_graph = false
  -> eager
```

不要把 `can_run_graph=True` 理解为“模型结果已计算”；它只表示 runner 选择 graph 执行，结果仍要经过 logits/sample 和 M04 result processor。

## 6. 与 KV、overlap 和并行的关系

Attention backend 使用 M08 的 request-to-token/KV metadata，`ForwardBatch` 负责把 request row、sequence lengths 和 output cache location 传入 worker；M04 overlap 则要求 backend 声明共享读结束点。DP attention/MLP sync 还会影响 padding 和 graph eligibility。[`python/sglang/srt/model_executor/forward_batch_info.py:722-756`][`python/sglang/srt/model_executor/model_runner.py:1795-1805`]

所以改 attention backend 不能只验证 kernel 输出，还要联查：

- `ForwardBatch.init_new` 的 metadata shape；
- KV index translation 和 cache layout；
- graph capture/replay 的 buffer address；
- overlap 的 stream/event/fence；
- TP/DP/PP collective 的输入形状。

## 7. 调试断点

| 断点 | 观察内容 |
|---|---|
| `build_attention_backends:69-143` | resolved 名称、PDMUX/overlap wrapper、backend 对象 |
| `AttentionBackend.init_forward_metadata:88-129` | 动态 metadata 与 graph 内 metadata 是否分离 |
| `ModelRunner._forward_raw:1763-1805` | decode graph eligibility、eager preparation |
| `ModelRunner._forward_raw:1811-1854` | split/prefill graph/eager 分支 |
| `DecodeCudaGraphRunner.can_run_graph:673-718` | 具体回退原因和 graph key |

## 8. 验证边界

**已定位但未执行**：attention backend unit tests、CUDA Graph tests、不同 batch bucket 的 profiling 和多卡运行均未执行。本文依据静态源码确认选择和生命周期分支，不声称任意 GPU/模型配置可 graph replay。

## 相关文档

- [M05 模型执行](../M05-model-execution/README.md)
- [M08 KV Cache 与 Radix Cache](../M08-kv-cache/README.md)
- [M07 分布式并行](../M07-分布式并行.md)
- [跨模块性能关键路径](../../90-cross-module/performance-critical-paths.md)

## 源码证据摘要

- [`python/sglang/srt/layers/attention/base_attn_backend.py:22-159`](../../../python/sglang/srt/layers/attention/base_attn_backend.py)
- [`python/sglang/srt/layers/attention/attention_registry.py:40-120`](../../../python/sglang/srt/layers/attention/attention_registry.py)
- [`python/sglang/srt/model_executor/model_runner_components/attention_backend_setup.py:69-178`](../../../python/sglang/srt/model_executor/model_runner_components/attention_backend_setup.py)
- [`python/sglang/srt/model_executor/model_runner.py:1763-1854`](../../../python/sglang/srt/model_executor/model_runner.py)
- [`python/sglang/srt/model_executor/runner/decode_cuda_graph_runner.py:664-718`](../../../python/sglang/srt/model_executor/runner/decode_cuda_graph_runner.py)

## 深度审计

| 对象 | 入口 | 正常 | 分支 | 异常 | 清理 | 生命周期 | 执行上下文 | 行级证据 | Demo | 状态 |
|---|---|---|---|---|---|---|---|---|---|---|
| backend selection | 已完成 | 已完成 | overlap/PDMUX/MLA 部分完成 | 部分完成 | 部分完成 | 已完成 | worker/device | 已完成 | D01 静态映射 | 部分：专用 backend 待补 |
| graph/eager choice | 已完成 | 已完成 | decode/prefill/split 已覆盖 | 回退已覆盖 | 部分完成 | 已完成 | GPU stream/graph | 已完成 | D01 静态映射 | 需实际 capture 验证 |
| metadata lifetime | 已完成 | 部分完成 | graph contract 已覆盖 | 部分完成 | 部分完成 | 部分完成 | overlap/device | 已完成 | D01 未实际覆盖 | 需 batch-overlap 专题 |

## 未解决问题

- 各 attention kernel 的 KV layout 和模型专用 backend 尚未逐一分析；
- CUDA Graph capture 初始化和内存预算路径尚未完整展开；
- 真实 graph capture、replay、fallback 和性能未验证。
