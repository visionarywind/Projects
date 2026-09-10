# M04 接口与调用链

- 文档目的：逐个说明 C ABI 函数的输入、输出和落点。
- 证据状态：声明/实现静态确认。
- 最后更新：2026-09-10
- 前置阅读：[M04 README](README.md)
- 后续阅读：[审计](line-level-analysis.md)

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
