# M05 源码地图

| 文件/位置 | 职责 |
|---|---|
| `core/optimizer/__init__.py:1004+` | optimizer factory |
| `core/optimizer/optimizer.py` | base/mixed/FP32/chained optimizer |
| `core/optimizer/distrib_optimizer.py` | range 和 distributed state |
| `training/checkpointing.py:176+` | checkpoint 参数校验和路径 |
| `checkpointing.py:611+` | save |
| `checkpointing.py:1567+` | state dict 生成 |
| `checkpointing.py:2469+` | load |
| `core/models/gpt/gpt_model.py:939+` | model sharded state dict |
| `tests/unit_tests/optimizer/` | optimizer 回归 |
| `tests/unit_tests/dist_checkpointing/` | sharded checkpoint 回归 |

底层文件系统、collective 和异步写入效果需动态测试确认。