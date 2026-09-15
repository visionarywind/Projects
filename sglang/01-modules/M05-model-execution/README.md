# M05 模型执行

- 文档目的：解释 `ScheduleBatch` 如何变成 `ForwardBatch`，如何经过 TP worker 和 `ModelRunner` 完成 forward、logits 处理与 sampling。
- 适用范围：`TpModelWorker.forward_batch_generation`、`ForwardBatch.init_new`、`ModelRunner.forward`、`ModelRunner.sample`、PP proxy 和 prefill-only 分支。
- 对应源码版本：`78be4b50af88e9ea72d75b4c3a3e42b7297d2501`
- 证据状态：部分完成
- 最后更新：2026-09-10
- 前置阅读：[M04 Scheduler 与连续批处理](../M04-scheduler-batching/README.md)、[M07 分布式并行](../M07-分布式并行.md)
- 后续阅读：[M08 KV Cache 与 Radix Cache](../M08-kv-cache/README.md)、[模型执行与输出基础文章](../../02-request-flow/04-模型执行与输出.md)

## 1. 先用一句话理解

**通俗解释**：M04 决定“这一轮要算哪些请求、哪些 token、写入哪些 cache 位置”；M05 把这个计划翻译成模型可以执行的 device batch，运行 Transformer，再把 logits 变成下一 token。它不是重新决定 admission 的地方。

一个最小例子：

```text
两个请求进入 decode batch
  -> ScheduleBatch.seq_lens = [101, 57]
  -> ForwardBatch 持有 input_ids、req_pool_indices、out_cache_loc
  -> Transformer 读取两行 KV 映射
  -> last-rank 产生 logits
  -> sampler 返回 [token_a, token_b]
  -> GenerationBatchResult 交回 M04
```

## 2. 组件边界

| 组件 | 真实职责 | 证据 |
|---|---|---|
| `ScheduleBatch` | scheduler-side 请求、cache 和计划 | [`python/sglang/srt/managers/schedule_batch.py:2184-2417`] |
| `ForwardBatch` | 一次 forward 所需的 tensor/metadata 快照 | [`python/sglang/srt/model_executor/forward_batch_info.py:758-908`] |
| `TpModelWorker` | 构造 `ForwardBatch`，调用 runner，按 PP rank 组织结果 | [`python/sglang/srt/managers/tp_worker.py:593-703`] |
| `ModelRunner.forward` | attention backend、graph/eager、模型 forward 和输出包装 | [`python/sglang/srt/model_executor/model_runner.py:1612-1706`][`python/sglang/srt/model_executor/model_runner.py:1756-1805`] |
| `ModelRunner.sample` | grammar/logits bias、sampler、observer 和 decode side effect | [`python/sglang/srt/model_executor/model_runner.py:1884-1938`] |

M05 的出口不是用户文本，而是 `GenerationBatchResult`。M04 的 result processor 再负责将 token、finish、KV 和输出状态写回请求。[`python/sglang/srt/managers/scheduler.py:4548-4589`]

## 3. `ScheduleBatch` 到 `ForwardBatch`

### 3.1 为什么要复制一个 forward 对象？

`ScheduleBatch` 是可被 scheduler 在 overlap 逻辑中保存、恢复和更新的工作对象；`ForwardBatch` 是当前 worker forward 所需的 device 输入。二者不能简单混为一个可任意原地修改的对象。

`ForwardBatch.init_new` 明确声明不应修改输入 `ScheduleBatch`，随后读取 forward mode、batch size、`input_ids`、request pool indices、sequence lengths、cache locations、sampling metadata、LoRA ids 和 rids。[`python/sglang/srt/model_executor/forward_batch_info.py:758-861`]

**已确认**：对于 extend，`extend_lens`、`prefix_lens` 和 logprob 起点被放入 forward metadata；对于 decode/idle，这些 extend-only 字段置为 `None`。[`python/sglang/srt/model_executor/forward_batch_info.py:786-790`]

### 3.2 关键不变量

- `req_pool_indices` 指向 M08 的 request-to-token pool 行；
- `out_cache_loc` 指向这一轮要写入的 KV 位置；
- `seq_lens` 是当前 forward 使用的序列长度，不是任意 Python list 的展示值；
- `rids` 与 batch 请求顺序保持一致，用于后续输出关联；
- sampling info 与 grammar 信息必须和同一批请求对应；
- `init_new` 的 GPU tensor 创建可能使用 pinned CPU memory 和 non-blocking copy，不能在调试时假定所有复制都同步完成。[`python/sglang/srt/model_executor/forward_batch_info.py:865-900`]

