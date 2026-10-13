# D02 cuGetExportTable 导出表机制

- 文档目的：解释 80-demos/D02-cugetexporttable/README.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：`source/cuda` 快照（仅含 `src/`；无 `.git`，提交未知；文件时间戳 2022-02-22）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-10-05
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)、[Demo 注册表](../demo-registry.md)。
- 配套文档：[执行轨迹](./execution-trace.md)、[数据与状态轨迹](./data-and-state-trace.md)、[失败路径](./failure-paths.md)、[调试走查](./debug-walkthrough.md)、[修改练习](./modification-exercises.md)。

## 结论摘要

`cuGetExportTable` 是 CUDA 驱动**唯一的导出表解析入口**：外部模块（如 memcheck、OpenCL ICD）凭 UUID 向驱动索要版本化的函数指针表。本 Demo 将该入口从导出符号一路追踪到 `g_etblMap` 线性查找和具体的表实现，全程落在快照内，是当前 `source/cuda` 检出中唯一可复核的公开入口链。构建目标不可确认（快照无 `.nvmk`/Makefile），故不满足 `project-prompt` §9.3 第 1 项，状态记为**部分完成（静态）**而非“完整解剖”。

## 入口链条

```
公开符号 [src/cuda_master.def:454]
   │  cuGetExportTable  ← 名称→cuapi* 重命名在缺失的 generated_cuapi.h 中，见 src/cuda_internal.h:41
   ▼
实现 [src/api/apietbl.c:48-58]  cuapiGetExportTable
   │  无 cuiInitCheck（[src/api/apietbl.c:53-55]：表为静态初始化，cuInit 前即可查）
   ▼
声明 [src/cuda_internal.h:1515]  cuiGetExportTable
   ▼
查找 [src/cui/cuietbl.c:625-648]  cuiGetExportTable
   │  线性遍历 g_etblMap[:521-603]，memcmp 比对 UUID（cuiCompareUuid[:611-614]）
   ▼
命中 → 返回 g_etbl* 表指针（如 g_etblCudaGraphs[:429]）
```

## 真实消费者（快照内）

- [src/devtools/memcheck/check_core.c:1031-1102]：memcheck 连发 13 次 `cuGetExportTable` 取 callback/context/device/module/memory/memcpy/… 各工具表；清理在 [memcheckCleanup:1221-1259]（`EnableAllCallbacks(0)`，不释放表指针）。
- [src/etbl/tools_cu_etbl.c:10]：OpenCL 侧把 `cuiGetExportTable` 包成 CL 可用的 `cuGetExportTable`，并用 `NvCliMapCUErrorToCL` 转换错误码。
- [src/etbl/cbl/cbl.c:1203]：CBL 用 `cuiGetExportTable(ppEtblToolsModule, &CU_ETID_ToolsModule)` 取 ToolsModule 表。

## 表如何被注册（对照 §9.2 第 5 项“实现如何从注册表被选中”）

1. 每个表的**实现**是一个 `static const CUetbl* g_etbl* = { sizeof, 槽位… }`（例：[src/etbl/suballoc/suballoc.c:75-79]）。
2. `src/cui/cuietbl.c:42-504` 用 `#include "cuda_etbl/*.h"`（72 个、均在快照外）引入 `CU_ETID_*` UUID 与 `ETBL_ENTRY_*` 宏定义，然后以 `ETBL_ENTRY_名称 { &CU_ETID_名称, &g_etbl名称 }` 形式（[src/cui/cuietbl.c:35-36]）注册。
3. 初始值列表 [g_etblMap:521-603] 共 75 个 `ETBL_ENTRY_`（`:523-597`），末尾哨兵 `{0,0}`（`:602`）；表长 `g_etblCount = sizeof/sizeof - 1`（`:605-608`）。
4. 查找按 UUID `memcmp` 线性扫描（`:634-645`），`pEntry->pUuid` 空判为防御（`:639`）。
5. 符号名与 UUID 的绑定仅存在于快照外的 `cuda_etbl/*.h`，故**快照内可验证“表被注册”**、**不可验证“UUID 值与头文件声明一致”**。

