# 推理方向知识域详解

18 个知识域覆盖大模型在线推理服务全栈。每篇包含概念定义、机制拆解、关键参数/公式、常见误区与本仓库证据引用，并与 `01-question-bank/` 中对应题目互链。

| 域 | 范围 | 题目数 |
|---|---|---|
| [I01](I01.md) 启动、配置与协议 | CLI/环境→ServerArgs，进程/资源创建 | 5 |
| [I02](I02.md) 协议、tokenizer 与请求状态 | HTTP/OpenAI/SSE→ReqState 状态机 | 5 |
| [I03](I03.md) IPC 与进程边界 | 消息、错误、背压、关闭跨进程一致性 | 2 |
| [I04](I04.md) scheduler 与 continuous batching | admission、预算、retraction、公平性 | 6 |
| [I05](I05.md) Transformer 推理计算图 | attention/MLP/norm/RoPE/mask 执行 | 3 |
| [I06](I06.md) prefill/decode 与 ForwardBatch | 双阶段共享接口又保持 shape 正确 | 4 |
| [I07](I07.md) attention backend 与 CUDA Graph | kernel 选择、图捕获、eager fallback | 3 |
| [I08](I08.md) KV pool、paged attention 与 radix cache | token/page/block ownership、命中、淘汰 | 5 |
| [I09](I09.md) sampling、grammar 与停止 | logits 变换、约束采样、停止条件 | 4 |
| [I10](I10.md) TP/PP 与分布式执行 | rank 分片、collective、并行策略 | 4 |
| [I11](I11.md) speculative decoding | draft/verify/接受率、语义等价 | 3 |
| [I12](I12.md) 量化与低精度 KV | 权重/激活/KV 量化误差-容量-性能权衡 | 3 |
| [I13](I13.md) disaggregation 与 KV transfer | prefill/decode 分离、KV 传输流控恢复 | 2 |
| [I14](I14.md) LoRA、适配器与多模态 | adapter/cache key/batching 隔离 | 2 |
| [I15](I15.md) 长上下文与容量 | context window/paging/eviction/尾延迟 | 3 |
| [I16](I16.md) 观测、测试与故障 | TTFT/TPOT、oracle、故障注入、诊断 | 4 |
| [I17](I17.md) 部署、扩缩与优雅关闭 | canary、autoscaling、drain、回滚 | 4 |
| [I18](I18.md) 安全与多租户 | 鉴权、限流、隔离、脱敏、DoS | 2 |

总计 65 道题目。

## 阅读路径

建议按依赖顺序：I01→I02→I03→I04→I05→I06→I07→I08→I09→I10→I11→I12→I13→I14→I15→I16→I17→I18。前 6 域是请求接入与调度基座，I07–I12 是执行与显存核心，I13–I18 是生产化扩展。

## 证据说明

每篇引用的 `path:line` 来自本仓库 `sglang/` 源码分析文档与 `insights/` 论文笔记。薄弱域（I14、I18）标 `[U]`，不编造。
