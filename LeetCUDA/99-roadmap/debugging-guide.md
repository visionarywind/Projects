# 调试指南

- 文档目的：解释 99-roadmap/debugging-guide.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：`main` / `4513b31`。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-10
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 99-roadmap/debugging-guide.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


- 状态：命令为建议流程，未在本环境执行。

## 分层排查

1. **环境**：确认 Python/PyTorch/CUDA/nvcc/GPU 与目标 SM。
2. **编译**：确认 include、库、C++ ABI、extension cache。
3. **输入**：打印 dtype/device/shape/stride/contiguous，缩小到最小 case。
4. **运行**：加入同步并检查 launch/runtime error。
5. **正确性**：与 torch/torchvision/reference 对拍，记录 max error 和索引顺序。
6. **内存/竞态**：使用 `compute-sanitizer`（未验证）。
7. **性能**：最后再使用 Nsight，固定 warmup/iters/同步点。

## 专项检查

- elementwise：pack 对齐、尾部、二维 K 分支。
- GEMM：非 tile 倍数、K-loop 最后 tile、shared barrier。
- HGEMM/FA：目标架构、动态 shared memory、layout 和 accumulator。
- NMS：同分数 stable sort、mask 初始化、原始 index mapping、N=0。

## 证据

`[kernels/interview/notes-v2.cu:82-97]` 展示显式资源检查；`[kernels/nms/nms.py:87-121]` 展示随机 correctness 范围；`[kernels/hgemm/hgemm.py:210-328]` 展示同步 benchmark。

## 报告格式

记录 commit、GPU、driver、CUDA、PyTorch、命令、输入 shape、预期、实际、是否同步、是否 sanitizer。把源码推断、历史输出、本机实测分栏。

## 相关文档
- [项目入口](../README.md)
- [分析状态](../00-overview/analysis-state.md)
- [源码证据索引](../00-overview/evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](../00-overview/analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
