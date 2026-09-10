# M02 示例

- 文档目的：把 DBImpl 生命周期和测试夹具连接起来。
- 适用范围：M02。
- 源码版本：`main` / `7ee830d`。
- 证据状态：调用入口来自源码；命令未在本轮执行。
- 最后更新：2026-09-10
- 前置阅读：[M02 README](README.md)
- 后续阅读：[M02 testing](testing.md)

## Open/重开

使用 `Options` 和 `DB::Open` 创建数据库，执行写入后 delete DB，再用相同 name/Options 重开，验证 WAL/MANIFEST 恢复后的可见值。对应实现入口为 [`DB::Open`](../../../../db/db_impl.cc#L1503-L1546) 和 [`DBImpl::Recover`](../../../../db/db_impl.cc#L292-L383)。

## 后台 compaction

用较小 `write_buffer_size` 反复写入，观察 immutable MemTable、后台调度和表文件生成；测试应使用现有 `TEST_CompactMemTable`/`TEST_CompactRange` 或 `DBTest` fixture，而不是靠固定 sleep 猜测完成时间。

## 错误路径

用 MemEnv 或 fault injection Env 注入写入、Sync、读取失败，检查公开 Status、`bg_error_`、文件清理和下次 Open 的行为。故障注入测试的 Env 设计见 [`fault_injection_test.cc`](../../../../db/fault_injection_test.cc#L4-L6)。
