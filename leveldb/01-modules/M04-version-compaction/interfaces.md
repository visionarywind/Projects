# M04 接口与协议

- 文档目的：说明 VersionSet 内部接口和持久化编辑协议。
- 适用范围：M04。
- 证据状态：已确认。
- 最后更新：2026-09-10
- 前置阅读：[M04 design](design.md)
- 后续阅读：[M05 interfaces](../M05-sstable-table/interfaces.md)

## VersionEdit

可设置 comparator、log/prev-log、next-file、last-sequence、compact pointer，添加或删除 `(level,file)`；`EncodeTo/DecodeFrom` 是 MANIFEST/WAL 风格记录边界。[db/version_edit.h:28-100](../../../../db/version_edit.h#L28-L100)

## VersionSet

`LogAndApply` 要求调用者持有 mutex 且不允许并发调用；`Recover` 从 CURRENT 指向的 descriptor 重建 current；`PickCompaction` 返回堆分配 Compaction，调用者负责释放。[db/version_set.h:175-184](../../../../db/version_set.h#L175-L184)、[db/version_set.h:229-249](../../../../db/version_set.h#L229-L249)

## Compaction

输入是 level 与 level+1 的表集合，输出通过 `VersionEdit` 删除输入、添加输出；必须保持边界文件和 snapshot 可见性。

## 兼容性

MANIFEST 中 comparator name 必须匹配当前 comparator，否则 Recover 返回 InvalidArgument（[db/version_set.cc:909-920](../../../../db/version_set.cc#L909-L920)）。

## 相关文档

- [data-structures](data-structures.md)
- [global error model](../../00-overview/global-error-model.md)

## 源码证据摘要

见正文。

## 未解决问题

MANIFEST 记录格式的每个 tag 需结合 `version_edit.cc` 逐字段补表。

## 下一步阅读建议

阅读 `version_edit.cc` 的 Encode/Decode。
