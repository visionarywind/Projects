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

## 所有权与异步边界

```text
CLIobjectData(parent/children + public/internal refs)
 → public clRelease*
 → cliObjectTryRelease
 → type-specific destroy
 → CUI queue/memobj/event/task cleanup
 → parent can be destroyed only after children/internal refs vanish
```

```text
clWaitForEvents
 → validate + wait until submitted
 → group by device
 → ctxMarkerSetMax
 → marker status / flush / wait
 → clear pinned memcpy tracking on GPU-completed fast path
```

```text
GL/D3D/external handle
 → resource registration or DMAL handle open
 → CLI/CUmemobj backing
 → enqueue acquire/release or semaphore acquire/release
 → marker/task completion
 → object/handle close
```

最后一条链的 acquire/release 到具体 HAL/RM fence 的映射未在当前证据中完全闭合；不要将 public `clReleaseMemObject` 直接等同于外部图形资源已完成释放。
