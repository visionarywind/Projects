# M02 设计与取舍

- 文档目的：解释 01-modules/M02-parallelism/design.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：`3703d4e33a3a2b2d11ebcc8e41f45af7ce7d1eda`
- 证据状态：静态源码分析
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M02-parallelism/design.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。

## 设计原则

1. **拓扑与计算解耦**：`parallel_state` 提供 group/accessor，模型层和 schedule 通过 group 使用通信，而不自行推导全局 rank。
2. **正交轴统一生成**：`RankGenerator` 将 TP、DP、PP、CP、EP 及 GTP 映射到 shape/stride，减少为每种组合手写 rank 列表。
3. **schedule 显式选择**：`get_forward_backward_func` 以 PP/VP 和多模块 collection 为输入，返回一个统一签名的函数。
4. **通信由调度拥有**：模型只输出 hidden states；stage 间发送/接收和 activation 生命周期由 pipeline schedule 管理。

## 取舍

- 全局 `parallel_state` 便于兼容旧调用方，但显式 `ProcessGroupCollection` 更利于多模块、多网格场景。
- warmup/1F1B/cooldown 能隐藏流水线气泡，但增加 tensor 保存、P2P shape 和调试复杂度。
- `deallocate_output_tensor` 通过替换已发送输出的 storage 降低显存，但依赖自定义 backward 和 viewless tensor 不变量。
- CP 不切分权重，因此需要在 backward 额外归约复制权重的梯度。

## 边界

M02 不决定 optimizer 是否 sharded；它只提供 optimizer 和 gradient finalize 所需的通信域。M02 也不把 NCCL 的实际算法或带宽结果当作源码可推导事实。

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
