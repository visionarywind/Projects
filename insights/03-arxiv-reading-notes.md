# arXiv PDF 原文解析：KV Cache 与池化

> 本文基于 arXiv 原始 PDF 的 `pdfinfo` 与 `pdftotext -layout` 解析结果编写。原始 PDF、抽取文本和解析元数据保存在 [`arxiv-pdf/`](arxiv-pdf/)，中文详译保存在 [`arxiv-pdf/zh/`](arxiv-pdf/zh/)。论文中的吞吐、延迟、成本、显存节省等数字仍然只是作者在其评测条件下的报告，不是本仓库复现实验；凡需要 GPU、模型、RDMA、多机或线上压测才能确认的结论继续标为 `[待验证]`。

## 0. PDF 解析状态

| arXiv ID | 论文 | PDF 状态 | 页数 | 本专题用途 |
|---|---|---:|---:|---|
| 2309.06180 | PagedAttention / vLLM | parsed | 16 | 分页 KV block、block table、copy-on-write 与 waste 分析 |
| 2405.04437 | vAttention | parsed | 18 | CUDA VMM、虚拟连续 KV、避免 paged kernel 代价 |
| 2411.01142 | NEO | parsed | 13 | CPU offload、GPU/CPU 非对称流水、load-aware scheduling |
| 2411.17089 | KVPR | parsed | 15 | CPU offload + partial KV recomputation、PCIe overlap |
| 2408.08147 | P/D-Serve | parsed | 15 | prefill/decode 分离、KVCache transfer、RoCE/D2D 边界 |
| 2506.15155 | eLLM | parsed | 8 | virtual tensor、elastic memory、CPU buffer、SLO-aware scheduling |
| 2508.13231 | Dynamic KV Cache Placement | parsed | 4 | HBM/off-package DRAM placement 数学建模 |
| 2510.09665 | LMCache | parsed | 19 | GPU/CPU/storage/network cache layer、prefix reuse、PD disaggregation |
| 2607.16892 | Robust KV Cache Management | parsed | 10 | 输出长度不确定下的 reservation、routing、prefix caching 和 SLO |

## 1. PagedAttention / vLLM

