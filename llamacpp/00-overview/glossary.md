# 术语表

- 文档目的：解释 00-overview/glossary.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/llamacpp HEAD 093a2f86c（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](analysis-state.md)。
## 结论摘要

本页聚焦 00-overview/glossary.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


| 术语 | 含义 | 主要证据 |
|---|---|---|
| GGUF | 模型 metadata、词表和 tensor 的文件格式 | `src/llama.cpp` loader |
| ggml | tensor、graph、allocator、scheduler 和 backend 抽象 | `ggml/` CMake |
| backend | CPU/GPU/NPU 的设备实现和注册项 | `src/llama.cpp:122-135` |
| model | 已加载权重、架构和 hparams | `llama_model` |
| context | 一次推理执行的 memory/KV/线程/调度状态 | `llama_init_from_model` |
| batch | 一批 token/embedding 及位置、sequence、输出标志 | `include/llama.h` |
| slot | server 中对应一个 llama sequence 的请求状态 | `tools/server/README-dev.md` |
| ubatch | context 内部实际送入一次 graph 的 micro-batch | `llama_context` 参数 |
| KV cache | attention 的 key/value 历史状态 | `src/llama-context.cpp` |
| common | 面向应用的参数、模板、grammar、采样和缓存工具 | `common/CMakeLists.txt` |
| mtmd | multimodal model/context 适配库 | `tools/mtmd/CMakeLists.txt` |
| SSE | HTTP Server-Sent Events 流式传输 | server stream 文档 |
| router mode | 多 inference 子进程的路由/模型管理模式 | `tools/server/README-dev.md` |
| vocab-only | 只加载词表，不加载权重 tensor | `src/llama.cpp:364-366` |
| tensor split | 多设备按 tensor 规则分配的模式 | `src/llama.cpp:158-220` |

---

**页尾：** 新术语应附源码或开发文档证据，避免把 CLI 参数名误写成稳定 API。

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