## 4. TP worker 的执行分支

`forward_batch_generation` 先在 batch 存在时设置 HiCache consumer，再调用 `ForwardBatch.init_new`。[`python/sglang/srt/managers/tp_worker.py:593-623`]

随后存在三个决定性分支：

1. dLLM 专用执行路径；
2. PP last rank 运行 forward 并拥有 logits；
3. 非 last rank 运行 forward 并把 proxy tensors 交给下一个 PP stage。[`python/sglang/srt/managers/tp_worker.py:624-703`]

last rank 的普通 generation 路径：

```text
ModelRunner.forward
  -> logits_output + can_run_cuda_graph
  -> GenerationBatchResult
  -> 非 verify 且非 prefill-only 时 ModelRunner.sample
```

verify 模式会跳过 sampling，因为 speculative worker 有自己的后续发布逻辑。[`python/sglang/srt/managers/tp_worker.py:627-646`]

prefill-only 请求不产生正常 decode token；worker 创建与 batch size 对齐的 CPU/device dummy token，并在需要 input logprob 时调用 `compute_logprobs_only`。[`python/sglang/srt/managers/tp_worker.py:670-692`]

overlap + grammar 或 delay-sample 配置下，sampling 可以被包装成 `delay_sample_func`，让 scheduler 在结果处理时机执行，而不是立即同步完成。[`python/sglang/srt/managers/tp_worker.py:647-668`]

## 5. `ModelRunner.forward` 的真实工作

`forward` 为每一轮增加 `forward_pass_id`，建立 profiler/canary/expert recording 上下文，然后调用 `_forward_raw`，最后把专家统计等辅助输出挂到结果对象。[`python/sglang/srt/model_executor/model_runner.py:1612-1672`]

`_forward_raw` 首先决定是否已有 forward context；随后判断当前模式和 decode CUDA Graph runner 是否允许 graph replay。可以 replay 时直接执行 graph 并返回。[`python/sglang/srt/model_executor/model_runner.py:1756-1793`]

不能 graph replay 时，路径会：

1. 进行 DP/MLP-sync padding 和 attention-TP normalization；
2. 执行需要在 forward stream 上发生的 Mamba clear/COW；
3. 继续进入模型和 attention backend 的 eager forward。[`python/sglang/srt/model_executor/model_runner.py:1795-1805`]

**已确认**：CUDA Graph 并不是“设置了 decode 就必然使用”。它必须同时满足 forward mode、runner 存在以及 `can_run_graph(forward_batch)`；否则仍走 eager 路径。[`python/sglang/srt/model_executor/model_runner.py:1769-1793`]

## 6. sampling 如何产生 token

`ModelRunner.sample` 先清除可能由 CUDA graph backing object 残留的 auxiliary output，然后根据 observer 是否 active 选择 logits preprocessing。[`python/sglang/srt/model_executor/model_runner.py:1884-1913`]

preprocess 会更新 grammar vocab mask、应用 logits bias，并立即释放 grammar mask GPU tensor，避免 overlap 延迟闭包持有它造成显存持续增长。[`python/sglang/srt/model_executor/model_runner.py:1860-1882`]

真正 sampler 的位置接收：

- logits output；
- sampling info；
- 是否返回 logprob；
- top logprobs 数量；
- token ids logprob 请求；
- decode 使用 positions，prefill 使用 `seq_lens - 1` 的最后位置。[`python/sglang/srt/model_executor/model_runner.py:1915-1928`]

最后，observer 处理采样结果，ngram embedding manager 更新 decode 状态，并返回 `next_token_ids`。[`python/sglang/srt/model_executor/model_runner.py:1929-1938`]

## 7. 返回 M04 的边界

M05 返回的 `GenerationBatchResult` 仍可能包含 logits、next token ids、PP proxy、graph eligibility、expert/indexer auxiliary outputs。M04 的 `process_batch_result` 根据 `ForwardMode` 选择 decode、prefill、prebuilt 或 idle processor。[`python/sglang/srt/managers/tp_worker.py:627-703`][`python/sglang/srt/managers/scheduler.py:4548-4579`]

这条边界很重要：

