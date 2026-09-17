# M03 Object Store 与 Object Manager

- 文档目的：登记 Ray 对象数据面、对象引用和存储边界。
- 版本：HEAD `cfe4725d23`；状态：目录与公共 API 已确认，内部算法待补。
- 前置：[M01](../M01-public-api/README.md)。后续：[M04](../M04-raylet-scheduling/README.md)。

## 结论摘要

`ray.get` 等待 ObjectRef 对应对象在本地可用，必要时从其他 object store 获取；`ray.put` 将值写入对象存储并返回引用，store 满会暴露 `ObjectStoreFullError`。[已确认：`python/ray/_private/worker.py:2881-3081`]

## 边界

- M01 负责引用 API 和错误表面。
- M03 负责对象存储/可用性/传输/spill 的底层数据面。
- M02 负责 worker 侧调用和生命周期；M05 提供控制元数据与 RPC。

## 代表流程

```text
ray.put(value) → worker.put_object → Object Manager/Store → ObjectRef
ray.get(ref) → worker.get_objects → local/pull → deserialize → value
```

## 深度审计

| 分析对象 | 入口落地 | 正常路径 | 分支 | 异常 | 清理 | 数据生命周期 | 执行上下文 | 行级证据 | Demo 映射 | 状态/缺口 |
|---|---|---|---|---|---|---|---|---|---|---|
| Object Store | Python API 已落地 | 部分完成 | 未完成 | store full/lost 已见 | 未完成 | 部分完成 | Driver/worker 边界已见 | Python 行号已有 | D01 get 已映射 | 部分完成 |

## 相关文档
[全局数据流](../../00-overview/global-data-flow.md) · [共享数据](../../90-cross-module/shared-data-and-types.md)

## 源码证据摘要
`python/ray/_private/worker.py:2996-3081`；`src/ray/object_manager/`。

## 未解决问题
spill、pull、ownership、reference counting、plasma/新对象路径及跨节点错误待逐符号确认。

## 下一步阅读建议
定位 `worker.put_object/get_objects` 实现和 object_manager RPC。
