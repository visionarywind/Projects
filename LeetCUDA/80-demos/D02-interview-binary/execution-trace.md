# D02-S02 执行追踪

## 构建控制流

```text
build.sh --arch
  → select gencode/defines/libs/output
  → nvcc compile notes-v2.cu
  → link notes-v2.o
  → notes_v2_<arch>.bin
```

证据：[kernels/interview/build.sh:45-81,134-185]。

## BlockReduce 正常路径

```text
main/phase dispatch
  → test_block_reduce(N)
  → malloc h_a + CPU reference
  → cudaMalloc d_a/d_y
  → H2D + cudaMemset
  → block_reduce_all<<<grid,128>>>
  → check(cudaGetLastError)
  → check(cudaDeviceSynchronize)
  → D2H result
  → fabs error / print
  → free + cudaFree
```

证据：[kernels/interview/notes-v2.cu:510-542]。这是最短完整资源 Demo。

## HGEMM benchmark 路径

```text
bench_hgemm_tflops
  → allocate inputs/reference/output
  → cublas reference
  → choose stages/swizzle
  → check_smem_feasible
  → warmup
  → events + repeated kernel
  → event elapsed time
  → D2H + max error
  → destroy events / free / cublas destroy
```

`launch_timed_hgemm_mma` 的 tile/grid/SMEM/warmup/events 见 `[kernels/interview/notes-v2.cu:2471-2505]`；整体 benchmark 见 `[kernels/interview/notes-v2.cu:2420-2579]`。

## FlashAttention benchmark 路径

D64/D128 dispatch 后按 Br/Bc 和 padding/swizzle 形成 layout tag；不满足 `seqlen < Br`、SMEM 或对齐条件的配置可能跳过。独立 timing stream + events 包住 measured kernel，再 D2H 做 correctness。[kernels/interview/notes-v2.cu:3094-3190]。

TMA MMA WS/FA3 路径额外创建 TensorMap、dynamic shared memory 和 consumer warp group；结束时清理 map/stream/events/buffer。[kernels/interview/notes-v2.cu:3400-3497,3519-3579]。

## 错误流

统一 helper：

```cpp
check(cudaError_t, msg)
  → fprintf(cudaGetErrorString)
  → exit(EXIT_FAILURE)
```

证据：[kernels/interview/notes-v2.cu:92-97]。不过并非每个内部同步都包在 `check()` 中；HGEMM warmup 后同步是源码可见例外。

## 外部副作用

D02 的副作用是编译生成 object/binary、CUDA allocation/kernel execution、stdout/stderr 输出和可能的进程 exit；没有网络服务或持久数据库。
