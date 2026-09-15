# M06 推理与工具

- 对应源码版本：`3703d4e33a3a2b2d11ebcc8e41f45af7ce7d1eda`
- 证据状态：静态源码分析；未启动推理服务。

## 组成

```text
server/CLI
  -> model builder + checkpoint load
  -> GPTInferenceWrapper
  -> TextGenerationController
  -> Static/DynamicInferenceEngine
    -> Scheduler
    -> Dynamic/StaticInferenceContext
    -> KV block allocator
    -> sampling backend
  -> request/output/stream
```

## Dynamic engine

动态 engine 允许不同请求拥有不同 prompt/output 当前长度，并以总 token 容量约束 batch；context 管理 block-level KV cache，engine 管理 waiting/running/paused 等请求状态。[`core/inference/engines/dynamic_engine.py:292-427`]

## Static compatibility

`StaticInferenceEngine` 接收 static context，但非 legacy 路径会创建 `DynamicInferenceContext` 和 `DynamicInferenceEngine`；初始化异常时恢复原 static context 并回退 legacy。代码明确将其标记为未来弃用。[`core/inference/engines/static_engine.py:35-132`]

## 服务入口

`tools/run_text_generation_server.py` 解析参数、初始化 Megatron、构建/加载模型、创建 static engine，并在首个 PP/TP/EP rank 上运行 `MegatronServer`。[`tools/run_text_generation_server.py:51-192`]

## 未验证

KV cache 命中率、跨 rank 服务行为、KV handoff 和网络部署都需硬件/依赖实测。CUDA Graph 的 graph pool、static input、capture/replay 和 reset 语义见 [CUDA Graph 与显存池生命周期](../../90-cross-module/cuda-graph-resource-lifecycle.md)，同样尚未 GPU 实测。
