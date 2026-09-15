# 功能开发配方

- 文档目的：把常见功能/修复映射到代码、测试和验收。
- 适用范围：LevelDB 修改工作。
- 对应源码版本：source/leveldb HEAD 7ee830d（2026-09-15 只读确认）。
- 证据状态：影响路径来自源码和测试注册；具体 patch 仍需复核。
- 最后更新：2026-09-10
- 前置阅读：[修改影响地图](../90-cross-module/change-impact-map.md)
- 后续阅读：[测试配方](testing-recipes.md)
## 结论摘要

本页聚焦 99-roadmap/feature-development-recipes.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


- 源码版本：`main` / `7ee830d`。

## 修改公共 API

改 `include/leveldb`、实现/C API（如适用）、安装清单和文档；补成功/错误/所有权/并发测试；检查 ABI 和默认值。

## 修改 WAL/WriteBatch

同步修改编码、Reader/Writer、恢复和 MemTable 插入；补边界 fragment、CRC、截断、损坏、重开和旧数据测试。不要只测试正常完整记录。

## 修改 MemTable/快照

追踪 sequence、snapshot 可见性、引用计数、Arena 生命周期和删除标记；补多版本读、Iterator、compaction drop 规则测试。

## 修改 Version/Compaction

明确 VersionEdit 字段、MANIFEST/CURRENT 写入顺序、Version refs、pending outputs、live files；补重开、边界文件、Level-0 overlap、故障注入和并发压测。

## 修改 SSTable/Block

保持 builder/reader 格式对称，覆盖 comparator、restart、footer、CRC、压缩、filter、cache handle 和迭代器 cleanup。

## 修改 Env/平台

同步 POSIX、Windows、MemEnv、EnvWrapper 和测试；明确线程/文件 ownership；至少做故障注入和对应平台构建。

## 修改构建/测试

只在必要时改 CMake；确认目标注册、CTest、共享库条件、可选依赖和 install/export；同步 CI 文档。

## 交付前检查

源码行号、相对链接、构建、CTest、benchmark、重开/损坏/并发、Sanitizer（若可用）和未解决问题状态都要如实记录。

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
