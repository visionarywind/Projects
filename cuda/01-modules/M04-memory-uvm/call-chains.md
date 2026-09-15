# M04 调用链与生命周期

- 文档目的：解释 01-modules/M04-memory-uvm/call-chains.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M04-memory-uvm/call-chains.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


## 分配链

```text
cuMemAlloc_v2
 → cuapiMemAlloc_v2                       [src/api/apimem.c:129-136]
 → cuapiMemAlloc_common                   [src/api/apimem.c:52-118]
 → cuiInitCheckEx / capture check
 → memobjAlloc(ctx->memmgr, memdesc)
 → memobjAllocMemblockBacking
 → HAL memblockGetAllocSize
 → memblock allocation / VA mapping
 → memglobalsRegisterMemobj
 → memobjGetDevicePtr
```

最后两步是重要的所有权边界：局部 memmgr 成功不代表全局/P2P 登记成功；登记失败必须回滚。

## 池化命中与新 block 链

```text
memobjAlloc
 → memobjAllocMemblockBacking
 → canSuballocate + HAL genericBlocksize/alignment
 → memmgrGetSuballocatorRadixTree (descriptor compatibility)
 → radixTreeFindGEQ (best-fit)
 → split free region / attach memobj

no fit
 → allocSize = max(request, genericBlocksize)
 → memblockAlloc
 → DMAL.memblockAlloc + VA/mapping
 → suballocatorSuballocateFromNewMemblock
```

可池化请求复用已有 `CUmemblock`，不会增加 `memblock->serial`；不兼容、过大、固定地址、sharing 或禁用 suballocator 时才创建新 block（静态确认：[src/cui/memobj.c:82-110,265-375]；[src/cui/memmgr.c:89-136]）。

## Free 链

```text
cuMemFree_v2
 → cuapiMemFree_v2                       [src/api/apimem.c:346-349]
 → cuapiMemFree_common                   [src/api/apimem.c:231-339]
 → memobjGetByUnifiedPtr / GetByDevicePtr
 → API-source + sharing + base-pointer checks
 → memglobalsUnregisterMemobj
 → cuiCtxSynchronize
 → cuiMemToolsNotifyFree
 → memobjFreeNoSync
```

这里显式同步 context，故 free 的 API 返回边界不同于普通异步操作；同步失败时不执行 `memobjFreeNoSync`，但仍解锁并返回错误（静态确认：[src/api/apimem.c:321-339]）。

## Host mapped 链

```text
cudaHostAlloc / cuMemAllocHost
 → host CUmemdesc(location=HOST,mapHost=HOST_VA)
 → memobj/memblock
 → hostvaddr + devptr/uvavaddr
 → cudaHostGetDevicePointer
```

D01 的 `cudaHostAlloc(...Mapped)` 和 UVA 断言覆盖该对象关系（静态确认：[tests/cuda_test/basic_sanity.cu:174-209]）。

## 清理链

全局 init 失败按 `primary memmgr → userd VA → UVA → UVM → globals` 回滚；单对象失败按登记/lock 级别回滚。stream detach 还可能持有 launch 对 memobj 的异步引用，不能仅凭 host pointer 消失判断 backing 已可回收。

池化 free 链为：

```text
memobjFree（先同步 context）
 → memobjFreeInternal
 → suballocatorFreeMemobj
 → radix tree insert + adjacent coalesce
 → 若 memblock 无 memobj：suballocatorRemoveMemblockNode
 → memblockFree → DMAL free + VA/unmap
```

证据：[src/cui/memobj.c:946-964,878-943]；[src/cui/suballocator.c:343-405]；[src/cui/memblock.c:682-740]。

## UVM DAG / stream 状态

```text
stream/event/context work
 → process-global DAG node (unique ID)
 → stream/event/null-stream dependency traversal
 → UVM abstraction layer
 → set stream running/stopped
 → host-access changes / managed-memory visibility
```

DAG 节点跨 context 保存 owner stream、GPU/CPU work 和 null-stream barrier；IPC event 明确不纳入该 DAG。`uvmSetStreamsStopped` 在 manager lock 下更新 backend 和 running-stream count，失败时恢复已停止 stream（静态确认：[src/cui/cuiuvmdag.c:1-25,71-153]；[src/cui/cuiuvm.c:2621-2669]）。

## UVM 注册与 submemblock 映射

```text
memobj allocation / managed object
 → uvmRegisterMemobj
 → attach first stream + record stream range
 → AL.memobjRegister
 → registered flag
 → uvmRegisterMemobjFinalize / stream changes
 → uvmUnregisterMemobj
```

backend 注册失败会 detach 当前 stream；因此对象的 UVM 可见性和 stream tracking 必须一起回滚（静态确认：[src/cui/cuiuvm.c:约 1800-1950]）。submemblock 的 map/unmap 公共入口在 UVM manager lock 下调用动态 AL；Kd8 路径还可能先拆分 host pages、更新 non-managed heap ranges，或为 SKED/reflected memory 建立 dynamic-parallelism 映射。fault/migration 最终实现仍未从当前树闭合。

## P2P bookkeeping

```text
cuCtxEnablePeerAccess / mapping request
 → peerMapRetainAccess(type)
 → first-reference edge notification
 → memblock/DMAL peer map (backend)

release
 → peerMapReleaseAccess(type)
 → last-reference edge notification
 → remove peer entry when all refcounts reach zero
```

`CUpeerMap` 的访问类型各自计数，`destroy` 可一次清除 peer 的所有类型；这部分只证明软件引用表，不证明设备映射已完成（静态确认：[src/cui/peermap.c:12-134]）。

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
