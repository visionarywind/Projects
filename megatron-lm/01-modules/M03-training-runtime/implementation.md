# M03 设计与实现

## `pretrain` 的编排角色

`pretrain` 不直接执行每个 batch 的数学计算，而是按固定顺序连接初始化、模型/优化器构造、数据构造和训练循环。它还可在正式训练前建立 RL inference model，并把 fault tolerance、telemetry 等横切能力注入运行时。[`megatron/training/training.py:1530+`]

## `get_model`

`get_model` 将一个 provider 转换为模型 list。VP 场景按虚拟 stage 创建多个 chunk，并根据 PP/VP rank 决定哪些 chunk 拥有 embedding/output；随后设置 TP 参数属性、materialize meta device，并按配置包装 DDP、Megatron FSDP 或 Torch FSDP2。[`training.py:2435+`]

## `train_step`

单步顺序是清理 grad buffer、`optimizer.zero_grad`、处理特殊 iterator/shape、调用 schedule、收集 loss/activation 统计、调用 `optimizer.step`，然后聚合更新状态并推进 param scheduler。只有 update successful 时才推进学习率和迭代相关计数。[`training.py:3092+`]

## `train`

`train` 设置 model mode、梯度同步回调和 finalize callback，选择 schedule，并可包装 full-iteration CUDA graph、paged stash、optimizer CUDA graph。主循环还处理 checkpoint、验证、profiling、动态 batch size 和退出请求。[`training.py:4309+`]

## 设计取舍

- 将模型 list 作为 schedule 输入，支持 VP，但使 checkpoint 和日志需要遍历多个 chunk。
- 将 no-sync/finalize 回调写入 config，使 schedule 可复用，但要求配置在训练开始前完整设置。
- 将保存、验证、退出放在 train loop，而非 optimizer，使状态一致性由 M03 负责。