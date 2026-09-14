# M10 行级分析

- `[tests/cuda_test/cuda_test.nvmk:60-87]`：separate compilation device-link 和 binary object 规则。
- `[tests/cuda_test/cuda_test.nvmk:93-120]`：test name/disposition/binary 引用的 make-time 检查。
- `[tests/cuda_test/cuda_test.nvmk:140-153]`：生成 test list 并链接 dispatcher。
- `[tests/cuda_test/dispatch_main.cpp:22-50]`：初始化、GPU list、环境检查和 run/list。
- `[tests/run_tests.py:104-132,171-189]`：子进程双 pipe 消费、DVS 提取和无结果失败。
