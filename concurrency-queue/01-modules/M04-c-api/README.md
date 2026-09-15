# M04 C ABI

- 文档目的：解释 C opaque handle 如何落到 C++ queue，并列出 ABI 边界的未决风险。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/concurrency-queue HEAD 683b9e3（2026-09-15 只读确认）。
- 证据状态：声明和实现基本路径已确认；非法输入和跨语言契约需验证。
- 最后更新：2026-09-10
- 前置阅读：[模块注册表](../module-registry.md)
- 后续阅读：[接口与调用链](interfaces.md)
## 结论摘要

本页聚焦 01-modules/M04-c-api/README.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。

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

## 文档元数据（规范补充）

- 文档目的：说明 `01-modules/M04-c-api/README.md` 的源码分析范围、结论和维护入口。
- 适用范围：当前项目对应模块/入口的静态源码与测试分析。
- 对应源码版本：以本项目 `00-overview/analysis-state.md` 或同页版本字段为准。
- 证据状态：静态源码证据；未执行的构建、测试、GPU、网络或多进程行为保持“未验证”。
- 最后更新：2026-09-15
- 前置阅读：本项目根 README 与 `00-overview/analysis-state.md`。
- 后续阅读：本模块/示例的实现、测试和风险页面。

## 深度审计

| 分析对象 | 入口落地 | 正常路径 | 分支 | 异常 | 清理 | 数据生命周期 | 执行上下文 | 行级证据 | Demo 映射 | 状态/缺口 |
|---|---|---|---|---|---|---|---|---|---|---|
| concurrency-queue/01-modules/M04-c-api/README.md | 已定位 | 已追踪代表路径 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 已标注 | 已引用或待补 | 已映射或无专用 Demo | 部分完成：动态构建、运行和硬件边界仍未验证 |

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
