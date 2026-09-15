# 证据索引

- 文档目的：解释 00-overview/evidence-index.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/llamacpp HEAD 093a2f86c（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](analysis-state.md)。
## 结论摘要

本页聚焦 00-overview/evidence-index.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


## 构建与版本

- 版本、选项和子目录：[CMakeLists.txt:5-23] [CMakeLists.txt:129-149] [CMakeLists.txt:204-258]
- ggml backend 选项：[ggml/CMakeLists.txt:86-90] [ggml/CMakeLists.txt:189-281]
- server target：[tools/server/CMakeLists.txt:36-67]
- mtmd target 和解耦：[tools/mtmd/CMakeLists.txt:15-35] [tools/mtmd/CMakeLists.txt:83-115]

## Runtime

- 池化与资源管理专题：[source/llamacpp/src/llama-kv-cache.cpp:65-304]、[source/llamacpp/src/llama-memory-recurrent.cpp:20-159]、[source/llamacpp/src/llama-context.cpp:582-721]、[source/llamacpp/tools/server/server-context.cpp:1249-1433]。
- ggml tensor allocator：[source/llamacpp/ggml/src/ggml-alloc.c:53-91,94-177]；graph reserve/reset：[source/llamacpp/src/llama-context.cpp:582-712,819-843,2418-2464]。

- backend init：[src/llama.cpp:122-151]
- device preparation：[src/llama.cpp:158-312]
- model load：[src/llama.cpp:316-444]
- architecture dispatch：[src/llama-model.cpp:343-369]
- context validation：[src/llama-context.cpp:3660-3765]
- graph scheduler：[src/llama-context.cpp:2492-2518]
- decode wrapper：[src/llama-context.cpp:4247-4255]

## Server

- startup/error wrapper：[tools/server/server.cpp:52-112]
- batch/decode/error：[tools/server/server-context.cpp:3645-3781]
- completion handler：[tools/server/server-context.cpp:4227-4414]
- architecture/thread/stream overview：[tools/server/README-dev.md:37-149]
- sleep and wake：[tools/server/README-dev.md:294-322]

## Tests/Demo

- CTest helpers and fixtures：[tests/CMakeLists.txt:1-117] [tests/CMakeLists.txt:201-257]
- server runner：[tools/server/tests/tests.sh:1-20]
- chat completion assertions：[tools/server/tests/unit/test_chat_completion.py:28-51]
- stream assertions：[tools/server/tests/unit/test_chat_completion.py:82-120]
- UI scripts：[tools/ui/package.json:7-27]

---

**页尾：** 引用行号针对版本快照；更新源码后应重新核验。

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
