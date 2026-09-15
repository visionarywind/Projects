# M04 行级分析

- 文档目的：解释 01-modules/M04-memory-uvm/line-level-analysis.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M04-memory-uvm/line-level-analysis.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


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
