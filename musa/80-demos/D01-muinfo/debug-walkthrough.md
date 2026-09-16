# D01：`muInfo` 调试 walkthrough

## 推荐断点（按真实调用顺序）

1. `main` 的 `muInit(0)`：确认 API 返回值。[src/tools/muInfo.cpp:372-378]
2. `muapiInit`：观察 flags 检查和 `Musa::CreatePlatform(nullptr)`。[src/driver/mu_context.cpp:121-133]
3. `Platform::Init`：观察 `std::call_once`、HAL 创建、设备枚举、visible device 筛选和 `m_Devices.emplace_back`。[src/musa/core/platform.cpp:84-138]
4. `Hal::M3d::Platform::CreatePlatform/CreateDevices`：确认 `IM3d::CreatePlatform` 和 `EnumerateDevices` 的边界。[src/hal/m3d/platform.cpp:116-189]
5. `muapiDeviceGetCount`：确认返回的 `m_Devices.size()`。[src/driver/mu_device.cpp:40-51]
6. `muapiDevicePrimaryCtxRetain`：确认 `Platform::GetDevice` 是否进入 `Device::LaterInit`，以及 `Finalize` 是否成功。[src/driver/mu_context.cpp:472-488; src/musa/core/device.cpp:856-940]
7. `muapiCtxSetCurrent`：检查 TLS context stack 的 push/pop。[src/driver/mu_context.cpp:280-295]
8. `GetDeviceProps` 的 lambda：检查第一次静态初始化，以及失败时 `checkMuErrors` 的退出点。[src/tools/muInfo.cpp:9-20,119-274]
9. `Device::GetAttribute`：用 `attrib` 对照 switch，确认字段来自哪一组 HAL properties。[src/musa/core/device.cpp:40-104]
10. `muapiMemGetInfo_v2`：确认 free 值经过 `QueryGlobalMemFreeSize`，total 值直接来自 Device properties。[src/driver/mu_memory.cpp:542-560]

## 建议记录的变量

| 位置 | 变量 | 观察目的 |
|---|---|---|
| `Platform::Init` | `status`, `deviceCount`, `visibleDeviceIndices` | 区分 HAL 无设备、可见设备过滤为空、CommitSettings 失败 |
| `muapiDeviceGet` | `ordinal`, `pDevice`, `*device` | 区分 ordinal 错误和句柄生成 |
| `Device::LaterInit` | `queueFamilyCount`, `m_EngineProperties` | 确认 CDM 队列是否存在及各 engine 映射 |
| `GetDeviceProps` | `devCnt`, 当前 `i`, 当前 attribute | 定位某个属性不支持/返回错误的位置 |
| `muapiMemGetInfo_v2` | `TlsCtxTop()`, `free`, `total` | 确认 TLS context 是否被设置及内存查询来源 |

## 不应作出的推断

- 不能因为 `src/tools/CMakeLists.txt` 构建了 `muInfo`，就声称本地已经运行成功。
- 不能把 `Platform::Init` 中的 HAL 接口调用写成底层内核驱动已执行；`IM3d::CreatePlatform` 之后的私有实现需要目标源码/运行日志支持。
- 不能把 `GetDeviceProps` 的静态缓存当成实时监控数据。
