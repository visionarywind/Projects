# Driver API 导出层（M02）

- 源码范围：`src/driver/muapi.h`、`mu_entry.cpp`、`mu_context.cpp`、`mu_memory.cpp`、`mu_stream.cpp`、`mu_module.cpp`、`mu_mempool.cpp`、`mu_wrappers_generated.cpp`、`cmake/export_symbols.cmake`
- 对应版本：`5.3.0 develop`
- 文档目的：driver api：总览：职责、结构、关键结论与文档导航。
- 适用范围：M02 模块的《Driver API 导出层（M02）》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：静态源码已确认；硬件行为、性能和完整异常覆盖未验证
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)

## 一句话职责

Driver API 导出层负责 `外部 C ABI（mu*）-> 包装/观测 -> muapi* 入口 -> Core 对象 -> MUresult`。

`muapi.h` 声明 577 个 `muapi*` 入口（[src/driver/muapi.h:13-602]）；公共符号 `mu*` 包装集中在 `mu_wrappers_generated.cpp`（23325 行），每个包装按「是否启用 tools 回调」走观测路径或直通路径。[src/driver/mu_wrappers_generated.cpp:3819-3857]

## 阅读入口

1. 先读本页和 [`source-map.md`](source-map.md)。
2. 再读 [`implementation.md`](implementation.md)、[`call-chains.md`](call-chains.md)、[`interfaces.md`](interfaces.md)。
3. 涉及导出表、句柄误用、ABI 兼容时读 [`risks-and-debt.md`](risks-and-debt.md)。

## 边界

- **本模块拥有**：`mu*`/`muapi*` 入口、句柄到 Core 对象的转换、导出表分派、wrapper 生成规则。
- **本模块不拥有**：上游 API 的公共 ABI 契约（由 `loader_api_list.inc` 与上游头文件决定）和下游 GPU/内核驱动的最终实现。
- **证据边界**：以下结论来自源码行号，不等同于目标机运行成功。

## 核心证据

1. 分派与导出表填充：`muapiGetExportTable` 按 `Client` 枚举 switch 分派，`Driver`/`Tools`/`ToolsMuapi`/`Injection` 用 `FillAccessors` 按 `AccessorHint` 槽位回填函数指针，其余 client 直接返回静态表。[src/driver/mu_entry.cpp:2164-2304] [src/musa_shared_include/export_table.h:1954-1964]
2. 双路径包装：`muGetExportTable` 先经 `ApiTrace` 与 `toolsIssueCallback`，`skipDriverImpl` 未置位时才调用 `muapiGetExportTable`。[src/driver/mu_wrappers_generated.cpp:3819-3857]
3. 初始化与句柄校验：`muapiInit` 要求 flags==0 并 `CreatePlatform`；多数入口先 `InitPlatform()` 再取 `TlsCtxTop()`。[src/driver/mu_context.cpp:121-133] [src/driver/internal.h:307-321]
4. 导出符号清单：`cmake/export_symbols.cmake` 从 `src/loader/loader_api_list.inc` 读取 `MUSA_LOADER_API(...)` 宏并去重校验。[cmake/export_symbols.cmake:5-26]

## 深度状态

正常路径已建立：参数检查 → 句柄转换（`IntrusiveCast`/`ICast`）→ Core 方法 → `MUresult` 向上传播。分支、失败回滚、并发和清理见子文档。凡标记“推断/未知”的项目，均需在目标源码或硬件环境中补证。

## Demo 覆盖

- **真实经过本模块的 Demo 步骤**（逐步见 [`../../80-demos/D01-muinfo/execution-trace.md`](../../80-demos/D01-muinfo/execution-trace.md) §1–§2）：
  1. D01-S002〈muInit → `muapiInit`〉的 flags 校验与 `CreatePlatform` [src/driver/mu_context.cpp:121-133]；
  2. D01-S004〈`muapiDeviceGetCount`〉与 D01-S006〈GetDeviceProps 批量查询〉的设备视图/属性入口 [src/driver/mu_device.cpp:40-51]、[src/driver/mu_device.cpp:71-94]；
  3. D01-S005〈retain primary context + set current〉的 `muapiDevicePrimaryCtxRetain` [src/driver/mu_context.cpp:472-488] 与 `muapiCtxSetCurrent` [src/driver/mu_context.cpp:280-295]；
  4. D01-S007 的 peer/内存查询 `muapiDeviceCanAccessPeer` [src/driver/mu_peer.cpp:13-37] 与 `muapiMemGetInfo_v2` [src/driver/mu_memory.cpp:548-567]。
- **候选但未覆盖**：D02–D07（`tests/*.cu`）——本快照无 `tests/` 目录，入口未核实；候选表显示 D02/D03 计划覆盖 memcpy/stream 入口、D04/D05/D07 覆盖 graph 入口、D06 覆盖 graph memory alloc node 入口，均未走通。
- **尚无 Demo 覆盖的重要实现**：
  1. kernel launch 入口 `muapiLaunchKernel` [src/driver/mu_module.cpp:231-268]；
  2. stream 创建与同步入口 [src/driver/mu_stream.cpp:17-42]；
  3. graph 创建入口 [src/driver/mu_graph.cpp:1188-1208]；
  4. mempool 入口 [src/driver/mu_mempool.cpp:21-45] 与导出表分发（`muapiGetExportTable` 的 Client switch）[src/driver/mu_entry.cpp:2164-2304]。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/driver/mu_wrappers_generated.cpp`（2 处，例：[src/driver/mu_wrappers_generated.cpp:3819-3857]）
- `src/driver/muapi.h`（1 处，例：[src/driver/muapi.h:13-602]）
- `src/driver/mu_entry.cpp`（1 处，例：[src/driver/mu_entry.cpp:2164-2304]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
