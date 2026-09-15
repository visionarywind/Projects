# 功能开发配方

- 文档目的：解释 99-roadmap/feature-development-recipes.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/sglang HEAD 78be4b50af（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 99-roadmap/feature-development-recipes.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


## 新增请求字段

1. 在输入结构和 normalize/batch slicing 中定义语义；
2. 检查 tokenized IPC 的序列化；
3. 在 scheduler handler 构造 `Req` 并保留生命周期；
4. 检查 `ScheduleBatch`、`ForwardBatch` 和 worker 是否需要消费；
5. 更新 output/result processor、abort 和 cleanup；
6. 添加正常、失败、batch 对齐测试。

## 修改 admission

联查 `get_next_batch_to_run`、`PrefillAdder`、prefix match、KV allocator、chunked prefill、priority/preemption、metrics 和 retraction。至少验证“能加入”“预算不足等待”“无法恢复 abort”三类。

## 修改 batch 字段

确认是否被 overlap snapshot、ForwardBatch alias、attention metadata 或 CUDA Graph buffer 持有。优先使用显式 snapshot/restore，不要在共享生命周期未确认时原地改字段。

## 修改 sampling

同时检查 SamplingParams verify、SamplingBatchInfo merge/filter、grammar mask、logprob、delay sampling、finish reason 和 M03 输出清理。

## 修改配置

先确定值在哪个 resolution 阶段固定，再追踪 publish 到哪个进程、哪个 worker 和哪个 device tensor。不要把派生配置放到热路径反复判断。

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
