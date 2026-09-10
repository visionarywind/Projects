# 总体架构

> 证据等级：已确认；架构归纳为推断。源码版本：`311d4211bf1611ff7ca6b67035a4a07c79766efc`

## 分层

```text
应用/HTTP/UI
    -> common 参数、模板、采样、缓存
    -> llama C API / C++ runtime
    -> ggml graph、scheduler、backend registry
    -> CPU/GPU/NPU backend
    -> host/device memory 与硬件
```

**已确认：** CMake 先建立 ggml，再建立 src/llama；common、tests、tools、app 条件加入，说明构建依赖由底向上。[CMakeLists.txt:204-249]

**已确认：** server 将 `server-context`（核心逻辑）和 `llama-server-impl`（HTTP/model/router）分开，最终 executable 只链接 impl。[tools/server/CMakeLists.txt:3-67]

## 运行对象

- `llama_model`：GGUF metadata、词表、权重、架构和设备映射。
- `llama_context`：上下文参数、KV/memory、线程池、scheduler、采样相关状态。
- `llama_batch`：token/embedding、位置、sequence id 和 output 标志。[include/llama.h:300-460]
- ggml tensor/graph：计算表达式及其 backend placement。
- server slot/task/result：并行请求的序列状态和跨线程消息。[tools/server/README-dev.md:46-60]

## 关键控制点

1. Backend registry 在 `llama_backend_init` 中加载；模型设备选择由 runtime 决定。[src/llama.cpp:122-135] [src/llama.cpp:158-312]
2. 架构枚举通过 `llama_model_mapping` 选择具体类；未知架构直接抛异常。[src/llama-model.cpp:343-369]
3. `llama_init_from_model` 在创建 context 前验证 batch/context/cache/flash-attn 约束。[src/llama-context.cpp:3660-3747]
4. `graph_compute` 统一把 graph 交给 scheduler 异步计算。[src/llama-context.cpp:2492-2518]
5. server 在 HTTP 层解析和格式化，在专用 context 线程批处理并调用 decode。[tools/server/README-dev.md:76-119]

## 修改影响

改变公共 API 会影响所有消费者；改变 graph 或 backend 会影响模型和 `test-backend-ops`；改变 server handler 会影响 HTTP、SSE、UI 和 pytest。以上是基于 target 和调用边界的影响推断。

## 未覆盖

每个模型的 `build_*` graph、每个 backend 的具体 kernel、UI 每个 route 尚未全部追踪。

---

**页尾：** 图示是职责抽象，不代表所有构建配置都启用全部层。
