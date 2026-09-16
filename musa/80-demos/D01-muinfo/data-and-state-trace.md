# D01：`muInfo` 数据、状态与控制点

## 主状态转移

```text
未初始化
  -- muInit(0) --> Platform::m_InitStatus == MUSA_SUCCESS
  -- 失败 --> checkMuErrors 退出进程

平台成功、TLS 无当前 context
  -- muDevicePrimaryCtxRetain + muCtxSetCurrent --> TLS context stack 顶部 = primary Context

TLS 有 context
  -- GetDeviceProps --> static DeviceProp vector（首次建立，之后只读缓存）
  -- muMemGetInfo_v2 --> 当前 Context 的 parent Device / HAL memory state
```

## 字段来源矩阵

| `muInfo` 字段 | API | Core/HAL 来源 | 证据 |
|---|---|---|---|
| name | `muDeviceGetName` | `IDevice::GetName()` 返回 HAL properties.name | [src/tools/muInfo.cpp:129], [src/driver/mu_device.cpp:96-123], [src/musa/core/device.h:71] |
| totalGlobalMem | `muDeviceTotalMem_v2` | `IDevice::GetTotalMem()` | [src/tools/muInfo.cpp:131], [src/driver/mu_device.cpp:126-148] |
| max threads/grid | `muDeviceGetAttribute` | `Device::GetAttribute` switch → `computeProperties` | [src/tools/muInfo.cpp:133-139], [src/musa/core/device.cpp:40-64] |
| shared/constant/cache | `muDeviceGetAttribute` | `memoryProperties` / `ipProperties` | [src/tools/muInfo.cpp:141-200], [src/musa/core/device.cpp:65-104] |
| UUID | `muDeviceGetUuid_v2` | `GetProperties().uuid` | [src/tools/muInfo.cpp:267], [src/driver/mu_device.cpp:234-254] |
| peers | `muDeviceCanAccessPeer` | peer API 的设备对能力判断 | [src/tools/muInfo.cpp:341-359], [src/driver/mu_peer.cpp:13-...] |
| free memory | `muMemGetInfo_v2` | `Device::QueryGlobalMemFreeSize` | [src/tools/muInfo.cpp:363-369], [src/driver/mu_memory.cpp:542-560] |

## 重要的别名/缓存关系

1. `MUdevice` 是由 `pDevice->GetId()` 写入的整数句柄；查询时再通过 `Platform::GetIDeviceView(dev)` 找回内部设备视图。[src/driver/mu_device.cpp:14-37,71-94]
2. `muDeviceGet` 使用 `GetIDeviceView`，而 `muDevicePrimaryCtxRetain` 使用 `Platform::GetDevice`；后者会调用 `LaterInit`，不能把两个 API 的初始化副作用混为一谈。[src/driver/mu_context.cpp:472-488; src/musa/core/platform.cpp:140-147]
3. `GetDeviceProps` 的静态缓存意味着第一次 `printDeviceProp` 可能触发大量 API 调用，后续设备打印只取缓存；这是 C++ 局部静态初始化的源码事实，初始化时刻尚未在二进制上运行验证。[src/tools/muInfo.cpp:119-124,269-274]
