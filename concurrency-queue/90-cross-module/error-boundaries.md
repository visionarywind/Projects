# 跨模块错误边界

- 文档目的：定位错误在核心、阻塞、ABI、测试和构建层之间如何传播。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/concurrency-queue HEAD 683b9e3（2026-09-15 只读确认）。
- 证据状态：静态错误类别已确认；具体日志未验证。
- 最后更新：2026-09-10
- 前置阅读：[全局错误模型](../00-overview/global-error-model.md)
- 后续阅读：[变更影响图](change-impact-map.md)
## 结论摘要

本页聚焦 90-cross-module/error-boundaries.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。

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

## 相关文档
- [项目入口](../README.md)
- [分析状态](../00-overview/analysis-state.md)
- [源码证据索引](../00-overview/evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](../00-overview/analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
