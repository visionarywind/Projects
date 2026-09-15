# M01 API/ABI 与导出层

- 文档目的：解释 01-modules/M01-api-abi/README.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M01-api-abi/README.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


- **职责**：把公开 CUDA Driver API 参数转换为内部 CUI 调用，并维护旧 ABI、v2 类型、PTDS/PTSZ 入口和跨平台导出。
- **证据状态**：核心入口静态确认；生成器与最终链接细节部分未知。
- **源码范围**：`inc/cuda.h`、`src/api/`、`src/cuda_master.def`、`generated_cuapi.h`。

## 入口地图

| 入口 | wrapper | 主要落点 |
|---|---|---|
| `cuInit` | `cuapiInit` | `cuiGlobalMutexInitOnce` → `cuiInit` |
| `cuCtxCreate_v2` | `cuapiCtxCreate_v2` | `cuapiCtxCreate_common` → `cuiCtxCreate` |
| `cuMemAlloc_v2` | `cuapiMemAlloc_v2` | `cuapiMemAlloc_common` → `memobjAlloc` |
| `cuStreamCreate` | `cuapiStreamCreate` | `cuapiStreamCreate_common` → `cuiStreamCreate` |
| `cuLaunchKernel` | `cuapiLaunchKernel` | `cuapiLaunchKernelCommon` → launch/graph |

## 设计约束

1. 公开函数处理 ABI 版本、空指针、范围和句柄错误；不应在 wrapper 中直接持有硬件资源（静态确认：[src/api/apimem.c:52-118]、[src/api/apilaunch.c:212-301]）。
2. 内部实现使用 v2 类型：`CUdeviceptr` 等类型在 `cuda_types.h` 中显式别名为 `_v2`（静态确认：[src/cuda_types.h:29-34]）。
3. 旧入口和 v2 入口共享 common 函数，但传入不同 context/API 检查策略（静态确认：[src/api/apimem.c:120-136]、[src/api/apictx.c:38-95]）。
4. 导出表与构建对象列表是两个契约：`.def` 决定符号可见性，`cuda.nvmk` 决定对象纳入 `libcuda`；二者的自动生成关系尚未完全确认。

## 关键风险

- 版本宏、函数后缀和参数宽度必须同时修改，否则会形成可链接但语义不一致的 ABI。
- wrapper 通过 TLS/context 获取状态，绕过 common 检查会改变错误码和 sticky error 观察时机。
- `inc/cuda.h` 的宏可能令源码调用名与实际导出名不同；修改前必须同时检查 `.def` 和 generated API 输入。

## 文档元数据（规范补充）

- 文档目的：说明 `01-modules/M01-api-abi/README.md` 的源码分析范围、结论和维护入口。
- 适用范围：当前项目对应模块/入口的静态源码与测试分析。
- 对应源码版本：以本项目 `00-overview/analysis-state.md` 或同页版本字段为准。
- 证据状态：静态源码证据；未执行的构建、测试、GPU、网络或多进程行为保持“未验证”。
- 最后更新：2026-09-15
- 前置阅读：本项目根 README 与 `00-overview/analysis-state.md`。
- 后续阅读：本模块/示例的实现、测试和风险页面。

## 深度审计

| 分析对象 | 入口落地 | 正常路径 | 分支 | 异常 | 清理 | 数据生命周期 | 执行上下文 | 行级证据 | Demo 映射 | 状态/缺口 |
|---|---|---|---|---|---|---|---|---|---|---|
| `01-modules/M01-api-abi` | 已完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 已映射或不适用 | 部分完成：动态行为、边界或专用变体仍需验证 |

## 相关文档
- [项目入口](../../README.md)
- [分析状态](../../00-overview/analysis-state.md)
- [源码证据索引](../../00-overview/evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](../../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](../../00-overview/analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
