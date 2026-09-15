# M03 源码地图与测试

- 文档目的：解释 01-modules/M03-training-runtime/source-map.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/megatron-lm HEAD 3703d4e33（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M03-training-runtime/source-map.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


| 源码 | 职责 |
|---|---|
| `megatron/training/initialize.py:48-176` | distributed/CUDA/seed 初始化 |
| `megatron/training/training.py:1530+` | `pretrain` 编排 |
| `training.py:2435+` | model list、DDP/FSDP 包装 |
| `training.py:2632+` | optimizer/scheduler setup |
| `training.py:3092+` | 单次 train step |
| `training.py:3396+` | training log |
| `training.py:4309+` | 主训练循环 |
| `training.py:5627+` | dataset/loader/iterator 构造 |
| `tests/unit_tests/training/` | training helper regression |
| `tests/unit_tests/test_utilities.py` 等 | 初始化/辅助路径 |

建议验证顺序：先用 CPU 可静态检查的 helper/unit test，再在匹配 CUDA/NCCL 容器运行 D01，最后验证 PP、FSDP、async checkpoint。

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
