# 分级 QA 题库

本题库用于检验对 SGLang 源码架构与大模型推理基础的整体理解，证据锚点为 checkout `f1a512c51c73ab660cf41e1af3110c7c11e3b600`。

## 题库导航

- [入门级：100 题](qa-beginner.md)：概念、普通请求主线、进程边界、调度、KV、模型执行和基础清理。
- [中级：50 题](qa-intermediate.md)：请求状态、admission、batch 快照、KV ownership、sampling、并行、加载、故障和测试设计。
- [高级：50 题](qa-advanced.md)：连续批处理取舍、overlap、CUDA Graph、attention contract、多维并行、speculative/multimodal、性能实验和修改审计。
- [专家级：50 题](qa-expert.md)：运行时不变量、异步并发、KV ownership、数值与采样正确性、分布式拓扑、扩展边界和系统级验证。

## 难度标准

### 入门级

能解释术语和普通主线，回答“对象是什么、在哪个阶段、数据如何变化”。题目仍要求连接真实对象，例如 `ReqState`、`Req`、`ScheduleBatch`、`ForwardBatch` 和 `rid`，不是纯词汇记忆。

### 中级

能阅读关键函数并解释字段、状态转移、资源分配和错误清理。应能从一个输入追踪到跨进程输出，并区分逻辑请求、batch 行、request row 和 KV slot。

### 高级

能跨模块推导不变量、竞态、性能瓶颈和修改影响，提出验证实验或测试方案。涉及 GPU、多卡、CUDA Graph、模型加载和性能时，必须区分源码静态证据与实际运行证据。

## 覆盖矩阵

| 主题 | 入门 | 中级 | 高级 |
|---|---:|---:|---:|
| 通用推理与 Transformer | ✓ | ✓ | ✓ |
| CLI/Engine/Tokenizer/ReqState | ✓ | ✓ | ✓ |
| Scheduler/continuous batching | ✓ | ✓ | ✓ |
| KV/Prefix/Radix Cache | ✓ | ✓ | ✓ |
| ForwardBatch/ModelRunner | ✓ | ✓ | ✓ |
| Attention/CUDA Graph | ✓ | ✓ | ✓ |
| Sampling/grammar/logprob | ✓ | ✓ | ✓ |
| TP/PP/DP/EP 与通信 | ✓ | ✓ | ✓ |
| 模型加载/量化/MoE | ✓ | ✓ | ✓ |
| speculative/multimodal/disaggregation | ✓ | 部分 | ✓ |
| IPC/错误/cleanup/测试 | ✓ | ✓ | ✓ |
| 性能与修改影响 | 部分 | ✓ | ✓ |

## 作答和证据规则

1. 先独立作答，再查看每题参考答案；答案中的“参考”不是唯一表述。
2. 代码级题至少说明调用者、输入、输出和状态变化；不能只复述函数名。
3. 资源题要说明 request row、KV slot、Radix node、lock/reference、GPU tensor 或 host backup 的所有权。
4. 性能题要给出可测量指标和对照组，不把架构直觉写成 benchmark 结果。
5. 题目关联的行号可能随源码变化；以函数名和当前 checkout 复核为准。

题库与现有实现文章的关系：题目用于检验整体理解，不代表尚未完成的 M01/M02/M11-M18 已有完整实现层文章；相关内容仍应查看 `source-doc/00-overview/analysis-state.md` 中的未完成和未验证范围。
