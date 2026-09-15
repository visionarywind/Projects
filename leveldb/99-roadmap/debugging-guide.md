# 调试指南

- 文档目的：按症状定位 LevelDB 的跨模块边界。
- 适用范围：调试和故障分析。
- 源码版本：`main` / `7ee830d`。
- 证据状态：调试变量来自源码；工具命令需按环境验证。
- 最后更新：2026-09-10
- 前置阅读：[错误边界](../90-cross-module/error-boundaries.md)
- 后续阅读：[测试配方](testing-recipes.md)

## 先收集上下文

记录提交、构建类型、`HAVE_*` feature、Options（comparator、Env、cache、compression、sync）、数据库文件列表、线程、最后一个 Status 和相关 sequence/file number。

## 打不开数据库

1. 检查 LOCK 是否被其他进程持有。
2. 检查 CURRENT 指向的 MANIFEST 是否存在。
3. 从 `VersionSet::Recover` 看 MANIFEST record 解码和 comparator name。
4. 再看 WAL Reader 的截断/CRC/record 错误。
5. 用 `leveldbutil` 或复制数据库做只读复现，避免直接修改生产文件。

## Get 返回错误/不存在

1. 确认 snapshot sequence 和 comparator。
2. 分别检查 mem_、imm_、Version::Get。
3. 检查 TableCache 打开文件、footer/index、filter 是否短路。
4. 关闭 filter/checksum 只用于定位，不应作为最终修复。

## 数据未持久化/崩溃后丢失

检查 WriteOptions.sync、WAL Append/Flush/Sync、Env WritableFile 实现和故障注入 Env；不要把 OS buffer 中“写成功”当作崩溃持久性保证。

## compaction 不推进

记录 `compaction_score_`、`compaction_level_`、compact pointer、pending outputs、`bg_error_`、后台 scheduled 标志和是否存在 immutable MemTable。然后检查 VersionSet 选入范围和 DBImpl 安装结果。

## 资源/崩溃

优先使用 Debug + ASan/TSan（如环境支持）检查 Slice、Snapshot、Iterator cleanup、Cache handle、Arena 和后台关闭顺序。Cache 析构时仍有 unreleased handle 会触发断言，这是线索而非可忽略的噪声。[util/cache.cc:205-215](../../source/leveldb/util/cache.cc#L205-L215)

## 常用验证

```bash
ctest --test-dir build --output-on-failure
ctest --test-dir build -N
```

命令是否可用、具体目标名和结果必须以当前构建输出为准。
