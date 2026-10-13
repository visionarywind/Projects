# D02 数据与状态轨迹

- 文档目的：解释 80-demos/D02-cugetexporttable/data-and-state-trace.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：`source/cuda` 快照（仅含 `src/`；无 `.git`，提交未知；文件时间戳 2022-02-22）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-10-05
- 前置阅读：[项目入口](../../README.md)、[D02 执行轨迹](./execution-trace.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。

## 查询状态机（以 memcheck 调用为例）

`cuGetExportTable` 是无状态查询：不建对象、不改全局、无资源所有权转移。数据流如下：

```text
① 调用方预置      void *slot = NULL;                 （check_core.c:1031 的目标地址）
② 入口入参        &slot（ppExportTable）、&CU_ETID_Tools*（pExportTableId）
③ 入口立即写      *ppExportTable = 0                   （cuietbl.c:632）
④ 逐槽比较        每个 g_etblMap[i].pUuid vs pExportTableId（:637-644）
⑤ 命中写回        *ppExportTable = pEntry->pTable      （:641）
⑥ 返回状态        CUDA_SUCCESS / CUDA_ERROR_INVALID_VALUE（:642/:647）
⑦ 调用方收尾      memcheck 校验 status != CUDA_SUCCESS 即 goto error（:1032-1036），
                  成功后用表内函数指针（如 :816-817 EnableCallback、:811 域掩码）
```

## 代表性具体值（推断，非运行值）

`*ppExportTable` 在返回后的典型取值：

| 查询 UUID | 返回指针 | 指向表 | 首个真实函数槽 |
|---|---|---|---|
| `&CU_ETID_CudaGraphs` | → `g_etblCudaGraphs` | `src/etbl/cuda_graphs/cuda_graphs.c:389` | `cuapiStreamGetCaptureInfo`（`:418`） |
| `&CU_ETID_ToolsModule` | → `g_etblToolsModule`（extern `src/cui/cuietbl.c:79`） | `src/etbl/tools/` 下表实现 | 未读入本 Demo |
| `&CU_ETID_ToolsCallbackArbiter` | → `g_etblToolsCallbackArbiter`（extern `:51`） | memcheck 消费 | memcheck `check_core.c:816` 调用 `EnableCallback` |

UUID 常量值本身只有 `cuda_etbl/*.h` 持有，快照内不可核；以上仅为“存在且被注册”的静态结论，非运行观测。

## 数据生命周期与所有权

- **所有权不转移**：`g_etblMap`、`g_etbl*` 均为 `static const`（如 `src/etbl/suballoc/suballoc.c:75`、`g_etblMap` at `cuietbl.c:521`），随驱动进程生命周期常驻，不随 `cuGetExportTable` 创建或销毁。
- **调用方保存指针**：memcheck 存入 `gvars->tools.*`（如 `:1031` 写入 `gvars->tools.callbacks`）。
- **不使用即释放**：导出表无释放接口；memcheck 只在清理时停止使用（`memcheckCleanup` 调 `EnableAllCallbacks(0,...)`，`check_core.c:1229-1230`），不释放表指针。
- **空槽契约**：表内 NULL 槽位表示不支持的特性（如 `g_etblCudaGraphs:392-417` 的 26 个 NULL），调用方使用前必须判空——这是表数据生命周期中唯一的动态风险点。

## 与查询并发相关

[src/api/apietbl.c:51-55] 注释明确“表为静态初始化，cuInit 前即可查询”，且实现不触碰进程全局：`cuapiGetExportTable` 只读 `g_etblMap`。因此同一进程任意线程并发调用安全；无锁的代价是**增加表只能在静态初始化期完成**，任何运行期注入都必须走 UUID 映射而非改表。

## 相关文档

- [项目入口](../../README.md)
- [分析状态](../../00-overview/analysis-state.md)
- [源码证据索引](../../00-overview/evidence-index.md)

## 源码证据摘要

本页结论所需的源码路径和行号以 [源码证据索引](../../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。代表性取值均为静态推断，未运行验证。

## 未解决问题

目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议

继续阅读 [失败路径](./failure-paths.md) 了解错误码转换与悬空引用，再阅读 [调试走查](./debug-walkthrough.md)。