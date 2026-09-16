# 字节 / 阿里 / 腾讯算法面试题库

本题库面向准备字节跳动、阿里巴巴、腾讯等国内大厂算法面试的学习者，目标是把算法题从“记答案”变成“能建模、能证明、能编码、能举例、能应对追问”。题库采用目录化 Markdown，默认规划入门、中等、高级、专家级各 100 题，共 400 题。

## 当前状态

- 目标规模：B001–B100、I001–I100、A001–A100、E001–E100，共 400 题。
- 当前批次：已一次性生成 B001–B100、I001–I100、A001–A100、E001–E100，共 400 道题。
- 实现语言：默认 C++17。
- 公司标签：`字节`、`阿里`、`腾讯` 表示公开经验和岗位能力侧重点的训练标签，不表示保密题库或官方原题。
- 证据状态：算法结论按公开经典资料与静态检查记录；未逐题在目标 OJ 执行的结果仍标 `[待验证]`。

## 导航

- [领域总览](00-overview/domain-overview.md)
- [知识地图](00-overview/knowledge-map.md)
- [能力地图](00-overview/competency-map.md)
- [覆盖矩阵](00-overview/coverage-matrix.md)
- [生成状态](00-overview/generation-state.md)
- [资料与证据](00-overview/sources-and-evidence.md)
- [术语表](00-overview/terminology.md)
- [入门题索引](01-question-bank/beginner/index.md)
- [中等级索引](01-question-bank/intermediate/index.md)
- [高级索引](01-question-bank/advanced/index.md)
- [专家级索引](01-question-bank/expert/index.md)
- [知识域索引](02-knowledge-domains/README.md)
- [字节路线](03-company-tracks/bytedance.md)、[阿里路线](03-company-tracks/alibaba.md)、[腾讯路线](03-company-tracks/tencent.md)、[通用核心](03-company-tracks/cross-company-core.md)
- [练习与模拟](04-practice/timed-mock-interviews.md)
- [质量审计](05-review/quality-audit.md)
- [30 天路线](99-roadmap/30-day-plan.md)、[90 天路线](99-roadmap/90-day-plan.md)

## 单题学习方法

1. 先读题目和约束，用自己的话复述输入、输出和边界。
2. 先写暴力解，再说明瓶颈。
3. 推导优化解的不变量或状态转移。
4. 手写 C++17 代码并跑题目给出的样例。
5. 用“易错点”和“追问”检查能否迁移到变体。

## 难度说明

| 难度 | 目标 | 典型能力 |
|---|---|---|
| 入门 B | 单一模式和基础数据结构 | 正确建模、写出无明显边界 bug 的代码 |
| 中等 I | 多约束或双模式组合 | 解释优化来源、复杂度和测试策略 |
| 高级 A | 高数据规模、在线或复杂状态 | 证明正确性、处理空间/时间权衡和工程边界 |
| 专家 E | 开放约束和方案决策 | 澄清假设、比较方案、定义验证与失败模式 |

## 续作规则

继续生成时先读取 `00-overview/generation-state.md` 和 `00-overview/coverage-matrix.md`，从“下一批起始题目 ID”续作；不得重复题目、重排 ID 或把未运行代码写成已验证。
