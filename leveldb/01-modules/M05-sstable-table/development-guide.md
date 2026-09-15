# M05 开发指南

- 文档目的：指导安全修改 SSTable/Block/Iterator。
- 适用范围：M05。
- 对应源码版本：source/leveldb HEAD 7ee830d（2026-09-15 只读确认）。
- 证据状态：修改约束来自源码和测试；性能结论需实验。
- 最后更新：2026-09-10
- 前置阅读：[M05 line-level-analysis](line-level-analysis.md)
- 后续阅读：[跨模块影响](../../90-cross-module/change-impact-map.md)
## 结论摘要

本页聚焦 01-modules/M05-sstable-table/development-guide.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


- 源码版本：`main` / `7ee830d`。

## 修改配方

### 修改文件格式

1. 先更新 `format.h/.cc` 的编码/解码和错误检查。
2. 再更新 builder 和 reader 两端，保持 trailer、handle size、footer 长度一致。
3. 添加损坏、截断、checksum 和重开测试。
4. 检查旧表文件是否仍需读取；不要只验证新文件。

### 修改 block 编码

1. 保持 comparator 递增和 restart 不变量。
2. 同时修改 `BlockBuilder` 和 `Block::Iter`。
3. 覆盖长 varint、共享前缀、restart 边界、Prev/Seek。
4. 注意 `Slice` 指向 builder/block 内存，不能跨 Reset 或析构使用。[table/block_builder.h:26-37](../../../source/leveldb/table/block_builder.h#L26-L37)

### 修改压缩或过滤

1. 明确新的 compression type 是否改变持久化兼容性。
2. 在 `WriteBlock` 和 `ReadBlock` 两端增加对称分支。
3. 过滤器只能产生 false positive，不能 false negative。
4. 检查 block cache 的 `cachable` 与 heap ownership。

### 修改 iterator/cache

1. 追踪每个 cache handle 的 Lookup/Insert/Release。
2. 让 iterator cleanup 覆盖 block、TableCache entry 和 block cache handle。
3. 测试 iterator 销毁顺序、读错误和 cache eviction。
4. 不要把 Table 传入文件的借用指针误当成拥有指针。

## 跨模块检查

- M02/M04：`BuildTable` 输出文件的 file number、size 和 VersionEdit 安装。
- M06：RandomAccessFile/WritableFile 的短读、错误和线程语义。
- M07：CMake 是否把新测试加入目标和 CTest。

## 命令

仓库 README 提供的 CMake 构建命令见 [quick-start](../../99-roadmap/quick-start.md)；本页不把未执行命令标为成功。

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
