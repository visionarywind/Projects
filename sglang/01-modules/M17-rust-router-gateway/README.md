# M17 Rust、Router 与 Gateway

- 文档目的：解释 01-modules/M17-rust-router-gateway/README.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/sglang HEAD 78be4b50af（2026-09-15 只读确认）。
- 证据状态：Rust workspace、router/indexer 和 gateway 的静态边界已确认；跨语言编译、真实服务发现、网络故障恢复和吞吐未验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M17-rust-router-gateway/README.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


- 源码证据：`/home/mtuser/workspace/repos/Github/sglang` 当前 checkout。
- 关联：[M01 CLI/启动](../M01-cli-service-startup/README.md)、[M02 HTTP/API](../M02-http-api-protocol/README.md)、[M04 Scheduler](../M04-scheduler-batching/README.md)、[M15 IPC](../M15-ipc-control-plane/README.md)。

## 1. 三个层次

M17 不等于单一“Rust server”：Rust workspace 中有面向 Python/服务的 crate；`experimental/sgl-router` 负责请求路由与 KV/prefix 索引；`sgl-model-gateway` 负责更外层的 worker 注册、服务发现、策略、重试和可靠性。它们可以组合，但进程边界和协议不能混写。

```text
client / gateway
  -> worker registry and discovery
  -> routing policy / circuit breaker
  -> sgl-router
  -> prefix/KV indexer
  -> SGLang HTTP/gRPC worker
  -> upstream response / cancellation
```

## 2. Rust workspace 与 FFI

`rust/Cargo.toml` 的 workspace 包含 `sglang-grpc`、`sglang-mm`、`sglang-server`，并将 `sglang-radix-tree` 作为独立 extension manifest 排除在 workspace dependency graph 外。[`rust/Cargo.toml:1-50`] `sglang-server` 的 API 层只负责 axum/tokio HTTP，TokenizerManager 的 flume wiring 把 intake、tokenized、MM completion、abort 和 shutdown 分开，且 abort lane 独立为无界 channel。[`rust/sglang-server/src/api_server.rs:1-15`][`rust/sglang-server/src/tokenizer_manager/wiring.rs:9-77`] PyO3 或 native extension 配置把 Rust 类型/函数暴露给 Python；因此序列化字段、错误码、生命周期和线程模型是跨语言契约。

调用链可概括为：

```text
Python entrypoint or extension import
  -> PyO3/native Rust boundary
  -> Rust request/response or gRPC handling
  -> Python scheduler/worker or network peer
  -> translated error/result
```

Rust crate 能编译不代表 Python packaging、ABI、模型服务和 GPU worker 已联通；本仓库只记录源码结构，未将构建命令当作验证证据。

## 3. Router 与 KV-aware routing

`experimental/sgl-router` 提供独立 router；`sgl-kv-indexer` 维护请求前缀与 worker/KV 位置相关的索引，router 据此选择目标 worker，并需要在 worker 注册、健康状态和 prefix index 变化时更新。路由策略必须兼顾新请求、已有 prefix 命中、worker 负载和不可用节点；索引是提示，不应绕过 worker 对请求与 KV 的最终校验。

`experimental/sgl-router` 的请求入口把 route/method 计数包在 axum middleware，chat route 额外限制 body size；proxy 在发送前检查 circuit breaker，解析 worker URL，转发允许的 headers，并把非流式 status/body 或流式 SSE body 映射为 API response。流式请求把 load guard 保持到 body 完成/断开，只有成功完成才记 breaker success，中途断流记 failure。[`experimental/sgl-router/src/server/app.rs:14-82`][`experimental/sgl-router/src/proxy/mod.rs:19-31`][`experimental/sgl-router/src/proxy/mod.rs:82-153`][`experimental/sgl-router/src/proxy/mod.rs:155-257`]

## 4. Model Gateway

`sgl-model-gateway` 的 app/context、middleware、server、service-discovery、routing、reliability 和 metrics 模块组成外层控制面。它处理 worker 注册/发现、认证或请求边界、路由策略、超时、circuit breaker、retry、上游 cancellation 和指标；模型推理仍由 worker/SGLang runtime 完成。

```text
Gateway request
  -> middleware validation/auth/metrics
  -> service discovery snapshot
  -> route selection
  -> upstream stream
  -> cancellation / retry / response translation
```

服务发现快照、健康检查和 circuit breaker 状态存在并发更新问题；修改字段时需同时检查 gateway、router 和 worker 协议。安全测试、reliability 测试和 e2e 测试是行为证据入口，但本次未运行。

## 5. 小例子和断点

例：两个 worker 都可服务一个请求，router 发现 worker-A 的 prefix index 命中而 worker-B 空闲；选择后转发请求。若连接在首个 SSE chunk 前失败，可以按幂等策略重试；若已经向客户端发送部分输出，则通常应终止并报告，而不是静默切换 worker。

断点：Rust crate 的 PyO3/gRPC入口、router request handler、KV index lookup/update、worker registry、route selection、circuit breaker、upstream cancellation 和 response stream 转换点。

## 6. 测试与未验证

定位入口包括 Rust crate 测试、`experimental/sgl-router` 的 indexer/router benchmark 与测试，以及 `sgl-model-gateway` 下 e2e、reliability、routing、security、metrics、MCP 和 WASM 测试。本次未执行 Cargo build、跨语言 ABI 检查、真实多 worker 服务发现、网络故障注入或吞吐 benchmark。

## 7. 修改影响

修改 wire schema 会同时影响 Python API、Rust serde/PyO3、gRPC、router 和 gateway；修改 prefix/KV routing 会影响 M08 cache ownership、M13 transfer 状态和 M04 admission；修改 retry/cancellation 会影响 M02 SSE 语义、请求幂等性和 M15 worker 生命周期。外部 gateway 的健康状态不能替代 scheduler/worker 的 readiness。

## 文档元数据（规范补充）

- 文档目的：说明 `01-modules/M17-rust-router-gateway/README.md` 的源码分析范围、结论和维护入口。
- 适用范围：当前项目对应模块/入口的静态源码与测试分析。
- 对应源码版本：以本项目 `00-overview/analysis-state.md` 或同页版本字段为准。
- 证据状态：静态源码证据；未执行的构建、测试、GPU、网络或多进程行为保持“未验证”。
- 最后更新：2026-09-15
- 前置阅读：本项目根 README 与 `00-overview/analysis-state.md`。
- 后续阅读：本模块/示例的实现、测试和风险页面。

## 深度审计

| 分析对象 | 入口落地 | 正常路径 | 分支 | 异常 | 清理 | 数据生命周期 | 执行上下文 | 行级证据 | Demo 映射 | 状态/缺口 |
|---|---|---|---|---|---|---|---|---|---|---|
| `01-modules/M17-rust-router-gateway/README.md` | 已完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 已映射或不适用 | 部分完成：动态行为、边界或专用变体仍需验证 |

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
