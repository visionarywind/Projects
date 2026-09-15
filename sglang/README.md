# SGLang 源码级架构分析

这是一套面向初学者、以当前 checkout 源码为证据的 SGLang 项目深度理解知识库。它不只罗列目录，而是把“配置如何解析、进程如何启动、请求如何流动、模型如何加载、GPU 如何执行、结果如何返回”拆成可以回到源码验证的模块和调用链。

## 版本与证据约定

- 当前分析锚点见 [项目总览](00-overview/project-overview.md) 和 [分析状态](00-overview/analysis-state.md)。源码变化后，文章中的行号需要复核。
- 重要结论标记为“已确认”“推断”或“未知”。
- 源码引用使用 `[仓库相对路径:起始行-结束行]`；带 `...` 的旧索引范围不是精确行号。
- 未执行的 GPU、模型下载、多卡通信和端到端命令明确标记为“未验证”。
- 普通 HTTP LLM 是主线；Ray、diffusion、speculative、multimodal、disaggregation 等变体在对应模块或跨模块文章中单列。

## 推荐阅读顺序

### A. 总览层：先建立地图

1. [项目总览](00-overview/project-overview.md)：分析目标、版本锚点、仓库地图和缺口。
2. [总体架构](00-overview/architecture.md)：模块 ID、控制面/执行面和主要连接。
3. [设计原则](00-overview/design-principles.md)：配置投影、资源编排、snapshot 和证据规则。
4. [运行时模型](00-overview/runtime-model.md)：主进程、子进程、启动/稳态/退出生命周期。
5. [全局数据流](00-overview/global-data-flow.md)：启动方向和请求方向的数据、状态变化。
6. [依赖地图](00-overview/dependency-map.md)：模块依赖与动态调用边界。
7. [构建与部署](00-overview/build-and-deploy.md)：Python、Rust、入口、测试和部署分支。
8. [全局错误模型](00-overview/global-error-model.md)：输入、调度、设备、进程和外部服务错误边界。
9. [术语表](00-overview/glossary.md)：统一 `Req`、`ScheduleBatch`、TP、PP、KV 等术语。
10. [源码证据索引](00-overview/evidence-index.md)：按主题查找已经定位的源码。
11. [决策记录](00-overview/decision-log.md)：记录知识库的范围和组织决策。
12. [分析状态](00-overview/analysis-state.md)：查看已完成、未验证和下一批任务。

### B. 基础概念与已有请求主线

13. [整体架构（原文章）](00-overview/01-整体架构.md)：较早的叙述式入口，可与新总览互相参照。
14. [推理服务基础概念](01-concepts/01-推理服务基础概念.md)：请求、token、prefill、decode 和批处理。
15. [Transformer 与 KV Cache](01-concepts/02-Transformer与KV-Cache.md)：Transformer 计算、KV cache 和 Radix Cache 基础。
16. [启动链路](02-request-flow/01-启动链路.md)：从服务入口到 runtime 进程。
17. [请求进入与分发](02-request-flow/02-请求进入与分发.md)：HTTP/API、TokenizerManager 和请求状态。
18. [调度器与连续批处理](02-request-flow/03-调度器与连续批处理.md)：waiting/running batch、admission、抢占和调度策略。
19. [模型执行与输出](02-request-flow/04-模型执行与输出.md)：ForwardBatch、ModelRunner、采样和增量输出。
20. [配置解析与运行时上下文](02-request-flow/05-配置解析与运行时上下文.md)：raw input、resolution stash 和 runtime config bags。
21. [多进程与 IPC](02-request-flow/06-多进程与IPC.md)：Tokenizer、Scheduler、Detokenizer、ZMQ 和 startup pipe。

### C. 实现层：按模块 ID 深入源码

23. [M01 CLI 与服务启动](01-modules/M01-cli-service-startup/README.md)：延迟解析、backend registry、部署分支和进程树清理。
24. [M02 HTTP/API 与协议](01-modules/M02-http-api-protocol/README.md)：native/OpenAI 协议、请求转换、SSE、错误、disconnect 和 abort。
25. [M03 Tokenizer 与请求状态](01-modules/M03-tokenizer-request-state/README.md)：请求状态、tokenize、IPC dispatch、增量输出与清理。
26. [M04 Scheduler 与连续批处理](01-modules/M04-scheduler-batching/README.md)：waiting/running admission、PrefillAdder、ScheduleBatch、forward 和 retraction。
27. [M05 模型执行](01-modules/M05-model-execution/README.md)：ForwardBatch、TP worker、ModelRunner、CUDA Graph、sampling 与结果边界。
28. [M06 模型加载与权重](01-modules/M06-model-loading/README.md)：ModelConfig、loader、PP filter、weight mapping、量化和 barrier。
29. [M07 分布式并行](01-modules/M07-分布式并行.md)：WORLD、TP/PP、attention/MoE 拓扑、GroupCoordinator、collective 与清理。
30. [M08 KV Cache 与 Radix Cache](01-modules/M08-kv-cache/README.md)：request row、KV slot、prefix match、Radix 插入、回收与 retraction。
31. [池化与资源管理专题](90-cross-module/pooling-and-resource-management.md)：request row、slot/page allocator、物理 KV/Mamba buffer、Radix ownership 与 flush。
32. [M09 Attention 与 CUDA Graph](01-modules/M09-attention-cuda-graph/README.md)：backend metadata、eager/graph 选择、capture eligibility 与 overlap 边界。
33. [M10 采样与约束输出](01-modules/M10-sampling-constraints/README.md)：SamplingBatchInfo、penalty、grammar mask、sampler 和输出边界。
34. [M11 Speculative decoding](01-modules/M11-speculative-decoding/README.md)：draft/target、accept/reject、bonus token、KV 回滚和算法变体。
35. [M12 多模态 Runtime](01-modules/M12-multimodal-runtime/README.md)：media preprocess、placeholder、feature cache、M-RoPE 和 rank 分片。
36. [M13 分离部署与 HiCache](01-modules/M13-disaggregation-hicache/README.md)：KV transfer、bootstrap、staging、prefetch/restore 和失败状态。
37. [M14 MoE、量化与 LoRA](01-modules/M14-moe-quantization-lora/README.md)：quant method、expert location/EPLB 和 adapter 生命周期。
38. [M15 多进程与 IPC](01-modules/M15-ipc-control-plane/README.md)：scheduler/detokenizer 子进程、ZMQ、startup pipe、序列化和 cleanup。
39. [M16 Kernel 与设备后端](01-modules/M16-kernel-device-backend/README.md)：platform resolver、AOT/JIT/fallback 和设备能力边界。
40. [M17 Rust、Router 与 Gateway](01-modules/M17-rust-router-gateway/README.md)：PyO3/gRPC、KV-aware router、service discovery 和可靠性。
41. [M18 测试、Benchmark 与 CI](01-modules/M18-testing-benchmark-ci/README.md)：suite 发现、过滤、分片、registered/manual、benchmark 和验证证据。

