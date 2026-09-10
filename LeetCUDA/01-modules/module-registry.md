# 模块注册表

- 文档目的：登记稳定职责边界、入口、依赖、数据和测试边界。
- 对应源码版本：`main` / `0983c65`。
- 证据状态：目录和主要入口已确认；小模块的反向调用关系部分推断。
- 最后更新：2026-09-10
- 前置阅读：[总览架构](../00-overview/architecture.md)
- 后续阅读：各模块 README

## 注册表

| ID | 模块名称 | 源码目录 | 入口 | 对外接口/输出 | 依赖 | 测试边界 | 风险 |
|---|---|---|---|---|---|---|---|
| M01 | 基础公共层 | `kernels/interview/common.cuh`, `base.cuh` | interview include | device helpers、基础 kernel | CUDA/CUTLASS（条件） | interview test | 高 |
| M02 | 逐元素算子 | `elementwise/`, `relu/`, `gelu/`, `elu/`, `swish/`, `hardswish/`, `hardshrink/` | `<op>.py` | Python extension functions | PyTorch/CUDA | 各目录脚本 | 中 |
| M03 | 归约与归一化 | `reduce/`, `softmax/`, `layer-norm/`, `rms-norm/` | Python scripts / base.cuh | reduction/softmax/norm outputs | CUDA/PyTorch | 各目录脚本/interview | 高 |
| M04 | 索引与位置算子 | `embedding/`, `histogram/`, `mat-transpose/`, `rope/` | Python scripts | tensor transforms | PyTorch/CUDA | 各目录脚本 | 中 |
| M05 | GEMV/SGEMM | `sgemv/`, `hgemv/`, `sgemm/` | Python scripts | GEMV/GEMM functions | CUDA/PyTorch/cuBLAS | benchmark scripts | 高 |
| M06 | HGEMM/Tensor Core | `hgemm/`, `swizzle/`, `ws-hgemm/` | `hgemm.py`, `setup.py`, make | wheel/functions/binaries | CUTLASS/cuBLAS/CUDA | Python/C++ benchmark | 高 |
| M07 | FlashAttention | `flash-attn/` | `flash_attn_mma.py`, setup | attention extension | flash-attn/CUTLASS/CUDA | attention compare | 高 |
| M08 | PyTorch 扩展边界 | 各 `.py`、`pybind/`、`setup.py` | `load()`/setuptools | Python-callable ops | PyTorch C++ ABI | module-specific | 高 |
| M09 | Interview 整合 | `kernels/interview/` | `notes-v2.cu`, `build.sh` | architecture-specific binaries | nvcc/cuBLAS/cuDNN/CUTLASS | built-in tests/bench | 高 |
| M10 | NMS | `kernels/nms/` | `nms.py`, `nms()` | int64 kept indices | torchvision reference/CUDA | fixed/random sweep | 高 |
| M11 | Triton/CUTLASS/Profiling | `openai-triton/`, `cutlass/`, `nvidia-nsight/` | per-example scripts | comparison/reference material | Triton/CUTLASS/Nsight | local examples | 中 |

## 边界审查

- 循环依赖：源码目录层面未发现强制循环；编译/运行时都可能共同依赖 PyTorch/CUDA。
- 跳层调用：M09 直接 include 多个实现层，属于其整合目标；普通模块之间没有统一调用图。
- 隐式全局：Python benchmark 的 module-level 参数、计时状态和 extension cache；interview 的静态 benchmark globals。
- 职责重叠：HGEMM、SGEMM、interview 都含矩阵乘，但 M09 是整合/教学入口，不是独立算法实现。

## 相关文档

- [../00-overview/architecture.md](../00-overview/architecture.md)
- [../90-cross-module/change-impact-map.md](../90-cross-module/change-impact-map.md)

## 源码证据摘要

- `[kernels/interview/README.md:6-16]`。
- `[README.md:265-279]`。
- `[kernels/hgemm/setup.py:44-67]`。

## 未解决问题

没有统一模块 API 注册表或构建 manifest；本表是知识库稳定 ID，而不是仓库运行时元数据。

## 下一步阅读建议

从目标 ID 的 README 进入；需要修改公共绑定时先读 M08。
