# 平台与设备管理：调用链

- 模块：M03 Platform / Device
- 证据状态：从 Driver init 到 HAL/M3D platform/device 枚举已静态确认；M3D 子模块内部再往内核驱动的路径仍未运行验证。

## CHAIN-M03-INIT：平台创建与设备枚举

```text
muapiInit(flags)                                           [src/driver/mu_context.cpp:121-133]
  -> Musa::CreatePlatform(nullptr)                         [src/musa/core/lib.cpp:14-18]
  -> Platform::Get().Init()                                [src/musa/core/platform.cpp:84-138]
     ├─ SettingsLoader.Init / InitCoreDumpAttribute
     ├─ Hal::CreatePlatform(createInfo, &m_pHalPlatform)   [src/musa/core/platform.cpp:90-98]
     │  -> new Hal::M3d::Platform                          [src/hal/m3d/lib.cpp:6-14]
     │  -> M3d::Platform::Init                             [src/hal/m3d/platform.cpp:102-113]
     │     ├─ IM3d::CreatePlatform                         [src/hal/m3d/platform.cpp:116-126]
     │     └─ CreateDevices()
     │        ├─ m_M3dPlatform->EnumerateDevices           [src/hal/m3d/platform.cpp:128-138]
     │        ├─ m3dDevice->GetProperties                  [src/hal/m3d/platform.cpp:140-163]
     │        ├─ new Hal::M3d::Device + Init               [src/hal/m3d/platform.cpp:180-189]
     │        └─ CreateDevicesP2pTopoMatrix                [src/hal/m3d/platform.cpp:192-195]
     ├─ Core 读取 HAL device count / device array           [src/musa/core/platform.cpp:105-107]
     ├─ ReorderHalDevices                                  [src/musa/core/platform.cpp:109]
     ├─ GetVisibleDeviceIndices                            [src/musa/core/platform.cpp:111]
     └─ new Musa::Device(seqId, halDevice)                 [src/musa/core/platform.cpp:115-117]
```

## CHAIN-M03-LATER-INIT：设备惰性初始化

```text
Platform::GetDevice(ordinal)                               [src/musa/core/platform.cpp:140-147]
  -> Device::LaterInit()                                   [src/musa/core/device.cpp:856-940]
     ├─ 构造 Hal::DeviceFinalizeInfo（超时、调度、user queue、FP exception 等）
     ├─ m_pHalDevice->Finalize(finalizeInfo)
     ├─ GetQueueFamilies，映射 CDM/TDM/CE/ACE/DMA/UNIVERSAL/MMU
     ├─ 检查 CDM 必须存在
     ├─ primary Context::Init
     ├─ SetLLCPersistingProperties
     ├─ InitCopyManagers
     └─ Init UniversalManager（后续行需继续展开）
```

## CHAIN-M03-QUERY：属性查询

```text
muDeviceGetAttribute
  -> Platform::GetIDeviceView(dev)                         [src/musa/core/platform.cpp:358-364]
  -> Device::GetAttribute(pi, attrib)                      [src/musa/core/device.cpp:40-104]
     -> const Hal::DeviceProperties& properties = m_pHalDevice->GetProperties()
     -> switch attrib -> computeProperties / memoryProperties / ipProperties / textureProperties / boardProperties
```

## 关键边界

- `GetIDeviceView`：只返回已存在 device 视图，不触发 `LaterInit`。
- `GetDevice`：会调用 `LaterInit`，因此 `muDevicePrimaryCtxRetain` 的副作用比普通属性查询更重。
- `M3d::Platform::CreateDevices` 已确认会调用 `IM3d::IDevice::GetProperties`，但这些 properties 如何来自内核驱动仍属于 M3D 内部/运行时验证范围。
