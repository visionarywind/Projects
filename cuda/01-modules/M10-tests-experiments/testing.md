# M10 测试矩阵

| 层次 | 代表入口 | 覆盖重点 | 状态 |
|---|---|---|---|
| dispatcher | `dispatch_main.cpp` | GPU 枚举、过滤、list/run | 静态确认，未运行 |
| sanity | `basic_sanity.cu` | init/context/memory/kernel/CNP | 静态确认，未运行 |
| stream/sync | `stream/`、`sync/` | push、marker、semaphore | 静态盘点，未运行 |
| memory | `memcpy/`、`memmgr/`、`memobj/` | pageable/device/P2P/manager | 静态盘点，未运行 |
| build/aggregate | `cuda_test.nvmk`、`run_tests.py` | device-link、DVS、双 pipe、timeout | 静态确认 |

`run_tests.py` 将 failures>0 或 passes==0 视为失败；waived 与 summary 去重规则不能替代具体 case 日志（静态确认：[tests/run_tests.py:8-73,171-189,208-245]）。
