# M06 故障与验证清单

| 现象 | 首查位置 | 静态可得结论 |
|---|---|---|
| engine 初始化失败 | `StaticInferenceEngine.__init__` | 可能回退 legacy static |
| request 超出容量 | dynamic engine/scheduler | admission 受 max tokens/requests 限制 |
| KV handoff 不可用 | dynamic engine handoff stubs | 需 disaggregation engine |
| logits 未 gather | GPTModel forward | inference mode 要求 gather |
| 服务无响应 | server rank gate、distributed broadcast | 需检查首 stage/TP/EP rank 和进程同步 |

验证应分别覆盖单进程 offline、动态 batching、多 rank inference、server request、CUDA graph 和 disaggregation；本知识库当前仅完成源码阅读。