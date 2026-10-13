# 构建/发布/安装系统：测试策略

- 文档目的：给出 M01 的静态检查项、CI 测试入口、按分支的验证矩阵，并明确哪些结论只是静态分析。
- 适用范围：根 `CMakeLists.txt`、`cmake/`、`scripts/`、`.ciConfig.yaml`、`.mthreads-ci.yml`、`install.sh`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：静态分析，未验证。本知识库没有在任何环境执行过配置、编译、`ctest`、打包或安装命令，下面所有"期望"列都不是测试结果。
- 最后更新：2026-10-08
- 前置阅读：[`execution-flows.md`](execution-flows.md)、[`source-map.md`](source-map.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)

## 测试基建在哪

| 项 | 位置 | 说明 |
|---|---|---|
| CTest 总开关 | [CMakeLists.txt:125-125] `enable_testing` | 无条件打开 CTest 支持，即使不开单测 |
| 单测分支 | [CMakeLists.txt:146-154] `FetchContent_MakeAvailable` `googletest` | `MUSA_BUILD_UT` 为真时二次 `enable_testing` 并拉取 googletest 1.17.0 |
| 单测子目录 | [CMakeLists.txt:167-169] `add_subdirectory` `unittest` | `MUSA_BUILD_UT` 为真时进入 `unittest` |
| 单测排除项 | [scripts/clang_tidy.py:81-88] `exclude_dirs` `unittest` | clang-tidy 分析显式跳过 `unittest` |

## 快照缺口（已确认）

- `unittest/` 目录在本快照中不存在，因此 `MUSA_BUILD_UT=ON` 会在 [CMakeLists.txt:167-169] 的 `add_subdirectory` 处失败。
- `tool/` 目录在本快照中不存在，`CSV_UNSPPORTED` 分支要执行的 `code_reason.py` 不可用。[CMakeLists.txt:134-136] `CSV_UNSPPORTED`
- 仓库中没有任何 `tests/` 目录；可执行的测试入口只存在于 CI 配置文件里。
- option 名称不一致：声明的是 `CSV_UNSUPPORTED`，条件判断用的是 `CSV_UNSPPORTED`。[CMakeLists.txt:25-25] [CMakeLists.txt:134-136]

## 静态检查（本地可执行）

| 检查项 | 方法 | 证据 | 状态 |
|---|---|---|---|
| 新增 option 必须落在 option 块内 | 对照 `option(...)` 声明列表 | [CMakeLists.txt:21-31] `option` `MUSA_BUILD_UT` | 静态分析，未验证 |
| 子目录顺序决定目标依赖 | 对照 `src/CMakeLists.txt` 的注册顺序 | [src/CMakeLists.txt:7-15] `add_subdirectory` | 静态分析，未验证 |
| 导出符号清单无重复 | `list(REMOVE_DUPLICATES)` 后长度比对 | [cmake/export_symbols.cmake:52-59] `UNIQUE_MUSA_EXPORTED_SYMBOLS` | 静态分析，未验证 |
| 导出文件按平台生成 | `.def` 走 WIN32、`.ver` 走 UNIX | [cmake/export_symbols.cmake:64-109] `generate_export_files` | 静态分析，未验证 |
| 失败路径不登记半初始化对象 | 读 `Create*` 的 `Init` 失败分支 | [src/musa/core/context.cpp:1198-1217] `CreateStream` | 静态分析，未验证 |
| 本知识库格式 | `git diff --check` | 无（不涉及远端源码） | 仅本仓库 |
| 驱动库 WSL2 分支有独立产物属性 | 对照 `MUSA_WSL2_BUILD` 分支的 `SUFFIX`/soname | [src/driver/CMakeLists.txt:38-52] `MUSA_WSL2_BUILD` | 静态分析，未验证 |
| 工具安装目标与库目录分离 | 对照工具的 `install(TARGETS ...)` 落点 | [src/tools/CMakeLists.txt:16-18] `CMAKE_INSTALL_BINDIR` | 静态分析，未验证 |
| HAL 侧宏与根 option 同步 | 对照 `src/hal/m3d/CMakeLists.txt` 的宏注入块 | [src/hal/m3d/CMakeLists.txt:45-53] `add_definitions` | 静态分析，未验证 |

## CI 侧检查

- clang-tidy 作业独立于打包作业，失败只体现在报告与 job 状态上。[.ciConfig.yaml:90-97] `clang_tidy`
- clang-tidy 脚本先删旧构建目录再重新配置，避免脏缓存。[scripts/clang_tidy.py:40-44] `BUILD_DIR`
- clang-tidy 旧报告会被删除后重写。[scripts/clang_tidy.py:64-79] `report_file_path`
- 性能用例按 `pkgName` 拉取构建产物，用 `testMark` 过滤用例集。[.ciConfig.yaml:99-119] `testMark` `pkgName`
- CTS 用 `testType` 区分 smoke 变体，`testParallel` 控制并发。[.ciConfig.yaml:121-145] `testType`
- ASan 构建有独立 CTS 作业，通过 `envExport` 注入 `ASAN_LIB`。[.ciConfig.yaml:163-177] `testType`
- photon 链路先等 `commit_status` 再跑样例测试。[.mthreads-ci.yml:146-169] `commit_status`

