# M07 行级分析

- 文档目的：标记工程配置的修改敏感区。
- 适用范围：M07。
- 源码版本：`main` / `7ee830d`。
- 证据状态：已确认。
- 最后更新：2026-09-10
- 前置阅读：[M07 source-map](source-map.md)
- 后续阅读：[M07 development-guide](development-guide.md)

| 行段 | 语义 | 修改注意 |
|---|---|---|
| `CMakeLists.txt:4-22` | 版本和语言标准 | ABI/编译器兼容 |
| `CMakeLists.txt:32-53` | 选项与依赖探测 | 不要误报可选能力 |
| `CMakeLists.txt:55-77` | exceptions/RTTI 规则 | 代码不能依赖异常/RTTI |
| `CMakeLists.txt:119-231` | 库源文件与平台文件 | 新源文件要纳入正确平台 |
| `CMakeLists.txt:264-285` | 编译选项和链接库 | feature macro 与链接必须一致 |
| `CMakeLists.txt:312-364` | 聚合测试 | 源文件和目标依赖 |
| `CMakeLists.txt:366-404` | 独立 CTest | test name/平台条件 |
| `CMakeLists.txt:406-468` | benchmark 与外部 DB | 可选依赖不能阻断基础构建 |
| `CMakeLists.txt:470-518` | install/export | 公共 ABI 和 package 配置 |
| `build.yml:20-47` | CI 矩阵 | 支持平台声明 |
| `build.yml:74-102` | CI 命令序列 | 文档命令应与其一致 |
| `README.md:89-115` | 贡献/维护政策 | 变更范围和测试要求 |

## 调试变量

查看 CMake cache 中 `HAVE_*`、`LEVELDB_BUILD_*`、`BUILD_SHARED_LIBS`、编译器标识、CTest 列表和 benchmark flags。
