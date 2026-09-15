# 有序容器 Benchmark：`rb_tree`、`skip_list` 与标准库

本目录包含两类 benchmark：

- `rb_map_benchmark`：比较 map-like `rb_map` 与 `std::map`；
- `ordered_benchmark`：比较 set-like `rb_tree`、`skip_list` 与 `std::set`。

benchmark 不是“实现一定更快”的证明，而是用于在相同 workload、编译器、硬件和编译选项下定位优势与瓶颈。

## 构建和运行

```bash
cmake -S data-structure -B data-structure/build/ordered-release \
  -DCMAKE_BUILD_TYPE=Release \
  -DRBTREE_BUILD_ORDERED_BENCHMARKS=ON
cmake --build data-structure/build/ordered-release -j2
ctest --test-dir data-structure/build/ordered-release --output-on-failure

./data-structure/build/ordered-release/ordered_benchmark \
  --sizes=1024,16384,65536,262144 \
  --repetitions=7 \
  --seed=20260911 \
  --csv=data-structure/build/ordered-release/ordered.csv
```

参数：

- `--sizes`：逗号分隔的数据规模；
- `--repetitions`：每个 workload 的重复次数，输出中位数、最小值和最大值；
- `--seed`：生成随机插入顺序和 skip list 随机层级的固定种子；
- 本次实验将 `skip_list` 的最大层数设为 `12`（`skip_list<int, std::less<int>, 12>`），每次向上一层晋升的概率约为 `1/4`；
- `--csv`：可选的 CSV 输出路径。

## 当前 workload

`ordered_benchmark` 测量 `int` set-like 容器：

1. `insert`：随机顺序批量插入；
2. `find`：对所有存在 key 和同等数量不存在 key 查找；
3. `iterate`：完整有序遍历；
4. `erase`：按随机顺序删除全部 key。

每次测量都使用 checksum 消除编译器删除无效工作的可能性。填充容器的时间不计入 find、iterate 和 erase 的计时；三种容器都使用同一份数据填充。所有容器都使用普通的全局 `new/delete` 基线；本轮 skip list 优化没有引入池化、Arena 或自定义分配器。

## Redis-style 布局优化

本轮优化参考 Redis `zskiplist` 的结构思想，但保持分配基线不变：

- 节点只分配实际高度的 forward link，而不是每个节点携带完整 `MaxLevel` 数组；
- level-0 增加 `backward` 链接并缓存 `tail`；
- 插入和删除保留单次普通节点分配以及逐节点释放；
- 没有引入内存池、Arena、对象池或自定义 allocator；
- 当前 API 仍是 `set-like`，没有加入 Redis 的 score/member、dictionary、span/rank 和 range API。

因此本轮结果主要测量“Redis-style 变长节点布局 + backward/tail”对现有 set-like workload 的影响，而不是完整 Redis sorted set。

## 基线：LevelDB-style 参数、旧固定节点布局

此前版本使用固定大小的节点 forward 数组，仅调整了 `MaxLevel=12` 和晋升概率约 `1/4`。在当前环境使用 GCC 11.4、Release、固定 seed `20260911`、7 次重复得到：

| 容器 | workload | 1024 | 16384 | 65536 | 262144 |
|---|---:|---:|---:|---:|---:|
| `std::set` | insert | 43,673 ns | 1,532,684 ns | 9,092,454 ns | 88,382,461 ns |
| `rb_tree` | insert | 51,291 ns | 1,811,098 ns | 11,265,404 ns | 121,610,563 ns |
| `skip_list` | insert | 89,106 ns | 2,694,044 ns | 16,318,903 ns | 188,255,983 ns |
| `std::set` | find | 83,268 ns | 2,353,647 ns | 15,617,179 ns | 160,745,643 ns |
| `rb_tree` | find | 78,667 ns | 2,068,752 ns | 13,364,262 ns | 133,409,038 ns |
| `skip_list` | find | 101,308 ns | 3,577,990 ns | 24,046,070 ns | 240,844,000 ns |
| `std::set` | iterate | 4,219 ns | 165,957 ns | 1,321,805 ns | 18,574,515 ns |
| `rb_tree` | iterate | 3,322 ns | 146,221 ns | 1,124,589 ns | 19,474,141 ns |
| `skip_list` | iterate | 4,069 ns | 216,794 ns | 2,328,729 ns | 35,693,147 ns |
| `std::set` | erase | 61,635 ns | 1,956,816 ns | 11,498,278 ns | 98,258,498 ns |
| `rb_tree` | erase | 60,584 ns | 1,811,902 ns | 10,778,717 ns | 85,487,459 ns |
| `skip_list` | erase | 72,632 ns | 2,439,830 ns | 17,269,776 ns | 174,784,333 ns |

## 本轮 Redis-style 实测结果

参数为 `MaxLevel=12`、晋升概率约 `1/4`、seed `20260911`、7 次重复。单位为 ns，中位数越低越好：

