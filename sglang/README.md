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

23. [M03 Tokenizer 与请求状态](01-modules/M03-tokenizer-request-state/README.md)：请求状态、tokenize、IPC dispatch、增量输出与清理。
24. [M04 Scheduler 与连续批处理](01-modules/M04-scheduler-batching/README.md)：waiting/running admission、PrefillAdder、ScheduleBatch、forward 和 retraction。
25. [模型加载与并行初始化](03-model-loading/01-模型加载与并行初始化.md)：M06/M07/M05/M08/M09 的启动主链。
26. [M07 分布式并行](01-modules/M07-分布式并行.md)：WORLD、TP/PP、attention/MoE 拓扑、GroupCoordinator、collective 与清理。
27. [M05 模型执行](01-modules/M05-model-execution/README.md)：ForwardBatch、TP worker、ModelRunner、CUDA Graph、sampling 与结果边界。
28. [M08 KV Cache 与 Radix Cache](01-modules/M08-kv-cache/README.md)：request row、KV slot、prefix match、Radix 插入、回收与 retraction。
29. [M09 Attention 与 CUDA Graph](01-modules/M09-attention-cuda-graph/README.md)：backend metadata、eager/graph 选择、capture eligibility 与 overlap 边界。
30. [M10 采样与约束输出](01-modules/M10-sampling-constraints/README.md)：SamplingBatchInfo、penalty、grammar mask、sampler 和输出边界。

> `03-model-loading/` 是模型加载专题目录；模块注册表中的 M06/M07 状态会随着实现级文章和专项文章补齐而更新。Radix Cache 专题文章尚待补充，当前先使用基础概念文章和现有请求链路中的证据。

后续实现层将继续覆盖：CLI/API、Tokenizer、Scheduler、ModelRunner、KV/Radix Cache、attention/CUDA Graph、sampling/grammar、speculative decoding、多模态、disaggregation/HiCache、MoE/quantization/LoRA、IPC、kernel/device backend、Rust 服务和测试系统。只有达到模块注册表中的最低标准后，模块状态才会改为完成。

### D. 真实 Demo 解剖层

- 目标目录：`80-demos/`。
- 候选主线：`examples/runtime/engine/offline_batch_inference.py`，需要从 argparse、`ServerArgs`、`sgl.Engine`、子进程、`generate` 一直追到真实执行和 `shutdown`。
- 当前状态：已创建 D01 深度文章；真实模型启动、checkpoint 加载、GPU 输出仍未验证。

### E. 跨模块串联层

- 目标目录：`90-cross-module/`。
- 计划集中记录 system wiring、接口契约、端到端 runtime trace、共享数据、配置影响、错误边界、修改影响和性能关键路径。
- 当前状态：已建立 system wiring、接口契约、运行轨迹、端到端流程，以及调用链、共享数据、配置影响、错误边界、修改影响和性能路径；仍需与后续专题复核。

### F. 开发实践与路线层

- 目标目录：`99-roadmap/`。
- 计划包括快速上手、阅读路线、调试、功能开发、测试、性能、风险登记、技术债务和后续步骤。
- 当前状态：已创建开发实践导航、快速上手、阅读、调试、开发配方、测试、性能、风险、技术债务和后续步骤；完整模块和真实运行验证仍未完成。
- [分级 QA 题库](99-roadmap/qa.md)：入门级 100 题、中级 50 题、高级 50 题、专家级 50 题；用于检验概念、调用链、状态、资源、性能和故障分析。

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
