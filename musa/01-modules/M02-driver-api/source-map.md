# Driver API 导出层：源码地图

- 文档目的：driver api：源码文件/目录到文档章节的映射。
- 适用范围：M02 模块的《Driver API 导出层：源码地图》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


| 层 | 路径 | 关注点 |
|---|---|---|
| 公共符号/包装 | `src/driver/mu_wrappers_generated.cpp`、`src/musa_shared_include/generated_musa_meta.h` | `mu*` 符号、ApiTrace、tools 回调双路径 |
| 入口声明 | `src/driver/muapi.h` | 577 个 `muapi*` 声明 |
| 入口实现 | `src/driver/mu_context.cpp`、`mu_device.cpp`、`mu_memory.cpp`、`mu_stream.cpp`、`mu_module.cpp`、`mu_mempool.cpp`、`mu_event.cpp` 等 | 参数检查、句柄转换、错误码 |
| 导出表/分派 | `src/driver/mu_entry.cpp`、`src/musa_shared_include/export_table.h` | `muapiGetExportTable`、`Client` 枚举、`FillAccessors`、观测 accessor |
| 工具钩子 | `src/driver/mupti`、`mugdb`、`muasan`、`callback.cpp` | trace/accessor/debug/回放 |
| 内部辅助 | `src/driver/internal.h`、`internal.cpp` | `InitPlatform`、TLS、`DriverEntryPointTable` |
| Core | `src/musa/core` | 对象、状态、生命周期 |
| HAL | `src/hal/hal*.h`、`src/hal/m3d` | 资源映射和硬件调用 |
| 导出清单 | `cmake/export_symbols.cmake`、`src/loader/loader_api_list.inc` | `MUSA_LOADER_API(...)` 宏驱动的导出符号 |

## 证据锚点

- `muapiInit`/`muapiCtxCreate_v2`：[src/driver/mu_context.cpp:121-182]
- `muapiGetProcAddress`/`muapiGetExportTable`：[src/driver/mu_entry.cpp:1222-2308]
- `muGetExportTable` 双路径包装：[src/driver/mu_wrappers_generated.cpp:3819-3857]
- `Client` 枚举与 `Driver::ExportTable`：[src/musa_shared_include/export_table.h:1640-1661] [src/musa_shared_include/export_table.h:1954-1964]
- 导出符号清单：[cmake/export_symbols.cmake:5-26]

## 搜索建议

```bash
rg -n "^MUresult muapi" src/driver/muapi.h
rg -n "FillAccessors" src/driver/mu_entry.cpp
rg -n "ADDMEMBER" src/musa_shared_include/export_table.h
```

上述命令是阅读建议，不代表已在本地或远端执行。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/musa_shared_include/export_table.h`（2 处，例：[src/musa_shared_include/export_table.h:1640-1661]）
- `src/driver/mu_context.cpp`（1 处，例：[src/driver/mu_context.cpp:121-182]）
- `src/driver/mu_entry.cpp`（1 处，例：[src/driver/mu_entry.cpp:1222-2308]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
