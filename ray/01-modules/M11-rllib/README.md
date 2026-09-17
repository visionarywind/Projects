# M11 RLlib

- 文档目的：登记RLlib的源码边界和后续深审入口。
- 对应源码版本：HEAD `cfe4725d23`（2026-09-17）。
- 证据状态：目录/构建边界已确认；实现、运行和测试未验证。
- 前置阅读：[模块注册表](../module-registry.md)。后续阅读：本模块后续实现文档。

## 结论摘要

RLlib位于 `rllib` 与 `python/ray/rllib`，主要职责是强化学习 Algorithm、EnvRunner、Learner、RLModule 与采样/训练数据流。它依赖或关联 M01、M08-M10；当前仅完成模块识别，不能把目录存在误写成实现深审完成。[已确认目录，行为待验证]

## 测试边界

代表验证范围：RLlib tests。本轮未执行任何测试。

## 深度审计

| 分析对象 | 入口落地 | 正常路径 | 分支 | 异常 | 清理 | 数据生命周期 | 执行上下文 | 行级证据 | Demo 映射 | 状态/缺口 |
|---|---|---|---|---|---|---|---|---|---|---|
| M11 | 目录已定位 | 未完成 | 未完成 | 未完成 | 未完成 | 未完成 | 未完成 | 部分 | 待选 | 部分完成：新旧 API stack、采样/学习并发、checkpoint 和多智能体数据待补 |

## 相关文档
[项目架构](../../00-overview/architecture.md) · [修改影响](../../90-cross-module/change-impact-map.md)

## 源码证据摘要
`rllib` 与 `python/ray/rllib` 及其 BUILD/tests。

## 未解决问题
新旧 API stack、采样/学习并发、checkpoint 和多智能体数据。

## 下一步阅读建议
先从包公共入口和 BUILD/test target 确认稳定边界，再追到真实状态变化和资源副作用。
