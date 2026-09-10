# M06 接口

Python CLI 选择 `--wmma`、`--mma`、`--cuda`、`--cute-tn` 及 `--*-all` 等家族，并传递 M/N/K、warmup、iters 等参数。[kernels/hgemm/hgemm.py:18-177]

扩展由 `CUDAExtension(name="toy_hgemm", ...)` 构建，include 和 NVCC flags 覆盖多个实现目录。[kernels/hgemm/setup.py:42-67]

实现之间的 layout、accumulator、可用 SM 和 stride 不是统一 ABI，必须按导出函数核对。
