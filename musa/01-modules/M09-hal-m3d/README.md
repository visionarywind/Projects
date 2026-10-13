# HAL/M3D 适配（M09）

- 源码范围：`src/hal/hal*.h`、`src/hal/m3d/*`（不含 `src/hal/m3d/m3d` 私有子模块内部）
- 对应版本：`5.3.0 develop`
- 文档目的：hal m3d：总览：职责、结构、关键结论与文档导航。
- 适用范围：M09 模块的《HAL/M3D 适配（M09）》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：静态源码已确认；硬件行为、性能和完整异常覆盖未验证
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)

## 一句话职责

把 `Hal::IPlatform/IDevice/IQueue/IMemory/IMemMgr` 接口族实现成 `Hal::M3d::*` 对象，再由 `OwnPtr` 转发到 `IM3d::*` 私有接口，形成 `HAL interface -> M3D device/queue/memory/cmd buffer` 这条边界 [src/hal/m3d/lib.cpp:6-13] [src/hal/m3d/queue.h:11-37]。

## 阅读入口

1. 先读本页和 [`source-map.md`](source-map.md)。
2. 再读 [`implementation.md`](implementation.md)、[`call-chains.md`](call-chains.md)。
3. 涉及内存池、锁或失败回滚时，结合 [`risks-and-debt.md`](risks-and-debt.md)。

## 边界

- **本模块拥有**：HAL 接口头的契约、`Hal::M3d` 实现类的状态与生命周期、`halM3d` 构建目标与宏 [src/hal/m3d/CMakeLists.txt:56-59]。
- **本模块不拥有**：`src/hal/m3d/m3d` 私有子模块的内部实现，本目录只声明其构建边界 [src/hal/m3d/CMakeLists.txt:22-23]；上游 `src/musa/core` 的公共 ABI 契约；下游 GPU/内核驱动的最终行为。
- **证据边界**：以下结论来自源码行号，不等同于目标机运行成功。

## 核心证据

- 平台入口：`Hal::CreatePlatform` 建 `M3d::Platform` 并 `Init`，失败即 `delete` [src/hal/m3d/lib.cpp:6-13]
- 构建与消费：`halM3d` 静态库由目录 `.cpp` 自动生成，唯一消费方是 `musaCore` [src/hal/m3d/CMakeLists.txt:56-59] [src/musa/core/CMakeLists.txt:11]
- 内存：`MemMgr::Allocate` 先做规模校验与选池，再 `FullAllocate` 落到池 [src/hal/m3d/memMgr.cpp:81-147]
- 提交：`Queue::Submit` 装配信号量后转发 `m_M3dQueue->Submit` [src/hal/m3d/queue.cpp:182-357]
- 录制：`CmdBuffer` 的 `Begin`/`End`/`Reset` 严格按 `CmdBufferState` 推进 [src/hal/m3d/cmdBuffer.cpp:145-228]

## 深度状态

正常路径已建立；分支、失败回滚、并发和清理见子文档。`src/hal/m3d/m3d` 内部行为一律标注为未验证，凡标记"推断/未验证"的项目，均需在目标源码或硬件环境中补证。

## Demo 覆盖

- **真实经过本模块的 Demo 步骤**：只到初始化边界——D01-S002/D01-S003 的 `CreatePlatform` [src/hal/m3d/lib.cpp:6-13] 与 `Platform::Init` 的设备枚举 [src/hal/m3d/platform.cpp:102-189]；停在 HAL 接口，`src/hal/m3d/m3d` 内部与 firmware 行为仍未验证（见 [`../../80-demos/D01-muinfo/execution-trace.md`](../../80-demos/D01-muinfo/execution-trace.md) §1）。
- **候选但未覆盖**：D02–D07（`tests/*.cu`）——本快照无 `tests/` 目录，入口未核实；候选表把 D02/D03/D04/D05 的 M09 列为计划覆盖，D06/D07 为“可能”。
- **尚无 Demo 覆盖的重要实现**：
  1. 内存 `MemMgr::Allocate` [src/hal/m3d/memMgr.cpp:81-147]；
  2. 提交 `Queue::Submit` [src/hal/m3d/queue.cpp:182-357]；
  3. 录制 `CmdBuffer::Begin` [src/hal/m3d/cmdBuffer.cpp:145-228]；`src/hal/m3d/m3d` 内部风险见 [`risks-and-debt.md`](risks-and-debt.md)。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/hal/m3d/CMakeLists.txt`（3 处，例：[src/hal/m3d/CMakeLists.txt:56-59]）
- `src/hal/m3d/lib.cpp`（2 处，例：[src/hal/m3d/lib.cpp:6-13]）
- `src/hal/m3d/queue.h`（1 处，例：[src/hal/m3d/queue.h:11-37]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
