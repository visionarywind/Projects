# 性能关键路径

- 文档目的：标记读写、压缩、缓存和系统调用的主要成本位置。
- 适用范围：全运行时。
- 源码版本：`main` / `7ee830d`。
- 证据状态：热点机制由源码确认；没有把旧 benchmark 数字当作当前基线。
- 最后更新：2026-09-10
- 前置阅读：[配置影响](configuration-impact-map.md)
- 后续阅读：[性能指南](../99-roadmap/performance-guide.md)

## 写入路径

writer 排队、WAL Append/Sync、WriteBatch 解析、MemTable Arena/SkipList 插入、immutable flush 和 compaction。关键配置为 `sync`、`write_buffer_size`、`max_file_size`、压缩。

## 读取路径

MemTable/immutable lookup、Version 文件选择、TableCache lookup、index block、filter、block cache、RandomAccessFile Read 和 comparator 比较。关键配置为 comparator、filter、block size、cache、max open files。

## 后台路径

TableBuilder 顺序写、压缩 CPU、MANIFEST Sync、compaction 输入输出和 obsolete file 删除；后台吞吐受 Env 调度和磁盘带宽影响。

## 基准方法

`db_bench` 提供 fillseq/fillrandom/fillsync/readrandom/readseq/readreverse/compact 和压缩场景。[benchmarks/db_bench.cc:23-64](../../../benchmarks/db_bench.cc#L23-L64) 修改后固定数据量、value size、线程数、compression、block/write buffer 参数，重复运行并记录硬件/构建配置。

## 不可直接断言

源码不能证明某配置在所有负载上更快；读放大、写放大、cache 命中和 Sync 持久性收益必须通过实际 benchmark/故障实验确认。
