# llama.cpp 项目理解知识库

> 状态：已确认（基于工作区当前源码）  
> 源码版本：`master` / `093a2f86c3e37c54fa3e1f9efb17b304f3433abd`
> 版本字符串：`0.4.0-dev`（CMake 默认开发构建）  
> 文档语言：中文  
> 输出路径：当前知识库仓库根目录（`00-overview/`、`01-modules/`、`80-demos/`、`90-cross-module/`、`99-roadmap/`）

本知识库按“总览 -> 模块 -> Demo -> 跨模块 -> 实践”组织。源码引用使用 `[相对路径:起始行-结束行]`；“已确认/推断/未知”表示证据等级。所有命令的“来源已确认”不等于“本工作区已执行”：本批仅执行了版本、目录和静态盘点，没有执行完整构建、测试或模型 Demo。

## 阅读入口

### 总览

- [项目概览](00-overview/project-overview.md)
- [总体架构](00-overview/architecture.md)
- [运行模型](00-overview/runtime-model.md)
- [全局数据流](00-overview/global-data-flow.md)
- [依赖地图](00-overview/dependency-map.md)
- [构建与部署](00-overview/build-and-deploy.md)
- [全局错误模型](00-overview/global-error-model.md)
- [设计原则与决策](00-overview/design-principles.md)
- [术语表](00-overview/glossary.md)
- [证据索引](00-overview/evidence-index.md)
- [分析状态](00-overview/analysis-state.md)

### 模块

- [模块注册表](01-modules/module-registry.md)
- [M01 ggml 张量与后端](01-modules/M01-ggml-backend/README.md)
- [M02 llama Runtime 与模型](01-modules/M02-llama-runtime/README.md)
- [M03 common 应用基础设施](01-modules/M03-common-app-infra/README.md)
- [M04 llama-server](01-modules/M04-server/README.md)
- [M05 CLI 与统一应用](01-modules/M05-cli-app/README.md)
- [M06 mtmd 多模态](01-modules/M06-mtmd/README.md)
- [M07 转换与 GGUF Python](01-modules/M07-conversion-gguf/README.md)
- [M08 构建、测试与 CI](01-modules/M08-build-tests-ci/README.md)
- [M09 Web UI](01-modules/M09-web-ui/README.md)

### 真实流程

- [Demo 注册表](80-demos/demo-registry.md)
- [D01 Server 非流式 Chat Completion](80-demos/D01-server-chat/README.md)
- [D02 CLI 文本推理](80-demos/D02-cli-inference/README.md)

### 串联与实践

- [跨模块调用链](90-cross-module/call-chains.md)
- [线程、队列与资源生命周期](90-cross-module/thread-queue-resource.md)
- [数据与配置边界](90-cross-module/data-and-configuration.md)
- [池化与资源管理专题](90-cross-module/pooling-and-resource-management.md)
- 专题补充：ggml allocator 与 graph reserve/reset 已纳入上述资源专题。
- [快速开始](99-roadmap/quick-start.md)
- [阅读路线](99-roadmap/reading-guide.md)
- [调试指南](99-roadmap/debugging-guide.md)
- [开发配方](99-roadmap/feature-development-recipes.md)
- [测试配方](99-roadmap/testing-recipes.md)
- [性能路径](99-roadmap/performance-guide.md)
- [风险登记](99-roadmap/risk-register.md)
- [技术债与后续](99-roadmap/technical-debt-next-steps.md)

## 推荐阅读路径

1. [项目概览](00-overview/project-overview.md) -> [总体架构](00-overview/architecture.md)。
2. 先读 [M02](01-modules/M02-llama-runtime/README.md)，再读 [M01](01-modules/M01-ggml-backend/README.md)，理解 API 如何落到图执行。
3. 读 [M03](01-modules/M03-common-app-infra/README.md) 和 [M05](01-modules/M05-cli-app/README.md)，再进入 [D02](80-demos/D02-cli-inference/README.md)。
4. 读 [M04](01-modules/M04-server/README.md)，跟踪 [D01](80-demos/D01-server-chat/README.md) 和 [跨模块调用链](90-cross-module/call-chains.md)。
5. 最后阅读 [M08](01-modules/M08-build-tests-ci/README.md)、实践层和 [分析状态](00-overview/analysis-state.md)。

## 当前确认的十个关键入口

1. `CMakeLists.txt`：构建开关和目标编排。
2. `include/llama.h`：稳定 C API（参数、批次、生命周期）。
3. `src/llama.cpp:llama_backend_init`：后端初始化。
4. `src/llama.cpp:llama_model_load_from_file`：模型加载公开入口。
5. `src/llama-model.cpp:llama_model_create`：架构到具体模型派发。
6. `src/llama-context.cpp:llama_init_from_model`：上下文约束检查和创建。
7. `src/llama-context.cpp:llama_context::graph_compute`：调度器图计算。
8. `src/llama-context.cpp:llama_decode`：解码 API wrapper。
9. `tools/server/server-context.cpp:server_routes::handle_completions_impl`：HTTP completion 任务入口。
10. `app/llama.cpp:main`：统一 `llama` 命令分发。

## 十条关键调用链

1. `app/main -> llama_cli -> common 参数/模型处理 -> llama_model_load_from_file -> llama_init_from_model -> llama_decode -> graph_compute`。
2. `app/main -> llama_server -> server_routes::handle_completions_impl -> server_queue -> update_slots -> llama_decode`。
3. `llama_backend_init -> ggml_backend_load_all -> backend registry/device discovery`。
4. `llama_model_load_from_file -> llama_model_loader -> llama_model_create(loader) -> architecture subclass`。
5. `llama_model_load -> load_hparams -> load_vocab -> load_tensors`。
6. `llama_init_from_model -> new llama_context -> scheduler/compute buffers/KV memory`。
7. `llama_decode -> llama_context::decode -> graph construction -> graph_compute -> ggml_backend_sched_graph_compute_async`。
8. `handle_completions_impl -> tokenize_input_prompts/process_mtmd_prompt -> server_task -> server_response_reader`。
9. `server_res_generator -> response reader -> task_result::to_json -> SSE/JSON HTTP output`。
10. `POST + X-Conversation-Id -> stream producer -> bounded ring -> GET /v1/stream replay`。

## 交付边界

本知识库追踪了公共 wrapper 之后的关键实现（模型加载、上下文创建、图调度、server 队列和响应），但不会把 3000+ 文件机械复制成目录清单。具体模型算子、每个后端 kernel 和每个 UI route 仍属于增量分析范围；见 [分析状态](00-overview/analysis-state.md)。

---

**页尾：** 本页最后复核于 `2026-09-10`；源码版本变化后必须重新确认行号和结论。
