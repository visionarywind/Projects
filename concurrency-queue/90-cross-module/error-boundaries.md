# 跨模块错误边界

- 文档目的：定位错误在核心、阻塞、ABI、测试和构建层之间如何传播。
- 证据状态：静态错误类别已确认；具体日志未验证。
- 最后更新：2026-09-10
- 前置阅读：[全局错误模型](../00-overview/global-error-model.md)
- 后续阅读：[变更影响图](change-impact-map.md)

## 边界表

| 边界 | 上游 | 下游 | 语义 |
|---|---|---|---|
| M01→caller | bool/exception | C++ 调用方 | 容量、空、对象异常 |
| M01→M02 | enqueue/dequeue bool/exception | semaphore pairing | 只有成功 enqueue 才 signal |
| M02→M03 | wait/waitMany/timeout | count/platform wait | permit 和元素数量必须协调 |
| M01/M02→M04 | C++ object methods | int ABI | 异常/非法指针边界需定义 |
| build→test | compiler/linker | process | 非零退出和日志 |
| benchmark→script | log | CSV | schema 需实际输出确认 |

## 典型故障路径

- `try_enqueue=false` 不能被 blocking wrapper 当作成功 signal。
- semaphore timeout 不等于 queue 永久为空，也不应自动销毁 queue。
- C API 返回 0 不能直接区分容量不足、空队列、非法 handle 或构造失败，除非 API 文档另行定义。
- unit test 进程通过不等于所有平台或 benchmark 通过。

## 调试证据

每个失败保留：源 commit、命令、平台、线程数、测试名、返回值/异常、stderr、是否可重现。没有这些信息时只标记未知。
