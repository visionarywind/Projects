# M04 数据结构、接口与源码地图

## 数据结构

| 类型 | 语义 | 关键字段/关系 |
|---|---|---|
| `CUmemdesc` | 一次请求的属性 | `flags`、`memrange`、`memshare`、固定地址参数 |
| `CUmemobj` | 可查找/可释放的逻辑对象 | `memblock`、`memblockOffset`、`allocSize`、device/host pointer |
| `CUmemblock` | backing 与映射单元 | VA、host VA、物理/共享实例、alignment |
| `CUmemmgr` | context/device 内对象索引和分配策略 | pointer lookup、suballocator、device |
| `CUuvaManager` | 进程统一 VA 关系 | host/device unified pointer lookup |
| `CUuvmManagedAttr` | managed range 属性 | preferred/accessed-by/read-mostly/prefetch/parent block |
| `CUIuvmDag`/`CUIuvmDagNode` | 跨 stream/context 的同步依赖 | unique node ID、event、null barriers |
| `CUpeerMap` | peer access 软件引用表 | 每 access type 独立 refcount |

## 池化专用结构

| 类型 | 作用 | 生命周期/索引 |
|---|---|---|
| `CUsuballocatorRadixTree` | 按 descriptor 兼容性分组、按空闲区大小索引 | 挂在 `CUmemmgr->suballocatorRadixTrees`；manager 销毁时要求为空 |
| `CUsuballocatorRadixNode` | 描述 memblock 内一个 free region | `node.key=size`；同时嵌入 `CUmemregion` 链表，分配时摘除，free 时重建并合并 |
| `CUmemregion` | 标记 block 内区域是否已分配 | `CUmemblock.memRegions` 双向链表，覆盖 allocated/free 区间 |
| `CUmembins`/`CUmembin` | 旧的 bin 字段 | 仅见声明和测试计划，当前树未找到初始化/使用路径 |

## 实现关系

- `uvmInitAL` 按 zero-copy、Mac lite、UVM8/KdLite 条件填充后端函数表；`uvmMapSubmemblock` 和 `uvmUnmapSubmemblock` 在 manager lock 下转发到该表。
- `uvmRegisterMemobj` 先附着首个 stream、记录 first/last stream，再调用 AL 注册；失败时撤销附着，`registered` 只在后端成功后置位。
- `CUpeerMap` 以 peer memmgr 为键保存按 access type 分离的 refcount；总计数为零时移除 entry。

这些字段把“对象 backing”“统一 VA”“异步 stream 依赖”和“P2P 权限”连接起来。后端 fault/migration 与 RM 映射仍不是这些结构自身能证明的行为。
## 接口契约

- `memobjAlloc(memmgr, desc, size, out)`：调用者须保证 memdesc 合法；成功后由 out memobj 持有资源。
- `memobjFree(&obj)`：要求对象没有未完成依赖，调用者将指针交给释放函数；具体 refcount/异步追踪由对象实现。
- `memglobalsRegisterMemobj`/`Unregister`：全局/P2P 关系的登记边界，不能省略。
- `memobjGetByUnifiedPtr`：查找可能不依赖 current context；调用方仍须处理 manager 不存在和对象来源。

## 文件地图

- `src/api/apimem.c`：公开分配/free/host API 的参数与 ABI。
- `src/cui/cuimem.c`：默认 flags、内部 device/host 分配/free。
- `src/cui/memobj.c`：合法性、backing、子分配、共享实例。
- `src/cui/suballocator.c`、`src/cui/radix.c`：best-fit、split、free/coalesce 索引。
- `src/cui/memblock.c`：block 资源、`memRegions`、DMAL 释放和碎片率。
- `src/cui/memmgr.c`：manager、兼容 tree、查找、对象集合。
- `src/cui/heap.c`：VA reservation/lookup（不是物理显存池）。
- `src/cui/cuivamanager.c`：UVA 管理器。
- `src/cui/cuiuvm.c`、`cuiuvmdag.c`：UVM 运行机制；完整 fault 路径待补。

## 行级审计

- `[src/cui/memobj.c:82-110]` 子分配可行性条件。
- `[src/cui/memobj.c:113-163]` descriptor 兼容性比较。
- `[src/cui/memobj.c:210-280]` backing 复用与 HAL block size。
- `[src/cui/memobj.c:265-375]` noSuballoc 决策、新 block 扩容和尾部 free region。
- `[src/cui/suballocator.c:163-220,278-405]` best-fit、split、free/coalesce。
- `[src/cui/memblock.c:471-562,565-680,682-740,967-1015]` DMAL backing、释放和碎片度量。
- `[src/api/apimem.c:280-339]` pointer lookup、来源/base 检查和同步释放。
