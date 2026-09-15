# M01 行级审计表

- 文档目的：为修改核心协议提供最小审计清单。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/concurrency-queue HEAD 683b9e3（2026-09-15 只读确认）。
- 证据状态：按当前 HEAD 行区间整理。
- 最后更新：2026-09-10
- 前置阅读：[调用链](call-chains.md)
- 后续阅读：[M05](../M05-verification/README.md)
## 结论摘要

本页聚焦 01-modules/M01-core-queue/line-level-analysis.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


| 审计维度 | 检查点 | 代码证据 | 失败后果 |
|---|---|---|---|
| 发布顺序 | T 构造先于 tail release | `1877-1981`, `2515-2577` | consumer 看到未构造对象 |
| 领取 | head claim 与 optimistic count 配对 | `1983-2081`, `2579-2647` | 重复消费/漏消费 |
| 异常 | 构造和赋值异常有恢复/Guard | `1877-2081` | 活对象或 block 泄漏 |
| 空状态 | empty counter/flags 与析构配对 | `1588-1714` | block 过早复用 |
| 回收 | 全空后才进入 free list | `3068-3143` | use-after-free |
| hash | resize 发布和旧表链生命周期 | `3323-3509` | producer 查找错误 |
| token | token 析构 inactive，不直接 delete producer | `671-733` | 并发访问失效 |
| 销毁 | 先停止访问再遍历所有资源 | `875-919` | 数据竞争/未定义行为 |

## 变更验收

任何 M01 patch 至少需要：编译 unit tests；运行单线程 enqueue/dequeue、bulk、异常和 threaded tests；若改动 atomic 协议，增加 Relacy/CDSChecker 或等价模型检查；若改动 traits/block size，验证 `try_enqueue` 失败和回收路径。

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
