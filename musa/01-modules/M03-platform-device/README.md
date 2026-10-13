# 平台与设备管理（M03）

- 源码范围：`src/musa/core/platform.cpp`、`src/musa/core/platform.h`、`src/musa/core/device.cpp`、`src/musa/core/device.h`、`src/musa/musaPlatform.h`、`src/musa/musaDevice.h`
- 对应版本：`5.3.0 develop`
- 文档目的：platform device：总览：职责、结构、关键结论与文档导航。
- 适用范围：M03 模块的《平台与设备管理（M03）》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：静态源码已确认；硬件行为、性能和完整异常覆盖未验证
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)

## 一句话职责

进程内平台/设备管理器：`Platform::Get().Init()` 创建 HAL 平台、按 PCI/IP 排序并筛选可见设备、构造 `Device` 对象；`Device::LaterInit()` 在首次使用时完成设备 finalize 与上下文/拷贝管理器初始化。[src/musa/core/platform.cpp:14-17] [src/musa/core/platform.cpp:85-139] [src/musa/core/device.cpp:911-1017]

## 阅读入口

1. 先读本页和 [`source-map.md`](source-map.md)。
2. 再读 [`implementation.md`](implementation.md)、[`call-chains.md`](call-chains.md)。
3. 涉及异步、硬件或安装时，结合 [`risks-and-debt.md`](risks-and-debt.md)。

## 边界

- **本模块拥有**：进程内单例 `Platform` 的初始化状态、设备可见性筛选、`Device`/Context 登记集合、devResource/memory-pool 登记、退出清理顺序。[src/musa/core/platform.h:141-171]
- **本模块不拥有**：上游 API 的公共 ABI 契约和下游 GPU/内核驱动的最终实现。
- **证据边界**：以下结论来自源码行号，不等同于目标机运行成功。

## 核心证据

- 进程单例与初始化：`Platform::Get()` 用函数内静态对象实现单例；`Init()` 用 `std::call_once` 包住 HAL 创建、设备枚举、排序与可见性筛选。[src/musa/core/platform.cpp:14-17] [src/musa/core/platform.cpp:85-139]
- 设备惰性初始化：`Device::LaterInit()` 做 HAL `Finalize`、queue family 映射、primary context 初始化、copy managers 初始化。[src/musa/core/device.cpp:911-1017]
- 析构清理：先等待各设备、释放未回收内存、删除默认 pool、销毁 devResource 描述与 HAL 平台。[src/musa/core/platform.cpp:610-656]

## 深度状态

正常路径已建立；分支、失败回滚、并发和清理见子文档。凡标记“推断/未知”的项目，均需在目标源码或硬件环境中补证。

## Demo 覆盖

- **真实经过本模块的 Demo 步骤**（见 [`../../80-demos/D01-muinfo/execution-trace.md`](../../80-demos/D01-muinfo/execution-trace.md) §1–§2）：
  1. D01-S002 的 `Platform::Init`：HAL 平台创建、设备排序与可见性筛选 [src/musa/core/platform.cpp:85-138]；
  2. D01-S005 经 `Platform::GetDevice` 触发的 `Device::LaterInit`：HAL finalize、queue family 扫描、primary context 与 copy managers 初始化 [src/musa/core/device.cpp:911-1017]；
  3. D01-S006 的 `Device::GetAttribute` 属性采集 [src/musa/core/device.cpp:42-104]；
  4. D01-S007 的 `Device::CanAccessPeer` 判定 [src/musa/core/device.cpp:598-606]。
- **候选但未覆盖**：D02–D07（`tests/*.cu`）——本快照无 `tests/` 目录，入口未核实；候选表把 D02/D03/D04/D05/D06/D07 的 M03 都列为计划覆盖（矩阵填“是”）。
- **尚无 Demo 覆盖的重要实现**：
  1. 退出清理 `Platform::~Platform` 与未回收内存释放 [src/musa/core/platform.cpp:610-656]；
  2. `Platform::GetP2PAttribute`（`muDeviceGetP2PAttribute` 走此路径，D01 只用 CanAccessPeer）[src/musa/core/platform.cpp:197-226]；
  3. 默认/当前 NUMA memory pool 选取 `Device::GetDefaultMemoryPool` [src/musa/core/device.cpp:451-498]；更多见 [`call-chains.md`](call-chains.md)、[`risks-and-debt.md`](risks-and-debt.md)。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/musa/core/platform.cpp`（5 处，例：[src/musa/core/platform.cpp:14-17]）
- `src/musa/core/device.cpp`（2 处，例：[src/musa/core/device.cpp:911-1017]）
- `src/musa/core/platform.h`（1 处，例：[src/musa/core/platform.h:141-171]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
