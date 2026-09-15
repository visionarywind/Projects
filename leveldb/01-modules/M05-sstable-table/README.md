# M05 SSTable、Block 与迭代器

- 文档目的：解释不可变表文件的生成、布局、校验和读取路径。
- 适用范围：`table/`、`include/leveldb/table.h`、`db/table_cache.*`。
- 对应源码版本：source/leveldb HEAD 7ee830d（2026-09-15 只读确认）。
- 证据状态：核心机制已确认；性能结论需基准验证。
- 最后更新：2026-09-10
- 前置阅读：[M04 Version/Compaction](../M04-version-compaction/README.md)
- 后续阅读：[M06 Env](../M06-env-platform/README.md)

- 源码版本：`main`，HEAD `7ee830d`（LevelDB 1.23 系列）。

## 结论摘要

M05 将有序键值序列编码为不可变 SSTable：`TableBuilder` 依次写 data block、filter/metaindex、index block 和 footer；`Table::Open` 先验证 footer 并加载 index；读取时先定位 index，再按 `BlockHandle` 读取 data block，必要时使用 filter 和 block cache。表对象及其迭代器通过 cleanup 回调释放文件、缓存句柄和 block。

## 关键入口

- 生成：[table/table_builder.cc:93-122](../../../source/leveldb/table/table_builder.cc#L93-L122)、[table/table_builder.cc:212-277](../../../source/leveldb/table/table_builder.cc#L212-L277)
- 打开：[table/table.cc:37-78](../../../source/leveldb/table/table.cc#L37-L78)
- 单键读取：[table/table.cc:213-241](../../../source/leveldb/table/table.cc#L213-L241)
- 迭代器：[table/table.cc:207-211](../../../source/leveldb/table/table.cc#L207-L211)
- 缓存入口：[db/table_cache.cc:40-111](../../../source/leveldb/db/table_cache.cc#L40-L111)

## 阅读顺序

1. `format.h/.cc`：BlockHandle、Footer、块 trailer 和校验。
2. `block_builder.cc` 与 `block.cc`：前缀压缩和 restart 搜索。
3. `table_builder.cc`：把各类 block 组装成文件。
4. `table.cc`、`two_level_iterator.cc`：按索引惰性读取。
5. `table_cache.cc`：文件和 Table 的缓存生命周期。

## 相关文档

- [implementation](implementation.md)
- [source-map](source-map.md)
- [interfaces](interfaces.md)
- [data-structures](data-structures.md)
- [call-chains](call-chains.md)
- [diagrams](diagrams.md)
- [line-level-analysis](line-level-analysis.md)
- [examples](examples.md)
- [testing](testing.md)
- [development-guide](development-guide.md)
- [risks-and-debt](risks-and-debt.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](../../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题

不同压缩算法、block size、filter policy 对实际读放大和缓存命中率的影响尚未在本机 benchmark 中验证。

## 下一步阅读建议

先用 `table/table_test.cc` 观察 Block/Table 构造夹具，再回到 `Version::Get` 理解表文件如何进入全局读路径。

## 文档元数据（规范补充）

- 文档目的：说明 `01-modules/M05-sstable-table/README.md` 的源码分析范围、结论和维护入口。
- 适用范围：当前项目对应模块/入口的静态源码与测试分析。
- 对应源码版本：以本项目 `00-overview/analysis-state.md` 或同页版本字段为准。
- 证据状态：静态源码证据；未执行的构建、测试、GPU、网络或多进程行为保持“未验证”。
- 最后更新：2026-09-15
- 前置阅读：本项目根 README 与 `00-overview/analysis-state.md`。
- 后续阅读：本模块/示例的实现、测试和风险页面。

## 深度审计

| 分析对象 | 入口落地 | 正常路径 | 分支 | 异常 | 清理 | 数据生命周期 | 执行上下文 | 行级证据 | Demo 映射 | 状态/缺口 |
|---|---|---|---|---|---|---|---|---|---|---|
| `01-modules/M05-sstable-table/README.md` | 已完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 已映射或不适用 | 部分完成：动态行为、边界或专用变体仍需验证 |
