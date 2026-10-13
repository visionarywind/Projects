# Driver API 导出层：风险与技术债

- 文档目的：driver api：风险、技术债及源码证据与修复/验证建议。
- 适用范围：M02 模块的《Driver API 导出层：风险与技术债》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


| 风险 | 影响 | 证据/状态 | 建议 |
|---|---|---|---|
| ABI/导出变化 | 外部程序加载失败、工具表错位 | 已确认敏感：`Client` 枚举与 17 组导出表顺序 | 符号回归 + 表结构 diff |
| 句柄即对象指针 | 伪造/悬垂句柄在 `IntrusiveCast` 处直接 UB | 已确认结构；无地址合法性校验 | 校验 + 防御性封装 |
| 未知 client 静默回退 | 其他工具拿到 MUPTI 表却不知情 | 已确认：`default` 返回 `&muptiDriverTable`，源码含 FIXME | 更新 MUPTI 后改 `INVALID_VALUE` |
| `FillAccessors` 源端 0 中止 | 后续槽位全部不填，调用方读空 | 已确认：`*pSrc != 0` 结束循环 | 补齐源表末尾哨兵，校验回填数 |
| `FillAccessors` 目标非 Hint | 调用方未预填 `AccessorHint` 则整组跳过 | 推断：槽位判等即回填条件 | 调用方侧契约文档化 |
| 调用链跨多层 | 错误定位困难 | 已确认结构 | 保留行级调用链 |
| 异步生命周期 | use-after-free/泄漏 | 推断风险 | 检查 waiting/inflight/同步 |
| 双版本/变体符号多 | 漏导出、版本分支难维护 | 已确认：v1/v2、`_ptsz`/`_ptds` | 生成脚本统一维护 |
| 恒值实现 | 行为名不符实 | 已确认：cache/sharedmem config 恒返回固定值 | 逐项核对实现 |
| 未实现路径 | 调用即错 | 已确认：`muapiModuleLoadDataEx` 恒 `NOT_SUPPORTED`、`muapiLaunchCooperativeKernel` 未做能力检查 | 按需补齐 |

## 重点风险说明

### 1. 未知 client 静默返回 MUPTI 表

`muapiGetExportTable`（[src/driver/mu_entry.cpp:1255]）的 `default:` 分支把 `*ppExportTable = &muptiDriverTable`，源码注释 `FIXME(Zhangjian.Zhao): backward compatible for current MUPTI, return INVALID_VALUE when MUPTI updated`。任何未登记的 client 都会拿到 MUPTI 表，且 `status` 仍为 `MUSA_SUCCESS`，调用方无法区分。[src/driver/mu_entry.cpp:2300-2303]

### 2. `FillAccessors` 遇源端 0 中止

回填条件是 `!stop && *pDst == AccessorHint && *pSrc != 0`：一旦源表出现 0（如某槽位漏填），循环停止，后续所有槽位保持 `AccessorHint` 不变。[src/driver/mu_entry.cpp:2146-2156] 另外若 `pDst`/`pSrc` 为 NULL 会提前 `stop=true`。[src/driver/mu_entry.cpp:2147-2149]

### 3. 句柄误用

`MUcontext`/`MUstream`/`MUmodule` 等句柄即 C++ 对象指针：`Musa::IntrusiveCast<Musa::Context>(ctx)` 直接强转解引用。`ValidateContext` 只检查「是否在 `Platform::ctxs()` 登记集合中且 refcount>0」，对伪造地址无防护。[src/driver/mu_context.cpp:24-28] [src/musa/core/platform.cpp:448-454]

### 4. 恒值/未实现接口

- `muapiCtxGetCacheConfig` 恒返回 `MU_FUNC_CACHE_PREFER_NONE`；`muapiCtxSetCacheConfig` 仅接受该值。[src/driver/mu_context.cpp:355-394]
- `muapiCtxGetSharedMemConfig` 恒返回 `MU_SHARED_MEM_CONFIG_DEFAULT_BANK_SIZE`。[src/driver/mu_context.cpp:431-449]
- `muapiModuleLoadDataEx` 恒返回 `MUSA_ERROR_NOT_SUPPORTED`。[src/driver/mu_module.cpp:148-151]
- `muapiLaunchCooperativeKernel` 直接复用 `muapiLaunchKernel`，注释 `TODO: Add check for cooperative launch capability`。[src/driver/mu_module.cpp:336-339]

本模块具体风险按其入口和资源类型排序；未执行的测试不标为通过。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/driver/mu_entry.cpp`（4 处，例：[src/driver/mu_entry.cpp:1255]）
- `src/driver/mu_context.cpp`（3 处，例：[src/driver/mu_context.cpp:24-28]）
- `src/driver/mu_module.cpp`（2 处，例：[src/driver/mu_module.cpp:148-151]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 上表风险的运行期复现与修复验证均未执行。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
