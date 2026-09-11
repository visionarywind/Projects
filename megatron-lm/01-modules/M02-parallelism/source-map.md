# M02 源码地图与测试

| 源码 | 符号/职责 |
|---|---|
| `megatron/core/parallel_state.py:232-260` | `create_group`，封装 distributed group 创建 |
| `parallel_state.py:269-570` | rank group 正交生成和 `RankGenerator` |
| `parallel_state.py:600-约 1580` | TP/PP/DP/CP/EP/GTP group 初始化 |
| `parallel_state.py:2505+` | destroy/accessor 相关状态 |
| `pipeline_parallel/schedules.py:53-220` | schedule 选择、输出释放、backward helper |
| `schedules.py:723-928` | 无 pipeline |
| `schedules.py:1019+` | 交错 pipeline |
| `schedules.py:2147+` | 非交错 pipeline |
| `pipeline_parallel/p2p_communication.py` | stage 间通信 |
| `tests/unit_tests/test_parallel_state.py` | rank/group 初始化与销毁测试 |
| `tests/unit_tests/pipeline_parallel/` | schedule/P2P 回归测试 |

测试命令依赖上游锁定环境；本批只做文档静态分析，没有运行测试。