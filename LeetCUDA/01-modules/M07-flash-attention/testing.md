# M07 测试

覆盖 B/H/N/D 小组合、D=32/64/128/256 及更大 D 的过滤行为；比较 unfused torch、SDPA、官方 flash-attn（若安装）和不同 accumulator。检查 causal/非 causal（如接口支持）、非整 tile、极值输入、误差阈值和输出 layout。当前未在真实 CUDA 环境执行。

README 给出的依赖是 flash-attn、PyTorch 和 CUDA 的特定版本范围，需按目标环境确认。[kernels/flash-attn/README.md:106-118]
