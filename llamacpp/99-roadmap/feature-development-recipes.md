# 功能开发配方

- 文档目的：解释 99-roadmap/feature-development-recipes.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/llamacpp HEAD 093a2f86c（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 99-roadmap/feature-development-recipes.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


## 新模型架构

1. 先读 `docs/development/HOWTO-add-model.md`。
2. 增加架构 metadata/枚举与 `src/models/` 实现。
3. 在 `llama_model_mapping` 接入具体类并保留未知架构失败路径。[src/llama-model.cpp:343-369]
4. 同步 `conversion/` 和必要 GGUF metadata。
5. 使用 dummy model/架构测试和实际模型质量测试；不要声称未运行的结果。

## Server endpoint

1. 明确 feature 属于 server scope；外部文件能力默认关闭。[tools/server/README-dev.md:7-32]
2. 在 HTTP 层解析 JSON/template/tokenize；转换成 native task，不把 raw JSON 传入 slot。[tools/server/README-dev.md:86-103]
3. 通过 queue/result 与 context 通信，避免在 inference thread 做重 post-processing。
4. 覆盖非流式、流式、断开和错误清理分支。

## Backend/operator

保持 ggml backend 抽象；同时检查 runtime graph 使用者和 `test-backend-ops`。具体 kernel 修改需要目标硬件验证。

---

**页尾：** 这是现有边界的开发配方，不是替代 maintainer 设计审查的审批流程。

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
