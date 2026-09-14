# M04 调用链与生命周期

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

## 清理链

全局 init 失败按 `primary memmgr → userd VA → UVA → UVM → globals` 回滚；单对象失败按登记/lock 级别回滚。stream detach 还可能持有 launch 对 memobj 的异步引用，不能仅凭 host pointer 消失判断 backing 已可回收。

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