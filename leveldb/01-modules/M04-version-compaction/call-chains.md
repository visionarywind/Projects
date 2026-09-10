# M04 调用链

- 文档目的：追踪恢复、版本提交、选压缩和执行。
- 适用范围：M04。
- 证据状态：已确认。
- 最后更新：2026-09-10
- 前置阅读：[M04 README](README.md)
- 后续阅读：[M05 call-chains](../M05-sstable-table/call-chains.md)

## 恢复

```text
VersionSet::Recover
  -> Read CURRENT
  -> log::Reader::ReadRecord(MANIFEST)
  -> VersionEdit::DecodeFrom
  -> Builder::Apply
  -> AppendVersion
```

[db/version_set.cc:861-993](../../../../db/version_set.cc#L861-L993)

## 提交

```text
DBImpl::InstallCompactionResults
  -> VersionSet::LogAndApply
    -> Builder.Apply/SaveTo
    -> descriptor_log_->AddRecord
    -> Sync/CURRENT
    -> AppendVersion
```

## 选压缩

```text
VersionSet::PickCompaction
  -> size score 或 seek file
  -> Level-0 overlap 收集
  -> SetupOtherInputs
    -> boundary/grandparent inputs
```

[db/version_set.cc:1252-1446](../../../../db/version_set.cc#L1252-L1446)

## 相关文档

- [diagrams](diagrams.md)
- [line-level-analysis](line-level-analysis.md)

## 源码证据摘要

见各链。

## 未解决问题

`DoCompactionWork` 的版本丢弃判定在 M02 执行，需联合阅读。

## 下一步阅读建议

对照 M02 后台链和 M05 TableBuilder。
