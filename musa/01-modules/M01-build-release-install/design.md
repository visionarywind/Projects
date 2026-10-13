# 构建/发布/安装系统：设计分析

- 文档目的：build release install：设计意图、职责边界与关键取舍。
- 适用范围：M01 模块的《构建/发布/安装系统：设计分析》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-08
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


## 设计问题

该模块把 `CMake 配置 -> 编译 -> 链接 -> 测试/安装` 拆成稳定边界：根 `CMakeLists.txt` 负责契约（option 与版本），各子目录负责目标与依赖，`install.sh`/CPack 负责落盘，CI 配置负责流水线。这样做使产物规则与底层 m3d 子模块解耦，但也造成多层条件分支（平台、调试、打包、WSL2）交织在同一个配置树里。

## 设计取舍

| 取舍 | 好处 | 代价 | 证据状态 |
|---|---|---|---|
| 单根 `CMakeLists.txt` 承载全部 `option` | 一处看全开关 | 平台/打包/调试分支互相影响 | 已确认 [CMakeLists.txt:21-31] |
| 静态库分层（util / `halM3d` / `musaCore`） | 编译期顺序清晰、便于复用 | 每层透传 include 与宏 | 已确认 [src/musa/core/CMakeLists.txt:1-11] |
| 导出统一由 loader 清单驱动 | 单份真实清单 `MUSA_LOADER_API_MANIFEST`，防符号漂移 | Windows/Linux 双 manifest 维护 | 已确认 [cmake/export_symbols.cmake:11-59] |
| mt-toolchain 可选接入 | 支持 CPack 版本化打包 | 需要 `MTUB_BUILD` 显式打开 | 已确认 [CMakeLists.txt:40-61] |
| install.sh 按发行版探测路径 | 兼容多发行版（`get_distro` 探测） | 脚本内重复实现 CMake 已有的安装逻辑 | 已确认 [install.sh:51-81] |
| 静态核 `musaCore` 只链接 `halM3d` 一个硬件出口 | 硬件相关性收敛到单一边界 | include 路径由各层 `target_include_directories` 手工透传 | 已确认 [src/musa/core/CMakeLists.txt:7-11] |
| 驱动目标名由平台决定再拼后缀 | Linux/Windows/WSL2 共用同一套目标定义 | 任何平台改动都要回到 `DRIVER_LIB_NAME` 的赋值处 | 已确认 [src/CMakeLists.txt:1-5] |
| 统一导出只在 Release 分支触发 | 防符号漂移，产物只暴露清单内符号 | Debug/Windows 分支的 `WINDOWS_EXPORT_ALL_SYMBOLS` 口径与 Release 不同 | 已确认 [src/driver/CMakeLists.txt:19-36] |
| 工具目标由 `file(GLOB_RECURSE ...)` 循环生成 | 新增一个工具不改构建脚本 | 新增源文件要重新配置才被 `TOOLS_SOURCES` 捕获 | 已确认 [src/tools/CMakeLists.txt:10-18] |

## 分层边界的单向依赖

- 子目录层只做两件事：按平台设 `DRIVER_LIB_NAME`，再按固定顺序注册 util → hal → musa → driver →（Windows/WSL2 才有）loader → gdb → tools；目标级细节全部下沉到子目录。[src/CMakeLists.txt:1-15]
- `musaCore` 把 core/command/node/graph/copyManager2 的源码聚成静态库并链接 `halM3d`，硬件差异只能从这条边进入 Core。[src/musa/core/CMakeLists.txt:1-11]
- loader 目标 `musa_loader` 的平台前置是配置期硬门禁：非 64 位 Windows/WSL 直接 `message(FATAL_ERROR ...)`。[src/loader/CMakeLists.txt:1-12]

## 不变量

1. 公共句柄在使用前必须能映射到有效对象。（对构建系统：产物路径必须由 `BUILD_ROOT_DIR` 统一派生，各子目录的输出目录都落到它下面。）[CMakeLists.txt:157-158]
2. Core 资源登记成功前，底层初始化失败不得暴露半初始化对象。（对构建系统：`unittest` 目录缺失时 `MUSA_BUILD_UT` 配置期即失败，不进入半成品构建。）[CMakeLists.txt:167-169]
3. 异步操作的资源必须活到对应 Command 完成。（对构建系统：CI 测试 job 必须等 `commit_status` 门禁通过才拉取驱动包。）[.mthreads-ci.yml:146-168]
4. 销毁路径必须先处理未完成工作，再释放 HAL 对象。（对构建系统：`install.sh` 先检查 `LIB_BUILD_DIR` 存在再复制。）[install.sh:27-30]

以上第 1、2、4 项由源码行号支持；第 3 项是 CI 流水线的门禁约束，完整证明仍需覆盖所有测试 job。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/musa/core/CMakeLists.txt`（3 处，例：[src/musa/core/CMakeLists.txt:1-11] `musaCore`）
- `src/CMakeLists.txt`（2 处，例：[src/CMakeLists.txt:1-15] `add_subdirectory`）
- `src/driver/CMakeLists.txt`（1 处，例：[src/driver/CMakeLists.txt:19-36] `setup_unified_exports`）
- `src/tools/CMakeLists.txt`（1 处，例：[src/tools/CMakeLists.txt:10-18] `TOOLS_SOURCES`）
- `src/loader/CMakeLists.txt`（1 处，例：[src/loader/CMakeLists.txt:1-12] `musa_loader`）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
