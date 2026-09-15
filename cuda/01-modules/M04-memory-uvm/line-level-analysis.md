# M04 行级分析

- `[src/api/apimem.c:52-118]`：分配 wrapper 的 output、TLS、descriptor、lock、register 顺序。
- `[src/api/apimem.c:231-276]`：free 的 unified/device 双查找和 API source 白名单。
- `[src/api/apimem.c:277-339]`：base pointer 检查、unregister、同步和 no-sync free。
- `[src/cui/memobj.c:82-110]`：固定地址/mapping/sharing 等条件禁止子分配。
- `[src/cui/memobj.c:210-280]`：generic block size 影响 `noSuballoc`。
- `[src/cui/memobj.c:265-375]`：已有 block 命中、新 block 扩容、page rounding 和尾部 free region。
- `[src/cui/suballocator.c:163-220]`：`radixTreeFindGEQ` best-fit 与按 alignment split。
- `[src/cui/suballocator.c:225-340]`：已有/new memblock 的 suballocation 状态转移。
- `[src/cui/suballocator.c:343-405]`：free node 重用、相邻空闲区 coalesce、最后一个 block node 移除。
- `[src/cui/memblock.c:471-562,565-680,682-740]`：UVA/DMAL 资源申请、映射、反向释放。
- `[src/cui/memblock.c:967-1015]`：best-fit fragmentation 指标。
- `[src/cui/memmgr.c:89-136,334-365]`：descriptor-compatible tree 选择和 manager teardown 空树断言。
- `[src/cui/cuiuvm.c:1252-1328]`：submemblock map/unmap 与 managed memobj register 入口。
- `[src/cui/cuiuvm.c:2313-2428]`：zero-copy/Mac/UVM8/KdLite AL 选择。
- `[src/cui/cuiuvm.c:3950-4235]`：Kd8 host-page split、non-managed heap 与 reflected/dynamic-parallelism mapping。
- `[src/cui/peermap.c:12-134]`：peer access 按类型引用计数和零计数移除。
