# M02 实现机制

## `initialize_model_parallel`

`initialize_model_parallel` 首先校验 distributed 状态和并行参数，计算模型规模与 data parallel size；随后分别创建 decoder/expert `RankGenerator`。rank generator 根据 `order` 和各轴尺寸生成正交 rank groups。函数按特定顺序创建 DP/CP、partial-DP、GTP、TP、PP、embedding、position-embedding 等 group，并可同时创建 Gloo group、应用 NCCL 配置、SHARP 和高优先级 stream 配置。[`parallel_state.py:600-1029`]

注意：`world_size` 的计算并非只看 TP；CP、PP、GTP 和 expert 侧尺寸会改变可整除约束。初始化还会保存当前 rank 对应的全局 ranks，供 accessor 返回。

## `RankGenerator`

`RankGenerator(tp, ep, dp, pp, cp, order, rank_offset, gtp_remat)` 将未参与计算的轴压缩为 1，并拒绝 order 中缺失的非 1 轴。`get_mask` 决定哪些轴形成 group，`get_ranks` 生成 rank 列表；其核心组合算法是 `generate_masked_orthogonal_rank_groups`。[`parallel_state.py:269-570`]

## Schedule 分派

`get_forward_backward_func` 的选择规则：PP=1 走 `forward_backward_no_pipelining`；PP>1 且无 VP 走非交错调度；PP>1 且存在 VP 走交错调度；多模块 process-group collection 走多模块兼容的非交错路径。[`schedules.py:53-168`]

## 非交错调度

调度先计算当前 stage 的 warmup microbatch 数，得到发送/接收 tensor shapes；warmup 只 forward，steady-state 交替执行 forward/backward，cooldown 补齐剩余 backward。每个阶段通过 `P2PCommunicator` 收发 activation/gradient，并在完成发送后释放不再需要的 output storage。[`schedules.py:2147-约 2660`]

## 无 pipeline 调度

无 pipeline 时逐 microbatch 调用 `forward_step` 和 backward。除最后一个 microbatch 外使用 `no_sync_func`，让梯度同步延迟到最后一次；结束后统一调用 `config.finalize_model_grads_func`。因此该函数既计算 loss，也承担 schedule 级的同步边界。[`schedules.py:723-928`]

## 输出释放

`deallocate_output_tensor` 不是真正删除 Python tensor，而是保留标量 storage/grad_fn 关系并配合 `custom_backward` 完成反向。TransformerBlock 对最终输出创建 viewless tensor，正是为了避免该机制遇到 view tensor。[`schedules.py:171-220`; `transformer_block.py:762-780`]