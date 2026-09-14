# M11 Speculative Decoding

- 文档目的：解释 draft proposal 与 target verification 如何在普通 scheduler/forward 链上形成可回滚的候选 token。
- 源码证据：`/home/mtuser/workspace/repos/Github/sglang` 当前 checkout。
- 证据状态：接口、数据结构和静态控制流已确认；实际接受率、GPU graph 和端到端输出未验证。
- 关联模块：[M04 Scheduler](../M04-scheduler-batching/README.md)、[M05 Model Execution](../M05-model-execution/README.md)、[M08 KV Cache](../M08-kv-cache/README.md)、[M10 Sampling](../M10-sampling-constraints/README.md)。

## 1. 核心边界

Speculative decoding 不是“多生成几次再拼接”，而是 draft 先提出候选，target 一次验证候选前缀，依据 target/draft 分布接受连续 token，并处理 rejected suffix 与 bonus token。目标是减少 target forward 次数，同时保持 target sampling 语义。

```text
ScheduleBatch decode
  -> BaseSpecWorker / algorithm-specific draft worker
  -> SpecInput / draft candidates
  -> target ragged verify
  -> accept/reject + bonus token
  -> commit accepted KV / retract rejected suffix
  -> Req output_ids / next schedule
```

## 2. 抽象对象

`python/sglang/srt/speculative/base_spec_worker.py:29-350`：

- `HiCacheDraftMode`、`HiCacheDraftPlan` 描述 draft 与 HiCache 的兼容策略；
- `EagleDraftWorkerBase` 暴露 draft runner、memory pool、attention backend、CUDA graph、prewarm 和统计接口；
- `BaseSpecWorker` 持有 target worker、可选 draft worker、draft KV pool、spec v2 attention backend，并提供 `alloc_memory_pool`、`init_attention_backends`、`init_cuda_graphs`、`on_verify_complete_cpu`、`note_request_finished` 等边界。

算法专用输入继承 `SpecInput`，例如 `DFlashVerifyInput`（`dflash_info.py:26-139`）和 `DFlashDraftInputV2`（`dflash_info_v2.py:36-268`）。输入通常携带候选 token、序列长度、attention metadata、cache locations 和过滤/merge 所需 host/device buffer。

Decoupled spec IPC 的消息契约位于 `decoupled_spec_io.py:8-381`：`DraftReqKey` 标识请求，`DraftSync`/`VerifyCommit`/`DraftClose` 传递同步、提交和关闭事件；`VerifierCommitSegment` 保证 committed prefix 的顺序。

## 3. 主链：draft → verify → commit

```text
scheduler forward path
  -> spec worker forward_batch_generation
  -> draft model proposes k tokens
  -> target ModelRunner verifies k-token block
  -> compute correct drafts and bonus
  -> accept length
  -> append accepted tokens
  -> retract rejected cache / close request if finished
```

DFlash 代表链的静态证据：`dflash_worker_v2.py:336-...` 构造 draft worker；`:1203-1328` 负责 selector/greedy proposal；`:1824-1958` 准备 accept/bonus buffers；`:1988-2004` 构造下一轮输入；`:2004-...` 执行 forward generation。`dflash_utils.py:833-1093` 提供 correct-drafts/bonus 计算和 probability 处理，`:1163-1168` 校验请求兼容性。

接受后的关键不变量：

- accepted tokens 才能写入请求可见输出；
- target KV 对 accepted prefix 可提交；
- rejected suffix 不得继续占用 request 的有效 cache length；
- bonus token 只在 target 验证语义允许时提交；
- next input 的 `seq_lens`、cache locations 和 sampling metadata 必须与新的 committed length 对齐。

## 4. 算法注册和差异

当前 checkout 包含 EAGLE/MTP、ngram、DFlash、DSpark、UNO、standalone/decoupled 等路径。文档采用接口优先，而不是逐算法复制：

