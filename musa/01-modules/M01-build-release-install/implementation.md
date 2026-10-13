# 构建/发布/安装系统：实现说明

- 文档目的：build release install：核心函数/流程的逐段实现解读。
- 适用范围：M01 模块的《构建/发布/安装系统：实现说明》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


## 正常实现路径

```text
CMake 配置 -> 编译 -> 链接 -> 测试/安装
```

源码证据：[CMakeLists.txt:21-31] [src/driver/CMakeLists.txt:9-17] [install.sh:27-105]

## 关键实现观察

- 三个 CMake 入口变量：`CMAKE_INSTALL_PREFIX`（`/usr/local/musa`）、`BUILD_ROOT_DIR`（输出根）、`DRIVER_LIB_NAME`（`musa`/`mtmusa64`）。[CMakeLists.txt:157-163] [src/CMakeLists.txt:1-5]
- 默认 Release：`MUSA_BUILD_DEBUG` 关闭时强制 `CMAKE_BUILD_TYPE=Release` 并开启 IPO。[CMakeLists.txt:127-132]
- sanitizer 通过编译宏注入，且 ASAN/TSAN 互斥（同一编译单元不能同时注入两类标志）。[CMakeLists.txt:110-118]
- 测试开关 `MUSA_BUILD_UT` 除 `enable_testing()` 外还会 FetchContent 拉取 googletest 并 `add_subdirectory(unittest)`。[CMakeLists.txt:146-154] [CMakeLists.txt:167-169]
- 导出管理由 `cmake/export_symbols.cmake` 统一：读取 loader 清单生成 `.ver`/`.def`。[cmake/export_symbols.cmake:11-59] [cmake/export_symbols.cmake:64-109]
- Release 下对驱动动态库执行符号瘦身（`--strip-unneeded`），Debug 下 Windows 改用 `WINDOWS_EXPORT_ALL_SYMBOLS`。[src/driver/CMakeLists.txt:19-36]

## 分支矩阵

| 分支 | 触发 | 结果 | 当前证据 |
|---|---|---|---|
| Debug 构建 | `MUSA_BUILD_DEBUG=ON` | `CMAKE_BUILD_TYPE=Debug`，跳过 IPO | [CMakeLists.txt:127-132] |
| mt-toolchain 打包 | `MTUB_BUILD=ON` | 走 CPack，生成 deb/rpm | [CMakeLists.txt:40-61] [.mthreads-ci.yml:37-65] |
| WSL2 构建 | `MUSA_WSL2_BUILD=ON` | 驱动库后缀 `.so.1.1`、soname 固定 | [src/driver/CMakeLists.txt:38-52] |
| Windows 构建 | WIN32 | 库名 `mtmusa64`、`WINDOWS_EXPORT_ALL_SYMBOLS` | [src/CMakeLists.txt:1-5] [src/driver/CMakeLists.txt:33-36] |
| 单测构建 | `MUSA_BUILD_UT=ON` | FetchContent googletest + `unittest/` | [CMakeLists.txt:146-154] [CMakeLists.txt:167-169] |
| CSV 支持原因整理 | `CSV_UNSUPPORTED` 打开（实际判断用 `CSV_UNSPPORTED`） | 运行 python3 code_reason.py | [CMakeLists.txt:25] [CMakeLists.txt:134-136] |

## 尚未证明

该模块在快照缺失的目录（`unittest/`、`tool/`、`src/musa/core/armory/include/`）存在时能否按预期构建、CI 各 job 在目标机上是否全绿，必须结合完整源码（含 m3d 子模块与 linux-ddk）和目标机测试确认。[CMakeLists.txt:134-136] [src/tools/CMakeLists.txt:5-8] [CMakeLists.txt:167-169]

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/driver/CMakeLists.txt`（4 处，例：[src/driver/CMakeLists.txt:9-17]）
- `src/CMakeLists.txt`（2 处，例：[src/CMakeLists.txt:1-5]）
- `src/tools/CMakeLists.txt`（1 处，例：[src/tools/CMakeLists.txt:5-8]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
