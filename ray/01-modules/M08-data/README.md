# M08 Ray Data

- 版本：HEAD `cfe4725d23`；Dataset/read API、logical operator、streaming executor 和安全边界已静态确认；动态执行与测试未验证。

## 结论摘要

Ray Data 位于 `python/ray/data`，Dataset API 先构造 lazy logical plan，read API 生成 Read operator，StreamingExecutor 再将其物化为 physical stages、blocks、tasks 和 backpressure 管道；Dataset API 调用不等于立即执行。[`dataset.py:270-8709`; `read_api.py:678-828`; `streaming_executor.py:107-740`]

## 代表链

```text
ray.data.read_datasource / range
→ Read logical operator
→ Dataset transformation (map/map_batches/filter/repartition)
→ StreamingExecutor.execute/run
→ physical tasks/operators
→ ObjectRef blocks + BlockMetadata
→ iterator/show/materialize
```

`Read/ReadFiles/ListFiles` 的逻辑输入和 datasource 边界位于 `read_operator.py:44-506`；executor 的 `execute`、`shutdown`、`run` 位于 `streaming_executor.py:185-461`。

## 数据、分支与安全

- Dataset、Block、BlockMetadata、ExecutionTask 和 block ObjectRefs 分离记录计划、内容和执行状态。
- map/map_batches、streaming、repartition、split、cache、parallelism、datasource retry 改变 physical plan 和资源压力。
- datasource/decode、task failure、OOM、backpressure 和 block loss 沿 executor 错误边界传播。
- 外部输入反序列化的 `pa.Table` 在 yield/materialize 前必须调用 `raise_on_pickle_object_columns(table)`，避免 pickle object column 形成远程代码执行风险。[`.claude/rules/security.md`]

## 清理

`StreamingExecutor.shutdown` 取消/结束 stages，释放 block references/cache；lazy Dataset plan 本身不证明已有 task。真实引用计数、spill 和 backpressure 时序未动态验证。

## 深度审计

|对象|入口落地|正常|分支|异常|清理|数据|上下文|证据|Demo|状态|
|---|---|---|---|---|---|---|---|---|---|---|
|M08|read/Dataset/operator/executor|logical→physical→blocks|map/stream/split/cache/retry|decode/task/OOM/object|executor shutdown/cache|Dataset/Block/Metadata/refs|driver+tasks/actors+Object Store|代表源码已列|D02|动态未验证|

## 相关文档

[模块注册表](../module-registry.md) · [实现](implementation.md) · [D02](../../80-demos/D02-data-tune-resource/README.md) · [性能路径](../../90-cross-module/performance-critical-paths.md)

## 源码证据摘要

`python/ray/data/dataset.py:270-8709`；`python/ray/data/read_api.py:678-828`；`python/ray/data/_internal/execution/streaming_executor.py:107-740`；`python/ray/data/_internal/logical/operators/read_operator.py:44-506`。

## 未解决问题

具体 physical operator、shuffle/backpressure、block ownership、datasource failure recovery 和运行性能仍需专项测试；静态代表链已覆盖主要边界。

## 下一步阅读建议

从 `read_datasource` 进入 Read operator，再读 executor `execute/run/shutdown`，最后结合 D02 观察 Tune 资源交互。
