# 构建/发布/安装系统（M01）

- 源码范围：`CMakeLists.txt、src/*/CMakeLists.txt、install.sh、CI`
- 对应版本：`5.3.0 develop`
- 文档目的：build release install：总览：职责、结构、关键结论与文档导航。
- 适用范围：M01 模块的《构建/发布/安装系统（M01）》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：静态源码已确认；硬件行为、性能和完整异常覆盖未验证
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)

## 一句话职责

构建/发布/安装系统负责 `CMake 配置 -> 编译 -> 链接 -> 测试/安装`：根 `CMakeLists.txt` 声明全部开关与产物规则，`src/*/CMakeLists.txt` 逐层搭出目标依赖图，`install.sh` 与 CPack 负责落盘，`.ciConfig.yaml` / `.mthreads-ci.yml` 驱动 Linux 与 Windows 的构建、打包和测试流水线。

## 阅读入口

1. 先读本页和 [`source-map.md`](source-map.md)。
2. 再读 [`implementation.md`](implementation.md)、[`call-chains.md`](call-chains.md)。
3. 涉及异步、硬件或安装时，结合 [`risks-and-debt.md`](risks-and-debt.md)。

## 边界

- **本模块拥有**：本目录列出的对象、状态或构建规则，包括 CMake 选项（`MUSA_BUILD_DEBUG`、`MUSA_BUILD_UT`、`ENABLE_ASAN_CHECK`、`ENABLE_TSAN_CHECK`、`MTUB_BUILD`、`MUSA_WSL2_BUILD` 等）、目标名（`musaCore`、`halM3d`、`util`、`musa_dynamic`、`musadebugger`、`musa_loader`）、安装路径（`/usr/local/musa`）与 CI 阶段。
- **本模块不拥有**：上游 API 的公共 ABI 契约和下游 GPU/内核驱动的最终实现（在 `src/hal/m3d/m3d` 子模块与 linux-ddk 仓库）。
- **证据边界**：以下结论来自源码行号，不等同于目标机运行成功。

## 核心证据

[CMakeLists.txt:21-31] [src/driver/CMakeLists.txt:9-17] [src/musa/core/CMakeLists.txt:1-11] [src/hal/m3d/CMakeLists.txt:56-59] [install.sh:27-105] [.ciConfig.yaml:8-23] [.mthreads-ci.yml:37-65]

## 深度状态

正常路径（配置 → 编译 → 链接 → 测试/安装）已建立并逐跳给出行号。分支、失败回滚、并发和清理见子文档。凡标记“推断/未知”的项目，均需在目标源码或硬件环境中补证。

## Demo 覆盖

- **真实经过本模块的 Demo 步骤**：无。D01 的运行期轨迹从 `main()` 起，只覆盖 loader 首跳到属性输出（见 [`../../80-demos/D01-muinfo/execution-trace.md`](../../80-demos/D01-muinfo/execution-trace.md) §0–§2），不含配置、编译、安装任一步骤；D01 的构建/运行命令只登记在 [`../../80-demos/D01-muinfo/build-and-run.md`](../../80-demos/D01-muinfo/build-and-run.md) 且全部标注未验证。
- **候选但未覆盖**：D02–D07（`tests/*.cu`）——本快照无 `tests/` 目录，入口未核实；按 [`../../80-demos/demo-registry.md`](../../80-demos/demo-registry.md) 的候选表，它们计划由 MUSA 编译器编译 `.cu` 并链接 driver，覆盖矩阵中 M01 列均填“是”，但没有一个 Demo 的构建过程被静态解剖过。
- **尚无 Demo 覆盖的重要实现**：
  1. `muInfo` 可执行目标的自动生成与链接 [src/tools/CMakeLists.txt:10-18]；
  2. 产物安装与打包（`install.sh`、CPack）[install.sh:27-105]；
  3. Linux/Windows CI 流水线 [.ciConfig.yaml:8-23]、[.mthreads-ci.yml:37-65]；
  4. 根构建开关 `MUSA_BUILD_DEBUG`、`ENABLE_ASAN_CHECK` 等 [CMakeLists.txt:21-31]；细节见 [`implementation.md`](implementation.md)、[`testing.md`](testing.md)。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/driver/CMakeLists.txt`（1 处，例：[src/driver/CMakeLists.txt:9-17]）
- `src/musa/core/CMakeLists.txt`（1 处，例：[src/musa/core/CMakeLists.txt:1-11]）
- `src/hal/m3d/CMakeLists.txt`（1 处，例：[src/hal/m3d/CMakeLists.txt:56-59]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
