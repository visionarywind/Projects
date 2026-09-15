# GPU 显存池化：suballocator、memblock 与真实 backing

- 文档目的：说明当前源码中的“显存池化”究竟由哪些层实现，以及对象、地址和物理 backing 的所有权边界。
- 适用范围：`memobj`/`memblock` 内部 suballocation；不把 CUDA 11+ 的公开 `cuMemPool*` API 或外部 DMAL 实现臆测为本快照行为。
- 对应源码版本：`source/cuda` 快照；源码内部版本为 CUDA 10.2（`CUDA_VERSION=10020`）。
- 证据状态：suballocator 主链已确认；DMAL/RM 物理分配和运行时性能未验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

当前树没有 `cuMemPool*` 公共 API。可确认的池化机制是：一次较大的 `CUmemblock` backing 被切成多个 `CUmemobj` 区域，由 `suballocator.c` 的 best-fit radix tree 管理空闲区；释放时回收到树并与相邻空闲区合并。物理显存并不是由该树直接申请，而是在 block 创建时交给 `memmgr->dmal.memblockAlloc`，因此“池化命中”只证明复用已有 block，不证明 RM/设备端采用了某种物理页分配算法。

## 三层资源模型

```text
CUmemobj（用户可见分配，offset + size）
        ↓ 切片
CUmemblock（一次 backing，memRegions 记录已用/空闲区）
        ↓ 真实申请/释放
DMAL.memblockAlloc / DMAL.memblockFree（RM/平台边界）
```

`CUheap` 另负责 device pointer、device VA、host VA 和 range 的地址索引/保留；它是虚拟地址管理器，不是显存池。`CUmemmgr` 同时持有这些 heap、memblock 链表和兼容 descriptor 的 suballocator radix tree（静态确认：[src/cui/memmgr.h:31-138]；初始化：[src/cui/memmgr.c:140-213]）。

## 分配决策

1. `canSuballocate()` 拒绝显式 `noSuballoc`、固定 device VA/物理地址、host/device 都不映射、API 可见分配的全局禁用开关以及所有 sharing 请求（[src/cui/memobj.c:82-110]）。
2. HAL 根据内存类型返回 block 建议大小和 heap alignment；Pascal 等架构把 generic、pushbuffer、GPFIFO、function、texture、managed 等类型分开处理（[src/cui/hal/pascal/pascal_mem.c:55-145]），并在设备初始化时绑定到 HAL 函数表（[src/cui/hal/pascal/pascal_devstate.c:192-193]）。
3. `memobjAllocMemblockBacking()` 将 `noSuballoc` 最终设为“不支持 suballocation 或请求 `size > genericBlocksize`”。可池化请求先按 descriptor 兼容性查找已有 tree，再调用 best-fit；找不到时申请至少 `max(size, genericBlocksize)` 的新 block，并按设备/host page size 向上取整（[src/cui/memobj.c:265-308]）。
4. 新 block 若允许 suballocation，先把请求区标记为已用，将剩余区插入 radix tree；若整 block 被请求占满，则把 block 转为 `noSuballoc` 以走更快的单对象释放路径（[src/cui/memobj.c:352-369]；[src/cui/suballocator.c:278-340]）。

descriptor 兼容性不是只比较大小，而是比较 location、cache、type、layout、owner、mapping、page size、compression、handle types 等字段；`CU_MEM_MAP_DEVICE_RANGE` 还要求同一 `memrange`（[src/cui/memobj.c:113-163]）。因此每种属性组合都可能拥有独立的 suballocator tree。

## Best-fit、切分、合并和释放

- 空闲区以 `CUsuballocatorRadixNode.node.key = size` 为 radix-tree key，同时挂在所属 memblock 的 `memRegions` 双向链表上（[src/cui/suballocator.h:20-39]）。
- `radixTreeFindGEQ()` 取得不小于请求大小的最小区；切分时按 memblock alignment 向上取整，返回前半段、把尾部重新插回 tree（[src/cui/suballocator.c:163-220]）。
- 释放 memobj 时复用 memobj/节点 union 的存储创建空闲节点，插回 tree 后与前后相邻空闲区合并；最后一个 memobj 释放时移除 block 节点并调用 `memblockFree`（[src/cui/suballocator.c:343-405]；[src/cui/memobj.c:878-943]）。
- `memblockFragmentation()` 计算 `(totalFree - largestFree) / totalFree`，仅对可 suballocate block 报告碎片率；`noSuballoc` block 的碎片率固定为 0（[src/cui/memblock.c:967-1015]）。

## 真实 backing 与所有权

`memblockAlloc()` 负责复制 descriptor、确定 HAL alignment、按 suballocation 需要对 block size 取整，然后调用上下文策略或默认的 `memblockAllocResources()`；后者依次保留 UVA、检查 pinned-memory 限额、调用 DMAL `memblockAlloc`，再按 flags 映射 host/device（[src/cui/memblock.c:565-680]；[src/cui/memblock.c:471-562]）。释放反向执行 unmap、DMAL `memblockFree`、注销 pinned memory 和 UVA（[src/cui/memblock.c:682-740]）。因此应把“复用 block”与“释放 GPU 物理资源”分开描述。

## 相关池化但非同一机制

Graph 的 QMD、constant-bank、stream、semaphore 和 texture/sampler pool 是执行资源池，见 M05/M06；它们不等于 memobj suballocator。NVN interop 另维护 `CUINVNmempool`，按 NVN pool 句柄复用一个 `CUmemobj` 并以 `offsetInPool` 返回 buffer 切片（[src/cuda_structs_nvn.h:37-86]；[src/cui/cuinvn.cpp:573-630]），这是平台互操作 bookkeeping，不应泛化为 CUDA 通用显存池。

## 测试与缺口

`memmgr` nvmake 只注册创建、VA 禁用和 range overlap 测试（[tests/cuda_test/memmgr/tests.nvmk:1-4]）。`unittest_memobj_alloc.hpp` 仍保留 suballocation 分区计划，但引用了当前未见运行时初始化的 `membins/membin` 字段（[tests/cuda_test/memobj/unittest_memobj_alloc.hpp:170-255]）。当前没有执行 nvmake/GPU 测试；应补充 best-fit、split/coalesce、descriptor 不兼容、`disableSuballocator`、DMAL 失败回滚和异步 free 的运行验证。

## 深度审计

| 分析对象 | 入口落地 | 正常路径 | 分支 | 异常 | 清理 | 数据生命周期 | 执行上下文 | 行级证据 | Demo 映射 | 状态/缺口 |
|---|---|---|---|---|---|---|---|---|---|---|
| suballocator/memblock | 已完成 | 已完成 | 已完成 | 部分完成 | 已完成 | 已完成 | memmgr lock 已确认 | 已完成 | D01 仅覆盖基础 memobj | 部分完成：DMAL 物理端和运行时测试待补 |

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
