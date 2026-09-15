# D01 深度审计表

- 文档目的：检查主 Demo 是否覆盖构建、控制、数据、错误和清理。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/concurrency-queue HEAD 683b9e3（2026-09-15 只读确认）。
- 证据状态：静态覆盖确认；运行状态未验证。
- 最后更新：2026-09-10
- 前置阅读：[执行轨迹](execution-trace.md)
- 后续阅读：[D02](../D02-benchmark-run/README.md)
## 结论摘要

本页聚焦 80-demos/D01-unit-test-smoke/audit-table.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


| 维度 | 证据 | 状态 |
|---|---|---|
| 构建 | `build/makefile` C++11 + C API objects | 已确认 |
| 输入 | `--run`, `--iterations`, prompt flag | 已确认 |
| dispatch | test registry/main | 已确认 |
| 算法 | producer/block/head/tail | 已确认 |
| object lifecycle | Foo/Moveable/ThrowingMovable + tracking | 已确认 |
| blocking | blocking tests and semaphore | 已确认存在，覆盖需按测试名复核 |
| C ABI | c_api tests | 已确认 |
| cleanup | postTest and test teardown | 已确认 |
| actual run | command execution | 未验证 |

## 最小运行记录模板

记录 commit、命令、编译输出、测试名、迭代次数、退出码、耗时和完整错误；不要只记录“通过”。

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
