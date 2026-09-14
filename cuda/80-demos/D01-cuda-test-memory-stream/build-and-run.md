# D01 构建与运行

源码构建关系来自 `tests/cuda_test/cuda_test.nvmk` 和 `tests/makefile.nvmk`：先生成 `libcuda_test`，再生成各测试 binary 与 `cuda_test` dispatcher；`.cu` separate compilation 需要 nvcc device-link（静态确认：[tests/makefile.nvmk:3-18]；[tests/cuda_test/cuda_test.nvmk:60-153]）。

典型运行入口由 `tests/run_tests.py` 调度，先列 GPU，再用 `--listGpus --forceRun` 运行 suite；Linux 可能调用 persistence service 和 `nvidia-smi`（静态确认：[tests/run_tests.py:208-245]）。

本工作区未执行构建、运行或 GPU 命令，所有结果未验证。
