# D01：`muInfo` 失败路径

- 文档目的：失败路径与错误输出对照。
- 适用范围：80-demos《D01：`muInfo` 失败路径》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`../demo-registry.md`](../demo-registry.md)、[`../../00-overview/project-overview.md`](../../00-overview/project-overview.md)
- 后续阅读：[`../../01-modules/module-registry.md`](../../01-modules/module-registry.md)、[`../../99-roadmap/risk-register.md`](../../99-roadmap/risk-register.md)


## 0. Loader 层失败（发生在驱动代码执行之前）

`muInfo` 链接的是 M11 loader，任何驱动定位/加载失败都会在进入 `muapiInit` 之前返回，且**错误原因不在 `muInfo` 输出中体现**。

| 失败点 | loader 返回码 | 证据 |
|---|---|---|
| thunk（gdi32/libdxcore）加载或符号解析失败 | 折叠为单一失败原因 | [src/loader/driver_resolver.cpp:71-79] |
| 适配器数超过 256 | `MUSA_ERROR_SYSTEM_DRIVER_MISMATCH` | [src/loader/driver_resolver.cpp:141] |
| MUSA 适配器包 ID 不一致 | `MUSA_ERROR_SYSTEM_DRIVER_MISMATCH` | [src/loader/driver_resolver.cpp:342] |
| 驱动文件不存在 | 传播文件系统错误码 | [src/loader/driver_resolver.cpp:356-362] |
| `std::bad_alloc` / 其他异常 | `MUSA_ERROR_OUT_OF_MEMORY` / `MUSA_ERROR_UNKNOWN` | [src/loader/driver_resolver.cpp:374]、[src/loader/driver_resolver.cpp:376] |
| `dlopen` / `LoadLibraryExW` 失败 | `MUSA_ERROR_SHARED_OBJECT_INIT_FAILED` | [src/loader/loader.cpp:67-71] |
| 导出符号解析为 `nullptr`（惰性检查） | `MUSA_ERROR_SHARED_OBJECT_SYMBOL_NOT_FOUND` | [src/loader/loader.cpp:180-208] |

诊断手段：`probe.cpp` 只调用驱动定位函数并打印包 ID 与路径（[src/loader/probe.cpp:7]、[src/loader/probe.cpp:26]），是定位 loader 层问题的正确工具——**但它在本快照下不被构建**（[src/loader/CMakeLists.txt:74-93] 整段注释）。

## 1. Driver/Core 层失败

| 触发点 | 源码行为 | 工具可观察结果 | 状态 |
|---|---|---|---|
| `muInit(非 0)` | `muapiInit` 返回 `MUSA_ERROR_INVALID_VALUE` | stderr 打印 `checkMusaErrors` 后进程退出 | 已确认 [src/driver/mu_context.cpp:124-130; src/tools/muInfo.cpp:9-20] |
| HAL 创建失败 | `Platform::Init` 记录错误并写 `m_InitStatus` | `muInit` 检查失败后退出 | 已确认 [src/musa/core/platform.cpp:85-140] |
| 枚举到 0 设备 | HAL platform `CreateDevices` 返回 `errorNoDevice`，Core 也把空可见列表转成 `MUSA_ERROR_NO_DEVICE` | 初始化失败，不能进入设备循环 | 已确认 [src/hal/m3d/platform.cpp:128-138; src/musa/core/platform.cpp:105-117] |
| 可见设备过滤为空 | `GetVisibleDeviceIndices` 结果为空 | `Platform::Init` 返回 `MUSA_ERROR_NO_DEVICE` | 已确认 [src/musa/core/platform.cpp:111-117] |
| primary context retain 无效 ordinal | `Platform::GetDevice` 返回空 | `MUSA_ERROR_INVALID_DEVICE`，工具退出 | 已确认 [src/driver/mu_context.cpp:476-480] |
| `Device::Finalize` 失败 | `LaterInit` 保存失败状态 | primary retain 失败，工具退出 | 已确认 [src/musa/core/device.cpp:911-925] |
| 缺 CDM queue family | `LaterInit` 返回 `MUSA_ERROR_OUT_OF_MEMORY` | primary retain 失败 | 已确认 [src/musa/core/device.cpp:888-921] |
| 某个属性 API 失败 | `checkMuErrors` 立即退出 | 后续属性和设备不会打印 | 已确认 [src/tools/muInfo.cpp:123-267] |
| `muDeviceGetName` 空 name | API 返回 `MUSA_ERROR_INVALID_VALUE` | 退出 | 已确认 [src/driver/mu_device.cpp:105-108] |
| `muMemGetInfo_v2` 无当前 context | 返回 `MUSA_ERROR_INVALID_CONTEXT` | 退出；正常 `main` 已先 set current | 已确认 [src/driver/mu_memory.cpp:542-560] |
| `free/total` 数值异常 | 当前源码未在工具层检查 `total == 0` | 可能出现除零/非预期百分比；需运行验证 | 推断 |

## 诊断顺序

先看初始化返回值，再看 `Platform::Init` 的 `m_InitStatus`，然后看 `LaterInit` 的 `Finalize`/queue family，最后定位 `GetDeviceProps` 当前 enum。不要只根据最终 stderr 的数字猜测 HAL 根因；工具的错误输出只包含最近失败 API 的文件和行号。

## 相关文档

- [`../../00-overview/analysis-state.md`](../../00-overview/analysis-state.md)
- [`../../01-modules/module-registry.md`](../../01-modules/module-registry.md)

## 源码证据摘要

- `src/loader/driver_resolver.cpp`（6 处，例：[src/loader/driver_resolver.cpp:71-79]）
- `src/loader/loader.cpp`（2 处，例：[src/loader/loader.cpp:67-71]）
- `src/loader/probe.cpp`（2 处，例：[src/loader/probe.cpp:7]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 风险与未决项见知识库风险登记。

## 下一步阅读建议

阅读 [`../../00-overview/analysis-state.md`](../../00-overview/analysis-state.md)；阅读 [`../../01-modules/module-registry.md`](../../01-modules/module-registry.md)。
