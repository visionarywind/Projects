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

## Public object 与 context 销毁

`CLIobjectData` 位于所有 public object 起始位置，internal tree 的 parent 是创建对象所依附的对象；`cliObjectDestroy` 的契约要求对象没有 children 和非零引用后才释放并从树中摘除（静态确认：[src/cl/cliobject.h:98-169]）。例如 command queue 创建时先以 context 为 parent 创建 object，再建立 CUI queue；创建失败统一销毁半成品，成功后才初始化 public refcount（[src/cl/clcommandqueue.c:15-76]）。

context 的销毁顺序是 worker thread、callback thread、task graph、pinned memory tracking、每个 device 的 `CUctx`，最后释放数组、锁与 migration mutex；这说明 task/queue/event 的后台消费者必须先停止，不能仅依靠 public context refcount（静态确认：[src/cl/clicontext.c:467-545]）。

## Event 与异步提交

`clWaitForEvents` 对每个 event 轮询 `hasBeenSubmitted`，先聚合 task result；随后按 `deviceIdx` 把 event marker 合成一个 `ctxMarker`，调用 marker status/flush/wait。GPU 已完成的分支绕过普通 marker wait，因此手动清除 pinned memcpy tracking（静态确认：[src/cl/clevent.c:106-217]）。这里的 submitted、GPU completed 与 public event release 是三种不同状态。

## Graphics/external interop

GL memory object 保存 `interopType`、resource 数组、GL name/type、map hash 和 image level/slice；注册或取 image/size 失败都会 unregister 并 destroy object（静态确认：[src/cl/clgl.c:108-204]）。CUDA external memory 则先由 DMAL 打开共享 handle，再通过 `memobjAlloc` 建立 buffer 或 shared semaphore backing；`cuiExternalMemoryMapAsSemaphore` 保存 `semaMemobj`，mapped buffer 返回 device pointer，但当前片段没有证明该 pointer 的 public owner 和 close 时机（静态确认：[src/cui/cuiextinterop.c:21-171]）。
