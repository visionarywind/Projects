# M06 调用链

## Python 动态扩展/benchmark

```text
hgemm.py args
 -> tools/utils.py:get_build_sources / build helper
 -> pybind/hgemm.cc exported variant
 -> launcher / template instantiation
 -> cp.async staged shared memory
 -> ldmatrix register fragments
 -> mma.sync.aligned.m16n8k16 / accumulator
 -> collective shuffle + vectorized store
 -> synchronize, correctness, TFLOPS
```

代表 staged MMA 的实际实现证据为 `[kernels/hgemm/mma/basic/hgemm_mma_stage.cu:120-142,691-839,1850-1885]`。动态 shared-memory launcher 见 `[kernels/hgemm/mma/basic/hgemm_mma_stage.cu:1900-1959]`。

## 资源和错误边界

cublas benchmark 显式 init/destroy handle；Python benchmark 负责 warmup/synchronize；PyTorch tensor 由调用者/allocator 持有。自定义 kernel 的统一 launch-error 检查未从所有导出函数确认，因此标为部分完成。[kernels/hgemm/hgemm.py:210-328]

## 变体边界

WMMA、MMA、TN/swizzle、CuTe、WGMMA 和 cuBLAS 通过不同导出/源文件落地，不能从公共 wrapper 推断相同 layout。完整 family call graph 仍为部分完成。
