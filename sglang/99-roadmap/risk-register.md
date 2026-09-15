# 风险登记

- 文档目的：解释 99-roadmap/risk-register.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/sglang HEAD 78be4b50af（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 99-roadmap/risk-register.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


| 风险 | 触发场景 | 防护 |
|---|---|---|
| IPC 字段错位 | 新字段未同步 batch slicing/serializer | 检查 M03/M04/M15 契约和 batch tests |
| state 泄漏 | dispatch 后只删本地 state | 区分 dispatch 前后 cleanup，检查 abort |
| KV 悬挂映射 | 释放 slot 未更新 row/radix lock | 联查 M08 allocator、prefix node 和 Req metadata |
| overlap race | 原地修改共享 batch/metadata | 检查 snapshot、stream/event 和 shared read ends |
| graph 错用 | 动态 shape 或 buffer 不满足 replay | 依赖 `can_run_graph`，验证 eager fallback |
| 输出错配 | filter/merge 后数组顺序改变 | 按 rid 和 batch index 做断言 |
| 配置漂移 | 派生值在不同进程重新计算 | resolve once 后 publish |
| 过度声称验证 | 未跑 GPU/模型却写成功 | 明确“未验证”并记录命令状态 |

该表是静态分析风险登记，不是生产安全评估。

## 相关文档
- [项目入口](../README.md)
- [分析状态](../00-overview/analysis-state.md)
- [源码证据索引](../00-overview/evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](../00-overview/analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