```text
ModelRunner.sample
  -> token id tensor
  -> M04 result processor
  -> Req.output_ids / finish / output streamer
  -> M03 BatchStrOutput / ReqState
```

**未知/待补**：具体 model class 如何把 hidden states 变成 logits、attention backend 如何消费 KV layout、不同 quantization/model architecture 的 forward 差异，需在模型和 attention 专题中继续展开。

## 8. 调试断点

| 断点 | 观察内容 |
|---|---|
| `ForwardBatch.init_new:758-908` | batch fields 是否与请求顺序、mode 和 cache loc 对齐 |
| `TpModelWorker.forward_batch_generation:593-703` | PP rank、verify、prefill-only、delay sampling 分支 |
| `ModelRunner.forward:1612-1706` | forward pass id、output flags、辅助上下文 |
| `ModelRunner._forward_raw:1756-1805` | graph replay 条件、eager preparation |
| `ModelRunner.sample:1884-1938` | grammar mask、sampler 输入和 next token ids |
| `Scheduler.process_batch_result:4548-4589` | M05 输出如何交给 M04 processor |

## 9. 测试和验证边界

**已定位但未执行**：本篇没有运行 GPU、模型、CUDA Graph、PP 或 sampling 测试。静态源码只能确认分支和对象传递，不能证明某一模型的 logits 或 token 输出正确。

建议对照：

- `test/registered/unit/batch_overlap/`：overlap result/sampling 边界；
- scheduler retraction、chunked prefill 和 output 测试；
- 与具体模型、attention backend、quantization 相关的 registered tests。

## 10. 修改影响

修改 `ForwardBatch` 字段时，必须同步检查 `ScheduleBatch` preparation、`ForwardBatch.init_new`、worker 分支、attention backend metadata、CUDA Graph eligibility 和 result processor。修改 sampling 时还要检查 grammar mask、logprob、observer、delay sampling 和 output cleanup。

## 相关文档

- [M04 Scheduler 与连续批处理](../M04-scheduler-batching/README.md)
- [M07 分布式并行](../M07-分布式并行.md)
- [M08 KV Cache 与 Radix Cache](../M08-kv-cache/README.md)
- [跨模块调用链](../../90-cross-module/cross-module-call-chains.md)
- [D01 离线 Engine](../../80-demos/D01-offline-engine/01-离线批量推理.md)

## 源码证据摘要

- [`python/sglang/srt/managers/tp_worker.py:593-703`](../../../source/sglang/python/sglang/srt/managers/tp_worker.py)
- [`python/sglang/srt/model_executor/forward_batch_info.py:758-908`](../../../source/sglang/python/sglang/srt/model_executor/forward_batch_info.py)
- [`python/sglang/srt/model_executor/model_runner.py:1612-1706`](../../../source/sglang/python/sglang/srt/model_executor/model_runner.py)
- [`python/sglang/srt/model_executor/model_runner.py:1756-1805`](../../../source/sglang/python/sglang/srt/model_executor/model_runner.py)
- [`python/sglang/srt/model_executor/model_runner.py:1884-1938`](../../../source/sglang/python/sglang/srt/model_executor/model_runner.py)
- [`python/sglang/srt/managers/scheduler.py:4548-4589`](../../../source/sglang/python/sglang/srt/managers/scheduler.py)

## 深度审计

| 对象 | 入口 | 正常 | 分支 | 异常 | 清理 | 生命周期 | 执行上下文 | 行级证据 | Demo | 状态 |
|---|---|---|---|---|---|---|---|---|---|---|
| `ForwardBatch.init_new` | 已完成 | 已完成 | 部分完成 | 部分完成 | 部分完成 | 已完成 | 已完成 | 已完成 | D01 静态映射 | 部分：backend 细节待补 |
| TP worker | 已完成 | 已完成 | 已完成 | 部分完成 | 部分完成 | 已完成 | 已完成 | 已完成 | D01 静态映射 | 部分：PP/spec 分支待补 |
| `ModelRunner.forward/sample` | 已完成 | 已完成 | 已完成 | 部分完成 | 部分完成 | 已完成 | 已完成 | 已完成 | D01 静态映射 | 部分：模型层/attention 待补 |

## 未解决问题

- 具体模型 forward 和 logits processor 尚未逐模型追踪；
- attention backend 的 KV layout 和 graph capture 条件尚未独立成 M09；
- 真实 GPU 执行、采样和多卡通信未验证。
