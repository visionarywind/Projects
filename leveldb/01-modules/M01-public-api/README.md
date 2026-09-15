# M01 公共 API 与契约

- 文档目的：从公开头文件理解调用者可依赖的 LevelDB 接口。
- 适用范围：`include/leveldb/*.h`，目标提交 `7ee830d`。
- 对应源码版本：source/leveldb HEAD 7ee830d（2026-09-15 只读确认）。
- 证据状态：已确认；行为细节以头文件契约为准。
- 最后更新：2026-09-10
- 前置阅读：[项目概览](../../00-overview/project-overview.md)
- 后续阅读：[M02 DB 协调器](../M02-db-coordinator/README.md)
## 结论摘要

本页聚焦 01-modules/M01-public-api/README.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。

## 一句话说明

M01 是应用与内部实现之间的稳定边界：它定义 DB 生命周期、读写、迭代器、快照、批量写、Options、Env、Status、Comparator 和 Slice。

## 初学者解释

调用方只需要包含 `leveldb/*.h`，创建 `Options`，调用 `DB::Open`，然后通过 `DB` 做读写。函数出错不抛异常，而返回 `Status`；返回的 Iterator/Snapshot/DB 以及 Slice 对象有明确的释放或存活约束。

## 职责与非职责

- 负责：API 形状、参数语义、线程安全说明、错误类型和跨版本公开契约。
- 不负责：WAL、MemTable、MANIFEST、SSTable 具体实现、平台系统调用。

## 推荐阅读顺序

`db.h` → `options.h` → `status.h`/`slice.h` → `write_batch.h` → `iterator.h` → `env.h`。

## 关键入口

| 符号 | 位置 | 作用 |
|---|---|---|
| `DB::Open` | [include/leveldb/db.h:45-53](../../../source/leveldb/include/leveldb/db.h#L45-L53) | 打开/创建 DB，返回堆对象。 |
| `DB::Put/Delete/Write` | [include/leveldb/db.h:62-77](../../../source/leveldb/include/leveldb/db.h#L62-L77) | 单项或原子批量写。 |
| `DB::Get/NewIterator` | [include/leveldb/db.h:79-95](../../../source/leveldb/include/leveldb/db.h#L79-L95) | 读取和遍历。 |
| `DB::GetSnapshot/ReleaseSnapshot` | [include/leveldb/db.h:97-105](../../../source/leveldb/include/leveldb/db.h#L97-L105) | 一致性视图生命周期。 |
| `WriteBatch` | [include/leveldb/write_batch.h:32-78](../../../source/leveldb/include/leveldb/write_batch.h#L32-L78) | 有序编辑集合。 |
| `Options` | [include/leveldb/options.h:32-147](../../../source/leveldb/include/leveldb/options.h#L32-L147) | 行为、性能和格式相关配置。 |

## 最小示例

真实示例见 [doc/index.md:11-25](../../../source/leveldb/doc/index.md#L11-L25)。核心步骤是设置 `create_if_missing`，调用 `DB::Open`，检查 `Status`，结束时 `delete db`。这是文档示例，尚未在本次环境执行。

## 并发和所有权摘要

`DB` 可被同一进程多个线程安全共享；Iterator 和 WriteBatch 的共享访问可能需要调用方同步（[doc/index.md:130-141](../../../source/leveldb/doc/index.md#L130-L141)）。DB/Iterator/Snapshot 均有显式释放规则；Slice 不拥有底层字节，必须保证被引用数据存活（[doc/index.md:201-229](../../../source/leveldb/doc/index.md#L201-L229)）。

## 相关文档

- [设计](design.md)
- [接口](interfaces.md)
- [数据结构](data-structures.md)
- [测试](testing.md)

## 源码证据摘要

见正文链接。

## 未解决问题

二进制 ABI 兼容还需结合 `export.h`、编译器和发布实践确认；本模块只记录源码契约。

## 下一步阅读建议

继续看 [interfaces.md](interfaces.md)，然后追踪 `DB::Open` 到 M02。

## 文档元数据（规范补充）

- 文档目的：说明 `01-modules/M01-public-api/README.md` 的源码分析范围、结论和维护入口。
- 适用范围：当前项目对应模块/入口的静态源码与测试分析。
- 对应源码版本：以本项目 `00-overview/analysis-state.md` 或同页版本字段为准。
- 证据状态：静态源码证据；未执行的构建、测试、GPU、网络或多进程行为保持“未验证”。
- 最后更新：2026-09-15
- 前置阅读：本项目根 README 与 `00-overview/analysis-state.md`。
- 后续阅读：本模块/示例的实现、测试和风险页面。

## 深度审计

| 分析对象 | 入口落地 | 正常路径 | 分支 | 异常 | 清理 | 数据生命周期 | 执行上下文 | 行级证据 | Demo 映射 | 状态/缺口 |
|---|---|---|---|---|---|---|---|---|---|---|
| `01-modules/M01-public-api/README.md` | 已完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 已映射或不适用 | 部分完成：动态行为、边界或专用变体仍需验证 |
