# 快速上手

1. 先读根 README 的 5 分钟快速理解。
2. 读 D01：理解 Core API 的最小训练链。
3. 读 M02：理解 TP/PP/DP/CP/EP group 和 schedule。
4. 读 M03：理解正式 `pretrain` 如何编排模型、数据、优化器。
5. 根据任务进入 M01/M04/M05/M06。

## 不建议直接做的事

- 不要在 docs-only checkout 根目录运行上游训练命令。
- 不要把未运行的 GPU/NCCL 命令写成已验证。
- 不要修改原始源码目录，除非任务明确要求。

## 最小验证顺序

```text
静态阅读 -> markdown diff/check -> D01 torchrun -> focused unit tests -> 正式 pretrain smoke
```

硬件相关命令需在上游源码和正确容器/依赖中运行。