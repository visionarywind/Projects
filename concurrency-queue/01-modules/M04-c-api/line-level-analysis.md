# M04 行级审计

- 文档目的：修改 C ABI 时的兼容性检查表。
- 证据状态：当前 HEAD 静态整理。
- 最后更新：2026-09-10
- 前置阅读：[接口](interfaces.md)
- 后续阅读：[测试矩阵](test-matrix.md)

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
