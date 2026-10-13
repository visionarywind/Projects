# Loader 薄加载层（M11）

- 源码范围：`src/loader/`
- 对应版本：`5.3.0 develop`
- 文档目的：loader：总览：职责、结构、关键结论与文档导航。
- 适用范围：M11 模块的《Loader 薄加载层（M11）》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：静态源码已确认；Windows/WSL 实际加载行为、驱动枚举结果和导出符号完整性未验证
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`risks-and-debt.md`](risks-and-debt.md)

## 一句话职责

在应用与 `libmusa.so` 之间插入一层可替换的转发壳：首次 API 调用时按 DriverStore 包 ID 定位并 `dlopen`/`LoadLibraryExW` 真实驱动，解析全部导出符号，之后把所有 `mu*` 调用零开销转发到驱动内部实现。

## 阅读入口

1. 先读本页和 [`source-map.md`](source-map.md)。
2. 再读 [`interfaces.md`](interfaces.md)、[`call-chains.md`](call-chains.md)、[`implementation.md`](implementation.md)。
3. 关心死代码和构建问题时，读 [`risks-and-debt.md`](risks-and-debt.md)。

## 边界

- **本模块拥有**：驱动包定位、平台 thunk 加载、适配器枚举、包 ID 比对、导出符号解析、570 个 `extern "C"` 转发包装。
- **本模块不拥有**：驱动的实际 API 实现（在 `src/driver`）、驱动枚举 API 语义（在 gdi32/libdxcore）、工具与 profiler（在 `src/tools`、`src/driver/mupti`）。
- **关键边界特征**：本模块**不链接** `util` 目标，只因 `Util::Library` 与 `Util::SplayTree` 等头文件全为 header-only 才能通过链接（见 [`risks-and-debt.md`](risks-and-debt.md) 风险 3）。
- **证据边界**：以下结论来自源码行号，不等同于目标机加载成功。

## 核心证据

[src/loader/CMakeLists.txt:1-93] [src/loader/loader.cpp:44-215] [src/loader/driver_resolver.cpp:301-377] [src/loader/loader_api_list.inc:1-568] [cmake/export_symbols.cmake:11-14]

## 深度状态

正常路径（首次调用 → 定位 → 加载 → 转发）已建立并逐跳给出行号。分支、错误吞掉、死代码和构建可行性见 [`risks-and-debt.md`](risks-and-debt.md)。本模块在本地快照中**无法配置编译**（缺 `shared_include`、缺 `hwdefs/mtgpu_asics.h` 目标路径），因此所有行级结论均为静态推导，未编译验证。

## Demo 覆盖

- **真实经过本模块的 Demo 步骤**：D01 的首跳（见 [`../../80-demos/D01-muinfo/execution-trace.md`](../../80-demos/D01-muinfo/execution-trace.md) §0）：
  1. 应用不直接链接驱动，首个 API 由宏生成的 extern "C" 包装转发 [src/loader/loader.cpp:211-215]；
  2. 每次调用先过 once 检查 `EnsureInitialized` [src/loader/loader.cpp:97-112]，再由 `Forward` 调驱动实现 [src/loader/loader.cpp:180-205]；
  3. 首次调用定位并加载驱动：`ResolveDriverLocation` [src/loader/driver_resolver.cpp:301-377] → `LoadDriverModule` [src/loader/loader.cpp:47-56] → `ResolveDriverExports` [src/loader/loader.cpp:146-155]。
- **候选但未覆盖**：D02–D07（`tests/*.cu`）——本快照无 `tests/` 目录，入口未核实；它们同为 MUSA 应用、理论上都会走 loader 首跳，但没有任何一个被解剖。
- **尚无 Demo 覆盖的重要实现**：
  1. debugger 数据同步 `SynchronizeDebuggerData`（只在转发 `muDebugInit` 时触发，D01 不走）[src/loader/loader.cpp:165-177]；
  2. Windows 专属加载分支（`#if defined(_WIN32)` 一侧）[src/loader/loader.cpp:47-56]，D01 在 Linux 上只走 dlopen 路径；
  3. 独立诊断入口 `probe.cpp`：加载失败时 `muInfo` 侧看不到原因，需它单独诊断，但本快照不参与构建（见 [`risks-and-debt.md`](risks-and-debt.md) 风险 4）；调用链见 [`call-chains.md`](call-chains.md)。

## 相关文档

- [`README.md`](README.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)
- [`../module-registry.md`](../module-registry.md)

## 源码证据摘要

- `src/loader/CMakeLists.txt`（1 处，例：[src/loader/CMakeLists.txt:1-93]）
- `src/loader/loader.cpp`（1 处，例：[src/loader/loader.cpp:44-215]）
- `src/loader/driver_resolver.cpp`（1 处，例：[src/loader/driver_resolver.cpp:301-377]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`risks-and-debt.md`](risks-and-debt.md)。
