# 快速开始

- 文档目的：让新开发者从源码得到可复现的构建、测试和阅读入口。
- 适用范围：LevelDB `main` / `7ee830d`。
- 对应源码版本：source/leveldb HEAD 7ee830d（2026-09-15 只读确认）。
- 证据状态：Debug configure/build/CTest、小规模 benchmark 和本地 prefix install 已在本机执行；Release、Sanitizer、跨平台未验证。
- 最后更新：2026-09-10
- 前置阅读：[知识库入口](../README.md)
- 后续阅读：[阅读指南](reading-guide.md)
## 结论摘要

本页聚焦 99-roadmap/quick-start.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。

## 获取源码

```bash
git clone --recurse-submodules https://github.com/google/leveldb.git
```

来源：[README.md:33-37](../../source/leveldb/README.md#L33-L37)。本知识库目标版本是当前工作树已确认的 `7ee830d02b623e8ffe0b95d59a74db1e58da04c5`，不是自动假设的最新提交。

若已有仓库但子模块未初始化，执行：

```bash
git submodule update --init --recursive
```

本轮已验证该命令成功检出 `third_party/googletest` 和 `third_party/benchmark`。

## POSIX Debug 构建

仓库 README 的 Release 风格快速命令：

```bash
mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE=Release .. && cmake --build .
```

来源：[README.md:39-50](../../source/leveldb/README.md#L39-L50)。本知识库已验证的 Debug 命令为：

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j2
ctest --test-dir build --output-on-failure
```

本机结果：configure 成功，build 成功，CTest 3/3 全部通过。

```text
Test #1: leveldb_tests   Passed   88.73 sec
Test #2: c_test          Passed    0.35 sec
Test #3: env_posix_test  Passed    2.16 sec
100% tests passed, 0 tests failed out of 3
```

## 构建选项

- `-DLEVELDB_BUILD_TESTS=ON|OFF`
- `-DLEVELDB_BUILD_BENCHMARKS=ON|OFF`
- `-DLEVELDB_INSTALL=ON|OFF`
- `-DBUILD_SHARED_LIBS=ON|OFF`

默认值见 [CMakeLists.txt:32-34](../../source/leveldb/CMakeLists.txt#L32-L34)。可选 crc32c/Snappy/Zstd/tcmalloc 由配置探测决定。[CMakeLists.txt:36-53](../../source/leveldb/CMakeLists.txt#L36-L53)

## 测试、benchmark 和本地安装

```bash
ctest --test-dir build --output-on-failure
./build/db_bench --benchmarks=fillseq,readrandom --num=10000 --value_size=100 --threads=1 --db=/tmp/leveldb-knowledge-bench-20260910-r2
cmake --install build --prefix build/install
```

本轮 benchmark 结果：

```text
fillseq    : 3.114 micros/op; 35.5 MB/s
readrandom : 1.199 micros/op; (10000 of 10000 found)
```

这是 Debug 构建可运行性验证，不是性能基线；输出提示优化关闭、断言开启且 Snappy 未启用。更完整的 benchmark 应按 [性能指南](performance-guide.md) 使用 Release/RelWithDebInfo 和固定实验条件。

本轮 install 使用 `--prefix build/install`，只写入仓库内 build 目录，已成功安装 `libleveldb.a`、公共头文件和 CMake package 文件。

## 第一次运行建议

先阅读 `docs/project-understanding/README.md`，然后运行/查看 `ctest -N` 了解目标，再选择 `db/db_test.cc`、`db/version_set_test.cc` 或 `table/table_test.cc`。不要直接依赖 `db/` 或 `table/` 内部头文件开发应用。

## 相关文档

- [构建、运行与部署总览](../00-overview/build-and-deploy.md)
- [阅读指南](reading-guide.md)
- [测试配方](testing-recipes.md)
- [性能指南](performance-guide.md)

## 源码证据摘要

- [README 构建命令](../../source/leveldb/README.md#L33-L50)
- [CMake 默认选项](../../source/leveldb/CMakeLists.txt#L32-L53)
- [测试与 benchmark 注册](../../source/leveldb/CMakeLists.txt#L292-L469)

## 未解决问题

Release、Sanitizer、跨平台和 fault injection 未在本轮执行；Debug benchmark 不能代表真实性能。

## 下一步阅读建议

先确认 Debug 构建和 CTest 通过，再按 [端到端深度链路](../90-cross-module/end-to-end-traces.md) 追踪一次 Open/Write/Get/flush/reopen。
