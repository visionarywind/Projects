# Util 基础库（M12）

- 源码范围：`src/util/`
- 对应版本：`5.3.0 develop`
- 文档目的：util：总览：职责、结构、关键结论与文档导航。
- 适用范围：M12 模块的《Util 基础库（M12）》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：静态源码已确认；`SplayTree`/`ObjectPool`/`Executor` 的运行行为与并发正确性未验证
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`risks-and-debt.md`](risks-and-debt.md)

## 一句话职责

为驱动与 HAL 提供无依赖的基础设施：动态库加载包装、日志、动态库与对象池、单线程执行器、Splay 树、OS 抽象、位运算数学与断言。

## 阅读入口

1. 先读本页和 [`source-map.md`](source-map.md)。
2. 再读 [`interfaces.md`](interfaces.md)、[`call-chains.md`](call-chains.md)。
3. **必读** [`risks-and-debt.md`](risks-and-debt.md)：本模块含两个已定位的实际缺陷（`SplayTree::Search` 递归方向反转、`Find` 返回空指针后被调用方解引用）。

## 边界

- **本模块拥有**：STATIC 目标 `util`、约 15 个头文件提供的工具设施、2 个 `.cpp` 的实现。
- **本模块不拥有**：任何业务逻辑。M11 loader 只包含头文件而不链接本目标（见 [`../M11-loader/risks-and-debt.md`](../M11-loader/risks-and-debt.md) 风险 3）。
- **关键边界特征**：除 `utils.cpp` 与 `perfStatistics.cpp` 外**全部 header-only**；包含目录为 `PRIVATE`，消费方通过 `src/` 包含根访问头文件而非依赖本目标传播。
- **证据边界**：以下结论来自源码行号，不等同于目标机运行成功。

## 核心证据

[src/util/CMakeLists.txt:1-4] [src/util/utilLog.h:59-227] [src/util/utilLibrary.h:25-179] [src/util/utilObjectPool.h:14-167] [src/util/utilSplayTree.h:61-226]

## 深度状态

接口与调用链已逐跳给出行号。本模块是全库缺陷密度最高的部分之一（[`risks-and-debt.md`](risks-and-debt.md) 列出 20 项），其中 `SplayTree` 的递归方向反转直接影响 M09 HAL 的内存池查找路径，需优先关注。

## Demo 覆盖

- **真实经过本模块的 Demo 步骤**（均为 header-only 路径，见 [`../../80-demos/D01-muinfo/execution-trace.md`](../../80-demos/D01-muinfo/execution-trace.md) §0、§1）：
  1. D01 首跳加载驱动用 `Util::Library::LoadPath` [src/loader/loader.cpp:47-56]；
  2. 解析导出符号用 `Util::Library::GetSymbol` [src/loader/loader.cpp:138-145]；
  3. 驱动侧日志宏 `tprintf` 在 `Device::LaterInit` 中被调用 [src/musa/core/device.cpp:911-917]，宏定义在 [src/util/utilLog.h:222-227]。
  注：`src/tools/muInfo.cpp` 自身不含 `MUSA_LOG` 与 `Util::` 调用（已用 rg 核实），M12 只经 loader 与驱动侧间接到达；运行未验证。
- **候选但未覆盖**：D02–D07（`tests/*.cu`）——本快照无 `tests/` 目录，入口未核实；候选表未单列 M12，覆盖矩阵把 M12 当作这些 Demo 的 header-only 支撑（“候选”）。
- **尚无 Demo 覆盖的重要实现**：
  1. 日志开关与位标志 `Util::Log::Flag` [src/util/utilLog.h:59-72]（D01 未设置日志环境变量，实际输出未验证）；
  2. `SplayTree` 查找 [src/util/utilSplayTree.h:61-226]，直接影响 M09 池查找路径（见 [`risks-and-debt.md`](risks-and-debt.md)）；
  3. 对象池 `SpinLock`/`ObjectPool` [src/util/utilObjectPool.h:14-167]；
  4. `Util::Library` 的非 loader 用法 [src/util/utilLibrary.h:25-179]。

## 相关文档

- [`README.md`](README.md)
- [`call-chains.md`](call-chains.md)
- [`risks-and-debt.md`](risks-and-debt.md)
- [`../module-registry.md`](../module-registry.md)

## 源码证据摘要

- `src/util/CMakeLists.txt`（1 处，例：[src/util/CMakeLists.txt:1-4]）
- `src/util/utilLog.h`（1 处，例：[src/util/utilLog.h:59-227]）
- `src/util/utilLibrary.h`（1 处，例：[src/util/utilLibrary.h:25-179]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`risks-and-debt.md`](risks-and-debt.md)。
