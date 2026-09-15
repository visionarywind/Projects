# 模块注册表

- 文档目的：解释 01-modules/module-registry.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/llamacpp HEAD 093a2f86c（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/module-registry.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


> 划分原则：按可独立修改、构建边界、运行时责任和数据/线程边界划分，而非按每个目录机械拆分。

| ID | 模块 | 主要目录/目标 | 输入/输出 | 关键依赖 | 证据状态 |
|---|---|---|---|---|---|
| M01 | ggml 张量与后端 | `ggml/`、`ggml/src` | tensor/graph -> backend buffer/device 结果；allocator/scheduler reserve | C11/C++17、平台 SDK | 已确认（allocator/graph 资源专题补证） |
| M02 | llama Runtime 与模型 | `include/llama.h`、`src/`、`src/models/` | GGUF -> model/context/logits/state | M01 | 已确认 |
| M03 | common 应用基础设施 | `common/` | 参数/模板/grammar/cache -> typed params/text | M02、vendor | 已确认 |
| M04 | llama-server | `tools/server/` | HTTP JSON/SSE -> task/result | M02、M03、M06 | 已确认 |
| M05 | CLI 与统一应用 | `tools/cli/`、`app/` | argv/stdin -> text/output | M02-M04、M03 | 已确认 |
| M06 | mtmd 多模态 | `tools/mtmd/` | image/audio/video -> multimodal tokens/embeddings | M01、M02、媒体依赖 | 已确认 |
| M07 | 转换与 GGUF Python | `conversion/`、`gguf-py/` | HF/旧格式 -> GGUF | Python/numpy 等 | 已确认 |
| M08 | 构建、测试与 CI | CMake、`tests/`、`ci/`、`.github/` | options/source -> artifacts/test reports | toolchains、pytest、CTest | 已确认 |
| M09 | Web UI | `tools/ui/` | browser state/API -> chat UI | SvelteKit/Vite/server API | 已确认 |

## 修改影响速查

- 改 tensor op/backend/allocator/scheduler：M01 -> M02 graph 使用者 -> `test-backend-ops`/质量测试（M08）；同时核对 `ggml_tallocr`/`ggml_dyn_tallocr` 和 `sched_reserve`。
- 改模型架构：M02 -> M07 converter、M08 dummy/model tests，可能影响 M04/M05。
- 改 chat template/schema：M03 -> M04 HTTP 行为和 M09 UI；不要放入 M02。
- 改 server task/slot：M04 -> HTTP tests、streaming、UI；必须先读 server development 文档。
- 改公共 API：M02 -> 所有 C/C++ consumer、安装 headers 和 bindings。
- 改 UI：M09 -> embedded assets/llama-server build；运行 `npm run build` 后再重建 server。
- 改 CMake/backend option：M08 -> 目标库、平台 CI、安装包和运行时设备发现。

## 依赖类型分离

1. **编译链接依赖：** CMake target 链接关系，例如 `llama` 链接 `ggml`，server impl 链接 `server-context`、UI 和 httplib。[src/CMakeLists.txt:1-67] [tools/server/CMakeLists.txt:36-53]
2. **静态源码依赖：** include、架构注册、模板 parser 和模型文件命名约定。
3. **运行时调用：** HTTP/CLI 通过 M03/M04 形成 task，再经 M02 进入 M01。
4. **数据依赖：** GGUF metadata/tensor、词表、chat template、模型缓存和 UI IndexedDB。
5. **生命周期依赖：** backend 必须先初始化；server stream GC 在 backend free 前停止；context 依附 model；HTTP reader 依附 response queue。

---

**页尾：** 新模块只有在构建、数据或线程边界确实独立时才新增 ID。

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
