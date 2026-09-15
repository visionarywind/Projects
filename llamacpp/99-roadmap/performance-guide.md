# 性能路径

- 文档目的：解释 99-roadmap/performance-guide.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/llamacpp HEAD 093a2f86c（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 99-roadmap/performance-guide.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


## 主要计算路径

server 文档将 `update_slots` 中的 `llama_decode` 标为主要计算瓶颈；所有 slots 共享 batch，decode 后采样或取 embeddings。[tools/server/README-dev.md:76-84]

## 影响因素（无测量结论）

- batch/ubatch 和 slot 并发；
- backend placement、tensor split、设备间 transfer；
- KV cache 类型、flash attention、context 长度；
- prompt cache/checkpoint、speculative decoding；
- HTTP worker 是否承担了重 post-processing。

## 观测命令

`llama-bench`、runtime perf output、server metrics 和 `llama-perplexity` 可用于不同维度验证；具体命令和结果必须绑定模型、commit、backend、线程和参数。本批未执行 benchmark。

## 修改建议

先区分 prompt eval、generation eval、queue wait、HTTP formatting 和 device transfer，再优化；不要只看端到端单次 wall time。上述分类是 profiling 方法推断。

---

**页尾：** 本页没有提供 tokens/s、延迟或显存承诺。

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
