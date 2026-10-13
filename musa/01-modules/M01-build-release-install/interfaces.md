# 构建/发布/安装系统：接口契约

- 文档目的：build release install：接口签名、前置条件、错误码与跨层契约。
- 适用范围：M01 模块的《构建/发布/安装系统：接口契约》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-08
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


| 接口方向 | 输入 | 输出/副作用 | 前置条件 | 错误 |
|---|---|---|---|---|
| 开发者/CI -> 配置 | CMake 选项（`MUSA_BUILD_DEBUG`、`MUSA_BUILD_UT`、`ENABLE_ASAN_CHECK`、`ENABLE_TSAN_CHECK`、`MTUB_BUILD`、`MUSA_WSL2_BUILD`） | 目标、库、工具 | 源码完整（含 m3d 子模块、shared_include） | 配置期 `FATAL_ERROR` 或缺失目录报错 |
| 配置 -> 编译/链接 | 目标名与依赖 | 静态库/动态库/可执行文件 | 子目录 `add_subdirectory` 顺序正确 | 链接失败 |
| 构建 -> 导出 | `loader_api_list.inc`、`windows_api_list.inc` | `.ver`/`.def` 与受限符号表 | 清单无重复项 | `message(FATAL_ERROR ...)` |
| 构建 -> 安装 | `install.sh` 参数（`-l`/`-b`）或 `install(TARGETS ...)` 规则 | 库进 lib 目录、工具进 bin 目录 | build 目录存在 | `exit 1` |
| 构建 -> 打包 | `MTUB_BUILD=ON` + CPack 配置 | deb/rpm 到 `build/packages/` | mt-toolchain 可用 | CPack 失败 |

证据：[CMakeLists.txt:21-31] [src/driver/CMakeLists.txt:54-64] [install.sh:27-30] [cmake/export_symbols.cmake:16-19] [.mthreads-ci.yml:48-52]

公共 ABI 兼容性是 M02 的硬约束；其他模块应避免直接改变句柄布局和导出表顺序。此处的“应”是开发约束，不是源码事实。

## 构建系统侧契约的源码锚点

上表的每条方向都对应一处可核对的源码；下表只登记“契约本身写在哪里”，不含运行结果。

| 契约 | 谁依赖它 | 源码锚点 |
|---|---|---|
| 驱动动态库的目标名由 `DRIVER_LIB_NAME` 派生 | `src/` 下所有引用该变量的目标 | [src/CMakeLists.txt:1-5] `DRIVER_LIB_NAME` |
| Core 只允许通过 `halM3d` 接触硬件层 | Core 的链接依赖与 include 边界 | [src/musa/core/CMakeLists.txt:7-11] `halM3d` |
| 驱动动态库必须链接 `musaCore` | 驱动侧对 Core 符号的引用 | [src/driver/CMakeLists.txt:11-17] `target_link_libraries` |
| loader 只在 64 位 Windows/WSL 上配置 | CI 与跨平台配置脚本 | [src/loader/CMakeLists.txt:1-7] `FATAL_ERROR` |
| 工具的运行时安装目录是 `CMAKE_INSTALL_BINDIR` | `install.sh` 的 `-b` 参数与打包脚本 | [src/tools/CMakeLists.txt:16-18] `CMAKE_INSTALL_BINDIR` |
| HAL 层的编译宏在 `src/hal/m3d/CMakeLists.txt` 顶部集中设置 | m3d 子模块的构建变体 | [src/hal/m3d/CMakeLists.txt:1-12] `M3D_CLIENT_INTERFACE_MAJOR_VERSION` |

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/driver/CMakeLists.txt`（2 处，例：[src/driver/CMakeLists.txt:54-64] `MTUB_BUILD`）
- `src/CMakeLists.txt`（1 处，例：[src/CMakeLists.txt:1-5] `DRIVER_LIB_NAME`）
- `src/musa/core/CMakeLists.txt`（1 处，例：[src/musa/core/CMakeLists.txt:7-11] `musaCore`）
- `src/loader/CMakeLists.txt`（1 处，例：[src/loader/CMakeLists.txt:1-7] `FATAL_ERROR`）
- `src/tools/CMakeLists.txt`（1 处，例：[src/tools/CMakeLists.txt:16-18] `CMAKE_INSTALL_BINDIR`）
- `src/hal/m3d/CMakeLists.txt`（1 处，例：[src/hal/m3d/CMakeLists.txt:1-12] `M3D_CLIENT_INTERFACE_MAJOR_VERSION`）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
