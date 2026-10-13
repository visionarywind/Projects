# Module 与 Kernel 执行（M07）

- 模块：M07 Module / Kernel
- 源码范围：`src/driver/mu_module.cpp`、`src/musa/core/module.{cpp,h}`、`src/musa/core/library.{cpp,h}`、`src/musa/core/command/dispatchCommand.{cpp,h}`、`src/hal/m3d/kernel.{cpp,h}`、`src/musa/core/context.cpp`
- 对应版本：`5.3.0 develop`
- 文档目的：module kernel：总览：职责、结构、关键结论与文档导航。
- 适用范围：M07 模块的《Module 与 Kernel 执行（M07）》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：静态源码已确认；硬件行为、性能和完整异常覆盖未验证
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)

## 一句话职责

Module 与 Kernel 执行负责把 `module load -> function -> launch params -> DispatchCommand` 从 driver 入口推进到 HAL/M3D 提交。[src/driver/mu_module.cpp:87-146] [src/driver/mu_module.cpp:231-268] [src/musa/core/command/dispatchCommand.cpp:78-300]

## 阅读入口

1. 先读本页和 [`source-map.md`](source-map.md)。
2. 再读 [`implementation.md`](implementation.md)、[`call-chains.md`](call-chains.md)。
3. 涉及异步、硬件或安装时，结合 [`risks-and-debt.md`](risks-and-debt.md)。

## 边界

- **本模块拥有**：`Module`/`Function`/`Kernel` 对象生命周期、符号表、launch 参数组装和 `DispatchCommand` 构建。[src/musa/core/module.h:18-31] [src/musa/core/library.h:55-98]
- **本模块不拥有**：fatbin 格式与 device code 编译（`src/hal/m3d/library.cpp` 的 M3D/Scpc 边界）、公共 ABI 契约（M02）、硬件执行行为。[src/hal/m3d/library.cpp:86-110]
- **证据边界**：以下结论来自源码行号，不等同于目标机运行成功。

## 核心证据

- 加载：`muapiModuleLoad` -> `muapiModuleLoadData` -> `Context::CreateModule` -> `Module::LoadFatBinary` -> HAL `CreateLibrary`。[src/driver/mu_module.cpp:87-146] [src/musa/core/context.cpp:1280-1286] [src/musa/core/module.cpp:20-54]
- 符号解析：`muapiModuleGetFunction` -> `Module::GetFunction` -> `FindSymbolByName` -> `Function::Init`。[src/driver/mu_module.cpp:157-178] [src/musa/core/module.cpp:56-73]
- Launch 组装：`muapiLaunchKernel` -> `Context::GeneralLaunchKernel` -> `CreateKernelNodeOnStream` -> `DispatchCommand::Build` -> `CmdDispatch`。[src/driver/mu_module.cpp:231-268] [src/musa/core/context.cpp:631-677] [src/musa/core/command/dispatchCommand.cpp:78-300]
- 提交：`DispatchCommand::Submit` -> `CmdBuffer::End` -> `Queue::Submit`。[src/musa/core/command/dispatchCommand.cpp:302-347] [src/hal/m3d/queue.cpp:182-355]

## 深度状态

正常路径已建立；失败回滚、并发和清理见子文档。凡标记“推断/未知”的项目，均需在目标源码或硬件环境中补证。

## Demo 覆盖

- **真实经过本模块的 Demo 步骤**：无。D01 全程没有 module 加载、符号解析或 kernel launch（见 [`../../80-demos/D01-muinfo/execution-trace.md`](../../80-demos/D01-muinfo/execution-trace.md) §1–§2）。
- **候选但未覆盖**：D02–D07（`tests/*.cu`）——本快照无 `tests/` 目录，入口未核实；候选表未把 M07 列入任一 Demo 的覆盖模块，覆盖矩阵中只有 D04/D05 的 M07 列为“可能”（graph kernel 节点）。
- **尚无 Demo 覆盖的重要实现**：
  1. module 加载 `muapiModuleLoad` [src/driver/mu_module.cpp:87-146] 与 `Module::LoadFatBinary` [src/musa/core/module.cpp:20-54]；
  2. kernel launch `muapiLaunchKernel` [src/driver/mu_module.cpp:231-268]；
  3. `DispatchCommand::Build` 把依赖翻成 semaphore [src/musa/core/command/dispatchCommand.cpp:78-300]；
  4. 符号解析 `Module::GetFunction` [src/musa/core/module.cpp:56-73]；完整链条见 [`call-chains.md`](call-chains.md)。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/driver/mu_module.cpp`（5 处，例：[src/driver/mu_module.cpp:87-146]）
- `src/musa/core/command/dispatchCommand.cpp`（3 处，例：[src/musa/core/command/dispatchCommand.cpp:78-300]）
- `src/musa/core/context.cpp`（2 处，例：[src/musa/core/context.cpp:1280-1286]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
