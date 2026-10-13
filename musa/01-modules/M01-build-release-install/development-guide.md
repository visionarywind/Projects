# 构建/发布/安装系统：开发指南

- 文档目的：build release install：常见修改场景的落地入口、注意点与验证方式。
- 适用范围：M01 模块的《构建/发布/安装系统：开发指南》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-08
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


## 修改前

1. 锁定源码提交并更新证据行号（`.ciConfig.yaml` 引用的 `ddk_build.sh` 在 linux-ddk 仓库，不在本快照）。
2. 判断改动属于：CMake 选项/目标、导出清单、安装脚本、CI 配置中的哪一层。
3. 先画出目标依赖与安装路径，再改代码。

## 修改中

- 新 option 必须在根 `CMakeLists.txt` 的 option 块（L21-31）声明，避免在子目录临时拼变量。
- 新目标保持 `add_library`/`add_executable` 与 `target_link_libraries` 的现有风格，include 用 `target_include_directories(... PRIVATE ...)`。
- 公开符号必须进入 `src/loader/loader_api_list.inc` 或 `windows_api_list.inc`，由 `setup_unified_exports` 统一导出。[cmake/export_symbols.cmake:11-14] [cmake/export_symbols.cmake:135-138]
- 安装规则走 `install(TARGETS ...)` + `CMAKE_INSTALL_LIBDIR`/`CMAKE_INSTALL_BINDIR`，或 `install.sh` 的分发探测。[src/driver/CMakeLists.txt:54-64] [install.sh:51-81]

### 改动落点速查

| 改动类型 | 落点 | 注意点 |
|---|---|---|
| 增删子目录 | [src/CMakeLists.txt:7-15] `add_subdirectory` | 顺序即依赖：util → hal → musa → driver →（Windows/WSL2）loader → gdb → tools |
| 调整驱动库产物属性 | [src/driver/CMakeLists.txt:19-36] `setup_unified_exports` | 仅 Release 走统一导出；Windows 非 Release 用 `WINDOWS_EXPORT_ALL_SYMBOLS` |
| 给 Core 增删源码目录 | [src/musa/core/CMakeLists.txt:1-7] `aux_source_directory` | 按目录整体收集，新增子目录要补一行，不会自动递归到孙目录 |
| 改 HAL 侧编译宏 | [src/hal/m3d/CMakeLists.txt:41-54] `add_definitions` | 与根 `CMakeLists.txt` 的 option 各自独立，改一边要对照另一边 |
| 加一个命令行工具 | [src/tools/CMakeLists.txt:12-18] `add_executable` | 一个 `.cpp` 生成一个可执行文件，安装目标是 `CMAKE_INSTALL_BINDIR` |

## 修改后

先做静态 diff/格式检查（`git diff --check`），再在授权目标机做最小构建（`cmake -S . -B build` + `cmake --build build`）与专项测试；`install.sh` 与 CI 改动只在明确授权时执行。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/driver/CMakeLists.txt`（2 处，例：[src/driver/CMakeLists.txt:54-64] `MTUB_BUILD`）
- `src/CMakeLists.txt`（1 处，例：[src/CMakeLists.txt:7-15] `add_subdirectory`）
- `src/musa/core/CMakeLists.txt`（1 处，例：[src/musa/core/CMakeLists.txt:1-7] `aux_source_directory`）
- `src/hal/m3d/CMakeLists.txt`（1 处，例：[src/hal/m3d/CMakeLists.txt:41-54] `add_definitions`）
- `src/tools/CMakeLists.txt`（1 处，例：[src/tools/CMakeLists.txt:12-18] `add_executable`）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
