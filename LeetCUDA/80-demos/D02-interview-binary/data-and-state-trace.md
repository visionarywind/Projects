# D02-S03 数据和状态追踪

## 生命周期表

| 阶段 | host 状态 | device/辅助状态 | 释放 |
|---|---|---|---|
| 初始化 | `malloc` 输入/reference/output | `cudaMalloc` A/B/C 等 | 函数尾 |
| reference | CPU double/half reference 或 cuBLAS 输出 | reference buffers | 函数尾 |
| transfer | host 输入已填充 | H2D 完成后 device 可读 | host 尾 |
| launch | 参数、grid/block、SMEM 字节 | kernel 异步更新 C | sync/event |
| observe | D2H 到 host 输出 | event/stream 完成 | event/stream destroy |
| validate | `max_err`/TFLOPS | device output 已读取 | cudaFree |

BlockReduce 的完整实例见 `[kernels/interview/notes-v2.cu:510-542]`；HGEMM buffer/timing 清理见 `[kernels/interview/notes-v2.cu:2420-2579]`；FA/TMA 的 stream/map 清理见 `[kernels/interview/notes-v2.cu:3094-3190,3400-3497]`。

## 执行状态

```text
host initialized
  → H2D pending/completed
  → kernel queued on selected stream
  → launch checked (部分路径)
  → stream/device synchronized
  → D2H completed
  → error/TFLOPS observable
  → all resources destroyed
```

CUDA launch 默认可能异步；`cudaGetLastError` 只检查 launch/先前错误，`cudaDeviceSynchronize`/event synchronize 才建立完成观察点。具体路径是否每一步调用 `check` 以函数源码为准。

## 架构状态

`--arch` 不只是输出命名：它决定 gencode 和宏，进而决定是否编译 WGMMA/TMA/cuDNN/CuTe 代码。[kernels/interview/build.sh:45-81]。因此同一 CLI 在不同 binary 上不一定有相同 phase/benchmark 集合。

## 清理注意

TMA map、events、streams、cuBLAS handle 和 host/device allocation 是不同资源类型；不能用 `cudaFree` 代替 event/stream/map destroy。代表函数已记录显式清理，但全文件所有 phase 尚未逐个审计。
