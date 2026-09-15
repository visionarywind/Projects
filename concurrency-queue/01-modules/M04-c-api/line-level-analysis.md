# M04 行级审计

- 文档目的：修改 C ABI 时的兼容性检查表。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/concurrency-queue HEAD 683b9e3（2026-09-15 只读确认）。
- 证据状态：当前 HEAD 静态整理。
- 最后更新：2026-09-10
- 前置阅读：[接口](interfaces.md)
- 后续阅读：[测试矩阵](test-matrix.md)
## 结论摘要

本页聚焦 01-modules/M04-c-api/line-level-analysis.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


| 检查点 | 证据 | 风险 |
|---|---|---|
| C 类型保持 opaque | `c_api/concurrentqueue.h` typedef | ABI 调用方依赖变化 |
| create 返回 handle | `c_api/concurrentqueue.cpp` | 空指针/异常语义 |
| destroy 使用对应类型 | 两个 `.cpp` destroy | 错误 handle 导致未定义行为 |
| enqueue/dequeue cast 正确 | C API 实现 | 错队列类型/崩溃 |
| blocking wait 只落到 BCQ | `blockingconcurrentqueue.cpp` | 丢失 signal/wait 协议 |
| value ownership 文档化 | API 当前需补充 | 内存泄漏或重复释放 |
| 异常不越过 C 边界 | 当前需测试/审查 | C 调用方无法处理 C++ exception |

## 必要验证

在 C++ 测试中覆盖 create/enqueue/dequeue/destroy、blocking wait；另需明确非法输入是否属于调用方责任，不应凭空添加防御语义。

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
