# Driver API 导出层：开发指南

- 文档目的：driver api：常见修改场景的落地入口、注意点与验证方式。
- 适用范围：M02 模块的《Driver API 导出层：开发指南》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


## 修改前

1. 锁定源码提交并更新证据行号；行号以 `rg -n` 实测为准，不允许凭记忆写。
2. 判断改动属于哪一层：
   - 生成包装层（`mu_wrappers_generated.cpp`）：一般由 `generated_musa_meta.h` 与生成脚本产出，优先改生成来源；
   - `muapi*` 入口层：契约、参数检查、句柄转换；
   - 导出表（`mu_entry.cpp`/`export_table.h`）：client 分派与 accessor 槽位；
   - Core/HAL：对象生命周期与硬件调用。
3. 画出调用链和资源所有权，再改代码。新增 `muapi*` 时先确认它是否需要进入 `Driver::ExportTable` 某分组或 `muapiGetProcAddress` 的 `DriverEntryPointTable` 登记。[src/musa_shared_include/export_table.h:1198-1206] [src/driver/internal.cpp:175-179]

## 修改中

- 保持现有风格：v1/v2 双版本用 `_v2` 后缀；PTDS 变体用 `_ptsz`/`_ptds` 后缀并在 wrapper 或 `muapi*_ptsz` 里把 `musaStreamDefault` 换成 `musaStreamPerThread`。[src/driver/mu_module.cpp:542-573]
- 所有新建异步资源明确完成条件和持有者；同步路径在入口层用 `true`，异步路径读 `GetSettings().memBlocking`/`launchBlocking`。[src/driver/mu_memory.cpp:845-855] [src/driver/mu_memory.cpp:861-871]
- 参数检查顺序：flags/枚举 -> 输出指针非空 -> `TlsCtxTop()` -> 对象校验 -> 调 Core，错误码用对应 `MUSA_ERROR_*`。[src/driver/mu_stream.cpp:21-42]
- 句柄转换统一用 `IntrusiveCast`/`ICast`/`reinterpret_cast`，不要裸解引用 C 句柄。[src/driver/mu_context.cpp:24-28]
- 需要被 tools 观测的 `mu*` 符号在包装层加 trace/callback 分支；纯内部路径只改 `muapi*` 实现。
- 导出表新槽位用 `ADDMEMBER(type, name)` 声明（初始为 `AccessorHint`），`FillAccessors` 会自动回填。[src/musa_shared_include/export_table.h:859-860] [src/driver/mu_entry.cpp:2146-2156]

## 修改后

先做静态 diff/格式检查，并在本仓库跑 `verify_kb.py` 与 `verify_content.py`；再在授权目标机做最小构建和专项测试。安装脚本只在明确授权时执行。公共 ABI 相关改动（导出符号、句柄布局、导出表顺序）必须单独评审。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/musa_shared_include/export_table.h`（2 处，例：[src/musa_shared_include/export_table.h:1198-1206]）
- `src/driver/mu_memory.cpp`（2 处，例：[src/driver/mu_memory.cpp:845-855]）
- `src/driver/internal.cpp`（1 处，例：[src/driver/internal.cpp:175-179]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
