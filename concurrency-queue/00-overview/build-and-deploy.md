# 构建、运行与交付

- 文档目的：区分实际存在的构建入口、命令和验证状态。
- 证据状态：命令来自 Makefile/CMake/CI；已执行的 native Make/unit 命令有真实结果，其他命令仍明确标为未验证。
- 最后更新：2026-09-10
- 前置阅读：[项目概览](project-overview.md)
- 后续阅读：[M07](../01-modules/M07-packaging-and-ci/README.md)

## Legacy Make

```sh
cd build
make                 # default: tests benchmarks
make tests
make benchmarks
make bin/unittests
make bin/fuzztests
make bin/benchmarks
```

运行：

```sh
./build/bin/unittests --disable-prompt --iterations 1
./build/bin/unittests --help
./build/bin/unittests --disable-prompt --run test_threaded --iterations 1
./build/bin/fuzztests
./build/bin/benchmarks
```

也可使用包装目标：`make -C tests/unittests run`、`make -C tests/fuzztests run`、`make -C benchmarks run`。具体目标来自 `build/makefile` 和各目录 makefile；本轮未执行这些包装目标。

## CMake

```sh
cmake -S . -B cmake-build
cmake --build cmake-build
cmake --install cmake-build --prefix "$PWD/stage"
```

CMake 只安装 header 和导出的 interface target `concurrentqueue::concurrentqueue`，不定义 unit/fuzz/benchmark target。[../../../CMakeLists.txt:1-16](../../../CMakeLists.txt#L1-L16)

## CI 平台

CI 包含 native Linux 和 RISC-V Linux 交叉编译，RISC-V 使用 `qemu-riscv64-static -L /usr/riscv64-linux-gnu` 运行 unit tests。CI 的成功不能推导为当前工作站已具备相同工具链；目标环境必须单独确认。

## 验证状态

| 命令/目标 | 源码存在 | 本次执行 |
|---|---:|---:|
| Make tests | 是 | 成功（2026-09-10） |
| unit smoke | 是 | 全部通过（1 iteration） |
| 单个 `--run` 测试 | 是 | `test_threaded` 通过 |
| fuzz | 是 | 仅构建，未运行 |
| benchmark | 是 | 未验证 |
| CMake configure/build/install | 是 | 未执行（权限被拒绝） |
| RISC-V/QEMU | CI 有 | 未验证 |

## 常见误读

- CMake 成功只证明 interface/export 配置可生成，不证明 legacy 测试构建成功。
- `--iterations 1` 只控制测试程序运行次数，不是测试覆盖率指标。
- benchmark 输出是测量结果，不是正确性证明。

## 下一步

运行命令后应把实际命令、编译器、退出码和失败输出补入 [analysis-state](analysis-state.md)，不得以预期结果替代日志。
