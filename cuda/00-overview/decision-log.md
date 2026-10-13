# 决策与冲突记录

- 文档目的：解释 00-overview/decision-log.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：`source/cuda` 快照（仅含 `src/`；无 `.git`，提交未知；文件时间戳 2022-02-22）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-10-05
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](analysis-state.md)。
## 结论摘要

本页聚焦 00-overview/decision-log.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


## 目录定位

将 `source/cuda` 视为 NVIDIA CUDA Driver/libcuda 源码快照，而不是 CUDA Toolkit 应用项目；依据是 `src/api`、`src/cui` 和 `libcuda` 相关目标定义。

## 快照边界冲突（2026-10-05 记录，本轮修复）

**冲突**：知识库正文引用了 `inc/cuda.h`、`common/version.h`、`cuda.nvmk`、`cudaSyscalls.nvmk`、`tests/**`、`mods/Makefile`、`import/r*`，但当前 `source/cuda` 实际只包含 `src/` 一个子树，上述路径全部不存在。

**只读验证证据**：

- `source/cuda` 顶层只有 `src/`；`source/src.tar.gz` 解包清单同样只含 `src/`（2906 项）。
- `source/cuda` 无 `.git`；`/source/` 在仓库 `.gitignore` 中被整体忽略。
- `src/` 下 2387 个文件时间戳一致为 `2022-02-22`（1 个为 2026-02-03）。
- 在 `src/` 内检索 `CUDA_VERSION`、`NVCUDA_VER`、`CUDA_API_VERSION` 无版本定义命中（仅命中生成的 `.cubins.c` 十六进制数据）。

**处置**：保留这些引用，但统一降级为“当前检出不可验证”，不得据此声明已确认事实。原因见下条。

## 版本冲突

`CUDA_VERSION=10020` 与 `common/version.h` 的内部字段共同支持“约 CUDA Driver API 10.2”；但这两个文件都不在当前快照内，因此该版本判断在本检出中**不可复验**，只能保留为历史推断。由于无 `.git`，不声称具体提交、分支或发行版。

**已撤销的错误声明**：本知识库此前在 170 个文档的页头写入“对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）”。该声明不成立：`39d4a83` 是本 Projects 仓库的提交（“Update doc”），并非 `source/cuda` 的提交，且 `source/cuda` 无 `.git`。该字段已统一改为“`source/cuda` 快照（仅含 `src/`；无 `.git`，提交未知；文件时间戳 2022-02-22）”。

## 证据策略

源码控制流标为静态确认；跨文件动态分派标为推断；外部树、GPU 和生成物行为标为未知/未验证。历史 import 快照和实验 cubin 不并入主实现证据。

## Demo 入口决策（2026-10-05 记录）

**决策**：D01（`tests/cuda_test/basic_sanity.cu`）入口在快照外，保持受阻；新建 D02（`src/api/apietbl.c` → `src/cui/cuietbl.c` 的 `cuGetExportTable` 链路）作为快照内唯一可复核的公开入口 Demo。

**理由**：
- 快照是库源码而非应用仓库：无 `tests/`、无 `.nvmk`，全树仅两个 `main()`（`src/devtools/debugger/exec/cudbgbin2c.c:38`、`cudbgprocess.c:45`，82/113 行的辅助工具），不存在可构建的 Demo 程序。
- 本页此前建议将 `src/etbl/` 用例作为 Demo 候选，**该建议不成立**：`src/etbl/` 是驱动内测试用例体（123 个 `eti*` 定义、`g_etbl*` 分发表），无 `main()`，调用方 runner 在快照外的 `tests/`。将 `src/etbl/` 文件改记为“驱动内测试体”，其注册链改由 D02 追踪。
- D02 满足 §9.3 大部分门槛（入口、轨迹、正常/失败/清理路径、具体值、跨模块边界、断点、修改练习），但**构建目标（第 1 项）不可确认**，故状态为“部分完成（静态）”而非“完整解剖”。

**D02 附带的静态发现**：
- `src/etbl/cuda_graphs/cuda_graphs.c:425,426,428` 的 `cuapiGraphAddConditionalNode`、`cuapiGraphExecGetConditionalNodeSchedulingHandle`、`cuapiStreamCaptureGetLastNode` 在快照全树 0 命中，属快照不完整或条件编译排除分支的证据，非知识库引用越界。
- `cuGetExportTable` 文档声明未命中返回 `CUDA_ERROR_NOT_FOUND`（`apietbl.c:36,45`），实现返回 `CUDA_ERROR_INVALID_VALUE`（`cuietbl.c:647`），为源码自身文档/实现偏差。

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
