# 平台与设备管理：数据结构与所有权

- 文档目的：platform device：核心数据结构、字段含义与持有关系。
- 适用范围：M03 模块的《平台与设备管理：数据结构与所有权》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


## Platform（进程内单例）

- `Platform::Get()` 返回函数内静态对象；构造函数把 `m_InitStatus` 初始化为 `MUSA_ERROR_NOT_INITIALIZED`。[src/musa/core/platform.cpp:14-31]
- 关键成员声明见 [src/musa/core/platform.h:141-171]：
  - `m_pHalPlatform`：HAL 平台接口指针，在 `Init()` 中由 `Hal::CreatePlatform` 创建。[src/musa/core/platform.cpp:91-98]
  - `m_Devices`：`std::vector<std::unique_ptr<Device>>`，与可见设备一一对应。
  - `m_MemoryTracker`：虚拟内存/句柄登记表，`CreateMemory`/`GetMemoryByDevicePointer` 依赖它。[src/musa/core/platform.cpp:386-411]
  - `m_DefaultNumaPools`/`m_CurrentNumaPools`：按 NUMA id 索引的默认/当前 memory pool 数组。
  - `m_DefaultHostMemoryPools`/`m_CurrentHostMemoryPools`：Host 侧默认/当前 pool 数组。
  - `m_DevResourceDescs`：devResource 描述符集合，配 `m_DevResourceDescMutex` 使用。
  - `m_MemorySeqID`：内存顺序号计数器。
  - `m_Executor`：异步执行器，Windows 退出时需显式 `Shutdown()`。[src/musa/core/platform.cpp:620-624]

## Platform::CriticalBase（跨线程登记集合）

- 由 `ReadWriteLockedBase<std::shared_mutex>` 派生，内部含 ctx、library、memoryPool、detachedStream、capturedStream 五个 `unordered_set`。[src/musa/core/platform.h:63-93]
- 所有 `Locked*` 方法通过 `WriteLockedAccessor`/`ReadLockedAccessor` 读写集合，例如 `LockedAddCtx`、`LockedFindCtx`。[src/musa/core/platform.cpp:430-446]

## Device

- `Device` 持有 HAL 设备指针与缓存的 `musaDeviceProp`（`m_Props`），属性查询与导出共用该缓存。[src/musa/core/device.h:236-242]
- 关键成员声明见 [src/musa/core/device.h:247-275]：
  - `m_PrimaryCtx`：primary context，`Device` 构造时创建并注册进 Platform。[src/musa/core/device.cpp:690-716]
  - `m_DefaultMemoryPools`/`m_CurrentMemoryPools`/`m_GraphMemoryPool`：设备侧 pool。[src/musa/core/device.cpp:451-561]
  - `m_CpyMgrs`：CopyManager 数组，`LaterInit` 里初始化。[src/musa/core/device.cpp:1069-1153]
  - `m_EngineProperties`：engine 到 queue family index 的映射。
  - `m_CriticalData`：设备级 context 集合（`Device::CriticalBase`）。
  - `m_DeviceMpxMask`：SM 资源掩码，`FillProperties` 从 `multiCoreClusterMask` 推导。[src/musa/core/device.cpp:906-908]
- `Engine` 枚举列出设备引擎（cdm/tce/tdm/ce/dma/ace/universal/mmu）。[src/musa/core/device.h:177-187]

## 接口层

- `IPlatform`：平台级抽象，定义设备计数/按序或按 PCI 查找/P2P 属性/虚拟内存创建与查找/coredump 属性等纯虚接口。[src/musa/musaPlatform.h:27-165]
- `IDevice`：设备级抽象，定义属性查询、上下文创建销毁、primary context、memory pool、peer 访问、PCI bus id、BVH 接口。[src/musa/musaDevice.h:25-205]

## 所有权规则

- `Platform` 拥有全部 `Device`（`unique_ptr`），析构时依次释放。
- `Device` 拥有 primary context 与各 memory pool；用户 context 由设备集合登记、由调用方销毁。
- 虚拟内存：`CreateMemory` 以 `std::shared_ptr` 创建并交 `m_MemoryTracker` 登记，`DestroyMemory` 只负责解登记。[src/musa/core/platform.cpp:386-403] [src/musa/core/platform.cpp:413-428]
- `DevResourceDesc` 由 Platform 登记并统一 `delete` 释放。[src/musa/core/platform.cpp:602-608]

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/musa/core/platform.cpp`（8 处，例：[src/musa/core/platform.cpp:14-31]）
- `src/musa/core/device.cpp`（4 处，例：[src/musa/core/device.cpp:690-716]）
- `src/musa/core/device.h`（3 处，例：[src/musa/core/device.h:236-242]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
