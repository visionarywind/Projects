# M04 source map

| 文件 | 证据焦点 |
|---|---|
| `src/api/apimem.c` | device/host allocation/free wrapper、参数和同步 |
| `src/cui/memobj.c` | descriptor、backing、suballocation、兼容性 |
| `src/cui/memblock.c` | backing block 管理（完整释放链仍待深读） |
| `src/cui/memmgr.c` | context/device memory manager（完整策略仍待深读） |
| `src/cui/cuimem.c` | host/device CUI 内存辅助 |
| `src/cui/cuiuvm.c`、`cuiuvmdag.c` | UVM 注册、DAG/fault/migration 边界 |
| `src/cui/cuivamanager.c`、`cuiuserdvamanager.c` | UVA/user VA 管理 |
| `src/cui/peermap.c` | P2P 映射关系 |

核心已读证据：[src/api/apimem.c:52-118,231-339,352-414]、[src/cui/memobj.c:82-110,113-163,210-280]。UVM fault/migration 和 external sharing 尚未完整追踪。
