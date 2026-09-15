# M10 采样与约束输出

- 文档目的：解释 logits 如何经过 penalty、logit bias、grammar mask 和 sampler，形成 `next_token_ids`，以及这些状态如何随 batch 合并、过滤和完成而变化。
- 适用范围：`SamplingParams`、`SamplingBatchInfo`、grammar backend 接口、`ModelRunner.sample`、logprob/top-k/top-p 和 custom logit processor。
- 对应源码版本：`78be4b50af88e9ea72d75b4c3a3e42b7297d2501`
- 证据状态：部分完成
- 最后更新：2026-09-10
- 前置阅读：[M05 模型执行](../M05-model-execution/README.md)、[M04 Scheduler 与连续批处理](../M04-scheduler-batching/README.md)
- 后续阅读：[M03 Tokenizer 与请求状态](../M03-tokenizer-request-state/README.md)、[D01 离线 Engine](../../80-demos/D01-offline-engine/01-离线批量推理.md)
## 结论摘要

本页聚焦 01-modules/M10-sampling-constraints/README.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


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

### 6.1 编译缓存、异步等待和失败传播

`BaseGrammarBackend` 在初始化时创建 `ThreadPoolExecutor` 和按 `(key_type, key_string)` 索引的 cache。请求首次到达时，`get_cached_or_future_value` 不同步阻塞编译，而是提交 `_init_value_dispatch` future；命中缓存时返回 `value.copy()`，让每个请求拥有独立 matcher 状态，同时把 `GrammarStats.is_cache_hit` 标记为 true。[`python/sglang/srt/constrained/base_grammar_backend.py:201-206`](../../../source/sglang/python/sglang/srt/constrained/base_grammar_backend.py)[`python/sglang/srt/constrained/base_grammar_backend.py:258-298`](../../../source/sglang/python/sglang/srt/constrained/base_grammar_backend.py)

`GrammarManager.process_req_with_grammar` 把 future 放入 `grammar_queue`；PP0 周期性轮询 `Future.done()`，超过 `SGLANG_GRAMMAR_MAX_POLL_ITERATIONS` 的请求进入 failed 集合，再在 DP/TP 组内取 ready 的交集、failed 的并集，并通过 PP group 向后续 stage 传播。这样只有所有相关 rank 都准备好的 grammar 才能进入 waiting queue，而某一 rank 编译失败不会让其他 rank 单独继续。[`python/sglang/srt/constrained/grammar_manager.py:145-196`](../../../source/sglang/python/sglang/srt/constrained/grammar_manager.py)[`python/sglang/srt/constrained/grammar_manager.py:198-243`](../../../source/sglang/python/sglang/srt/constrained/grammar_manager.py)

完成 future 的 `.result()` 若抛异常，会被包装成 `InvalidGrammarObject`，并写入 backend cache；缓存命中的无效对象和本轮编译得到的无效对象都会调用 `req.set_finish_with_abort`，把编译错误转成请求级失败，而不是把异常留在 sampler 内部。[`python/sglang/srt/constrained/grammar_manager.py:1-243`](../../../source/sglang/python/sglang/srt/constrained/grammar_manager.py)

XGrammar 的 dispatch 分别调用 builtin/JSON schema、EBNF、regex 和 structural-tag compiler；编译异常转成 `InvalidGrammarObject`，结构化 tag 还会先兼容 legacy 格式并把缺失 schema 规范化为空 schema。backend reset 同时清空 SGLang cache 和 xgrammar compiler cache。[`python/sglang/srt/constrained/xgrammar_backend.py:336-402`](../../../source/sglang/python/sglang/srt/constrained/xgrammar_backend.py)

NUL 字节在进入 backend dispatch 前由基类递归检查 JSON/structural-tag 内容并直接拒绝，这是对上游 regex converter 崩溃风险的输入防护；它与普通 schema 语法错误一样最终表现为 `InvalidGrammarObject`，但错误来源不同，排查时应先看 key 类型和原始请求。[`python/sglang/srt/constrained/base_grammar_backend.py:161-187`](../../../source/sglang/python/sglang/srt/constrained/base_grammar_backend.py)

## 7. sampler backend、数值路径与 TP 同步

`Sampler.forward` 先执行 custom logit processor 和 NaN/Inf 清理；全 greedy batch 直接 `argmax`，否则先按 temperature 缩放，再根据执行配置选择 Ascend logits 路径、RL on-policy 的 log-softmax 路径，或标准 softmax 后的概率采样路径。[`python/sglang/srt/layers/sampler.py:114-260`]

