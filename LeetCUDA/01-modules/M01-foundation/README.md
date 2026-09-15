# M01 基础公共层

- 文档目的：解释 interview 版本的公共 CUDA 抽象和学习基础。
- 适用范围：`kernels/interview/common.cuh`, `base.cuh`。
- 对应源码版本：`4513b31`。
- 证据状态：入口/依赖已确认；全部模板实现未逐符号覆盖。
- 最后更新：2026-09-10
- 前置阅读：[模块注册表](../module-registry.md)
- 后续阅读：[M05](../M05-gemv-sgemm/README.md)、[M09](../M09-interview-benchmark/README.md)

## 结论摘要

M01 提供 interview 单文件体系共享的类型别名、向量化宏、warp 常量、MMA/WGMMA PTX、swizzle、TMA/mbarrier/TensorMap 辅助和基础 kernel。它不是普通 `kernels/*` 的公共库；其主要消费者是 `base.cuh` 及后续 GEMV/GEMM/attention 头文件。

## 设计与边界

`common.cuh` 负责平台/指令辅助，`base.cuh` 负责可讲解的基础原语；上层头文件通过 include 复用。公共宏改变 ABI/布局时会影响所有 interview kernel。线程协作、shared memory 和 PTX 辅助的错误可能跨多个阶段传播。

## 源码地图

| 文件 | 作用 | 入口/消费者 |
|---|---|---|
| `common.cuh` | CUDA headers、INT4/FLOAT4/HALF2、warp、MMA/WGMMA/TMA helpers | 所有 interview `.cuh` |
| `base.cuh` | reduce/dot/elementwise/softmax/norm/RoPE/transpose | `notes-v2.cu`、M05-M07 |
| `notes-v2.cu` | 编译单元和测试入口 | M09 |

## 调试与测试

优先从 `notes-v2.cu` 的小尺寸 correctness test 开始，确认宏展开、架构 define 和动态 shared memory；再运行 benchmark。没有独立 M01 Python 测试。

## 相关文档

- [design.md](design.md)
- [source-map.md](source-map.md)
- [testing.md](testing.md)
- [../M09-interview-benchmark/README.md](../M09-interview-benchmark/README.md)

## 源码证据摘要

- `[kernels/interview/README.md:8-16]`。
- `[kernels/interview/notes-v2.cu:22-28]`。

## 未解决问题

不同 SM 条件下每个 helper 的可用性需要编译矩阵确认。
