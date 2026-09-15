# M01 执行流程

- 文档目的：给出构造、forward、异常和清理轨迹。
- 适用范围：GPT model path。
- 对应源码版本：`3703d4e33a3a2b2d11ebcc8e41f45af7ce7d1eda`
- 证据状态：部分完成
- 最后更新：2026-09-10

## M01-FLOW-INIT-001：模型构造

| 步骤 | 符号/证据 | 输入/分支 | 状态/资源 | 下一步 |
|---|---|---|---|---|
| 1 | `model_provider` [model_provider.py:34-58] | args, builder | 选择 ModelOpt 或原 builder | 2 |
| 2 | `gpt_builder` [gpt_builders.py:31-87] | spec/TE/MoE/heterogeneous | 确定 ModuleSpec | 3 |
| 3 | `GPTModel.__init__` [gpt_model.py:98-130] | config, vocab, stage flags | module fields/PG context | 4 |
| 4 | embedding/decoder/output [gpt_model.py:175-242,256-279] | pre/post process | 注册参数和子模块 | forward |

## M01-FLOW-MAIN-001：stage forward

入口 `GPTModel.forward` 位于 [gpt_model.py:567-676]，将输入交给 preprocess、decoder 和 postprocess；`TransformerBlock.forward` 位于 [transformer_block.py:506+]，逐层执行 ModuleSpec 对应实现。具体 kernel 目标依赖 config/backend，尚未逐候选验证。

## M01-FLOW-BRANCH-001：规格分支

`args.spec` 优先；否则 experimental attention、MoE、heterogeneous、普通 local/TE/inference 按顺序判断。[gpt_builders.py:31-54] 这条分支改变的是 Transformer layer spec，不改变 provider 的外部返回契约。

## M01-FLOW-ERROR-001

builder 对 inference optimized + MTP 等非法组合使用 `assert`；TE/optional backend 缺失在初始化或 layer import 时可能引发 ImportError/RuntimeError。错误向 M03 的 model setup 传播，未确认是否统一转换。

## M01-FLOW-CLEANUP-001

正常清理依赖调用方释放 model 与 distributed state；M01 的 `sharded_state_dict` 产生只读/映射型 checkpoint 表示，之后模型仍由调用方持有。[gpt_model.py:939+] 具体 offload/cudagraph 清理需要专项追踪。

## 相关文档

- [调用链](call-chains.md)
- [行级分析](line-level-analysis.md)

## 源码证据摘要

见各流程表。

## 未解决问题

缺少完整异常和资源释放轨迹，当前状态为部分完成。

## 下一步阅读建议

补 `forward` 的 preprocess/decoder/postprocess 行级表。
