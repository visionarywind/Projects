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
