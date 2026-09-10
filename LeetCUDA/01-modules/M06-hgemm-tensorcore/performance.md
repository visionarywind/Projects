# M06 性能路径

## 关键路径

```text
global A/B
  → cp.async overlap
  → padded/swizzled shared stage
  → ldmatrix
  → mma.sync.m16n8k16
  → register accumulator
  → warp shuffle + vector store
```

证据：[kernels/hgemm/mma/basic/hgemm_mma_stage.cu:691-839,1850-1885]。

## 可观测指标

- kernel elapsed time / TFLOPS：脚本用 `2*M*N*K/time`，见 `[kernels/hgemm/hgemm.py:210-328]`；
- shared bank conflicts、global memory throughput、tensor utilization、occupancy、register pressure：需要 Nsight Compute，当前未执行；
- stage/padding/swizzle 对性能的因果结论：只能在同一 GPU、同一尺寸和相同 warmup/iterations 下比较。

## 容量约束

动态 shared-memory launcher 请求最多 98304 bytes，并组合 stage/padding/grid。[kernels/hgemm/mma/basic/hgemm_mma_stage.cu:1900-1959]。更多 stage 可能提高 overlap，也可能降低 occupancy；这是待 profile 假设，不是保证。

## 验证顺序

1. torch/cuBLAS correctness；
2. 非整 tile 与不同 layout；
3. compute-sanitizer；
4. Nsight Systems/Compute；
5. 最后比较 TFLOPS。

当前均未验证。
