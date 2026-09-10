# 项目概览

> 模块：全局 / 阶段 0-1  
> 证据等级：已确认，少量范围判断为推断  
> 源码版本：`311d4211bf1611ff7ca6b67035a4a07c79766efc`

## 1. 项目是什么

**已确认：** README 将项目定位为 C/C++ 的 LLM 推理实现；CMake 项目同时生成库、工具、示例、测试、server 和统一 `llama` 应用。[README.md:1-20] [CMakeLists.txt:217-249]

**已确认：** 推理运行时以 GGUF 模型为输入，公共 API 在 `include/llama.h`，实现集中在 `src/`；张量图和设备后端由 `ggml/` 提供。构建关系先加入 ggml，再加入 llama。[CMakeLists.txt:204-221]

## 2. 源码版本与状态

| 项目 | 结果 | 状态 |
|---|---|---|
| 分支 | `master` | 已确认 |
| HEAD | `311d4211bf1611ff7ca6b67035a4a07c79766efc` | 已确认 |
| `git describe` | `b10887-2-g311d4211b-dirty` | 已确认；工作区有 `CLAUDE.md` 修改 |
| CMake 版本 | `0.4.0-dev` 默认 | 已确认；`LLAMA_BUILD_IS_DEV=ON` |
| 文件规模 | `src ggml common tools tests examples conversion gguf-py` 共 3099 个文件（find 统计） | 已确认 |
| 实际编译/测试 | 未执行 | 未知 |

## 3. 运行时主线

模型文件经 loader 读取 metadata、词表和 tensor；架构枚举派发为具体 `llama_model_*`；context 保存调度器、KV/memory、线程和 sampling 状态；batch 被转为 ggml graph，异步交给 backend scheduler，输出再由 CLI 或 server 采样/格式化。[src/llama.cpp:316-377] [src/llama-model.cpp:362-369] [src/llama-context.cpp:3660-3765] [src/llama-context.cpp:2492-2518] [src/llama-context.cpp:4247-4255]

## 4. 产品入口

- `llama-cli`：交互式/批量文本入口。
- `llama-server`：HTTP API，inference/router 两种模式。
- `llama`：统一命令分发，公开 `serve`、`cli`、`download` 等命令。[app/llama.cpp:64-78]
- 专项工具：bench、quantize、perplexity、completion、GGUF split、tokenize、mtmd 等，由 `tools/` 的 CMake 选择性加入。[CMakeLists.txt:229-258]
- Web UI：SvelteKit/Vite，作为 server 的嵌入资源或独立开发前端；当前未运行 UI 构建。[tools/ui/package.json:1-27]

## 5. 边界和假设

- **推断：** 目标读者是需要修改 runtime、server、后端或构建系统的开发者；用户未单独填写读者和平台。
- **推断：** 主 Demo 选 server chat completion，因为它串联 HTTP、common、server、llama、ggml；D02 作为最小 API/CLI 对照。
- **未知：** 当前机器是否拥有模型缓存、GPU 驱动和可用网络，因此 Demo 不能声称已运行。

## 未解决问题

1. 每个具体模型架构的 graph builder 和特殊 memory 实现尚未逐一展开。
2. 每个 accelerator backend 的 kernel、编译器和运行时依赖需按目标平台另建证据页。
3. 没有在当前环境实际执行 CMake、CTest、pytest 或模型推理。

## 下一步阅读

先看 [architecture.md](architecture.md)，然后 [M02 README](../01-modules/M02-llama-runtime/README.md)。

---

**页尾：** 若源码移动或提交变化，重新运行版本盘点并更新本页。
