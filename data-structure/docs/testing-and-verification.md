# 测试与验证

## 当前测试

`rbtree_tests` 不依赖第三方框架，包含以下测试。总体验证数据流见 [`architecture.md`](architecture.md) 的“测试与 benchmark 数据流”章节。

- 空树、重复插入、查找、清空和重新使用；
- 左左、右右、左右、右左插入序列；
- 叶子、单子树、双子树、根、递增插入后反向删除；
- 正向和反向迭代器、`find`、const 访问；
- 固定 seed 的 10000 步随机操作，并与 `std::set<int>` 逐步比较。

所有结构性测试在操作后调用 `verify_invariants()`，这样能尽量定位首次破坏不变量的操作。

## Debug 和 Sanitizer

```bash
cmake -S data-structure -B data-structure/build/debug -DCMAKE_BUILD_TYPE=Debug
cmake --build data-structure/build/debug
ctest --test-dir data-structure/build/debug --output-on-failure

cmake -S data-structure -B data-structure/build/asan \\
  -DCMAKE_BUILD_TYPE=Debug \\
  -DCMAKE_CXX_FLAGS='-fsanitize=address,undefined -fno-omit-frame-pointer -g'
cmake --build data-structure/build/asan
ctest --test-dir data-structure/build/asan --output-on-failure
```

这些命令需要实际执行后才能记录为“已验证”。ASan/UBSan 主要用于发现越界、use-after-free、double free、未定义指针访问等问题；它们不能替代红黑不变量检查。

## `rb_map` 回归与 benchmark

`rb_map` 测试额外覆盖：

- `try_emplace` 的重复 key 语义和 `operator[]` 默认构造；
- map value 的中序顺序、`--end()`、删除、清空和哨兵状态；
- 固定 seed 的 20000 步随机操作，并逐步与 `std::map<int, int>` 比较 size、内容、查找、插入和删除结果。

Release benchmark 使用 `rb_map<int, uint64_t>` 对照 `std::map<int, uint64_t>`，固定随机 seed，测量 insert/find/iterate/erase，并把 checksum 写入 CSV。benchmark 不是正确性测试：它不调用昂贵的不变量检查，正式运行前应先通过 CTest 和 Sanitizer。

```bash
cmake -S data-structure -B data-structure/build/bench-release \\
  -DCMAKE_BUILD_TYPE=Release -DRBTREE_BUILD_BENCHMARKS=ON
cmake --build data-structure/build/bench-release -j2
ctest --test-dir data-structure/build/bench-release --output-on-failure
./data-structure/build/bench-release/rbtree_benchmark \\
  --sizes=1024,16384,262144 --repetitions=7 --seed=20260911
```

## 后续测试

核心正确后应增加：

- 自定义比较器（例如降序）；
- 更长的固定 seed 操作序列和失败 seed 重放；
- GCC 与 Clang 的构建；
- Release benchmark，与 `std::set` 对比随机/有序输入、查找、删除和遍历。

没有固定 workload、编译器、硬件和 seed 的数据时，不应声称实现“高性能”。
