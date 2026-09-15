# M06 性能路径

- 文档目的：解释 01-modules/M06-hgemm-tensorcore/performance.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/LeetCUDA HEAD 4513b31（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M06-hgemm-tensorcore/performance.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


## 关键路径

```text
global A/B
  → cp.async overlap
  → padded/swizzled shared stage
  → ldmatrix
  → mma.sync.m16n8k16
  → register accumulator
  → warp shuffle + vector store
```

证据：[kernels/hgemm/mma/basic/hgemm_mma_stage.cu:691-839,1850-1885]。

## 可观测指标

- kernel elapsed time / TFLOPS：脚本用 `2*M*N*K/time`，见 `[kernels/hgemm/hgemm.py:210-328]`；
- shared bank conflicts、global memory throughput、tensor utilization、occupancy、register pressure：需要 Nsight Compute，当前未执行；
- stage/padding/swizzle 对性能的因果结论：只能在同一 GPU、同一尺寸和相同 warmup/iterations 下比较。

## 容量约束

动态 shared-memory launcher 请求最多 98304 bytes，并组合 stage/padding/grid。[kernels/hgemm/mma/basic/hgemm_mma_stage.cu:1900-1959]。更多 stage 可能提高 overlap，也可能降低 occupancy；这是待 profile 假设，不是保证。

## 验证顺序

1. torch/cuBLAS correctness；
2. 非整 tile 与不同 layout；
3. compute-sanitizer；
4. Nsight Systems/Compute；
5. 最后比较 TFLOPS。

当前均未验证。

## 相关文档
- [项目入口](../../README.md)
- [分析状态](../../00-overview/analysis-state.md)
- [源码证据索引](../../00-overview/evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](../../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](../../00-overview/analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
