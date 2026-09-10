# D01 深度审计表

- 文档目的：检查主 Demo 是否覆盖构建、控制、数据、错误和清理。
- 证据状态：静态覆盖确认；运行状态未验证。
- 最后更新：2026-09-10
- 前置阅读：[执行轨迹](execution-trace.md)
- 后续阅读：[D02](../D02-benchmark-run/README.md)

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
