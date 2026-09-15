# 推理方向资料与证据索引

## 主要证据入口

- `../../sglang/README.md`：服务入口、运行边界和阅读路线。
- `../../sglang/00-overview/`：架构、数据流、错误模型和性能路径。
- `../../sglang/01-modules/M02-http-api-protocol/`：HTTP/OpenAI、SSE、错误和 disconnect。
- `../../sglang/01-modules/M03-tokenizer-request-state/`：tokenizer、ReqState、状态终态。
- `../../sglang/01-modules/M04-scheduler-batching/`：admission、continuous batching、retraction。
- `../../sglang/01-modules/M05-model-execution/`：ForwardBatch、ModelRunner、执行契约。
- `../../sglang/01-modules/M07-分布式并行.md`：TP/PP、rank 和 collective。
- `../../sglang/01-modules/M08-kv-cache/`：request-to-token、page/block、ownership。
- `../../sglang/01-modules/M09-attention-cuda-graph/`：attention backend、CUDA Graph、fallback。
- `../../sglang/01-modules/M10-sampling-constraints/`：sampling、grammar、停止。
- `../../sglang/90-cross-module/`：资源、错误、性能与部署。
- `../../sglang/99-roadmap/`：测试、性能和上线方案。

## 证据状态

本题库不声明 GPU、模型、网络、RDMA、NCCL 或多进程实验已经执行。性能、容量、硬件兼容性、压测和故障恢复若没有运行记录，必须标为 `U / 待验证`；源码文档中的推断标 `I`，明确引用标 `C`。

## 外部专题

量化、speculative decoding、disaggregation、KV transfer、LoRA、长上下文、多模态和生产安全题目先给机制、假设、oracle 与实验方案；目标版本差异必须在题目中明确。

## KV Cache 池化专题

- `../../sglang/90-cross-module/pooling-and-resource-management.md`：SGLang 三层池、allocator、物理 KV/Mamba pool、Radix ownership、retraction、flush 和 CUDA Graph/custom pool 边界。
- `../../sglang/01-modules/M08-kv-cache/README.md`：request row、token/page allocator、prefix match、page alignment、finished/unfinished cache。
- 专题入口：`../../insights/README.md`。
- 外部论文索引：
  - [PagedAttention / vLLM](https://arxiv.org/abs/2309.06180)：分页式 KV 管理、共享与碎片控制。
  - [vAttention](https://arxiv.org/abs/2405.04437)：虚拟地址连续与物理动态映射。
  - [NEO](https://arxiv.org/abs/2411.01142)：CPU offload 与 GPU/CPU 协同。
  - [KVPR](https://arxiv.org/abs/2411.17089)：I/O-aware partial KV recomputation。
  - [P/D-Serve](https://arxiv.org/abs/2408.08147)：prefill/decode 分离与 KV transfer。
  - [eLLM](https://arxiv.org/abs/2506.15155)：elastic/virtual tensor memory management。
  - [Dynamic KV Cache Placement](https://arxiv.org/abs/2508.13231)：HBM/DRAM 动态放置。
  - [LMCache](https://arxiv.org/abs/2510.09665)：跨 GPU、CPU、storage、network 的 KV cache 层。
  - [Robust KV Cache Management](https://arxiv.org/abs/2607.16892)：输出长度不确定性下的 reservation、routing 和 SLO 权衡。

专题中的论文性能数字只代表作者在论文条件下的报告；当前环境没有复现 GPU、模型、网络、RDMA 或压测结果。
