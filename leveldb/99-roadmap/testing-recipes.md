# 测试配方

- 文档目的：为不同风险选择最小而充分的验证组合。
- 适用范围：源码、格式、并发和工程改动。
- 对应源码版本：source/leveldb HEAD 7ee830d（2026-09-15 只读确认）。
- 证据状态：测试入口已确认；运行结果未在文档生成阶段预设。
- 最后更新：2026-09-10
- 前置阅读：[M07 testing](../01-modules/M07-tests-build/testing.md)
- 后续阅读：[性能指南](performance-guide.md)
## 结论摘要

本页聚焦 99-roadmap/testing-recipes.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


- 源码版本：`main` / `7ee830d`。

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

CI 还会运行 benchmark 和 install；修改性能或公共安装接口时纳入同等步骤。[.github/workflows/build.yml:80-102](../../source/leveldb/.github/workflows/build.yml#L80-L102)

## 报告格式

记录提交、编译器、构建类型、依赖探测、命令、退出码、测试数、失败输出、是否重开/故障/并发、未执行项目和原因。

## 相关文档
- [项目入口](../README.md)
- [分析状态](../00-overview/analysis-state.md)
- [源码证据索引](../00-overview/evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](../00-overview/analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
