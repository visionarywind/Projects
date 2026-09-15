# M05 风险与技术债

- 文档目的：解释 01-modules/M05-stream-submit/risks-and-debt.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M05-stream-submit/risks-and-debt.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


| 风险 | 证据 | 说明 |
|---|---|---|
| 延迟回收 | `[src/cui/cuistream.c:1926-1954]` | marker 未完成时 detached 对象不能复用 |
| 锁序耦合 | `[src/api/apistream.c:83-109]` | context/pool/QMD semaphore 跨锁操作 |
| 多 backend | `[src/cui/channel.c:78-101]` | RM/MRM/WDDM/amodel 行为不同 |
| QMD 轮换容量 | `[src/cui/cuistream.c:1973-1986]` | 并发 launch 对槽位深度有隐含约束 |
| Marker 异构完成源 | tracking/QMD/CPU/UVM/cross-context entry 统一聚合 | 等待、flush、RM interrupt 和 firmware 完成值之间的时序难以单靠 host 源码验证 |
| GPFIFO/pushbuffer 游标耦合 | `gpuGet` 仅在 tracking semaphore 完成后推进 pushbuffer get | tracking 丢失或 DMAL 不推进会表现为长期占用/回收延迟 |

**深度状态**：stream/channel 创建、锁、QMD、pushbuffer/GPFIFO、marker 和 detach/reclaim 已完成 host 侧静态主线；DMAL doorbell/RM/WDDM 末端及 firmware 完成时序仍未完成。

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
