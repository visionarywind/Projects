# 构建/发布/安装系统：行级分析

- 文档目的：build release install：关键代码位置的行级证据与解读。
- 适用范围：M01 模块的《构建/发布/安装系统：行级分析》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


## 证据表

| 行号 | 观察 | 结论状态 |
|---|---|---|
| [CMakeLists.txt:21-31] | 11 个 CMake option 开关，含 `MUSA_BUILD_DEBUG`、`MUSA_BUILD_UT`、`ENABLE_ASAN_CHECK`、`ENABLE_TSAN_CHECK`、`MTUB_BUILD` | 已确认 |
| [CMakeLists.txt:33-35] | 版本 `5.3.0` + 状态 `develop` | 已确认 |
| [CMakeLists.txt:127-132] | Debug/Release 互斥与 IPO | 已确认 |
| [CMakeLists.txt:146-154] | 单测开关与 `googletest` FetchContent | 已确认 |
| [src/CMakeLists.txt:7-15] | 子目录顺序：util → hal → musa → driver → loader/gdb/tools | 已确认 |
| [src/driver/CMakeLists.txt:9-17] | 驱动动态库目标与 `musaCore` 链接 | 已确认 |
| [src/musa/core/CMakeLists.txt:1-11] | `musaCore` 静态库聚合源码并链接 `halM3d` | 已确认 |
| [src/hal/m3d/CMakeLists.txt:56-59] | `halM3d` 静态库链接 m3d/scpc/util | 已确认 |
| [install.sh:27-105] | 安装前检查、发行版探测、复制库与工具 | 已确认 |
| [.ciConfig.yaml:8-23] | Linux 首个构建 job 与产物 | 已确认 |
| [.mthreads-ci.yml:37-65] | MTUB 打包（deb/rpm） | 已确认 |
| 下游范围 | `src/hal/m3d/m3d` | 构建/接口已确认，硬件副作用未知 |

## 阅读方法

1. 先看 option 声明与默认值（根 `CMakeLists.txt` 的 option 块）。[CMakeLists.txt:21-31]
2. 再看目标创建、链接与输出目录（各子目录 `CMakeLists.txt`）。[src/CMakeLists.txt:7-15]
3. 最后看安装与 CI 落盘（`install.sh`、CPack、CI 配置）。[install.sh:27-105]

## 逐行注记（示例）

- [CMakeLists.txt:22] 声明 `ENABLE_ASAN_CHECK`，默认 OFF；[CMakeLists.txt:110-113] 打开后注入 `-fsanitize=address`。
- [CMakeLists.txt:29] 声明 `MUSA_BUILD_UT`，默认 OFF。
- [src/driver/CMakeLists.txt:9] `set(target ${DRIVER_LIB_NAME}_dynamic)` 生成驱动动态库目标名。
- [install.sh:100] `cp -P build/lib/libmusa*` 复制驱动库。
- [cmake/export_symbols.cmake:5-9] 声明调试数据导出符号 `muDebugFlag`、`muGdbPid`、`muGdbSessionId`。
- [module_version/generate_version_query.cmake:119-121] 通过 `configure_file` 生成版本源文件。

本页不把没有逐行读取的代码扩写成确定行为。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/CMakeLists.txt`（2 处，例：[src/CMakeLists.txt:7-15]）
- `src/driver/CMakeLists.txt`（2 处，例：[src/driver/CMakeLists.txt:9-17]）
- `src/musa/core/CMakeLists.txt`（1 处，例：[src/musa/core/CMakeLists.txt:1-11]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
