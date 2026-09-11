# M06 设计与执行

## 请求路径

```text
prompt/request
  -> controller tokenize
  -> scheduler admission
  -> context allocate/reuse KV blocks
  -> wrapper/model forward
  -> sampling
  -> append token / update request
  -> detokenize and stream/finalize
```

`TextGenerationController` 把 tokenizer、模型 wrapper 和输出后处理隔离于 engine；engine 负责请求生命周期和 batch 调度；context 负责推理状态和 KV cache。

## Static 到 Dynamic

旧 API 仍能用 static context，但当前实现尝试将其转换到 dynamic context。失败时保留 legacy fallback，这意味着安装环境和模型配置可能决定实际路径，不能从调用方名称断定执行 backend。

## 推理接口

| 接口 | 输入 | 输出 |
|---|---|---|
| `get_inference_engine` | model、inference args | engine |
| `engine.add_request` | prompt/sampling/request | request id |
| `engine.generate` | prompts + sampling params | request records |
| `MegatronServer.run` | host/port | 网络服务循环 |

动态 engine 的 KV handoff API 在未启用 disaggregation 时主动抛出 RuntimeError。