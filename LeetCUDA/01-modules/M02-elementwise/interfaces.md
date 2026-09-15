# M02 接口

- 对应源码版本：`main` / `4513b31`。
- 证据状态：elementwise add 代表接口已确认；其它激活目录需逐文件核验。
- 最后更新：2026-09-10

## Python 构建与导出接口

`elementwise.py` 在导入阶段调用 `torch.utils.cpp_extension.load`，源文件是当前脚本目录下的 `elementwise.cu`，并设置 CUDA 快速数学和 C++17 编译选项。加载成功后，模块暴露六个 add 变体：`elementwise_add_f32`、`elementwise_add_f32x4`、`elementwise_add_f16`、`elementwise_add_f16x2`、`elementwise_add_f16x8` 和 `elementwise_add_f16x8_pack`。[kernels/elementwise/elementwise.py:8-24] [kernels/elementwise/elementwise.cu:184-198]

## C++/CUDA ABI

每个函数接收三个 `torch::Tensor`：输入 `a`、输入 `b` 和调用者提供的可写输出 `c`，返回类型为 `void`。PyBind 名称由 `TORCH_BINDING_ELEM_ADD` 宏和 `m.def` 注册；Python 端必须传入三个位置参数，源码没有 default output 或新建输出语义。[kernels/elementwise/elementwise.cu:130-138,184-198]

## 已实现的入口检查

- `a`、`b`、`c` 的 dtype 分别与变体声明匹配；失败时打印 tensor options 并抛出 `runtime_error`。[kernels/elementwise/elementwise.cu:134-145]
- wrapper 读取 `a.dim()`、`a.size(0)` 和 `a.size(1)` 来选择 launch；没有检查 `b/c` 的 shape 是否与 `a` 相等。[kernels/elementwise/elementwise.cu:140-180]

## 未形成统一保证的前置条件

| 条件 | 源码现状 | 影响 |
|---|---|---|
| CUDA device | 未见显式 `is_cuda`/device 检查 | CPU tensor 或跨设备 tensor 的故障形式未验证 |
| shape 相等 | 未检查 | 可能错误读写；需运行实验确认 |
| contiguous/stride | 未检查 | raw pointer 按连续扁平数组解释，非连续 view 不满足隐含布局 |
| pack 对齐 | 未检查 | `float4`/128-bit alias 依赖地址对齐 |
| stream | wrapper 未显式传 stream | 归属遵循 PyTorch extension 常规上下文是推断 |
| launch error | 未见 `cudaGetLastError` | 错误可能延迟到后续同步或 CUDA API |

## 数据和所有权

`a.data_ptr()`、`b.data_ptr()` 和 `c.data_ptr()` 只在 kernel launch 时借给 device kernel；wrapper 不保存指针、不分配 tensor、不释放 tensor。`c` 的初始化和结果读取由 Python benchmark 负责。[kernels/elementwise/elementwise.cu:146-180] [kernels/elementwise/elementwise.py:27-66]

## 相关文档

- [implementation.md](implementation.md)
- [testing.md](testing.md)
- [risks-and-debt.md](risks-and-debt.md)

## 未解决问题

需要在可用 GPU 上确认 CPU/device/stride/aliasing 错误的实际异常或内存影响，并决定是否将隐含连续布局升级为显式入口契约。
