# 有序容器 Benchmark：`rb_tree`、`skip_list` 与标准库

本目录包含两类 benchmark：

- `rb_map_benchmark`：比较 map-like `rb_map` 与 `std::map`；
- `ordered_benchmark`：比较 set-like `rb_tree`、`llrb_tree`、`skip_list` 与 `std::set`。

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

每次测量都使用 checksum 消除编译器删除无效工作的可能性。填充容器的时间不计入 find、iterate 和 erase 的计时；三种容器都使用同一份数据填充。所有容器共享同一套 key 数据、命中/未命中 lookup、删除顺序、重复次数和 checksum。`llrb_tree` 也使用默认全局 `new/delete`，不使用池化、Arena 或自定义 allocator。

## 优化路线

### 1. LevelDB-style 参数、旧固定节点布局

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

### 2. Redis-style 变长节点布局

随后参考 Redis `zskiplist` 的节点布局思想，但保持分配基线不变：

- 节点只分配实际高度的 forward link，而不是每个节点携带完整 `MaxLevel` 数组；
- level-0 曾加入 `backward` 链接并缓存 `tail`，用于模拟 Redis-style 反向遍历能力；
- 插入和删除保留单次普通节点分配以及逐节点释放；
- 没有引入内存池、Arena、对象池或自定义 allocator；
- 当前 API 仍是 `set-like`，没有加入 Redis 的 score/member、dictionary、span/rank 和 range API。

该版本的代表结果如下。单位为 ns，中位数越低越好：

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

### 3. 当前 compact set-like 热路径优化

当前版本继续围绕“接近或超过 `rb_tree`，但不做池化”的目标优化 set-like workload：

- 去掉当前 benchmark 不使用的 `backward`/`tail` 字段，保留单向 level-0 迭代；
- `height` 从 `std::size_t` 压缩为 `std::uint8_t`，并限制 `MaxLevel <= 255`；
- `random_level()` 不再使用 `std::uniform_int_distribution`，而是直接读取 `mt19937` 输出的低两位决定 `1/4` 晋升；
- 搜索结束后的等价判断少做一次比较：搜索已保证候选节点不小于目标，只需再判断 `!(key < candidate)`；
- `update[]` 不再默认清零，只填充搜索实际使用的 active levels；新升高层级再显式设为 header 前驱。

这仍然保持每个真实节点一次普通全局 `operator new` 和一次对应 `operator delete`。

当前版本在同一轮 benchmark 中的结果如下：

| 容器 | workload | 1024 | 16384 | 65536 | 262144 |
|---|---:|---:|---:|---:|---:|
| `std::set` | insert | 44,104 | 1,491,270 | 8,788,393 | 81,129,599 |
| `rb_tree` | insert | 50,913 | 1,794,925 | 11,286,088 | 119,899,442 |
| `skip_list` | insert | 87,939 | 2,300,877 | 13,454,286 | 96,738,434 |
| `std::set` | find | 74,222 | 2,285,324 | 15,656,561 | 161,384,828 |
| `rb_tree` | find | 84,552 | 2,332,209 | 15,158,190 | 133,997,929 |
| `skip_list` | find | 95,674 | 2,933,473 | 18,891,893 | 154,318,776 |
| `std::set` | iterate | 4,396 | 164,755 | 1,385,152 | 18,577,305 |
| `rb_tree` | iterate | 4,169 | 148,069 | 1,139,469 | 16,227,856 |
| `skip_list` | iterate | 1,380 | 82,781 | 884,524 | 18,613,554 |
| `std::set` | erase | 67,248 | 2,020,548 | 11,896,939 | 102,785,963 |
| `rb_tree` | erase | 56,402 | 1,796,106 | 10,787,876 | 80,719,203 |
| `skip_list` | erase | 54,011 | 1,759,373 | 10,663,537 | 82,252,402 |

相对同轮 `rb_tree`，当前 `skip_list` 的中位数约为：

- insert：1.73x / 1.28x / 1.19x / 0.81x；
- find：1.13x / 1.26x / 1.25x / 1.15x；
- iterate：0.33x / 0.56x / 0.78x / 1.15x；
- erase：0.96x / 0.98x / 0.99x / 1.02x。

这说明当前目标已经部分达成：`skip_list` 在大规模 insert、多个 erase 规模和中小规模 iterate 上已经接近或超过 `rb_tree`；find 仍稳定落后，262144 的 iterate 也受运行环境抖动和 cache 状态影响，没有稳定领先。

### 4. 加入左倾红黑树后的验证结果

`ordered_benchmark` 现同时测量 `std::set`、parent-based `rb_tree`、无 parent 的 `llrb_tree` 和 `skip_list`。以下是当前环境 GCC Release 构建、固定 seed `20260911`、5 次重复、规模 `1024,16384,65536` 的一次验证运行；单位为 ns，中位数越低越好。四个容器每个 workload 的 checksum 一致，说明遍历和查找处理的是同一批 key。

