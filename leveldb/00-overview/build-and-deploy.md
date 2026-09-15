# 构建、运行与部署总览

- 文档目的：把仓库真实构建/测试入口整理成可执行配方，并记录本机验证结果。
- 适用范围：目标提交 `7ee830d02b623e8ffe0b95d59a74db1e58da04c5`；本机 Linux Debug 构建。
- 对应源码版本：source/leveldb HEAD 7ee830d（2026-09-15 只读确认）。
- 证据状态：命令来源和本机执行结果已确认；Release、Sanitizer、跨平台和故障注入未验证。
- 最后更新：2026-09-10
- 前置阅读：[项目概览](project-overview.md)
- 后续阅读：[快速上手](../99-roadmap/quick-start.md)
## 结论摘要

项目使用 CMake 3.22+，默认构建 LevelDB、`leveldbutil`、测试和 benchmark；测试由 CTest 注册。本轮在获得授权后初始化 `third_party/googletest` 与 `third_party/benchmark` 子模块，Debug configure/build/CTest、小规模 `db_bench` 和本地 prefix install 均已执行成功。

CI 在 Ubuntu/macOS/Windows 上用 Debug/RelWithDebInfo 组合运行构建、测试、benchmark 和 install；本机只验证 Linux Debug 配置，不代表其他平台或 Release 性能。

## 配置与构建

```bash
git submodule update --init --recursive
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j2
ctest --test-dir build --output-on-failure
./build/db_bench --benchmarks=fillseq,readrandom --num=10000 --value_size=100 --threads=1 --db=/tmp/leveldb-knowledge-bench
cmake --install build --prefix build/install
```

