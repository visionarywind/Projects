# M10 风险与技术债

| 风险 | 静态依据 | 后果 |
|---|---|---|
| 测试 binary 与 case 脱节 | make-time unknown binary check `[tests/cuda_test/cuda_test.nvmk:93-114]` | list 能生成但无法运行 |
| dispatcher 生成物依赖规则 | `cuda_test_list.cpp` `[tests/cuda_test/cuda_test.nvmk:140-153]` | 手工改生成文件会丢失 |
| 外部依赖缺失 | gpgpuHelpers、testutils、Boost、nvcc `[tests/cuda_test/cuda_test.nvmk:10-43]` | 无法在 docs-only tree 构建 |
| runner 改变系统状态 | sudo persistence mode/nvidia-smi `[tests/run_tests.py:208-230]` | 需要权限和 GPU |
| DVS 结果误读 | no result 转 failure、waived 不计 score `[tests/run_tests.py:37-73,171-189]` | 聚合结果与实际覆盖混淆 |
| 实验产物混杂 | experiments 中 cubin/SASS/报告并存 | 二进制被误当源码 |

**深度等级：中等。** 构建/dispatcher/runner 和测试地图已闭合；具体各 test case 的硬件前置、MODS 适配和 CI 发布流仍需进一步读取。
