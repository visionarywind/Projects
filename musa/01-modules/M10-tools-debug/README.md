# 工具与调试接口（M10）

- 源码范围：`src/tools`、`src/gdb`、`src/driver/mupti`、`src/driver/mugdb`、`src/driver/muasan`
- 对应版本：`5.3.0 develop`
- 文档目的：tools debug：总览：职责、结构、关键结论与文档导航。
- 适用范围：M10 模块的《工具与调试接口（M10）》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：静态源码已确认；硬件行为、性能和完整异常覆盖未验证
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)

## 一句话职责

工具与调试接口负责 `工具/调试表 -> Driver API/core accessors -> 输出/回调`，包括 `muInfo` 设备查询工具、GDB 调试器客户端接入，以及 MUPTI/MUASAN/MUgdb 三类观测 hook。

## 阅读入口

1. 先读本页和 [`source-map.md`](source-map.md)。
2. 再读 [`interfaces.md`](interfaces.md)、[`call-chains.md`](call-chains.md)、[`implementation.md`](implementation.md)。
3. 涉及导出表分发、hook 注入或 loader 绕过时，结合 [`risks-and-debt.md`](risks-and-debt.md)。

## 边界

- **本模块拥有**：`muInfo` 工具源码与构建规则、GDB 客户端 `MusaGdb`、驱动侧 mugdb/mupti/muasan 的 hook 与 tracepoint、导出表分发入口 `muapiGetExportTable` 的消费侧。
- **本模块不拥有**：`Client` 枚举、`ExportTable` 结构布局、`AccessorHint` 契约（属 M13 公共头文件）；`mu*` API 的真实实现（属 M02 及以下模块）。
- **证据边界**：以下结论来自源码行号，不等同于目标机运行成功。

## 核心证据

`muInfo` 主流程 [src/tools/muInfo.cpp:372-388]、设备属性批量缓存 [src/tools/muInfo.cpp:119-274]、GDB 客户端直接 `dlopen` 取导出表 [src/gdb/musaGdb.cpp:58-78]、驱动侧导出表分发 switch [src/driver/mu_entry.cpp:2164-2304]、三类 hook 存储与启停 [src/driver/mupti/hooks.cpp:3-16]、[src/driver/muasan/hooks.cpp:3-16]、[src/driver/mugdb/hooks.cpp:3-23]。

## 深度状态

正常路径已建立：`muInfo` 查询链、ExportTable 三种分发模式、GDB 调试器双向 FIFO 消息流、MUPTI/MUASAN tracepoint 门控均已在源码中定位。分支、失败回滚、并发和清理见子文档。凡标记“推断/未知”的项目，均需在目标源码或硬件环境中补证。

## Demo 覆盖

- **真实经过本模块的 Demo 步骤**：D01 本身就是本模块的 `muInfo`（见 [`../../80-demos/D01-muinfo/execution-trace.md`](../../80-demos/D01-muinfo/execution-trace.md) §1–§2）：
  1. D01-S001〈入口打印 compiler〉[src/tools/muInfo.cpp:372-375]；
  2. D01-S006〈`GetDeviceProps` 静态缓存批量查询〉[src/tools/muInfo.cpp:119-274]；
  3. D01-S007〈`printDeviceProp` 输出属性/peer/memInfo〉[src/tools/muInfo.cpp:283-370]；
  4. 失败即退出的错误宏 `checkMuErrors` [src/tools/muInfo.cpp:9-20]。
- **候选但未覆盖**：D02–D07（`tests/*.cu`）——本快照无 `tests/` 目录，入口未核实；它们是驱动侧测试，不经过 `src/tools`（覆盖矩阵 M10 列为“否”）。
- **尚无 Demo 覆盖的重要实现**：
  1. GDB 客户端 `MusaGdb::GetDriverImportTable` [src/gdb/musaGdb.cpp:58-78]；
  2. MUPTI hook 启停 `EnableMUptiDriver` [src/driver/mupti/hooks.cpp:3-16] 与 MUASAN hook 启停 [src/driver/muasan/hooks.cpp:3-16]；
  3. 导出表分发 `muapiGetExportTable` [src/driver/mu_entry.cpp:1255-1300]；观测链见 [`call-chains.md`](call-chains.md)。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/tools/muInfo.cpp`（2 处，例：[src/tools/muInfo.cpp:372-388]）
- `src/gdb/musaGdb.cpp`（1 处，例：[src/gdb/musaGdb.cpp:58-78]）
- `src/driver/mu_entry.cpp`（1 处，例：[src/driver/mu_entry.cpp:2164-2304]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
