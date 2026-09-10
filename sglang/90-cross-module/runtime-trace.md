# 统一运行轨迹

- 文档目的：用一个可回到源码的时间顺序连接启动和一次普通生成请求。
- 对应源码版本：`f1a512c51c73ab660cf41e1af3110c7c11e3b600`
- 证据状态：部分完成
- 最后更新：2026-09-10

## 结论摘要

普通 Engine 请求不是“调用模型函数后返回字符串”，而是主进程、scheduler 子进程、detokenizer 和 worker 之间的多段轨迹。启动先建立资源，request 再使用资源；`rid` 是主进程状态与批量返回之间的关联线。[`python/sglang/srt/entrypoints/engine.py:1051-1260`][`python/sglang/srt/managers/tokenizer_manager.py:776-845`]

## 启动轨迹

```text
D01 main
 -> Engine.__init__
 -> Engine._launch_subprocesses
 -> ServerArgs.resolve_once / publish
 -> PortArgs.init_new
 -> scheduler/detokenizer process
 -> run_scheduler_process
 -> Scheduler(...)
 -> model worker / ModelRunner / KV / backend
 -> ready pipe
 -> TokenizerManager usable
```

**已确认**：ready 发送位于 `Scheduler(...)` 构造之后，因此 process started 不等于 runtime ready。[`python/sglang/srt/managers/scheduler.py:5744-5833`]

## 请求轨迹

```text
Engine.generate
 -> GenerateReqInput
 -> TokenizerManager.generate_request
 -> _init_req_state
 -> _tokenize_one_request
 -> _create_tokenized_object
 -> _send_one_request
 -> sock_send
 -> scheduler ingest_requests
 -> get_next_batch_to_run
 -> run_batch
 -> TpModelWorker.forward_batch_generation
 -> ModelRunner.forward/sample
 -> process_batch_result
 -> output streamer/detokenizer
 -> TokenizerManager.handle_loop
 -> _handle_batch_output
 -> state.event
 -> _stream_one_response
 -> output['text']
```

## 跨上下文切换

| 边界 | 前后上下文 | 传递物 | 可观察证据 |
|---|---|---|---|
| Engine→TokenizerManager | 同一主进程同步/async | `GenerateReqInput` | `Engine.generate` |
| TokenizerManager→Scheduler | ZMQ IPC | tokenized request | `_dispatch_to_scheduler` |
| Scheduler→worker | scheduler/worker execution | `ScheduleBatch` | `run_batch` |
| worker→device | device stream/torch execution | tensors/logits | `ModelRunner.forward` |
| Scheduler→Detokenizer | IPC | token ids/output | output streamer |
| Detokenizer→TokenizerManager | ZMQ IPC | `BatchStrOutput` | `handle_loop` |
| state→caller | asyncio event/generator | dict | `_stream_one_response` |

## 相关文档

- [系统 wiring](system-wiring.md)
- [端到端流程](end-to-end-flows.md)
- [D01 深度文章](../80-demos/D01-offline-engine/01-离线批量推理.md)

## 源码证据摘要

- [`python/sglang/srt/entrypoints/engine.py:383-491`](../../python/sglang/srt/entrypoints/engine.py)
- [`python/sglang/srt/managers/scheduler.py:1839-1925`](../../python/sglang/srt/managers/scheduler.py)
- [`python/sglang/srt/managers/scheduler.py:4199-4209`](../../python/sglang/srt/managers/scheduler.py)
- [`python/sglang/srt/managers/tp_worker.py:593-704`](../../python/sglang/srt/managers/tp_worker.py)

## 未解决问题

Overlapping scheduler、PP、PDMUX、disaggregation 会插入不同事件循环和队列；本轨迹是普通路径的统一基线，不代表每个变体的精确调用顺序。

## 下一步阅读建议

用 D01 的步骤表逐步对照本轨迹，然后在 M04/M05 文章中展开 `run_batch` 内部状态变化。
