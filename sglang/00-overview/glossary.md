# 术语表

- 文档目的：解释 00-overview/glossary.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/sglang HEAD 78be4b50af（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](analysis-state.md)。
## 结论摘要

本页聚焦 00-overview/glossary.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


| 术语 | 在本文中的含义 |
|---|---|
| SRT | SGLang Runtime，面向 autoregressive LLM/VLM 的主要 Python runtime |
| token / token id | tokenizer 产生的离散文本单元及其整数编号 |
| prefill | 对 prompt 的已有 token 做首次/扩展计算 |
| decode | 基于已有上下文逐步生成新 token |
| continuous batching | 每轮调度可加入、移除请求的持续批处理 |
| `Req` | Scheduler 侧请求对象；不等于 HTTP 请求或 `ReqState` |
| `ScheduleBatch` | Scheduler 计划执行的一批请求和 mode |
| `ForwardBatch` | ModelRunner 使用的设备侧输入和 metadata |
| KV cache | attention 历史 key/value 的缓存 |
| Radix Cache | 按 token 前缀组织和复用 KV 映射的树结构 |
| TP | Tensor Parallel，沿张量维度分片并通信 |
| PP | Pipeline Parallel，沿模型层分阶段 |
| DP | Data Parallel；attention/MoE 的 data parallel 语义需看具体 group |
| EP | Expert Parallel，MoE experts 的分布式布局 |
| CP / DCP | attention context parallel / decode context parallel |
| WORLD group | `torch.distributed` 基础进程组 |
| `ModelConfig` | worker 使用的模型架构、dtype、能力和上下文配置 |
| `LoadConfig` | loader 使用的格式、缓存、下载和分片配置 |
| weight loader | 参数或模块把 checkpoint tensor 写入实际参数的逻辑 |
| eager | 逐次执行的非 CUDA Graph 路径 |
| CUDA Graph | 捕获固定形状/缓冲区的设备执行图 |
| grammar constraint | 对下一 token 合法集合施加结构约束 |
| IPC | 进程间通信；SGLang 使用 ZMQ、pipe、shared memory 等机制 |
| startup ready | worker 初始化完成后通过启动通道报告的状态 |
| resolved config | resolution pipeline 后、业务真正消费的有效配置 |

术语若在不同模块有不同 rank/group 含义，以对应模块的源码证据为准。

## 相关文档
- [项目入口](../README.md)
- [分析状态](analysis-state.md)
- [源码证据索引](evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
