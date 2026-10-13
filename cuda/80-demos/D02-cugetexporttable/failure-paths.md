# D02 失败路径

- 文档目的：解释 80-demos/D02-cugetexporttable/failure-paths.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：`source/cuda` 快照（仅含 `src/`；无 `.git`，提交未知；文件时间戳 2022-02-22）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-10-05
- 前置阅读：[项目入口](../../README.md)、[D02 执行轨迹](./execution-trace.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。

## 失败路径 1：空参

```text
cuGetExportTable(NULL, &uuid) 或 cuGetExportTable(&out, NULL)
  → cuapiGetExportTable [apietbl.c:49]
  → cuiGetExportTable  [cuietbl.c:629-631]
  → return CUDA_ERROR_INVALID_VALUE
```

调用方对 `NULL` 无重试逻辑；memcheck 只在 `status != CUDA_SUCCESS` 时打印并 goto error（[check_core.c:1032-1036]）。

## 失败路径 2：UUID 未命中

```text
线性扫描 g_etblMap 无匹配（[cuietbl.c:634-645]）
  → return CUDA_ERROR_INVALID_VALUE（:647）
```

**与文档不一致**：[apietbl.c:36] 与返回列表 [apietbl.c:45] 声明未命中返回 `CUDA_ERROR_NOT_FOUND`，且该常量在快照内被广泛使用（67 处，如 [src/cui/cuisym.c:253] 符号未找到返回它），因此 `cuiGetExportTable` 用 `CUDA_ERROR_INVALID_VALUE` 属于实现与文档的偏差。可能后果：把“表不存在”与“参数非法”混为一谈；对 `CUDA_ERROR_NOT_FOUND` 特判的调用方会把未命中当参数错误。

## 失败路径 3：表槽为 NULL（不支持特性）

```text
成功取得表 g_etblCudaGraphs（:389）
  → 槽位多为 NULL（:392-417 连续 26 个）
  → 调用方若未判空就解引用 → 空指针崩溃
```

快照内证据：表体全部槽位以 NULL 显式占位，无默认实现。契约是“查询成功 ≠ 槽位可用”，调用方必须逐槽判空。

## 失败路径 4：悬空函数引用（本 Demo 独有的静态缺陷）

`g_etblCudaGraphs` 12 个函数槽中 **3 个符号在快照全树不存在**：

| 槽行号 | 符号 | 快照内命中 |
|---|---|---|
| :425 | `cuapiGraphAddConditionalNode` | **0** |
| :426 | `cuapiGraphExecGetConditionalNodeSchedulingHandle` | **0** |
| :428 | `cuapiStreamCaptureGetLastNode` | **0** |

同表其余 9 槽均可在快照内定位（`src/api/apistream.c:757,763`、本文件 `:50/:139/:166/:209/:224/:330/:384`）。这说明：(a) 该表不能直接作为整树编译单元（链接期即失败）；(b) 该快照是**部分版本**，这三处要么对应实现文件缺失，要么属于条件编译排除分支。已记录到 `analysis-state.md` 的版本推断，属“源码自身不完整”而非本知识库引用越界。

## 错误码跨模块转换（补充路径）

OpenCL 侧包装 [src/etbl/tools_cu_etbl.c:10-17]：`cuiGetExportTable` 返回的 CUDA 错误码经 `NvCliMapCUErrorToCL(status)` 映射为 CL 错误码再上抛——失败路径存在一次显式的错误语义转换边界。

## 深度审计（失败/异常维度）

| 失败路径 | 触发 | 实现 | 证据行 | 调用方处理 |
|---|---|---|---|---|
| 空参 | 任一入参 NULL | `CUDA_ERROR_INVALID_VALUE` | `cuietbl.c:629-631` | memcheck goto error（`check_core.c:1035`） |
| 未命中 | UUID 不在表内 | `CUDA_ERROR_INVALID_VALUE`（文档称 `NOT_FOUND`） | `cuietbl.c:647`/`apietbl.c:36,45` | 同上 |
| 空槽 | 表已取得但槽位 NULL | 未防护、由调用方判空 | `cuda_graphs.c:392-417` | 是本 Demo 观察到的调用契约风险 |
| 悬空符号 | 编译/链接 | 3 个 `cuapi*` 无定义 | `cuda_graphs.c:425-428` | 快照无法构建该表 |

## 相关文档

- [项目入口](../../README.md)
- [分析状态](../../00-overview/analysis-state.md)
- [源码证据索引](../../00-overview/evidence-index.md)

## 源码证据摘要

本页结论所需的源码路径和行号以 [源码证据索引](../../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。悬空符号判定基于对 `src/` 全树的 `grep` 计数为 0。

## 未解决问题

目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议

继续阅读 [调试走查](./debug-walkthrough.md) 获取断点与观察变量，或直接看 [修改练习](./modification-exercises.md)。