# D02 调试走查

- 文档目的：解释 80-demos/D02-cugetexporttable/debug-walkthrough.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：`source/cuda` 快照（仅含 `src/`；无 `.git`，提交未知；文件时间戳 2022-02-22）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-10-05
- 前置阅读：[项目入口](../../README.md)、[D02 执行轨迹](./execution-trace.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。

## 场景：运行时看不到自定义工具回调 / 表未生效

先在本检出复盘 T3-trace（调查“Export Table 是否真的被解析”，作为一个可运行的诊断设计）：

### 断点（对运行环境的建议，非运行操作）

| 位置 | 断点函数 | 观察变量/表达式 | 预期 |
|---|---|---|---|
| `src/cui/cuietbl.c:637` | `cuiGetExportTable` | `index`、`pExportTableId->bytes[0..15]`、`pEntry->pUuid` | 逐槽对比 UUID |
| `src/cui/cuietbl.c:640` | `cuiGetExportTable` | `pEntry->pUuid` vs `pExportTableId` | 命中时 `memcmp==0` |
| `src/cui/cuietbl.c:641` | `cuiGetExportTable` | `pEntry->pTable` | 应为非 NULL 表指针 |
| `src/cui/cuietbl.c:647` | `cuiGetExportTable` | 返回状态 | `CUDA_ERROR_INVALID_VALUE`（未命中） |
| `src/api/apistream.c:757` | `cuapiStreamGetCaptureInfo` | `hStream`、`captureStatus` | 经表槽 :418 进入 |

### 预期调用栈（命中 `CU_ETID_CudaGraphs`）

```text
NvApps (memcheck等) 
  └ cuGetExportTable           [公开符号，.def:454]
    └ cuapiGetExportTable      [src/api/apietbl.c:49]
      └ cuiGetExportTable      [src/cui/cuietbl.c:625]
        └ 返回 g_etblCudaGraphs [src/etbl/cuda_graphs/cuda_graphs.c:389]
```

若程序在 `CU_TRACE_FUNCTION()`（`apietbl.c:51`）处直接返回错误，先确认入参 UUID 是否为 `cuda_etbl/*.h` 中定义的值（该头文件缺失，需要从完整树提供，本检出无法核）。

### 观察技巧

- 用 `watchpp` 风格观察 `g_etblMap` 的 `pUuid` 字段是否与请求 UUID 前 4 字节相似（memcmp 全 16 字节相等才算命中）。
- 若要在不改驱动的情况下验证表内容，可对齐 `src/etbl/suballoc/suballoc.c:75` 的 `g_etblSuballoc`：它只有 `{sizeof, 2 个函数}`，槽位最少，最易人工核对。

### 常见误判

- 把 `CUDA_ERROR_INVALID_VALUE` 当参数错误：实为 “表未命中” 的实现行为（见 failure-paths 路径 2）。
- 把取表成功当成槽位可用：`g_etblCudaGraphs` 26 个 NULL 槽在表格 :392-417，必须先判空。

## 相关文档

- [项目入口](../../README.md)
- [分析状态](../../00-overview/analysis-state.md)
- [源码证据索引](../../00-overview/evidence-index.md)
- [D02 失败路径](./failure-paths.md)

## 源码证据摘要

本页结论所需的源码路径和行号以 [源码证据索引](../../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。断点与调用栈为对运行环境的建议，未在本轮执行。

## 未解决问题

目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议

继续阅读 [修改练习](./modification-exercises.md)。