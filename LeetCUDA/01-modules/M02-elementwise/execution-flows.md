# M02 执行流程

- 文档目的：按源码步骤追踪 elementwise 代表实现的初始化、调用、同步和失败边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：`main` / `4513b31`。
- 证据状态：elementwise add 代表路径已确认；GPU 实际运行未验证。
- 最后更新：2026-09-10
- 前置阅读：[implementation.md](implementation.md)
- 后续阅读：[M08 runtime](../M08-pytorch-extension/runtime.md)
## 结论摘要

本页聚焦 01-modules/M02-elementwise/execution-flows.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。

## 正常主路径：`M02-FLOW-MAIN-001`

| 步骤 | 源码符号与位置 | 输入/前置状态 | 状态变化与副作用 | 执行上下文 | 下一步 |
|---|---|---|---|---|---|
| 1 | `elementwise.py` module import `[kernels/elementwise/elementwise.py:8-24]` | 当前工作目录含 `elementwise.cu`，PyTorch/CUDA 可用 | `load()` 请求编译/加载 `elementwise_lib` | Python 主线程；编译器和 extension loader | 2 |
| 2 | `PYBIND11_MODULE` `[kernels/elementwise/elementwise.cu:191-198]` | 动态库已加载 | 注册六个 Python-callable 函数 | extension 初始化上下文 | 3 |
| 3 | `run_benchmark` `[kernels/elementwise/elementwise.py:27-46]` | `a,b,c` 为 CUDA tensor，`c` 可写 | 可先将 `c` 清零，发射 warmup calls | Python 调用线程；kernel 异步提交 | 4 |
| 4 | 生成 wrapper `[kernels/elementwise/elementwise.cu:140-145]` | 三个 tensor 传入 | 检查 dtype；取得 `ndim` 和 shape-derived `N` | 当前 PyTorch/CUDA 调用上下文 | 5 |
| 5 | dispatch `[kernels/elementwise/elementwise.cu:146-180]` | `ndim`、二维 `K` 和 pack 宽度已知 | 计算 block/grid 并把 tensor 指针借给 launch | 当前 CUDA stream（源码未显式传参） | 6 |
| 6 | `elementwise_add_f32x4_kernel` `[kernels/elementwise/elementwise.cu:32-49]` | 每线程得到 pack 起点 `idx` | 完整 pack 读 `a/b`、算四个和、写 `c`；尾部逐元素写 | GPU block/thread | 7 |
| 7 | `torch.cuda.synchronize()` `[kernels/elementwise/elementwise.py:47-56]` | warmup kernel 已排队 | 等待 GPU 完成，随后以 wall-clock 计时正式迭代 | Python 等待 GPU | 8 |
| 8 | 输出观察 `[kernels/elementwise/elementwise.py:58-66]` | `c` 已完成 | 把少量样本复制到 CPU；可打印全量输出 | Python + 默认 CUDA stream | 结束 |

这里的“当前 CUDA stream”是基于 PyTorch extension 常规调用上下文的推断；`elementwise.cu` 本身没有显示 stream 参数或 `at::cuda::CUDAGuard`，因此需要以运行时实验确认 stream 归属。

## 关键分支路径：`M02-FLOW-DISPATCH-001`

1. 对 `a.dim() != 2`，wrapper 将所有维度相乘为 `N`，以 `256/n_elements` threads per block 发射扁平 kernel。[kernels/elementwise/elementwise.cu:146-157]
2. 对二维 `[S,K]`，若 `K/n_elements <= 1024`，block 的 x 维等于 `K/n_elements`，grid 的 x 维等于 `S`，即一个 block 对应一行。[kernels/elementwise/elementwise.cu:159-168]
3. 否则再次折叠 `N=S*K`，使用扁平 grid。[kernels/elementwise/elementwise.cu:169-180]
4. kernel 内部再根据 `idx + pack_width - 1 < N` 选择 vector path 或 scalar tail path。[kernels/elementwise/elementwise.cu:34-48,62-72,77-103,109-127]

## 错误路径：`M02-FLOW-ERROR-001`

- dtype 不匹配：`CHECK_TORCH_TENSOR_DTYPE` 打印 tensor options 并抛出 `runtime_error`，不会发射 kernel。[kernels/elementwise/elementwise.cu:134-145]
- shape 不匹配：代表 wrapper 没有检查，因此源码层没有在入口拒绝；实际结果可能错误，非连续或越界行为需要 CUDA 实验确认。[kernels/elementwise/elementwise.cu:140-157]
- kernel launch 错误：代表 wrapper 没有显式 `cudaGetLastError`；Python 侧只有在后续同步、张量读取或其它 CUDA API 中才可能观察错误，这是基于源码缺口的推断。[kernels/elementwise/elementwise.cu:154-180]
- 编译/加载错误：`load()` 在模块导入阶段抛出构建或加载异常，后续 benchmark 函数不会执行。[kernels/elementwise/elementwise.py:8-24]

## 清理与生命周期：`M02-FLOW-CLEANUP-001`

M02 没有自定义 device allocation、句柄或显式 shutdown。`a`、`b`、`c` 由 Python/PyTorch 引用计数和 CUDA caching allocator 管理；动态扩展由 PyTorch extension loader 管理。`run_benchmark` 只同步和读取输出，不负责销毁 tensor。[kernels/elementwise/elementwise.py:27-66] 状态：tensor 所有权已确认；extension cache 的进程级销毁细节不在本模块源码中，标记为未知。

## 并发与同步观察

一个 kernel 内没有 block 间共享状态，也没有 `__syncthreads`；每个线程写独立逻辑元素。Python benchmark 在 warmup 和正式迭代之间、正式迭代之后调用 device-wide `torch.cuda.synchronize()`。[kernels/elementwise/elementwise.py:41-56] 这保证计时观察到已完成工作，但也会把同一 device 上其它 stream 的工作纳入等待，具体影响需根据 PyTorch runtime 验证。

## 伪代码

```text
load extension
register six add wrappers
for each selected variant:
    if output exists: zero output
    repeat warmup:
        wrapper(a, b, c)
    synchronize
    start timer
    repeat iters:
        wrapper(a, b, c)
    synchronize
    inspect a small output sample
```

## 相关文档

- [implementation.md](implementation.md)
- [source-map.md](source-map.md)
- [call-chains.md](call-chains.md)
- [testing.md](testing.md)

## 源码证据摘要

- `[kernels/elementwise/elementwise.py:8-24,27-66]`。
- `[kernels/elementwise/elementwise.cu:140-180]`。
- `[kernels/elementwise/elementwise.cu:22-127]`。

## 未解决问题

需要实验确认当前 stream、异步 launch 错误的暴露时间和不合法 shape/stride 的实际故障形式。

## 下一步阅读建议

接着阅读 `call-chains.md`，再用 `testing.md` 的 tail/stride 矩阵设计回归。
