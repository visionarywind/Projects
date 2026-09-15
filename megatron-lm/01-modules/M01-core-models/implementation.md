# M01 实现机制

- 文档目的：追踪模型从公开 builder 到真实 module 副作用的实现链。
- 适用范围：GPT 主路径。
- 对应源码版本：`3703d4e33a3a2b2d11ebcc8e41f45af7ce7d1eda`
- 证据状态：部分完成
- 最后更新：2026-09-10
- 前置阅读：[设计](design.md)
- 后续阅读：[调用链](call-chains.md)

## 实现组件表

| 组件 | 声明/定义 | 创建者 | 主要调用者 | 核心状态 | 副作用 |
|---|---|---|---|---|---|
| `model_provider` | `model_provider.py:19-58` | training model setup | `get_model` | args/modelopt choice | 选择 builder |
| `gpt_builder` | `gpt_builders.py:24-110` | provider | `get_model` | config/spec | 创建 GPT module |
| `TransformerConfig` | `transformer_config.py:55+` | builder/args | GPT/blocks | dimensions/features | 约束模型结构 |
| `GPTModel` | `gpt_model.py:52-312` | builder | schedule/inference | embedding/decoder/output | 注册 params |
| `TransformerBlock` | `transformer_block.py:267-782` | GPTModel | GPT forward | layers/final norm | 计算 hidden state |
| `sharded_state_dict` | `gpt_model.py:939+` | checkpoint caller | M05 | shard metadata | 输出 checkpoint mapping |

## 声明到落地链

```text
model_provider(args,...)
  -> model_builder(args,...)
    -> gpt_builder()
      -> branch: args.spec / experimental / num_experts / heterogeneous / default
        -> get_*_layer_spec()
          -> GPTModel(...)
            -> LanguageModelEmbedding + TransformerBlock + output_layer
              -> GPTModel.forward()
                -> TransformerBlock.forward()
                  -> layer attention/MLP/TE kernels
                    -> logits or hidden-state output
```

已确认的分派分支：[gpt_builders.py:31-54]；实例创建：[gpt_builders.py:89-108]；子模块：[gpt_model.py:175-242]。`TransformerBlock.forward` 的具体 attention/MLP 目标取决于 spec，候选定义在各 `gpt_layer_specs.py`，未对每个候选动态验证。

## 关键不变量

- `config` 与 layer spec 的维度、normalization、MoE 属性必须兼容；builder 对 inference optimized/MTP 有断言。[gpt_builders.py:55-87]
- `pre_process`/`post_process` 决定 pipeline stage 是否拥有 embedding/output；GPTModel 根据它们条件创建子模块。[gpt_model.py:175-182,256-279]
- `parallel_output=True` 时 output logits 保持 TP 分片（构造默认值见 [gpt_model.py:104-109]），调用方不能假定全 vocab 已 gather。
- `pg_collection` 被下传给 embedding、decoder 和 MTP；缺失时存在兼容 fallback/全局路径，需按调用者确认。

## 核心算法伪代码

```text
build(args, stage):
  config = YAML(args) or args_to_config(args)
  if explicit spec: use it
  elif experimental attention: choose experimental spec
  elif experts: choose MoE block spec
  elif heterogeneous: choose heterogeneous spec
  else: choose local/TE/inference spec
  return GPTModel(config, spec, stage flags, process groups)

forward(tokens, positions, mask, labels):
  if this stage owns input: embed tokens and positions
  hidden = decoder(hidden, mask, rotary/context metadata)
  if this stage owns output: project hidden to logits and optionally loss
  return stage output
```

这是对真实分支的简化，不替代 `GPTModel.forward`；核心 forward 行号 `gpt_model.py:567-676` 已定位，需后续逐块补充。

## 创建与销毁

构造通过 `nn.Module` 注册 embedding/decoder/output 参数；显式 Python `del` 不在构造路径出现，通常由 module 引用释放和训练 cleanup 管理。checkpoint 通过 `sharded_state_dict` 借用模型状态而非转移模型所有权。[gpt_model.py:939+] 具体 CUDA graph/offload buffers 的释放需继续跟踪。

## 相关文档

- [执行流程](execution-flows.md)
- [数据结构](data-structures.md)
- [M05 checkpoint](../M05-optimizer-checkpointing/implementation.md)

## 源码证据摘要

见组件表和链路。

## 未解决问题

需要逐行分析 `GPTModel.forward`、`TransformerBlock.forward`、local/TE attention 和 output loss 分支，才可标记 M01 完成。

## 下一步阅读建议

从 `gpt_model.py:567` 开始，记录每个 stage 的 hidden/logits 变化。
