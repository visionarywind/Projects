# M10 Tests/Experiments：构建适配、测试聚合与实验资料

- **职责**：把 `libcuda_test`、各测试 binary、dispatcher 和 DVS 结果聚合起来，并保存用于回归/性能/bug 研究的 experiments。
- **性质**：M10 是验证层，不是 runtime 实现层；测试源码可证明调用意图和断言，不能证明本工作区已经在 GPU 上执行。
- **当前状态**：构建依赖和聚合逻辑已静态确认；nvmake、nvcc、DVS、GPU 和 sudo persistence mode 均未运行。

```text
cuda_test.nvmk
 → libcuda_test dependency
 → per-test binary
 → generated cuda_test_list.cpp
 → dispatch_main.cpp
 → run_tests.py
 → DVS Pass/Failure/Waived aggregate
```

## 结果语义与边界

- **静态确认**：make rule 先检查 `TEST_BINARY`、case name 和 disposition，再生成 `cuda_test_list.cpp`；生成的 dispatcher 负责列 GPU 或运行测试，不能把生成列表当成测试已执行（[tests/cuda_test/cuda_test.nvmk:60-120,140-153]；[tests/cuda_test/dispatch_main.cpp:22-50]）。
- **静态确认**：runner 并行消费 stdout/stderr，按每测试 timeout 处理子进程；没有可解析结果的测试会被视为 failure，`Waived` 与 Pass/Failure 分开聚合，零 Pass 或存在 failure 时返回非零（[tests/run_tests.py:8-73,85-132,171-189,191-245]）。
- **未验证**：Linux 入口还可能调用 persistence mode、`nvidia-smi` 和 `cuda_test --listGpus --forceRun`，这些是运行环境动作，不是本知识库的验证结果。

experiments 中的 cubin、SASS、日志和报告只能作为历史/边界线索；源码级证据与实际硬件结果必须分开记录。
