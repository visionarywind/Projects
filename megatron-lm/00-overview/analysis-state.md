# 分析状态

- 文档目的：支持分批续作，记录事实、覆盖和缺口。
- 目标源码：分支 `main`，HEAD `8190837c2b6ce176a431bc2a6ffd3439507648a7`。
- 最后更新：2026-09-11
- 当前批次：M02-M06、D01、跨模块与 roadmap 已补齐首版静态分析；M01 深挖和动态验证仍待继续。

## 已确认范围

- 已读取：总览、模块注册表、M01 文档、核心训练入口、并行状态、pipeline schedule、训练循环、GPTDataset、optimizer/checkpoint、D01 示例和推理 engine/server 入口。
- 已创建：M02-M06 模块文档、D01 Demo 文档、90-cross-module 跨模块文档、99-roadmap 实践文档。
- 已识别稳定主链：entry → initialize → process groups → builder/model + dataset → schedule → gradients/optimizer → checkpoint/output。

## 文档完成度与深度审计

| 分析对象 | 入口落地 | 正常路径 | 分支 | 异常 | 清理 | 数据生命周期 | 执行上下文 | 行级证据 | Demo 映射 | 状态/缺口 |
|---|---|---|---|---|---|---|---|---|---|---|
| 总览 | 已完成 | 已完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 已完成 | 已完成 | 待动态验证 |
| M01 Core 模型 | 部分完成 | 部分完成 | 已完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 已完成 | 需继续逐行追 postprocess/backend |
| M02 并行与调度 | 已完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 已完成 | 交错 PP/overlap 仍需深入 |
| M03 训练运行时 | 已完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 已完成 | fault/async/cudagraph 需专项 |
| M04 数据管线 | 已完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 已完成 | 真实 indexed/SFT/packing 待验证 |
| M05 优化器与检查点 | 已完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 已完成 | 部分完成 | 部分完成 | 已完成 | save/load round-trip 未运行 |
| M06 推理与工具 | 已完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 未覆盖 D01 | offline/server Demo 未运行 |
| D01 主 Demo | 已完成 | 已完成 | 部分完成 | 部分完成 | 部分完成 | 已完成 | 部分完成 | 已完成 | 已完成 | 静态分析，未运行 |
| 跨模块系统 | 已完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 已完成 | 需随源码行号继续精化 |

状态取值严格为：未开始、部分完成、已完成、受阻。未将“文件已创建”当作动态行为已验证。

## 覆盖维度

| 维度 | 当前状态 | 缺口 |
|---|---|---|
| 目录/模块 | M01-M06 首版已覆盖 | M01 backend、M06 Demo 需深入 |
| 实现/入口 | 主入口与关键函数已定位 | optional backend/kernel 需专项 |
| 流程/串联 | 主训练、D01、推理服务首版 | 错误、资源、异步和关闭路径待实测 |
| Demo | D01 静态完成 | GPU 运行和完整清理未验证 |
| 符号/证据 | 关键入口有行号 | 部分新文档行号需二次精确化 |
| 图示 | 总体图与文本流程 | 模块级 mermaid 可继续补 |
| 开发场景 | roadmap 首版完成 | 测试命令和性能指南待实测补充 |

## 未解决问题

1. 当前环境是否有 CUDA/GPU、目标容器和可用 NCCL。
2. `GPTModel._postprocess`、Transformer layer backend 和 P2P overlap 的完整动态调用路径。
3. 多节点、异步 checkpoint、fault-injection 和 full-iteration CUDA graph 的实际清理顺序。
4. M06 推理服务应以哪个入口作为代表 Demo 并运行验证。

## 下一批起点

先精化 M01 forward/postprocess 与 M02 交错 pipeline；随后对 M05 save/load 和 M06 offline inference 做源码深挖；最后在可用硬件上运行 D01 和 focused tests，并把实测结果写入 D01、testing-recipes 与 analysis-state。

## 相关文档

- [根入口](../README.md)
- [证据索引](evidence-index.md)
- [模块注册表](../01-modules/module-registry.md)

## 源码证据摘要

入口证据见 `evidence-index.md`。
