# M07 源码地图

- 文档目的：定位工程入口、测试目标和 benchmark。
- 适用范围：M07。
- 源码版本：`main` / `7ee830d`。
- 证据状态：已确认。
- 最后更新：2026-09-10
- 前置阅读：[M07 design](design.md)
- 后续阅读：[M07 line-level-analysis](line-level-analysis.md)

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
