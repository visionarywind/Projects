# M02 接口契约

| 接口 | 输入 | 输出/副作用 | 约束 |
|---|---|---|---|
| `initialize_model_parallel` | TP/PP/VP/CP/EP/order 等 | 创建 process groups、设置全局状态 | torch.distributed 已初始化；规模可整除 |
| `RankGenerator.get_ranks` | 轴 mask/尺寸 | rank group 列表 | order 必须包含所有非 1 轴 |
| `get_forward_backward_func` | PP size、VP size、可选多模块 PG | schedule callable | PP/VP 组合必须有效 |
| schedule callable | forward step、iterator、model、microbatch、shape、communicator | reduced losses/副作用 | model 与 iterator 数量匹配 |
| `P2PCommunicator` | activation/gradient、tensor shapes | stage 间通信 tensor | 首尾 stage 方向不同 |
| `destroy_model_parallel` | 无 | 清理并行状态 | 调用方应停止后续 group 使用 |

`forward_step_func` 的返回值通常是 `(output_tensor, loss_func)`；schedule 不要求模型知道具体 loss 聚合策略。