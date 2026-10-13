# 构建/发布/安装系统：源码地图

- 文档目的：build release install：源码文件/目录到文档章节的映射。
- 适用范围：M01 模块的《构建/发布/安装系统：源码地图》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-08
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


| 层 | 路径 | 关注点 |
|---|---|---|
| 配置/入口 | `CMakeLists.txt` | 全部 option、版本号、工具链/平台分支、安装前缀 |
| 子目录调度 | `src/CMakeLists.txt`、`src/{util,hal,musa,driver,loader,gdb,tools}/CMakeLists.txt` | 目标命名、依赖图、产物输出目录 |
| HAL | `src/hal/m3d/CMakeLists.txt`、`src/hal/m3d/m3d`（子模块） | `halM3d` 静态库与 m3d 编译宏 |
| 导出 | `cmake/export_symbols.cmake`、`src/loader/*.inc` | 符号清单 → `.def` / version script |
| 版本 | `module_version/generate_version_query.cmake` | `musa_driver_version` 生成与安装 |
| 安装 | `install.sh`、`CPackProjectConfig.cmake` | 库/工具落盘路径与多架构库目录 |
| CI | `.ciConfig.yaml`、`.mthreads-ci.yml` | Linux/Windows 构建、打包、测试流水线 |
| 辅助 | `ci/build_musa.py`、`cmake_mtmusa.py`、`scripts/clang_tidy.py` | Windows 构建驱动与静态检查 |

## 证据锚点

[CMakeLists.txt:21-31] [CMakeLists.txt:157-163] [src/CMakeLists.txt:1-15] [src/driver/CMakeLists.txt:9-17] [install.sh:27-105] [.mthreads-ci.yml:37-65]

## 关键文件首读区间

| 文件 | 先看哪一段 | 那一段在做什么 |
|---|---|---|
| `src/musa/core/CMakeLists.txt` | [src/musa/core/CMakeLists.txt:1-11] `musaCore` | 按目录收集 Core 源码、聚成静态库并链接 `halM3d` |
| `src/hal/m3d/CMakeLists.txt` | [src/hal/m3d/CMakeLists.txt:56-59] `halM3d` | 声明 `halM3d` 静态库并链接 m3d/scpc/util |
| `src/loader/CMakeLists.txt` | [src/loader/CMakeLists.txt:1-23] `FATAL_ERROR` | 位数/平台前置检查，随后解析 `SHARED_INCLUDE_PATH` |
| `src/tools/CMakeLists.txt` | [src/tools/CMakeLists.txt:10-18] `TOOLS_SOURCES` | glob 收集工具源码、逐个生成目标并安装到 bin |

## 搜索建议

```bash
rg -n "option\(|add_subdirectory|add_library|install\(TARGETS" CMakeLists.txt src cmake
rg -n "MUSA_BUILD_UT|MTUB_BUILD|MUSA_WSL2_BUILD" CMakeLists.txt .mthreads-ci.yml
```

上述命令是阅读建议，不代表已在本地或远端执行。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/CMakeLists.txt`（1 处，例：[src/CMakeLists.txt:1-15] `add_subdirectory`）
- `src/driver/CMakeLists.txt`（1 处，例：[src/driver/CMakeLists.txt:9-17] `musaCore`）
- `src/musa/core/CMakeLists.txt`（1 处，例：[src/musa/core/CMakeLists.txt:1-11] `halM3d`）
- `src/hal/m3d/CMakeLists.txt`（1 处，例：[src/hal/m3d/CMakeLists.txt:56-59] `halM3d`）
- `src/loader/CMakeLists.txt`（1 处，例：[src/loader/CMakeLists.txt:1-23] `SHARED_INCLUDE_PATH`）
- `src/tools/CMakeLists.txt`（1 处，例：[src/tools/CMakeLists.txt:10-18] `TOOLS_SOURCES`）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
