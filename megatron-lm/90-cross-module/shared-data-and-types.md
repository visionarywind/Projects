# 共享数据与类型

- 文档目的：解释 90-cross-module/shared-data-and-types.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/megatron-lm HEAD 3703d4e33（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 90-cross-module/shared-data-and-types.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


| 类型/数据 | 使用方 | 说明 |
|---|---|---|
| `TransformerConfig` | M01/M02/M03/M06 | 模型结构和并行/内核配置 |
| `ProcessGroupCollection` | M01/M02/M03/M06 | 显式 group 容器 |
| `ModuleSpec` | M01 | layer/block 规格 |
| batch dict | M04/M03/M01 | tokens、labels、mask、position、packed metadata |
| `PackedSeqParams` | M01/M04/M06 | packed/varlen attention metadata |
| model list | M03/M02/M05 | VP/PP chunk 集合 |
| sharded state dict | M01/M05 | checkpoint mapping |
| `SamplingParams`/request | M06 | 推理请求行为 |

共享类型的风险在于字段来源分散：部分来自 args/YAML，部分来自 dataset，部分来自 runtime PG。修改字段时需追踪生产方和所有消费方。

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
