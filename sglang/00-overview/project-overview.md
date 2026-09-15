# 项目概览

## 1. 文档目标

本知识库面向第一次阅读 SGLang 源码的读者，回答三个问题：**系统由哪些模块组成、一次请求如何穿过这些模块、修改一个模块会影响什么**。文章中的结论优先来自当前 checkout 的源码；旧文章和上游网页只作为导航，不能替代源码证据。

每条重要结论尽量标注状态：

- **已确认**：已由当前 checkout 的源码、构建文件或测试文件直接支持。
- **推断**：由多个源码事实推导出的运行关系，仍应在相关模块文章中继续核对。
- **未知**：当前没有足够证据，不能把它写成事实。

代码引用统一使用 `[仓库相对路径:起始行-结束行]`。行号是当前 checkout 的快照，源码变化后应以符号和相邻逻辑复核。

## 2. 当前源码版本

- 分支：`main`。
- HEAD：`78be4b50af88e9ea72d75b4c3a3e42b7297d2501`。
- HEAD 提交：`[Config] msgspec.Struct for the config tier (#38753)`，提交时间为 `2026-09-09T19:41:19-07:00`。
- HEAD 没有发现指向它的本地 tag；远程 `origin` 为 `https://github.com/sgl-project/sglang.git`。
- 当前工作树包含未跟踪或新增的 `sglang/` 知识库目录；本知识库本身不是生产源码改动。

**已确认边界**：以上是本地 checkout 的版本，不等同于远程最新 release。任何文档结论都以该 commit 的源码为准。

## 3. 仓库地图

| 区域 | 主要内容 | 阅读入口 |
|---|---|---|
| `python/sglang/cli` | 顶层命令和后端分发 | `python/sglang/cli/main.py`、`serve.py` |
| `python/sglang/srt/entrypoints` | HTTP、OpenAI、gRPC 等服务入口 | `http_server.py` |
| `python/sglang/srt/managers` | TokenizerManager、Scheduler、批次和 IPC | `scheduler.py`、`tokenizer_manager.py` |
| `python/sglang/srt/model_executor` | ModelRunner、ForwardBatch 和执行路径 | `model_runner.py` |
| `python/sglang/srt/model_loader` | checkpoint、量化、权重缓存和专用 loader | `loader.py`、`auto_loader.py` |
| `python/sglang/srt/models` | 具体模型架构及权重映射 | `llama.py`、`qwen2.py` 等 |
| `python/sglang/srt/distributed` | WORLD、TP/PP/DP/EP/CP/DCP 进程组 | `bootstrap.py`、`parallel_state.py` |
| `python/sglang/srt/mem_cache` | KV pool、分配器和 Radix Cache | `memory_pool.py`、`radix_cache.py` |
| `python/sglang/srt/layers`、`kernels` | 算子、attention、量化和设备实现 | 各 backend 目录 |
| `python/sglang/multimodal_gen` | 独立的图像/视频/音频生成 runtime | `runtime/` |
| `rust` | PyO3 扩展和服务组件 | `rust/Cargo.toml` |
| `experimental/sgl-router` | 独立 KV-aware router | 该目录 README |
| `sgl-model-gateway` | 独立 Rust model gateway | `sgl-model-gateway/Cargo.toml` |
| `examples` | 可运行示例和脚本 | `examples/runtime`、`frontend_language` |
| `test/registered` | CI 自动发现的测试 | `test/run_suite.py` |
| `test/manual` | 需要特殊硬件、模型或人工环境的测试 | 各专题目录 |
| `docs` | 面向使用者和开发者的文档 | `docs/docs` |
| `.github/workflows` | CI、发布和 nightly 流程 | workflow YAML |

## 4. 最短主线

```text
sglang serve
  -> prepare_server_args / resolve_once
  -> HTTP launch_server
  -> TokenizerManager
  -> Scheduler process
  -> TpModelWorker / ModelRunner
  -> model loader + distributed groups + KV pools
  -> ForwardBatch / transformer / sampler
  -> Scheduler result processing
  -> detokenizer / HTTP response
```

这条链路是**普通 LLM HTTP 模式的主线**。Ray、encoder-only、diffusion、Rust server、PD 分离和 speculative worker 是分支，不应被误写成所有部署都必经的步骤。

## 5. 当前缺口

已完成的 `02-request-flow/` 文章详细解释了请求、调度、执行、配置和 IPC；`01-modules/` 已建立 M01-M18 的实现层文章，并将多模态、分离部署、MoE/量化/LoRA、设备后端、Rust/router/gateway 和测试系统纳入模块地图。M06-M10 已补充 loader/weight mapping、group 拓扑、KV ownership、attention graph/fallback 和 sampling/grammar 的代表路径；完整专用变体与真实环境运行证据仍待补充。静态文章不替代 GPU、多卡、模型下载、网络传输和端到端测试。
