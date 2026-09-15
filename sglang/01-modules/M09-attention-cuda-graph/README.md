# M09 Attention 与 CUDA Graph

- 文档目的：解释 model runner 如何选择 attention backend、准备 forward metadata，并在 eager、prefill graph 和 decode graph 之间选择执行路径。
- 适用范围：`AttentionBackend` 合约、backend registry、`build_attention_backends`、`ModelRunner._forward_raw`、decode/prefill graph eligibility。
- 对应源码版本：`78be4b50af88e9ea72d75b4c3a3e42b7297d2501`
- 证据状态：部分完成
- 最后更新：2026-09-15
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

### 3.1 代表 backend 的能力边界

registry 当前同时覆盖普通 full-attention、MLA、稀疏 attention、hybrid/linear attention 和 CPU/框架 fallback；这里不应把所有实现压缩成同一套 KV layout。`flashinfer` 在非 MLA 时实例化 `FlashInferAttnBackend`，并根据 quantized KV access、sliding-window 或 encoder-decoder 可能维护多个 wrapper；MLA 则转到独立 `FlashInferMLAAttnBackend`。`trtllm_mla`、`tokenspeed_mla`、`cutedsl_mla` 在创建时强制要求 MLA，且 TRT-LLM MLA 在 DCP+speculative 的特定组合下直接拒绝，因为该实现不能提供跨 rank merge 所需的 metadata。[`python/sglang/srt/layers/attention/attention_registry.py:51-113`](../../../source/sglang/python/sglang/srt/layers/attention/attention_registry.py)[`python/sglang/srt/layers/attention/flashinfer_backend.py:291-391`](../../../source/sglang/python/sglang/srt/layers/attention/flashinfer_backend.py)

`HybridAttnBackend` 是一个协议适配器而非 kernel：EXTEND 通常交给 prefill backend，decode/idle 交给 decode backend，TARGET_VERIFY 根据 speculative attention mode 选择其中之一，并把 `needs_cpu_seq_lens`、ragged verify 和 CUDA graph state 的能力向外汇总。[`python/sglang/srt/layers/attention/hybrid_attn_backend.py:24-127`](../../../source/sglang/python/sglang/srt/layers/attention/hybrid_attn_backend.py)

`TorchNativeAttnBackend` 是明确的正确性 fallback：它从 runner 保存 request/KV pool 引用，初始化时在需要 sliding-window 时把 full pool 的 output location 翻译为 SWA location，再使用 PyTorch SDPA；它的逐请求 Python 循环与 fused backend 的性能语义不同，但说明 metadata 契约仍必须提供 request row、sequence length 和 cache location。[`python/sglang/srt/layers/attention/torch_native_backend.py:19-79`](../../../source/sglang/python/sglang/srt/layers/attention/torch_native_backend.py)

DSA、DSv4、QSA、NSA、Mamba/linear 等专用 backend 还会额外维护 indexer metadata、压缩/稀疏 page table 或 recurrent state；它们不是普通 FlashAttention 的换名实现。分析某一模型时必须同时记录 backend 名称、ForwardMode、KV pool 类型、page size、DCP/TP 和是否 speculative，不能仅凭 `--attention-backend` 推断 kernel layout。


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

## 7. capture 初始化与 replay 生命周期

`DecodeCudaGraphRunner.capture` 不是简单调用一次 `torch.cuda.graph`：它先 warmup/autotune，并根据 warmup 结果重新决定 torch.compile bucket；然后恢复 capture 所需的 `seq_lens` 填充值、重置 index buffers，在冻结 GC 的上下文中为每个 capture stream 建立 `graph_capture` 和 backend capture session。[`python/sglang/srt/model_executor/runner/decode_cuda_graph_runner.py:1026-1088`]

`_capture_one_stream` 按**大 batch 到小 batch**的逆序遍历 capture buckets，以便较小 graph 尽量复用已建立的 CUDA memory pool。每个 bucket 还可能按 LoRA variant 和 DSA dense/sparse variant 各捕获一份图；这意味着 graph key 不只是 batch size。[`python/sglang/srt/model_executor/runner/decode_cuda_graph_runner.py:1093-1150`]

