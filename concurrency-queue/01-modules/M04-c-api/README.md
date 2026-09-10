# M04 C ABI

- 文档目的：解释 C opaque handle 如何落到 C++ queue，并列出 ABI 边界的未决风险。
- 证据状态：声明和实现基本路径已确认；非法输入和跨语言契约需验证。
- 最后更新：2026-09-10
- 前置阅读：[模块注册表](../module-registry.md)
- 后续阅读：[接口与调用链](interfaces.md)

## 入口

`c_api/concurrentqueue.h` 声明 `MoodycamelCQHandle`、`MoodycamelBCQHandle` 和 `MoodycamelValue` 为 `void*`，导出 create/destroy/enqueue/try_dequeue/wait_dequeue 函数。两个 `.cpp` 文件分别实例化 `ConcurrentQueue<void*>` 和 `BlockingConcurrentQueue<void*>`。

## 真实路径

```text
moodycamel_cq_create
  -> new ConcurrentQueue<void*>
  -> opaque handle
moodycamel_cq_enqueue
  -> reinterpret_cast<MoodycamelCQPtr>(handle)
  -> ConcurrentQueue<void*>::enqueue
  -> M01 block/producer protocol
moodycamel_bcq_wait_dequeue
  -> reinterpret_cast<MoodycamelBCQPtr>(handle)
  -> BlockingConcurrentQueue<void*>::wait_dequeue
  -> M03 wait + M01 dequeue
```

## 所有权与错误

C API 实现把 queue 对象的创建/销毁交给 create/destroy；value 只是 `void*` 元素，静态代码不能证明其指向对象的释放责任。create 的 `new` 失败语义、空 handle、空输出指针和异常穿越 C 边界必须由 API 约定或运行测试确认，不能从函数名推断。

## 审计卡片

| 维度 | 状态 |
|---|---|
| ABI | C 导出函数 + opaque handle |
| 类型 | `void*` queue 元素 |
| 正常返回 | 1/0 整数 |
| 生命周期 | create → calls → destroy |
| 线程安全 | 底层 queue 契约适用；handle 生命周期由调用方负责 |
| 未知 | 非法指针、异常、跨编译器 ABI |

## 子页

- [interfaces](interfaces.md)
- [line-level-analysis](line-level-analysis.md)
- [test-matrix](test-matrix.md)
