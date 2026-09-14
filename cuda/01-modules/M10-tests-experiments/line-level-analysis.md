# M10 行级分析

- `[tests/cuda_test/cuda_test.nvmk:60-87]`：separate compilation device-link 和 binary object 规则。
- `[tests/cuda_test/cuda_test.nvmk:93-120]`：test name/disposition/binary 引用的 make-time 检查。
- `[tests/cuda_test/cuda_test.nvmk:140-153]`：生成 test list 并链接 dispatcher。
- `[tests/cuda_test/dispatch_main.cpp:22-50]`：初始化、GPU list、环境检查和 run/list。
- `[tests/run_tests.py:8-73,85-132]`：子进程双 pipe 消费、DVS 提取、timeout 和结果状态。
- `[tests/run_tests.py:171-189,191-245]`：no-result→failure、Waived/Pass/Failure 聚合、zero-pass/nonzero exit 和 Linux persistence/GPU 前置动作。
- `[tests/dvs-build-unix.sh:1-43]`：Linux nvmake/DVS 构建、复制和 tar 打包。

这些行只支持构建规则和 runner 控制流；本工作区没有由此产生的已验证 GPU/DVS 结果。
