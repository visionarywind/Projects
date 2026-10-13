# 平台与设备管理：调用链

- 模块：M03 Platform / Device
- 文档目的：platform device：关键调用链：从公共入口到真实副作用的追踪。
- 适用范围：M03 模块的《平台与设备管理：调用链》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：从 Driver init 到 HAL/M3D platform/device 枚举已静态确认；M3D 子模块内部再往内核驱动的路径仍未运行验证。
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)

## CHAIN-M03-INIT：平台创建与设备枚举

```text
muapiInit(flags)                                           [src/driver/mu_context.cpp:121-133]
  -> Musa::CreatePlatform(nullptr)                         [src/musa/core/lib.cpp:14-18]
  -> Platform::Get().Init()                                [src/musa/core/platform.cpp:85-139]
     ├─ SettingsLoader.Init / InitCoreDumpAttribute       [src/musa/core/platform.cpp:88-89]
     ├─ Hal::CreatePlatform(createInfo, &m_pHalPlatform)   [src/musa/core/platform.cpp:98]
     │  -> new Hal::M3d::Platform                          [src/hal/m3d/lib.cpp:6-14]
     │  -> M3d::Platform::Init                             [src/hal/m3d/platform.cpp:102-113]
     │     ├─ IM3d::CreatePlatform                         [src/hal/m3d/platform.cpp:116-126]
     │     └─ CreateDevices()
     │        ├─ m_M3dPlatform->EnumerateDevices           [src/hal/m3d/platform.cpp:128-138]
     │        ├─ m3dDevice->GetProperties                  [src/hal/m3d/platform.cpp:140-163]
     │        ├─ new Hal::M3d::Device + Init               [src/hal/m3d/platform.cpp:180-189]
     │        └─ CreateDevicesP2pTopoMatrix                [src/hal/m3d/platform.cpp:192-195]
     ├─ Core 读取 HAL device count / device array           [src/musa/core/platform.cpp:106-108]
     ├─ ReorderHalDevices（按 PCI 或 IP+PCI 排序）          [src/musa/core/platform.cpp:110]
     ├─ GetVisibleDeviceIndices（可见性筛选）               [src/musa/core/platform.cpp:112]
     └─ new Musa::Device(seqId, halDevice)                 [src/musa/core/platform.cpp:116-118]
```

## CHAIN-M03-LATER-INIT：设备惰性初始化

```text
Platform::GetDevice(ordinal)                               [src/musa/core/platform.cpp:141-150]
  -> Device::LaterInit()                                   [src/musa/core/device.cpp:911-1017]
     ├─ 构造 Hal::DeviceFinalizeInfo（超时、调度、user queue、FP exception 等） [src/musa/core/device.cpp:917-939]
     ├─ m_pHalDevice->Finalize(finalizeInfo)               [src/musa/core/device.cpp:941]
     ├─ GetQueueFamilies，映射 CDM/TDM/CE/ACE/TCE/DMA/UNIVERSAL/MMU [src/musa/core/device.cpp:948-980]
     ├─ 检查 CDM 或 TCE 至少一个有效                         [src/musa/core/device.cpp:981-986]
     ├─ primary Context::Init                               [src/musa/core/device.cpp:988-993]
     ├─ SetLLCPersistingProperties（置为最大）              [src/musa/core/device.cpp:995-996]
     ├─ InitCopyManagers                                    [src/musa/core/device.cpp:998-1002]
     └─ Init UniversalManager（universal engine 有效时）     [src/musa/core/device.cpp:1004-1011]
```

## CHAIN-M03-CTX：上下文创建

```text
Device::CreateContext(ppContext, flags)                    [src/musa/core/device.cpp:635-675]
  -> 读 device CriticalBase 统计 context 数                 [src/musa/core/device.cpp:637-641]
  -> 按 flags 选 ScheduleMode（AUTO/SPIN/YIELD/BLOCKING_SYNC） [src/musa/core/device.cpp:642-661]
  -> new Context(this, flags) + ctx->Init()                [src/musa/core/device.cpp:663-664]
  -> LockedAddContext / SetWaitMode / Retain               [src/musa/core/device.cpp:668-670]
  -> 失败时 delete ctx                                      [src/musa/core/device.cpp:671-673]
```

## CHAIN-M03-QUERY：属性查询

```text
muDeviceGetAttribute
  -> Platform::GetIDeviceView(dev)                         [src/musa/core/platform.cpp:378-384]
  -> Device::GetAttribute(pi, attrib)                      [src/musa/core/device.cpp:42-448]
     -> const Hal::DeviceProperties& properties = m_pHalDevice->GetProperties()  [src/musa/core/device.cpp:44]
     -> switch attrib -> computeProperties / memoryProperties / ipProperties / textureProperties / boardProperties
     -> 未识别且 attrib < MU_DEVICE_ATTRIBUTE_MAX 时 *pi = 0，否则 MUSA_ERROR_INVALID_VALUE [src/musa/core/device.cpp:439-445]
```

## CHAIN-M03-DESTROY：进程退出清理

```text
进程退出 -> ~Platform()                                    [src/musa/core/platform.cpp:610-656]
  -> 各 Device::LockedWait() 收集 device ids                [src/musa/core/platform.cpp:614-618]
  -> Windows 下 m_Executor.Shutdown()                      [src/musa/core/platform.cpp:620-624]
  -> ReleaseUnfreedMemories() 释放未回收内存               [src/musa/core/platform.cpp:582-600]
  -> MUgdb::CloseDebugger()                                [src/musa/core/platform.cpp:628]
  -> 删除默认 NUMa/Host memory pools                       [src/musa/core/platform.cpp:630-645]
  -> m_Devices.clear()（触发 ~Device）                     [src/musa/core/platform.cpp:647]
  -> ReleaseDevResourceDescs()                             [src/musa/core/platform.cpp:649]
  -> MUasan::DeinitDevice / Hal::DestroyPlatform           [src/musa/core/platform.cpp:651-655]
```

## 关键边界

- `GetIDeviceView`：只返回已存在 device 视图，不触发 `LaterInit`。[src/musa/core/platform.cpp:378-384]
- `GetDevice`：会调用 `Device::LaterInit()` 并在成功后 `MUasan::InitDevice(ordinal)`，因此首次取设备副作用比普通属性查询更重。[src/musa/core/platform.cpp:141-150]
- `Init` 用 `std::call_once` 保护，`m_InitStatus` 在初始化末尾才写入，保证其他线程看到的是完整初始化结果。[src/musa/core/platform.cpp:86-87] [src/musa/core/platform.cpp:133-138]
- `M3d::Platform::CreateDevices` 已确认会调用 `IM3d::IDevice::GetProperties`，但这些 properties 如何来自内核驱动仍属于 M3D 内部/运行时验证范围。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/musa/core/platform.cpp`（22 处，例：[src/musa/core/platform.cpp:85-139]）
- `src/musa/core/device.cpp`（18 处，例：[src/musa/core/device.cpp:911-1017]）
- `src/hal/m3d/platform.cpp`（6 处，例：[src/hal/m3d/platform.cpp:102-113]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
