# Demo 注册表

- 文档目的：解释 80-demos/demo-registry.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：`source/cuda` 快照（仅含 `src/`；无 `.git`，提交未知；文件时间戳 2022-02-22）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。D01 受阻；D02 为快照内静态解剖。
- 最后更新：2026-10-05
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 80-demos/demo-registry.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


| ID | 入口 | 覆盖 | 状态 |
|---|---|---|---|
| D01 | `tests/cuda_test/basic_sanity.cu` | init、device、context、memory、module/function、kernel、CNP | **受阻**：入口不在当前快照内，行号不可复核；未运行 |
| D02 | `src/api/apietbl.c` → `src/cui/cuietbl.c` | 导出表机制：M01 导出、M02 API 层、M03 注册表、M08/M09 消费者、M10 测试体 | **完成（静态）**：入口与全链路均在快照内；未构建、未运行 |

D01 是源码级追踪样例，不是已通过的 GPU 测试报告。

**阻塞说明（2026-10-05）**：当前 `source/cuda` 只含 `src/`，D01 的入口 `tests/cuda_test/basic_sanity.cu` 及 `tests/cuda_test/*.nvmk` 均不存在，因此无法在本检出中确认其构建目标、入口函数与行号。D02 是为此补选的快照内入口，见 [D02](./D02-cugetexporttable/README.md)。

> **2026-10-05 更正**：本页此前建议以 `src/etbl/cuda_graphs/cuda_graphs.c`、`src/etbl/suballoc/suballoc.c` 作为 Demo 候选，该建议不成立。`src/etbl/` 是**驱动内测试用例体**（`eti*` 函数 + `g_etbl*` 分发表），不含 `main()`，其调用方 runner 在缺失的 `tests/` 下，因此不构成 `project-prompt` 第 9.3 节要求的“真实构建目标和入口”。快照全树仅 `src/devtools/debugger/exec/` 下两个 `main()`，且均为行数不足 120 的工具（二进制转 C 数组、调试子进程），不是 CUDA 使用流程示例。D02 因此改选 `cuGetExportTable` 这一真实公开入口。

### 快照内是否存在可运行 Demo

否。`source/cuda` 快照是**库源码树**而非应用仓库：无 `tests/`、无 `.nvmk` 构建描述、全树仅两个 `main()`（`src/devtools/debugger/exec/cudbgbin2c.c:38`、`cudbgprocess.c:45`，82/113 行的辅助工具）。因此本知识库不存在“可构建、可运行”的 Demo；`project-prompt` 第 9.3 节的“确认真实构建目标和入口”一节，只能对**调用链**成立，对**构建目标**不成立。D01/D02 的状态栏均已据此限定。

## 覆盖缺口

当前没有专门覆盖 `cuStreamBeginCapture` → `cuGraphInstantiate` → `cuGraphLaunch` → `cuGraphExecDestroy` 的 Graph Demo。M06 的 [Graph 资源生命周期](../01-modules/M06-module-launch/graph-resource-lifecycle.md) 提供静态轨迹和待补测试矩阵；在 nvmake/GPU 环境可用前，不把该路径标记为端到端已验证。

D02 不填补本缺口：它覆盖导出表机制，不覆盖 Graph 生命周期。快照内的 Graph 测试体位于 `src/etbl/cuda_graphs/cuda_graphs.c`（8 个 `eti*` 用例，经 `ETBL_ENTRY_CudaGraphs` 注册到 `g_etblMap`），但其调用方与 `cuda_etbl/cuda_graphs.h` 均在快照外，无法独立判定为已覆盖。

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
