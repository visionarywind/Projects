# 调试指南

- 文档目的：解释 99-roadmap/debugging-guide.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/sglang HEAD 78be4b50af（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 99-roadmap/debugging-guide.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


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

## 相关文档
- [项目入口](../README.md)
- [分析状态](../00-overview/analysis-state.md)
- [源码证据索引](../00-overview/evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](../00-overview/analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
