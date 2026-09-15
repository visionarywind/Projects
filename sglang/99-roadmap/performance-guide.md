# 性能指南

- 文档目的：解释 99-roadmap/performance-guide.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/sglang HEAD 78be4b50af（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 99-roadmap/performance-guide.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


## 先分阶段测量

- 请求侧：tokenize、IPC、队列等待；
- prefill：输入 token、prefix hit、chunk 数和 KV 分配；
- decode：batch size、step 时间、retraction；
- worker：forward、collective、attention、sampling；
- 输出侧：copy-to-CPU、detokenize、stream/event 通知。

## 解释指标时的约束

吞吐和延迟受模型、硬件、并行拓扑、KV 容量、workload 和 backend 共同影响。静态源码只能指出关键路径，不能替代 benchmark。

## 调优顺序

1. 确认 workload 和资源是否稳定；
2. 看 prefix/KV 和 admission 是否成为瓶颈；
3. 比较 decode batch 与 prefill budget；
4. 再检查 overlap、CUDA Graph 和 attention backend；
5. 最后分析输出/IPC 和多卡通信。

**未验证**：本批未运行 benchmark、nsys、GPU profiler 或多卡性能测试。

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
