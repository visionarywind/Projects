# M02 设计与取舍

- 对应源码版本：`3703d4e33a3a2b2d11ebcc8e41f45af7ce7d1eda`
- 证据状态：静态源码分析

## 设计原则

1. **拓扑与计算解耦**：`parallel_state` 提供 group/accessor，模型层和 schedule 通过 group 使用通信，而不自行推导全局 rank。
2. **正交轴统一生成**：`RankGenerator` 将 TP、DP、PP、CP、EP 及 GTP 映射到 shape/stride，减少为每种组合手写 rank 列表。
3. **schedule 显式选择**：`get_forward_backward_func` 以 PP/VP 和多模块 collection 为输入，返回一个统一签名的函数。
4. **通信由调度拥有**：模型只输出 hidden states；stage 间发送/接收和 activation 生命周期由 pipeline schedule 管理。

## 取舍

- 全局 `parallel_state` 便于兼容旧调用方，但显式 `ProcessGroupCollection` 更利于多模块、多网格场景。
- warmup/1F1B/cooldown 能隐藏流水线气泡，但增加 tensor 保存、P2P shape 和调试复杂度。
- `deallocate_output_tensor` 通过替换已发送输出的 storage 降低显存，但依赖自定义 backward 和 viewless tensor 不变量。
- CP 不切分权重，因此需要在 backward 额外归约复制权重的梯度。

## 边界

M02 不决定 optimizer 是否 sharded；它只提供 optimizer 和 gradient finalize 所需的通信域。M02 也不把 NCCL 的实际算法或带宽结果当作源码可推导事实。