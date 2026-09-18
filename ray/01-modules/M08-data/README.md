# M08 Ray Data

- 版本：HEAD `cfe4725d23`；静态代表链已补充；动态执行与测试仍未验证。

## 结论摘要

Ray Data 位于 `python/ray/data`，在 Core tasks/actors/objects 上实现分布式数据读取、转换与执行。Datasource、execution internals、Dataset API 和 tests 是主要边界；不能把 Dataset API 调用直接等同为一个远程任务。[已确认目录，内部流程待补]

## 安全约束

Datasource 从外部输入反序列化 `pa.Table` 时，在 yield/materialize 前必须调用 `raise_on_pickle_object_columns(table)`，否则 pickle object column 可形成远程代码执行风险。[已确认：仓库 security rule]

## 深度审计

| 对象 | 入口 | 正常 | 分支 | 异常 | 清理 | 数据 | 上下文 | 行证据 | Demo | 状态 |
|---|---|---|---|---|---|---|---|---|---|---|
| M08 | 包入口已定位 | 未完成 | 未完成 | 未完成 | 未完成 | 未完成 | task/actor 推断 | 部分 | D02 待选 | 静态深化完成，动态未验证 |

## 相关文档
[模块注册表](../module-registry.md) · [性能路径](../../90-cross-module/performance-critical-paths.md)

## 源码证据摘要
`python/ray/data/`、`python/ray/data/datasource/`、`python/ray/data/_internal/`、`python/ray/data/tests/`。

## 未解决问题
Dataset logical/physical plan、streaming executor、block ownership/backpressure/shuffle 和 datasource 安全链待分析。

## 下一步阅读建议
从 `python/ray/data/__init__.py` 公共 API 进入 Dataset，再追 execution internals。
