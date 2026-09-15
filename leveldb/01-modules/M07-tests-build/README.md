# M07 测试、基准与工程入口

- 文档目的：解释 CMake、测试注册、CI、benchmark 和安装入口。
- 适用范围：`CMakeLists.txt`、`.github/workflows/build.yml`、测试和 `benchmarks/`。
- 对应源码版本：source/leveldb HEAD 7ee830d（2026-09-15 只读确认）。
- 证据状态：构建配置和 CI 已确认；本机执行状态需以实际命令结果更新。
- 最后更新：2026-09-10
- 前置阅读：[M06 Env](../M06-env-platform/README.md)
- 后续阅读：[实践层 quick-start](../../99-roadmap/quick-start.md)

- 源码版本：`main`，HEAD `7ee830d`。

## 结论摘要

项目以 CMake 为唯一主要工程入口：C++17、C11、POSIX/Windows 平台选择、可选 crc32c/Snappy/Zstd/tcmalloc；默认构建静态库、测试、benchmark 和安装规则。GoogleTest 测试多数聚合到 `leveldb_tests`，另有 C API 和平台 Env 独立测试；CI 覆盖 Ubuntu/macOS/Windows 与 clang/gcc/msvc 的 Debug/RelWithDebInfo 组合。

## 关键入口

- 版本和语言：[CMakeLists.txt:4-22](../../../source/leveldb/CMakeLists.txt#L4-L22)
- 核心库源文件：[CMakeLists.txt:119-231](../../../source/leveldb/CMakeLists.txt#L119-L231)
- 测试注册：[CMakeLists.txt:292-404](../../../source/leveldb/CMakeLists.txt#L292-L404)
- benchmark：[CMakeLists.txt:406-468](../../../source/leveldb/CMakeLists.txt#L406-L468)
- 安装：[CMakeLists.txt:470-518](../../../source/leveldb/CMakeLists.txt#L470-L518)
- CI：[.github/workflows/build.yml:10-102](../../../source/leveldb/.github/workflows/build.yml#L10-L102)

## 相关文档

- [design](design.md)
- [source-map](source-map.md)
- [interfaces](interfaces.md)
- [data-structures](data-structures.md)
- [call-chains](call-chains.md)
- [diagrams](diagrams.md)
- [line-level-analysis](line-level-analysis.md)
- [examples](examples.md)
- [testing](testing.md)
- [development-guide](development-guide.md)
- [risks-and-debt](risks-and-debt.md)

## 文档元数据（规范补充）

- 文档目的：说明 `01-modules/M07-tests-build/README.md` 的源码分析范围、结论和维护入口。
- 适用范围：当前项目对应模块/入口的静态源码与测试分析。
- 对应源码版本：以本项目 `00-overview/analysis-state.md` 或同页版本字段为准。
- 证据状态：静态源码证据；未执行的构建、测试、GPU、网络或多进程行为保持“未验证”。
- 最后更新：2026-09-15
- 前置阅读：本项目根 README 与 `00-overview/analysis-state.md`。
- 后续阅读：本模块/示例的实现、测试和风险页面。

## 深度审计

| 分析对象 | 入口落地 | 正常路径 | 分支 | 异常 | 清理 | 数据生命周期 | 执行上下文 | 行级证据 | Demo 映射 | 状态/缺口 |
|---|---|---|---|---|---|---|---|---|---|---|
| `01-modules/M07-tests-build/README.md` | 已完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 已映射或不适用 | 部分完成：动态行为、边界或专用变体仍需验证 |

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](../../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](../../00-overview/analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
