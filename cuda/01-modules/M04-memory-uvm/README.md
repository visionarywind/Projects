# M04 Memory/UVA/UVM：内存对象与地址映射

- **职责**：把 API 内存请求变成 `CUmemdesc`、`CUmemobj`、`CUmemblock` 层次，管理 device/host/managed memory、内部 suballocation、UVA/UVM、P2P 和外部共享。
- **证据状态**：设备/host 分配、base-pointer 释放、suballocator best-fit/split/coalesce、UVM AL 选择、managed registration rollback、DAG/stream 状态和 P2P bookkeeping 已静态确认；fault/migration、IPC/external backend 未闭合。
- **核心文件**：`src/api/apimem.c`、`src/cui/cuimem.c`、`memobj.c`、`memblock.c`、`memmgr.c`、`suballocator.c`、`heap.c`、`cuivamanager.c`、`cuiuvm.c`。

专题阅读：[GPU 显存池化](gpu-memory-pooling.md)。这里的“池化”指内部 memblock suballocator，不是 CUDA 11+ 的公开 `cuMemPool*` API。

## 对象层次

```text
API pointer
  → CUmemdesc (属性/来源/映射策略)
  → CUmemobj (用户可见 allocation / mapping)
  → CUmemblock (物理 backing、VA、共享实例)
  → DMAL/HAL memory resource
```

同一个 backing block 可以支持多个 memobj，例如 P2P 或 external sharing；因此“一个设备指针等于一次物理分配”是不成立的推断。

## 关键约束

- allocation 先经过 `cuiInitCheckEx` 和 capture interaction 检查，再进入 memmgr（静态确认：[src/api/apimem.c:67-98]）。
- `memdesc` 的 owner、location、type、apiSource、mapDevice 影响对象合法性、查找和释放策略（静态确认：[src/api/apimem.c:90-98]）。
- 子分配被固定 VA/固定物理地址、noSuballoc、不可映射、sharing、环境开关等条件禁止（静态确认：[src/cui/memobj.c:82-110]）。
- 可 suballocate 的请求按 HAL generic block size 选择已有兼容 memblock；空闲区由 radix tree 做 best-fit，释放后 split 区域与相邻空闲区 coalesce（静态确认：[src/cui/memobj.c:265-308]；[src/cui/suballocator.c:163-220,343-405]）。
- `CUheap` 只管理 VA reservation/lookup；物理 backing 的申请与释放仍由 DMAL `memblockAlloc`/`memblockFree` 完成（静态确认：[src/cui/memblock.c:471-562,682-740]）。
- 释放不仅查找指针，还要求来源属于允许的 malloc/managed/external API，且必须是 base pointer（静态确认：[src/api/apimem.c:280-319]）。

## 相关模块

M03 提供设备/HAL 的 block size 和平台内存操作；M05 为异步 memcpy、同步和 stream 生命周期提供提交环境；M06 的 launch memory tracking 会把 function、module、QMD 和参数引用加入追踪列表。
