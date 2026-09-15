# 跨模块系统串联

- 文档目的：解释 90-cross-module/system-wiring.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：`3703d4e33a3a2b2d11ebcc8e41f45af7ce7d1eda`
- 证据状态：静态源码分析。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 90-cross-module/system-wiring.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


```text
entry/pretrain_gpt
  -> M03 initialize/pretrain/train
  -> M02 distributed/process groups/schedule
  -> M04 dataset/get_batch
  -> M01 model forward
  -> M05 optimizer/checkpoint
```

## 责任矩阵

| 责任 | 所属模块 | 说明 |
|---|---|---|
| 参数与作业编排 | M03 | 控制生命周期和训练循环 |
| 并行拓扑 | M02 | 提供 TP/PP/DP/CP/EP group |
| microbatch 调度 | M02/M03 | schedule 被 train_step 调用 |
| 模型计算 | M01 | embedding、decoder、output/loss |
| batch 生成 | M04 | dataset/sampler/get_batch |
| 参数更新 | M05 | optimizer 和 scheduler |
| 状态保存 | M05/M03 | checkpoint 命名、state dict、tracker |
| 推理服务 | M06 | 复用 M01/M02 并独立管理 request/KV |

跨模块边界的核心原则：M03 调度调用时机，M02 管通信和 microbatch，M01 只做 tensor 计算，M05 管训练状态持久化。

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
