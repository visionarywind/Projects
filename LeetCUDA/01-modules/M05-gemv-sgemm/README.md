# M05 GEMV 与 SGEMM

- 文档目的：说明从朴素矩阵/向量乘到 tiling、vectorization、double buffer 的实现梯度。
- 适用范围：`sgemv/`, `hgemv/`, `sgemm/`。
- 对应源码版本：`4513b31`。
- 证据状态：SGEMM 代表 kernel 已逐逻辑块确认；GEMV 归纳。
- 最后更新：2026-09-10
- 前置阅读：[M03](../M03-reduction-normalization/README.md)
- 后续阅读：[M06](../M06-hgemm-tensorcore/README.md)

## 结论摘要

M05 展示 CUDA Core GEMM 的优化阶梯：naive 每线程一个 C 元素；sliced-K 把 A/B tile 放入 shared memory；thread tile 增加每线程计算密度；`float4` 减少 load/store 指令；bank-conflict-aware layout、double buffer 和 WMMA/TF32 版本进一步重叠访存与计算。SGEMV 采用 warp-per-row 等映射处理矩阵向量乘。

## 代表调用链

```text
sgemm.py -> torch load(sgemm*.cu) -> sgemm_* binding
  -> grid(M/BM,N/BN), block tile
  -> global -> shared K tile -> register accumulator
  -> sync -> store C -> synchronize/TFLOPS
```

## 关键行级证据

- naive：`sgemm.cu:20-33`。
- shared sliced-K：`sgemm.cu:36-86`。
- 128x128 block + 8x8 thread tile + vec4：`sgemm.cu:88-166`。
- BCF/double buffer：`sgemm.cu:168-345,347-482`。
- Python benchmark：`sgemm.py:9-29,68-123`。

## 数据/资源

A `[M,K]`、B `[K,N]`、C `[M,N]` 是 row-major 代表契约；shared tile 由 block 共同填充，`__syncthreads` 形成 load/compute 边界，register `r_c` 由线程私有累加。边界尺寸若不是 tile 整数需要额外保护，当前代表代码需按具体 launcher 核验。

## 风险与测试

优先测试 M/N/K 小于 tile、非倍数、K 非 BK 倍数、矩阵结果误差、shared bank conflicts、双缓冲最后 tile parity 和 cublas 基线。用 compute-sanitizer 和 Nsight Compute 复核越界/同步/occupancy。

## 相关文档

- [design.md](design.md)
- [source-map.md](source-map.md)
- [call-chains.md](call-chains.md)
- [testing.md](testing.md)
- [performance.md](performance.md)

## 源码证据摘要

见上列 SGEMM 行号；构建/测试命令见 `sgemm.py` 和 `README.md`。

## 未解决问题

各 SGEMM 变体的边界保护和 WMMA TF32 精度契约仍需逐实现确认。

## 深度审计

| 分析对象 | 入口落地 | 正常路径 | 分支 | 异常 | 清理 | 数据生命周期 | 执行上下文 | 行级证据 | Demo 映射 | 状态/缺口 |
|---|---|---|---|---|---|---|---|---|---|---|
| `01-modules/M05-gemv-sgemm/README.md` | 已完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 已映射或不适用 | 部分完成：动态行为、边界或专用变体仍需验证 |

## 下一步阅读建议
先阅读 [分析状态](../../00-overview/analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