| 容器 | workload | 1024 | 16384 | 65536 | 262144 |
|---|---:|---:|---:|---:|---:|
| `std::set` | insert | 48,578 | 1,484,151 | 8,496,112 | 58,471,134 |
| `rb_tree` | insert | 52,699 | 1,862,216 | 10,797,923 | 72,158,401 |
| `skip_list` | insert | 96,032 | 2,516,227 | 14,395,964 | 96,102,860 |
| `std::set` | find | 82,130 | 2,283,425 | 14,189,350 | 100,074,829 |
| `rb_tree` | find | 81,633 | 2,202,775 | 13,159,292 | 94,021,340 |
| `skip_list` | find | 93,682 | 2,978,162 | 18,174,880 | 123,829,081 |
| `std::set` | iterate | 4,162 | 164,542 | 1,190,998 | 8,933,266 |
| `rb_tree` | iterate | 3,410 | 151,931 | 1,119,107 | 6,699,030 |
| `skip_list` | iterate | 2,086 | 93,092 | 993,234 | 8,532,975 |
| `std::set` | erase | 81,723 | 2,003,694 | 11,830,860 | 72,662,512 |
| `rb_tree` | erase | 57,803 | 1,831,476 | 10,384,413 | 65,249,894 |
| `skip_list` | erase | 70,568 | 1,988,619 | 12,065,211 | 81,082,947 |

相对 `rb_tree`，本轮 `skip_list` 的中位数约为：

- insert：1.82x / 1.35x / 1.33x / 1.33x；
- find：1.15x / 1.35x / 1.38x / 1.32x；
- iterate：0.61x / 0.61x / 0.89x / 1.27x；
- erase：1.22x / 1.09x / 1.16x / 1.24x。

与旧固定节点布局相比，skip list 在相同参数和相同 workload 下的变化为：

| workload | 1024 | 16384 | 65536 | 262144 |
|---|---:|---:|---:|---:|
| insert | +7.8% | -6.6% | -11.8% | -48.9% |
| find | -7.5% | -16.8% | -24.5% | -48.6% |
| iterate | -48.7% | -57.1% | -55.8% | -76.1% |
| erase | -2.8% | -18.5% | -30.2% | -53.7% |

正值表示变慢，负值表示变快。262144 的改进非常明显，但由于 benchmark 分批运行且机器状态会变化，百分比应理解为同一环境下的工程样例，而不是硬件无关的精确结论。

## 结果分析

### 1. 变长节点布局是本轮最大收益来源

旧实现中每个节点都携带完整的 `MaxLevel=12` 个 forward 指针；新实现只为实际高度分配 link。由于 `p=1/4` 时约 75% 节点高度为 1，新布局显著减少节点大小和 level-0 遍历的 cache 流量。遍历从 262144 节点的 35.69 ms 降到 8.53 ms，说明顺序访问的 cache locality 是旧实现的主要问题。

### 2. 查找、插入、删除仍落后于 rb_tree，但差距缩小

新布局没有改变 SkipList 的指针追踪特征，也没有改变默认分配器；因此 find 仍然需要跨多个离散节点，insert/erase 仍需要维护多层链。它们在大规模下比旧布局明显改善，但尚未超过 `rb_tree`。

### 3. 本轮没有池化，比较仍保持普通分配基线

每个真实节点仍通过一次普通全局 `operator new` 分配，并通过对应的 `operator delete` 释放。变长节点使用的是一次分配内的尾部 link 存储，不是第二次 link 分配，也不是池化。因此本轮主要隔离了节点布局影响，没有把 allocator 优势混入比较。

### 4. backward/tail 的收益主要是能力和遍历方向

当前 benchmark 只执行正向 level-0 遍历，`backward` 和 `tail` 并不会直接帮助这个 workload；本轮收益主要来自变长节点，而不是 backward/tail。它们为后续 Redis-style reverse range API 做了结构准备。

### 5. 当前结果不能证明 SkipList 全面胜出

在当前 set-like 整数 workload 下，`rb_tree` 仍然更快：新 SkipList 在 262144 规模下比 `rb_tree` 慢约 33%（insert）、32%（find）、27%（iterate）、24%（erase）。但相较旧布局，SkipList 已从“明显劣势”改善到可解释的常数差距。

Redis 使用 SkipList 的价值仍然不在简单 `set<int>` 的全面速度，而在 ordered set 所需的 score/member 排序、rank 和 range 操作。当前实现尚未加入这些 API，不能据此评价 Redis sorted set 的整体设计。

## 公平性与后续优化

- 三种容器使用同一 key 序列、查找键、删除键、重复次数和 checksum；
- skip list 使用同一 `MaxLevel=12`、晋升概率约 `1/4` 和 seed `20260911`；
- find、iterate、erase 的预填充不计入计时，insert 从空容器开始；
- 所有容器都使用普通全局 `new/delete`；
- 本轮没有池化、Arena、自定义 allocator 或内存资源；
- 本轮 skip list 节点的实际高度使用一次分配内的尾部存储，不能与旧固定布局直接解释为“算法复杂度改变”；
- 后续若实现 Redis 的 rank/range，需要增加 span，但应单独 benchmark，因为 span 会增加每个 link 的元数据和更新成本。

## 原有 `rb_map` benchmark

如果需要比较 map-like 容器，请使用 `rb_map_benchmark.cpp`，其命令和说明仍适用于 `rb_map` 与 `std::map` 的对比。map-like benchmark 不应与本 set-like benchmark 混为一谈。

## 参考源码

- [Redis `src/t_zset.c`](https://github.com/redis/redis/blob/unstable/src/t_zset.c)
- [Redis sorted sets 文档](https://redis.io/docs/latest/develop/data-types/sorted-sets/)
