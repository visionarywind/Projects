# D02 修改练习

- 文档目的：解释 80-demos/D02-cugetexporttable/modification-exercises.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：`source/cuda` 快照（仅含 `src/`；无 `.git`，提交未知；文件时间戳 2022-02-22）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-10-05
- 前置阅读：[项目入口](../../README.md)、[D02 README](./README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。

## 修改练习 1：新增一张导出表（照源码自带配方走）

源码在 [src/cui/cuietbl.c:35-39] 给出了**添加新表的四步配方**：定义 `ETBL_ENTRY_*` 宏（含 `#else` 为空的分支）→ 将其加入 `g_etblMap`（[:521-603]）→ 配套头文件提供 UUID 与结构体 → 编译。

具体落点：

1. 在 `src/cui/cuietbl.c`：`#include "cuda_etbl/mynew.h"`、`extern` 声明 `g_etblMyNew`、写 `#define ETBL_ENTRY_MyNew { &CU_ETID_MyNew, &g_etblMyNew },`（对照 :42-43）。
2. 在上面的列表加一行 `ETBL_ENTRY_MyNew`（对照 :523）：该宏在 `#else` 分支下应展开为空（对照 :39），避免条件编译不一致。
3. 表体实现仿照 [src/etbl/suballoc/suballoc.c:75-79]：首槽 `sizeof`、后续函数指针、空槽写 `NULL`。
4. 不要用 `#if` 包住 `g_etblMap` 内部条目（[:516-520] 明确禁止），条件逻辑必须落在 `ETBL_ENTRY_*` 宏定义处。

**验证方式（本检出不可执行）**：库内调用方（如 memcheck）改用新 UUID 取表、走表内回调并观察无回归；受本快照限制，只能做源码级核对（新增宏与 extern 成对、表体槽数与原头文件一致）。

## 修改练习 2：修复“未命中错误码与文档不一致”

把 [src/cui/cuietbl.c:647] 的 `CUDA_ERROR_INVALID_VALUE` 改为文档承诺的 `CUDA_ERROR_NOT_FOUND`（[apietbl.c:36,45]）。

- 影响面：memcheck [check_core.c:1032-1036] 对所有非 `CUDA_SUCCESS` 一律 goto error，不受影响；但任何对 `CUDA_ERROR_NOT_FOUND` 判空的替代调用方会从此正确区分“表不存在”与“参数非法”。
- 验证：缺符号场景下返回码变化；对齐 `cuisym.c:253` 的既有语义。
- 遗留：需判断 `CU_ETID_*` 头文件（缺失）中是否有其他调用方依赖旧码。

## 修改练习 3：清点并恢复 `g_etblCudaGraphs` 的悬空引用

三处符号在快照内 0 命中：[src/etbl/cuda_graphs/cuda_graphs.c:425,426,428]（`cuapiGraphAddConditionalNode`、`cuapiGraphExecGetConditionalNodeSchedulingHandle`、`cuapiStreamCaptureGetLastNode`）。

- 若快照缺失对应实现文件，则恢复该文件后本表即可整树编译；若属条件编译排除，则应在 `#else` 分支将槽位写 `NULL`（对照 :392-417 的占位做法）。
- 验证：在完整源树下 `nm libcuda.so | grep cuapiGraphAddConditionalNode` 应出现符号；快照内无 `make`/`.nvmk`，不可在本检出构建。

## 调试命令（在完整环境）

- 反查导出符号：`nm -D libcuda.so | grep cuGetExportTable`
- 表内符号：`nm libcuda.a | grep -E 'g_etblCudaGraphs|cuapiGraphAddConditionalNode'`
- UUID 来源：`grep -n CU_ETID_CudaGraphs cuda_etbl/cuda_graphs.h`（头文件缺失，需从完整树获取）

## 相关文档

- [项目入口](../../README.md)
- [分析状态](../../00-overview/analysis-state.md)
- [源码证据索引](../../00-overview/evidence-index.md)

## 源码证据摘要

本页结论所需的源码路径和行号以 [源码证据索引](../../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。`nm` 命令为建议，未在本检出运行。

## 未解决问题

目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议

如需补全 Graph 生命周期覆盖，回到 [M06 模块](../../01-modules/M06-module-launch/graph-resource-lifecycle.md) 阅读其静态轨迹；本 Demo 不覆盖 Graph capture/launch 路径。