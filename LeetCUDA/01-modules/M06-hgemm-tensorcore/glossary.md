# M06 HGEMM 术语

- `WMMA`：CUDA C++ 的 warp-level matrix multiply-accumulate API。
- `MMA`：更接近 PTX 指令形态的 matrix multiply-accumulate 路径。
- `WGMMA`：Hopper warp-group matrix multiply-accumulate 指令路径。
- `NN/TN`：A/B 矩阵布局组合；具体 stride 由实现和 launcher 约定。
- `stage`：用于重叠 tile 搬运和计算的 pipeline 缓冲阶段。
- `SMEM padding`：在 shared memory layout 中增加步长，降低 bank conflict。
- `SMEM swizzle`：重排 shared-memory 地址/线程映射以改善访问模式。
- `accumulator`：Tensor Core 乘加结果使用的寄存器或 fragment 存储。
- `CuTe/CUTLASS`：用于描述 layout、tile 和高性能矩阵运算的 NVIDIA 模板抽象。

这些术语不能替代具体 kernel 的实例化参数；实际 layout 和可用指令需以目标架构源码为准。
