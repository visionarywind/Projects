# 跨模块调用链

- 文档目的：解释 90-cross-module/call-chains.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/llamacpp HEAD 093a2f86c（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 90-cross-module/call-chains.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


## 主链：D01 非流式 completion

```text
HTTP request
 -> server_routes::handle_completions_impl
 -> tokenize_input_prompts / process_mtmd_prompt
 -> server_task + server_res_generator
 -> server_queue
 -> server_context::update_slots / decode
 -> llama_decode
 -> llama_context::decode
 -> graph_compute
 -> ggml_backend_sched_graph_compute_async
 -> logits
 -> sampler/result
 -> server_response_reader
 -> JSON HTTP response
```

入口和关键边界：[tools/server/server-context.cpp:4257-4344] [tools/server/server-context.cpp:3645-3686] [src/llama-context.cpp:4247-4255] [src/llama-context.cpp:2492-2518] [tools/server/server-context.cpp:4348-4380]

## 模型准备链

```text
llama_model_load_from_file
 -> llama_model_load_from_file_impl
 -> llama_model_loader
 -> llama_model_create(loader)
 -> llama_model_<architecture>
 -> load_hparams / load_vocab / load_tensors
```

证据：[src/llama.cpp:316-377] [src/llama-model.cpp:343-369]

## 关键非直达点

- `llama_decode` 不是算法终点：真正执行在 context graph/scheduler/backend。
- `handle_completions_impl` 不是生成终点：task 通过 queue 才进入专用 context thread。
- `llama_model_create` 不是权重加载终点：loader 后续调用 hparams/vocab/tensors。

## 失败链

模型失败返回 null；context 参数错误返回 null；server decode 错误清 slot、清 prompt 并结束本轮；HTTP 异常包装为 400/500。[src/llama.cpp:380-444] [src/llama-context.cpp:3663-3765] [tools/server/server-context.cpp:3688-3725] [tools/server/server.cpp:52-85]

---

**页尾：** 动态 dispatch、模型 graph builder 和 backend kernel 的完整路径需按具体架构补证。

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
