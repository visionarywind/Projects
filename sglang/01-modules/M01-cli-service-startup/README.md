# M01 CLI 与服务启动

- 文档目的：解释 `sglang` 命令如何从顶层子命令进入具体 backend，再进入 SRT runtime。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/sglang HEAD 78be4b50af（2026-09-15 只读确认）。
- 证据状态：源码静态分析已确认；真实服务启动未验证。
- 最后更新：2026-09-15
- 前置阅读：[启动链路](../../02-request-flow/01-启动链路.md)、[运行时模型](../../00-overview/runtime-model.md)。
- 后续阅读：[M02 HTTP/API 与协议](../M02-http-api-protocol/README.md)、[M15 IPC](../../02-request-flow/06-多进程与IPC.md)。
## 结论摘要

本页聚焦 01-modules/M01-cli-service-startup/README.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


- 源码证据 checkout：`/home/mtuser/workspace/repos/Github/sglang`，当前 checkout。

## 1. 一句话模型

`sglang` 顶层 parser 只识别子命令；`serve` 保留未知参数，把模型类型和 positional model path 规范化后交给 `ServeBackendRegistry`；默认 LLM backend 再把参数交给 `prepare_server_args` 和 `run_server`。因此 CLI 是控制面入口，不是 scheduler，也不直接执行 Transformer。

```text
argv
  -> cli.main.main / parse_known_args
  -> cli.serve.serve
  -> ServeRequest
  -> registry.get 或 registry.auto_detect
  -> backend.run
  -> prepare_server_args
  -> launch_server.run_server
  -> resolve_once / resolving_view
  -> HTTP、Ray、gRPC、encoder-only 分支
```

## 2. 入口和参数边界

### 2.1 顶层延迟解析

`python/sglang/cli/main.py:7-30` 的 `main` 建立 required subparser，登记 `serve`、`generate` 和 `version`。`serve` 与 `generate` 使用 `parse_known_args()`，详细参数不在顶层重复声明，而是作为 `extra_argv` 传入子命令。这使模型 backend 可以拥有自己的 flags，但也意味着错误可能延迟到 backend parser 才出现。

### 2.2 `serve` 规范化

`python/sglang/cli/serve.py:23-59`：

- `_extract_model_type_override` 读取 `--model-type`，并校验其是否为单值；
- `_normalize_positional_model_path` 将 `sglang serve <model> ...` 改写为 `--model-path <model> ...`，同时保留 `model_path_is_positional`；
- `ServeRequest` 是 frozen dataclass，字段为 `argv`、`model_path`、`model_path_is_positional`。

这个标记会传给 diffusion backend，使显式 positional 参数与默认参数区分开；不能简单把所有参数都当作用户显式设置。

## 3. Backend registry 契约

源码：`python/sglang/cli/serve_backends.py:28-194`。

| 对象 | 关键字段/契约 |
|---|---|
| `ServeBackendDetection` | `MATCH`、`NO_MATCH`、`UNKNOWN`；探测器不确定时不应误选 backend |
| `ServeRequest` | 不可变 argv 快照及模型路径信息 |
| `ServeBackend` | `api_version`、阻塞式 `run`、可选 `detect`、`requires_model_path` |
| `RegisteredServeBackend` | registry 名称、backend 实例、entry point 元数据 |
| `ServeBackendRegistry` | entry point 发现、显式加载缓存、自动探测和 LLM fallback |

entry point group 是 `sglang.serve_backends`，API version 为 `1`。发现阶段只收集 entry point，不立即 import 外部实现。`get()` 被调用时才加载工厂，并检查 callable、返回类型和 API version。显式加载失败会抛错；自动探测时单个 detector 异常被记录并跳过。

自动探测顺序的语义是：跳过内置 `llm` detector，收集所有 `MATCH`；多个匹配报错；单个匹配选中；无匹配回退 `llm`。外部 backend 不能覆盖保留名 `auto` 或内置实现（registry 构造与冲突检查见同文件 `79-194`）。

## 4. LLM 启动调用链

```text
main()
  -> serve(args, extra_argv)                         [cli/main.py:12-30; cli/serve.py:166-207]
  -> _normalize_positional_model_path()              [cli/serve.py:55-59]
  -> ServeBackendRegistry.auto_detect()/get()        [cli/serve_backends.py:79-194]
  -> _run_llm(request)                               [cli/serve.py:90-100]
  -> prepare_server_args(list(request.argv))         [cli/serve.py:90-100]
  -> run_server(server_args)                         [launch_server.py:17-58]
  -> server_args.resolve_once()
  -> resolving_view(server_args)
```

