# M06 HGEMM 与 Tensor Core

- 文档目的：解释 HGEMM 从 CUDA Core 到 WMMA/MMA/CuTe/WGMMA 的优化结构。
- 适用范围：`kernels/hgemm/`, `swizzle/`, `ws-hgemm/`。
- 对应源码版本：`0983c65`。
- 证据状态：Python/构建入口、代表 staged MMA 的 shared/register/pipeline/store 已完成深度分析；其他 kernel family 和真实 GPU 运行仍部分覆盖。
- 最后更新：2026-09-10
- 前置阅读：[M05](../M05-gemv-sgemm/README.md)
- 后续阅读：[M07](../M07-flash-attention/README.md)

## 结论摘要

M06 是仓库性能主线：Python `hgemm.py` 选择 kernel 家族并统计 TFLOPS；`setup.py` 将多目录 `.cu` 编成 `toy-hgemm`；makefile 直接编译 C++ benchmark。实现从 naive/sliced-K/thread tile 进入 WMMA `m16n16k16`、MMA `m16n8k16`，再通过 multi-stage、register double buffer、block/warp/SMEM swizzle 和 CuTe/WGMMA 处理访存与 Tensor Core 利用率。

## 设计结构

```text
hgemm.py -> tools/utils.py -> CUDAExtension sources
         -> pybind/hgemm.cc -> kernel family
         -> warmup/sync -> 2*M*N*K/time -> plot
setup.py -> same source graph -> wheel
makefile -> selected .cu -> standalone benchmark
```

## 关键契约

A/B/C 的 NN/TN layout、FP16 input、F16/F32 accumulator、stages、swizzle 和目标 SM 必须匹配。CuTe 路径依赖 CUTLASS headers；SM90 WGMMA/TMA 路径依赖架构和 CUDA 支持。`hgemm.py` 的 cublas handle 在 benchmark 前初始化、后销毁。[kernels/hgemm/hgemm.py:242-244,319-322]

## 性能解释

block swizzle 面向 L2/cache locality，SMEM swizzle/padding 面向 bank conflict；double buffer 让下一 tile load 与当前 tile compute 重叠。性能数字只能在同 GPU、同驱动/toolkit、同矩阵和同迭代下比较。

## 风险与测试

优先测试 correctness against torch.matmul/cuBLAS，再测试 M/N/K 非 tile 倍数、TN column-major 转换、stages=1/2/3、sm_80/89/90a 编译和动态 shared memory。不要以“超过 cuBLAS”作为稳定行为断言。

## 相关文档

- [implementation.md](implementation.md)：Python→PyBind→launcher→staged MMA 的实际落地、dsmem、register double buffer 和写回
- [execution-flows.md](execution-flows.md)：控制流、数据流、同步、资源和错误边界
- [data-structures.md](data-structures.md)：tile、stage、fragment、accumulator 与生命周期
- [line-level-analysis.md](line-level-analysis.md)：代表符号和深度审计表
- [visual-guide.md](visual-guide.md)：pipeline、block/warp/tile 和片上生命周期图
- [examples.md](examples.md)：未验证运行配方和修改练习
- [performance.md](performance.md)：性能关键路径和验证顺序
- [design.md](design.md)
- [source-map.md](source-map.md)
- [interfaces.md](interfaces.md)
- [call-chains.md](call-chains.md)
- [testing.md](testing.md)
- [development-guide.md](development-guide.md)
- [risks-and-debt.md](risks-and-debt.md)

## 源码证据摘要

- `[kernels/hgemm/hgemm.py:18-177]`：选项。
- `[kernels/hgemm/hgemm.py:210-328]`：benchmark。
- `[kernels/hgemm/setup.py:44-67]`：extension source/include/flags。
- `[kernels/hgemm/makefile:10-18,80-90]`：standalone targets。
- `[kernels/hgemm/README.md:195-204]`：padding/swizzle 取舍。

## 未解决问题

深层 `mma/`, `wgmma/`, `cutlass/` 每个 kernel 的完整 layout 图和 ABI 仍需按目标 GPU 增量解析。
