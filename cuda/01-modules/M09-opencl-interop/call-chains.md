# M09 调用链、数据结构与互操作边界

## Platform 查询

```text
clGetPlatformIDs
 → khrIcdInitialize
 → khrIcdOsVendorsEnumerateOnce
 → khrIcdVendors list
 → platform handle
```

## Context 创建

```text
clCreateContext
 → ICD initialize
 → validate devices[0]
 → devices[0]->dispatch->clCreateContext
 → NVIDIA vendor clCreateContext
 → cliGlobals / CUI API_OPENCL_NV context
```

`clCreateContextFromType` 则从 context properties、device type 或首个 vendor platform 选择 platform 后 dispatch（静态确认：[src/icd_rel/icd_dispatch.c:217-247]）。

## 对象关系

- `KHRicdVendor`：loader vendor list 节点，持有 platform、suffix 和 library handle。
- `CLIglobals`：OpenCL vendor 全局状态，持有 platform、dispatch table、配置和 initialized。
- `CLIicdDispatchTable`：按 ABI 固定顺序存储入口指针。
- OpenCL context/queue/mem/program/kernel/event：`src/cl` 的 vendor 对象，底层可关联 `CUctx`、`CUIstream`、`CUmemobj` 和 task/event。

## 互操作

GL 入口在 ICD table 的 OpenCL 1.0 区段中出现，D3D10 入口按 `_WIN32` 条件加入或置 NULL（静态确认：[src/cl/cliicd.c:99-126]）。这证明入口/编译条件，不足以证明 GL/D3D resource acquire/release 已成功映射到 CUDA memobj；后者需要继续追踪 `clgl.c`、`clid3d.cpp`、`cuiextinterop.c` 和底层 sync。

## 所有权边界

loader 的临时库句柄和 platform 查询数组由 `khrIcdVendorAdd` 清理；vendor 节点和 dispatch table 由 loader/vendor deinitialize 清理。OpenCL public object 的 retain/release 与 CUI context/memobj 的释放并非同一引用计数，修改时必须分别核对两套生命周期。
