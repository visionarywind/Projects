# D01 运行与构建

- 文档目的：解释 80-demos/D01-dbtest-lifecycle/build-and-run.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/leveldb HEAD 7ee830d（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 80-demos/D01-dbtest-lifecycle/build-and-run.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


## 1. 构建目标来源

`CMakeLists.txt` 把 `db/db_test.cc` 和恢复、日志、版本、表等测试源加入同一个 `leveldb_tests` 可执行文件；CTest 注册同名测试目标。[`CMakeLists.txt:313-365`](../../../source/leveldb/CMakeLists.txt#L313-L365)

因此 D01 的“运行对象”是既有测试二进制，不是新增源文件：

```text
leveldb_tests
  ├─ DBTest fixture / db/db_test.cc
  ├─ recovery_test.cc / log_test.cc / version_set_test.cc
  ├─ table/filter tests
  └─ util / memenv tests
```

## 2. 已验证的全量命令

本机已在用户授权后执行：

```bash
git submodule update --init --recursive
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j2
ctest --test-dir build --output-on-failure
```

结果：Debug configure/build 成功；本轮 CTest 的 `leveldb_tests`、`c_test`、`env_posix_test` 共 3/3 通过，耗时分别为 88.73 s、0.35 s、2.16 s，总计 91.24 s。该结果证明聚合测试目标可运行，但不等于下面每一个 D01 子场景都被单独记录过。

## 3. 已验证的有限 benchmark

本轮在同一 Debug 构建下运行：

```bash
./build/db_bench --benchmarks=fillseq,readrandom \
  --num=10000 --value_size=100 --threads=1 \
  --db=/tmp/leveldb-knowledge-bench-20260910-r2
```

结果：`fillseq` 为 3.114 micros/op（35.5 MB/s）；`readrandom` 为 1.199 micros/op，10000/10000 命中。输出同时提示优化关闭、断言开启、Snappy 未启用，因此这只是入口可运行性证据，不是生产性能基线。

## 4. 运行 D01 相关测试

先列出 GoogleTest 名称：

```bash
./build/leveldb_tests --gtest_list_tests | grep -E 'DBTest\.|DBTest'
```

按链路分组运行：

```bash
# 基本写读、删除、Snapshot、Iterator
./build/leveldb_tests --gtest_filter='DBTest.ReadWrite:DBTest.PutDeleteGet:DBTest.GetSnapshot:DBTest.Snapshot:DBTest.IterMulti:DBTest.IteratorPinsRef'

# flush、reopen、WAL 恢复
./build/leveldb_tests --gtest_filter='DBTest.GetFromImmutableLayer:DBTest.GetFromVersions:DBTest.Recover:DBTest.RecoverDuringMemtableCompaction:DBTest.RecoverWithLargeLog'

# compaction、删除标记和读取层级
./build/leveldb_tests --gtest_filter='DBTest.CompactionsGenerateMultipleFiles:DBTest.DeletionMarkers1:DBTest.DeletionMarkers2:DBTest.GetLevel0Ordering'

# Env 注入的错误边界
./build/leveldb_tests --gtest_filter='DBTest.WriteSyncError:DBTest.ManifestWriteError:DBTest.LogCloseError:DBTest.MissingSSTFile'
```

过滤器名称必须以当前二进制的 `--gtest_list_tests` 输出为准；若测试名因依赖版本或构建选项不同而变化，不能凭本页猜测。

## 5. 运行时目录与清理

`DBTest` 使用 `testing::TempDir() + "db_test"`，构造时先 `DestroyDB`，析构时删除 DB、测试 Env 和 Bloom filter。不要把生产数据库路径传给这些测试，也不要在测试运行中手动删除目录。[`db/db_test.cc:260-281`](../../../source/leveldb/db/db_test.cc#L260-L281)

`SpecialEnv` 包装 `Env::Default()`，可模拟错误而不修改源码；每个测试应在结束前恢复原子开关，fixture 析构会删除 DB。[`db/db_test.cc:83-258`](../../../source/leveldb/db/db_test.cc#L83-L258)

## 6. 执行记录格式

每次单独实验记录：

```text
commit: 7ee830d
build: Debug / compiler / optional libraries
command: 完整命令
filter: gtest filter
exit code: 0 或实际值
observed: Status、Get 结果、文件层级/日志
not observed: 未检查的后台时序、断电语义或性能
```

当前知识库已确认 Debug configure/build、全量 CTest 和有限 benchmark 结果；本 D01 子命令的单独耗时和逐项输出尚未作为独立实验记录。

## 相关证据

- [M07 构建总览](../../01-modules/M07-tests-build/README.md)
- [测试配方](../../99-roadmap/testing-recipes.md)
- [`DBTest` fixture](../../../source/leveldb/db/db_test.cc#L260-L365)
- [`leveldb_tests` 注册](../../../source/leveldb/CMakeLists.txt#L313-L365)

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
