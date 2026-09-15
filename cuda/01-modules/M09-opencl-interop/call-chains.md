# M09 调用链、数据结构与互操作边界

- 文档目的：解释 01-modules/M09-opencl-interop/call-chains.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M09-opencl-interop/call-chains.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


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
