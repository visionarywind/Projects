# M07 示例

- 文档目的：给出真实仓库中的构建、测试和 benchmark 入口。
- 适用范围：M07。
- 源码版本：`main` / `7ee830d`。
- 证据状态：Linux Debug configure/build、CTest 和有限 `db_bench` 已在本机执行；Release/跨平台/Sanitizer 和更广泛 benchmark 尚未执行。
- 最后更新：2026-09-10
- 前置阅读：[M07 interfaces](interfaces.md)
- 后续阅读：[M07 testing](testing.md)

## POSIX 快速构建

README 提供：

```bash
mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE=Release .. && cmake --build .
```

证据：[README.md:39-50](../../../source/leveldb/README.md#L39-L50)。

## CI 等价配置

以下命令与 CI 的 configure/build/test/install 步骤一致：

```bash
cmake -S . -B build \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_INSTALL_PREFIX=/tmp/leveldb-install
cmake --build build --config Debug
ctest --test-dir build -C Debug --verbose
cmake --build build --target install
```

本机已验证等价的 Debug configure/build/CTest 流程；CTest 3/3 通过。此前已执行本地 prefix install（目标为 `build/install`）。上面的 `/tmp/leveldb-install` 路径不是本轮实际安装路径，若重新执行需单独记录结果。[.github/workflows/build.yml:74-102](../../../source/leveldb/.github/workflows/build.yml#L74-L102)

## Benchmark 场景

`db_bench` 默认包含 `fillseq`、`fillsync`、`fillrandom`、`readrandom`、`readseq`、`readreverse`、`compact`、CRC 和压缩场景。[benchmarks/db_bench.cc:23-64](../../../source/leveldb/benchmarks/db_bench.cc#L23-L64) 修改性能相关模块时优先固定 `--benchmarks`、数据量、线程数和 block/write buffer 参数。
