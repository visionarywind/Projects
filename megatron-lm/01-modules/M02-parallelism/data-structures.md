# M02 数据结构与生命周期

| 对象 | 创建 | 使用 | 释放/持有 |
|---|---|---|---|
| rank list | `RankGenerator` | `torch.distributed.new_group` | Python 临时列表；group 由 distributed 持有 |
| ProcessGroup | `create_group` | collective/P2P | `destroy_model_parallel` 清理引用 |
| global memory buffer | parallel state | 通信/临时 tensor | parallel-state destroy 时释放 |
| microbatch state | schedule | 输入、输出、梯度栈 | 每次 schedule 调用结束 |
| activation tensor | 前一 stage/model forward | 下一 stage/backward | 发送后可伪释放 output storage |
| tensor shapes | `get_tensor_shapes` | P2P recv/send 分配 | 单次 schedule 使用 |

重要所有权边界：模型产生 activation，schedule 管理跨 stage 传递和保存；optimizer 不应直接依赖 schedule 内部的 microbatch 栈。