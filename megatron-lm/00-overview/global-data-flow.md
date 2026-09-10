# 全局数据流

- 文档目的：追踪训练样本从数据源到参数与 checkpoint 的变化。
- 适用范围：D01 和 GPT 训练主路径。
- 对应源码版本：`8190837c2b6ce176a431bc2a6ffd3439507648a7`
- 证据状态：已确认（静态）
- 最后更新：2026-09-10
- 前置阅读：[架构](architecture.md)
- 后续阅读：[D01 数据轨迹](../80-demos/D01-simple-mcore-training/data-and-state-trace.md)

## 结论摘要

D01 中 mock dataset 产出字典，`forward_step_func` 取出 tokens、position_ids、mask、labels，并把它们传给 `GPTModel`；模型输出被 loss function 聚合，autograd 形成梯度，`finalize_model_grads` 进行并行同步，Adam 更新参数，sharded state dict 将最终模型变成 checkpoint 文件。

```mermaid
flowchart LR
  DS[MockGPTDataset] --> DL[DataLoader iterator]
  DL --> F[forward_step_func]
  F --> X[tokens/positions/masks/labels]
  X --> G[GPTModel embedding + decoder + output]
  G --> L[loss_func + loss mask]
  L --> A[autograd gradients]
  A --> R[finalize_model_grads]
  R --> U[Adam.step parameters]
  U --> SH[sharded_state_dict]
  SH --> CK[dist_checkpointing files]
```

证据：[examples/run_simple_mcore_train_loop.py:81-120,123-165,168-217,246-283]。这里的箭头表示 Python tensor 数据或梯度流；DataLoader 的具体 batch 字段由 `MockGPTDataset`/collate 运行时决定，D01 静态代码可确认读取字段名但未运行确认形状。

## 数据边界表

| 边界 | 输入 | 输出/变化 | 主要证据 |
|---|---|---|---|
| dataset→loader | mock 样本 | batch dict | `examples/...:99-120` |
| loader→forward | batch dict | device tensors | `examples/...:154-160` |
| forward→model | tokens/positions/mask/labels | output tensor | `examples/...:161-165` |
| model→loss | logits/loss output | scalar loss + metrics | `examples/...:143-152` |
| gradients→optimizer | parameter grads | updated parameters | `examples/...:263-268` |
| model→checkpoint | sharded state dict | directory shards | `examples/...:183-216` |

## 资源流

GPU device 由每个 rank 设置；模型被移动到 CUDA；分布式 groups 负责通信；checkpoint 目录由进程写入。数据 iterator 的 Python 对象由调用方创建并传给 schedule，activation 的生命周期由 autograd/pipeline schedule 管理。

## 相关文档

- [跨模块共享数据](../90-cross-module/shared-data-and-types.md)
- [M04 数据管线](../01-modules/M04-data-pipeline/README.md)

## 源码证据摘要

`examples/run_simple_mcore_train_loop.py:99-165,183-216,246-283`。

## 未解决问题

需运行 D01 确认实际 batch shape、loss 数值和 checkpoint 目录布局。

## 下一步阅读建议

查看 D01 的状态快照表。
