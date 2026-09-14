# `rb_map` 与 `std::map` Benchmark

本目录包含 `rb_map` 与标准库 `std::map` 的可重放性能对比。benchmark 不是“实现一定更快”的证明，而是用于在相同 workload、编译器、硬件和编译选项下定位优势与瓶颈。

## 构建和运行

```bash
cmake -S data-structure -B data-structure/build/bench-release \
  -DCMAKE_BUILD_TYPE=Release \
  -DRBTREE_BUILD_BENCHMARKS=ON
cmake --build data-structure/build/bench-release -j2

./data-structure/build/bench-release/rbtree_benchmark \
  --sizes=1024,16384,262144 \
  --repetitions=7 \
  --seed=20260911 \
  --csv=data-structure/build/bench-release/rb-vs-std-map.csv
```

参数：

- `--sizes`：逗号分隔的数据规模；
- `--repetitions`：每个 workload 的重复次数，输出中位数、最小值和最大值；
- `--seed`：生成随机插入顺序的固定种子；
- `--csv`：可选的 CSV 输出路径。

## 当前 workload

当前实现测量 `int -> uint64_t` 的：

1. `insert`：随机顺序批量插入；
2. `find`：对所有存在 key 和同等数量不存在 key 查找；
3. `iterate`：完整中序遍历；
4. `erase`：按随机顺序删除全部 key。

每次测量都使用 checksum 消除编译器删除无效工作的可能性。填充容器的时间不计入 find、iterate 和 erase 的计时；两种容器都在计时前用同一份数据填充。

## 样例结果

在当前环境使用 Release 构建、固定 seed `20260911`、3 次重复运行 `1024` 和 `8192` 得到：

| 容器 | workload | size | median ns |
|---|---:|---:|---:|
| `std::map` | insert | 1024 | 57109 |
| `rb_map` | insert | 1024 | 64993 |
| `std::map` | find | 1024 | 78321 |
| `rb_map` | find | 1024 | 81227 |
| `std::map` | iterate | 1024 | 6997 |
| `rb_map` | iterate | 1024 | 6866 |
| `std::map` | erase | 1024 | 74820 |
| `rb_map` | erase | 1024 | 66129 |
| `std::map` | insert | 8192 | 667558 |
| `rb_map` | insert | 8192 | 825801 |
| `std::map` | find | 8192 | 967669 |
| `rb_map` | find | 8192 | 949011 |
| `std::map` | iterate | 8192 | 74997 |
| `rb_map` | iterate | 8192 | 69920 |
| `std::map` | erase | 8192 | 882504 |
| `rb_map` | erase | 8192 | 794531 |

这组结果表明，当前实现已经在部分删除、查找和遍历 workload 上接近或超过 `std::map`，但批量插入仍较慢。不能把局部领先外推为普遍性能优势。

## 公平性和后续优化

- 当前两者都使用各自默认 allocator；这是真实端到端容器对比，但不能隔离 allocator 成本。
- 后续如果实现节点池，应额外提供“双方使用同等分配策略”的对照，避免把 allocator 优势误认为树算法优势。
- 只有在 benchmark 和 profiling 指出分配是主要瓶颈后，才引入池化节点；不要只凭理论优化。
- 每次优化都应先运行 CTest 和 Sanitizer，再用相同 seed、规模和重复次数重跑 CSV。
- CPU、操作系统、编译器版本和完整编译选项应随正式结果一并记录。
