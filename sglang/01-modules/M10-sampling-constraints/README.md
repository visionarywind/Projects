# M10 采样与约束输出

- 文档目的：解释 logits 如何经过 penalty、logit bias、grammar mask 和 sampler，形成 `next_token_ids`，以及这些状态如何随 batch 合并、过滤和完成而变化。
- 适用范围：`SamplingParams`、`SamplingBatchInfo`、grammar backend 接口、`ModelRunner.sample`、logprob/top-k/top-p 和 custom logit processor。
- 对应源码版本：`f1a512c51c73ab660cf41e1af3110c7c11e3b600`
- 证据状态：部分完成
- 最后更新：2026-09-10
- 前置阅读：[M05 模型执行](../M05-model-execution/README.md)、[M04 Scheduler 与连续批处理](../M04-scheduler-batching/README.md)
- 后续阅读：[M03 Tokenizer 与请求状态](../M03-tokenizer-request-state/README.md)、[D01 离线 Engine](../../80-demos/D01-offline-engine/01-离线批量推理.md)

## 1. 先用一个小例子理解

模型给出一行 vocabulary logits：

```text
原始 logits
  -> repetition/presence/frequency penalty
  -> grammar mask（结构化输出时禁止非法 token）
  -> logit bias/custom processor
  -> temperature/top-k/top-p/min-p sampler
  -> token id
```

**通俗解释**：模型只说“每个 token 有多合适”，采样层还要根据请求参数和当前生成状态决定哪些 token 可选、怎样随机选择。约束输出不是生成后再修正文案，而是在 logits 上把非法 token 屏蔽掉。

## 2. 请求参数到 batch tensors

`SamplingParams` 是 msgspec 结构，包含 `max_new_tokens`、stop、temperature、top-k/top-p 等 API 参数；tokenizer manager 在创建 tokenized object 时 normalize/verify 参数。[`python/sglang/srt/sampling/sampling_params.py:114-130`][`python/sglang/srt/managers/tokenizer_manager.py:1369-1383`]

`SamplingBatchInfo.from_schedule_batch` 把每个 `Req` 的 temperature、top-p、top-k、min-p 和 seed 变成 device tensors；这些 tensor 的第 i 行必须对应 batch 第 i 个请求。[`python/sglang/srt/sampling/sampling_batch_info.py:86-129`]

因此 batch 合并和过滤不能只改变 `reqs` 列表，还要同步过滤/拼接所有采样字段。

## 3. `SamplingBatchInfo` 的状态

核心字段分为四类：

- 基本采样参数：temperatures、top_ps、top_ks、min_ps；
- 约束状态：vocab size、grammars、grammar mask；
- penalty/custom processor：penalizer orchestrator、累积 penalty、custom params/processor；
- 输出和设备：sampling seed、return sampling masks、device、logit bias。[`python/sglang/srt/sampling/sampling_batch_info.py:30-85`]

### 3.1 grammar mask

`update_regex_vocab_mask` 找到 batch 中的 grammar，申请 vocab mask，为仍需约束的行填充 mask，再把 mask 移到 device 并包装成 `GrammarMask`。[`python/sglang/srt/sampling/sampling_batch_info.py:234-259`]

finished、terminated 或没有 grammar 的行不会被填充；但新分配的 buffer 对这些行不能被误解为自动合法，后续逻辑必须保持行对齐和明确语义。

### 3.2 penalty

`update_penalties` 在需要时分配 additive penalty tensor，并通过 orchestrator 累积 additive/scaling penalty；不需要时把字段设为 `None`。[`python/sglang/srt/sampling/sampling_batch_info.py:261-276`]

## 4. logits 变换顺序

`apply_logits_bias` 的顺序是：

```text
pre-grammar transforms
  -> grammar mask
  -> post-grammar logit bias
```

pre 阶段可能包含 additive/scaling penalty 或非 overlap 的 penalizer；post 阶段加入 logit bias。[`python/sglang/srt/sampling/sampling_batch_info.py:278-301`]

`ModelRunner._preprocess_logits` 先更新 regex vocab mask，再调用带 observer 或普通的 `apply_logits_bias`；应用后立即把 `grammar_mask` 清空，避免 overlap 的延迟闭包持有大 GPU tensor。[`python/sglang/srt/model_executor/model_runner.py:1856-1882`]

**已确认**：sampling 是 device logits 的原地/近原地变换阶段，不能把它当作纯 Python 后处理；mask、penalty 和 batch 行顺序都属于执行正确性的一部分。

## 5. 采样调用

`ModelRunner.sample` 把 logits、sampling info、logprob 选项、top-logprobs 数量、token-logprob 请求和位置传给 `self.sampler`。decode 用 `forward_batch.positions`；prefill 使用每条序列最后位置 `seq_lens - 1`。[`python/sglang/srt/model_executor/model_runner.py:1884-1928`]

sampler 返回 `next_token_ids`；若 observer active，observer 还会得到 sampling 后的辅助输出；随后 ngram embedding manager 更新 decode 状态。[`python/sglang/srt/model_executor/model_runner.py:1929-1938`]

## 6. grammar backend 的边界