## 运行验证（未执行）

```bash
cmake -S . -B build -DMUSA_BUILD_UT=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

目标环境需要私有子模块（`src/hal/m3d/m3d`）、mt-toolchain、内网 googletest 源、驱动和 GPU；本知识库没有声称上述命令成功。在当前快照里第一步就会因缺 `unittest/` 失败。

## 按分支的验证矩阵（未执行）

| 分支 | 命令 | 期望 | 证据 |
|---|---|---|---|
| Debug | `cmake -S . -B build -DMUSA_BUILD_DEBUG=ON` | `CMAKE_BUILD_TYPE=Debug`，不做 IPO | [CMakeLists.txt:127-132] `MUSA_BUILD_DEBUG` |
| 单测 | `cmake -S . -B build -DMUSA_BUILD_UT=ON` | 配置期失败（缺 `unittest/`） | [CMakeLists.txt:167-169] `unittest` |
| ASan | `cmake -S . -B build -DENABLE_ASAN_CHECK=ON` | 注入 `-fsanitize=address` | [CMakeLists.txt:110-113] `ENABLE_ASAN_CHECK` |
| TSan | `cmake -S . -B build -DENABLE_TSAN_CHECK=ON` | 注入 `-fsanitize=thread` | [CMakeLists.txt:115-118] `ENABLE_TSAN_CHECK` |
| gcov | `GCOV_TEST=1 cmake -S . -B build` | 注入 `-fprofile-arcs -ftest-coverage` | [CMakeLists.txt:120-123] `GCOV_TEST` |
| 打包 | `cmake -S . -B build -DMTUB_BUILD=ON && cmake --build build --target package` | 生成 deb/rpm | [CMakeLists.txt:40-61] `MTUB_BUILD` |
| 安装 | `bash install.sh -l /usr/lib/x86_64-linux-gnu -b /usr/local/bin` | 库与工具落盘 | [install.sh:32-49] `BIN_INSTALL_DIR` |
| 导出表 | 构建后比对 `.def`/`.ver` 与 `loader_api_list.inc` | 两份清单一致 | [cmake/export_symbols.cmake:64-109] `generate_export_files` |

## 建议新增的回归用例

| 用例 | 覆盖点 | 为什么现在没有 |
|---|---|---|
| 缺 `unittest/` 时的配置失败诊断 | [CMakeLists.txt:167-169] `add_subdirectory` | 快照本身缺目录，本地无法跑 |
| `CSV_UNSUPPORTED` 开关是否可达 | [CMakeLists.txt:25-25] `CSV_UNSUPPORTED` | 声明名与判断名不一致 |
| 导出符号重复即中止 | [cmake/export_symbols.cmake:52-59] `UNIQUE_MUSA_EXPORTED_SYMBOLS` | 需要构造重复条目 |
| install.sh 各种缺目录/缺权限分支 | [install.sh:27-30] `LIB_BUILD_DIR` | 需要可写文件系统 |
| CPack multiarch 与 lib64 分支 | [CPackProjectConfig.cmake:3-33] `ACTUAL_INSTALL_LIBDIR` | 需要 cpack 运行期 |

## 未覆盖与已知缺口

- 本知识库不声称任何构建、测试、打包或安装命令成功执行过。
- CI 测试作业依赖内网镜像、私有 `pkgName` 产物和真实 GPU 节点，本地不可复现。[.ciConfig.yaml:99-119] `containerImage`
- HAL/firmware 层的执行结果不在 M01 范围内，见 M09 边界。
- 模块专项用例见 [`../../80-demos/`](../../80-demos/)。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/CMakeLists.txt`（1 处，例：[src/CMakeLists.txt:7-15] `add_subdirectory`）
- `src/musa/core/context.cpp`（1 处，例：[src/musa/core/context.cpp:1198-1217] `CreateStream`）
- `src/driver/CMakeLists.txt`（1 处，例：[src/driver/CMakeLists.txt:38-52] `MUSA_WSL2_BUILD`）
- `src/tools/CMakeLists.txt`（1 处，例：[src/tools/CMakeLists.txt:16-18] `CMAKE_INSTALL_BINDIR`）
- `src/hal/m3d/CMakeLists.txt`（1 处，例：[src/hal/m3d/CMakeLists.txt:45-53] `MUSA_WSL2_BUILD`）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
