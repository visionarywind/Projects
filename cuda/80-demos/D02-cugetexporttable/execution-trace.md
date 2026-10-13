# D02 执行轨迹

- 文档目的：解释 80-demos/D02-cugetexporttable/execution-trace.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：`source/cuda` 快照（仅含 `src/`；无 `.git`，提交未知；文件时间戳 2022-02-22）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-10-05
- 前置阅读：[项目入口](../../README.md)、[D02 README](./README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。

## 执行轨迹

```text
调用方（memcheck 例）
OpenCL/CBL 侧          memcheck 侧
  │                      │
tools_cu_etbl.c:10      check_core.c:1031/1037/1043/1049/…（13 处）
  └─ cuiGetExportTable     └─ cuGetExportTable(&table, &CU_ETID_Tools*)
        │                        │
        │              （公开名→cuapi* 重命名由缺失的 generated_cuapi.h 完成，cuda_internal.h:41）
        ▼                        ▼
           cuapiGetExportTable  [src/api/apietbl.c:48-58]   CU_TRACE_FUNCTION；无 cuiInitCheck
                        │
                        ▼
           cuiGetExportTable  [src/cui/cuietbl.c:625-648]
                        │  校验指针非空（:629-631）
                        │  *ppExportTable = 0（:632）
                        ▼
              线性扫描 g_etblMap（:521-603，75 项）
              逐项 cuiCompareUuid = memcmp(bytes,16)（:611-614）
                        │
              ┌─────────┴────────────┐
         命中（:640-642）            未命中（:647）
         *ppExportTable=表指针      返回 CUDA_ERROR_INVALID_VALUE
         return CUDA_SUCCESS        （文档称 NOT_FOUND，见 failure-paths）
```

## 命中的代表性结果（UUID → 表）

以 `CU_ETID_CudaGraphs`（UUID 常量在缺失的 `cuda_etbl/cuda_graphs.h`，具体值不可复核）为例，返回 `g_etblCudaGraphs`：

```text
g_etblCudaGraphs  [src/etbl/cuda_graphs/cuda_graphs.c:389]
 ├─ :391  sizeof 槽位（=表大小，ABI 版本字段）
 ├─ :392-417  26× NULL（占位，调用方必须判空后使用）
 └─ :418-429  12× 函数指针
     ├─ :418 cuapiStreamGetCaptureInfo        → [src/api/apistream.c:757]
     ├─ :419 cuapiStreamGetCaptureInfo_ptsz   → [src/api/apistream.c:763]
     ├─ :420-424  eti* 驱动内测试用例（本文件 :50/:139/:166/:209/:224）
     ├─ :427 cuapiGraphConditionalNodeGetBody → 本文件 :330
     └─ :429 cuapiGraphSetNonpublicCaptureEnabled → 本文件 :384
     （:425/:426/:428 为悬空引用，见 failure-paths）
```

## 调用方如何用表

```text
memcheck: check_core.c:1031 → gvars->tools.callbacks; :1037 → .context;
         :1043 → .device; :1049 → .module; :1055 → .memory; :1061 → .memcpy;
         :1067 → .pushBufferHal; :1073 → .trap; :1079 → .cnp;
         :1087 → .uvm8; :1095 → .link; :1102 → .graph
         随后 :1109 Subscribe，用表内函数指针操作工具链
```

跨线程/跨模块边界：导出表机制本身无锁、无初始化依赖（[apietbl.c:51-55]），表为只读静态数据，任意线程可并发查表。真正的并发控制转移到**表内回调实现**（如 memcheck 的 `EnableAllCallbacks`），不在本 Demo 链路上。

## 相关文档

- [项目入口](../../README.md)
- [分析状态](../../00-overview/analysis-state.md)
- [源码证据索引](../../00-overview/evidence-index.md)
- [D02 数据与状态轨迹](./data-and-state-trace.md)

## 源码证据摘要

本页结论所需的源码路径和行号以 [源码证据索引](../../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题

目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议

继续阅读 [数据与状态轨迹](./data-and-state-trace.md) 查看查询前后数据变化，再阅读 [失败路径](./failure-paths.md)。