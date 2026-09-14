# M10 数据结构与构建产物

| 数据 | 作用 |
|---|---|
| `TEST_BINARIES`/`TEST_NAMES` | nvmake 中的 binary 和 case 注册集合 |
| `TEST_DEFINITION_<name>` | case 命令、disposition、arguments 定义 |
| generated `cuda_test_list.cpp` | dispatcher 注册测试列表 |
| `libcuda_test` | 测试公共库及其依赖 |
| DVS summary | passes/failures/waived/score 聚合 |
| runner process/pipe state | 单测试 stdout/stderr、timeout 和退出码状态 |

## 关系与所有权

case 依赖已定义 binary；dispatcher 再链接生成列表和 `dispatch_main.cpp`。runner 只拥有子进程、pipe 和聚合结果，不拥有被测 CUDA context 或 GPU 资源；后者由测试 binary/libcuda_test 管理。构建/运行均未验证（静态确认：[tests/cuda_test/cuda_test.nvmk:60-153]；[tests/run_tests.py:8-73,171-245]）。
