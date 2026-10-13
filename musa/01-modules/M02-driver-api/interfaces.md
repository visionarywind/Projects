# Driver API 导出层：接口契约

- 文档目的：driver api：接口签名、前置条件、错误码与跨层契约。
- 适用范围：M02 模块的《Driver API 导出层：接口契约》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


## 接口方向

| 接口方向 | 输入 | 输出/副作用 | 前置条件 | 错误 |
|---|---|---|---|---|
| 上游 -> 本模块 | `mu*` 参数、句柄或 `MUuuid` 导出表 id | 状态更新、对象、导出表指针 | 版本/对象有效 | `MUresult` |
| 本模块 -> Core | 规范化参数、`MemoryCreateInfo`/`MemoryPoolCreateInfo` | Core 对象方法调用、登记/注销 | `TlsCtxTop()`/Device 可用 | 向上传播 |
| 本模块 -> HAL | `Hal::MemoryProperty`/`Hal::MemoryPoolCreateInfo`/queue 参数 | 底层资源/提交 | HAL device 有效 | 在 Core 边界转换 |
| 观测 -> 本模块 | accessor/query 请求、`AccessorHint` 槽位 | ID、时间戳、metadata、函数指针 | 对象仍存活 | 访问失败/空值 |

## 入口签名

- `muapi.h` 集中声明 577 个 `muapi*` 入口，全部返回 `MUresult`；公共 `mu*` 符号由 `mu_wrappers_generated.cpp` 包装。[src/driver/muapi.h:13-602] [src/driver/mu_wrappers_generated.cpp:3819-3857]
- 导出符号清单由 `loader_api_list.inc` 的 `MUSA_LOADER_API(...)` 宏驱动，`cmake/export_symbols.cmake` 解析去重。[cmake/export_symbols.cmake:5-26]

## 导出表结构

- 分派键：`enum class Client`，`MUpti=0x76543210` 起，其余依次递增。[src/musa_shared_include/export_table.h:1954-1964]
- `Driver::ExportTable` 聚合 17 组 accessor，末尾 `reserved` 槽位必须为 `nullptr`。[src/musa_shared_include/export_table.h:1640-1659]
- 观测侧结构（`MUpti::*Accessors`/`MUgdb::*Accessors`/`Tools::*Accessors`）全部以 `ADDMEMBER` 初始化，默认 `AccessorHint`，由 `FillAccessors` 回填。[src/musa_shared_include/export_table.h:860] [src/driver/mu_entry.cpp:2146-2156]

## 兼容性约束

公共 ABI 兼容性是 M02 的硬约束：句柄布局、`mu*` 导出符号、`Driver::ExportTable` 分组顺序、`Client` 枚举取值都不能随意改动。`muapiGetExportTable` 的 `default` 分支当前静默返回 MUPTI 表（`FIXME(Zhangjian.Zhao)`），改动前需确认 MUPTI 版本已跟随更新。[src/driver/mu_entry.cpp:2300-2303]

此处的“应”是开发约束，不是源码事实。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/musa_shared_include/export_table.h`（3 处，例：[src/musa_shared_include/export_table.h:1954-1964]）
- `src/driver/mu_entry.cpp`（2 处，例：[src/driver/mu_entry.cpp:2146-2156]）
- `src/driver/muapi.h`（1 处，例：[src/driver/muapi.h:13-602]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