标准概率路径按请求是否需要 top-k/top-p/min-p 分为简单和复杂情况：简单情况使用 `sampling_from_probs_torch`；复杂情况按 `flashinfer` 或 `pytorch` backend 选择对应实现。PyTorch fallback 先排序，再按 top-k 和累计 top-p 置零，必要时按最大保留概率乘 `min_p` 过滤，最后 `torch.multinomial` 或带 seed 的 `multinomial_with_seed` 取样。[`python/sglang/srt/layers/sampler.py:299-445`][`python/sglang/srt/layers/sampler.py:717-770`]

```text
logits
  -> custom processor / finite-value sanitize
  -> temperature
  -> softmax（标准路径）或 backend fused path
  -> top-k
  -> top-p
  -> min-p
  -> multinomial / deterministic seeded sampling
  -> token id
```

采样 seed 不是所有 backend 都支持：flashinfer 复杂采样路径要求 seed 为空；PyTorch 的带 seed 路径把过滤后的概率转成 float64 log-probability 后调用 `multinomial_with_seed`，并明确承认过滤后未重新归一化的数值限制。[`python/sglang/srt/layers/sampler.py:357-366`][`python/sglang/srt/layers/sampler.py:745-764`]

`SamplingBatchInfo` 还可能捕获 sampling mask 供 logprob/观测路径使用。`_sample_from_probs` 只为显式请求 mask 的行建立 `capture_rows`，并根据简单采样、FlashInfer 或 PyTorch 路径保存过滤后的权重、token ids 和 selected weight；没有任何 opted-in 行时会直接报错，避免产生含糊的全 batch mask。[`python/sglang/srt/layers/sampler.py:299-458`]。greedy 路径则把选中的 token 作为唯一支持项并记录 logprob 0；这些 CPU 输出随后由 observer 或结果处理消费。[`python/sglang/srt/layers/sampler.py:460-474`]

sampling mask 不是 grammar mask 的别名：前者描述实际采样分布/观测结果，后者描述语法允许集合。两者都要求 batch 行顺序稳定，但生命周期不同；`_preprocess_logits` 清空 grammar mask 并不代表采样 mask 已经完成传递。

## 8. batch 合并、过滤和 overlap

continuous batching 会加入新请求、移除完成请求或 retraction 请求。`SamplingBatchInfo.filter_batch` 要同步过滤 penalty orchestrator、custom processor、temperature/top-p/top-k/min-p、seed、logit bias 和 return mask；custom processor 的 mask 全 False 时还会清除对应 processor。[`python/sglang/srt/sampling/sampling_batch_info.py:318-362`]

在 overlap 下，sampling 可能被包装为 delay closure。该 closure 捕获 logits/forward batch，因此 scheduler 处理完成后必须释放 closure 和 logits tensor；否则 grammar mask 和 logits 会延迟释放。[`python/sglang/srt/managers/tp_worker.py:1-564`][`python/sglang/srt/managers/scheduler.py:1-4005`]

## 9. 停止和输出的边界

sampler 只产生 token id。是否遇到 EOS、stop token、stop string、最大新 token、grammar termination，以及如何把 token 转成输出文本，要由 M04 result processing、请求状态和 M03 detokenization/streaming 协作完成。

因此新增一种停止条件时，至少检查：

- `SamplingParams` normalize/verify；
- scheduler-side `Req` 的 finish 判断；
- grammar 状态是否 rollback/terminate；
- `BatchStrOutput.finished_reasons`；
- M03 `ReqState` 删除与最终输出。

## 10. 调试断点

| 断点 | 观察内容 |
|---|---|
| `SamplingBatchInfo.from_schedule_batch:86-129` | 参数 tensor 是否与 req 顺序一致 |
| `update_regex_vocab_mask:234-259` | grammar 行、mask shape、device |
| `apply_logits_bias:295-301` | penalty、mask、bias 的顺序 |
| `ModelRunner.sample:1884-1938` | preprocess、sampler 输入和 next ids |
| `filter_batch:318-362` | 完成/撤回后所有采样字段是否同步 |
| `GrammarManager.get_ready_grammar_requests:198-304` | future 轮询、DP/TP/PP 同步、超时和 InvalidGrammarObject |
| `Scheduler` delay cleanup | closure、logits 和 grammar mask 是否释放 |

## 11. 验证边界

**已定位但未执行**：sampling、grammar、logprob、custom processor 和 stop condition 测试均未在本批执行；grammar cache/future、PP/DP/TP 同步和编译失败的请求级传播已由静态源码确认，但没有真实模型输出证据，不能声称具体 token 序列正确。

