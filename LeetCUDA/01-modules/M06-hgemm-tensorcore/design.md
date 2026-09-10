# M06 设计

M06 保留 CUDA Core、WMMA、MMA PTX、CuTe/WGMMA 和 cuBLAS 对照，让 layout、指令和 pipeline 的取舍可独立比较。NN 路径使用 shared-memory padding，TN 路径使用 CUTLASS/CuTe swizzle；multi-stage/double buffer 负责重叠数据搬运与 Tensor Core 计算。[kernels/hgemm/README.md:195-204,305-307]

这是一组教学/benchmark 实现，不是替代 cuBLAS 的生产接口。
