# 平台与设备管理：行级分析

- 文档目的：platform device：关键代码位置的行级证据与解读。
- 适用范围：M03 模块的《平台与设备管理：行级分析》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


## 证据表

| 区间 | 观察 | 结论状态 |
|---|---|---|
| 进程单例 | `Platform::Get()` 返回函数内静态对象，构造时初始化状态为未初始化。[src/musa/core/platform.cpp:14-31] | 已确认 |
| 平台初始化 | `Init()` 创建 HAL 平台、枚举并排序设备、筛选可见设备、提交 HAL 设置、初始化 debugger。[src/musa/core/platform.cpp:85-139] | 已确认 |
| 设备枚举与可见性 | `ReorderHalDevices` 排序、`GetVisibleDeviceIndices` 筛选、`Device` 构造并注册 primary context。[src/musa/core/platform.cpp:33-83] [src/musa/core/device.cpp:690-716] | 已确认 |
| 设备惰性初始化 | `LaterInit` 完成 HAL Finalize、queue family 映射、primary context、copy managers。[src/musa/core/device.cpp:911-1017] | 已确认 |
| 上下文创建 | `CreateContext` 统计 context 数、选调度模式、`new Context` + `Init`、登记并 Retain。[src/musa/core/device.cpp:635-675] | 已确认 |
| 属性查询 | `GetAttribute` 大 switch 映射 MU 属性到 Hal 属性；未识别返回 0 或 INVALID_VALUE。[src/musa/core/device.cpp:42-448] | 已确认 |
| 进程退出清理 | `~Platform` 等待设备、释放未回收内存、删除 pool、销毁 HAL 平台。[src/musa/core/platform.cpp:610-656] | 已确认 |
| 设备析构 | `~Device` 释放全部 context、copy managers、primary context 与默认 pool。[src/musa/core/device.cpp:718-748] | 已确认 |
| 下游范围 | src/hal/m3d | 构建/接口已确认，硬件副作用未知 |

## 阅读方法

1. 先看参数检查和 early return（如 `MU_CTX_SCHED_MASK` 的 flags 校验）。[src/musa/core/device.cpp:642-661]
2. 再看对象创建、登记、引用计数或 tracker 更新（如 `LockedAddContext`、`Retain`）。[src/musa/core/device.cpp:668-670]
3. 最后看 HAL 调用、错误回滚和析构（如 m_pHalDevice->Finalize）。[src/musa/core/device.cpp:941]

本页不把没有逐行读取的代码扩写成确定行为。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/musa/core/device.cpp`（8 处，例：[src/musa/core/device.cpp:690-716]）
- `src/musa/core/platform.cpp`（4 处，例：[src/musa/core/platform.cpp:14-31]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
