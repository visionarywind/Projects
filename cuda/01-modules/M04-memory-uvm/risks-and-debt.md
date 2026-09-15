# M04 风险与技术债

- 文档目的：解释 01-modules/M04-memory-uvm/risks-and-debt.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M04-memory-uvm/risks-and-debt.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


| 项目 | 现状 | 影响 |
|---|---|---|
| 多层对象所有权 | `memobj`、`memblock`、manager 可分离 | 释放顺序和共享引用复杂 |
| 子分配策略 | 由 flags、size、HAL block size 和环境变量共同决定 | 同一请求可能走不同 backing 路径 |
| UVM 末端 | 依赖 `cuiuvm`、DAG 和外部内核接口 | 当前无法证明 fault 到 migration 的完整时序 |
| P2P/共享对象 | source/loopback/proxy 分支存在 | pointer lookup 和 unregister 容易取错对象 |
| 动态 UVM AL | `uvmInitAL` 按平台/全局选项选择 zero-copy、Mac、UVM8 或 KdLite | 仅读公共入口不能推断 fault/migration 和 channel 行为 |
| submemblock 特殊映射 | Kd8 处理 host-page split、non-managed heap 与 SKED/reflected memory | 页粒度、重叠检测和动态并行映射的组合分支难以仅靠单元测试覆盖 |
| P2P 引用与真实映射分离 | `CUpeerMap` 只维护 access refcount | bookkeeping 成功不等于 RM/设备 VA map 成功 |
| 池化机制与遗留 membins 混淆 | 当前分配走 `CUsuballocatorRadixTree`；`CUmembins` 只见声明/测试计划 | 错误地把未启用的 bin 策略写成现行实现，导致测试和性能结论失真 |
| 物理 backing 边界未闭合 | `memblockAlloc` 最终调用 `dmal.memblockAlloc`，backend 在当前快照外 | 无法仅凭 suballocator 证明页分配、回收、压缩或显存碎片行为 |
| 空闲区碎片 | best-fit 只在兼容 descriptor tree 内搜索，碎片率为 `(totalFree-largestFree)/totalFree` | 兼容性分裂或交错 free 可能导致新 block 分配和显存占用增加 |

**深度状态**：M04 已完成 API 分配/free、suballocator best-fit/split/coalesce、对象属性、UVM AL 选择、managed registration rollback、DAG/stream 状态和 P2P bookkeeping 的静态主线；待完成 fault/migration backend、IPC/external handle 端到端路径、各 DMAL memory backend 和运行时碎片实验。

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
