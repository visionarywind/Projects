# D01：`muInfo` 失败路径

| 触发点 | 源码行为 | 工具可观察结果 | 状态 |
|---|---|---|---|
| `muInit(非 0)` | `muapiInit` 返回 `MUSA_ERROR_INVALID_VALUE` | stderr 打印 `checkMusaErrors` 后进程退出 | 已确认 [src/driver/mu_context.cpp:124-130; src/tools/muInfo.cpp:9-20] |
| HAL 创建失败 | `Platform::Init` 记录错误并写 `m_InitStatus` | `muInit` 检查失败后退出 | 已确认 [src/musa/core/platform.cpp:97-100,132-138] |
| 枚举到 0 设备 | HAL platform `CreateDevices` 返回 `errorNoDevice`，Core 也把空可见列表转成 `MUSA_ERROR_NO_DEVICE` | 初始化失败，不能进入设备循环 | 已确认 [src/hal/m3d/platform.cpp:128-138; src/musa/core/platform.cpp:105-117] |
| 可见设备过滤为空 | `GetVisibleDeviceIndices` 结果为空 | `Platform::Init` 返回 `MUSA_ERROR_NO_DEVICE` | 已确认 [src/musa/core/platform.cpp:111-117] |
| primary context retain 无效 ordinal | `Platform::GetDevice` 返回空 | `MUSA_ERROR_INVALID_DEVICE`，工具退出 | 已确认 [src/driver/mu_context.cpp:476-480] |
| `Device::Finalize` 失败 | `LaterInit` 保存失败状态 | primary retain 失败，工具退出 | 已确认 [src/musa/core/device.cpp:881-886] |
| 缺 CDM queue family | `LaterInit` 返回 `MUSA_ERROR_OUT_OF_MEMORY` | primary retain 失败 | 已确认 [src/musa/core/device.cpp:888-921] |
| 某个属性 API 失败 | `checkMuErrors` 立即退出 | 后续属性和设备不会打印 | 已确认 [src/tools/muInfo.cpp:123-267] |
| `muDeviceGetName` 空 name | API 返回 `MUSA_ERROR_INVALID_VALUE` | 退出 | 已确认 [src/driver/mu_device.cpp:105-108] |
| `muMemGetInfo_v2` 无当前 context | 返回 `MUSA_ERROR_INVALID_CONTEXT` | 退出；正常 `main` 已先 set current | 已确认 [src/driver/mu_memory.cpp:542-560] |
| `free/total` 数值异常 | 当前源码未在工具层检查 `total == 0` | 可能出现除零/非预期百分比；需运行验证 | 推断 |

## 诊断顺序

先看初始化返回值，再看 `Platform::Init` 的 `m_InitStatus`，然后看 `LaterInit` 的 `Finalize`/queue family，最后定位 `GetDeviceProps` 当前 enum。不要只根据最终 stderr 的数字猜测 HAL 根因；工具的错误输出只包含最近失败 API 的文件和行号。
