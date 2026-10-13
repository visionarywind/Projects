# 工具与调试接口：数据结构与所有权

- 文档目的：tools debug：核心数据结构、字段含义与持有关系。
- 适用范围：M10 模块的《工具与调试接口：数据结构与所有权》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


## 导出表分发：`muapiGetExportTable`

消费方（profiler / debugger / asan / 工具）通过一个 `Client` 枚举 id 换取内部状态表，入口为 `muapiGetExportTable` [src/driver/mu_entry.cpp:1255]。分派按 `*(const Client*)pExportTableId` 进行 [src/driver/mu_entry.cpp:2164]。

`Client` 枚举（9 值，以魔数 `0x76543210` 起始）定义在 M13 公共头文件，导出表结构布局同在该头文件给出，完整对照表见 [`interfaces.md`](interfaces.md) 的 `Client` 枚举章节 [src/musa_shared_include/export_table.h:1954-1964]。

## 三种分发模式与内存所有权

| 模式 | client | 表来源 | 所有权 | 填充时机 |
|---|---|---|---|---|
| 返回静态表指针 | `MUpti`/`Runtime`/`MUgdb`/`MUasan`/`Photon` | 驱动 static 表 | 驱动所有，进程期有效 | 首次进入函数时已静态初始化 |
| 拷贝进调用方缓冲区 | `Driver`/`Tools`/`Injection`/`ToolsMuapi` | 调用方自持的 `ExportTable` 实例 | **调用方所有** | 每次调用时 `FillAccessors` 逐槽覆盖 |
| 默认回退 MUPTI 表 | 未知 client id | `&muptiDriverTable` | 驱动所有 | 同上（向后兼容临时措施） |

- `AccessorHint = 0xDEADBEEF` 是“待填充”哨兵 [src/musa_shared_include/export_table.h:859]，`ADDMEMBER` 宏把每个成员预置为该值 [src/musa_shared_include/export_table.h:860]。
- 静态表定义：[src/driver/mu_entry.cpp:1471-1494]（`muptiDriverTable`）、[src/driver/mu_entry.cpp:1503-1506]（`runtimeTable`）、[src/driver/mu_entry.cpp:1942]（`photonTable`）、[src/driver/mu_entry.cpp:2024-2037]（`gdbTable`）、[src/driver/mu_entry.cpp:2059-2064]（`asanTable`）。
- `FillAccessors` 的哨兵循环 [src/driver/mu_entry.cpp:2146-2155]：逐槽把源 accessor 地址写入调用方目标槽，遇 0 或 `nullptr` 即中止。

## 驱动侧 hook 存储

三类观测共用同一种“存储 + 启停”结构：全局静态存储持有 `ready` 原子标志、hook 函数指针表，以及（gdb 额外的）调试器句柄/内存/线程状态。

| 结构 | 定义位置 | 字段要点 |
|---|---|---|
| `MUptiDriverHookStorage` | [src/driver/mupti/hooks.h:8-12] | `ready`、`MUptiDriverHooks`、`reserved[8]` |
| `MUasanDriverHookStorage` | [src/driver/muasan/hooks.h:8-12] | 同型 |
| `MUgdbDriverHookStorage` | [src/driver/mugdb/hooks.h:29-40] | 另含 `debuggerHandle`、`pShaderMem`/`pExchangeMem`、`hasInjectMem`、`trapRef`、`thread`、`mtx` |

全局实例：`G_MUPTI_DRIVER_HOOKS` [src/driver/mupti/hooks.cpp:3-7]、`G_MUASAN_DRIVER_HOOKS` [src/driver/muasan/hooks.cpp:3-7]、`G_MUGDB_DRIVER_HOOKS` [src/driver/mugdb/hooks.cpp:3-14]。

## GDB 侧状态

`MusaGdb` 单例持有 FIFO 管道（`m_WritePipe`/`m_ReadPipe`）、子进程 pid、每设备内存信息表与初始化 once_flag [src/gdb/musaGdb.h:30-46]。驱动侧通过 `libmusadebugger.so` 里的 `muGdb*` 符号反向调用它 [src/driver/mugdb/tracepoints.cpp:26]、[src/driver/mugdb/tracepoints.cpp:41]。

## 所有权总结

- 静态表族指针：消费方只读，不得释放。
- 拷贝模式表内存：调用方自持，生命周期不受驱动管理。
- hook 存储 `ready` 标志用 release/acquire 原子序管理可见性 [src/driver/mupti/hooks.cpp:11]、[src/driver/mugdb/hooks.cpp:18]。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/driver/mu_entry.cpp`（8 处，例：[src/driver/mu_entry.cpp:1255]）
- `src/musa_shared_include/export_table.h`（3 处，例：[src/musa_shared_include/export_table.h:1954-1964]）
- `src/driver/mupti/hooks.cpp`（2 处，例：[src/driver/mupti/hooks.cpp:3-7]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
