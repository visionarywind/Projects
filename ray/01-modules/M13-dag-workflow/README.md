# M13 DAG、Workflow 与 Experimental Channels

- 文档目的：说明 DAGNode、CompiledDAG、channel reader/writer 与 workflow 边界。
- 对应源码版本：HEAD `cfe4725d23`（2026-09-17）。
- 证据状态：DAGNode、CompiledDAG、Monitor、channel interfaces 代表链已静态确认；运行和测试未验证。
- 前置阅读：[模块注册表](../module-registry.md)。后续阅读：[实现](implementation.md)。

## 结论摘要

`DAGNode` 保存输入节点和 method call；未编译执行沿 Ray handles 运行，`CompiledDAG` 将图编译为 executable tasks/channel graph，Monitor 管理运行时状态；experimental channels 负责 reader/writer、同步/异步和 buffer 生命周期。[`dag_node.py:33-724`; `compiled_dag_node.py:813-3285`; `experimental/channel/common.py:61-688`]

## 代表链

```text
DAGNode bind/input
→ execute 或 experimental_compile
→ CompiledTask/ExecutableTask
→ ChannelContext + Writer/Reader
→ downstream task
→ execute/execute_async output
→ monitor/channel teardown
```

## 分支、错误与清理

同步/异步 reader、batch、backpressure、channel closed、任务取消、编译约束和 teardown 改变路径。节点执行异常应沿 compiled graph/channel 传播；Monitor 终止运行并关闭 reader/writer。Workflow 在当前 checkout 作为相关边界记录，具体 API 需以存在的源文件和测试为准。

## 深度审计

|对象|入口落地|正常|分支|异常|清理|数据|上下文|证据|Demo|状态|
|---|---|---|---|---|---|---|---|---|---|---|
|M13|DAGNode/CompiledDAG/channels|bind→compile→channel→output|sync/async/batch/backpressure|node/channel/cancel|DAG/monitor/channel teardown|ExecutableTask/Context/reader/writer|driver+compiled workers|代表源码已列|无专门 Demo|动态未验证|

## 相关文档

[项目架构](../../00-overview/architecture.md) · [实现](implementation.md) · [风险](risks-and-debt.md)

## 源码证据摘要

`python/ray/dag/dag_node.py:33-724`；`compiled_dag_node.py:813-3285`；`python/ray/experimental/channel/common.py:61-688`。

## 未解决问题

channel ownership/backpressure、compiled DAG failure recovery、workflow 持久化和实验 API 兼容性仍需专项测试；静态代表路径已建立。

## 下一步阅读建议

先读 DAGNode.execute，再追 CompiledDAG.execute/execute_async、Monitor.run 和 channel reader/writer 生命周期。