每个 shape 的 capture 先通过 `capture_prepare` 从 buffer registry 取出固定 storage 的 input/length/cache-location view，构造 dummy `ForwardBatch` 和 PP proxy；随后在同一个 `ForwardContext` 中执行 graph 外 metadata 准备、graph 内 metadata hook、模型 forward 和 backend `capture_one`。[`python/sglang/srt/model_executor/runner/decode_cuda_graph_runner.py:866-1024`][`python/sglang/srt/model_executor/runner/decode_cuda_graph_runner.py:1152-1262`]

```text
warmup/autotune
  -> reset capture-time buffers
  -> capture stream + backend session
  -> large-to-small bucket
  -> capture_prepare(static views)
  -> metadata out-of-graph
  -> metadata in-graph + model forward
  -> store graph by shape/variant key
  -> replay only after can_run_graph
```

Prefill graph 有自己独立的 bucket 和生命周期。`can_run_graph` 除了检查 DP rank、输入 embedding/replace embeds、target verify、LoRA、hidden mode 和最大 token 数，还拒绝超过 padding waste 阈值的 bucket；它允许 `load_batch` 把实际 token 数向 capture bucket 填充，而不是要求 exact shape。[`python/sglang/srt/model_executor/runner/prefill_cuda_graph_runner.py:1178-1251`]。`capture_prepare` 将一个 aggregate token bucket 拆成不超过 context length 的 synthetic requests，填充固定 buffer registry，并构造 `ForwardMode.EXTEND` 的 dummy `ForwardBatch`；DP gather、spec info、M-RoPE、mamba tracking 和 LoRA static views 也在此绑定。[`python/sglang/srt/model_executor/runner/prefill_cuda_graph_runner.py:1262-1409`]

`PrefillCudaGraphRunner.capture` 先 warmup，再在 graph capture stream 和 backend session 中逆序捕获 token buckets；chunked-prefix 配置还会为每个 prefix variant 额外捕获 shape。`capture_one_shape` 负责 CP BCG、LoRA metadata、chunked-prefix preparation、graph 外 attention metadata 和 backend `capture_one`；`load_batch` 在 serving 时把 live batch 写入同一组固定地址，并将 MIXED replay 映射到 EXTEND graph。[`python/sglang/srt/model_executor/runner/prefill_cuda_graph_runner.py:1411-1546`][`python/sglang/srt/model_executor/runner/prefill_cuda_graph_runner.py:1548-1669`]。因此 prefill 的 graph key 还可能包含 prefix variant、CP/DP gather 和 backend mode，实际 token 数与 capture bucket 不一致本身不是错误，但超出 padding/fitting 约束就必须回退 eager。

## 8. 调试断点

| 断点 | 观察内容 |
|---|---|
| `build_attention_backends:69-143` | resolved 名称、PDMUX/overlap wrapper、backend 对象 |
| `AttentionBackend.init_forward_metadata:88-129` | 动态 metadata 与 graph 内 metadata 是否分离 |
| `ModelRunner._forward_raw:1763-1805` | decode graph eligibility、eager preparation |
| `ModelRunner._forward_raw:1811-1854` | split/prefill graph/eager 分支 |
| `PrefillCudaGraphRunner.can_run_graph:1178-1251` | DP、padding waste、LoRA、CP bucket 和回退条件 |
| `PrefillCudaGraphRunner.capture_prepare:1262-1409` | aggregate token bucket、static buffer、dummy batch |
| `PrefillCudaGraphRunner.capture/load_batch:1411-1669` | capture variant、固定地址写入和 serving replay |

## 9. 验证边界

**已定位但未执行**：attention backend unit tests、CUDA Graph tests、不同 batch bucket 的 profiling 和多卡运行均未执行。本文依据静态源码确认选择和生命周期分支，不声称任意 GPU/模型配置可 graph replay。

