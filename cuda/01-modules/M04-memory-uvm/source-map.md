# M04 source map

| 文件 | 证据焦点 |
|---|---|
| `src/api/apimem.c` | device/host allocation/free wrapper、参数和同步 |
| `src/cui/memobj.c` | descriptor、backing、suballocation、兼容性 |
| `src/cui/suballocator.c`、`src/cui/radix.c` | best-fit、split、free/coalesce |
| `src/cui/memblock.c` | backing block、`memRegions`、DMAL 资源和碎片率 |
| `src/cui/memmgr.c` | context/device manager、descriptor-compatible trees、对象集合 |
| `src/cui/heap.c` | device/host VA reservation 和 lookup（虚拟地址，不是显存池） |
| `src/cui/cuimem.c` | host/device CUI 内存辅助 |
| `src/cui/cuiuvm.c`、`cuiuvmdag.c` | UVM 注册、DAG/fault/migration 边界 |
| `src/cui/cuivamanager.c`、`cuiuserdvamanager.c` | UVA/user VA 管理 |
| `src/cui/peermap.c` | P2P 映射关系 |

核心已读证据：[src/api/apimem.c:52-118,231-339,352-414]、[src/cui/memobj.c:82-110,113-163,210-375]、[src/cui/suballocator.c:163-220,278-405]、[src/cui/memblock.c:471-562,565-680,682-740]。UVM fault/migration、external sharing 和 DMAL 物理实现尚未完整追踪。
