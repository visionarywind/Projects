# M01 行级审计表

- 文档目的：为修改核心协议提供最小审计清单。
- 证据状态：按当前 HEAD 行区间整理。
- 最后更新：2026-09-10
- 前置阅读：[调用链](call-chains.md)
- 后续阅读：[M05](../M05-verification/README.md)

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
