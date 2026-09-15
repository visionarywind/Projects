# M01 Core 模型：Development Guide

- 文档目的：补齐 M01 核心模型的 Development Guide 入口。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：`main` / `3703d4e33`。
- 证据状态：部分完成；核心路径已由源码静态确认，GPU/分布式运行未验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

M01 的模型对象由 `gpt_builder`/`GPTModel` 创建，forward 继续进入 Transformer block；本文只记录当前可由源码确认的入口与资源边界，未将未运行行为写成已验证。

- 入口：`source/megatron-lm/megatron/core/models/gpt/gpt_model.py`。
- 关键构造：`source/megatron-lm/megatron/core/models/gpt/gpt_model.py:98-242`。
- forward：`source/megatron-lm/megatron/core/models/gpt/gpt_model.py:567-674`。
- 模型参数由上层 optimizer/DDP buffer 借用；M01 不负责 checkpoint 文件或进程组生命周期。

## 深度审计

| 分析对象 | 入口落地 | 正常路径 | 分支 | 异常 | 清理 | 数据生命周期 | 执行上下文 | 行级证据 | Demo 映射 | 状态/缺口 |
|---|---|---|---|---|---|---|---|---|---|---|
| M01 Development Guide | 已完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | D01 静态 | 部分完成：需继续逐行追 backend/TE 分支 |

## 相关文档

- [M01 README](README.md)
- [资源专题](../../90-cross-module/pooling-and-resource-management.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](../../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题

- 不同 ModuleSpec、Transformer Engine、重计算和 CUDA Graph 分支需要运行时验证。

## 下一步阅读建议
先阅读 [分析状态](../../00-overview/analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
