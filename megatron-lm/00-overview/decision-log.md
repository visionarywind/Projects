# 设计决策记录

- 文档目的：记录源码可见的设计选择、取舍和冲突。
- 适用范围：当前目标提交。
- 对应源码版本：`8190837c2b6ce176a431bc2a6ffd3439507648a7`
- 证据状态：已确认选择；动机多为推断
- 最后更新：2026-09-10

| ID | 决策 | 证据 | 收益/代价 | 状态 |
|---|---|---|---|---|
| DEC-01 | 用 `ModuleSpec` 选择模型层实现 | [gpt_builders.py:31-54] | 可替换 backend；配置路径增多 | 当前 |
| DEC-02 | 保留 `parallel_state` 全局兼容层 | [parallel_state.py:28-165] | 兼容旧调用；隐式状态风险 | 当前/迁移中 |
| DEC-03 | schedule 根据 PP/VP 返回不同函数 | [schedules.py:157-168] | 调度算法隔离；shape 契约复杂 | 当前 |
| DEC-04 | checkpoint 使用 sharded state dict | [examples/run_simple_mcore_train_loop.py:183-216] | 适合大模型；布局迁移复杂 | 当前 |
| DEC-05 | optimizer backend 分层 fallback | [optimizer/__init__.py:13-35] | 可在缺 TE 时运行；性能不同 | 当前 |
| DEC-06 | 训练入口为参考应用而非 Core API | [README.md:15-19] | 快速实验；生产编排需外部框架 | 当前 |

## 冲突记录

README 的发行 badge 为 `0.19.0`，Git 描述为 `core_v0.15.0rc7-2490-g8190837c2-dirty`。记录为两个不同语义的版本字段，不将其强行统一。

## 相关文档

- [设计原则](design-principles.md)
- [技术债务](../99-roadmap/technical-debt.md)

## 源码证据摘要

见表格。

## 未解决问题

不能仅从静态代码确认每个设计决策的历史原因；若需历史背景应查 issue/commit。

## 下一步阅读建议

结合对应模块的 risks 文档。