`_run_llm` 的 help 分支在调用 `prepare_server_args` 前结束，因此 `sglang serve --help` 不应启动 scheduler。真实启动时 `run_server` 根据 resolved config 选择：encoder-only HTTP/gRPC、legacy SMG gRPC、Ray HTTP 或默认 Python HTTP server。`grpc-port` 提供 native Rust gRPC 旁路时，不等同于 `smg_grpc_mode`。

## 5. 生命周期和失败边界

### 初始化

1. 解析子命令和 backend-owned argv；
2. 加载插件；
3. 构造或探测 backend；
4. backend 解析 `ServerArgs`；
5. `resolve_once` 将 raw config 固化为运行时视图；
6. 启动 Engine、scheduler/detokenizer 和 HTTP 或替代入口（Engine 细节见 M15）。

### 稳态

backend 的 `run` 通常阻塞在服务生命周期。CLI 不参与每个 token 的调度；请求由 M02/M03 进入 scheduler。

### 异常

- 未知 `--model-type`：registry 显式校验失败；
- 多个 detector 匹配：拒绝含糊选择；
- detector 异常：自动模式跳过并继续其他 detector；
- 显式 backend import/API version 错误：严格抛错；
- 缺失所需 model path：backend 运行前检查；
- `run` 抛错或返回：由 `serve` 的 `finally` 执行子进程树清理。

### 清理

`cli/serve.py:166-207` 的 `finally` 调用 `kill_process_tree(os.getpid(), include_parent=False)`。这覆盖正常返回、help 后异常和启动失败，但不能替代 scheduler 自身的 IPC/设备清理。

## 6. 小例子和调试断点

例：`sglang serve /models/qwen --tp-size 2`。

1. positional 参数变为 `--model-path /models/qwen --tp-size 2`；
2. auto registry 通常选 `llm`；
3. LLM parser 生成 `ServerArgs`；
4. `run_server` 进入默认 HTTP 分支；
5. Engine 在 scheduler rank/GPU 上继续初始化。

建议断点：

- `cli/serve.py:_normalize_positional_model_path`：确认 argv 是否被正确重写；
- `serve_backends.py:ServeBackendRegistry.auto_detect`：确认 detector 结果和 fallback；
- `launch_server.py:run_server`：检查 `resolving_view` 的分支字段；
- `serve.py:serve` 的 `finally`：检查启动失败后是否仍有子进程。

## 7. 测试与未验证范围

静态定位：

- `test/registered/core/test_srt_endpoint.py`：HTTP/Rust server 端点测试；
- `test/manual/test_config_integration.py`、`test/manual/test_ray_engine.py`：配置及 Ray 分支；
- CLI/backend 专项可在 `test/registered` 中按 `cli`、`server`、`backend` 搜索。

本次未执行 GPU 服务、模型下载、Ray、多卡、Rust server 或完整 suite。entry point 安装环境中的第三方 backend 发现也未运行验证。

## 8. 修改影响

- 修改顶层 argv：影响 M02 协议启动、所有 backend 和帮助文档；
- 修改 `ServeRequest`：影响 out-of-tree backend API；
- 修改自动探测：可能改变 diffusion/LLM 选择和错误暴露时机；
- 修改 `run_server` 分支：影响 HTTP、Ray、encoder、gRPC 和 M15 进程树；
- 修改 cleanup：需同时检查 scheduler、detokenizer、native gRPC、sidecar 和 weight-cache daemon。

**已确认**：CLI 与 runtime 的边界如上。**推断**：特定第三方 backend 的探测优先级由 entry point 名称/实现决定。**未验证**：实际 GPU 进程树和所有插件组合。

## 文档元数据（规范补充）

- 文档目的：说明 `01-modules/M01-cli-service-startup/README.md` 的源码分析范围、结论和维护入口。
- 适用范围：当前项目对应模块/入口的静态源码与测试分析。
- 对应源码版本：以本项目 `00-overview/analysis-state.md` 或同页版本字段为准。
- 证据状态：静态源码证据；未执行的构建、测试、GPU、网络或多进程行为保持“未验证”。
- 最后更新：2026-09-15
- 前置阅读：本项目根 README 与 `00-overview/analysis-state.md`。
- 后续阅读：本模块/示例的实现、测试和风险页面。

## 深度审计

| 分析对象 | 入口落地 | 正常路径 | 分支 | 异常 | 清理 | 数据生命周期 | 执行上下文 | 行级证据 | Demo 映射 | 状态/缺口 |
|---|---|---|---|---|---|---|---|---|---|---|
| `01-modules/M01-cli-service-startup/README.md` | 已完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 已映射或不适用 | 部分完成：动态行为、边界或专用变体仍需验证 |

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
