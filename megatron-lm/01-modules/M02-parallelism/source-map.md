# M02 源码地图与测试

- 文档目的：解释 01-modules/M02-parallelism/source-map.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/megatron-lm HEAD 3703d4e33（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M02-parallelism/source-map.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


| 源码 | 符号/职责 |
|---|---|
| `megatron/core/parallel_state.py:232-260` | `create_group`，封装 distributed group 创建 |
| `parallel_state.py:269-570` | rank group 正交生成和 `RankGenerator` |
| `parallel_state.py:600-约 1580` | TP/PP/DP/CP/EP/GTP group 初始化 |
| `parallel_state.py:2505+` | destroy/accessor 相关状态 |
| `pipeline_parallel/schedules.py:53-220` | schedule 选择、输出释放、backward helper |
| `schedules.py:723-928` | 无 pipeline |
| `schedules.py:1019+` | 交错 pipeline |
| `schedules.py:2147+` | 非交错 pipeline |
| `pipeline_parallel/p2p_communication.py` | stage 间通信 |
| `tests/unit_tests/test_parallel_state.py` | rank/group 初始化与销毁测试 |
| `tests/unit_tests/pipeline_parallel/` | schedule/P2P 回归测试 |

测试命令依赖上游锁定环境；本批只做文档静态分析，没有运行测试。

## 相关文档
- [项目入口](../../README.md)
- [分析状态](../../00-overview/analysis-state.md)
- [源码证据索引](../../00-overview/evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](../../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](../../00-overview/analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
