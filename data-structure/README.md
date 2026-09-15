# 学习型高性能红黑树与跳表

这是一个从零实现的、**单线程、唯一键** C++ 有序容器学习项目，位于本仓库的 `data-structure/`，可以独立使用 CMake 构建，不依赖仓库根目录的构建系统。

项目同时保留两条实现路线：

- `rb_tree<Key, Compare>`：学习优先的 set-like 实现，使用 `nullptr` 表示叶子；
- `rb_map<Key, T, Compare, Allocator>`：面向性能实验的 map-like 实现，使用黑色哨兵和节点 allocator；
- `skip_list<Key, Compare, MaxLevel>`：教学版 set-like 跳表，使用随机层级和多层 forward 指针。

## 当前状态

当前版本已经包含：

- `rb_tree` 的 `insert`、`contains`、`find`、`erase`、`clear`、`size`、`empty`；
- 左旋、右旋、插入修复和删除修复；
- 中序双向迭代器与 `--end()`；
- `rb_map` 的 `insert`、`try_emplace`、`emplace`、`operator[]`、`find`、`contains`、`erase`、`clear` 和遍历；
- `rb_map` 的黑色 `nil_` 哨兵、`leftmost_`/`rightmost_` 边界缓存和 allocator-aware 节点分配；
- 两套实现的不变量检查、基础测试和固定 seed 差分测试；
- `rb_map<int, uint64_t>` 与 `std::map<int, uint64_t>` 的 Release benchmark，支持中位数、最小值、最大值和 CSV 输出；
- `rb_tree<int>`、`skip_list<int>` 与 `std::set<int>` 的可重放性能对比 benchmark；
- `skip_list` 的固定 seed 基础测试和与 `std::set<int>` 的随机差分测试。

当前版本有意不包含：

- 线程安全和并发读写；
- 完整 STL 兼容 API、node handle 和异构查找；
- 节点池或自定义内存资源的专用实现；
- 序列化和持久化。

## 构建和运行

从仓库根目录执行：

```bash
cmake -S data-structure -B data-structure/build/debug \\
  -DCMAKE_BUILD_TYPE=Debug
cmake --build data-structure/build/debug
ctest --test-dir data-structure/build/debug --output-on-failure
./data-structure/build/debug/rbtree_demo
```

构建 benchmark：

```bash
cmake -S data-structure -B data-structure/build/bench-release \\
  -DCMAKE_BUILD_TYPE=Release \\
  -DRBTREE_BUILD_BENCHMARKS=ON
cmake --build data-structure/build/bench-release -j2
./data-structure/build/bench-release/rbtree_benchmark \\
  --sizes=1024,16384,262144 --repetitions=7 --seed=20260911 \\
  --csv=data-structure/build/bench-release/rb-vs-std-map.csv
```

建议的 Sanitizer 验证：

```bash
cmake -S data-structure -B data-structure/build/asan \\
  -DCMAKE_BUILD_TYPE=Debug \\
  -DCMAKE_CXX_FLAGS='-fsanitize=address,undefined -fno-omit-frame-pointer -g'
cmake --build data-structure/build/asan
ctest --test-dir data-structure/build/asan --output-on-failure
```

## 已验证的基准样例

在当前环境使用 GCC、Release 优化、固定 seed `20260911`、3 次重复运行了规模 `1024,8192` 的样例。结果显示：`rb_map` 在 erase 和部分 find/iterate workload 上领先，但 insert 仍慢于 `std::map`。因此不能据此宣称普遍“吊打” `std::map`；应以相同编译器、硬件、allocator、数据规模和 workload 做可复现比较。

## 推荐阅读顺序

1. [`docs/architecture.md`](docs/architecture.md)：先看总体架构、节点布局和插入/删除/验证流程图；
2. `docs/invariants.md`：两种实现必须保持的不变量；
3. `include/rbtree/rb_tree.hpp`：先看教学版的 `insert`、旋转和 `insert_fixup` 注释；
4. `include/rbtree/rb_map.hpp`：再看哨兵、allocator、map 节点布局和 `erase_fixup` 注释；
5. `tests/rb_map_differential_test.cpp`：看如何逐步对照 `std::map`；
6. `benchmark/rb_map_benchmark.cpp`：看如何固定 workload、校验 checksum 并记录时间；
7. `docs/implementation-notes.md` 与 `docs/testing-and-verification.md`：了解实现取舍和验证方法。

## 跳表快速示例

```cpp
#include "rbtree/skip_list.hpp"

rbtree::skip_list<int> list;
list.insert(3);
list.insert(1);
list.insert(2);
for (int key : list) {
    // 通过 level-0 按序访问：1, 2, 3
}
```

跳表的查找、插入和删除期望为 `O(log n)`，最坏情况可能退化为 `O(n)`；当前实现使用固定 seed 构造以便测试复现，并已通过 `ordered_benchmark` 与 `rb_tree`、`std::set` 做端到端性能对比。当前 benchmark 的跳表参数为 `MaxLevel=12`、晋升概率约 `1/4`，但仍使用默认 `new/delete` 和固定大小节点布局，不能视为完整的 LevelDB SkipList 实现。

## 性能原则

红黑树的 `O(log n)` 复杂度不意味着一定快过标准库。benchmark 的目标是找出明确 workload 上的优势，而不是预先承诺普遍胜出。任何 allocator、节点布局或算法优化，都必须同时通过正确性、Sanitizer 和可重放 benchmark。
