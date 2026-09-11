# M03 源码地图与测试

| 源码 | 职责 |
|---|---|
| `megatron/training/initialize.py:48-176` | distributed/CUDA/seed 初始化 |
| `megatron/training/training.py:1530+` | `pretrain` 编排 |
| `training.py:2435+` | model list、DDP/FSDP 包装 |
| `training.py:2632+` | optimizer/scheduler setup |
| `training.py:3092+` | 单次 train step |
| `training.py:3396+` | training log |
| `training.py:4309+` | 主训练循环 |
| `training.py:5627+` | dataset/loader/iterator 构造 |
| `tests/unit_tests/training/` | training helper regression |
| `tests/unit_tests/test_utilities.py` 等 | 初始化/辅助路径 |

建议验证顺序：先用 CPU 可静态检查的 helper/unit test，再在匹配 CUDA/NCCL 容器运行 D01，最后验证 PP、FSDP、async checkpoint。