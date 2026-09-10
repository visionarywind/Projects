# 测试配方

- 文档目的：为不同风险选择最小而充分的验证组合。
- 适用范围：源码、格式、并发和工程改动。
- 源码版本：`main` / `7ee830d`。
- 证据状态：测试入口已确认；运行结果未在文档生成阶段预设。
- 最后更新：2026-09-10
- 前置阅读：[M07 testing](../01-modules/M07-tests-build/testing.md)
- 后续阅读：[性能指南](performance-guide.md)

## 局部改动

```bash
cmake --build build --target leveldb_tests
ctest --test-dir build -R leveldb_tests --output-on-failure
```

目标名需以当前 CMake 生成结果确认。

## 格式/恢复改动

运行对应 `version_edit_test`、`log_test`、`table_test`、`recovery_test`，再做关闭重开、截断/CRC/坏 footer 和旧文件测试。

## 并发/生命周期改动

运行 DB/iterator/snapshot/cache 测试，使用 Debug/ASan/TSan（若工具链允许），重复后台 compaction 和 shutdown。

## Env/故障改动

运行 `env_posix_test`/Windows 对应测试、MemEnv、fault injection；验证 Sync 前后文件状态，不把测试替身结果直接等同生产文件系统。

## 全量验收

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
ctest --test-dir build --output-on-failure
```

CI 还会运行 benchmark 和 install；修改性能或公共安装接口时纳入同等步骤。[.github/workflows/build.yml:80-102](../../../.github/workflows/build.yml#L80-L102)

## 报告格式

记录提交、编译器、构建类型、依赖探测、命令、退出码、测试数、失败输出、是否重开/故障/并发、未执行项目和原因。
