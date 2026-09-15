# arXiv 论文阅读笔记：KV Cache 与池化

> 本文只记录论文页面/摘要明确支持的内容。论文评测数字是作者报告，不是本仓库复现实验；未阅读全文或未能从页面直接确认的算法细节标为 `[待验证]`。

## 1. PagedAttention / vLLM

- 论文：[Efficient Memory Management for Large Language Model Serving with PagedAttention](https://arxiv.org/abs/2309.06180)
- [已确认] 论文提出受虚拟内存分页启发的 PagedAttention 和 vLLM，用于管理动态增长的 KV cache。
- [已确认] 目标是减少碎片、冗余复制和 KV cache 浪费，并支持请求内/请求间的灵活共享。
- [已确认] 页面摘要报告接近零 KV-cache waste，并在其评测中相对 FasterTransformer/Orca 达到 2–4x throughput；这些数字依赖论文实验条件。
- [待验证] 具体 block size、free-list 实现、每个版本的 kernel 细节和 eviction 细节不能仅由页面摘要确认。

## 2. vAttention

- 论文：[vAttention: Dynamic Memory Management for Serving LLMs without PagedAttention](https://arxiv.org/abs/2405.04437)
- [已确认] vAttention 用 CUDA virtual memory management API 分离虚拟地址分配和物理内存分配。
- [已确认] 目标是在物理 backing 动态管理的同时，让 KV cache 保持虚拟地址连续。
- [已确认] 论文将 PagedAttention 的非连续虚拟布局与 kernel 编程复杂度/性能开销作为对比背景，并报告最高 1.23x throughput（特定评测条件）。
- [推断] 该路线适合已有连续-layout attention kernel 的生态，但不能自动解决 prefix eviction、输出长度不确定性或跨节点 KV transfer。

## 3. NEO CPU Offloading

- 论文：[NEO: Saving GPU Memory Crisis with CPU Offloading for Online LLM Inference](https://arxiv.org/abs/2411.01142)
- [已确认] NEO 将部分 attention 计算和 KV-cache 状态放到 host CPU，减轻 GPU memory pressure，扩大可行 batch。
- [已确认] 论文使用 GPU/CPU 非对称流水和负载感知调度；页面报告 T4、A10G、H100 等条件下收益差异明显，最高报告值包括 T4 7.5x、A10G 26%、H100 14%，更强 CPU 的 A10G 条件报告 79.3%。
- [待验证] 摘要页面不支持把 NEO 的具体 eviction policy、每层放置粒度或任意硬件上的收益外推出来。

## 4. KVPR：I/O-aware partial recomputation

- 论文：[Efficient LLM Inference with I/O-Aware Partial KV Cache Recomputation](https://arxiv.org/abs/2411.17089)
- [已确认] KVPR 结合 CPU KV offload 和 GPU partial recomputation，重叠 PCIe 传输与 GPU 计算，用额外计算换取更少的 GPU 空转。
- [已确认] 页面报告最高 35.8% lower latency 和 46.2% higher throughput（特定实验条件）。
- [推断] 当传输带宽是主要瓶颈且部分重算成本可接受时，重算可能优于等待完整 KV restore；必须用 trace 和硬件测量验证。

## 5. P/D-Serve

- 论文：[P/D-Serve: Serving Disaggregated Large Language Model at Scale](https://arxiv.org/abs/2408.08147)
- [已确认] P/D 将 prefill 和 decode 服务分离，动态调整两类资源比例，并关注大规模设备部署和 RoCE 资源映射。
- [已确认] 论文讨论 block-oriented D2D KV movement 的效率问题，并报告优化后 transfer time 降低 46%、端到端 throughput 提升 60%、TTFT SLO 提升 42% 等评测结果；这些是论文声明。
- [重要边界] 页面并未明确声称存在一个通用共享内存池；“资源编排/transfer 优化”不能直接等同于统一 memory pool。

## 6. eLLM

- 论文：[eLLM: Elastic Memory Management Framework for Efficient LLM Serving](https://arxiv.org/abs/2506.15155)
- [已确认] eLLM 区分相对固定的模型权重和运行时变化的 activation/KV cache，提出 virtual tensor abstraction，将虚拟地址与物理 GPU 位置分离。
- [已确认] 论文描述 elastic memory management、CPU overflow/extension 和 SLO-aware scheduling；页面报告最高 2.32x decoding throughput、128K 输入支持 3x batch size 等数字。
- [重要边界] 摘要未给出固定池划分大小、精确迁移协议、触发阈值或完整 eviction 算法，不能据此断言某种静态/动态分区实现。

## 7. Dynamic KV Cache Placement

- 论文：[Accelerating LLM Inference via Dynamic KV Cache Placement in Heterogeneous Memory System](https://arxiv.org/abs/2508.13231)
- [已确认] 论文研究 HBM 与高速 off-package DRAM 等异构内存中的动态 KV placement，目标是结合带宽并满足容量约束。
- [已确认] 页面强调 memory bandwidth 和 KV 访问压力，并给出 placement 的数学建模/理论上界方向。
- [重要边界] 页面没有明确实现 allocator、migration algorithm、迁移粒度、延迟数字或具体 runtime policy；这些均为 `[待验证]`。

## 8. LMCache

- 论文：[LMCache: An Efficient KV Cache Layer for Enterprise-Scale LLM Inference](https://arxiv.org/abs/2510.09665)
- [已确认] LMCache 将 KV cache 移出 GPU 以支持 prefix reuse，并提供 GPU、CPU、storage、network 层之间的编排接口；页面点名 vLLM 和 SGLang 支持，并描述 PD disaggregation / cross-engine transfer。
- [已确认] 论文页面报告与 vLLM 组合最高 15x throughput improvement，以及工业 context truncation 可能使 prefix hit ratio 减半等观察。
- [重要边界] 这些是论文和部署报告；本仓库没有验证其在当前 SGLang checkout、硬件或工作负载上的表现。

## 9. 输出长度不确定性下的 robust reservation

- 论文：[Robust KV Cache Management for LLM Serving under Output Token Length Uncertainty](https://arxiv.org/abs/2607.16892)
- [已确认] 论文把输出长度未知下的 KV reservation 建模为：预留过少增加 preemption/重算风险，预留过多浪费显存并降低吞吐。
- [已确认] 页面描述联合考虑 GPU parallelism、reservation、routing、prefix caching 和 latency SLO 的 robust optimization，并报告最高 56% lower cost（相对 fixed-quantile baseline，特定 trace/条件）。
- [推断] 这说明池化设计不能只研究 allocator；reservation policy 是调度层和容量层的共同问题。

## 10. 对比结论

| 路线 | 主要解决的问题 | 没有自动解决的问题 |
|---|---|---|
| PagedAttention | 动态 KV 的碎片、复制和共享 | page 内碎片、kernel 适配、reservation 不确定性 |
| vAttention | 物理动态 + 虚拟连续布局 | 跨层级迁移、淘汰、输出长度预测 |
| CPU/offload/recompute | GPU 容量和传输瓶颈 | 带宽、恢复时序、CPU 争用、端到端 tail |
| P/D disaggregation | prefill/decode 资源隔离和 KV transfer | 传输失败恢复、跨租户配额、统一 pool 语义 |
| Elastic/unified memory | 动态 workload 下的共享/弹性内存 | 具体策略、阈值、可证明一致性和平台差异 |

## 11. 证据纪律

- 不把论文摘要中的“up to”写成普遍收益；
- 不把论文设计目标写成当前 SGLang 已实现能力；
- 不把本仓库已有源码静态分析写成 GPU 运行验证；
- 每个迁移、淘汰、异步传输和 CUDA Graph 结论都必须说明证据或 `[待验证]`。