- 论文：[Efficient Memory Management for Large Language Model Serving with PagedAttention](https://arxiv.org/abs/2309.06180)
- PDF 材料：[`2309.06180-pagedattention-vllm.md`](arxiv-pdf/2309.06180-pagedattention-vllm.md)、[`txt`](arxiv-pdf/2309.06180-pagedattention-vllm.txt)

### PDF 原文解析

- [PDF 已确认] 论文把 LLM serving 的 KV cache 问题明确归因于动态序列长度导致的 fragmentation 和 redundant duplication，并提出 PagedAttention。PDF 原文说明 PagedAttention 借鉴 OS virtual memory with paging，将 request 的 KV cache 划分为 block，每个 block 保存固定数量 token 的 key/value。
- [PDF 已确认] PagedAttention 的关键抽象是 **logical KV blocks → block table → physical KV blocks**。block table entry 记录 logical block 对应的 physical block 以及已填充位置；逻辑与物理 block 分离，使 vLLM 可以按需增长请求 KV，而不要求物理连续。
- [PDF 已确认] 论文用“blocks as pages, tokens as bytes, requests as processes”解释 paging 类比：小 block 和按需分配缓解 internal fragmentation，不连续物理 block 避免 external fragmentation，并为 sharing 提供基础。
- [PDF 已确认] 对 parallel sampling / beam search 等共享 prefix 场景，论文使用 block-level sharing 和 copy-on-write；多个候选可共享 prefix block，写入分歧 block 时再复制。
- [PDF 已确认] 作者报告 vLLM 相对 FasterTransformer/Orca 的吞吐提升和 memory waste 降低；这些数字来自论文实验，不是本地复现。

### 与 KV pool 的关系

PagedAttention 是“固定物理 block pool + 动态逻辑 block table”的代表方案。它不等于每请求静态预留整段 KV，也不等于每步向 CUDA malloc；核心是用应用层 block allocator 处理 KV ownership、sharing 和 fragmentation。

### 边界

- [待验证] 当前仓库未复现论文中的 throughput、memory waste 或 block size sweep。
- [推断] Page/block size 仍需在 page 内浪费、kernel 访问开销、prefix sharing 粒度和 transfer 粒度之间取舍。

## 2. vAttention

- 论文：[vAttention: Dynamic Memory Management for Serving LLMs without PagedAttention](https://arxiv.org/abs/2405.04437)
- PDF 材料：[`2405.04437-vattention.md`](arxiv-pdf/2405.04437-vattention.md)、[`txt`](arxiv-pdf/2405.04437-vattention.txt)

### PDF 原文解析

- [PDF 已确认] 论文的出发点是：PagedAttention 通过 non-contiguous virtual layout 解决物理碎片，但会引入 attention kernel 适配和性能代价。PDF 中明确提到 vLLM 的 PagedAttention kernel 与 FlashAttention-2 等连续布局 kernel 的性能差异，并指出 block size 会影响 kernel execution time。
- [PDF 已确认] vAttention 使用 CUDA virtual memory management API，将虚拟地址空间和物理 GPU memory backing 解耦，使 KV cache 在虚拟地址上保持连续，同时让物理 memory 在运行时按需分配。
- [PDF 已确认] PDF 中的 observation 之一是 KV cache 单 token 的 allocation bandwidth 要求不高，因此可把 physical memory allocation 推迟到 runtime；设计目标是在不碎片化物理显存的前提下保持 virtual contiguity。
- [PDF 已确认] 论文报告 vAttention 在特定 workload 下的吞吐收益，并强调它复用/保留更接近连续布局的 attention kernel 路线。

### 与 KV pool 的关系

vAttention 把“动态池”问题下移到 CUDA VMM：请求侧看到连续 KV layout，系统侧动态 map/unmap physical memory。它适合希望保持连续 KV view、降低 paged kernel 改造成本的系统。

### 边界

- [重要边界] 虚拟连续不自动解决 prefix eviction、跨节点 transfer、输出长度 reservation 或多租户 quota。
- [待验证] CUDA VMM 的 page granularity、map/unmap 开销和驱动/硬件兼容性必须在目标平台验证。

## 3. NEO CPU Offloading

- 论文：[NEO: Saving GPU Memory Crisis with CPU Offloading for Online LLM Inference](https://arxiv.org/abs/2411.01142)
- PDF 材料：[`2411.01142-neo-cpu-offloading.md`](arxiv-pdf/2411.01142-neo-cpu-offloading.md)、[`txt`](arxiv-pdf/2411.01142-neo-cpu-offloading.txt)

### PDF 原文解析

- [PDF 已确认] NEO 的核心目标是通过把部分 decoding attention computation 和 KV cache 从 GPU offload 到本机 host CPU，扩大有效 GPU batch size，提升 online inference throughput。
- [PDF 已确认] 论文提出 asymmetric GPU-CPU pipelining：每个 iteration 中，两个非对称 sub-batch 并行执行，其中一部分请求的 attention computation 和 KV cache 被放到 CPU 路径，另一部分仍走 GPU 路径。
- [PDF 已确认] NEO 还使用 load-aware scheduling 在 iteration 之间平衡 GPU 与 CPU 负载，目标是同时利用 GPU compute/GPU memory 和 CPU compute/host memory。
- [PDF 已确认] 论文评测覆盖代码生成、摘要等 workload，以及 T4、A10G、H100 和 7B/8B/70B 模型；报告不同硬件上的收益差异明显。

### 与 KV pool 的关系

NEO 不是单纯扩大 GPU KV pool，而是把 KV capacity 问题转化为 GPU/CPU 协同调度问题。它要求 offload 粒度、CPU attention 路径、prefetch/restore 时序和 batch scheduler 联动。

### 边界

- [待验证] 当前仓库未验证 NEO 在 SGLang、当前机器或真实模型上的收益。
- [重要边界] CPU offload 可能缓解 HBM 容量，但会引入 PCIe/NVLink/CPU 争用和 P99 tail 风险。

## 4. KVPR：I/O-aware partial recomputation

- 论文：[Efficient LLM Inference with I/O-Aware Partial KV Cache Recomputation](https://arxiv.org/abs/2411.17089)
- PDF 材料：[`2411.17089-kvpr-partial-recompute.md`](arxiv-pdf/2411.17089-kvpr-partial-recompute.md)、[`txt`](arxiv-pdf/2411.17089-kvpr-partial-recompute.txt)

### PDF 原文解析

- [PDF 已确认] KVPR 的问题定义是：CPU KV offload 缓解 GPU memory pressure，但瓶颈会转移到 CPU-GPU PCIe 带宽，导致恢复 KV cache 时 GPU 空转。
- [PDF 已确认] KVPR 提出 I/O-aware KV cache partial recomputation：CPU 先传输一部分 activation/KV 相关数据，GPU 可以基于已到达的部分开始 recompute，而不是等待完整 KV restore。
- [PDF 已确认] 论文将 partial recomputation 与 offload 结合，核心是用额外 GPU 计算覆盖 I/O 等待，让 PCIe transfer 与 GPU computation overlap。
- [PDF 已确认] 作者报告 lower latency 和 higher throughput；这些数字依赖具体 PCIe、GPU、模型和 workload。

### 与 KV pool 的关系

KVPR 说明“offload pool”不能只定义存放位置，还要定义 restore/recompute 的执行计划。某些情况下，重算一部分 KV/activation 比完整恢复更优。

### 边界

- [推断] KVPR 更适合 transfer 是主瓶颈且 recomputation 成本可控的场景。
- [待验证] 本仓库未测量 PCIe 带宽、GPU idle、recompute overhead 或 P99 TPOT。

## 5. P/D-Serve

- 论文：[P/D-Serve: Serving Disaggregated Large Language Model at Scale](https://arxiv.org/abs/2408.08147)
- PDF 材料：[`2408.08147-pd-serve.md`](arxiv-pdf/2408.08147-pd-serve.md)、[`txt`](arxiv-pdf/2408.08147-pd-serve.txt)

### PDF 原文解析

- [PDF 已确认] P/D-Serve 面向大规模 disaggregated LLM serving，将 prefill phase 和 decoding phase 部署在不同实例，并使用不同 batch settings。
- [PDF 已确认] PDF 明确指出 prefill 产生的 intermediate data 即 KVCache 需要在 P/D 之间传输；在 tens of thousands of xPU devices（GPU 或 NPU）规模下，可靠性能面临挑战。
- [PDF 已确认] 论文列出三个关键挑战：请求/prefix 多样性导致简单 mixed pool 不足；workload 估计不准会造成 P/D 比例和迁移问题；cluster-level RDMA 上 block-fixed D2D KVCache transfer 难以达到理想利用率。
- [PDF 已确认] P/D-Serve 关注动态调整 P/D 资源比例、组织请求相似性、优化 D2D/RDMA KVCache transfer；作者报告 TTFT、throughput 和 transfer time 改善。

### 与 KV pool 的关系

P/D disaggregation 把 KV ownership 从单进程 allocator 扩展为跨实例协议：prefill owner 产生 KV，decode owner 恢复/消费 KV，transfer layer 需要 idempotent、backpressure 和错误恢复。

### 边界

- [重要边界] P/D-Serve 的“pool management / resource orchestration”不能简单等同于一个全局共享内存池。
- [待验证] 本仓库未运行多进程、多设备、RoCE/RDMA 或 D2D transfer 实验。

## 6. eLLM

- 论文：[eLLM: Elastic Memory Management Framework for Efficient LLM Serving](https://arxiv.org/abs/2506.15155)
- PDF 材料：[`2506.15155-ellm-elastic-memory.md`](arxiv-pdf/2506.15155-ellm-elastic-memory.md)、[`txt`](arxiv-pdf/2506.15155-ellm-elastic-memory.txt)

### PDF 原文解析

- [PDF 已确认] eLLM 把 LLM memory usage 分为相对固定的 model weights，以及动态且不可预测的 activations 和 KV caches。
- [PDF 已确认] eLLM 的核心组件包括 Virtual Tensor Abstraction：解耦 tensor virtual address space 与 physical GPU memory，形成统一且灵活的 memory pool。
- [PDF 已确认] Elastic Memory Mechanism 动态执行 memory allocation/deflation，并利用 CPU memory 作为 extensible buffer。
- [PDF 已确认] Lightweight Scheduling Strategy 使用 SLO-aware policy 优化 memory utilization，同时控制服务质量。作者报告最高 2.32x throughput 等收益。

### 与 KV pool 的关系

eLLM 是比 KV allocator 更宽的 elastic memory framework：它同时面对 activation、KV cache 和 CPU overflow/buffer。它强调统一抽象和调度，而不是只在 KV page allocator 内部做优化。

### 边界

- [重要边界] PDF 摘要和抽取文本没有给出所有阈值、迁移协议和完整一致性证明；实现细节需要进一步读源码或 artifact。
- [待验证] 当前仓库没有复现 eLLM 的 throughput 或 128K/batch-size 结果。

## 7. Dynamic KV Cache Placement

- 论文：[Accelerating LLM Inference via Dynamic KV Cache Placement in Heterogeneous Memory System](https://arxiv.org/abs/2508.13231)
- PDF 材料：[`2508.13231-dynamic-kv-placement.md`](arxiv-pdf/2508.13231-dynamic-kv-placement.md)、[`txt`](arxiv-pdf/2508.13231-dynamic-kv-placement.txt)

### PDF 原文解析

- [PDF 已确认] 论文指出 LLM inference 越来越受 memory bandwidth 限制，频繁访问 KV cache 主导数据移动。
- [PDF 已确认] 背景硬件是 heterogeneous memory system：HBM 与 high-speed off-package DRAM 通过 NVLink、LPDDR5X 等技术形成可用层级。
- [PDF 已确认] 论文研究在 HBM 和 off-package DRAM 间动态放置 KV cache，目标是在容量约束下最大化有效带宽/降低 latency。
- [PDF 已确认] PDF 提供 inference latency 的数学建模，并宣称这是 dynamic KV cache scheduling 的 formal treatment；其重点更偏理论上界和 placement formulation。

### 与 KV pool 的关系

这一路线把 KV pool 的“free slot/page”扩展为“slot/page 在哪个 memory tier”。allocator 需要结合 token/head/layer 的访问重要性、带宽和迁移代价做 placement。

### 边界

- [重要边界] 当前解析未确认可直接落地的 runtime allocator、迁移阈值或端到端系统实现。
- [待验证] 异构 HBM/DRAM bandwidth 和 placement 收益必须在目标硬件上验证。

## 8. LMCache

- 论文：[LMCache: An Efficient KV Cache Layer for Enterprise-Scale LLM Inference](https://arxiv.org/abs/2510.09665)
- PDF 材料：[`2510.09665-lmcache.md`](arxiv-pdf/2510.09665-lmcache.md)、[`txt`](arxiv-pdf/2510.09665-lmcache.txt)

### PDF 原文解析

- [PDF 已确认] LMCache 定位为 KV cache layer，用于把传统上位于 GPU memory 的 KV cache 移出 GPU，以支持跨 query 和 inference engine 的 cache reuse。
- [PDF 已确认] PDF 描述 LMCache 提供 lookup、cleanup、movement、compression 等接口，在 GPU、CPU、storage 和 network layers 之间进行 cache orchestration。
- [PDF 已确认] LMCache 同时支持 cache offloading（prefix reuse across queries）和 prefill-decode disaggregation / cross-engine transfer。
- [PDF 已确认] 作者报告 LMCache 与 vLLM 组合可带来最高 15x throughput improvement，并给出企业 workload 中 reuse/prefix truncation 的观察。

### 与 KV pool 的关系

LMCache 更接近“跨引擎、多层级 KV cache 管理层”，不是单 GPU 内部 allocator。它要求 cache key、序列化/压缩、跨进程/跨节点传输、清理策略和 serving engine 接口协同。

### 边界

- [待验证] 当前仓库没有验证 LMCache 在当前 SGLang checkout、当前硬件或本地 workload 上的表现。
- [重要边界] prefix reuse 的收益依赖真实流量重复度、上下文截断策略和跨请求 cache 命中率。

## 9. 输出长度不确定性下的 robust reservation

- 论文：[Robust KV Cache Management for LLM Serving under Output Token Length Uncertainty](https://arxiv.org/abs/2607.16892)
- PDF 材料：[`2607.16892-robust-kv-management.md`](arxiv-pdf/2607.16892-robust-kv-management.md)、[`txt`](arxiv-pdf/2607.16892-robust-kv-management.txt)

### PDF 原文解析

- [PDF 已确认] 论文的问题定义是：request arrival 时必须为 KV cache 做 reservation，但 output token length 直到 generation 结束才知道。
- [PDF 已确认] Under-reservation 会触发 preemption，迫使请求终止并重算，带来显著 overhead；over-reservation 会浪费 memory，降低 throughput。Reservation 决策直接影响吞吐和服务质量。
- [PDF 已确认] 论文提出的 robust optimization 同时考虑 GPU parallelism configuration、per-request-class KV cache reservation、heterogeneous serving group routing、shared prompt prefix caching 和 latency SLO。
- [PDF 已确认] 论文强调 workload 分布具有 non-stationarity，不同 task 的输出长度差异很大，因此单一 global quantile reservation 不能同时适配所有请求类别。

### 与 KV pool 的关系

这篇论文说明 KV pool 的容量管理不能只看 allocator free page 数；admission、routing、parallelism、prefix caching 和 reservation policy 都会改变实际可服务请求数和 SLO 风险。

### 边界

- [待验证] 本仓库没有生产 trace、cost 模型或 SLO 复现实验。
- [推断] 面试/设计时应把 output length uncertainty 纳入容量预算，而不是把 `max_context_len` 当作唯一 reservation。

## 10. 横向对比结论

| 路线 | PDF 原文确认的主要机制 | 主要解决的问题 | 没有自动解决的问题 |
|---|---|---|---|
| PagedAttention | logical/physical KV block、block table、copy-on-write | 动态 KV 的碎片、复制和共享 | page 内碎片、kernel 适配、reservation 不确定性 |
| vAttention | CUDA VMM、虚拟连续 KV、物理按需 backing | 保持连续 layout 同时动态管理物理显存 | eviction、跨层迁移、输出长度预测 |
| NEO | CPU offload、GPU/CPU 非对称流水、load-aware scheduling | GPU HBM 容量不足和 batch size 受限 | CPU 争用、带宽、P99 tail |
| KVPR | CPU offload + partial recompute + I/O overlap | PCIe restore 阻塞和 GPU idle | 重算成本、窗口选择、硬件敏感性 |
| P/D-Serve | prefill/decode 分离、KVCache D2D/RDMA transfer | P/D 资源隔离与大规模部署 | transfer 失败恢复、跨租户配额、统一 pool 语义 |
| eLLM | virtual tensor、elastic memory、CPU extensible buffer | 动态 activation/KV 内存管理 | 具体阈值、迁移协议、平台差异 |
| Dynamic placement | HBM/off-package DRAM placement formulation | 异构内存容量/带宽协同 | runtime policy 和迁移实现 |
| LMCache | GPU/CPU/storage/network cache layer | 跨 query/engine prefix reuse 与 offload | 真实流量命中率、序列化/压缩成本 |
| Robust reservation | reservation/routing/prefix caching/SLO 联合优化 | 输出长度不确定下的容量与成本 | 需要 trace、SLO 和 cost model |

## 11. 证据纪律

- 不把论文 PDF 中的 “up to” 写成普遍收益。
- 不把论文设计目标写成当前 SGLang 已实现能力。
- 不把 PDF 解析成功写成本地 GPU 运行验证。
- 每个迁移、淘汰、异步传输、CUDA Graph 和性能结论都必须说明证据或标为 `[待验证]`。
