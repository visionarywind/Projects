# D01：`muInfo` 设备信息查询——完整源码轨迹

- 入口源码：`src/tools/muInfo.cpp`
- 对应版本：`b8dce2b2f23849e8e99350c20d7eaac3c129caba`
- 证据状态：下面的控制流和调用目标均来自静态源码；目标机设备数量、实际 stdout、错误码发生位置仍未验证。

## 1. 从进程入口到设备循环

```text
main()                                      [src/tools/muInfo.cpp:372-388]
├─ printCompilerInfo()                      [src/tools/muInfo.cpp:373, 276-278]
│  └─ printf("compiler: mcc")
├─ muInit(0)                                [src/tools/muInfo.cpp:375]
│  └─ 对外 API 分发到 muapiInit(0)
│     ├─ flags != 0 ? INVALID_VALUE
│     └─ Musa::CreatePlatform(nullptr)       [src/driver/mu_context.cpp:121-133]
│        └─ Platform::Get().Init()           [src/musa/core/lib.cpp:14-18;
│                                             src/musa/core/platform.cpp:84-138]
│           ├─ std::call_once，保证一次初始化
│           ├─ Hal::CreatePlatform(createInfo, &m_pHalPlatform)
│           │  └─ new Hal::M3d::Platform -> Platform::Init
│           │     [src/hal/m3d/lib.cpp:6-14; src/hal/m3d/platform.cpp:102-113]
│           │     ├─ IM3d::CreatePlatform(...) [src/hal/m3d/platform.cpp:116-126]
│           │     └─ EnumerateDevices -> GetProperties -> new M3d::Device -> Device::Init
│           │        [src/hal/m3d/platform.cpp:128-189]
│           ├─ 读取 HAL device 数量和指针
│           ├─ 按 PCI/IP 排序，再按 visibleDevices 生成可见设备索引
│           └─ 为每个可见设备创建 Musa::Device(seqId, halDevice)
│              [src/musa/core/platform.cpp:105-118]
├─ muDeviceGetCount(&deviceCount)             [src/tools/muInfo.cpp:377-378]
│  └─ muapiDeviceGetCount
│     └─ Platform::Get().GetDeviceCount()     [src/driver/mu_device.cpp:40-51]
└─ for (i = 0; i < deviceCount; ++i)           [src/tools/muInfo.cpp:380-385]
   ├─ muDevicePrimaryCtxRetain(&ctx, i)
   │  └─ Platform::Get().GetDevice(i)
   │     └─ 可能触发 Device::LaterInit()（call_once）
   │        ├─ HAL Device::Finalize(finalizeInfo)
   │        ├─ 扫描 queue families，记录 CDM/TDM/CE 等 engine index
   │        ├─ 初始化 primary Context
   │        └─ 初始化 copy managers / UniversalManager
   │        [src/driver/mu_context.cpp:472-488;
   │         src/musa/core/platform.cpp:140-147;
   │         src/musa/core/device.cpp:856-940]
   ├─ muCtxSetCurrent(ctx)
   │  └─ TlsCtxPop/Push，更新当前线程的 context 栈
   │     [src/driver/mu_context.cpp:280-295]
   └─ printDeviceProp(i)                       [src/tools/muInfo.cpp:283-370]
```

## 2. `printDeviceProp` 的实际子调用

### 2.1 属性缓存不是一次一项的直线查询

`GetDeviceProps(dev)` 内部的 `static std::vector<DeviceProp> devProps` 由 lambda 初始化。第一次调用时执行一次完整采集，后续直接返回 `devProps[dev]`；因此它不是每次打印都重新访问设备。[src/tools/muInfo.cpp:119-274]

```text
GetDeviceProps(dev)
└─ lambda 首次执行
   ├─ muDeviceGetCount(&devCnt)                         [123-124]
   └─ for each ordinal i                                [125-270]
      ├─ muDeviceGet(&device, i)                       [126-127]
      │  └─ muapiDeviceGet -> Platform::GetIDeviceView
      │     -> pDevice->GetId()                         [src/driver/mu_device.cpp:14-37]
      ├─ muDeviceGetName(prop.name, ..., device)        [129]
      │  └─ GetIDeviceView -> IDevice::GetName           [src/driver/mu_device.cpp:96-123;
      │                                                  src/musa/core/device.h:71]
      ├─ muDeviceTotalMem_v2(&prop.totalGlobalMem,...)  [131]
      │  └─ GetIDeviceView -> IDevice::GetTotalMem      [src/driver/mu_device.cpp:126-148]
      ├─ 多次 muDeviceGetAttribute(&field, enum, device) [133-265]
      │  └─ GetIDeviceView -> Device::GetAttribute
      │     └─ switch(enum)，从 HAL DeviceProperties 的
      │        compute/memory/ip/texture/board 字段填充 *pi
      │        [src/driver/mu_device.cpp:71-94;
      │         src/musa/core/device.cpp:40-104]
      └─ muDeviceGetUuid_v2(&prop.uuid, device)          [267]
         └─ GetIDeviceView -> GetProperties().uuid -> memcpy
            [src/driver/mu_device.cpp:234-254]
```

