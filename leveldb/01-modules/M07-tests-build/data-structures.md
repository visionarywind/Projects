# M07 数据结构与构建产物

- 文档目的：说明构建图和测试注册的主要对象。
- 适用范围：M07。
- 源码版本：`main` / `7ee830d`。
- 证据状态：已确认。
- 最后更新：2026-09-10
- 前置阅读：[M07 interfaces](interfaces.md)
- 后续阅读：[M07 call-chains](call-chains.md)

## 构建图

```text
CMake configure
  -> port_config.h + feature macros
  -> leveldb library
     -> platform Env + port + db/table/util
  -> leveldb_tests / c_test / env_*_test
  -> db_bench (optional)
  -> install export/package
```

## CTest 注册

`leveldb_tests` 作为一个 CTest；`leveldb_test()` 为每个独立源文件创建 executable 和 test name。[CMakeLists.txt:364-391](../../../source/leveldb/CMakeLists.txt#L364-L391) 因而失败输出可能来自聚合测试内部，也可能来自独立目标，排查时先看 CTest 名称。

## 配置状态

`port_config.h` 记录 include/library/symbol 探测结果；`HAVE_CRC32C`、`HAVE_SNAPPY`、`HAVE_ZSTD`、`HAVE_TCMALLOC` 会影响链接和代码分支。[CMakeLists.txt:39-53](../../../source/leveldb/CMakeLists.txt#L39-L53)
