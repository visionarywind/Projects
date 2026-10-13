# 构建/发布/安装系统：风险与技术债

- 文档目的：build release install：风险、技术债及源码证据与修复/验证建议。
- 适用范围：M01 模块的《构建/发布/安装系统：风险与技术债》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-08
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


| 风险 | 影响 | 证据/状态 | 建议 |
|---|---|---|---|
| 快照缺 `unittest/` 目录 | `MUSA_BUILD_UT=ON` 在 `add_subdirectory(unittest)` 直接失败 | 已确认：目录缺失，`add_subdirectory` 分支仍在 [CMakeLists.txt:167-169] | 使用前补齐目录或关闭该开关 |
| 快照缺 `tool/` 目录与 `code_reason.py` | `CSV_UNSUPPORTED` 路径无法运行 | 已确认：目录缺失，脚本引用仍在 [CMakeLists.txt:25] [CMakeLists.txt:134-136] | 补脚本或删除引用 |
| `src/musa/core/armory/include/` 不存在 | tools 的 include 目录悬空 | 已确认：目录缺失，`include_directories` 引用仍在 [src/tools/CMakeLists.txt:5-8] | 确认上游生成流程是否补充该目录 |
| `generated_*` 生成脚本不在快照 | 无法复现 `generated_musa*_meta.h` 等头文件的再生成 | 推断：快照中仅有产物头文件，无生成脚本 | 向源码方索要生成工具 |
| `ddk_build.sh` 不在本仓库 | `.ciConfig.yaml` 的 `parameters` 块命令无法在 musa 快照内复现 | 已确认：musa 仓库无此文件，命令引用在 [.ciConfig.yaml:12-16] | 属 linux-ddk 仓库内容 |
| 选项名拼写不一致 | `CSV_UNSUPPORTED` 声明但 `CSV_UNSPPORTED` 判断 | 已确认：[CMakeLists.txt:25] 与 [CMakeLists.txt:134] | 修正拼写或统一 |
| m3d 子模块为私有 | 无法静态证明硬件链路编译细节 | 已知缺口 | 在目标源码补证 |
| ASAN/TSAN 互斥 | 同时打开会双注入 sanitizer 标志 | 推断（README 亦声明互斥） | 二选一 |
| 外网依赖不可控 | CI 镜像与依赖仓库 `sh-harbor.mthreads.com/qa/linux-ddk:v36` 等需要外网 | 已确认：[.ciConfig.yaml:13] `containerImage` | 固化镜像与 artifact |
| 构建环境依赖 | 本地无法复现（缺子模块/驱动/GPU） | CI/子模块已确认 | 固化依赖和 artifact |
| 统一导出只在 Release 分支触发 | 非 Release 产物的导出面与 `.ver`/`.def` 对不上，比对脚本会误报 | 已确认：[src/driver/CMakeLists.txt:19-36] `setup_unified_exports` | 比对固定在 Release 产物上做 |
| 工具目标由 `file(GLOB_RECURSE ...)` 生成 | 新增 `.cpp` 不重新配置就漏编、漏装 | 已确认：[src/tools/CMakeLists.txt:10-15] `TOOLS_SOURCES` | 改动后强制重新 configure |
| loader 分支在非 Windows/WSL 直接配置失败 | CI 若误开 loader 相关目标，配置期整体中断 | 已确认：[src/loader/CMakeLists.txt:1-7] `FATAL_ERROR` | 平台判断保持在 `if` 条件里 |
| HAL 侧宏与根 option 是两套来源 | 根开关改动未必同步到 `src/hal/m3d` 的宏 | 已确认：[src/hal/m3d/CMakeLists.txt:1-12] `M3D_DEBUG_BUILD` | 两边成对检查 |
| Core 源码按目录整体收集 | 漏写一行收集规则，整个子目录不参与构建 | 已确认：[src/musa/core/CMakeLists.txt:1-7] `aux_source_directory` | 新增子目录时补收集行 |

本模块具体风险按其入口和资源类型排序；未执行的测试不标为通过。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/tools/CMakeLists.txt`（2 处，例：[src/tools/CMakeLists.txt:5-8] `include_directories`）
- `src/driver/CMakeLists.txt`（1 处，例：[src/driver/CMakeLists.txt:19-36] `setup_unified_exports`）
- `src/loader/CMakeLists.txt`（1 处，例：[src/loader/CMakeLists.txt:1-7] `FATAL_ERROR`）
- `src/hal/m3d/CMakeLists.txt`（1 处，例：[src/hal/m3d/CMakeLists.txt:1-12] `M3D_DEBUG_BUILD`）
- `src/musa/core/CMakeLists.txt`（1 处，例：[src/musa/core/CMakeLists.txt:1-7] `aux_source_directory`）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 上表风险的运行期复现与修复验证均未执行。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
