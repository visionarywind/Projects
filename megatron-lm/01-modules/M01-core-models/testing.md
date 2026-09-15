# M01 Core 模型：Testing

- 文档目的：补齐 M01 核心模型的 Testing 入口。
- 对应源码版本：`main` / `3703d4e33`。
- 证据状态：部分完成；核心路径已由源码静态确认，GPU/分布式运行未验证。

## 结论摘要

M01 的模型对象由 `gpt_builder`/`GPTModel` 创建，forward 继续进入 Transformer block；本文只记录当前可由源码确认的入口与资源边界，未将未运行行为写成已验证。

- 入口：`source/megatron-lm/megatron/core/models/gpt/gpt_model.py`。
- 关键构造：`source/megatron-lm/megatron/core/models/gpt/gpt_model.py:98-242`。
- forward：`source/megatron-lm/megatron/core/models/gpt/gpt_model.py:567-674`。
- 模型参数由上层 optimizer/DDP buffer 借用；M01 不负责 checkpoint 文件或进程组生命周期。

## 深度审计

| 分析对象 | 入口落地 | 正常路径 | 分支 | 异常 | 清理 | 数据生命周期 | 执行上下文 | 行级证据 | Demo 映射 | 状态/缺口 |
|---|---|---|---|---|---|---|---|---|---|---|
| M01 Testing | 已完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | D01 静态 | 需继续逐行追 backend/TE 分支 |

## 相关文档

- [M01 README](README.md)
- [资源专题](../../90-cross-module/pooling-and-resource-management.md)

## 未解决问题

- 不同 ModuleSpec、Transformer Engine、重计算和 CUDA Graph 分支需要运行时验证。
