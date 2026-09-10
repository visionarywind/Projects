# M05 行级分析

- 文档目的：把关键代码区间映射到可修改的语义。
- 适用范围：`table/` 和 `db/table_cache.cc`。
- 源码版本：`main` / `7ee830d`。
- 证据状态：已确认。
- 最后更新：2026-09-10
- 前置阅读：[M05 source-map](source-map.md)
- 后续阅读：[M05 development-guide](development-guide.md)

| 行段 | 语义 | 修改注意 |
|---|---|---|
| `table_builder.cc:93-122` | 排序输入、写入 data/filter、按估算大小 flush | 不要改变 comparator 顺序不变量 |
| `table_builder.cc:124-138` | 写 data block 后延迟 index entry | pending handle 必须在下一个 key 到来时补入 |
| `table_builder.cc:140-207` | 压缩、block trailer、CRC、offset | handle size 与 trailer 范围必须一致 |
| `table_builder.cc:212-265` | 追加 filter/meta/index/footer | footer 必须保持文件最后且固定长度 |
| `block_builder.cc:70-104` | prefix compression 和 restart | key 必须递增；Finish 后不能 Add |
| `block.cc:24-45` | 校验 restart 数量和释放 data | `owned_` 决定 delete[] |
| `block.cc:54-73` | entry 边界检查 | 所有 varint 和长度都要防越界 |
| `block.cc:163-218` | restart 二分、区间线性 Seek | comparator 不能被 bytewise 假设替代 |
| `format.cc:68-101` | 读取 trailer、校验 CRC | verify_checksums 是 ReadOptions 语义 |
| `format.cc:101-160` | 解压和 BlockContents ownership | 失败路径释放压缩/解压 buffer |
| `table.cc:37-78` | footer/index 初始化 Table | 核心 index 错误必须返回 |
| `table.cc:81-131` | 读取可选 metadata/filter | metadata 错误当前不阻止读服务 |
| `table.cc:152-204` | block cache 和 iterator cleanup | cache handle 不能泄漏或过早 Release |
| `table.cc:213-240` | filter + index + data 单键读 | 过滤器不得产生假阴性 |
| `two_level_iterator.cc:145-159` | index 到 data 的惰性切换 | 保存 handle 副本以比较当前 block |
| `table_cache.cc:40-74` | 文件名回退与 Table 缓存 | 失败结果不缓存，修复文件后可重试 |

## 调试变量

关注 `TableBuilder::Rep::offset`、`pending_index_entry`、`BlockHandle` 的 offset/size、`BlockContents` 的 ownership flags、cache handle 生命周期，以及 iterator `status()`。[table/table_builder.cc:20-62](../../../../table/table_builder.cc#L20-L62)
