# M03 Object Store 与 Object Manager

- 版本：HEAD `cfe4725d23`；已完成代表对象传输链静态分析；动态执行与测试未验证。

## 结论摘要

M03 管理对象内容在节点间的可用性，而不是只包装 `ray.put/get`。ObjectManager 启动 RPC，跟踪 pull/push 请求，选择 Plasma 或 filesystem 来源，分块发送/接收对象，并在节点删除、取消或释放时终止传输。[已确认：`src/ray/object_manager/object_manager.cc:66-718`]

## 代表链

```text
ray.put / task return
→ object ownership/reference
→ ObjectManager::Push 或 Pull
→ PushFromPlasma / PushFromFilesystem
→ SendObjectChunk / ReceiveObjectChunk
→ object available
→ ray.get / consumer
```

核心符号：构造/Stop/StartRpcService `66-178`；pull/cancel/failure `207-299`；push `369-547`；RPC receive/pull/free/node removal `602-718`。

## 内容、引用与所有权

- ObjectID/ObjectRef 是引用；对象 bytes 位于本地对象存储或 spill 文件。
- owner worker 保持逻辑所有权和失败通知；Object Manager 负责节点侧传输与可用性。
- `ray.put` 通过 `worker.put_object` 进入 CoreWorker/object store；`ray.get` 通过 `get_objects` 等待或取回，不代表调用方拥有底层存储生命周期。

## 分支与错误

- 对象在本地 Plasma 可读时走 Plasma source；已 spill 时走 filesystem source。
- pull 请求可合并、取消或因 node removal 失败；push 以 chunk 传输。
- 对象丢失、owner death、存储满和传输失败沿 ObjectRef/get 错误边界暴露。
- Python datasource 外部 Arrow 表必须在 yield/materialize 前调用 `raise_on_pickle_object_columns(table)`，这是安全边界而非可选优化。[`.claude/rules/security.md`]

## 清理

`CancelPull` 取消未完成请求，`FreeObjects` 释放可回收内容，`Stop` 关闭 RPC；节点移除 handler 处理与失联节点有关的传输。真实引用计数和 spill/restore 时序未动态验证。

## 深度审计

|对象|入口落地|正常|分支|异常|清理|数据|上下文|证据|Demo|状态|
|---|---|---|---|---|---|---|---|---|---|---|
|Object Manager|put/get、Push/Pull|source→chunk→available|get/spill/cancel/node removal|lost/full/transport failure|CancelPull/Free/Stop|ObjectID、buffer、chunk、spill URI|RPC/IO/Plasma callbacks|`object_manager.cc:66-718`|D01/D02|动态未验证|

## 相关文档

[实现](implementation.md) · [数据结构](data-structures.md) · [M02](../M02-core-worker/README.md) · [M04](../M04-raylet-scheduling/README.md)

## 未解决问题

跨节点对象传输、owner death、spill backpressure、存储满和压力性能需运行专项测试；静态代表路径已确认。

## 下一步阅读建议

先从 `Push`、`HandlePush`、`HandlePull` 读数据路径，再回到 Python `get/put` 的错误翻译。

## 源码证据摘要

`src/ray/object_manager/object_manager.cc:66-718`；`python/ray/_private/worker.py:2881-3081`。
