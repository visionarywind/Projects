# M01 接口

- 文档目的：定义 builder、model 和 forward 的调用契约。
- 适用范围：GPT Core。
- 对应源码版本：`3703d4e33a3a2b2d11ebcc8e41f45af7ce7d1eda`
- 证据状态：部分完成
- 最后更新：2026-09-10
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M01-core-models/interfaces.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。

## 关键接口

| 接口 | 输入 | 输出/保证 | 错误与约束 |
|---|---|---|---|
| `model_provider` [model_provider.py:19-58] | builder、stage、config、PG | `GPTModel|HybridModel` | args 必须已初始化；ModelOpt 可替换 builder |
| `gpt_builder` [gpt_builders.py:24-110] | args、stage、config、PG | GPTModel | spec/config/feature 组合必须合法 |
| `GPTModel.__init__` [gpt_model.py:98-130] | TransformerConfig、ModuleSpec、vocab、sequence、stage | module | vocab/hidden/PG 契约 |
| `GPTModel.forward` [gpt_model.py:567-676] | tokens、position_ids、attention_mask、labels 等 | hidden/logits/loss | stage、dtype、shape 与 config 匹配 |
| `sharded_state_dict` [gpt_model.py:939+] | prefix/metadata | sharded mapping | checkpoint consumer 需理解 shard metadata |

## 线程安全与兼容

模型本身不是跨线程可变服务；同一 module 的 forward/parameter mutation 应由训练调度串行协调。TP/PP 通信安全由 M02 保证。GPTModel 明确发出 deprecated warning，新增能力应评估 HybridModel 兼容路径。[gpt_model.py:123-128]

## 最小调用

真实最小模型创建见 [examples/run_simple_mcore_train_loop.py:56-78]；真实 forward 调用见 [examples/run_simple_mcore_train_loop.py:154-165]。命令和运行状态见 D01，当前未验证。

## 相关文档

- [数据结构](data-structures.md)
- [跨模块契约](../../90-cross-module/interface-contracts.md)

## 源码证据摘要

见接口表。

## 未解决问题

需要列出每种 position embedding、packed sequence 和 inference context 的完整参数契约。

## 下一步阅读建议

对照 `GPTModel.forward` 签名与 D01 的 `forward_step_func`。
