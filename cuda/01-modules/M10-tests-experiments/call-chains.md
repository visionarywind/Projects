# M10 测试调用链、覆盖矩阵与实验边界

- 文档目的：解释 01-modules/M10-tests-experiments/call-chains.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M10-tests-experiments/call-chains.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


## 测试调用链

```text
makefile.nvmk
 → libcuda_test.build
 → cuda_test.nvmk TEST_BINARY / TEST_CASE
 → individual test binaries + cuda_test dispatcher
 → dispatch_main
 → testsuite generated list
 → subprocess.Popen
 → stdout/stderr consumer threads
 → extract_dvs_info
 → print_dvs_info / exit status
```

## 覆盖矩阵

| 领域 | 代表性源码 | 主要断言/证据 |
|---|---|---|
| 初始化/context | `basic_sanity.cu` | globals initialized、TLS、context、内部 memobj |
| kernel/CNP | `basic_sanity.cu` | launch count、push count、结果、child depth |
| memory | `memcpy/`, `memmgr/`, `memobj/`, `bik/` | pageable/device/3D/UVA/P2P/utility |
| stream | `stream/` | manager、push、sync、getters |
| channel | `channel/` | begin/end push、manager、marker、sync |
| event/sync | `event/`, `sync/` | event lifecycle、marker、CPU semaphore |
| containers | `util/` | list、queue、radix/hash helpers |

## Experiment 边界

`experiments/` 同时包含 `.cu`、SASS/cubin、P4 review、性能表、设计文档和独立 Makefile；例如 `experiments/cnp/` 的 spec/cubin 用于历史设计/回归资料，不能替代 `src/syscalls/cnp` 的当前实现证据。`memperf`、`syncblocks`、`stackalyze` 等可作为专项实验，但其命令、依赖和硬件结果必须分别标注。

## 错误路径

构建层错误包括 duplicate binary/test、unknown binary command、invalid disposition；运行层错误包括 executable 不存在、无 DVS summary、子测试 failure/timeout、无 pass；聚合器会把无结果视为 failure（静态确认：[tests/cuda_test/cuda_test.nvmk:74-114,tests/run_tests.py:171-189]）。

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
