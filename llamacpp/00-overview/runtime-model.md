# 运行模型

- 文档目的：解释 00-overview/runtime-model.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/llamacpp HEAD 093a2f86c（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](analysis-state.md)。
## 结论摘要

本页聚焦 00-overview/runtime-model.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


> 证据等级：已确认，性能和调度细节未测量。

## 初始化

1. 应用调用 `common_init`/参数解析（server 路径已确认）。[tools/server/server.cpp:88-112]
2. `llama_backend_init` 初始化时间、F16 表并加载 backend registry。[src/llama.cpp:122-135]
3. `llama_model_load_from_file` 创建 loader，读取架构并调用模型类的 hparams、vocab、tensor 加载。[src/llama.cpp:316-377] 
4. `llama_init_from_model` 检查参数，构造 context。[src/llama-context.cpp:3660-3765]

## 一次 decode

`llama_decode` 是 C API wrapper，调用 context 的 `decode`；context 构造 graph 后由 `graph_compute` 设置线程数并调用 `ggml_backend_sched_graph_compute_async`。[src/llama-context.cpp:4247-4255] [src/llama-context.cpp:2492-2518]

**推断：** 每轮生成的可见结果由上层 sampler 读取 output logits 决定；server 的 batching 文档明确指出 decode 后进行 embedding 或 `common_sampler_sample`。[tools/server/README-dev.md:76-84]

## Server 并发模型

HTTP worker 做 JSON、模板、tokenize、响应格式化；`server_context` 专用单线程拥有 slots；task/result queue 是边界。这样重计算不会直接发生在 HTTP handler 调用栈中。[tools/server/README-dev.md:86-119]

## 状态生命周期

`model` 必须先于 `context` 存在；context 释放后才能释放其 graph/KV/backend 资源。server 的 stream manager 在 main 初始化后启动，在 cleanup 中先于 backend free 停止。[tools/server/server.cpp:99-112] [tools/server/README-dev.md:127-149]

## 关键分支

- vocab-only 跳过 tensor 加载。[src/llama.cpp:364-370]
- tensor split 需要设备且架构支持；context 对 tensor split 强制 flash attention。[src/llama.cpp:158-207] [src/llama-model.cpp:349-356] [src/llama-context.cpp:3683-3695]
- quantized K/V cache 对 flash-attn 和 head block size 有约束，违规返回空 context。[src/llama-context.cpp:3702-3733]

---

**页尾：** 本页没有给出未实测的吞吐、延迟或显存数值。

## 相关文档
- [项目入口](../README.md)
- [分析状态](analysis-state.md)
- [源码证据索引](evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