| 容器 | workload | 1024 | 16384 | 65536 |
|---|---:|---:|---:|---:|
| `std::set` | insert | 53,132 | 1,456,013 | 8,275,929 |
| `rb_tree` | insert | 57,409 | 1,979,661 | 10,405,363 |
| `llrb_tree` | insert | 118,142 | 3,298,266 | 17,437,867 |
| `skip_list` | insert | 90,480 | 2,328,046 | 13,260,739 |
| `std::set` | find | 75,865 | 2,237,856 | 14,495,714 |
| `rb_tree` | find | 80,469 | 2,183,373 | 12,690,954 |
| `llrb_tree` | find | 82,286 | 2,111,018 | 12,812,182 |
| `skip_list` | find | 100,528 | 3,717,950 | 17,782,455 |
| `std::set` | iterate | 4,771 | 175,052 | 1,231,337 |
| `rb_tree` | iterate | 5,778 | 166,009 | 1,130,765 |
| `llrb_tree` | iterate | 35,334 | 758,656 | 4,275,046 |
| `skip_list` | iterate | 1,340 | 106,050 | 837,619 |
| `std::set` | erase | 70,250 | 1,983,790 | 11,423,845 |
| `rb_tree` | erase | 63,569 | 1,783,383 | 10,223,213 |
| `llrb_tree` | erase | 157,884 | 4,405,220 | 23,873,165 |
| `skip_list` | erase | 57,153 | 1,736,678 | 10,469,338 |

这次结果显示：LLRB 的 `find` 与普通 `rb_tree` 接近，在 16384 规模略快；但 `insert`、`iterate` 和 `erase` 明显更慢。主要原因不是节点分配基线变化，而是递归 top-down 颜色调整以及无 parent 迭代器每次从 root 搜索 successor/predecessor。LLRB 的价值是实现路径更短、更适合学习 2-3 树对应关系，并不意味着在这套端到端 workload 上全面优于 parent-based `rb_tree`。

### 1. 变长节点布局是最大收益来源

旧实现中每个节点都携带完整的 `MaxLevel=12` 个 forward 指针；新实现只为实际高度分配 link。由于 `p=1/4` 时约 75% 节点高度为 1，新布局显著减少节点大小和 level-0 遍历的 cache 流量，也降低了 insert/erase 时需要写入的高层指针数量。

### 2. 删除 set-like 不使用的字段能进一步逼近 rb_tree

`backward` 和 `tail` 对 Redis-style reverse range API 有价值，但当前 benchmark 只做正向迭代、查找、插入和按 key 删除。去掉它们后，每个节点少一个指针，insert/erase 少维护 level-0 反向链接；在当前数据中 erase 基本追平 `rb_tree`，大规模 insert 还超过了 `rb_tree`。

### 3. find 仍是主要短板

即使减少节点大小，skip list 查找仍是多层指针追踪：每次跨层和跨节点都可能造成 cache miss。红黑树虽然也追踪指针，但搜索路径更短、更确定；因此当前 find 仍比 `rb_tree` 慢约 13% 到 26%。后续若继续优化 find，需要考虑层级概率、最高层、比较器调用次数和节点布局对 cache 的影响。

### 4. 随机层级策略影响结构形状和可比性

直接用 `mt19937` 低两位生成高度减少了插入开销，但也改变了固定 seed 下的层级形状。因此跨版本比较时不能把所有差异都归因于单个字段或单条指令；同轮与 `rb_tree`、`std::set` 的相对结果更可靠。

### 5. 当前结果不能证明 SkipList 全面胜出

在当前 set-like 整数 workload 下，`skip_list` 已经不再“全方位差”：insert/erase/iterate 的部分规模可以接近或超过 `rb_tree`。但 find 仍落后，且 benchmark 对机器状态敏感，所以不能宣称跳表普遍优于红黑树。

Redis 使用 SkipList 的价值仍然不在简单 `set<int>` 的全面速度，而在 ordered set 所需的 score/member 排序、rank 和 range 操作。当前实现尚未加入这些 API，不能据此评价 Redis sorted set 的整体设计。

## 公平性与后续优化

- 三种容器使用同一 key 序列、查找键、删除键、重复次数和 checksum；
- skip list 使用同一 `MaxLevel=12`、晋升概率约 `1/4` 和 seed `20260911`；
- find、iterate、erase 的预填充不计入计时，insert 从空容器开始；
- 所有容器都使用普通全局 `new/delete`；
- 本轮没有池化、Arena、自定义 allocator 或内存资源；
- 本轮 skip list 节点的实际高度使用一次分配内的尾部存储，不能与旧固定布局直接解释为“算法复杂度改变”；
- 后续若实现 Redis 的 rank/range，需要增加 span 和可能的 backward/tail，但应单独 benchmark，因为它们会增加每个节点/每个 link 的元数据和更新成本。

## 原有 `rb_map` benchmark

如果需要比较 map-like 容器，请使用 `rb_map_benchmark.cpp`，其命令和说明仍适用于 `rb_map` 与 `std::map` 的对比。map-like benchmark 不应与本 set-like benchmark 混为一谈。

## 参考源码

- [Redis `src/t_zset.c`](https://github.com/redis/redis/blob/unstable/src/t_zset.c)
- [Redis sorted sets 文档](https://redis.io/docs/latest/develop/data-types/sorted-sets/)
