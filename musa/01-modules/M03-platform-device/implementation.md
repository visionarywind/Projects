# 平台与设备管理：实现说明

- 文档目的：platform device：核心函数/流程的逐段实现解读。
- 适用范围：M03 模块的《平台与设备管理：实现说明》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


## 正常实现路径

```text
Platform::Get().Init -> Hal::CreatePlatform -> ReorderHalDevices -> GetVisibleDeviceIndices
  -> Device 构造（FillProperties） -> GetDevice -> Device::LaterInit -> primary ctx / copy managers
```

## Init 的关键实现

- 单次初始化由 `m_InitFlag` 的 `std::call_once` 保证；仅当 `m_InitStatus != MUSA_SUCCESS` 时进入。[src/musa/core/platform.cpp:85-87]
- HAL 创建信息从 settings 派生：`enablePfm` 取全局 `g_pfm_initialized`，`disableDma` 取决于 memset/memcpy 路径是否 DMA，页面对齐取自 `forceDevicePageSize`。[src/musa/core/platform.cpp:91-97]
- HAL 创建失败仅打 `LOG_ERR`，后续按 `status != MUSA_SUCCESS` 跳过设备构造。[src/musa/core/platform.cpp:98-100]
- 设备排序：`deviceOrderMode == PCI_BUS_ID` 时按 PCI（domain/bus/device）排序，否则按 IP major/minor 降序后加 PCI 排序。[src/musa/core/platform.cpp:33-59]
- 可见性筛选：`inputIndices` 为空时返回全部设备；否则逐个加入，遇越界或重复 index 即 `break`。[src/musa/core/platform.cpp:61-83]
- 可见设备集合取自 `visibleDevices` settings，交给 `GetVisibleDeviceIndices` 计算。[src/musa/core/platform.cpp:112]
- 每个可见设备构造 `Device(seqId, halDevice)`，seqId 从 0 连续编号。[src/musa/core/platform.cpp:116-118]

## LaterInit 的关键实现

- 以 `std::call_once(m_InitFlag)` 保证单次；`Hal::DeviceFinalizeInfo` 全部来自平台 settings。[src/musa/core/device.cpp:911-941]
- queue family 名称映射 CDM/TDM/CE/ACE/TCE/DMA/UNIVERSAL/MMU 到 engine 下标。[src/musa/core/device.cpp:948-980]
- CDM 与 TCE 均无效视为失败（返回 `MUSA_ERROR_OUT_OF_MEMORY`）。[src/musa/core/device.cpp:981-986]
- 随后初始化 primary context（`m_PrimaryCtx->Init()`）、LLC persisting、`InitCopyManagers`，并在 universal engine 有效时初始化 `UniversalManager`。[src/musa/core/device.cpp:988-1011]

## 上下文实现

- `CreateContext` 用 `numCtx >= std::thread::hardware_concurrency()` 决定 AUTO 模式选 Yield 还是 Spin。[src/musa/core/device.cpp:635-661]
- 成功路径：`LockedAddContext`（写锁入 device 集合）→ `SetWaitMode` → `Retain`；失败路径直接 `delete ctx`。[src/musa/core/device.cpp:663-674]

## 分支矩阵

| 分支 | 触发 | 结果 | 当前证据 |
|---|---|---|---|
| 重复/越界 visible index | 输入 index 列表含越界或重复 | 收集过程 `break`，可能得到空集 | [src/musa/core/platform.cpp:69-80] |
| HAL 创建失败 | Hal::CreatePlatform 返回错误 | 跳过设备构造，m_InitStatus 记录错误 | [src/musa/core/platform.cpp:98-100] |
| 无可见设备 | 可见集为空 | `MUSA_ERROR_NO_DEVICE` | [src/musa/core/platform.cpp:112-119] |
| device 状态异常 | `LaterInit` 失败 | `GetDevice` 返回 nullptr | [src/musa/core/platform.cpp:141-150] |
| CDM/TCE 均缺失 | queue family 映射后 | `MUSA_ERROR_OUT_OF_MEMORY` | [src/musa/core/device.cpp:981-986] |
| 非法调度 flag | flags 不含合法 sched mask | `MUSA_ERROR_INVALID_VALUE` | [src/musa/core/device.cpp:642-661] |
| 销毁不存在的 context | `FindCtx` 未命中 | `MUSA_ERROR_CONTEXT_IS_DESTROYED` | [src/musa/core/device.cpp:677-684] |

## 尚未证明

- 所有错误分支是否都释放中间对象（`CreateContext` 失败时 `delete ctx` 已确认，其余需逐函数核实）。
- HAL `Finalize`/`CreatePlatform` 的失败是否稳定映射到 `MUSA_ERROR_*`，需结合完整 HAL/M3D 代码与目标机确认。
- GetVisibleDeviceIndices 的 `break` 语义对非法 index 的处理（`TODO` 注释）未验证。[src/musa/core/platform.cpp:71]

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/musa/core/platform.cpp`（12 处，例：[src/musa/core/platform.cpp:85-87]）
- `src/musa/core/device.cpp`（9 处，例：[src/musa/core/device.cpp:911-941]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
