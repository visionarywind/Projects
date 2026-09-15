# M02 实现机制

- 文档目的：把 elementwise 的 Python 入口、PyBind 导出、launcher 分支和实际 CUDA kernel 串起来。
- 适用范围：`kernels/elementwise/elementwise.py`、`elementwise.cu`；其它激活目录只在共性处引用。
- 对应源码版本：`main` / `4513b31`。
- 证据状态：已确认（elementwise add 代表实现）；其它激活变体部分推断。
- 最后更新：2026-09-10
- 前置阅读：[M02 README](README.md)
- 后续阅读：[M08 接口边界](../M08-pytorch-extension/interfaces.md)

## 结论摘要

M02 的代表实现不是一个泛化的张量算子框架，而是六个显式导出的 add 变体。Python 在模块导入时调用 `load()`，编译 `elementwise.cu`；PyBind 宏把 C++ 包装函数暴露为 Python 属性；包装函数只检查三个 tensor 的 dtype，然后根据 `a.dim()` 和二维输入的 `K` 选择 block/grid，最终把 `data_ptr()` 重解释为 CUDA 原始指针并发射对应 kernel。[kernels/elementwise/elementwise.py:8-24] [kernels/elementwise/elementwise.cu:130-197]

kernel 层有三类实际路径：标量 FP32/FP16 每线程处理一个元素；`float4`/`half2` 路径每线程处理 4/2 个元素；FP16 x8 pack 使用 128-bit 重解释加载和存储，再用四次 `__hadd2` 完成八个 half 的加法。每个向量化 kernel 对不完整 pack 回退到标量循环。[kernels/elementwise/elementwise.cu:19-127]

## 实现组件表

| 实现组件 | 声明/定义 | 创建者 | 主要调用者 | 核心状态 | 实际副作用 |
|---|---|---|---|---|---|
| `elementwise_add_f32_kernel` | `elementwise.cu:22-27` | C++ wrapper 的 launch | `elementwise_add_f32` | `idx`、`N` | 写 `c[idx]` |
| `elementwise_add_f32x4_kernel` | `elementwise.cu:32-49` | `elementwise_add_f32x4` | Python benchmark | `idx`、四元素寄存器 | 读写四个连续 FP32 |
| `elementwise_add_f16x8_pack_kernel` | `elementwise.cu:106-127` | `elementwise_add_f16x8_pack` | Python benchmark | `pack_a/b/c` | 128-bit global load/store |
| `TORCH_BINDING_ELEM_ADD` 生成的 wrapper | `elementwise.cu:140-182` | PyBind module 初始化时注册 | Python `lib.elementwise_add_*` | dtype、ndim、shape-derived `N` | 发射 kernel |
| `run_benchmark` | `elementwise.py:27-66` | Python 直接调用 | 各变体与 `torch.add` | warmup、计时、输出样本 | 同步并把样本拷回 host |

## 声明到落地链路

```text
Python import
  -> torch.utils.cpp_extension.load("elementwise_lib", ["elementwise.cu"])
  -> PYBIND11_MODULE(TORCH_EXTENSION_NAME, m)
  -> m.def("elementwise_add_f32x4", &elementwise_add_f32x4, ...)
  -> generated wrapper checks dtype and derives N
  -> dim3 block/grid selected from ndim and K
  -> elementwise_add_f32x4_kernel<<<grid, block>>>
  -> each thread loads, adds, stores c
  -> Python synchronizes and samples output
```

`m.def` 只是 ABI/名称边界，不执行加法；真正的状态改变发生在各 `__global__` kernel 对 `c` 的写入。包装函数没有分配或释放 tensor，tensor 生命周期仍由 Python/PyTorch 持有。[kernels/elementwise/elementwise.cu:130-138,184-198]

## Launcher 的两个映射分支

### 非二维输入

当 `a.dim() != 2` 时，wrapper 将各维乘积折叠成扁平 `N`，使用 `block = 256 / n_elements`，`grid = ceil(N / 256)`。这里的 `n_elements` 是每个线程的逻辑 pack 宽度；例如 x4 使用 64 个线程，逻辑覆盖每个 block 256 个元素。[kernels/elementwise/elementwise.cu:146-157]

### 二维输入

当输入为 `[S,K]` 且 `K / n_elements <= 1024` 时，wrapper 使用 `block = (K/n_elements, 1, 1)`、`grid = (S,1,1)`，让一个 block 对应一行。否则退回扁平映射。[kernels/elementwise/elementwise.cu:159-180]

这里的 `K / n_elements` 是整数除法。它用于选择映射，不是完整的向量化合法性证明；尾部由 kernel 处理，但非连续 stride、输入形状不一致和指针对齐并没有在 wrapper 中统一检查。因此“可发射”不等于“任意 PyTorch tensor 都安全”。状态：已确认风险。[kernels/elementwise/elementwise.cu:143-180]

