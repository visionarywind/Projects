# 构建/发布/安装系统：数据结构与所有权

- 文档目的：build release install：核心数据结构、字段含义与持有关系。
- 适用范围：M01 模块的《构建/发布/安装系统：数据结构与所有权》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


## 结构关系

```text
CMake 版本/选项变量 -> 目标（静态库/动态库/可执行文件） -> 安装产物
                        └-> 导出符号清单 -> .def / .ver
```

- 版本数据链：`version_string`(5.3.0) + `version_status`(develop) -> `DETAIL_VERSION` / `SONAME_VERSION` -> 驱动库文件名与 soname。[CMakeLists.txt:33-35] [CMakeLists.txt:80-81] [src/driver/CMakeLists.txt:47-52]
- 目标数据链：静态库（`util` -> `halM3d` -> `musaCore`）聚合源码，动态库（`musa_dynamic`、`musadebugger`、`musa_loader`）消费静态库，工具可执行文件消费动态库。[src/CMakeLists.txt:7-15] [src/musa/core/CMakeLists.txt:7-11] [src/driver/CMakeLists.txt:9-17] [src/tools/CMakeLists.txt:12-18]
- 导出数据链：`loader_api_list.inc`（567 条 `MUSA_LOADER_API`）与 `windows_api_list.inc`（3 条 Windows 独有导出）-> `DEFAULT_MUSA_EXPORTED_SYMBOLS` -> 生成 `.ver`/`.def`。[cmake/export_symbols.cmake:5-14] [cmake/export_symbols.cmake:22-59] [cmake/export_symbols.cmake:64-109] [src/loader/loader_api_list.inc:1-3]
- 安装数据链：`build/lib/` 与 `build/bin/` 由 `BUILD_ROOT_DIR` 派生，`install.sh` 据此探测目标目录并复制。[CMakeLists.txt:157-158] [install.sh:3-4] [install.sh:27-30]

## 所有权问题

- 静态库按 util → halM3d → musaCore → 驱动动态库的顺序被 add_subdirectory 创建，动态库与工具再依赖前者，顺序由 src/CMakeLists.txt 固定。[src/CMakeLists.txt:7-15]
- `install.sh` 把库与工具分开安装：库进 `LIB_INSTALL_DIR`，`musa_driver_version`/`muInfo` 进 `BIN_INSTALL_DIR`，并自行探测发行版选择 `/usr/lib64` 或 `/usr/lib/x86_64-linux-gnu`。[install.sh:51-81] [install.sh:100-105]
- `halM3d` 同时聚合本目录源码（`aux_source_directory`）并链接 m3d 子模块库，是 musaCore 与私有 m3d 之间的唯一编译期桥。[src/hal/m3d/CMakeLists.txt:56-59]
- `musa_loader` 仅在 WIN32 或 WSL2 下加入构建，其余平台不产生该目标。[src/CMakeLists.txt:11-13] [src/loader/CMakeLists.txt:12]

具体模块的字段布局应以对应头文件为准；本文不臆测未读字段。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/CMakeLists.txt`（3 处，例：[src/CMakeLists.txt:7-15]）
- `src/driver/CMakeLists.txt`（2 处，例：[src/driver/CMakeLists.txt:47-52]）
- `src/musa/core/CMakeLists.txt`（1 处，例：[src/musa/core/CMakeLists.txt:7-11]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
