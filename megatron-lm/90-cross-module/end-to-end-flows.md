# 端到端流程

## 流程 1：最小 Core 训练

见 D01。适合确认 Core API 组合方式。

## 流程 2：正式 GPT 预训练

```text
CLI args/YAML
  -> initialize_megatron
  -> model/dataset/optimizer setup
  -> train loop
  -> checkpoint/logging
```

## 流程 3：推理服务

```text
server args
  -> initialize_megatron
  -> build/load model
  -> GPTInferenceWrapper
  -> Static/DynamicInferenceEngine
  -> MegatronServer
  -> requests
```

## 流程 4：checkpoint round-trip

```text
model/optimizer/scheduler/RNG
  -> sharded/training state
  -> save_checkpoint
  -> tracker
  -> load_checkpoint
  -> restored iteration/state
```

所有流程的动态正确性都依赖 CUDA/NCCL/文件系统环境，本知识库当前只记录源码静态证据。