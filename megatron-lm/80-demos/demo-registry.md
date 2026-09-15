# D01 最小 Core GPT 训练

- 文档目的：用仓库自带最小示例串联 M01-M05。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：`3703d4e33a3a2b2d11ebcc8e41f45af7ce7d1eda`
- 证据状态：静态源码分析；命令未运行。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 80-demos/demo-registry.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。

## 入口

`examples/run_simple_mcore_train_loop.py` 创建 2 层小 GPT、MockGPTDataset、Core DDP、Adam、forward/backward schedule，并保存/加载 distributed checkpoint。[`examples/run_simple_mcore_train_loop.py:28-283`]

## 运行命令（未验证）

```bash
torchrun --nproc_per_node=2 examples/run_simple_mcore_train_loop.py
```

需要 CUDA、NCCL、两张 GPU 和上游依赖环境。

## 阶段

| 阶段 | 源码 | 模块 |
|---|---|---|
| distributed init | `initialize_distributed` | M02 |
| model provider | `model_provider` | M01 |
| dataset iterator | `get_train_data_iterator` | M04 |
| forward step | `forward_step_func` | M01/M03 |
| schedule | `get_forward_backward_func` | M02/M03 |
| grad finalize/update | `finalize_model_grads` + Adam | M05 |
| checkpoint | `dist_checkpointing.save/load` | M05 |

## 价值

D01 不覆盖正式 `pretrain` 参数系统和完整 checkpoint tracker，但它最小化外部依赖，适合作为理解 Core API 的第一条可读链。

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
