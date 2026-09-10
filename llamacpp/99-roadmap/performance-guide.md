# 性能路径

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
