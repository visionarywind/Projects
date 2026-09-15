# M10 数据结构、源码地图与开发指南

- 文档目的：解释 01-modules/M10-tests-experiments/source-map.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M10-tests-experiments/source-map.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


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
