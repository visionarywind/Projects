# 配置影响地图

- 文档目的：《配置影响地图》的分析、证据与结论。
- 适用范围：90-cross-module《配置影响地图》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`../01-modules/module-registry.md`](../01-modules/module-registry.md)、[`system-wiring.md`](system-wiring.md)
- 后续阅读：[`interface-contracts.md`](interface-contracts.md)、[`../99-roadmap/risk-register.md`](../99-roadmap/risk-register.md)


| 配置 | 影响模块 | 影响面 | 证据 |
|---|---|---|---|
| `MUSA_BUILD_DEBUG` | M01 | Debug/Release 编译 | [CMakeLists.txt:127-130] |
| `ENABLE_ASAN_CHECK`/`TSAN` | M01/M10 | sanitizer 编译和 hook | [CMakeLists.txt:110-118] |
| `MUSA_BUILD_UT` | M01/测试 | FetchContent/gtest/CTest | [CMakeLists.txt:146-154] |
| visible devices/order | M03 | Device ordinal 和 muInfo结果 | [src/musa/core/platform.cpp:32-80] |
| `launchBlocking` | M06/M07 | kernel API 返回时机 | [src/driver/mu_module.cpp:262-263] |
| `memBlocking` | M05/M06 | async allocation/copy等待 | [src/driver/mu_memory.cpp:336-437] |
| `graphUserQ` | M08/M09 | GraphExec资源路径 | [src/musa/core/context.cpp:1428-1442] |
| `MUSA_TRACK_COMMAND_TIMESTAMP` | M06 | 开关命令时间戳记录（begin/end/queued/submitted/completed） | [src/musa/core/platformSettings.cpp:27] |
| `MUSA_INFLIGHT_SUBMISSION_LIMIT` | M06 | 在途提交上限 | [src/musa/core/platformSettings.cpp:26] |
| `MUSA_TRACE_MERGE_THRESHOLD` | M06/M10 | trace 合并阈值 | [src/musa/core/platformSettings.cpp:58] |

README/CMake 的 `CSV_UNSUPPORTED`/`CSV_UNSPPORTED` 差异应在配置验证前视为风险。[CMakeLists.txt:134-136]

## 相关文档

- [`system-wiring.md`](system-wiring.md)
- [`interface-contracts.md`](interface-contracts.md)
- [`cross-module-call-chains.md`](cross-module-call-chains.md)
- [`../01-modules/module-registry.md`](../01-modules/module-registry.md)
- [`../99-roadmap/risk-register.md`](../99-roadmap/risk-register.md)

## 源码证据摘要

- `src/musa/core/platformSettings.cpp`（3 处，例：[src/musa/core/platformSettings.cpp:27]）
- `src/musa/core/platform.cpp`（1 处，例：[src/musa/core/platform.cpp:32-80]）
- `src/driver/mu_module.cpp`（1 处，例：[src/driver/mu_module.cpp:262-263]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`../99-roadmap/risk-register.md`](../99-roadmap/risk-register.md)。

## 下一步阅读建议

阅读 [`interface-contracts.md`](interface-contracts.md)；阅读 [`../99-roadmap/risk-register.md`](../99-roadmap/risk-register.md)。