## 12. 相关文档

- [M05 模型执行](../M05-model-execution/README.md)
- [M04 Scheduler 与连续批处理](../M04-scheduler-batching/README.md)
- [M03 Tokenizer 与请求状态](../M03-tokenizer-request-state/README.md)
- [M09 Attention 与 CUDA Graph](../M09-attention-cuda-graph/README.md)
- [接口契约](../../90-cross-module/interface-contracts.md)

## 13. 源码证据摘要

- [`python/sglang/srt/sampling/sampling_params.py:114-130`](../../../source/sglang/python/sglang/srt/sampling/sampling_params.py)
- [`python/sglang/srt/sampling/sampling_batch_info.py:30-129`](../../../source/sglang/python/sglang/srt/sampling/sampling_batch_info.py)
- [`python/sglang/srt/model_executor/model_runner.py:1856-1938`](../../../source/sglang/python/sglang/srt/model_executor/model_runner.py)
- [`python/sglang/srt/constrained/xgrammar_backend.py:320-402`](../../../source/sglang/python/sglang/srt/constrained/xgrammar_backend.py)
- [`python/sglang/srt/constrained/grammar_manager.py:145-243`](../../../source/sglang/python/sglang/srt/constrained/grammar_manager.py)
- [`python/sglang/srt/constrained/base_grammar_backend.py:161-298`](../../../source/sglang/python/sglang/srt/constrained/base_grammar_backend.py)
- [`python/sglang/srt/managers/scheduler.py:1-4005`](../../../source/sglang/python/sglang/srt/managers/scheduler.py)
- [`python/sglang/srt/layers/sampler.py:96-297`](../../../source/sglang/python/sglang/srt/layers/sampler.py)
- [`python/sglang/srt/layers/sampler.py:299-458`](../../../source/sglang/python/sglang/srt/layers/sampler.py)
- [`python/sglang/srt/layers/sampler.py:460-474`](../../../source/sglang/python/sglang/srt/layers/sampler.py)
- [`python/sglang/srt/layers/sampler.py:746-770`](../../../source/sglang/python/sglang/srt/layers/sampler.py)

## 14. 深度审计

| 对象 | 入口 | 正常 | 分支 | 异常 | 清理 | 生命周期 | 执行上下文 | 行级证据 | Demo | 状态 |
|---|---|---|---|---|---|---|---|---|---|---|
| sampling batch info | 已完成 | 已完成 | merge/filter 部分完成 | 部分完成 | 部分完成 | 已完成 | device/CPU | 已完成 | D01 静态映射 | sampler backend/mask 已补 |
| grammar mask | 已完成 | 已完成 | finished/terminated/overlap 已覆盖 | 部分完成 | 已完成 | 部分完成 | device/overlap | 已完成 | D01 未使用 grammar | 需结构化输出 Demo |
| grammar compilation lifecycle | 已完成 | 已完成 | cache/future/DP-TP-PP sync 已覆盖 | timeout/invalid 已覆盖 | reset/cache clear 已覆盖 | 已完成 | CPU threads/PP groups | 已完成 | D01 未使用 grammar | 需结构化输出 Demo |
| sampler/result boundary | 已完成 | 已完成 | prefill/decode/logprob 已覆盖 | 部分完成 | 部分完成 | 已完成 | worker/device/scheduler | 已完成 | D01 静态映射 | stop/output 需 M03/M04 联查 |

## 15. 未解决问题

- 各专用 sampler backend 的 kernel 细节、性能差异和全部数值边界尚未逐一展开；
- 不同 grammar backend（Outlines/LLGuidance）的实现差异尚未逐一展开；
- 真实约束输出、logprob 和 custom processor 未验证。

## 深度审计

| 分析对象 | 入口落地 | 正常路径 | 分支 | 异常 | 清理 | 数据生命周期 | 执行上下文 | 行级证据 | Demo 映射 | 状态/缺口 |
|---|---|---|---|---|---|---|---|---|---|---|
| sglang/01-modules/M10-sampling-constraints/README.md | 已定位 | 已追踪代表路径 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 已标注 | 已引用或待补 | 已映射或无专用 Demo | 部分完成：动态构建、运行和硬件边界仍未验证 |

## 相关文档
- [项目入口](../../README.md)
- [分析状态](../../00-overview/analysis-state.md)
- [源码证据索引](../../00-overview/evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](../../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](../../00-overview/analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
