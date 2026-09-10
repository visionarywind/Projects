# 术语表

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
