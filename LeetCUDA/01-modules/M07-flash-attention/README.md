# M07 FlashAttention

- 文档目的：说明 FlashAttention MMA/CuTe 变体的输入布局、构建和性能路径。
- 适用范围：`kernels/flash-attn/`。
- 对应源码版本：`0983c65`。
- 证据状态：Python/构建入口和代表 tiling-QKV 的 QK→online softmax→PV→store 路径已完成深度分析；其他变体和真实 GPU 运行仍部分覆盖。
- 最后更新：2026-09-10
- 前置阅读：[M03](../M03-reduction-normalization/README.md)、[M06](../M06-hgemm-tensorcore/README.md)
- 后续阅读：[M09](../M09-interview-benchmark/README.md)

## 结论摘要

M07 把 QKᵀ、缩放、safe/online softmax、PV 融合在 tiled attention 中，使用 MMA Tensor Core、shared-memory 复用、split-Q/KV、multi-stage、swizzle、fine-grained QK/QKV tiling，并提供 CuTe/TMA 变体。Python 脚本动态收集几十个源文件，按设备、head dimension、accumulator 和 CLI flags 过滤测试，再与官方 flash-attn、SDPA 或 unfused torch 比较。

## 关键调用链

```text
flash_attn_mma.py args
  -> get_build_sources/get_build_cuda_cflags
  -> torch.utils.cpp_extension.load
  -> pybind/flash_attn.cc + MMA/CuTe .cu
  -> run_benchmark (warmup/sync/TFLOPS)
  -> check_all_close(reference, output)
```

## 数据布局

脚本同时构造 `[B,H,N,D]` 以及为官方 FA/SMEM swizzle 转置的 `[B,N,H,D]`/`[B,H,D,N]` 视图。[kernels/flash-attn/flash_attn_mma.py:416-443]。调用变体的 layout 不能只凭函数名猜测，需跟对应 pybind/kernel 检查。

## 设计取舍与风险

减少中间 attention 矩阵的显存占用是融合/tiling 的核心收益；代价是复杂同步、动态 shared memory、架构宏和数值误差。`--D > 256` 自动启用 SDPA 比较，支持上限会按 tag 跳过。[kernels/flash-attn/flash_attn_mma.py:214-227,329-337]

## 相关文档

- [implementation.md](implementation.md)：tiling-QKV 的布局、shared 复用、online softmax 和输出
- [execution-flows.md](execution-flows.md)：控制流、数据流、同步、错误和资源
- [data-structures.md](data-structures.md)：Q/K/V/O 与片上状态生命周期
- [line-level-analysis.md](line-level-analysis.md)：代表符号和深度审计表
- [visual-guide.md](visual-guide.md)：Q/K/V 数据流、online softmax 和边界图
- [examples.md](examples.md)：未验证运行配方和修改练习
- [development-guide.md](development-guide.md)：变体修改与验证顺序
- [design.md](design.md)
- [source-map.md](source-map.md)
- [interfaces.md](interfaces.md)
- [call-chains.md](call-chains.md)
- [testing.md](testing.md)
- [performance.md](performance.md)
- [risks-and-debt.md](risks-and-debt.md)

## 源码证据摘要

- `[kernels/flash-attn/README.md:4-14,42-104]`。
- `[kernels/flash-attn/flash_attn_mma.py:80-203,220-227]`。
- `[kernels/flash-attn/flash_attn_mma.py:285-413]`。

## 未解决问题

需要在真实 CUDA 环境确认每个 `D/B/H/N` 组合和架构宏的可编译/可运行矩阵。
