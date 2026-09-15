# 决策与冲突记录

- 文档目的：解释 00-overview/decision-log.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](analysis-state.md)。
## 结论摘要

本页聚焦 00-overview/decision-log.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


## 目录定位

将 `source/cuda` 视为 NVIDIA CUDA Driver/libcuda 源码快照，而不是 CUDA Toolkit 应用项目；依据是 `src/api`、`src/cui`、`cuda.nvmk` 和 `libcuda` 目标。

## 版本冲突

`CUDA_VERSION=10020` 与 `common/version.h` 的内部字段共同支持“约 CUDA Driver API 10.2”；由于无 `.git`，不声称具体提交、分支或发行版。

## 证据策略

源码控制流标为静态确认；跨文件动态分派标为推断；外部树、GPU 和生成物行为标为未知/未验证。历史 import 快照和实验 cubin 不并入主实现证据。

## 模块策略

按 API、runtime、device、memory、submit、launch、syscall、tools、OpenCL、tests 的运行时职责拆分，允许一个目录跨模块出现但不重复声称所有权。

## 资源管理审查决策

- “GPU 显存池化”统一指 `CUmemblock` 内由 `CUsuballocatorRadixTree` 管理的 descriptor-compatible free regions；`CUheap` 仅是 VA 管理，QMD/constant-bank/stream pool 是执行资源，NVN pool 是互操作 bookkeeping。
- `CUmembins`/`membin` 只按遗留声明和测试计划记录，不作为当前实现事实；任何重新启用或删除都需先获得源码/运行证据。
- Graph instantiate/launch/destroy 的锁数组和临时 stream 恢复问题记录为静态疑点，不在本轮修改源码；须通过失败注入或 ASan 复现后再决定修复策略。

## 相关文档
- [项目入口](../README.md)
- [分析状态](analysis-state.md)
- [源码证据索引](evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
