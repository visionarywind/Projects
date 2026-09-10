# M07 接口

Python 入口支持 `--B/--H/--N/--D`、reference 开关、accumulator 开关、warmup/iters 和 tag/check 选项。[kernels/flash-attn/flash_attn_mma.py:22-55]

脚本会构造 `[B,H,N,D]` 及为特定实现准备的转置/布局视图；具体 kernel 的 layout 必须查看对应 pybind 和 `.cu`，不能从 tag 名称推断。[kernels/flash-attn/flash_attn_mma.py:416-443]
