# M06 调用链

- 文档目的：解释 01-modules/M06-hgemm-tensorcore/call-chains.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/LeetCUDA HEAD 4513b31（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M06-hgemm-tensorcore/call-chains.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


## Python 动态扩展/benchmark

```text
hgemm.py args
 -> tools/utils.py:get_build_sources / build helper
 -> pybind/hgemm.cc exported variant
 -> launcher / template instantiation
 -> cp.async staged shared memory
 -> ldmatrix register fragments
 -> mma.sync.aligned.m16n8k16 / accumulator
 -> collective shuffle + vectorized store
 -> synchronize, correctness, TFLOPS
```

代表 staged MMA 的实际实现证据为 `[kernels/hgemm/mma/basic/hgemm_mma_stage.cu:120-142,691-839,1850-1885]`。动态 shared-memory launcher 见 `[kernels/hgemm/mma/basic/hgemm_mma_stage.cu:1900-1959]`。

## 资源和错误边界

cublas benchmark 显式 init/destroy handle；Python benchmark 负责 warmup/synchronize；PyTorch tensor 由调用者/allocator 持有。自定义 kernel 的统一 launch-error 检查未从所有导出函数确认，因此标为部分完成。[kernels/hgemm/hgemm.py:210-328]

## 变体边界

WMMA、MMA、TN/swizzle、CuTe、WGMMA 和 cuBLAS 通过不同导出/源文件落地，不能从公共 wrapper 推断相同 layout。完整 family call graph 仍为部分完成。

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
