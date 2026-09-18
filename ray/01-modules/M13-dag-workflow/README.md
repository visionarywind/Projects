# M13 DAG、Workflow 与 Experimental Channels

- 文档目的：登记DAG、Workflow 与 Experimental Channels的源码边界和后续深审入口。
- 对应源码版本：HEAD `cfe4725d23`（2026-09-17）。
- 证据状态：目录/构建边界已确认；实现、运行和测试未验证。
- 前置阅读：[模块注册表](../module-registry.md)。后续阅读：本模块后续实现文档。

## 结论摘要

DAG、Workflow 与 Experimental Channels位于 `python/ray/dag`、`workflow`、`experimental`，主要职责是静态 DAG、持久 workflow 和 channel/collective 等实验能力。它依赖或关联 M01-M03；当前仅完成模块识别，不能把目录存在误写成实现深审完成。[已确认目录，行为待验证]

## 测试边界

代表验证范围：对应 tests。本轮未执行任何测试。

## 深度审计

| 分析对象 | 入口落地 | 正常路径 | 分支 | 异常 | 清理 | 数据生命周期 | 执行上下文 | 行级证据 | Demo 映射 | 状态/缺口 |
|---|---|---|---|---|---|---|---|---|---|---|
| M13 | 目录已定位 | 未完成 | 未完成 | 未完成 | 未完成 | 未完成 | 未完成 | 部分 | 待选 | 静态深化完成，动态未验证：compiled DAG、channel ownership、workflow recovery 和实验 API 稳定性待补 |

## 相关文档
[项目架构](../../00-overview/architecture.md) · [修改影响](../../90-cross-module/change-impact-map.md)

## 源码证据摘要
`python/ray/dag`、`workflow`、`experimental` 及其 BUILD/tests。

## 未解决问题
compiled DAG、channel ownership、workflow recovery 和实验 API 稳定性。

## 下一步阅读建议
先从包公共入口和 BUILD/test target 确认稳定边界，再追到真实状态变化和资源副作用。
