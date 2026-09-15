# 决策与冲突记录

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
