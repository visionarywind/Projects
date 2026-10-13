# 公共 API 头文件层（M13）

- 源码范围：`src/musa_shared_include/`（submodule）
- 对应版本：`5.3.0 develop`，`MUSA_VERSION 50300`
- 文档目的：musa shared include：总览：职责、结构、关键结论与文档导航。
- 适用范围：M13 模块的《公共 API 头文件层（M13）》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：静态源码已确认；ABI 兼容性、生成文件与实现的一致性未验证
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`risks-and-debt.md`](risks-and-debt.md)

## 一句话职责

定义 MUSA 的公共 C ABI 契约：句柄与错误码类型、API 函数原型、API 版本化宏别名、PTDS 变体、废弃标记，以及驱动/工具/MUPTI/GDB/MUASAN/Raytracing 使用的导出表结构。

## 阅读入口

1. 先读本页和 [`source-map.md`](source-map.md)。
2. 再读 [`interfaces.md`](interfaces.md)（API 版本化机制是本模块的核心）。
3. 再读 [`data-structures.md`](data-structures.md)（8 个命名空间各自的 `ExportTable`）。

## 边界

- **本模块拥有**：公共类型定义、函数声明、宏层、accessor 结构体。这是**唯一被应用直接包含**的源码目录。
- **本模块不拥有**：任何实现。`musa.h` 是纯声明头（29280 行，仅包含 `<stdlib.h>` 与 `<stdint.h>`）。
- **关键边界特征**：本目录是 **submodule**（见 [.gitmodules:3-8]），发布时随 `libmusa.so` 一同安装。
- **证据边界**：以下结论来自源码行号，不等同于 ABI 兼容性验证。

## 核心证据

[src/musa_shared_include/musa.h:1-29280] [src/musa_shared_include/musa.h:212] [src/musa_shared_include/musa.h:41-131] [src/musa_shared_include/export_table.h:859-860] [src/musa_shared_include/generated_musa_meta.h:1-4038]

## 深度状态

本模块文档集比 M01–M12 精简：它是**声明型**代码而非实现型代码，"调用链"与"执行流程"的分析价值有限，故只保留 README、source-map、interfaces、data-structures、risks-and-debt 五篇。类型与宏的准确性已逐条给出行号。

## Demo 覆盖

- **真实经过本模块的 Demo 步骤**：D01 的编译期与类型依赖（见 [`../../80-demos/D01-muinfo/execution-trace.md`](../../80-demos/D01-muinfo/execution-trace.md) §1）：
  1. `muInfo` 源文件只包含公共头 [src/tools/muInfo.cpp:7-7]；
  2. 句柄类型 `MUdevice`、`MUcontext` [src/musa_shared_include/musa.h:230-231]；
  3. D01 判定成功用的 `MUSA_SUCCESS` 错误码 [src/musa_shared_include/musa.h:2631-2631]；
  4. API 版本化宏别名层 `muMemGetInfo` [src/musa_shared_include/musa.h:44-51]（D01 直接调 `_v2` 形式）。
- **候选但未覆盖**：D02–D07（`tests/*.cu`）——本快照无 `tests/` 目录，入口未核实；它们编译时同样依赖本模块的类型与宏，但候选表未单列 M13（覆盖矩阵按“候选”填）。
- **尚无 Demo 覆盖的重要实现**：
  1. 导出表结构 `AccessorHint` 与各 ExportTable [src/musa_shared_include/export_table.h:859-860]（D01 不调用 `muGetExportTable`）；
  2. 生成的 API 元数据 `generated_musa_meta.h` [src/musa_shared_include/generated_musa_meta.h:1-4038]；
  3. PTDS/PTSZ 变体与废弃标记宏 [src/musa_shared_include/musa.h:41-131]；契约见 [`interfaces.md`](interfaces.md)、[`data-structures.md`](data-structures.md)。

## 相关文档

- [`README.md`](README.md)
- [`risks-and-debt.md`](risks-and-debt.md)
- [`../module-registry.md`](../module-registry.md)

## 源码证据摘要

- `src/musa_shared_include/musa.h`（3 处，例：[src/musa_shared_include/musa.h:1-29280]）
- `src/musa_shared_include/export_table.h`（1 处，例：[src/musa_shared_include/export_table.h:859-860]）
- `src/musa_shared_include/generated_musa_meta.h`（1 处，例：[src/musa_shared_include/generated_musa_meta.h:1-4038]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`risks-and-debt.md`](risks-and-debt.md)。
