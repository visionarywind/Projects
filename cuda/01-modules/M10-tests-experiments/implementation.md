# M10 构建、运行与结果模型

## 构建

`cuda_test.nvmk` 从 `gpgpuHelpers.nvmk`、`nvcc_common.nvmk` 和 `cuda.nvmk` 引入基础规则，加入 testutils、Boost program_options、CUDA includes/defines 和 common sources；每个 `TEST_BINARY` 依赖 `LIBCUDA_TEST_DEPS`，再由 `build::` 纳入构建（静态确认：[tests/cuda_test/cuda_test.nvmk:10-56,68-90]）。源码按 `.cu` 与非 `.cu` 分到不同 object 列表，并可执行 device-link separate compilation（静态确认：[tests/cuda_test/cuda_test.nvmk:60-87]）。

测试定义先生成 binary，再用 `TEST_CASE` 写入 `TEST_DEFINITION_*`；unknown binary、duplicate test name、invalid disposition 都在 make-time 报错（静态确认：[tests/cuda_test/cuda_test.nvmk:93-120]）。最终生成 `cuda_test_list.cpp`，与 `dispatch_main.cpp` 链接成 dispatcher（静态确认：[tests/cuda_test/cuda_test.nvmk:140-153]）。

## Dispatcher

`dispatch_main.cpp` 初始化 testsuite/testutils；`--listGpus` 打印 bus-order GPU 并禁用测试，`setCudaDeviceOrderEnvVars` 检查设备环境变量后运行或列出测试（静态确认：[tests/cuda_test/dispatch_main.cpp:12-50]）。

## 聚合器

`run_tests.py` 从子测试日志提取 Passes/Failures/Waived，去除重复 DVS summary，使用独立 stdout/stderr 线程避免 Windows pipe 阻塞，并默认设置每测试 15 秒 timeout（静态确认：[tests/run_tests.py:8-13,37-73,85-132,191-206]）。Linux 主入口会调用 `sudo nvidia-persistenced --persistence-mode`、`nvidia-smi`，以 `--listGpus --forceRun` 启动 `cuda_test`，失败或零 pass 时退出 1（静态确认：[tests/run_tests.py:208-245]）。

## DVS build

`dvs-build-unix.sh` 从脚本位置计算 `NV_SOURCE`，加载 `dvs-util.sh`，限制 target OS 为 Linux，调用 `run_nvmake` 构建 tests，复制 executable 和 `run_tests.py`，最后打包 tar（静态确认：[tests/dvs-build-unix.sh:1-43]）。
