# M02 接口契约

- 文档目的：解释 01-modules/M02-parallelism/interfaces.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/megatron-lm HEAD 3703d4e33（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M02-parallelism/interfaces.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


| 接口 | 输入 | 输出/副作用 | 约束 |
|---|---|---|---|
| `initialize_model_parallel` | TP/PP/VP/CP/EP/order 等 | 创建 process groups、设置全局状态 | torch.distributed 已初始化；规模可整除 |
| `RankGenerator.get_ranks` | 轴 mask/尺寸 | rank group 列表 | order 必须包含所有非 1 轴 |
| `get_forward_backward_func` | PP size、VP size、可选多模块 PG | schedule callable | PP/VP 组合必须有效 |
| schedule callable | forward step、iterator、model、microbatch、shape、communicator | reduced losses/副作用 | model 与 iterator 数量匹配 |
| `P2PCommunicator` | activation/gradient、tensor shapes | stage 间通信 tensor | 首尾 stage 方向不同 |
| `destroy_model_parallel` | 无 | 清理并行状态 | 调用方应停止后续 group 使用 |

`forward_step_func` 的返回值通常是 `(output_tensor, loss_func)`；schedule 不要求模型知道具体 loss 聚合策略。

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
