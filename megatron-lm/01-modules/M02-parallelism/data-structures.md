# M02 数据结构与生命周期

- 文档目的：解释 01-modules/M02-parallelism/data-structures.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/megatron-lm HEAD 3703d4e33（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M02-parallelism/data-structures.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


| 对象 | 创建 | 使用 | 释放/持有 |
|---|---|---|---|
| rank list | `RankGenerator` | `torch.distributed.new_group` | Python 临时列表；group 由 distributed 持有 |
| ProcessGroup | `create_group` | collective/P2P | `destroy_model_parallel` 清理引用 |
| global memory buffer | parallel state | 通信/临时 tensor | parallel-state destroy 时释放 |
| microbatch state | schedule | 输入、输出、梯度栈 | 每次 schedule 调用结束 |
| activation tensor | 前一 stage/model forward | 下一 stage/backward | 发送后可伪释放 output storage |
| tensor shapes | `get_tensor_shapes` | P2P recv/send 分配 | 单次 schedule 使用 |

重要所有权边界：模型产生 activation，schedule 管理跨 stage 传递和保存；optimizer 不应直接依赖 schedule 内部的 microbatch 栈。

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
