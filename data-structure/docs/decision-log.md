# 决策记录

## D1：先实现 set-like，而不是 map

学习目标集中在节点链接、旋转、颜色和修复算法。`map<Key, Value>` 会额外引入键提取、值构造和对象异常路径，因此推迟到核心树稳定之后。

## D2：初版使用 nullptr 叶子

优点是边界直观、调试容易；代价是删除修复时空位置没有 parent。通过 `erase_fixup(current, parent_of_current)` 显式传递父节点解决。黑色 `nil_` 哨兵作为后续有基准支持的独立优化，而不是首版复杂度。

## D3：公共 API 先保持最小

先提供唯一键插入、查找、删除、清空、遍历和不变量检查，不提供 allocator、节点池、并发、序列化或完整 STL 兼容承诺。每增加一个 API 都要先定义所有权、异常和迭代器失效语义。

## D4：不依赖第三方测试框架

当前仓库是 umbrella 文档/源码理解仓库，`data-structure` 需要独立且低依赖。轻量断言足够覆盖算法学习；以后若测试规模扩大，可以单独评估 Catch2/GoogleTest，而不是现在引入依赖。

## D6：新增独立的 `rb_map`

为了支持与 `std::map` 的同语义比较，新增 `rb_map<Key, T, Compare, Allocator>`，不修改教学版 `rb_tree` 的 set-like API。map 节点使用不可修改的 `const Key`，并通过 `allocator_traits` 重绑定 allocator 管理节点生命周期。当前拷贝和移动仍显式禁用，避免在资源传播语义未实现时制造假保证。

## D7：性能版使用共享黑色哨兵

`rb_map` 采用每棵树一个黑色 `nil_` 哨兵，统一真实节点的空孩子表示并简化删除修复；同时缓存最左和最右节点。共享哨兵带来一个额外约束：旋转不能无条件修改哨兵 parent，删除修复也不能把哨兵染红。教学版保留 `nullptr` 叶子，便于对比两种设计。

## D8：benchmark 结果只做条件性结论

benchmark 固定 seed、数据规模、重复次数和 checksum，输出 median/min/max 与 CSV。当前样例中 `rb_map` 在部分 find/iterate/erase workload 上领先，但 insert 仍慢于 `std::map`，所以项目不宣称普遍击败标准库；后续优化必须以可重放数据和公平 allocator 对照为依据。

## D9：注释与架构图和实现同步维护

代码注释负责解释局部状态变化、所有权和红黑树 case；`docs/architecture.md` 负责解释组件边界、节点布局、调用链和验证数据流。新增或修改算法路径时，两处都必须更新，并保持 Mermaid 节点名称与实际类/函数一致，避免文档图和代码逐渐分叉。

## D10：注释优先解释不变量，而不是重复语句

详细注释应回答“为什么这样链接、何时可以染色、谁拥有节点、哪一个状态暂时允许”，而不是逐行翻译 C++ 语法。这样初学者可以从局部代码连接到架构图和红黑树不变量，同时避免注释在重命名后失去价值。

详见 [`architecture.md`](architecture.md)。

## D11：第一版新增 set-like skip_list

跳表与红黑树都提供有序唯一键容器，但它们的平衡机制不同：红黑树依赖确定性颜色不变量，跳表依赖随机层级。第一版只实现 `skip_list<Key, Compare, MaxLevel>`，保留底层链表和多层 `forward` 指针的学习路径；map value、并发、节点池和完整 STL 兼容 API 延后，避免一次引入过多资源语义。

## D12：跳表使用固定 seed 的可复现测试

随机层级会影响结构形状和性能，因此测试构造函数接受 seed。固定 seed 不改变 level-0 的排序语义，却能让差分测试和 Sanitizer 失败稳定重放。跳表 benchmark 暂不加入，避免把单次随机结构误读为普遍性能结论。

## D13：采用 LevelDB-style 跳表层级参数

benchmark 的 set-like 跳表采用 `MaxLevel=12` 和约 `1/4` 的逐层晋升概率，参考 LevelDB SkipList 和 Redis zskiplist 的典型概率设置。该决策只复用层数和概率，不声称复刻 LevelDB 或 Redis：当前节点仍使用默认 `new/delete`，提供逐节点 `erase`，没有 Arena、节点池、score/member 字典、span/rank 或 range API。

## D14：跳表优先优化 set-like 热路径

为了让当前 `ordered_benchmark` 中的 `skip_list<int>` 更接近甚至超过 `rb_tree<int>`，节点改为只分配实际高度的 forward link，并进一步移除 set-like 正向遍历不使用的 `backward`/`tail` 字段。随机层级生成改用 `mt19937` 原始输出的低两位做 `1/4` 晋升判断，避免分布对象开销；节点高度压缩为 `uint8_t`。这些优化不改变分配基线：每个真实节点仍是一次普通全局 `operator new` 和一次对应 `operator delete`。若后续实现 Redis-style reverse range、rank 或 span，需要作为新的能力单独 benchmark，因为它会重新增加节点元数据和更新成本。

## D15：新增独立的左倾红黑树

为了比较两种红黑树组织方式，新增 `llrb_tree<Key, Compare>`，不改写已有 parent-based `rb_tree`。LLRB 使用左倾红链接、递归 `balance` 和 Sedgewick 风格 top-down 删除；节点不保存 parent，迭代器从 root 搜索 successor/predecessor。该实现仍是单线程、唯一键、set-like，并保持每个真实节点一次普通 `new/delete`，明确不引入池化、Arena 或自定义 allocator。由于删除的双孩子路径复制 successor 的 key，当前 API 要求 `Key` 可赋值；这是后续支持不可赋值 key 时需要重新设计的边界。

LLRB 必须通过与现有 `rb_tree` 相同的固定 seed 差分测试、红黑不变量检查、ASan/UBSan 和 ordered benchmark 后才能评价性能。benchmark 结论只针对 workload：无 parent 迭代器的额外 root 搜索和 top-down 删除的颜色调整成本不能从复杂度记号中忽略。



## 维护约定

代码注释、架构图和验证文档属于同一份设计说明：新增 public API、改变哨兵/叶子表示、改变生命周期或修改 benchmark 计时边界时，必须同步更新三者。架构图只使用当前实现中存在的类、字段和函数名称。
