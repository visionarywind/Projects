# M08 PyTorch 扩展边界

- 文档目的：统一理解 Python、C++ binding、CUDA kernel 和 tensor 契约。
- 适用范围：各模块 `.py`、`pybind/*.cc`、`setup.py`。
- 对应源码版本：`4513b31`。
- 证据状态：代表模式已确认；具体模块校验不一致。
- 最后更新：2026-09-15
- 前置阅读：[architecture](../../00-overview/architecture.md)
- 后续阅读：[M10 NMS](../M10-nms/README.md)

## 结论摘要

M08 是横切边界而非单一目录：即时路径用 `torch.utils.cpp_extension.load(name, sources, extra_cuda_cflags, extra_cflags)`，打包路径用 `CUDAExtension` + `BuildExtension`，C++ 侧用 PyBind 导出函数。Python 负责生成 CUDA tensor 和 benchmark；binding 负责 dtype/device/shape（若实现了）；kernel 负责并行计算。

## 两条构建路径

```text
script.py -> load() -> compile .cu/.cc -> Python module
setup.py -> CUDAExtension -> wheel -> import package
```

Elementwise 只显示 dtype 检查和宏绑定；NMS 还检查 device、dim、长度，说明不能假设全仓库统一输入验证。[kernels/elementwise/elementwise.cu:134-146]、[kernels/nms/nms.cu:126-150]。

## API 修改规则

新增函数必须同时更新 `.cu/.cc` 导出、Python 调用/参数、README 示例和 correctness test；改变 dtype/layout 必须更新 reference 和误差阈值。保持 `torch::Tensor` 生命周期借用，不在 binding 中保存裸指针到异步调用之后。

## 测试与 ABI

先用小 tensor 对拍，再做 benchmark；确认 Python、PyTorch、CUDA toolkit、C++ ABI 和 extension cache 一致。未提供统一 setup lockfile，构建失败通常应先检查环境而非 kernel 算法。

## 相关文档

- [runtime.md](runtime.md)
- [interfaces.md](interfaces.md)
- [testing.md](testing.md)
- [development-guide.md](development-guide.md)
- [risks-and-debt.md](risks-and-debt.md)

## 源码证据摘要

- `[kernels/elementwise/elementwise.py:9-24]`。
- `[kernels/hgemm/setup.py:44-95]`。
- `[kernels/nms/nms.cu:111-132]`。

## 未解决问题

全仓库缺少统一的 device/stream/error/contiguous 检查规范。

资源边界：binding 借用调用方 tensor 的 `data_ptr()`，不拥有 tensor 或其 allocator；异步 kernel 返回后调用方仍须保持 tensor 存活。项目没有自有 CPU arena、GPU memory pool 或统一 CUDA Graph runtime，PyTorch caching allocator、extension cache 和单个 benchmark 的 `cudaMalloc` 必须分别记录。

## 深度审计

| 分析对象 | 入口落地 | 正常路径 | 分支 | 异常 | 清理 | 数据生命周期 | 执行上下文 | 行级证据 | Demo 映射 | 状态/缺口 |
|---|---|---|---|---|---|---|---|---|---|---|
| `01-modules/M08-pytorch-extension/README.md` | 已完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 已映射或不适用 | 部分完成：动态行为、边界或专用变体仍需验证 |

## 下一步阅读建议
先阅读 [分析状态](../../00-overview/analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
