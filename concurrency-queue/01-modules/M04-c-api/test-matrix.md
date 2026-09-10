# M04 测试矩阵

- 文档目的：把 C ABI 语义和核心 queue 测试连接起来。
- 证据状态：测试注册名已确认，运行未验证。
- 最后更新：2026-09-10
- 前置阅读：[M04 README](README.md)
- 后续阅读：[M05 测试模块](../M05-verification/README.md)

| 场景 | 入口/测试 | 当前状态 |
|---|---|---|
| nonblocking create/enqueue/dequeue | `c_api_enqueue`, `c_api_try_dequeue` | 源码已确认，未运行 |
| size query | C API size test/调用 | 需补充运行记录 |
| blocking enqueue/wait | C API blocking tests | 源码已确认，未运行 |
| repeated create/destroy | C API tests | 未验证 |
| null/invalid handles | 当前未据静态资料确认 | 未知 |
| value lifetime | API contract | 未知 |

所有未覆盖边界都应在新增行为前先定义为调用方责任、返回失败还是未定义行为。