## kernel 不变量与算法

对每个逻辑索引 `i`，数学结果是 `c[i] = a[i] + b[i]`。标量 kernel 直接以 `idx = blockIdx.x * blockDim.x + threadIdx.x` 映射；x4/x2/x8 先乘 pack 宽度得到 pack 起点。完整 pack 由当前线程独占读写，尾部线程执行 `for` 循环覆盖 `[idx,N)`，从而避免最后一次完整向量访问越界。[kernels/elementwise/elementwise.cu:22-49,54-72,75-127]

FP16 x8 pack 的 `half pack_a[8]` 是线程私有的可寻址 local/register 候选存储；`reinterpret_cast<float4*>` 使八个 16-bit 元素以 128 bit 事务读写，随后 `HALF2` 视图每次处理两个 half。该优化依赖连续布局和满足向量访问的地址对齐；源码没有在运行时验证这两个前提。[kernels/elementwise/elementwise.cu:106-122]

## 资源、执行上下文和错误

- 输入 `a`/`b` 与输出 `c` 是借用的 `torch::Tensor`；wrapper 只取得 `data_ptr()`，没有保存裸指针。[kernels/elementwise/elementwise.cu:140-157]
- kernel launch 默认进入 PyTorch 当前 CUDA 执行上下文；源码没有显式 stream 参数，也没有在 wrapper 末尾调用 `cudaGetLastError` 或同步。[kernels/elementwise/elementwise.cu:154-180]
- Python benchmark 在读输出和计算耗时前调用 `torch.cuda.synchronize()`，因此 benchmark 能观察到异步 kernel 的完成；这不等于单次 API 调用已经同步或完成错误检查。[kernels/elementwise/elementwise.py:41-66]
- dtype 错误通过 C++ `runtime_error` 抛出；shape/device/contiguous/stride 不匹配没有同等统一保证。[kernels/elementwise/elementwise.cu:134-145]

## 配置与路径选择

| 条件 | 代码分支 | 影响 |
|---|---|---|
| `a.dim() != 2` | `elementwise.cu:146-157` | 扁平 grid |
| 二维且 `K/n_elements <= 1024` | `elementwise.cu:162-168` | 一行一个 block |
| 二维且 `K/n_elements > 1024` | `elementwise.cu:169-180` | 扁平 grid |
| `n_elements = 1/2/4/8` | `elementwise.cu:184-189` | wrapper block 宽度与 kernel pack 宽度 |
| `TORCH_CUDA_ARCH_LIST` | Python/CUDA extension toolchain | 编译目标；来自 README 的环境建议，未在本机验证 |

## 复杂度

每个输出元素做一次加法，计算复杂度为 `O(numel)`，输入和输出额外存储为 `O(numel)`；pack 版本改变访存指令粒度，不改变算法复杂度。实际速度取决于 global-memory 带宽、对齐、kernel launch 映射和 PyTorch 当前 stream。后一句是性能推断，需 profile 验证。

## 关键限制

1. wrapper 没有检查 `a`、`b`、`c` 的 shape 相等；错误 shape 可能导致读越界或产生错误结果。[kernels/elementwise/elementwise.cu:140-157]
2. wrapper 没有显式检查 CUDA device、contiguous 或 stride；packed 重解释路径尤其不能把任意 strided view 当作连续数组。状态：已确认源码缺少检查，具体越界后果需实验。
3. 没有统一 launch-error 检查；错误可能在后续同步或下一次 CUDA API 才暴露。状态：已确认缺少本地检查，传播时机是推断。
4. 激活目录有自己的 `.cu`/binding，不能只根据本文件把 add 的契约推广到所有激活函数。状态：已确认范围限制。

## 相关文档

- [README.md](README.md)
- [execution-flows.md](execution-flows.md)
- [source-map.md](source-map.md)
- [call-chains.md](call-chains.md)
- [interfaces.md](interfaces.md)
- [testing.md](testing.md)
- [risks-and-debt.md](risks-and-debt.md)

## 源码证据摘要

- `[kernels/elementwise/elementwise.py:8-24]`：动态扩展入口。
- `[kernels/elementwise/elementwise.cu:22-49,54-127]`：标量、向量化和 pack kernel。
- `[kernels/elementwise/elementwise.cu:140-198]`：wrapper、dispatch 和 PyBind 导出。
- `[kernels/elementwise/elementwise.py:27-66]`：warmup、同步和计时。

## 未解决问题

- 需要在真实 CUDA 环境验证非连续 view、非对齐地址、空 tensor、奇数 pack 尾部和错误 shape 的实际行为。
- 激活函数变体需要逐文件确认其 dtype、输入校验和边界实现。

## 下一步阅读建议

先读 `execution-flows.md` 追踪一次调用，再读 `risks-and-debt.md` 设计回归测试。
