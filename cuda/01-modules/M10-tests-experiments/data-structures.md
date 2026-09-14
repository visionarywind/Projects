# M10 数据结构与构建产物

| 数据 | 作用 |
|---|---|
| `TEST_BINARIES`/`TEST_NAMES` | nvmake 中的 binary 和 case 注册集合 |
| `TEST_DEFINITION_<name>` | case 命令、disposition、arguments 定义 |
| generated `cuda_test_list.cpp` | dispatcher 注册测试列表 |
| `libcuda_test` | 测试公共库及其依赖 |
| DVS summary | passes/failures/waived/score 聚合 |

case 依赖已定义 binary；dispatcher 再链接生成列表和 `dispatch_main.cpp`。构建/运行均未验证（静态确认：[tests/cuda_test/cuda_test.nvmk:60-153]；[tests/run_tests.py:8-73]）。
