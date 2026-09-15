# M04 source map

- 文档目的：解释 01-modules/M04-memory-uvm/source-map.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M04-memory-uvm/source-map.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


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
