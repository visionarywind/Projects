# 推理方向面试题库

覆盖大模型在线推理服务从请求接入、协议处理、调度 batching、Transformer 计算、KV cache 管理到分布式执行、量化部署、可观测性与生产运维的完整链路。题目以具体疑问句或设计指令取代名词式题干，每题答案含可验证事实单元。

## 当前状态

- 题量：65 题（入门 15、中级 20、高级 18、专家 12）
- 题号：`I-B001`–`I-B015` / `I-I001`–`I-I020` / `I-A001`–`I-A018` / `I-E001`–`I-E012`
- 知识域：18 个（I01–I18），覆盖启动、协议、IPC、调度、计算图、prefill/decode、attention/CUDA Graph、KV cache、sampling、分布式、speculative、量化、disaggregation、LoRA、长上下文、观测、部署、安全

## 证据规则

- `[C]`：本仓库源码/文档中明确记载的结论（附 `path:line`）
- `[I]`：基于多个源码入口的推导
- `[U]`：依赖 GPU、模型、网络、RDMA、多进程或压测，当前未执行
- 无运行记录不给具体性能数字；薄弱域（I14 LoRA、I18 生产安全）明确标注 U

## 导航

- [入门题](01-question-bank/beginner.md)（心智模型，15 题）
- [中级题](01-question-bank/intermediate.md)（调用链、排障，20 题）
- [高级题](01-question-bank/advanced.md)（架构取舍、量化论证，18 题）
- [专家题](01-question-bank/expert.md)（第一性原理、失效边界，12 题）
- [知识域详解](02-knowledge-domains/README.md)（I01–I18，与题目互链）

## 使用方式

按 B → I → A → E 顺序学习；每题先口述结论，再完成追问、反例与验证。涉及推理服务性能、容量、多卡或部署的结论，没有目标环境运行时按 `[U]` 处理。

## 格式约定

本题库沿用推理方向既有格式：三级标题 `### I-B001`、**加粗标签**元数据（元数据/背景知识/问题分析/具体答案/技术洞察/图文说明/拓展知识/常见追问及回答要点/易错点与反例/实践与验证/分层评分标准/证据与来源）、C/I/U 证据标签。每题 12 段正文。

## 知识源

主要引用本仓库 `sglang/`（M02–M10、M13、M15、M18、M20、request-flow、cross-module、99-roadmap）、`insights/`（vLLM/PagedAttention/PD-Serve 论文笔记、speculative decoding、KV pool 分析）及 `cuda/` 性能指南。每题末尾 `**证据与来源：**` 行列出具体引用。

## 薄弱域说明

本仓库知识供给不均：协议、调度、计算、KV cache、分布式、性能有丰富源码级分析；LoRA/多模态（I14）、生产安全/多租户（I18）无源码沉淀——相关题目写通用机制并标 `[U]`，不编造。