## ABI 版本化机制（本 Demo 的可落地点）

[src/cui/cuietbl.c:616-624] 说明：表可版本化、可取消支持；**表不再受支持时查询报错**；**不换 UUID 则不改表 ABI**。实现样例在 `g_etblCudaGraphs`：

- 表体 [src/etbl/cuda_graphs/cuda_graphs.c:389-430]：首槽 `:391` 为该表大小 `sizeof`，随后 `:392-417` 连续 **26 个 NULL 槽位**（占位/不支持），最后 `:418-429` 12 个真实函数指针。
- 表内容定位：4 个 `eti*` 测试用例（`:420-424`，本表自带驱动内测试体）＋ 2 个跨文件实现（`cuapiStreamGetCaptureInfo`、`_ptsz`，[src/api/apistream.c:757,763]）＋ 3 个本文件实现（`:330,384`）＋ **3 个悬空符号**（见失败路径）。

## 关键分支

- `:629-631`：`ppExportTable` 或 `pExportTableId` 为空 → `CUDA_ERROR_INVALID_VALUE`，且先写 `*ppExportTable = 0`（`:632`）。
- `:639-644`：`pEntry->pUuid` 命中（`cuiCompareUuid == 0`）→ 回填指针并返回 `CUDA_SUCCESS`。
- `:647`：扫描完仍无命中 → `CUDA_ERROR_INVALID_VALUE`（**与文档不一致**，见失败路径）。

## 状态

走查的每条符号、文件和行号均在本检出复核通过；**未构建、未运行**。构建目标不可确认：快照无 `.nvmk`/Makefile，`g_etblCudaGraphs` 中 3 个悬空引用也提示本表不能直接作为完整构建单元，仅能做源码级解剖。

## 文档元数据（规范补充）

- 文档目的：说明 `80-demos/D02-cugetexporttable/README.md` 的源码分析范围、结论和维护入口。
- 适用范围：当前项目对应模块/入口的静态源码与测试分析。
- 对应源码版本：以本项目 `00-overview/analysis-state.md` 或同页版本字段为准。
- 证据状态：静态源码证据；未执行的构建、测试、GPU、网络或多进程行为保持“未验证”。
- 最后更新：2026-10-05
- 前置阅读：本项目根 README 与 `00-overview/analysis-state.md`。
- 后续阅读：本模块/示例的实现、测试和风险页面。

## 深度审计

状态：`未开始` / `部分完成` / `已完成` / `受阻`。

| 分析对象 | 入口落地 | 正常路径 | 分支 | 异常 | 清理 | 数据生命周期 | 执行上下文 | 行级证据 | Demo 映射 | 状态/缺口 |
|---|---|---|---|---|---|---|---|---|---|---|
| `80-demos/D02-cugetexporttable` | 已完成 | 已完成 | 已完成 | 已完成 | 部分完成 | 部分完成 | 已完成 | 已完成 | 已映射或不适用 | 部分完成：表为静态，无自有清理；构建目标（§9.3 第 1 项）与 UUID 头文件均不可确认 |

- 入口落地：`cuGetExportTable` 公开符号 → `cuapiGetExportTable` → `cuiGetExportTable` 全链路有符号与行号。
- 清理：导出表本身 `static const`，无运行时分配、无清理（见数据生命周期）；消费者侧清理以 memcheck `memcheckCleanup:1221-1259` 为样例。
- 执行上下文：`cuapiGetExportTable` 无锁无初始化依赖（[apietbl.c:51-55]），任意线程任意时刻可调用，返回指针对应静态表、读安全。

## 相关文档

- [项目入口](../../README.md)
- [分析状态](../../00-overview/analysis-state.md)
- [源码证据索引](../../00-overview/evidence-index.md)
- [Demo 注册表](../demo-registry.md)

## 源码证据摘要

本页结论所需的源码路径和行号以 [源码证据索引](../../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题

目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议

先阅读[执行轨迹](./execution-trace.md)理解跨模块调用顺序，再沿[失败路径](./failure-paths.md)了解错误码与 UI 疑虑，最后对照[修改练习](./modification-exercises.md)确认如何新增一个导出表。