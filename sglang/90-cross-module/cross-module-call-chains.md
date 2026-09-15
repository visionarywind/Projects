# 跨模块调用链

- 文档目的：把普通 Engine 请求从入口追踪到 scheduler、worker 和输出，并标注可选的 speculative、多模态、KV transfer 与 gateway 扩展，不停在 wrapper 名称。
- 对应源码版本：`78be4b50af88e9ea72d75b4c3a3e42b7297d2501`
- 证据状态：部分完成
- 最后更新：2026-09-10

## 主调用链

```text
Engine.generate
  -> TokenizerManager.generate_request
  -> _tokenize_one_request / _create_tokenized_object
  -> _send_one_request
  -> _dispatch_to_scheduler
  -> Scheduler.ingest_requests
  -> process_input_requests
  -> get_next_batch_to_run
  -> run_batch
  -> TpModelWorker.forward_batch_generation
  -> ModelRunner.forward / sample
  -> process_batch_result
  -> BatchStrOutput
  -> TokenizerManager._handle_batch_output
  -> ReqState.event
  -> _stream_one_response
```

**已确认**：M03 的 dispatch 通过 socket 把 tokenized object 交给 scheduler；M04 的 `run_batch` 再进入 worker，结果处理按 forward mode 分派。[`python/sglang/srt/managers/tokenizer_manager.py:1577-1650`][`python/sglang/srt/managers/scheduler.py:2050-2105`][`python/sglang/srt/managers/scheduler.py:4199-4382`][`python/sglang/srt/managers/scheduler.py:4548-4589`]

## 状态改变点

| 边界 | 真实改变 | 上下文 |
|---|---|---|
| M03 本地 | 创建 `rid_to_state[rid]` | asyncio/主进程 |
| M03→M04 | 请求进入 scheduler receiver/dispatcher | socket/子进程 |
| M04 | 请求获得 `Req`、waiting/running 归属和 batch index | scheduler 进程 |
| M04→M05 | `ScheduleBatch` 被准备成 forward 输入 | scheduler/worker |
| M05 | 产生 logits 或 `next_token_ids` | device/worker |
| M04 | 写回 token、finish 和 KV 状态 | scheduler 进程/device cache |
| M04→M03 | 产生按 rid 对齐的输出对象 | IPC |
| M03 | 更新 `ReqState`，完成时删除 state | asyncio/主进程 |

## 未解决问题

具体 request dispatcher handler 如何从 tokenized object 构造 `Req`，以及全部 result processor 如何更新 KV，需结合 M04/M05/M08 专题继续补证。本文未执行端到端服务。

## 相关文档

- [接口契约](interface-contracts.md)
- [共享数据与类型](shared-data-and-types.md)
- [运行轨迹](runtime-trace.md)
- [M04 Scheduler](../01-modules/M04-scheduler-batching/README.md)