> `03-model-loading/` 是模型加载专题目录；实现层的模块文章位于 `01-modules/M06-model-loading/`。两者分别承担启动主链和独立实现细节。

所有模块都以普通 HTTP LLM 为主线，并把 Ray、diffusion、speculative、multimodal、disaggregation、Rust gateway 和设备后端写成替换或扩展路径。文章中的真实 GPU、多卡、模型下载、CUDA Graph、NCCL/NIXL/RDMA 和端到端执行均需另有运行证据；当前 checkout 只提供静态源码证据。

### D. 真实 Demo 解剖层

- 目标目录：`80-demos/`。
- 候选主线：`examples/runtime/engine/offline_batch_inference.py`，需要从 argparse、`ServerArgs`、`sgl.Engine`、子进程、`generate` 一直追到真实执行和 `shutdown`。
- 当前状态：已创建 D01 深度文章，并登记 D02 协议/结构化输出与 D03 speculative 静态链路；真实模型启动、checkpoint 加载、GPU 输出和端到端结果仍未验证。

### E. 跨模块串联层

- 目标目录：`90-cross-module/`。
- 计划集中记录 system wiring、接口契约、端到端 runtime trace、共享数据、配置影响、错误边界、修改影响和性能关键路径。
- 当前状态：已建立 system wiring、接口契约、运行轨迹、端到端流程，以及调用链、共享数据、配置影响、错误边界、修改影响和性能路径；静态链接与引用审计已完成，变体运行证据仍待补充。

### F. 开发实践与路线层

- 目标目录：`99-roadmap/`。
- 计划包括快速上手、阅读路线、调试、功能开发、测试、性能、风险登记、技术债务和后续步骤。
- 当前状态：已创建开发实践导航、快速上手、阅读、调试、开发配方、测试、性能、风险、技术债务和后续步骤；静态审计已完成，真实模型、GPU、CUDA Graph、多卡通信和完整测试仍未验证。
- [分级 QA 题库](99-roadmap/qa.md)：入门级 100 题、中级 100 题、高级 100 题、专家级 100 题；用于检验概念、调用链、状态、资源、性能和故障分析。

## QA 题库

题库按难度拆分为 [入门级](99-roadmap/qa-beginner.md)、[中级](99-roadmap/qa-intermediate.md)、[高级](99-roadmap/qa-advanced.md) 和 [专家级](99-roadmap/qa-expert.md)。参考答案以当前 checkout 为证据；涉及真实 GPU、模型、多卡通信和性能的题目明确要求区分静态源码推理与运行验证。

## 一条请求的总览

```text
客户端
  │ HTTP/OpenAI/gRPC 或 offline Engine API
  ▼
FastAPI 路由 / Engine.generate
  ▼
TokenizerManager：校验、tokenize、创建请求状态
  ▼ IPC/进程边界
Scheduler：排队、前缀匹配、分配 KV block、构造批次
  ▼
TP/PP/DP/EP worker
  ▼
ModelRunner：embedding → transformer layers → logits → sampler
  ▼
Scheduler：写回 token、更新 KV cache、判断结束
  ▼ IPC
TokenizerManager：detokenize、组织流式输出
  ▼
HTTP/SSE/OpenAI response 或 offline 返回值
```

这条请求链依赖一条先发生的启动链：

```text
ServerArgs
  → resolution / publish
  → scheduler worker 与 rank/GPU 绑定
  → WORLD、TP/PP/attention/MoE groups
  → ModelConfig / LoadConfig
  → model loader / model parameters
  → KV pool / Radix Cache
  → attention backend / CUDA Graph
  → ready
```

## 文档中的代码引用约定

- 路径使用仓库根目录作为相对路径。
- 行号对应撰写时的代码版本；代码变化后应以函数名和附近逻辑为准，并重新核对行号。
- “逐行分析”优先解释每一段代码的输入、输出、状态变化和设计原因，不机械翻译每个语句。
- 代码示例是帮助理解的最小例子，不一定是可直接用于生产的完整配置。
