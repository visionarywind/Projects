# M10 接口契约

- `TEST_BINARY`：定义 binary、sources、`LIBCUDA_TEST_DEPS` 和 build dependency。
- `TEST_CASE`：要求 command first token 已定义 binary，生成 disposition/arguments。
- `dispatch_main`：初始化 testsuite/testutils，负责 list/run。
- `run_tests.py`：执行子进程、并发消费 pipes、解析 DVS、决定最终 exit。
- `dvs-build-unix.sh`：仅 Linux，调用外部 nvmake 并打包 executable/script。
