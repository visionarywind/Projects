# 工具与调试接口：接口契约

- 文档目的：tools debug：接口签名、前置条件、错误码与跨层契约。
- 适用范围：M10 模块的《工具与调试接口：接口契约》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


## 统一入口：`muGetExportTable`

| 项 | 内容 | 证据 |
|---|---|---|
| 公共声明 | `MUresult MUSAAPI muGetExportTable(const void** ppExportTable, const MUuuid* pExportTableId)` | [src/musa_shared_include/musa.h:27753] |
| 兼容层声明 | `MUresult muapiGetExportTable(const void**, const MUuuid*)` | [src/driver/muapi.h:125] |
| 包装实现 | `muGetExportTable` | [src/driver/mu_wrappers_generated.cpp:3819] |
| 真实实现 | `muapiGetExportTable` | [src/driver/mu_entry.cpp:1255] |
| 分派 switch | 按 client id 选择表 | [src/driver/mu_entry.cpp:2164-2304] |

## `Client` 枚举

| 值 | 命名空间 | 表 | 分发模式 | 证据 |
|---|---|---|---|---|
| `MUpti = 0x76543210` | `MUpti` | `DriverExportTable` | 返回静态表指针 | [src/musa_shared_include/export_table.h:1955]、[src/driver/mu_entry.cpp:2165] |
| `Runtime` | `Runtime` | `ExportTable` | 返回静态表指针 | [src/musa_shared_include/export_table.h:1956]、[src/driver/mu_entry.cpp:2168] |
| `Driver` | `Driver` | `ExportTable`（约 20 组） | **拷贝进调用方缓冲区** | [src/musa_shared_include/export_table.h:1957]、[src/driver/mu_entry.cpp:2171] |
| `MUgdb` | `MUgdb` | `ExportTable`（约 23 组） | 返回静态表指针 | [src/musa_shared_include/export_table.h:1958]、[src/driver/mu_entry.cpp:2241] |
| `MUasan` | `MUasan` | `ExportTable`（约 18 组） | 返回静态表指针 | [src/musa_shared_include/export_table.h:1959]、[src/driver/mu_entry.cpp:2244] |
| `Tools` | `Tools` | `ExportTable`（约 17 组） | **拷贝进调用方缓冲区** | [src/musa_shared_include/export_table.h:1960]、[src/driver/mu_entry.cpp:2250] |
| `Photon` | `PhotonEx` | `ExportTable` | 返回静态表指针 | [src/musa_shared_include/export_table.h:1961]、[src/driver/mu_entry.cpp:2247] |
| `Injection` | `Injection` | `ExportTable` | **拷贝进调用方缓冲区** | [src/musa_shared_include/export_table.h:1962]、[src/driver/mu_entry.cpp:2276] |
| `ToolsMuapi` | `ToolsMuapi` | `ExportTable`（约 6 组） | **拷贝进调用方缓冲区** | [src/musa_shared_include/export_table.h:1963]、[src/driver/mu_entry.cpp:2282] |

枚举以魔数 `0x76543210` 起，源码注释标注为 "magic number" [src/musa_shared_include/export_table.h:1955]。

## 两种分发模式的契约差异

| 模式 | 调用方责任 | 内存归属 | 填充时机 |
|---|---|---|---|
| 返回静态表指针 | 只读使用；**不得释放** | 驱动所有，进程期有效 | `muapiGetExportTable` 首次进入时已静态初始化 |
| 拷贝进调用方缓冲区 | 声明 `ExportTable` 实例并传入地址 | **调用方所有**，生命周期自管 | 每次调用 `muapiGetExportTable` 时逐槽覆盖 |

静态表定义位置：

| 表 | 证据 |
|---|---|
| `muptiDriverTable`（21 个分组） | [src/driver/mu_entry.cpp:1471-1494] |
| `runtimeTable` | [src/driver/mu_entry.cpp:1503-1506] |
| `gdbTable` | [src/driver/mu_entry.cpp:2024] |
| `asanTable` | [src/driver/mu_entry.cpp:2059] |
| `photonTable` | [src/driver/mu_entry.cpp:1942] |

## `AccessorHint` 契约

```cpp
constexpr uintptr_t AccessorHint = 0xDEADBEEF;                                  // src/musa_shared_include/export_table.h:859
#define ADDMEMBER(type, name) type name = reinterpret_cast<type>(AccessorHint)  // src/musa_shared_include/export_table.h:860
```

- 语义：`AccessorHint` 表示**该槽位待填充**（驱动侧填充循环的哨兵），不是可直接调用的地址。
- 填充循环：`FillAccessors` [src/driver/mu_entry.cpp:2146-2155]。
- 调用方义务：使用任何 accessor 前确认其不等于 `AccessorHint`；使用完毕后不得释放静态表族指针。
- 未填充即调用 → 跳转到 `0xDEADBEEF`。

## 错误语义

| 情况 | 结果 | 证据 |
|---|---|---|
| `ppExportTable == nullptr` 或 `pExportTableId == nullptr` | `MUSA_ERROR_INVALID_VALUE`，不进入 switch | [src/driver/mu_entry.cpp:2160-2161] |
| 未知 client id | **静默返回 MUPTI 表**（向后兼容临时措施，带 `FIXME`） | [src/driver/mu_entry.cpp:2300-2303] |
| GDB 侧 `dlopen("libmusa.so")` 失败 | `MUSA_ERROR_NOT_INITIALIZED` | [src/gdb/musaGdb.cpp:61-63] |
| GDB 侧符号 `muGetExportTable` 未找到 | `MUSA_ERROR_NOT_INITIALIZED` | [src/gdb/musaGdb.cpp:66-72] |

## 工具侧接口

| 接口方向 | 输入 | 输出/副作用 | 前置条件 | 错误 |
|---|---|---|---|---|
| 上游 -> 工具 | CLI 参数、构建选项 | stdout / stderr | `libmusa.so` 可加载 | `MUresult` 经 `checkMuErrors` 转进程退出 |
| 工具 -> M02 | 标准 `mu*` API 调用 | 无状态副作用 | Platform 已初始化 | `MUSA_ERROR_INVALID_VALUE` 等 |
| 工具 -> M13 ExportTable | `Client::Tools` id | 内部状态读取能力 | 驱动已加载 | 见上表 |
| 观测 -> M02 | accessor/query 请求 | ID、时间戳、metadata | 对象仍存活 | 访问失败/空值 |

公共 ABI 兼容性是 M02 的硬约束；其他模块应避免直接改变句柄布局和导出表顺序。此处的"应"是开发约束，不是源码事实。

## 相关证据

[src/driver/mu_entry.cpp:1255-2307] [src/musa_shared_include/export_table.h:859-1964] [src/tools/muInfo.cpp:9-20] [src/tools/muInfo.cpp:372-388] [src/gdb/musaGdb.cpp:58-78]

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/driver/mu_entry.cpp`（20 处，例：[src/driver/mu_entry.cpp:1255]）
- `src/musa_shared_include/export_table.h`（11 处，例：[src/musa_shared_include/export_table.h:1955]）
- `src/gdb/musaGdb.cpp`（3 处，例：[src/gdb/musaGdb.cpp:61-63]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
