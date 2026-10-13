# 调试指南

- 文档目的：常见问题的定位路径与观察点。
- 适用范围：99-roadmap《调试指南》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-08
- 前置阅读：[`../00-overview/analysis-state.md`](../00-overview/analysis-state.md)
- 后续阅读：[`reading-guide.md`](reading-guide.md)、[`risk-register.md`](risk-register.md)


## 先分类

- 库加载/符号：M01/M02/export
- 初始化/无设备：M03
- 无 current context/句柄错误：M02/M04
- 地址、泄漏、异步 free：M05
- 卡住/顺序/完成错误：M06/M09
- 参数/模块加载：M07
- capture/update/拓扑：M08

## 证据采集

保留 commit、配置、设备 ordinal、context/stream id、API/correlation/submission id、MUresult 和同步点。不要用未执行命令的预期输出替代日志。

## 静态入口

`mu_entry.cpp` 的 1085-1249 段集中了 profiler、range profiler、PC sampling 入口和 correlation id helper（如 `GeneralIncrCorrelationId`），在这些函数上断点可以把 Command、内存备份动作与时间戳关联起来。[src/driver/mu_entry.cpp:1085-1249]

## 断点与观察点清单

下表每一项都是静态定位；断点是否命中、变量取值必须在目标环境确认（未验证）。

| 观察目标 | 行级锚点 | 断点后要看的东西 |
|---|---|---|
| accessor 哨兵 | [src/musa_shared_include/export_table.h:859-860] `AccessorHint` | 成员是否仍等于 `0xDEADBEEF`（即未被填充） |
| accessor 填充 | [src/driver/mu_entry.cpp:2146-2155] `FillAccessors` | `stop` 何时置位、`*pDst` 从哨兵往后推进了几项 |
| client 判定 | [src/driver/mu_entry.cpp:2160-2172] `pExportTableId` | 入参为空时返回 `MUSA_ERROR_INVALID_VALUE`，否则 `switch` 落到哪个 `case` |
| 未知 client | [src/driver/mu_entry.cpp:2300-2304] `muptiDriverTable` | 是否走 `default` 分支（源码留有 FIXME 注释，静默返回 MUPTI 表） |
| 日志开关 | [src/util/utilLog.h:209-222] `tprintf` | `Contains(flag)` 的真值与打印前缀，避免用"应该有日志"替代实际输出 |
| 内存池查找 | [src/hal/m3d/memMgr.cpp:41-44] `FindPool` | `Find` 的返回值：未命中时返回 `nullptr`，而该行直接 `->m_Value` 无空检查 |
| splay 查找方向 | [src/util/utilSplayTree.h:216-226] `Search` | 每层比较与走向：`m_Key < key` 时走左子树，方向与常规 BST 相反（见风险登记） |

## 相关文档

- [`risk-register.md`](risk-register.md)
- [`reading-guide.md`](reading-guide.md)
- [`technical-debt.md`](technical-debt.md)
- [`../00-overview/analysis-state.md`](../00-overview/analysis-state.md)
- [`../01-modules/module-registry.md`](../01-modules/module-registry.md)

## 源码证据摘要

- `src/driver/mu_entry.cpp`（4 处，例：[src/driver/mu_entry.cpp:2146-2155] `FillAccessors`）
- `src/musa_shared_include/export_table.h`（1 处，例：[src/musa_shared_include/export_table.h:859-860] `AccessorHint`）
- `src/util/utilLog.h`（1 处，例：[src/util/utilLog.h:209-222] `tprintf`）
- `src/hal/m3d/memMgr.cpp`（1 处，例：[src/hal/m3d/memMgr.cpp:41-44] `FindPool`）
- `src/util/utilSplayTree.h`（1 处，例：[src/util/utilSplayTree.h:216-226] `Search`）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`../99-roadmap/risk-register.md`](../99-roadmap/risk-register.md)。

## 下一步阅读建议

阅读 [`reading-guide.md`](reading-guide.md)；阅读 [`risk-register.md`](risk-register.md)。
