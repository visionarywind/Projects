# M10 测试调用链、覆盖矩阵与实验边界

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
