# M05 设计与实现

## 参数组

参数组阶段根据参数属性和配置构造 learning-rate、weight-decay、decoupled-LR 等组，并可携带 distributed buffer 信息。该阶段把模型参数的并行属性传给 optimizer factory，而不是复制模型参数。[`core/optimizer/__init__.py:_get_param_groups*`]

## 混合精度

`MixedPrecisionOptimizer` 维护低精度参数/主参数、loss scaling 和溢出处理；`FP32Optimizer` 走普通精度路径。具体 wrapper 选择依赖 config 和可选 backend，不能仅凭类名推导最终运行 kernel。

## distributed optimizer

`DistributedOptimizer` 以参数 range 和 DP shard 管理 optimizer state，保存/恢复时需要和 model sharding metadata 对齐。多个 optimizer 通过 `ChainedOptimizer` 汇总 step 结果。[`core/optimizer/distrib_optimizer.py`]

## checkpoint 两层

训练 checkpointing 负责命名、tracker、参数校验和全局训练状态；`dist_checkpointing` 负责 sharded mapping 的存储/加载。`GPTModel.sharded_state_dict` 是模型到 checkpoint 层的适配边界。