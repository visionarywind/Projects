# 性能指南

- 对应源码版本：`main` / `4513b31`。
- 最后更新：2026-09-10

## 优化顺序

1. 先确认输出正确。
2. 确认计时包含同步且排除首次编译。
3. 判断瓶颈是 global memory、shared memory、计算、同步还是 occupancy。
4. 再选择 vectorization、tiling、padding/swizzle、double buffer、Tensor Core 或融合。
5. 与 torch/cuBLAS/官方 FlashAttention 在相同条件下比较。

## 机制对照

- elementwise：pack/vector load。
- SGEMM：shared tile、thread tile、double buffer。
- HGEMM：WMMA/MMA/CuTe/WGMMA、padding/swizzle、stages。
- FlashAttention：融合、online softmax、Q/K/V tile 复用。
- NMS：mask 并行化受 ordered resolve 限制。

## 纪律

历史 benchmark 只用于学习趋势，不能作为当前硬件保证；提交性能改动时同时记录环境和 correctness。

证据：`[kernels/sgemm/sgemm.cu:88-482]`；`[kernels/hgemm/README.md:195-204,305-307]`；`[kernels/flash-attn/README.md:14-26]`。
