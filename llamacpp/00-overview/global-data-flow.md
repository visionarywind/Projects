# 全局数据流

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
