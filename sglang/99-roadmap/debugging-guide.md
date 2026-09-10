# 调试指南

## 请求没有输出

按顺序观察：

1. M03 `rid_to_state` 是否创建；
2. tokenization 是否完成，sampling params 是否 verify；
3. `_send_one_request` 是否 dispatch，dispatch 后失败是否发送 abort；
4. scheduler `ingest_requests` 是否收到对象；
5. `handle_generate_request` 是否构造 `Req` 并加入 queue；
6. `get_next_batch_to_run` 是 prefill、decode、idle 还是被 budget 拒绝；
7. `ScheduleBatch` 的 `forward_mode`、`seq_lens`、`out_cache_loc`；
8. worker 是否返回 `GenerationBatchResult`；
9. result processor 是否产生 `BatchStrOutput`；
10. M03 是否按 rid 找到 state 并 `event.set()`。

## 显存不足

同时记录 `PrefillAdder` 的 token/page budget、allocator capacity、running batch、retraction order、host backup 和 abort reason。不要只看 CUDA OOM 文本；很多资源不足会先变成 request-level retraction。

## 输出错位

优先检查 batch 顺序和 `rids[i]` 对齐，再检查 `SamplingBatchInfo.filter_batch`、完成请求移除、overlap result queue 和 PP rank。输出错位通常不是 detokenizer 单点问题。

## Graph 没有使用

检查 forward mode、runner 是否存在、`can_run_graph` 的 batch/width/embedding/DP 条件，以及是否回退到 eager。回退本身可能是预期行为。

## 调试断点

主断点见 M03、M04、M05、M08、M09、M10 各文档的表格。真实 GPU 调试命令和 profiler 在本批未执行。
