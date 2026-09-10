# 分析状态

- 文档目的：支持分批续作，记录事实、覆盖和缺口。
- 目标源码：分支 `main`，HEAD `8190837c2b6ce176a431bc2a6ffd3439507648a7`。
- 最后更新：2026-09-10
- 当前批次：阶段 0/1 总览、模块候选、主 Demo 选择已完成；模块实现与 Demo 深度分析进行中。

## 已确认范围

- 已读取：README、AGENTS/CLAUDE symlink、.cursorrules、pyproject、setup.py、贡献文档、测试 skill、lint/build skill、代表性入口/模型/schedule/parallel_state/optimizer/test/CI/doc 文件。
- 已识别主入口：`pretrain_gpt.py`、`pretrain_hybrid.py`、`pretrain_mamba.py`、`pretrain_vlm.py`、`examples/run_simple_mcore_train_loop.py`、tools 推理入口。
- 已识别稳定主链：entry → initialize → process groups → builder/model + dataset → schedule → gradients/optimizer → checkpoint/output。

## 文档完成度与深度审计

| 分析对象 | 入口落地 | 正常路径 | 分支 | 异常 | 清理 | 数据生命周期 | 执行上下文 | 行级证据 | Demo 映射 | 状态/缺口 |
|---|---|---|---|---|---|---|---|---|---|---|
| 总览 | 已完成 | 已完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 已完成 | 部分完成 | 主 GPT 静态骨架，待动态验证 |
| M01 Core 模型 | 部分完成 | 部分完成 | 已完成 | 未开始 | 未开始 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 需追 TransformerBlock.forward |
| M02 并行与调度 | 部分完成 | 部分完成 | 已完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 已完成 | 需补具体 schedule 主函数和 P2P |
| M03 训练运行时 | 部分完成 | 部分完成 | 部分完成 | 未开始 | 未开始 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 需追 pretrain/train loop |
| M04 数据管线 | 部分完成 | 部分完成 | 部分完成 | 未开始 | 未开始 | 部分完成 | 未开始 | 部分完成 | 已完成 | 需追 MockGPTDataset 真实字段 |
| M05 优化器与检查点 | 已完成 | 部分完成 | 部分完成 | 未开始 | 部分完成 | 已完成 | 部分完成 | 部分完成 | 已完成 | 需深入 save/load 实现 |
| M06 推理与工具 | 未开始 | 未开始 | 未开始 | 未开始 | 未开始 | 未开始 | 未开始 | 未开始 | 未开始 | 仅完成模块候选识别 |
| D01 主 Demo | 已完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 已完成 | 已完成 | 静态分析，未运行 |
| 跨模块系统 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 需统一轨迹 |

状态取值严格为：未开始、部分完成、已完成、受阻。未将“文件已创建”当作分析完成。

## 覆盖维度

| 维度 | 当前状态 | 缺口 |
|---|---|---|
| 目录/模块 | 已归类六个主模块 | M06 和辅助模块需深入 |
| 实现/入口 | GPT 主入口已定位 | 需越过 wrapper 到 schedule/model 实现 |
| 流程/串联 | 主训练骨架 | 错误、资源、异步和关闭路径 |
| Demo | D01 已选择 | GPU 运行和完整清理未验证 |
| 符号/证据 | 关键入口有行号 | 需补更多定义及测试 |
| 图示 | 总体/数据/启动图已写 | 模块级图示 |
| 开发场景 | 根指南和部分配方 | 需按模块补齐 |

## 未解决问题

1. 当前环境是否有 CUDA/GPU、目标容器和可用 NCCL。
2. `GPTModel.forward`、TransformerBlock 和 P2P 的完整动态调用路径。
3. 多节点、异步 checkpoint 和 fault-injection 的实际清理顺序。
4. M06 推理服务应以哪个入口作为代表 Demo。

## 下一批起点

先完成 M01/M02 的 implementation、execution-flows、source-map 和 call-chains；再补 D01 execution/data/debug/failure；随后完成 M03-M06、跨模块文档和 roadmap。每批更新本文件与 evidence-index。

## 相关文档

- [根入口](../README.md)
- [证据索引](evidence-index.md)
- [模块注册表](../01-modules/module-registry.md)

## 源码证据摘要

入口证据见 `evidence-index.md`。

## 下一步阅读建议

从 D01 开始续作，不要重复已确认的总览。
