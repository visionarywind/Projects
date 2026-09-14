# M10 数据结构、源码地图与开发指南

## 关键数据

- `TEST_BINARIES` / `TEST_NAMES`：make-time 的 binary/test 注册表。
- `TEST_DEFINITION_<name>`：生成 dispatcher test list 的文本定义。
- `COMMON_OBJECTS`、`CU_SOURCES`、`SOURCES`：共用、CUDA 和 C/C++ object 分类。
- DVS `info`：`passes/failures/waived/score` 聚合结构。
- `CudaGpuVector`：dispatcher 列出的 GPU 与过滤后的 CUDA ordinal。

## 源码地图

| 区域 | 文件 | 作用 |
|---|---|---|
| 顶层测试构建 | `tests/makefile.nvmk` | `libcuda_test` 先于 `cuda_test` |
| dispatcher build | `tests/cuda_test/cuda_test.nvmk` | sources、binary、case、list generation |
| dispatcher | `tests/cuda_test/dispatch_main.cpp` | options、GPU list、run/list |
| runner | `tests/run_tests.py` | process、timeout、DVS aggregation |
| DVS | `tests/dvs-build-unix.sh` | Linux nvmake build/package |
| experiments | `experiments/` | 专项实验与历史资料 |
| MODS | `mods/` | MODS 构建/适配与 fatbinary loader |

## 开发配方

1. 新测试源码加入对应子目录 `tests.nvmk`，再确认 binary/case 注册顺序。
2. 若含 device code，核对 `.cu` 编译、device-link 和 `__SKIP_INTERNAL_FATBINARY_HEADERS`。
3. 新增 API 测试必须覆盖成功、参数错误、context 错误和清理；不能只断言返回值。
4. 修改 runner 时保持 stdout/stderr 独立消费和 DVS summary 唯一性。
5. 实验资料标记输入、工具链、目标架构、产物和执行结果；二进制不作为源码唯一证据。

**验证状态：未验证。**
