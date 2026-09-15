# M05 测试分析

- 文档目的：列出表格式、块编码和迭代器的回归策略。
- 适用范围：M05。
- 对应源码版本：source/leveldb HEAD 7ee830d（2026-09-15 只读确认）。
- 证据状态：测试文件和夹具已确认；本轮执行状态未验证。
- 最后更新：2026-09-10
- 前置阅读：[M05 examples](examples.md)
- 后续阅读：[M07 testing](../M07-tests-build/testing.md)
## 结论摘要

本页聚焦 01-modules/M05-sstable-table/testing.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


- 源码版本：`main` / `7ee830d`。

## 已有测试入口

| 主题 | 文件/夹具 |
|---|---|
| BlockBuilder/Block 编解码 | `table/table_test.cc` 的 `BlockConstructor` |
| TableBuilder/Table 读写 | `table/table_test.cc` 的 `TableConstructor` |
| 自定义 comparator | `ReverseKeyComparator` |
| filter block | `table/filter_block_test.cc` |
| block cache / DB 集成 | `db/db_test.cc`、`db/table_cache.cc` 使用路径 |

`table/table_test.cc` 的构造器把相同的有序 map 输入分别送入 Block 和 Table 实现，再通过 Iterator 比较结果，适合发现编码/解码不对称。[table/table_test.cc:135-199](../../../source/leveldb/table/table_test.cc#L135-L199)

## 必测矩阵

1. 空表、单 entry、多 entry。
2. 长共享前缀和大 value，覆盖 varint 多字节路径。
3. `block_restart_interval` 为 1、默认值和较大值。
4. block 边界前后 Seek、Next、Prev、SeekToLast。
5. bytewise 和非字节序 comparator。
6. no compression、Snappy/Zstd 可用和压缩收益不足路径。
7. filter 命中、未命中、空 filter 和损坏 filter。
8. checksum 开启时的 trailer/CRC 损坏。
9. cache 命中、驱逐、iterator 尚未销毁时的句柄保护。
10. TableCache 找不到文件、旧 `.sst` 文件名回退和 transient error 重试。

## 回归要求

改变持久化格式时必须补新旧格式兼容/损坏测试；改变 `BlockHandle` 或 footer 时必须覆盖文件尾部解析；改变 cleanup 时必须运行 iterator 生命周期和 cache eviction 测试。仅通过“能读回数据”不足以证明资源释放正确。

## 当前验证状态

构建和 CTest 尚未在本轮文档生成过程中执行；不要把上述测试文件的存在误写成测试已通过。

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
