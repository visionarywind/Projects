# M05 接口与契约

- 文档目的：记录表、块和迭代器的输入输出及所有权。
- 适用范围：M05。
- 对应源码版本：source/leveldb HEAD 7ee830d（2026-09-15 只读确认）。
- 证据状态：公共契约已确认；内部函数契约由实现推导。
- 最后更新：2026-09-10
- 前置阅读：[M05 design](design.md)
- 后续阅读：[M05 data structures](data-structures.md)
## 结论摘要

本页聚焦 01-modules/M05-sstable-table/interfaces.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


- 源码版本：`main` / `7ee830d`。

## 公共接口

### `Table::Open`

输入 `Options`、文件指针和文件大小；成功返回新 Table，调用方负责 delete Table，并保证文件在 Table 存活期间有效；失败时 `*table == nullptr`。[include/leveldb/table.h:27-40](../../../source/leveldb/include/leveldb/table.h#L27-L40)

### `Table::NewIterator`

返回初始无效的 Iterator；调用者必须先 `Seek*`。迭代器由调用者 delete，内部可能持有缓存句柄或 block。[include/leveldb/table.h:47-50](../../../source/leveldb/include/leveldb/table.h#L47-L50)

### `TableBuilder`

`Add` 要求 key 按 comparator 严格递增；`Finish` 写完尾部并关闭 builder 状态；`Abandon` 放弃剩余内容。析构函数断言 caller 已经 Finish 或 Abandon，因此异常/错误路径必须显式收尾。[table/table_builder.cc:64-75](../../../source/leveldb/table/table_builder.cc#L64-L75)、[table/table_builder.cc:93-122](../../../source/leveldb/table/table_builder.cc#L93-L122)、[table/table_builder.cc:212-273](../../../source/leveldb/table/table_builder.cc#L212-L273)

## 内部契约

- `BlockHandle` 只描述 offset 和 stored size，不包含 trailer；读取时 `ReadBlock` 自动额外读取 5 字节 trailer。[table/format.h:20-43](../../../source/leveldb/table/format.h#L20-L43)、[table/format.cc:68-87](../../../source/leveldb/table/format.cc#L68-L87)
- `BlockContents::heap_allocated` 决定 Block 是否 delete 数据；`cachable` 决定是否可进入 block cache。[table/format.h:80-84](../../../source/leveldb/table/format.h#L80-L84)、[table/block.cc:24-45](../../../source/leveldb/table/block.cc#L24-L45)
- `FilterBlockReader` 不复制 policy 和 filter bytes；两者必须比 reader 活得久。[table/filter_block.h:51-63](../../../source/leveldb/table/filter_block.h#L51-L63)
- `NewTwoLevelIterator` 接管 index iterator，并在销毁时释放。[table/two_level_iterator.h:13-26](../../../source/leveldb/table/two_level_iterator.h#L13-L26)

## 错误与状态

格式错误、截断、CRC 不匹配、未知压缩类型都返回非 OK `Status`。[table/format.cc:84-97](../../../source/leveldb/table/format.cc#L84-L97)、[table/format.cc:119-160](../../../source/leveldb/table/format.cc#L119-L160) `Table::ReadMeta` 对非核心 metadata 读取错误不向上传播，而核心 footer/index 错误会使 Open 失败。[table/table.cc:54-78](../../../source/leveldb/table/table.cc#L54-L78)、[table/table.cc:81-107](../../../source/leveldb/table/table.cc#L81-L107)

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
