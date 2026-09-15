# M04 接口与调用链

- 文档目的：逐个说明 C ABI 函数的输入、输出和落点。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/concurrency-queue HEAD 683b9e3（2026-09-15 只读确认）。
- 证据状态：声明/实现静态确认。
- 最后更新：2026-09-10
- 前置阅读：[M04 README](README.md)
- 后续阅读：[审计](line-level-analysis.md)
## 结论摘要

本页聚焦 01-modules/M04-c-api/interfaces.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


| C 函数族 | C++ 落点 | 成功/失败 |
|---|---|---|
| `cq_create/destroy` | `new/delete ConcurrentQueue<void*>` | 1/0 或异常边界待确认 |
| `cq_enqueue` | `ConcurrentQueue<void*>::enqueue` | bool 转 1/0 |
| `cq_try_dequeue` | `ConcurrentQueue<void*>::try_dequeue` | bool 转 1/0，输出 value |
| `cq_size_approx` | `size_approx` | size 返回 |
| `bcq_create/destroy` | `new/delete BlockingConcurrentQueue<void*>` | 1/0 或异常边界待确认 |
| `bcq_enqueue` | blocking enqueue + signal | bool 转 1/0 |
| `bcq_wait_dequeue` | wait + core dequeue | 返回 bool |
| `bcq_try_dequeue` | nonblocking core dequeue | 返回 bool |

## 关键边界

`reinterpret_cast` 只是类型桥接，不执行输入验证、不转移 value 指针所有权，也不替调用方完成 queue 生命周期同步。C 调用者必须把成功返回的 output 指针解释为先前入队的 value，并自行定义 value 对象生命周期。

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
