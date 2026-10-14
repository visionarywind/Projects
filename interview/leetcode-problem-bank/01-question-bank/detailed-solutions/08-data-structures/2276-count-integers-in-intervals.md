# 2276. 统计区间中的整数数目

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/count-integers-in-intervals/
- 题目 slug：`count-integers-in-intervals`
- 来源专题：常用数据结构
- 来源分类路径：八、树状数组和线段树 / §8.5 动态开点线段树
- 难度分：2222
- 外部题解来源：https://leetcode.cn/problems/count-integers-in-intervals/solutions/1495396/by-endlesscheng-clk2/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种做法：珂朵莉树 / 动态开点线段树（Python/Java/C++/Go）](https://leetcode.cn/problems/count-integers-in-intervals/solutions/1495396/by-endlesscheng-clk2/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`by-endlesscheng-clk2`
- topic id：`1495396`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

思路同 [715. Range 模块](https://leetcode.cn/problems/range-module/)

## 方法一：珂朵莉树

用一棵平衡树维护**不相交**的区间，每次 `add(left,right)` 时，删除被该区间覆盖到的区间（部分覆盖也算），然后将被删除的区间与 $[\textit{left},\textit{right}]$ 合并成一个新的大区间（并集），插入平衡树中。

代码实现时，为方便找到第一个被 $[\textit{left},\textit{right}]$ 覆盖到的区间，我们可以用平衡树的 $\textit{key}$ 存区间右端点，$\textit{value}$ 存区间左端点。我们要找的就是第一个 $\textit{key}\ge\textit{left}$ 的区间。

```py [sol-Python3]
from sortedcontainers import SortedDict

class CountIntervals:
    def __init__(self):
        self.d = SortedDict()
        self.cnt = 0  # 所有区间长度和

    def add(self, left: int, right: int) -> None:
        # 遍历所有被 [left,right] 覆盖到的区间（部分覆盖也算）
        i = self.d.bisect_left(left)
        while i < len(self.d) and self.d.values()[i] <= right:
            r, l = self.d.items()[i]
            left = min(left, l)    # 合并后的新区间，其左端点为所有被覆盖的区间的左端点的最小值
            right = max(right, r)  # 合并后的新区间，其右端点为所有被覆盖的区间的右端点的最大值
            self.cnt -= r - l + 1
            self.d.popitem(i)
        self.cnt += right - left + 1
        self.d[right] = left  # 所有被覆盖到的区间与 [left,right] 合并成一个新区间

    def count(self) -> int:
        return self.cnt
```

```java [sol-Java]
class CountIntervals {
    TreeMap<Integer, Integer> m = new TreeMap<>();
    int cnt; // 所有区间长度和

    public CountIntervals() {}

    public void add(int left, int right) {
        // 遍历所有被 [left,right] 覆盖到的区间（部分覆盖也算）
        for (var e = m.ceilingEntry(left); e != null && e.getValue() <= right; e = m.ceilingEntry(left)) {
            int l = e.getValue(), r = e.getKey();
            left = Math.min(left, l);   // 合并后的新区间，其左端点为所有被覆盖的区间的左端点的最小值
            right = Math.max(right, r); // 合并后的新区间，其右端点为所有被覆盖的区间的右端点的最大值
            cnt -= r - l + 1;
            m.remove(r);
        }
        cnt += right - left + 1;
        m.put(right, left); // 所有被覆盖到的区间与 [left,right] 合并成一个新区间
    }

    public int count() { return cnt; }
}
```

```cpp [sol-C++]
class CountIntervals {
    map<int, int> m;
    int cnt = 0; // 所有区间长度和

public:
    CountIntervals() {}

    void add(int left, int right) {
        // 遍历所有被 [left,right] 覆盖到的区间（部分覆盖也算）
        for (auto it = m.lower_bound(left); it != m.end() && it->second <= right; m.erase(it++)) {
            int l = it->second, r = it->first;
            left = min(left, l);   // 合并后的新区间，其左端点为所有被覆盖的区间的左端点的最小值
            right = max(right, r); // 合并后的新区间，其右端点为所有被覆盖的区间的右端点的最大值
            cnt -= r - l + 1;
        }
        cnt += right - left + 1;
        m[right] = left; // 所有被覆盖到的区间与 [left,right] 合并成一个新区间
    }

    int count() { return cnt; }
};
```

```go [sol-Go]
type CountIntervals struct {
	*redblacktree.Tree
	cnt int // 所有区间长度和
}

func Constructor() CountIntervals {
	return CountIntervals{redblacktree.NewWithIntComparator(), 0}
}

func (t *CountIntervals) Add(left, right int) {
	// 遍历所有被 [left,right] 覆盖到的区间（部分覆盖也算）
	for node, _ := t.Ceiling(left); node != nil && node.Value.(int) <= right; node, _ = t.Ceiling(left) {
		l, r := node.Value.(int), node.Key.(int)
		if l < left { left = l }   // 合并后的新区间，其左端点为所有被覆盖的区间的左端点的最小值
		if r > right { right = r } // 合并后的新区间，其右端点为所有被覆盖的区间的右端点的最大值
		t.cnt -= r - l + 1
		t.Remove(r)
	}
	t.cnt += right - left + 1
	t.Put(right, left) // 所有被覆盖到的区间与 [left,right] 合并成一个新区间
}

func (t *CountIntervals) Count() int { return t.cnt }
```

#### 复杂度分析

- 时间复杂度：每个区间至多被添加删除各一次，因此 `add` 操作是均摊 $O(\log n)$ 的，这里 $n$ 是 `add` 的次数。
- 空间复杂度：$O(n)$。

## 方法二：动态开点线段树

前置知识：线段树、动态开点线段树

完整的动态开点线段树模板见我的 [算法竞赛模板库](https://github.com/EndlessCheng/codeforces-go/blob/master/copypasta/segment_tree.go)。

对于本题来说，线段树的每个节点可以保存对应范围的左右端点 $l$ 和 $r$，以及范围内 `add` 过的整数个数 $\textit{cnt}$。

代码实现时，无需记录 lazy tag，这是因为被覆盖的范围无需再次覆盖，因此若 $\textit{cnt}$ 等于范围的长度 $r-l+1$，则可直接返回。

```py [sol-Python3]
class CountIntervals:
    __slots__ = 'left', 'right', 'l', 'r', 'cnt'

    def __init__(self, l=1, r=10 ** 9):
        self.left = self.right = None
        self.l, self.r, self.cnt = l, r, 0

    def add(self, l: int, r: int) -> None:
        if self.cnt == self.r - self.l + 1: return  # self 已被完整覆盖，无需执行任何操作
        if l <= self.l and self.r <= r:  # self 已被区间 [l,r] 完整覆盖，不再继续递归
            self.cnt = self.r - self.l + 1
            return
        mid = (self.l + self.r) // 2
        if self.left is None: self.left = CountIntervals(self.l, mid)  # 动态开点
        if self.right is None: self.right = CountIntervals(mid + 1, self.r)  # 动态开点
        if l <= mid: self.left.add(l, r)
        if mid < r: self.right.add(l, r)
        self.cnt = self.left.cnt + self.right.cnt

    def count(self) -> int:
        return self.cnt
```

```java [sol-Java]
class CountIntervals {
    CountIntervals left, right;
    int l, r, cnt;

    public CountIntervals() {
        l = 1;
        r = (int) 1e9;
    }

    CountIntervals(int l, int r) {
        this.l = l;
        this.r = r;
    }

    public void add(int L, int R) { // 为方便区分变量名，将递归中始终不变的入参改为大写（视作常量）
        if (cnt == r - l + 1) return; // 当前节点已被完整覆盖，无需执行任何操作
        if (L <= l && r <= R) { // 当前节点已被区间 [L,R] 完整覆盖，不再继续递归
            cnt = r - l + 1;
            return;
        }
        int mid = (l + r) / 2;
        if (left == null) left = new CountIntervals(l, mid); // 动态开点
        if (right == null) right = new CountIntervals(mid + 1, r); // 动态开点
        if (L <= mid) left.add(L, R);
        if (mid < R) right.add(L, R);
        cnt = left.cnt + right.cnt;
    }

    public int count() {
        return cnt;
    }
}
```

```cpp [sol-C++]
class CountIntervals {
    CountIntervals *left = nullptr, *right = nullptr;
    int l, r, cnt = 0;

public:
    CountIntervals() : l(1), r(1e9) {}

    CountIntervals(int l, int r) : l(l), r(r) {}

    void add(int L, int R) { // 为方便区分变量名，将递归中始终不变的入参改为大写（视作常量）
        if (cnt == r - l + 1) return; // 当前节点已被完整覆盖，无需执行任何操作
        if (L <= l && r <= R) { // 当前节点已被区间 [L,R] 完整覆盖，不再继续递归
            cnt = r - l + 1;
            return;
        }
        int mid = (l + r) / 2;
        if (left == nullptr) left = new CountIntervals(l, mid); // 动态开点
        if (right == nullptr) right = new CountIntervals(mid + 1, r); // 动态开点
        if (L <= mid) left->add(L, R);
        if (mid < R) right->add(L, R);
        cnt = left->cnt + right->cnt;
    }

    int count() { return cnt; }
};
```

```go [sol-Go]
type CountIntervals struct {
	left, right *CountIntervals
	l, r, cnt   int
}

func Constructor() CountIntervals { return CountIntervals{l: 1, r: 1e9} }

func (o *CountIntervals) Add(l, r int) {
	if o.cnt == o.r-o.l+1 { return } // o 已被完整覆盖，无需执行任何操作
	if l <= o.l && o.r <= r { // 当前节点已被区间 [l,r] 完整覆盖，不再继续递归
		o.cnt = o.r - o.l + 1
		return
	}
	mid := (o.l + o.r) >> 1
	if o.left == nil { o.left = &CountIntervals{l: o.l, r: mid} } // 动态开点
	if o.right == nil { o.right = &CountIntervals{l: mid + 1, r: o.r} } // 动态开点
	if l <= mid { o.left.Add(l, r)}
	if mid < r { o.right.Add(l, r) }
	o.cnt = o.left.cnt + o.right.cnt
}

func (o *CountIntervals) Count() int { return o.cnt }
```

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/基环树/最短路/最小生成树/网络流）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/划分/状态机/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)
