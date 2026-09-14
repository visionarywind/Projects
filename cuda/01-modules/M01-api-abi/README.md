# M01 API/ABI 与导出层

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
