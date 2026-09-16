# 实现笔记

## 架构总览

完整的组件关系、节点布局和运行流程见 [`architecture.md`](architecture.md)。本文保留实现层面的细节，建议先阅读架构图，再按下面的生命周期顺序对照 header 中的注释。

第一版采用 `nullptr` 表示叶子，真实节点由 `rb_tree` 唯一拥有，并通过 `delete` 销毁。插入时先用 `std::unique_ptr` 暂时持有新节点：比较和分配完成后才把节点接入树，接入成功再释放临时所有权。析构和 `clear()` 递归销毁整棵树。

## 插入路径

插入先沿 BST 比较器搜索。找到等价 key 时立即返回 `false`，不会分配节点。新节点最初为红色；随后 `insert_fixup` 处理父节点为红色的情况：叔叔为红色时向上重新着色，叔叔为黑色时先处理内折，再处理外折并旋转。修复结束时根强制为黑色。

## 旋转

左旋把 pivot 的右孩子提升，右旋是完全镜像操作。两者都更新四类关系：子树中间链接、提升节点的父链接、原父节点指向以及 pivot 的新父链接。旋转不改变节点对象地址，因此插入修复不会使其他节点的迭代器因地址变化而失效。

## 删除路径

删除遵循三种结构：没有左孩子、没有右孩子、两个孩子。双孩子情况使用右子树最小后继，把后继移植到目标位置并继承目标颜色。保存“实际从树中移除的节点”的原颜色；只有移除黑节点时才执行 `erase_fixup`。

由于空叶子是 `nullptr`，修复函数使用 `(current, parent_of_current)` 而不是只使用 current。兄弟为空时视为黑色且两个孩子也为空，算法向父节点上移。修复结束后再次确保根为黑色且根的 parent 为空。

## `rb_map` 的性能实现

`rb_map<Key, T, Compare, Allocator>` 与教学版分开维护，避免为了 benchmark 破坏初版的可读性。它的真实节点继承一个只含链接和颜色的 `link`，再存放 `std::pair<const Key, T>`；共享的 `nil_` 只包含链接，不携带无效的 map value。

每棵 `rb_map` 都有一个黑色哨兵：空树时 `root_`、`leftmost_` 和 `rightmost_` 都指向它，真实节点的空孩子也指向它。这样删除修复可以直接读取 `current->parent`、兄弟和兄弟孩子，而不需要额外传递空节点的父节点。旋转时不能无条件改写共享哨兵的 `parent`；只有真实子节点的 parent 才能被更新，且删除修复结束后再次强制哨兵为黑色。

节点通过 `allocator_traits<Allocator>::rebind_alloc<node>` 分配和销毁。当前容器显式禁用拷贝和移动，先把资源语义控制在清晰范围内。`leftmost_`/`rightmost_` 缓存让 `begin()` 和 `--end()` 不需要从根重新搜索；插入、删除和清空必须同步维护这些缓存。

## 计时边界


## `llrb_tree` 的实现

`llrb_tree<Key, Compare>` 是与 `rb_tree` 并列的 set-like 实现，不改写 parent-based 的普通红黑树。节点仅保存 `key`、颜色和左右指针，不保存 parent；`insert_node` 在递归返回阶段执行 `balance`，通过左旋、右旋和颜色翻转维持左倾红链接。删除采用 top-down LLRB：`move_red_left`/`move_red_right` 在下降前把可借用的红链接移动到目标方向，`delete_min` 删除后继，回溯时再次 `balance`。

`erase` 先确认 key 存在，再把全黑根临时染红，删除结束后恢复根为黑色并递减 `size_`。删除分支必须使用比较器意义上的等价判断（两个方向都不小于），不能把 `!compare_(key, current->key)` 当成等价，因为它也匹配所有更大的 key。双孩子节点通过复制 successor 的 key 后删除右子树最小节点，因此当前 API 要求 `Key` 可赋值；这项取舍应在以后支持不可赋值 key 时改为物理节点移动。

LLRB 每个真实节点使用一次普通 `new`，删除使用一次 `delete`，没有池化、Arena 或自定义 allocator。没有 parent 也意味着迭代器每次求 successor/predecessor 都从 root 搜索，遍历计时不能与 parent-based `rb_tree` 混同解释。



`skip_list<Key, Compare, MaxLevel>` 是单线程、唯一键的 set-like 跳表。它用 `header_` 作为不存储 key 的多层入口，用 `level_` 记录当前最高有效层。真实节点采用变长布局：节点对象尾部只分配实际高度对应的 `forward[0..height)`，避免每个节点都携带完整 `MaxLevel` 指针数组。底层 `forward[0]` 是完整有序链表，也是迭代器唯一遍历的路径。

查找从 `level_ - 1` 开始：只要下一节点仍小于目标就向右，否则下降一层。插入在同一趟搜索中填充 `update[]`，其中每个元素是对应层最后一个小于目标的节点；重复 key 在分配和随机化后续链接前直接返回。新节点随机生成高度后，逐层执行 splice。删除复用 `update[]` 逐层绕过目标，删除后回收空的最高层，再释放目标节点。

随机高度当前采用 LevelDB/Redis-style 参数：`MaxLevel` 作为结构上限，每次以约 `1/4` 的概率晋升到下一层；实现直接使用 `mt19937` 输出的低两位做晋升判断，避免 `std::uniform_int_distribution` 在插入热路径上的额外开销。高度期望分布使查找、插入和删除为 `O(log n)`，但跳表没有红黑树那样的确定性高度上界，异常随机形状的最坏复杂度为 `O(n)`。固定 seed 只用于复现层级形状，不改变 level-0 的有序结果。`verify_invariants()` 检查每层有序、节点高度范围、无环以及 `size_` 计数。当前实现为了 set-like 正向遍历 benchmark 去掉了 Redis `backward/tail/span` 字段；如需 rank、range 或反向遍历，应单独加回并重新 benchmark。