CMake 默认选项：`LEVELDB_BUILD_TESTS=ON`、`LEVELDB_BUILD_BENCHMARKS=ON`、`LEVELDB_INSTALL=ON`（[CMakeLists.txt:32-34](../../source/leveldb/CMakeLists.txt#L32-L34)）。可选库由 feature detection 决定（[CMakeLists.txt:39-53](../../source/leveldb/CMakeLists.txt#L39-L53)）。

## 本机验证结果

### 子模块

已执行：

```text
git submodule update --init --recursive
```

结果：成功。检出：

- `third_party/benchmark`：`1a54956777ba672764db09a51960056ea042af7e`
- `third_party/googletest`：`a35bc7693c117a048152beeb34f6aac354b9423f`

### CMake configure

已执行：

```text
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
```

结果：成功。关键探测结果包括：

- Google Benchmark：`v1.9.5-24-g1a549567`；
- `HAVE_STD_REGEX`：success；
- `HAVE_POSIX_REGEX`：success；
- `HAVE_STEADY_CLOCK`：success；
- `HAVE_PTHREAD_AFFINITY`：success；
- `HAVE_CXX_FLAG_WSHORTEN_64_TO_32`：failed；
- `HAVE_CXX_FLAG_WTHREAD_SAFETY`：failed。

此前 configure 失败的原因是子模块未初始化；本轮已通过初始化子模块解决。

### Build

已执行：

```text
cmake --build build -j2
```

结果：成功，构建到 100%。生成/构建成功的关键目标包括：

- `leveldb`；
- `leveldbutil`；
- `leveldb_tests`；
- `c_test`；
- `env_posix_test`；
- `db_bench`；
- GoogleTest/GoogleMock；
- Google Benchmark。

### CTest

已执行：

```text
ctest --test-dir build --output-on-failure
```

结果：全部通过。

```text
100% tests passed, 0 tests failed out of 3

Test #1: leveldb_tests   Passed   88.73 sec
Test #2: c_test          Passed    0.35 sec
Test #3: env_posix_test  Passed    2.16 sec
Total Test time (real) = 91.24 sec
```

### Benchmark

已执行小规模可运行性 benchmark：

```text
./build/db_bench --benchmarks=fillseq,readrandom --num=10000 --value_size=100 --threads=1 --db=/tmp/leveldb-knowledge-bench-20260910-r2
```

结果：成功。

```text
LevelDB: version 1.23
CPU: 12 * 12th Gen Intel(R) Core(TM) i5-12400
Entries: 10000
RawSize: 1.1 MB (estimated)
FileSize: 0.6 MB (estimated)
fillseq    : 3.114 micros/op; 35.5 MB/s
readrandom : 1.199 micros/op; (10000 of 10000 found)
```

注意：该结果只证明 benchmark 在当前环境可运行，不应作为性能结论。输出明确提示：

```text
WARNING: Optimization is disabled: benchmarks unnecessarily slow
WARNING: Assertions are enabled; benchmarks unnecessarily slow
WARNING: Snappy compression is not enabled
```

也就是说，这是 Debug 构建、断言开启且 Snappy 未启用的结果。

### Install

已执行本地 prefix install，避免写入系统目录：

```text
cmake --install build --prefix build/install
```

结果：成功。安装内容包括：

- `build/install/lib/libleveldb.a`；
- `build/install/include/leveldb/*.h`；
- `build/install/lib/cmake/leveldb/*`；
- GoogleTest/GoogleMock/Benchmark 的本地安装产物。

## 构建目标说明

- `leveldb`：核心库，包含 `db/`、`table/`、`util/`、`helpers/memenv/`。
- `leveldbutil`：数据库工具，入口 `db/leveldbutil.cc`。
- `leveldb_tests`：主要 GoogleTest 聚合目标。
- `c_test`：C API 测试。
- `env_posix_test`：POSIX Env 测试。
- `db_bench`：benchmark 入口。

## CI 事实

CI checkout 子模块，Linux 安装 Snappy/SQLite/KyotoCabinet，调用 CMake configure/build、`ctest --verbose`、benchmark 和 install（[.github/workflows/build.yml:61-102](../../source/leveldb/.github/workflows/build.yml#L61-L102)）。本地没有这些库时，部分 benchmark 能力或压缩路径可能不同；这不是核心库构建失败。

## 部署边界

LevelDB 是库而非服务；“部署”通常是安装库和头文件到应用构建环境。安装规则在 [CMakeLists.txt:471-519](../../source/leveldb/CMakeLists.txt#L471-L519)。数据库目录应由应用管理备份、权限、磁盘容量和恢复策略。

本轮 install 只写入仓库内 `build/install`，不是系统级安装；如果要安装到 `/usr/local` 或打包发布，应另行确认 prefix、权限和清理策略。

## 相关文档

- [快速上手](../99-roadmap/quick-start.md)
- [测试配方](../99-roadmap/testing-recipes.md)
- [性能指南](../99-roadmap/performance-guide.md)
- [CI 工作流](../../source/leveldb/.github/workflows/build.yml)

## 源码证据摘要

- [CMake 选项](../../source/leveldb/CMakeLists.txt#L32-L53)
- [测试和 benchmark 注册](../../source/leveldb/CMakeLists.txt#L292-L469)
- [安装规则](../../source/leveldb/CMakeLists.txt#L471-L519)
- [CI 命令](../../source/leveldb/.github/workflows/build.yml#L61-L102)

## 未解决问题

- Release、RelWithDebInfo、Sanitizer、TSan/ASan、覆盖率和 fault injection 未执行。
- Windows/macOS、Snappy/Zstd/tcmalloc/SQLite/KyotoCabinet 等依赖组合未在本机验证。
- Debug benchmark 结果不能代表真实性能；需 Release 构建、固定环境和更完整 workload。

## 下一步阅读建议

若目标是开发，先使用本页 Debug 命令确认可构建和通过 CTest；若目标是性能，改用 Release/RelWithDebInfo、启用目标依赖并按 [性能指南](../99-roadmap/performance-guide.md) 记录完整实验条件。
