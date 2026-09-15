# 快速开始

- 文档目的：解释 99-roadmap/quick-start.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/llamacpp HEAD 093a2f86c（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 99-roadmap/quick-start.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


## CPU 开发路径

```bash
cmake -B build
cmake --build build --config Release -j$(nproc)
./build/bin/llama --help
```

这些是仓库记录的命令；本次知识库生成没有执行它们。模型推理需要准备 GGUF 文件：[D02](../80-demos/D02-cli-inference/README.md)。

## Server 路径

```bash
cmake --build build --target llama-server -j$(nproc)
./build/bin/llama-server -m PATH_TO_MODEL
```

HTTP 测试安装依赖后从 `tools/server/tests` 执行；单项流程见 [D01](../80-demos/D01-server-chat/README.md)。

## 变更前检查

先读 `AGENTS.md`、`CONTRIBUTING.md`；server 改动先读 `tools/server/README-dev.md`；模型架构改动参考 `docs/development/HOWTO-add-model.md`。不要使用废弃 Makefile。[AGENTS.md:1-6] [CLAUDE.md:4-10]

---

**页尾：** 所有命令状态均为“来源已确认、当前批未验证”。

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
