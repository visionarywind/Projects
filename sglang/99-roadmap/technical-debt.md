# 技术债务

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
- M06 ModelConfig、loader 抽象、`get_model_loader` 优先级、PP/filter、权重映射和 barrier 已补证；全部专用 loader 优先级和量化后处理仍需逐一核对。

## 验证债务

完整 pre-commit、Mintlify、Rust workspace、GPU 模型服务、TP/PP、NCCL/RCCL/HCCL、CUDA Graph 和 registered suites 均未在本批执行。
