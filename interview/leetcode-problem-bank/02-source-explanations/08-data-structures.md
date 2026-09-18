# 常用数据结构

- 来源 URL：https://leetcode.cn/circle/discuss/mOr1u6/
- 来源标题：分享丨【算法题单】常用数据结构（前缀和/栈/队列/堆/字典树/并查集/树状数组/线段树）
- 抓取时间：2026-09-18 18:26:08 +0800
- 授权状态：authorized-by-user-confirmation
- 导入状态：preview

## 授权导入：专题/分类解析

> 本节按用户确认的授权导入前提保存来源专题中的分类说明、套路说明和学习建议。题目清单本身仍以 `01-question-bank/by-source-category/` 为准。

### 零、常用枚举技巧 / §0.1 枚举右，维护左

对于 **双变量问题**，例如两数之和 $a_i+a_j=t$，可以枚举右边的 $a_j$，转化成 **单变量问题**，也就是在 $a_j$ 左边查找是否有 $a_i = t-a_j$，这可以用哈希表维护。
我把这个技巧叫做 **枚举右，维护左**。
> 下面这些题目，如果可以，请用**一次遍历**实现。

### 零、常用枚举技巧 / §0.2 枚举中间

对于有三个或者四个变量的问题，枚举中间的变量往往更好算。
为什么？比如问题有三个下标，需要满足 $0\le i<j<k<n$，对比一下：
- 枚举 $i$，后续计算中还需保证 $j < k$。
- 枚举 $j$，那么 $i$ 和 $k$ 自动被 $j$ 隔开，互相独立，后续计算中无需关心 $i$ 和 $k$ 的位置关系。
所以枚举中间的变量更简单。

### 一、前缀和 / §1.1 基础

**左闭右开公式**：子数组 $[\textit{left},\textit{right})$ 的元素和为 $\textit{sum}[\textit{right}] - \textit{sum}[\textit{left}]$。把下标区间定义成左闭右开，就不需要加一减一了。

### 一、前缀和 / §1.2 前缀和与哈希表

通常要用到「枚举右，维护左」的技巧（见本题单 §0.1 节）。
**前缀和与有序集合**：

### 一、前缀和 / §1.4 状态压缩前缀和

