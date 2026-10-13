# 平台与设备管理：接口契约

- 文档目的：platform device：接口签名、前置条件、错误码与跨层契约。
- 适用范围：M03 模块的《平台与设备管理：接口契约》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


## 本模块暴露的抽象

- `IPlatform`：平台级接口，定义 `GetDeviceCount`/`GetDevice`/`GetDeviceByPCIBusId`/`GetP2PAttribute`/`GetIDeviceView`/`CreateMemory`/`DestroyMemory`/`GetMemoryByDevicePointer`/`GetMemoryByHandle`/`GetCoreDumpAttribute`/`SetCoreDumpAttribute`。[src/musa/musaPlatform.h:27-165]
- `IDevice`：设备级接口，定义 `GetId`/`GetAttribute`/`CreateContext`/`CreateGreenContext`/`DestroyContext`/`GetPrimaryContext`/`GetDefaultMemoryPool`/`CanAccessPeer`/`GetPCIBusId` 等。[src/musa/musaDevice.h:25-205]

## 接口方向表

| 接口方向 | 输入 | 输出/副作用 | 前置条件 | 错误 |
|---|---|---|---|---|
| 上游 -> 本模块 | ordinal / pciBusId / flags / MUdevice_attribute | device 视图、属性值、上下文 | 平台已初始化、index 在范围内 | `GetDevice` 返回 nullptr |
| 本模块 -> HAL | `Hal::PlatformCreateInfo`、`Hal::DeviceFinalizeInfo`、queue/memory 参数 | 底层资源/提交 | HAL device 有效 | `HalToMuResult` 转换 |
| 观测 -> 本模块 | MUgdb/MUasan accessor 请求 | ID、初始化状态 | 对象仍存活 | 访问失败/空值 |

## 关键语义

- `GetDevice(ordinal)` 会触发 `Device::LaterInit()`，属性查询前的副作用更重；`GetIDeviceView(seqID)` 不触发。[src/musa/core/platform.cpp:141-150] [src/musa/core/platform.cpp:378-384]
- `CreateMemory` 当前仅支持 `memoryTypeVirtual`，其余类型返回 `MUSA_ERROR_INVALID_VALUE`。[src/musa/core/platform.cpp:386-403]
- `GetDeviceByPCIBusId` 支持 5 种 PCI 字符串长度格式（7/10/12/14/16 字符）。[src/musa/core/platform.cpp:152-179]
- P2P 属性查询的 `MU_DEVICE_P2P_ATTRIBUTE_PERFORMANCE_RANK` 语义在 CUDA 上存在已知怪癖，代码注释保留说明。[src/musa/core/platform.cpp:203-224]

公共 ABI 兼容性是 M02 的硬约束；其他模块应避免直接改变句柄布局和导出表顺序。此处的“应”是开发约束，不是源码事实。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/musa/core/platform.cpp`（5 处，例：[src/musa/core/platform.cpp:141-150]）
- `src/musa/musaPlatform.h`（1 处，例：[src/musa/musaPlatform.h:27-165]）
- `src/musa/musaDevice.h`（1 处，例：[src/musa/musaDevice.h:25-205]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
