# M07 源码地图

- 文档目的：定位工程入口、测试目标和 benchmark。
- 适用范围：M07。
- 对应源码版本：source/leveldb HEAD 7ee830d（2026-09-15 只读确认）。
- 证据状态：已确认。
- 最后更新：2026-09-10
- 前置阅读：[M07 design](design.md)
- 后续阅读：[M07 line-level-analysis](line-level-analysis.md)
## 结论摘要

本页聚焦 01-modules/M07-tests-build/source-map.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


- 源码版本：`main` / `7ee830d`。

| 文件/目录 | 作用 |
|---|---|
| `CMakeLists.txt:4-34` | 项目版本、C/C++ 标准、构建选项 |
| `CMakeLists.txt:36-109` | 依赖和平台探测、生成配置 |
| `CMakeLists.txt:119-285` | leveldb 库源文件、链接、编译定义 |
| `CMakeLists.txt:287-291` | leveldbutil |
| `CMakeLists.txt:292-404` | GoogleTest/CTest 注册 |
| `CMakeLists.txt:406-468` | Google Benchmark 注册 |
| `CMakeLists.txt:470-518` | 安装和 CMake package |
| `.github/workflows/build.yml` | CI 矩阵和真实命令 |
| `db/*_test.cc` | DB/WAL/Version/恢复/格式测试 |
| `table/*_test.cc` | block/filter/table 测试 |
| `util/*_test.cc` | cache/arena/coding/CRC 等测试 |
| `helpers/memenv/memenv_test.cc` | 内存 Env 测试 |
| `benchmarks/db_bench.cc` | 读写、压缩、compaction benchmark |

## 依赖边界

`third_party/googletest` 和 `third_party/benchmark` 只作为构建依赖，不在本知识库中分析其内部实现。

## 相关文档
- [项目入口](../../README.md)
- [分析状态](../../00-overview/analysis-state.md)
- [源码证据索引](../../00-overview/evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](../../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](../../00-overview/analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
