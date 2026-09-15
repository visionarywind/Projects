# 推理方向知识地图

> 范围：SGLang 风格的大模型在线推理，从请求协议到模型执行、KV cache、调度、性能与生产运维。源码路径是证据入口，不代表本工作区执行过上游命令。

| ID | 知识域 | 核心问题 | 重点级别 | 覆盖难度 |
|---|---|---|---|---|
| I01 | 启动与配置投影 | CLI/环境如何成为一致的运行时配置？ | 核心 | B/I/A/E |
| I02 | 协议、tokenizer 与请求状态 | 外部请求如何安全地映射为可终止 ReqState？ | 核心 | B/I/A/E |
| I03 | IPC 与进程边界 | 消息、错误、背压和关闭如何跨边界一致？ | 核心 | B/I/A/E |
| I04 | scheduler 与 continuous batching | admission、预算和公平性如何共同决定 batch？ | 核心 | B/I/A/E |
| I05 | Transformer 推理计算图 | attention、MLP、norm、RoPE、mask 如何执行？ | 核心 | B/I/A/E |
| I06 | prefill/decode 与 ForwardBatch | 两种阶段如何共享执行接口又保持 shape 正确？ | 核心 | B/I/A/E |
| I07 | attention backend 与 CUDA Graph | kernel 选择、图捕获和 eager fallback 如何切换？ | 核心 | I/A/E |
| I08 | KV pool、paged attention 与 radix cache | token/page/block ownership、命中和淘汰如何保持一致？ | 核心 | B/I/A/E |
| I09 | sampling、grammar 与停止 | logits 变换、约束和终态如何组合？ | 核心 | B/I/A/E |
| I10 | TP/PP 与分布式执行 | rank、shard、collective 和 ready 如何协同？ | 核心 | B/I/A/E |
| I11 | speculative decoding | draft、verify、接受率和回退如何保持语义等价？ | 重要 | I/A/E |
| I12 | 量化与低精度 KV | 权重、激活和 KV 量化的误差/容量/性能如何权衡？ | 重要 | I/A/E |
| I13 | disaggregation 与 KV transfer | prefill/decode 分离的传输、流控和恢复如何设计？ | 重要 | A/E |
| I14 | LoRA、适配器与多模态 | adapter/cache key、媒体 placeholder 和 batching 如何隔离？ | 重要 | I/A/E |
| I15 | 长上下文与容量 | context window、分页、eviction 和尾延迟如何规划？ | 核心 | I/A/E |
| I16 | 观测、测试与故障 | TTFT/TPOT、oracle、故障注入和诊断如何闭环？ | 核心 | B/I/A/E |
| I17 | 部署、扩缩与优雅关闭 | canary、autoscaling、drain 和回滚如何不丢请求？ | 核心 | A/E |
| I18 | 安全与多租户 | 鉴权、限流、隔离、脱敏和 DoS 防护如何落地？ | 核心 | I/A/E |

## 能力递进

- 入门（B）：建立 token、请求、batch、KV 和输出终态的心智模型。
- 中级（I）：解释组件调用链，给出输入/输出/状态和可执行单测 oracle。
- 高级（A）：量化显存、吞吐、尾延迟和故障恢复，做架构取舍。
- 专家（E）：用不变量和状态机评审多租户生产系统，制定演进、灰度和回滚判据。

## 证据约定

- **C（confirmed）**：知识库或目标版本源码文档明确记载。
- **I（inferred）**：基于多个源码入口推导，需回到目标版本核对。
- **U（unverified）**：依赖 GPU、模型、网络、RDMA、多进程或压测，当前未执行。
- 性能、容量、硬件支持和故障恢复结果没有运行日志时不得写成已验证事实。