## 10. 相关文档

- [M05 模型执行](../M05-model-execution/README.md)
- [M08 KV Cache 与 Radix Cache](../M08-kv-cache/README.md)
- [M07 分布式并行](../M07-分布式并行.md)
- [跨模块性能关键路径](../../90-cross-module/performance-critical-paths.md)

## 11. 源码证据摘要

- [`python/sglang/srt/layers/attention/base_attn_backend.py:22-159`](../../../source/sglang/python/sglang/srt/layers/attention/base_attn_backend.py)
- [`python/sglang/srt/layers/attention/attention_registry.py:40-180`](../../../source/sglang/python/sglang/srt/layers/attention/attention_registry.py)
- [`python/sglang/srt/layers/attention/flashinfer_backend.py:291-391`](../../../source/sglang/python/sglang/srt/layers/attention/flashinfer_backend.py)
- [`python/sglang/srt/layers/attention/hybrid_attn_backend.py:24-147`](../../../source/sglang/python/sglang/srt/layers/attention/hybrid_attn_backend.py)
- [`python/sglang/srt/layers/attention/torch_native_backend.py:19-79`](../../../source/sglang/python/sglang/srt/layers/attention/torch_native_backend.py)
- [`python/sglang/srt/model_executor/model_runner.py`](../../../source/sglang/python/sglang/srt/model_executor/model_runner.py)（当前 checkout 的 backend setup 逻辑）
- [`python/sglang/srt/model_executor/model_runner.py:1763-1854`](../../../source/sglang/python/sglang/srt/model_executor/model_runner.py)
- [`python/sglang/srt/model_executor/cuda_graph_runner.py`](../../../source/sglang/python/sglang/srt/model_executor/cuda_graph_runner.py)
- [`python/sglang/srt/model_executor/piecewise_cuda_graph_runner.py`](../../../source/sglang/python/sglang/srt/model_executor/piecewise_cuda_graph_runner.py)

## 12. 深度审计

| 对象 | 入口 | 正常 | 分支 | 异常 | 清理 | 生命周期 | 执行上下文 | 行级证据 | Demo | 状态 |
|---|---|---|---|---|---|---|---|---|---|---|
| backend selection | 已完成 | 已完成 | overlap/PDMUX/MLA/Hybrid/Native 代表路径已覆盖 | 部分完成 | 部分完成 | 已完成 | worker/device | 已完成 | D01 静态映射 | DSA/DSv4/Mamba 等专用 backend 待补 |
| graph/eager choice | 已完成 | 已完成 | decode/prefill/split 已覆盖 | 回退已覆盖 | 部分完成 | 已完成 | GPU stream/graph | 已完成 | D01 静态映射 | capture 生命周期已补，需实际 capture 验证 |
| metadata lifetime | 已完成 | 部分完成 | graph contract 已覆盖 | 部分完成 | 部分完成 | 部分完成 | overlap/device | 已完成 | D01 未实际覆盖 | 需 batch-overlap 专题 |

## 13. 未解决问题

- 各 attention kernel 的 KV layout 和模型专用 backend 尚未逐一分析；
- capture 失败后的内存预算、回收和降级路径尚未完整展开；
- 真实 graph capture、replay、fallback 和性能未验证。

## 14. 资源池边界

`CudaGraphRunner` 为 capture batch 创建固定的 `DecodeInputBuffers`，按 `global_graph_memory_pool` 复用 graph allocation；KV cache 则可能在 custom memory pool context 中创建物理 tensor。graph static buffers、KV slot/page allocator、Radix ownership 和 flush/reset 必须分别回收，不能仅凭 graph 对象或 `torch.cuda.empty_cache()` 判断 backing 已释放（[source/sglang/python/sglang/srt/model_executor/cuda_graph_runner.py:547-721]；[source/sglang/python/sglang/srt/mem_cache/memory_pool.py:703-742]）。
