# M01 源码地图

- 文档目的：将 M01 文件、符号、调用者和测试关联起来。
- 适用范围：GPT/Transformer Core。
- 对应源码版本：`3703d4e33a3a2b2d11ebcc8e41f45af7ce7d1eda`
- 证据状态：已确认（入口）；部分完成（全量符号）
- 最后更新：2026-09-10

| 源码位置 | 类型 | 作用 | 调用方 | 重要性 |
|---|---|---|---|---|
| `model_provider.py:19-58` | provider | 选择最终 builder | training.get_model | 高 |
| `gpt_builders.py:24-110` | builder | 选择 spec 并实例化 GPT | provider | 高 |
| `megatron/core/models/gpt/gpt_model.py:52-312` | 类构造 | 创建 embedding/decoder/output | builder | 高 |
| `gpt_model.py:567-676` | forward | 预处理、decoder、后处理 | schedule/inference | 高 |
| `gpt_model.py:939+` | checkpoint mapping | 生成 sharded state dict | M05 | 高 |
| `transformer_config.py:55+` | dataclass | 模型配置字段和校验 | builder/model | 高 |
| `transformer_block.py:267-407` | 类构造 | 分配层和 norm | GPTModel | 高 |
| `transformer_block.py:506-782` | forward/state | 层执行与分片状态 | GPTModel/checkpoint | 高 |
| `tests/unit_tests/models/test_gpt_model.py` | 测试 | GPT model regression | CI | 高 |

## 相关文档

- [README](README.md)
- [测试](testing.md)

## 源码证据摘要

见表格。

## 未解决问题

需将每个 `gpt_layer_specs.py` backend 分支和对应测试映射补齐。

## 下一步阅读建议

优先阅读高重要性文件。