文件中 `muDeviceGetAttribute` 的调用不是抽象的“查询能力”：每个字段有明确 enum 和目标成员。例如线程/网格维度在 [src/tools/muInfo.cpp:133-139]，内存与 cache 字段在 [src/tools/muInfo.cpp:145-200]，纹理/表面限制在 [src/tools/muInfo.cpp:206-231]。

### 2.2 输出阶段的调用

```text
printDeviceProp(i)
├─ GetDeviceProps(i)                                   [294-295]
├─ muDriverGetVersion(&driverVersion)                   [298]
├─ 输出 name/version/PCI/compute/memory/limits          [302-339]
├─ muDeviceGetCount(&deviceCnt)                         [342]
├─ 对每个 j：muDeviceCanAccessPeer(&isPeer, j, i)      [346,355]
│  └─ MUSA peer API；将设备分到 peers / non-peers
└─ muMemGetInfo_v2(&free, &total)                       [364]
   └─ TlsCtxTop() -> 当前 Context -> Device
      ├─ total = Device::GetProperties().totalGlobalMem
      └─ free = Device::QueryGlobalMemFreeSize(free)
      [src/driver/mu_memory.cpp:542-560]
```

## 3. 数据和所有权

| 数据 | 创建/填充 | 使用 | 生命周期 |
|---|---|---|---|
| `Platform` 单例 | `Platform::Get()` | 所有 `muapi*` 初始化/设备查询 | 进程内静态对象 |
| HAL device 指针 | M3D `EnumerateDevices` 后交给 HAL platform | `Musa::Device` 持有观察关系 | 由 HAL platform 管理；静态分析未确认所有析构顺序 |
| `Musa::Device` | `Platform::Init` 的 `m_Devices.emplace_back` | API 设备视图、primary context | `Platform` 成员容器 |
| primary `Context` | `Device` 构造时创建，`LaterInit` 调 `Init` | TLS 当前 context、`muMemGetInfo` | Device 成员；retain/release 计数语义需结合完整 Context 实现验证 |
| `DeviceProp` | `GetDeviceProps` lambda 栈上 `prop`，再 `ret.emplace_back` | `printDeviceProp` | 静态 vector，进程内缓存 |

## 4. 错误路径（源码可确认）

- `muInit` 的 flags 非 0 返回 `MUSA_ERROR_INVALID_VALUE`。[src/driver/mu_context.cpp:124-130]
- HAL platform 创建失败时，`Platform::Init` 记录错误并把状态写入 `m_InitStatus`；无可见设备时返回 `MUSA_ERROR_NO_DEVICE`。[src/musa/core/platform.cpp:97-117]
- 设备 ordinal 越界时 `muapiDeviceGet` 写入 `-1` 并返回 `MUSA_ERROR_INVALID_DEVICE`。[src/driver/mu_device.cpp:22-33]
- 属性查询在无设备、空输出指针、无效 device 时分别返回对应错误；具体 `Device::GetAttribute` 对未支持 enum 的分支需继续读取该函数后半段确认。[src/driver/mu_device.cpp:71-94]
- 任一 `checkMuErrors` 失败都会先调用 `muGetErrorString`，然后向 stderr 打印文件/行号并 `exit(EXIT_FAILURE)`，不会继续打印后续字段。[src/tools/muInfo.cpp:9-20]
- `muMemGetInfo_v2` 没有当前 TLS context 时返回 `MUSA_ERROR_INVALID_CONTEXT`。[src/driver/mu_memory.cpp:542-560]

## 5. 运行时未验证项

本地没有目标机的 `libmusa.so`、GPU 和该远端构建产物，因此不能填写真实设备名、设备数、内存值、peer 矩阵或 stdout。构建/运行命令只能作为待验证步骤，不能视为本次执行结果。
