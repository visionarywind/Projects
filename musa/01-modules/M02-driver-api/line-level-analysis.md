# Driver API 导出层：行级分析

- 文档目的：driver api：关键代码位置的行级证据与解读。
- 适用范围：M02 模块的《Driver API 导出层：行级分析》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


## 证据表

| 位置 | 观察 | 结论状态 |
|---|---|---|
| `muapiInit` flags 检查 | `flags != 0` 直接返回 `MUSA_ERROR_INVALID_VALUE`（[src/driver/mu_context.cpp:121-133]） | 已确认 |
| 句柄转换三态 | `ctx ? IntrusiveCast<Musa::Context>(ctx) : TlsCtxTop()`（[src/driver/mu_context.cpp:24]） | 已确认 |
| Context 校验 | `ValidateContext` 失败 -> `MUSA_ERROR_INVALID_CONTEXT`（[src/driver/mu_context.cpp:25-27]） | 已确认 |
| flags 掩码 | `MU_CTX_FLAGS_MASK != (flags | MU_CTX_FLAGS_MASK)` -> `MUSA_ERROR_INVALID_VALUE`（[src/driver/mu_context.cpp:166-167]） | 已确认 |
| Stream 创建检查 | flags 非 `MU_STREAM_DEFAULT`/`MU_STREAM_NON_BLOCKING` -> `INVALID_VALUE`（[src/driver/mu_stream.cpp:30-31]） | 已确认 |
| MemAlloc 检查 | `dptr==nullptr` -> `INVALID_VALUE`；`bytesize==0` -> `*dptr=0` 且 `INVALID_VALUE`（[src/driver/mu_memory.cpp:275-280]） | 已确认 |
| Launch 检查 | `!f` -> `MUSA_ERROR_INVALID_VALUE`（[src/driver/mu_module.cpp:246-249]） | 已确认 |
| 导出表回填 | `while (!stop && *pDst == AccessorHint && *pSrc != 0)`（[src/driver/mu_entry.cpp:2151-2155]） | 已确认 |
| 导出表参数检查 | `ppExportTable/pExportTableId` 为 NULL -> `MUSA_ERROR_INVALID_VALUE`（[src/driver/mu_entry.cpp:2160-2161]） | 已确认 |
| 未知 client 回退 | `default:` 返回 `&muptiDriverTable`（[src/driver/mu_entry.cpp:2300-2303]） | 已确认 |
| 包装直通 | `skipDriverImpl` 未置位时才调 `muapiGetExportTable`（[src/driver/mu_wrappers_generated.cpp:3845-3853]） | 已确认 |

## 阅读方法

1. 先看参数检查和 early return（多为 `if (nullptr == x) status = MUSA_ERROR_INVALID_VALUE;` 模式）。
2. 再看对象创建、登记、引用计数或 tracker 更新（`LockedAddCtx`/`LockedRemoveCtx`、`Retain`/`Release`）。
3. 最后看 HAL 调用、异步入队（`CmdMemAlloc`/`GeneralLaunchKernel`）、错误回滚（`DestroyModule`/`delete`）和析构。

本页不把没有逐行读取的代码扩写成确定行为。已列行号均为 `rg -n`/`sed -n` 核对过的位置。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/driver/mu_context.cpp`（4 处，例：[src/driver/mu_context.cpp:121-133]）
- `src/driver/mu_entry.cpp`（3 处，例：[src/driver/mu_entry.cpp:2151-2155]）
- `src/driver/mu_stream.cpp`（1 处，例：[src/driver/mu_stream.cpp:30-31]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
