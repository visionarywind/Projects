# M04 示例

- 文档目的：用重开、压缩和版本引用场景连接 VersionSet、MANIFEST 与 SSTable。
- 适用范围：M04。
- 对应源码版本：source/leveldb HEAD 7ee830d（2026-09-15 只读确认）。
- 证据状态：核心调用入口和版本/压缩机制来自源码；Linux Debug 聚合构建与 CTest 已通过，但 MANIFEST 损坏、输出提交窗口和独立 compaction filter 尚未执行。
- 最后更新：2026-09-10
- 前置阅读：[M04 README](README.md)
- 后续阅读：[M04 testing](testing.md)
## 结论摘要

本页聚焦 01-modules/M04-version-compaction/examples.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


- 源码版本：`main` / `7ee830d`。

## MANIFEST 与重开

创建数据库并写入数据，关闭后用相同 name 重开，检查 `CURRENT` 指向的 MANIFEST 能重建相同的 Version。VersionEdit 记录新增/删除文件、日志号和 sequence 等增量；恢复入口为 [`VersionSet::Recover`](../../../source/leveldb/db/version_set.cc#L861-L993)，安装新版本入口为 [`VersionSet::LogAndApply`](../../../source/leveldb/db/version_set.cc#L777-L859)。

验证时应同时检查正常重开和 MANIFEST 损坏的错误状态，避免把“文件存在”误当作“版本元数据已完整恢复”。

## 手动与自动 compaction

先用较小的 write buffer 产生多个 Level-0 表，再调用测试夹具提供的 compact 入口或等待已有后台调度完成，检查输出文件、读取结果和 obsolete 文件清理。压缩选择入口为 [`VersionSet::PickCompaction`](../../../source/leveldb/db/version_set.cc#L1252-L1304)，输入扩展入口为 [`SetupOtherInputs`](../../../source/leveldb/db/version_set.cc#L1385-L1446)。

Level-0 文件可能重叠，因此测试应使用相邻 key、重叠 key 和跨边界 key，验证压缩不会丢失可见版本，也不要用固定 sleep 作为唯一同步机制。

## 多版本与 Snapshot

写入同一 user key 的多个版本，在不同 Snapshot sequence 下读取，验证 Version/TableCache 读取路径遵循 InternalKey 顺序并返回各自可见值。保持 iterator 或 snapshot 存活期间，相关 Version 和表文件不能被提前释放；对应 Version 生命周期由 [`Version`](../../../source/leveldb/db/version_set.h#L59-L164) 的引用管理保护。

测试结束时显式释放 iterator、snapshot 和 DB，确认旧版本的引用归还后才允许清理不再 live 的表文件。

## 输出文件保护

在 compaction 生成新 SSTable 但尚未完成 VersionEdit 安装的窗口注入错误，检查 `pending_outputs_` 不会让后台清理误删仍在使用的输出文件；最终失败路径应返回 `Status` 并清理临时资源。具体协调逻辑位于 [`DBImpl::WriteLevel0Table`](../../../source/leveldb/db/db_impl.cc#L505-L548) 和 [`VersionSet::LogAndApply`](../../../source/leveldb/db/version_set.cc#L777-L859)。

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
