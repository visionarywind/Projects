# M04 测试矩阵

- 文档目的：把 C ABI 语义和核心 queue 测试连接起来。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/concurrency-queue HEAD 683b9e3（2026-09-15 只读确认）。
- 证据状态：测试注册名已确认，运行未验证。
- 最后更新：2026-09-10
- 前置阅读：[M04 README](README.md)
- 后续阅读：[M05 测试模块](../M05-verification/README.md)
## 结论摘要

本页聚焦 01-modules/M04-c-api/test-matrix.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


| 场景 | 入口/测试 | 当前状态 |
|---|---|---|
| nonblocking create/enqueue/dequeue | `c_api_enqueue`, `c_api_try_dequeue` | 源码已确认，未运行 |
| size query | C API size test/调用 | 需补充运行记录 |
| blocking enqueue/wait | C API blocking tests | 源码已确认，未运行 |
| repeated create/destroy | C API tests | 未验证 |
| null/invalid handles | 当前未据静态资料确认 | 未知 |
| value lifetime | API contract | 未知 |

所有未覆盖边界都应在新增行为前先定义为调用方责任、返回失败还是未定义行为。

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
