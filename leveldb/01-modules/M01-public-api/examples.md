# M01 示例

- 文档目的：提供来自仓库文档的 API 使用骨架。
- 适用范围：M01。
- 源码版本：`main` / `7ee830d`。
- 证据状态：示例来自 `doc/index.md`；本轮未执行。
- 最后更新：2026-09-10
- 前置阅读：[M01 interfaces](interfaces.md)
- 后续阅读：[M01 testing](testing.md)

## 打开与读写

```cpp
leveldb::DB* db = nullptr;
leveldb::Options options;
options.create_if_missing = true;
leveldb::Status s = leveldb::DB::Open(options, name, &db);
if (s.ok()) {
  s = db->Put(leveldb::WriteOptions(), key, value);
  std::string result;
  s = db->Get(leveldb::ReadOptions(), key, &result);
  delete db;
}
```

Open、Status 检查和 delete DB 的真实文档位置为 [doc/index.md:11-53](../../../../doc/index.md#L11-L53)。示例中的 `Slice`/string 必须满足调用期间的存活要求。

## WriteBatch

```cpp
leveldb::WriteBatch batch;
batch.Put("a", "1");
batch.Delete("b");
leveldb::Status s = db->Write(leveldb::WriteOptions(), &batch);
```

WriteBatch 将多个修改作为一个写操作提交；编码和插入路径见 M03。

## Snapshot 和 Iterator

```cpp
const leveldb::Snapshot* snap = db->GetSnapshot();
leveldb::ReadOptions ro;
ro.snapshot = snap;
leveldb::Iterator* it = db->NewIterator(ro);
it->SeekToFirst();
delete it;
db->ReleaseSnapshot(snap);
```

Snapshot 和 Iterator 的生命周期示例见 [doc/index.md:143-199](../../../../doc/index.md#L143-L199)。
