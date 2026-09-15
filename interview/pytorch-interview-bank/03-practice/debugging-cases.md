# PyTorch 故障排查案例

- 梯度为零：先区分 detach、冻结参数、数值下溢、loss mask 和优化器未更新。
- CUDA OOM：记录 allocation/reserved、峰值生命周期、batch/sequence、activation 和缓存策略。
- DDP hang：核对所有 rank 的 collective 顺序、输入 shape、异常传播和退出屏障。
- `torch.compile` 性能回退：检查 graph break、动态 shape、编译缓存和 eager 基线。
- checkpoint 不兼容：比较 state dict keys、dtype、设备、版本和迁移函数。
