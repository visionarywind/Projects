# 分析状态

> 这是增量分析的事实账本，不把“文件已创建”当成“源码已分析”。

## 版本快照（已确认）

- 日期：2026-09-10
- 分支：`master`
- HEAD：`311d4211bf1611ff7ca6b67035a4a07c79766efc`
- describe：`b10887-2-g311d4211b-dirty`
- 工作树：仅根 `CLAUDE.md` 有未提交修改；知识库文件为本次新增。
- CMake：`LLAMA_VERSION_BASE=0.4.0`，默认 `LLAMA_BUILD_IS_DEV=ON`，所以版本为 `0.4.0-dev`。[CMakeLists.txt:5-23]

## 阶段记录

| 阶段 | 状态 | 覆盖 |
|---|---|---|
| 0 盘点/模块划分 | 已完成（静态） | 顶层构建、运行入口、测试入口、9 个模块 |
| 1 总览层 | 已完成（首版） | 架构、运行、数据、依赖、错误、术语、证据 |
| 2 核心模块 | 已完成（首版） | M01-M05 的关键入口与正常/错误路径 |
| 3 辅助/平台/测试 | 已完成（首版） | M06-M09 的边界与命令 |
| 4 Demo | 已完成（静态解剖+构建 smoke） | D01 server chat、D02 CLI；server/CLI 构建和 help 已验证，模型/pytest 未运行 |
| 5 跨模块 | 已完成（首版） | 调用链、线程/队列、数据/资源 |
| 6 实践 | 已完成（首版） | quick start、debug、测试、性能、风险 |
| 7 深度审计 | 部分完成 | 链接和模块 ID 已人工统一；证据范围脚本已校验；未运行完整测试和全量 Markdown link checker |

## 已分析的关键文件

`AGENTS.md`、`CONTRIBUTING.md`、`README.md`、根和子目录 CMake、`include/llama.h`、`src/llama.cpp`、`src/llama-model.cpp`、`src/llama-context.cpp`、`tools/server/server.cpp`、`tools/server/server-context.cpp`、`tools/server/README-dev.md`、server tests README/tests.sh/test_chat_completion.py、`tests/CMakeLists.txt`、`app/llama.cpp`、`tools/ui/package.json`、CI 文档和工作流（此前已静态读取）。

## 调用链证据状态

- 模型：公开 load -> loader -> architecture dispatch -> hparams/vocab/tensors：已确认。
- Context：参数检查 -> `new llama_context`：已确认。
- Compute：`llama_decode` wrapper -> context decode -> async backend scheduler：入口和 scheduler 已确认，中间 graph builder 需继续细化。
- Server：HTTP handler -> task -> queue -> `llama_decode` -> response reader：README 和实现已确认。
- Streaming：producer/ring/consumer 生命周期：README-dev 已确认，具体所有分支未逐行审计。

## Demo 状态

- 构建：`cmake -B build -DLLAMA_BUILD_UI=OFF` 和 targets `llama-server llama-cli` 已通过（CPU/x86_64）；配置日志显示 OpenSSL 不可用，HTTPS disabled；UI assets provisioned。
- 统一应用：`llama-app` 已构建，`./build/bin/llama --help` smoke 已通过。
- CTest：`ctest --test-dir build -N` 已执行并确认注册，但未构建测试 executable，因此未执行测试用例。
- GGUF Python：`python3 -m unittest discover ./gguf-py -v` 已执行；11 项中 10 项通过，1 个测试模块因缺少 `pytest` 导入失败。
- D01：源码和测试步骤已记录；未下载模型、未启动 server、未执行 pytest。
- D02：CLI/server/统一 app 的 `--help` smoke 已通过；未加载模型、未生成 token。

## 术语与 ID

M01 ggml/backend；M02 llama/runtime；M03 common；M04 server；M05 CLI/app；M06 mtmd；M07 conversion/gguf；M08 build/tests/CI；M09 UI。D01 server chat；D02 CLI inference。ID 在本目录内保持稳定。

## 风险与缺口

1. 行号随源码变化；本知识库引用只对上述 HEAD 有效。
2. 后端矩阵很大，M01 仅覆盖统一接口和调度边界，不覆盖所有 kernel。
3. server 的 router、MCP、文件能力、sleep、resumable stream 需要专项审计。
4. 性能数字没有测量，文档只描述路径，不给未经运行的吞吐结论。
5. UI 的 route/store/service 只依据 package 和 server README，缺少源码逐文件追踪。

## 下一批起点

如继续深挖：先逐行审计 `src/llama-context.cpp::decode`、`llama_context` 构造函数和 `tools/server/server-context.cpp::update_slots`，然后在有模型和依赖时执行 D01 单测；再按实际平台分析一个 backend。

---

**页尾：** 每次增量分析必须先更新版本快照，再新增结论。
