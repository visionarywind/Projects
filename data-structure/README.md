# 学习型高性能红黑树

这是一个从零实现的、**单线程、唯一键、有序集合风格** C++ 红黑树。它位于本仓库的 `data-structure/`，可以独立使用 CMake 构建，不依赖仓库根目录的构建系统。

## 当前状态

当前版本已经包含：

- `insert`、`contains`、`find`、`erase`、`clear`、`size`、`empty`；
- 左旋、右旋、插入修复和删除修复；
- 中序双向迭代器；
- `verify_invariants()` 诊断检查器；
- 基础、旋转、删除、迭代器和 `std::set` 差分测试。

当前版本有意不包含：

- 线程安全和并发读写；
- `map<Key, Value>`、`operator[]`、`emplace`；
- allocator-aware API、节点池和自定义内存资源；
- 序列化、持久化和 node handle；
- 性能结论。正式 benchmark 会在正确性稳定后加入。

## 构建和运行

从仓库根目录执行：

```bash
cmake -S data-structure -B data-structure/build/debug \\
  -DCMAKE_BUILD_TYPE=Debug
cmake --build data-structure/build/debug
ctest --test-dir data-structure/build/debug --output-on-failure
./data-structure/build/debug/rbtree_demo
```

建议的 Sanitizer 验证：

```bash
cmake -S data-structure -B data-structure/build/asan \\
  -DCMAKE_BUILD_TYPE=Debug \\
  -DCMAKE_CXX_FLAGS='-fsanitize=address,undefined -fno-omit-frame-pointer -g'
cmake --build data-structure/build/asan
ctest --test-dir data-structure/build/asan --output-on-failure
```

上面的命令需要本机有 CMake、C++17 编译器以及可用的 AddressSanitizer/UBSan；命令执行前只是验证方案，实际结果以本地输出为准。

## 推荐阅读顺序

1. `docs/invariants.md`：红黑树必须保持什么不变量；
2. `include/rbtree/rb_tree.hpp`：先看 `insert`、旋转和 `insert_fixup`；
3. 再看 `erase`、`transplant` 和 `erase_fixup`；
4. `tests/rb_tree_insert_test.cpp` 和 `tests/rb_tree_erase_test.cpp`：每个算法案例如何被验证；
5. `docs/implementation-notes.md`：实现取舍和 `nullptr` 叶子方案；
6. `docs/testing-and-verification.md`：如何增加回归测试和运行 Sanitizer。

## 性能路线

第一版优先建立正确性证据。后续只有在固定 workload、编译器、数据规模和 seed 下完成基准后，才考虑黑色哨兵、节点布局、迭代化热路径或节点 allocator。一次本地运行不能证明“比 `std::set` 更快”。
