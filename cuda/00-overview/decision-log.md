# 决策与冲突记录

## 目录定位

将 `/home/mtuser/workspace/cuda` 视为 NVIDIA CUDA Driver/libcuda 源码快照，而不是 CUDA Toolkit 应用项目；依据是 `src/api`、`src/cui`、`cuda.nvmk` 和 `libcuda` 目标。

## 版本冲突

`CUDA_VERSION=10020` 与 `common/version.h` 的内部字段共同支持“约 CUDA Driver API 10.2”；由于无 `.git`，不声称具体提交、分支或发行版。

## 证据策略

源码控制流标为静态确认；跨文件动态分派标为推断；外部树、GPU 和生成物行为标为未知/未验证。历史 import 快照和实验 cubin 不并入主实现证据。

## 模块策略

按 API、runtime、device、memory、submit、launch、syscall、tools、OpenCL、tests 的运行时职责拆分，允许一个目录跨模块出现但不重复声称所有权。
