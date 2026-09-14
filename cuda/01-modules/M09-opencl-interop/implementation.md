# M09 实现与生命周期

## ICD vendor 枚举

`khrIcdInitialize` 只负责触发一次 OS vendor enumeration；`khrIcdVendorAdd` 加载动态库，取得 `clGetExtensionFunctionAddress` 和 `clIcdGetPlatformIDsKHR`，查询 platform 数量，读取每个平台的 ICD suffix，再把 library/platform/suffix 追加到 vendor list（静态确认：[src/icd_rel/icd.c:43-208]）。临时 platform array 和初始 library handle 在函数末尾释放/卸载，vendor 节点保留自己的 library handle。

## ICD dispatch

`clGetPlatformIDs` 初始化 loader、校验 `(num_entries, platforms, num_platforms)` 组合、先清零输出，再枚举 vendor platforms；没有 vendor 时返回 `CL_PLATFORM_NOT_FOUND_KHR`（静态确认：[src/icd_rel/icd_dispatch.c:43-90]）。`clGetPlatformInfo`、device、context 等入口先验证 public handle，再调用对象 dispatch table（静态确认：[src/icd_rel/icd_dispatch.c:93-108,111-214]）。

## Vendor 初始化

`cliGlobalsInitialize` 先调用 `cuiInit(CUI_API_OPENCL_NV)`，随后在 `globals.initMutex` 下读取配置、创建 ICD dispatch table 和 NVIDIA platform；任一步失败就销毁 platform/table 并清零全局结构，成功后设置 initialized（静态确认：[src/cl/cliglobals.c:58-118]）。deinitialize 使用同一把 mutex 销毁 platform 和 dispatch table（静态确认：[src/cl/cliglobals.c:121-139]）。

## Dispatch table

`cliIcdDispatchTableCreate` 分配并清零 256-entry table，按 OpenCL 版本和平台条件写入入口；Windows-only D3D10 入口在非 Windows 下用 NULL 占位，版本宏决定 1.1/1.2/2.0/SVM 入口是否存在（静态确认：[src/cl/cliicd.c:6-14,24-144,146-235]）。固定顺序是 ABI 兼容的一部分。

## 配置

OpenCL worker idle timeout、32-bit device pointers、GL event support、image handles 等配置从环境变量读取；`NVCL_IDLE_TIMEOUT_MS` 影响 DAG 为空后 worker thread 保持 active 的时间（静态确认：[src/cl/cliglobals.c:12-56]）。