推荐先阅读：[从集合论到位运算，常见位运算技巧分类总结！](https://leetcode.cn/circle/discuss/CaOJ45/)

### 一、前缀和 / §1.6 二维前缀和

[【图解】一张图秒懂二维前缀和](https://leetcode.cn/problems/range-sum-query-2d-immutable/solution/tu-jie-yi-zhang-tu-miao-dong-er-wei-qian-84qp/)
```py [sol-Python3]
class NumMatrix:
    def __init__(self, matrix: List[List[int]]):
        m, n = len(matrix), len(matrix[0])
        s = [[0] * (n + 1) for _ in range(m + 1)]
        for i, row in enumerate(matrix):
            for j, x in enumerate(row):
                s[i + 1][j + 1] = s[i + 1][j] + s[i][j + 1] - s[i][j] + x
        self.s = s
    # 返回左上角在 (r1, c1)，右下角在 (r2, c2) 的子矩阵元素和
    def sumRegion(self, r1: int, c1: int, r2: int, c2: int) -> int:

### 二、差分 / §2.1 一维差分

差分与前缀和的关系，类似**导数**与**积分**的关系。
数组 $a$ 的差分的前缀和就是数组 $a$（不变）。

### 二、差分 / §2.2 二维差分

[【图解】从一维差分到二维差分](https://leetcode.cn/problems/stamping-the-grid/solution/wu-nao-zuo-fa-er-wei-qian-zhui-he-er-wei-zwiu/)

### 三、栈 / §3.4 合法括号字符串（RBS）

注：部分题目可以不用栈，而是用一个数字记录嵌套深度。

### 三、栈 / §3.7 单调栈

见 [单调栈题单](https://leetcode.cn/circle/discuss/9oZFK9/)。

### 四、队列

队列常用在 BFS 中，见 [网格图题单](https://leetcode.cn/circle/discuss/YiXPXW/) 和 [图论题单](https://leetcode.cn/circle/discuss/01LUak/)。与此相比，栈常用在 DFS 中，但无需我们手动维护。

### 四、队列 / §4.4 单调队列

个人觉得叫**单调双端队列**更准确。
**单调队列 = 滑动窗口 + 单调栈**。必须先掌握滑动窗口和单调栈这两个知识点，再学单调队列。
**问**：入队、出队、更新答案，这三步的顺序如何思考？
**答**：有两种情况。如果更新答案时，用到的数据包含当前元素，那么就需要先入队，再更新答案；如果用到的数据不包含当前元素，那么就需要先更新答案，再入队。至于出队，一般写在前面，每遍历到一个新的元素，就看看队首元素是否失效（不满足要求），失效则弹出队首。
```py [sol-Python3]
# 计算 nums 的每个长为 k 的窗口的最大值
# 时间复杂度 O(n)，其中 n 是 nums 的长度
def maxSlidingWindow(nums: List[int], k: int) -> List[int]:
    ans = [0] * (len(nums) - k + 1)  # 窗口个数
    q = deque()  # 双端队列
    for i, x in enumerate(nums):
        # 1. 右边入

### 五、堆（优先队列） / §5.1 基础

[为什么堆化的时间复杂度是 O(n)？](https://leetcode.cn/problems/take-gifts-from-the-richest-pile/solutions/2501655/yuan-di-dui-hua-o1-kong-jian-fu-ti-dan-p-fzdh/)

### 五、堆（优先队列） / §5.3 第 K 小/大

部分题目也可以用二分解决。

### 五、堆（优先队列） / §5.5 反悔堆

基于堆的反悔贪心。

### 五、堆（优先队列） / §5.6 懒删除堆

支持删除堆中任意元素。
```py [sol-Python3]
# 模板来源 https://leetcode.cn/discuss/post/3583665/
class LazyHeap:
    def __init__(self):
        self.heap = []  # 最小堆（最大堆可以把数字取反或重载 __lt__）
        self.remove_cnt = defaultdict(int)  # 每个元素剩余需要删除的次数
        self.size = 0  # 堆的实际大小
    def remove(self, x: Any) -> None:
        self.remove_cnt[x] += 1  # 懒删除
        self.size -= 1
    # 正式执行删除操作

### 五、堆（优先队列） / §5.7 对顶堆（动态第 K 小/大）

部分题目需要结合懒删除堆。
做法不止一种，部分题目也可以用有序集合/值域树状数组等数据结构解决，见本题单「§8.1 树状数组」的模板二。
另见 [图论题单](https://leetcode.cn/circle/discuss/01LUak/) 中的 Dijkstra 算法。

### 六、字典树（trie） / §6.1 基础

[讲解（含模板代码）](https://leetcode.cn/problems/implement-trie-prefix-tree/solutions/2993894/cong-er-cha-shu-dao-er-shi-liu-cha-shu-p-xsj4/)

### 六、字典树（trie） / §6.4 0-1 字典树（异或字典树）

部分题目也可以用试填法解决。

### 七、并查集

```py [sol-Python3]
# 模板来源 https://leetcode.cn/discuss/post/3583665/
class UnionFind:
    def __init__(self, n: int):
        # 一开始有 n 个集合 {0}, {1}, ..., {n-1}
        # 集合 i 的代表元是自己，大小为 1
        self._fa = list(range(n))  # 代表元
        self._size = [1] * n  # 集合大小
        self.cc = n  # 连通块个数
    # 返回 x 所在集合的代表元
    # 同时做路径压缩，也就是把 x 所在集合中的所有元素的 fa 都改成代表元
    def find(self, x: int) -> int:

### 七、并查集 / §7.1 基础

更多基础题，见 [网格图题单](https://leetcode.cn/circle/discuss/YiXPXW/) 中的 DFS 和 [图论题单](https://leetcode.cn/circle/discuss/01LUak/) 中的 DFS，其中大部分题目也可以用并查集实现。

### 七、并查集 / §7.2 进阶

另见 [图论题单](https://leetcode.cn/circle/discuss/01LUak/) 中的最小生成树。

### 七、并查集 / §7.3 中介并查集

把 $n$ 个元素两两相连，要连 $\mathcal{O}(n^2)$ 条边，太慢了。而把元素连到中介上，通常只会连 $\mathcal{O}(n)$ 条边。

### 七、并查集 / §7.6 带权并查集（边权并查集）

```py [sol-Python3]
# 模板来源 https://leetcode.cn/discuss/post/3583665/
class UnionFind:
    def __init__(self, n: int):
        # 一开始有 n 个集合 {0}, {1}, ..., {n-1}
        # 集合 i 的代表元是自己，自己到自己的距离是 0
        self.fa = list(range(n))  # 代表元
        self.dis = [0] * n  # dis[x] 表示 x 到（x 所在集合的）代表元的距离
    # 返回 x 所在集合的代表元
    # 同时做路径压缩
    def find(self, x: int) -> int:
        fa = self.fa

### 八、树状数组和线段树

能用树状数组解决的题目，也能用线段树解决（反过来不一定）。但树状数组实现简单，代码短。
为方便大家练习，我把适合用树状数组解决的题目分到树状数组中，其余分到线段树中。
部分题目需要**离散化**，推荐先完成 [1331. 数组序号转换](https://leetcode.cn/problems/rank-transform-of-an-array/)。

### 八、树状数组和线段树 / §8.1 树状数组

讲解：[带你发明树状数组！附数学证明](https://leetcode.cn/problems/range-sum-query-mutable/solution/dai-ni-fa-ming-shu-zhuang-shu-zu-fu-shu-lyfll/)
**模板一：普通树状数组**
```py [sol-Python3]
# 模板来源 https://leetcode.cn/discuss/post/3583665/
class FenwickTree:
    def __init__(self, n: int):
        self.tree = [0] * (n + 1)  # 使用下标 1 到 n
    # a[i] 增加 val
    # 1 <= i <= n
    # 时间复杂度 O(log n)
    def update(self, i: int, val: int) -> None:
        t = self.tree

### 八、树状数组和线段树 / §8.2 逆序对

除了可以用树状数组解决，部分题目也可以在**归并排序**的同时计算。

### 八、树状数组和线段树 / §8.3 线段树（无区间更新）

线段树本质是二叉树，在学习之前，建议先做做 [104. 二叉树的最大深度](https://leetcode.cn/problems/maximum-depth-of-binary-tree/) 和 [111. 二叉树的最小深度](https://leetcode.cn/problems/minimum-depth-of-binary-tree/)（自底向上写法），当作热身。
[线段树：为什么要这样设计？](https://www.bilibili.com/video/BV15gRaYZE5o/) 理解线段树发明的动机。
把任意区间用 $\mathcal{O}(\log n)$ 个区间表示，线段树的每个节点记录对应区间的信息。
- **询问**：把询问区间拆分成 $\mathcal{O}(\log n)$ 个区间，对应着线段树的 $\mathcal{O}(\log n)$ 个节点，把这 $\mathcal{O}(\log n)$ 个节点的信息合并，即为答案。
- **单点更新**：有 $\mathcal{O}(\log n)$ 个区间包含被修改的位置，需要更新 $\mathcal{O}(\log n)$ 个节点的信息。
基础模板代码如下。为方便入门理解，我没有做复杂封装。更通用的写法可以参考 AtCoder Library 的 [segtree.hpp](https://github.com/atcoder/ac-library/blob/master/atcoder/segtree.hpp)。
```py [sol-Python3]
# 模板来源 https://leetcode.cn/discuss/post/3583665/
# 线段树有两个下标，一个是线段树节点的下标，另一个是线段树维护的区间的下标
# 节点的下标：从 1 开始，如果你想改成从 0 开始，需要把左右儿子下标分别改成 node*2+1 和 node*2+2
# 区间的下标：从 0 开始
class SegmentTree:

### 八、树状数组和线段树 / §8.4 Lazy 线段树（有区间更新）

把任意区间用 $\mathcal{O}(\log n)$ 个区间表示，线段树的每个节点记录对应区间的信息。
- **询问**：把询问区间拆分成 $\mathcal{O}(\log n)$ 个区间，对应着线段树的 $\mathcal{O}(\log n)$ 个节点，把这 $\mathcal{O}(\log n)$ 个节点的信息合并，即为答案。
- **区间更新**：仍然是拆分成 $\mathcal{O}(\log n)$ 个区间，对应着线段树的 $\mathcal{O}(\log n)$ 个节点。但对于其中的非叶节点，不把更新的内容往下传递给子节点，而是记录「发生了更新，内容为 xxx」，把更新的内容记录下来。直到后续的询问或更新操作，需要访问或修改更下面的子节点信息时，才把更新的内容往下传。
基础模板代码如下。为方便入门理解，我没有做复杂封装。更通用的写法可以参考 AtCoder Library 的 [lazysegtree.hpp](https://github.com/atcoder/ac-library/blob/master/atcoder/lazysegtree.hpp)。
```py [sol-Python3]
# 模板来源 https://leetcode.cn/discuss/post/3583665/
class Node:
    __slots__ = 'val', 'todo'
class LazySegmentTree:
    # 懒标记初始值
    _TODO_INIT = 0  # **根据题目修改**
    def __init__(self, arr, default=0):

### 八、树状数组和线段树 / §8.5 动态开点线段树

部分题目也可以用**珂朵莉树**解决。

### 八、树状数组和线段树 / §8.7 ST 表（Sparse Table）

[ST 表](https://oi-wiki.org/ds/sparse-table/) 支持区间最值查询（Range Minimum/Maximum Query，RMQ），但不支持修改。
优点是代码短，且查询的时间复杂度是 $\mathcal{O}(1)$。所以作为补充内容，附在此处。
```py [sol-Python3]
class SparseTable:
    # 时间复杂度 O(n * log n)
    def __init__(self, nums: List[int], op: Callable[[int, int], int]):
        n = len(nums)
        w = n.bit_length()
        st = [[0] * n for _ in range(w)]
        st[0] = nums[:]
        for i in range(1, w):
            for j in range(n - (1 << i) + 1):

### 十、根号算法 / §10.1 分块

**注**：上面树状数组和线段树的题目，可以尝试用**分块**解决。例如 [307. 区域和检索 - 数组可修改](https://leetcode.cn/problems/range-sum-query-mutable/)。

### 十、根号算法 / §10.2 莫队算法

属于离线算法的一种。

### 十、根号算法 / §10.3 根号分解（Sqrt Decomposition）

针对不同大小的询问使用不同的算法。

### 专题：离线算法

通过改变回答询问的顺序，使问题更容易处理。
> 相应的，**在线算法**就是按照 $\textit{queries}$ 的顺序一个一个处理。

### 编程能力强化训练

知道大概怎么做，但不好实现的题目。部分题目是模拟题。

### 编程能力强化训练 / Part A

另见本题单的「§3.5 表达式解析」。

### 关联题单

- 关于**链表**和**二叉树**的题目，见 [链表、树与回溯](https://leetcode.cn/circle/discuss/K0n2gO/)。
- 关于**字符串**算法，见 [字符串题单](https://leetcode.cn/circle/discuss/SJFwQI/)。

### 算法题单

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)
欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)
如果你发现有题目可以补充进来，欢迎评论反馈。

## 授权题解链接索引

| 分类路径 | 标题 | 链接 | 导入状态 |
|---|---|---|---|
| 二、差分 / §2.1 一维差分 / §2.1.2 进阶 | 我的评论 | https://leetcode.cn/problems/merge-intervals/solutions/2798138/jian-dan-zuo-fa-yi-ji-wei-shi-yao-yao-zh-f2b3/comments/2323402/ | pending-fetch |
| 五、堆（优先队列） / §5.1 基础 | 为什么堆化的时间复杂度是 O(n)？ | https://leetcode.cn/problems/take-gifts-from-the-richest-pile/solutions/2501655/yuan-di-dui-hua-o1-kong-jian-fu-ti-dan-p-fzdh/ | pending-fetch |
| 五、堆（优先队列） / §5.7 对顶堆（动态第 K 小/大） | 讲解 | https://leetcode.cn/problems/find-median-from-data-stream/solutions/3015873/ru-he-zi-ran-yin-ru-da-xiao-dui-jian-ji-4v22k/ | pending-fetch |
| 六、字典树（trie） / §6.1 基础 | 讲解（含模板代码） | https://leetcode.cn/problems/implement-trie-prefix-tree/solutions/2993894/cong-er-cha-shu-dao-er-shi-liu-cha-shu-p-xsj4/ | pending-fetch |

## 本地原创增强解析

常用数据结构 应优先沿用来源页的标题层级学习：先做低难度分题目，用同一套路反复观察边界条件；再回到进阶题处理多约束、多状态或跨专题组合。

### 分类层级说明

本专题的本地分类路径完全来自来源页面的 Markdown 标题层级，后续单题详解会回链到这些路径。

### 建议刷题节奏

先完成每个小节前 3–5 题，确认模板和边界；再按难度分上升补齐。遇到看完题解仍无法复述关键观察的题，先标记复盘，不阻塞下一小节。
