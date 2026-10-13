# 工具与调试接口：行级分析

- 文档目的：tools debug：关键代码位置的行级证据与解读。
- 适用范围：M10 模块的《工具与调试接口：行级分析》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


## 证据表

| 行号 | 观察 | 结论状态 |
|---|---|---|
| [src/tools/muInfo.cpp:9-20] | `checkMuErrors`：首次 API 错误即 `exit(EXIT_FAILURE)` | 已确认 |
| [src/tools/muInfo.cpp:119-274] | `GetDeviceProps` 函数内 static 缓存，批量属性采集 | 已确认 |
| [src/tools/muInfo.cpp:276-278] | `printCompilerInfo`：输出固定串 `compiler: mcc` | 已确认 |
| [src/tools/muInfo.cpp:283-370] | `printDeviceProp`：字段输出、peer 矩阵、`muMemGetInfo_v2` | 已确认 |
| [src/tools/muInfo.cpp:372-388] | `main`：`muInit` → 逐设备 primary ctx → `printDeviceProp` | 已确认 |
| [src/gdb/musaGdb.cpp:58-78] | `GetDriverImportTable`：`Util::Library` 装载 `libmusa.so` 后取 `muGetExportTable`（装载内部经 dlopen） | 已确认 |
| [src/gdb/musaGdb.cpp:80-136] | `Init`：取表 → hook/accessor → FIFO → `fork` agent | 已确认 |
| [src/gdb/interface.cpp:155-258] | `KernelLoadingInternal`：组包、code object dump、FIFO 发送 | 部分确认（agent 端未验证） |
| [src/driver/mu_entry.cpp:2164-2304] | ExportTable 分发 switch，三种模式 | 已确认 |
| [src/driver/mu_entry.cpp:2146-2155] | `FillAccessors` 逐槽填充哨兵循环 | 已确认 |
| [src/driver/mupti/hooks.cpp:3-16] | `G_MUPTI_DRIVER_HOOKS` 静态存储与启停 | 已确认 |
| [src/driver/mupti/tracepoints.h:13-21] | `EnterMemcpyV2` 门控 + `AccessorHint` 检查 | 已确认 |
| [src/driver/muasan/hooks.cpp:3-16] | `G_MUASAN_DRIVER_HOOKS` 静态存储与启停 | 已确认 |
| [src/driver/muasan/tracepoints.h:16-29] | `KernelLoadingProcess`（MUasan 命名空间）门控调用 | 已确认 |
| [src/driver/mugdb/tracepoints.cpp:53-79] | `InitDebugger`：`Util::Library::Load` 装载 `libmusadebugger.so`、调 `muGdbInit` | 已确认 |
| [src/driver/mugdb/tracepoints.cpp:129-171] | `Monitor`：轮询 trap buffer，`raise(SIGINT)` | 部分确认（硬件行为未验证） |
| [src/musa/core/platform.cpp:130] | 驱动初始化末尾调 `MUgdb::InitDebugger()` | 已确认 |

## 阅读方法

1. 先看 `muInfo` 的 `checkMuErrors` 与 early exit，理解单错误即退出的约束。
2. 再看 `GetDriverImportTable`/`InitDebugger` 两条 `dlopen` 路径如何互指。
3. 最后看 `mu_entry.cpp` 的导出表分发 switch 与 `FillAccessors`，对照 `interfaces.md` 的模式表。

本页不把没有逐行读取的代码扩写成确定行为；agent 端与硬件侧副作用未验证。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/tools/muInfo.cpp`（5 处，例：[src/tools/muInfo.cpp:9-20]）
- `src/gdb/musaGdb.cpp`（2 处，例：[src/gdb/musaGdb.cpp:58-78]）
- `src/driver/mu_entry.cpp`（2 处，例：[src/driver/mu_entry.cpp:2164-2304]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
