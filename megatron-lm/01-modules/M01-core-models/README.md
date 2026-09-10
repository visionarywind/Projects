# M01 Core 模型

- 文档目的：解释配置、规格和 GPT 模型如何落地为可执行 Transformer。
- 适用范围：`megatron/core/models/gpt` 与 `transformer`；Hybrid/Mamba 只做边界说明。
- 对应源码版本：`main@8190837c2b6ce176a431bc2a6ffd3439507648a7`
- 证据状态：部分完成
- 最后更新：2026-09-10
- 前置阅读：[总体架构](../../00-overview/architecture.md)
- 后续阅读：[实现机制](implementation.md)

## 结论摘要

M01 的真实装配链是 `model_provider` → `gpt_builder` → `GPTModel.__init__` → `TransformerBlock`；builder 按显式 spec、Transformer Engine、MoE、异构配置选择 ModuleSpec，GPTModel 再创建 embedding、decoder 和 output layer。[model_provider.py:19-58] [gpt_builders.py:24-110] [gpt_model.py:98-242]

初学者可以把 `TransformerConfig` 理解为“模型蓝图”，`ModuleSpec` 理解为“每层使用哪个实现”，`GPTModel` 是把蓝图实例化的容器。它不负责 pipeline 的 microbatch 顺序，顺序由 M02 schedule 控制。

## 职责与非职责

负责 embedding/position encoding、decoder block、LM head、forward 输入输出和 sharded state dict；不负责创建默认 distributed group、不负责读取训练数据、不负责 optimizer.step。模型接受 `ProcessGroupCollection` 作为通信上下文。[gpt_model.py:95-130]

## 入口与阅读顺序

1. `model_provider.py:19-58`：provider wrapper 和 ModelOpt 分支。
2. `gpt_builders.py:24-110`：spec 选择和 GPTModel 构造。
3. `gpt_model.py:98-312`：字段、子模块和 output 初始化。
4. `gpt_model.py:567-676`：forward/pre/postprocess（待逐行补全）。
5. `transformer_block.py:267-782`：层构建、forward 和 shard state。

## 深度审计

| 分析对象 | 入口落地 | 正常路径 | 分支 | 异常 | 清理 | 数据生命周期 | 执行上下文 | 行级证据 | Demo 映射 | 状态/缺口 |
|---|---|---|---|---|---|---|---|---|---|---|
| M01 | 已完成 | 部分完成 | 已完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | D01-S004 | 需继续追 `forward` 到 attention/MLP kernel |

## 相关文档

- [设计](design.md) · [实现](implementation.md) · [执行流程](execution-flows.md) · [源码地图](source-map.md)
- [接口](interfaces.md) · [数据结构](data-structures.md) · [调用链](call-chains.md) · [图示](diagrams.md)
- [行级分析](line-level-analysis.md) · [示例](examples.md) · [测试](testing.md) · [开发](development-guide.md) · [风险](risks-and-debt.md)

## 源码证据摘要

`model_provider.py:19-58`; `gpt_builders.py:31-108`; `megatron/core/models/gpt/gpt_model.py:52-130,175-242`。

## 未解决问题

不同 attention backend、CUDA graph 和 TE 动态目标需要运行时 profiler 确认。

## 下一步阅读建议

先读 implementation，再对照 D01 的 model construction。
