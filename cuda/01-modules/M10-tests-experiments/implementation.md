# M10 构建、运行与结果模型

- 文档目的：解释 01-modules/M10-tests-experiments/implementation.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M10-tests-experiments/implementation.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


## 构建

`cuda_test.nvmk` 从 `gpgpuHelpers.nvmk`、`nvcc_common.nvmk` 和 `cuda.nvmk` 引入基础规则，加入 testutils、Boost program_options、CUDA includes/defines 和 common sources；每个 `TEST_BINARY` 依赖 `LIBCUDA_TEST_DEPS`，再由 `build::` 纳入构建（静态确认：[tests/cuda_test/cuda_test.nvmk:10-56,68-90]）。源码按 `.cu` 与非 `.cu` 分到不同 object 列表，并可执行 device-link separate compilation（静态确认：[tests/cuda_test/cuda_test.nvmk:60-87]）。

测试定义先生成 binary，再用 `TEST_CASE` 写入 `TEST_DEFINITION_*`；unknown binary、duplicate test name、invalid disposition 都在 make-time 报错（静态确认：[tests/cuda_test/cuda_test.nvmk:93-120]）。最终生成 `cuda_test_list.cpp`，与 `dispatch_main.cpp` 链接成 dispatcher（静态确认：[tests/cuda_test/cuda_test.nvmk:140-153]）。

## Dispatcher 与 runner

`dispatch_main.cpp` 初始化 testsuite/testutils；`--listGpus` 打印 bus-order GPU 并禁用测试，`setCudaDeviceOrderEnvVars` 检查设备环境变量后运行或列出测试（静态确认：[tests/cuda_test/dispatch_main.cpp:12-50]）。

`run_tests.py` 从子测试日志提取 Passes/Failures/Waived，去除重复 DVS summary，使用独立 stdout/stderr 线程避免 Windows pipe 阻塞，并默认设置每测试 15 秒 timeout（静态确认：[tests/run_tests.py:8-13,37-73,85-132,191-206]）。它把 no-result 归为 failure；Waived 不等于 Pass，也不能与硬件通过混合解释。Linux 主入口会调用 `sudo nvidia-persistenced --persistence-mode`、`nvidia-smi`，以 `--listGpus --forceRun` 启动 `cuda_test`，失败或零 pass 时退出 1（静态确认：[tests/run_tests.py:208-245]）。

## DVS build 与证据边界

`dvs-build-unix.sh` 从脚本位置计算 `NV_SOURCE`，加载 `dvs-util.sh`，限制 target OS 为 Linux，调用 `run_nvmake` 构建 tests，复制 executable 和 `run_tests.py`，最后打包 tar（静态确认：[tests/dvs-build-unix.sh:1-43]）。该规则只能证明发布打包流程；构建、测试、timeout 和 DVS 结果在本轮均未执行。

## 相关文档
- [项目入口](../../README.md)
- [分析状态](../../00-overview/analysis-state.md)
- [源码证据索引](../../00-overview/evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](../../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](../../00-overview/analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