| 维度 | 可能替换点 |
|---|---|
| proposal | draft Transformer、MTP head、ngram corpus、DFlash selector |
| verify input | 普通 ragged verify、专用 attention metadata、decoupled IPC |
| acceptance | greedy、rejection sampling、grammar-aware mask |
| KV ownership | 同 worker pool、draft 独立 pool、HiCache staging |
| execution | eager、overlap、CUDA Graph、adaptive steps |

`adaptive_runtime_state.py:11-147` 的 `SpecRuntimeState`/`AdaptiveController` 根据 batch size 和 verify 结果选择 candidate steps，并可为每个 step 配置 CUDA graph batch size。它改变的是下一轮运行策略，不改变 target correctness contract。

## 5. grammar、overlap 与 graph 边界

候选 token 必须服从请求 sampling/grammar 约束；grammar mask 可能使 draft 候选无效，target verify 需要在正确的 token domain 上比较。overlap 模式下 draft/target future 的完成顺序不能破坏 commit 顺序；失败时必须取消 future、清除 draft buffers 并恢复 scheduler 可见状态。ragged verify 和不同候选长度会使固定 batch CUDA Graph 不可用或需要专用 bucket，不能从开启 speculative flag 推断 graph 必定使用。

## 6. 生命周期

### 初始化

按 server args 解析 speculative algorithm；创建 draft model/runner 或 ngram corpus；分配 draft KV pool、attention backend、sampling buffers；可选 capture/prewarm。

### 正常运行

scheduler 提交 decode batch；spec worker 生成 proposal；target worker 验证；CPU/device acceptance 结果更新 `Req`、sampling 和 KV ownership；下一轮继续。

### 异常

请求不支持某采样参数、grammar 或模型架构时由 `validate_*` 拒绝；draft/target forward、future、IPC 或 transfer 失败时禁止部分 commit；按请求撤销 draft state，并将错误传播至 M03/M02。

### 清理

请求自然结束调用 `note_request_finished` 并清理 corpus/match state、draft pool 和 graph buffers 的 per-request 部分；server shutdown 关闭 draft runners、IPC channel 和 device resources。

## 7. 小例子和断点

例：k=4 的 greedy decode：draft 给出 `[a,b,c,d]`，target 验证前缀，在 `b` 处不匹配：提交 `a`，按 target 规则处理 `b` 的替代 token，丢弃 `c,d`，下一轮从新 committed length 开始。具体 bonus/rejection 取决于算法和 sampling 模式。

断点：

- `BaseSpecWorker.on_verify_complete_cpu`：观察 verify 结果进入请求状态的位置；
- `dflash_worker_v2.py:_accept_block`：观察 accepted length、bonus 和 cache；
- `dflash_utils.py:compute_dflash_correct_drafts_and_bonus`：核对接受计算；
- `adaptive_runtime_state.py:AdaptiveController.on_verify_complete`：观察 steps 调整；
- `decoupled_spec_io.py:VerifyCommit.validate_committed_tokens`：检查跨进程提交顺序。

## 8. 测试和未验证

- `test/manual/test_aiter_unified_draft_extend_env.py`；
- `test/manual/test_kda_spec_integration.py`、`test/manual/test_kda_target_verify.py`；
- `test/manual/test_kv_events.py`、`test/manual/test_mori_transfer_engine_e2e.py`：与 KV/transfer 交叉；
- `benchmark/bench_adaptive_speculative.py`、`benchmark/uno/`：性能或评估脚本。

本次未执行 draft/target GPU forward、真实 acceptance rate、CUDA Graph、grammar/speculative 组合、多进程 decoupled 或跨机 transfer。

## 9. 修改影响

修改 acceptance 或 committed length 会直接影响 M04 result processing、M08 cache ownership、M10 sampling/logprob 和 M03 output state。修改 draft pool/IPC 会影响 M15；修改 graph eligibility 会影响 M09；修改 adaptive policy 会改变性能与资源峰值但必须保持 target distribution correctness。

**已确认**：speculation 的核心是可提交/可回滚的验证协议。**推断**：不同算法共享上述接口但可能拥有不同 KV/graph 路径。**未验证**：硬件运行和数值正确性。
