# 跨模块调用链

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
