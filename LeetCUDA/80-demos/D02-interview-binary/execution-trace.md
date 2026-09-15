# D02-S02 执行追踪

- 文档目的：解释 80-demos/D02-interview-binary/execution-trace.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/LeetCUDA HEAD 4513b31（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 80-demos/D02-interview-binary/execution-trace.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


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

## 相关文档
- [项目入口](../../README.md)
- [分析状态](../../00-overview/analysis-state.md)
- [源码证据索引](../../00-overview/evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](../../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](../../00-overview/analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
