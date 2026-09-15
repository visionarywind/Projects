# 全局数据流

- 文档目的：解释 00-overview/global-data-flow.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/llamacpp HEAD 093a2f86c（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](analysis-state.md)。
## 结论摘要

本页聚焦 00-overview/global-data-flow.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


```text
GGUF bytes
  -> GGUF metadata / tensor weights / vocab
  -> llama_model + architecture hparams
  -> llama_context + memory/KV
  -> llama_batch (tokens, positions, seq ids, output flags)
  -> model graph / ggml tensor
  -> backend scheduler / device buffers
  -> logits or embeddings
  -> sampler / response formatter
  -> CLI text or HTTP JSON/SSE
```

**已确认：** loader 按 `load_hparams`、`load_vocab`、`load_tensors` 的顺序建立 model 状态，失败以错误返回或取消返回。[src/llama.cpp:316-377]

**已确认：** batch 的 output 标志用于选择需要 logits 的位置；server 在 prompt 完成时只对最后 token 设置 output。[tools/server/server-context.cpp:3591-3604]

**已确认：** server completion handler 将 JSON prompt tokenize 为 `server_tokens`，封装成 task，非流式等待全部 result，流式把 result 转为 SSE。[tools/server/server-context.cpp:4275-4344] [tools/server/server-context.cpp:4348-4414]

## 数据所有权

- loader/model：模型文件映射和 tensor 权重由 M02/ggml 生命周期管理。
- batch：一次 decode 的输入视图；server batch 是多个 slot 共享的临时聚合。
- queue task/result：HTTP 与 inference thread 间的消息所有权通过 move/队列转移。
- SSE：resumable stream 将原始 SSE bytes 放入有界 ring；consumer 用 offset replay。[tools/server/README-dev.md:121-149]

## 数据依赖风险

GGUF metadata 与实际 tensor naming 必须匹配具体架构；chat template 影响 token 数和模型输出；KV cache 容量约束可能导致 decode 重试或请求失败。后两项的错误行为分别见 [global-error-model.md](global-error-model.md)。

---

**页尾：** 数据流图省略了 speculative、LoRA、MTMD 和 router 分支；这些在对应模块页列出。

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
