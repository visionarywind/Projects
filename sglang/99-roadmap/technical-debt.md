# 技术债务

- 文档目的：解释 99-roadmap/technical-debt.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/sglang HEAD 78be4b50af（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 99-roadmap/technical-debt.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


## 文档债务

- M01-M18 已建立模块文章，但 M06-M10 的专用实现仍以代表路径为主，尚未逐一覆盖所有 backend/算法变体；
- HTTP/OpenAI、multimodal、disaggregation、Rust gateway 已完成静态边界梳理，但尚未与普通 Engine 的真实运行证据达到同等深度；
- D01 已有静态端到端文章，但真实模型、GPU、权重和 stdout 未验证；
- 部分源码行号会随 checkout 变化，需要在版本更新后复核。

## 源码理解债务

- M08 request row、Radix insert、eviction、allocator/page 语义和 HiCache ownership 已完成代表路径补证；SWA、Mamba、HiSparse、unified sub-pool 及跨设备组合仍需逐一展开；
- M09 backend selection、metadata lifetime、capture 和 fallback 已完成代表路径补证；FlashInfer/MLA、Hybrid/Native 代表实现已记录，各稀疏、压缩和 linear attention kernel 的 layout 仍需逐一展开；
- M10 SamplingBatchInfo、grammar mask、sampler 分支、batch filter、grammar cache/future 和编译失败传播已完成代表路径补证；全部 fused sampler kernel、Outlines/LLGuidance 差异仍需逐一展开；
- M07 标准 group 创建、collective、attention TP/DP、logits all-to-all、MoE gather 和 alias 清理已补证；各专用 group 在所有 forward 路径的消费点仍需继续核对；
- M06 ModelConfig、loader 抽象、`get_model_loader` 优先级、PP/filter、权重映射和 barrier 已补证；ShardedState、Presharded、BitsAndBytes、GGUF、Remote/RemoteInstance、ModelOpt、Run:ai Streamer 的专用边界也已完成代表路径补证，但真实依赖、传输和权重加载仍未验证；全部专用 loader 优先级、复杂模型映射和量化后处理仍需逐一核对。

## 验证债务

完整 pre-commit、Mintlify、Rust workspace、GPU 模型服务、TP/PP、NCCL/RCCL/HCCL、CUDA Graph 和 registered suites 均未在本批执行。

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