grammar manager/不同 grammar backend 负责把 JSON schema、regex、structural tag 等请求编译成 grammar object，并提供 vocab mask 相关操作；sampling batch 只依赖统一的 `allocate_vocab_mask`、`fill_vocab_mask_batched`、`move_vocab_mask` 和 mask apply 约定。[`python/sglang/srt/constrained/xgrammar_backend.py:320-402`][`python/sglang/srt/sampling/sampling_batch_info.py:234-259`]

这是一条重要的模块边界：

```text
用户结构化输出请求
  -> M03 normalize/tokenized fields
  -> scheduler Req.grammar
  -> SamplingBatchInfo.grammars
  -> grammar backend 生成 vocab mask
  -> logits mask
  -> sampler
```

grammar backend 的具体编译实现可替换，但必须保持 batch 行、device 和生命周期契约。

## 7. batch 合并、过滤和 overlap

continuous batching 会加入新请求、移除完成请求或 retraction 请求。`SamplingBatchInfo.filter_batch` 要同步过滤 penalty orchestrator、custom processor、temperature/top-p/top-k/min-p、seed、logit bias 和 return mask；custom processor 的 mask 全 False 时还会清除对应 processor。[`python/sglang/srt/sampling/sampling_batch_info.py:318-362`]

在 overlap 下，sampling 可能被包装为 delay closure。该 closure 捕获 logits/forward batch，因此 scheduler 处理完成后必须释放 closure 和 logits tensor；否则 grammar mask 和 logits 会延迟释放。[`python/sglang/srt/managers/tp_worker.py:647-668`][`python/sglang/srt/managers/scheduler.py:4537-4546`]

## 8. 停止和输出的边界

sampler 只产生 token id。是否遇到 EOS、stop token、stop string、最大新 token、grammar termination，以及如何把 token 转成输出文本，要由 M04 result processing、请求状态和 M03 detokenization/streaming 协作完成。

因此新增一种停止条件时，至少检查：

- `SamplingParams` normalize/verify；
- scheduler-side `Req` 的 finish 判断；
- grammar 状态是否 rollback/terminate；
- `BatchStrOutput.finished_reasons`；
- M03 `ReqState` 删除与最终输出。

## 9. 调试断点

| 断点 | 观察内容 |
|---|---|
| `SamplingBatchInfo.from_schedule_batch:86-129` | 参数 tensor 是否与 req 顺序一致 |
| `update_regex_vocab_mask:234-259` | grammar 行、mask shape、device |
| `apply_logits_bias:295-301` | penalty、mask、bias 的顺序 |
| `ModelRunner.sample:1884-1938` | preprocess、sampler 输入和 next ids |
| `filter_batch:318-362` | 完成/撤回后所有采样字段是否同步 |
| `Scheduler` delay cleanup | closure、logits 和 grammar mask 是否释放 |

## 10. 验证边界

**已定位但未执行**：sampling、grammar、logprob、custom processor 和 stop condition 测试均未在本批执行；没有真实模型输出证据，不能声称具体 token 序列正确。

## 相关文档

- [M05 模型执行](../M05-model-execution/README.md)
- [M04 Scheduler 与连续批处理](../M04-scheduler-batching/README.md)
- [M03 Tokenizer 与请求状态](../M03-tokenizer-request-state/README.md)
- [M09 Attention 与 CUDA Graph](../M09-attention-cuda-graph/README.md)
- [接口契约](../../90-cross-module/interface-contracts.md)

## 源码证据摘要

- [`python/sglang/srt/sampling/sampling_params.py:114-130`](../../../python/sglang/srt/sampling/sampling_params.py)
- [`python/sglang/srt/sampling/sampling_batch_info.py:30-129`](../../../python/sglang/srt/sampling/sampling_batch_info.py)
- [`python/sglang/srt/sampling/sampling_batch_info.py:234-362`](../../../python/sglang/srt/sampling/sampling_batch_info.py)
- [`python/sglang/srt/model_executor/model_runner.py:1856-1938`](../../../python/sglang/srt/model_executor/model_runner.py)
- [`python/sglang/srt/constrained/xgrammar_backend.py:320-402`](../../../python/sglang/srt/constrained/xgrammar_backend.py)
- [`python/sglang/srt/managers/scheduler.py:4537-4546`](../../../python/sglang/srt/managers/scheduler.py)

## 深度审计

| 对象 | 入口 | 正常 | 分支 | 异常 | 清理 | 生命周期 | 执行上下文 | 行级证据 | Demo | 状态 |
|---|---|---|---|---|---|---|---|---|---|---|
| sampling batch info | 已完成 | 已完成 | merge/filter 部分完成 | 部分完成 | 部分完成 | 已完成 | device/CPU | 已完成 | D01 静态映射 | 部分：sampler kernel 待补 |
| grammar mask | 已完成 | 已完成 | finished/terminated/overlap 已覆盖 | 部分完成 | 已完成 | 部分完成 | device/overlap | 已完成 | D01 未使用 grammar | 需结构化输出 Demo |
| sampler/result boundary | 已完成 | 已完成 | prefill/decode/logprob 已覆盖 | 部分完成 | 部分完成 | 已完成 | worker/device/scheduler | 已完成 | D01 静态映射 | stop/output 需 M03/M04 联查 |

## 未解决问题

- 具体 sampler kernel 的 top-k/top-p/min-p 算法尚未展开；
- grammar compiler 的缓存、异步编译和错误响应尚未完整追踪；
- 真实约束输出、logprob 和 custom processor 未验证。
