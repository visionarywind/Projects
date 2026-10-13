# 平台与设备管理：开发指南

- 文档目的：platform device：常见修改场景的落地入口、注意点与验证方式。
- 适用范围：M03 模块的《平台与设备管理：开发指南》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-08
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


## 修改前

1. 锁定源码提交并更新证据行号；本模块主入口是 `Platform::Get().Init()` 与 `Device::LaterInit()`，改动会影响全局生命周期。
2. 判断改动属于 ABI（IPlatform/IDevice）、Core 生命周期（Platform/Device/CriticalBase）、HAL 适配（Hal::IPlatform/Hal::IDevice）还是观测层（MUgdb/MUasan）。
3. 画出调用链和资源所有权，再改代码；尤其注意 `std::call_once` 与 `m_InitStatus` 的写入顺序。

## 修改中

- 保持现有接口风格：新增查询类接口走 `IPlatform`/`IDevice` 纯虚方法。[src/musa/musaPlatform.h:27-165]
- 涉及初始化状态时，遵循「所有初始化完成后才写 `m_InitStatus`」的顺序。[src/musa/core/platform.cpp:133-138]
- HAL 错误通过 `HalToMuResult` 既有转换边界返回。
- 登记/解登记成对：新增集合成员必须同时实现 `LockedAdd*`/`LockedRemove*` 并纳入析构路径。
- 不在没有硬件证据时改变同步语义。

### 各类改动的具体落点

| 改动类型 | 落点 | 注意点 |
|---|---|---|
| 新增/调整设备属性 | [src/musa/core/device.cpp:42-45] `MUdevice_attribute` | `Device::GetAttribute` 的 switch 与公共枚举要成对改，未命中分支落到 default 判定 |
| 调整设备排序与可见性 | [src/musa/core/platform.cpp:33-83] `GetVisibleDeviceIndices` | `ReorderHalDevices` 与筛选在同一段，改排序规则要一起看越界/重复 index 的 `break` |
| 改 HAL 侧设备枚举 | [src/hal/m3d/platform.cpp:128-163] `EnumerateDevices` | 先枚举拿 `deviceCount`，再逐个 `GetProperties` 汇总内存属性，中途失败即返回 |
| 改 HAL 平台工厂的失败语义 | [src/hal/m3d/lib.cpp:6-15] `CreatePlatform` | 失败路径 `delete platform`，不把半初始化对象交给 Core |

## 修改后

先做静态 diff/格式检查，再在授权目标机做最小构建和专项测试（设备枚举、可见性、上下文创建、退出清理）；安装脚本只在明确授权时执行。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/musa/musaPlatform.h`（1 处，例：[src/musa/musaPlatform.h:27-165] `IPlatform`）
- `src/musa/core/platform.cpp`（2 处，例：[src/musa/core/platform.cpp:133-138] `m_InitStatus`）
- `src/musa/core/device.cpp`（1 处，例：[src/musa/core/device.cpp:42-45] `GetAttribute`）
- `src/hal/m3d/platform.cpp`（1 处，例：[src/hal/m3d/platform.cpp:128-163] `EnumerateDevices`）
- `src/hal/m3d/lib.cpp`（1 处，例：[src/hal/m3d/lib.cpp:6-15] `CreatePlatform`）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
