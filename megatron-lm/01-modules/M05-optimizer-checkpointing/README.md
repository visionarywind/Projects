# M05 优化器与分布式检查点

- 文档目的：解释 01-modules/M05-optimizer-checkpointing/README.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：`3703d4e33a3a2b2d11ebcc8e41f45af7ce7d1eda`
- 证据状态：静态源码分析；未运行保存/恢复实验。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M05-optimizer-checkpointing/README.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。

## 主链

```text
model parameters
  -> _get_param_groups / buffers
  -> get_megatron_optimizer
    -> FP32 / mixed precision / distributed / chained optimizer
  -> train_step optimizer.step
    -> update_successful, grad_norm, zeros
  -> scheduler.step

model.sharded_state_dict + optimizer state
  -> save_checkpoint/generate_state_dict
  -> dist_checkpointing.save
  -> load_checkpoint / dist_checkpointing.load
```

## optimizer factory

`get_megatron_optimizer` 根据参数组、混合精度、distributed optimizer、FSDP、CPU offload、MuP、TE/Apex/PyTorch fallback 等配置选择实现。`ChainedOptimizer` 可组合多个 optimizer。关键实现位于 `core/optimizer/__init__.py` 和 `optimizer.py`。[`core/optimizer/__init__.py:13-约 1100`]

## 更新不变量

`train_step` 只有在 `optimizer.step()` 报告 update successful 时推进 scheduler；梯度 norm 和 zero count 用于日志/诊断。distributed optimizer 的 shard 所有权不能与普通 model state 混淆。

## checkpoint

传统 checkpoint 按 iteration、TP/PP/EP rank 和 distributed optimizer 形成路径；distributed checkpoint 使用 sharded state dict 与 metadata。save/load 同时涉及模型、optimizer、scheduler、RNG、tokenizer、数据加载状态和 tracker。[`training/checkpointing.py:176-230,611+,1567+,2469+`]

## 风险

checkpoint 兼容性取决于模型结构和并行配置；`check_checkpoint_args` 会拒绝/警告关键结构参数不匹配。异步保存和 non-persistent/local checkpoint 的清理及恢复语义尚未实测。

启用 optimizer CUDA Graph 时，optimizer step 与 full-iteration graph 可能共享 capture stream 和 graph memory pool；NCCL `MemPool` 注册/注销必须覆盖实际通信 group。相关资源边界见 [CUDA Graph 与显存池生命周期](../../90-cross-module/cuda-graph-resource-lifecycle.md)，当前未运行验证。

## 文档元数据（规范补充）

- 文档目的：说明 `01-modules/M05-optimizer-checkpointing/README.md` 的源码分析范围、结论和维护入口。
- 适用范围：当前项目对应模块/入口的静态源码与测试分析。
- 对应源码版本：以本项目 `00-overview/analysis-state.md` 或同页版本字段为准。
- 证据状态：静态源码证据；未执行的构建、测试、GPU、网络或多进程行为保持“未验证”。
- 最后更新：2026-09-15
- 前置阅读：本项目根 README 与 `00-overview/analysis-state.md`。
- 后续阅读：本模块/示例的实现、测试和风险页面。

## 深度审计

| 分析对象 | 入口落地 | 正常路径 | 分支 | 异常 | 清理 | 数据生命周期 | 执行上下文 | 行级证据 | Demo 映射 | 状态/缺口 |
|---|---|---|---|---|---|---|---|---|---|---|
| `01-modules/M05-optimizer-checkpointing/README.md` | 已完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 已映射或不适用 | 部分完成：动态行为、边界或专用变体仍需验证 |

## 相关文档
- [项目入口](../../README.md)
- [分析状态](../../00-overview/analysis-state.md)
- [源码证据索引](../../00-overview/evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](../../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](../../00-overview/analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
