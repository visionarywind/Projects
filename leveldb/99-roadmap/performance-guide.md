# 性能指南

- 文档目的：指导读写、压缩、缓存和 compaction 的可重复性能分析，并区分可运行性结果与性能结论。
- 适用范围：benchmark 和生产前实验。
- 对应源码版本：source/leveldb HEAD 7ee830d（2026-09-15 只读确认）。
- 证据状态：热点、flags 和本轮 Debug 小样本结果已确认；性能结论必须用 Release/固定条件实测。
- 最后更新：2026-09-10
- 前置阅读：[性能关键路径](../90-cross-module/performance-critical-paths.md)
- 后续阅读：[风险登记](risk-register.md)
## 结论摘要

本页聚焦 99-roadmap/performance-guide.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


- 源码版本：`main` / `7ee830d`。

## 基线

使用同一构建类型、编译器、CPU、磁盘、内存、数据库目录和依赖组合。清理或复用数据库的选择必须明确。

本机已执行两次小规模 Debug 运行，仅用于确认 benchmark 入口和读写链路可运行：

```bash
./build/db_bench --benchmarks=fillseq,readrandom --num=10000 --value_size=100 --threads=1 --db=/tmp/leveldb-knowledge-bench
./build/db_bench --benchmarks=fillseq,readrandom --num=10000 --value_size=100 --threads=1 --db=/tmp/leveldb-knowledge-bench-20260910-r2
```

最近一次实际输出：

```text
LevelDB: version 1.23
CPU: 12 * 12th Gen Intel(R) Core(TM) i5-12400
Entries: 10000
RawSize: 1.1 MB (estimated)
FileSize: 0.6 MB (estimated)
fillseq    : 3.114 micros/op; 35.5 MB/s
readrandom : 1.199 micros/op; (10000 of 10000 found)
```

此前运行得到 `fillseq 3.104 micros/op`、`readrandom 1.141 micros/op`；不同临时目录和运行时状态下的单次差异不构成性能回归结论。
## db_bench 场景

关注 `fillseq`、`fillrandom`、`fillsync`、`overwrite`、`readrandom`、`readseq`、`readreverse`、`compact` 和 compression 场景。[benchmarks/db_bench.cc:23-64](../../source/leveldb/benchmarks/db_bench.cc#L23-L64)

## 变量

固定或分别扫描：`--num`、`--reads`、`--threads`、`--value_size`、`--compression_ratio`、write buffer、max file size、block size、cache、filter、sync。一次只改变少数变量。

## 指标

记录 ops/sec、延迟分布（若开启 histogram）、文件大小、读/写放大、compaction backlog、CPU、RSS、系统调用和错误。不要引用 README 中旧硬件数字作为当前结论。

## 解释

- fillsync 反映持久性路径，不等于普通异步写吞吐。
- readrandom 对 cache/filter/file descriptor 很敏感。
- compact 需要等待后台稳定后再比较。
- 压缩结果受数据分布和可选库编译能力影响。
- Debug 构建中的 micros/op 不能与 Release 或 CI 数据直接比较。

## 结果状态

本轮已验证 benchmark 命令成功运行并且 10000/10000 次随机读取命中；尚未执行 Release/RelWithDebInfo、多线程、fillsync、fillrandom、compact、压缩库组合或重复统计实验。因此当前没有生产性能基线。

## 相关文档

- [构建、运行与部署总览](../00-overview/build-and-deploy.md)
- [快速开始](quick-start.md)
- [性能关键路径](../90-cross-module/performance-critical-paths.md)

## 源码证据摘要

- [db_bench 场景](../../source/leveldb/benchmarks/db_bench.cc#L23-L64)
- [CI benchmark/install 流程](../../source/leveldb/.github/workflows/build.yml#L85-L102)

## 未解决问题

- 需要在 Release/RelWithDebInfo 下重复实验，并固定 CPU、磁盘、目录和依赖组合。
- Snappy、Zstd、block cache、filter、fd/mmap 限制对性能的影响尚未测量。

## 下一步阅读建议

先用本页 Debug 命令确认入口，再建立独立实验记录，逐次改变一个变量并报告中位数/分位数，而不是引用单次 Debug 结果。
