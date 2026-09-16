# 3525. 求出数组的 X 值 II

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/find-x-value-of-array-ii/
- 题目 slug：`find-x-value-of-array-ii`
- 来源专题：常用数据结构
- 来源分类路径：八、树状数组和线段树 / §8.3 线段树（无区间更新）
- 难度分：2645
- 外部题解来源：https://leetcode.cn/problems/find-x-value-of-array-ii/solutions/3656583/dan-dian-xiu-gai-xian-duan-shu-by-endles-i3jy/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[从分治到线段树（Python/Java/C++/Go）](https://leetcode.cn/problems/find-x-value-of-array-ii/solutions/3656583/dan-dian-xiu-gai-xian-duan-shu-by-endles-i3jy/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`dan-dian-xiu-gai-xian-duan-shu-by-endles-i3jy`
- topic id：`3656583`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

请先完成上一题 [3524. 求出数组的 X 值 I](https://leetcode.cn/problems/find-x-value-of-array-i/)。

注意本题的子数组和上一题不一样，本题只能移除后缀，所以（每个询问的）子数组的左端点是**固定**的，即 $\textit{start}$。

本题有修改操作，如何处理？

**套路**：如果一个问题可以用分治解决，那么其带修版本可以用线段树解决。

考虑用分治解决如下问题：

- 在左端点为 $l$，右端点为 $l,l+1,l+2,\ldots,r$ 的子数组中，元素积模 $k$ 等于 $x$ 的子数组有多少个？

**思路**：

- 把子数组 $[l,r]$ 一分为二，左半下标范围为 $[l,m]$，右半下标范围为 $[m+1,r]$。其中 $m=\left\lfloor\dfrac{l+r}{2}\right\rfloor$。
- 递归左半，计算在左端点为 $l$，右端点为 $l,l+1,l+2,\ldots,m$ 的子数组中，元素积模 $k$ 等于 $x$ 的子数组有多少个。
- 递归右半，计算在左端点为 $m+1$，右端点为 $m+1,m+2,\ldots,r$ 的子数组中，元素积模 $k$ 等于 $x$ 的子数组有多少个。
- 设整个左半的元素积模 $k$ 为 $\textit{leftMul}$。把右半子数组的元素积乘以 $\textit{leftMul}$，就得到了左端点为 $l$，右端点为 $m+1,m+2,\ldots,r$ 的子数组的元素积，从而统计个数。假设右半有 $c=\textit{cnt}[\textit{rx}]$ 个乘积模 $k$ 等于 $\textit{rx}$ 的子数组，那么在左端点为 $l$，右端点为 $m+1,m+2,\ldots,r$ 的子数组中，有 $c$ 个元素积模 $k$ 等于 $\textit{leftMul}\cdot \textit{rx}\bmod k$ 的子数组。
- 二者相加，就得到了左端点为 $l$、右端点为 $l,l+1,l+2,\ldots,r$ 的、元素积模 $k$ 等于 $x$ 的子数组的个数。

这样就可以用线段树了。线段树的每个节点维护一段子数组（区间），其中维护区间 $[l,r]$ 的节点保存：

- $\textit{mul}$：整个区间 $[l,r]$ 的元素积模 $k$ 的结果。这可以用左右子树的 $\textit{mul}$ 相乘模 $k$ 得到。
- $\textit{cnt}$ 数组：其中 $\textit{cnt}[x]$ 表示在左端点为 $l$，右端点为 $l,l+1,l+2,\ldots,r$ 的子数组中，元素积模 $k$ 等于 $x$ 的子数组的个数。计算规则就是上文的分治。

对于线段树的叶子（递归边界），维护区间 $[i,i]$ 的节点保存：

- $\textit{mul}=\textit{nums}[i]\bmod k$。
- $\textit{cnt}[\textit{mul}]=1$，其余 $\textit{cnt}[x]=0$。

完整的线段树模板，请看我的 [数据结构题单](https://leetcode.cn/discuss/post/3583665/fen-xiang-gun-ti-dan-chang-yong-shu-ju-j-bvmv/)。

[本题视频讲解](https://www.bilibili.com/video/BV1NALczNERr/?t=37m50s)，欢迎点赞关注~

```py [sol-Python3]
# 线段树有两个下标，一个是线段树节点的下标，另一个是线段树维护的区间的下标
# 节点的下标：从 1 开始，如果你想改成从 0 开始，需要把左右儿子下标分别改成 node*2+1 和 node*2+2
# 区间的下标：从 0 开始
class SegmentTree:
    def __init__(self, a: List[int], k: int):
        self._n = n = len(a)
        self._k = k
        self._tree = [None] * (2 << (n - 1).bit_length())
        self._build(a, 1, 0, n - 1)

    # 合并信息
    def _merge_data(self, a: Tuple[int, List[int]], b: Tuple[int, List[int]]) -> Tuple[int, List[int]]:
        cnt = a[1].copy()
        left_mul = a[0]
        for rx, c in enumerate(b[1]):
            cnt[left_mul * rx % self._k] += c
        return left_mul * b[0] % self._k, cnt

    def _new_data(self, val: int) -> Tuple[int, List[int]]:
        mul = val % self._k
        cnt = [0] * self._k
        cnt[mul] = 1
        return mul, cnt

    # 合并左右儿子的信息到当前节点
    def _maintain(self, node: int) -> None:
        self._tree[node] = self._merge_data(self._tree[node * 2], self._tree[node * 2 + 1])

    # 用 a 初始化线段树
    # 时间复杂度 O(n)
    def _build(self, a: List[int], node: int, l: int, r: int) -> None:
        if l == r:  # 叶子
            self._tree[node] = self._new_data(a[l])  # 初始化叶节点的值
            return
        m = (l + r) // 2
        self._build(a, node * 2, l, m)  # 初始化左子树
        self._build(a, node * 2 + 1, m + 1, r)  # 初始化右子树
        self._maintain(node)

    def _update(self, node: int, l: int, r: int, i: int, val: int) -> None:
        if l == r:  # 叶子（到达目标）
            self._tree[node] = self._new_data(val)
            return
        m = (l + r) // 2
        if i <= m:  # i 在左子树
            self._update(node * 2, l, m, i, val)
        else:  # i 在右子树
            self._update(node * 2 + 1, m + 1, r, i, val)
        self._maintain(node)

    def _query(self, node: int, l: int, r: int, ql: int, qr: int) -> Tuple[int, List[int]]:
        if ql <= l and r <= qr:  # 当前子树完全在 [ql, qr] 内
            return self._tree[node]
        m = (l + r) // 2
        if qr <= m:  # [ql, qr] 在左子树
            return self._query(node * 2, l, m, ql, qr)
        if ql > m:  # [ql, qr] 在右子树
            return self._query(node * 2 + 1, m + 1, r, ql, qr)
        l_res = self._query(node * 2, l, m, ql, qr)
        r_res = self._query(node * 2 + 1, m + 1, r, ql, qr)
        return self._merge_data(l_res, r_res)

    # 更新 a[i] 为 _new_data(val)
    # 时间复杂度 O(log n)
    def update(self, i: int, val: int) -> None:
        self._update(1, 0, self._n - 1, i, val)

    # 返回用 _merge_data 合并所有 a[i] 的计算结果，其中 i 在闭区间 [ql, qr] 中
    # 时间复杂度 O(log n)
    def query(self, ql: int, qr: int) -> Tuple[int, List[int]]:
        return self._query(1, 0, self._n - 1, ql, qr)

class Solution:
    def resultArray(self, nums: List[int], k: int, queries: List[List[int]]) -> List[int]:
        t = SegmentTree(nums, k)
        n = len(nums)
        ans = []
        for index, value, start, x in queries:
            t.update(index, value)
            _, cnt = t.query(start, n - 1)
            ans.append(cnt[x])
        return ans
```

```java [sol-Java]
// 线段树有两个下标，一个是线段树节点的下标，另一个是线段树维护的区间的下标
// 节点的下标：从 1 开始，如果你想改成从 0 开始，需要把左右儿子下标分别改成 node*2+1 和 node*2+2
// 区间的下标：从 0 开始
class SegmentTree {
    private record Data(int mul, int[] cnt) {
    }

    private final int k;
    private final int n;
    private final Data[] tree;

    // 合并两个 Data
    private Data mergeData(Data a, Data b) {
        int[] cnt = a.cnt.clone();
        for (int rx = 0; rx < k; rx++) {
            cnt[a.mul * rx % k] += b.cnt[rx];
        }
        return new Data(a.mul * b.mul % k, cnt);
    }

    private Data newData(int val) {
        int mul = val % k;
        int[] cnt = new int[k];
        cnt[mul] = 1;
        return new Data(mul, cnt);
    }

    // 线段树维护数组 a
    public SegmentTree(int[] a, int k) {
        this.k = k;
        n = a.length;
        tree = new Data[2 << (32 - Integer.numberOfLeadingZeros(n - 1))];
        build(a, 1, 0, n - 1);
    }

    // 更新 a[i] 为 newData(val)
    // 时间复杂度 O(log n)
    public void update(int i, int val) {
        update(1, 0, n - 1, i, val);
    }

    // 返回用 mergeData 合并所有 a[i] 的计算结果，其中 i 在闭区间 [ql, qr] 中
    // 时间复杂度 O(log n)
    public int query(int ql, int qr, int x) {
        return query(1, 0, n - 1, ql, qr).cnt[x];
    }

    // 合并左右儿子的 Data 到当前节点
    private void maintain(int node) {
        tree[node] = mergeData(tree[node * 2], tree[node * 2 + 1]);
    }

    // 用 a 初始化线段树
    // 时间复杂度 O(n)
    private void build(int[] a, int node, int l, int r) {
        if (l == r) { // 叶子
            tree[node] = newData(a[l]); // 初始化叶节点的值
            return;
        }
        int m = (l + r) / 2;
        build(a, node * 2, l, m); // 初始化左子树
        build(a, node * 2 + 1, m + 1, r); // 初始化右子树
        maintain(node);
    }

    private void update(int node, int l, int r, int i, int val) {
        if (l == r) { // 叶子（到达目标）
            tree[node] = newData(val);
            return;
        }
        int m = (l + r) / 2;
        if (i <= m) { // i 在左子树
            update(node * 2, l, m, i, val);
        } else { // i 在右子树
            update(node * 2 + 1, m + 1, r, i, val);
        }
        maintain(node);
    }

    private Data query(int node, int l, int r, int ql, int qr) {
        if (ql <= l && r <= qr) { // 当前子树完全在 [ql, qr] 内
            return tree[node];
        }
        int m = (l + r) / 2;
        if (qr <= m) { // [ql, qr] 在左子树
            return query(node * 2, l, m, ql, qr);
        }
        if (ql > m) { // [ql, qr] 在右子树
            return query(node * 2 + 1, m + 1, r, ql, qr);
        }
        Data lRes = query(node * 2, l, m, ql, qr);
        Data rRes = query(node * 2 + 1, m + 1, r, ql, qr);
        return mergeData(lRes, rRes);
    }
}

class Solution {
    public int[] resultArray(int[] nums, int k, int[][] queries) {
        SegmentTree t = new SegmentTree(nums, k);
        int n = nums.length;
        int[] ans = new int[queries.length];
        for (int qi = 0; qi < queries.length; qi++) {
            int[] q = queries[qi];
            t.update(q[0], q[1]);
            ans[qi] = t.query(q[2], n - 1, q[3]);
        }
        return ans;
    }
}
```

```cpp [sol-C++]
// 线段树有两个下标，一个是线段树节点的下标，另一个是线段树维护的区间的下标
// 节点的下标：从 1 开始，如果你想改成从 0 开始，需要把左右儿子下标分别改成 node*2+1 和 node*2+2
// 区间的下标：从 0 开始
class SegmentTree {
    using T = pair<int, array<int, 5>>; // 数组比 vector 快！

    int k;
    int n;
    vector<T> tree;

    // 合并两个 T
    T merge_val(T a, T b) const {
        auto [left_mul, cnt] = a;
        for (int rx = 0; rx < k; rx++) {
            cnt[left_mul * rx % k] += b.second[rx];
        }
        return {left_mul * b.first % k, cnt};
    }

    T new_val(int val) const {
        int mul = val % k;
        array<int, 5> cnt{};
        cnt[mul] = 1;
        return {mul, cnt};
    }

    // 合并左右儿子的 T 到当前节点的 T
    void maintain(int node) {
        tree[node] = merge_val(tree[node * 2], tree[node * 2 + 1]);
    }

    // 用 a 初始化线段树
    // 时间复杂度 O(n)
    void build(const vector<int>& a, int node, int l, int r) {
        if (l == r) { // 叶子
            tree[node] = new_val(a[l]); // 初始化叶节点的值
            return;
        }
        int m = (l + r) / 2;
        build(a, node * 2, l, m); // 初始化左子树
        build(a, node * 2 + 1, m + 1, r); // 初始化右子树
        maintain(node);
    }

    void update(int node, int l, int r, int i, int val) {
        if (l == r) { // 叶子（到达目标）
            tree[node] = new_val(val);
            return;
        }
        int m = (l + r) / 2;
        if (i <= m) { // i 在左子树
            update(node * 2, l, m, i, val);
        } else { // i 在右子树
            update(node * 2 + 1, m + 1, r, i, val);
        }
        maintain(node);
    }

    T query(int node, int l, int r, int ql, int qr) const {
        if (ql <= l && r <= qr) { // 当前子树完全在 [ql, qr] 内
            return tree[node];
        }
        int m = (l + r) / 2;
        if (qr <= m) { // [ql, qr] 在左子树
            return query(node * 2, l, m, ql, qr);
        }
        if (ql > m) { // [ql, qr] 在右子树
            return query(node * 2 + 1, m + 1, r, ql, qr);
        }
        T l_res = query(node * 2, l, m, ql, qr);
        T r_res = query(node * 2 + 1, m + 1, r, ql, qr);
        return merge_val(l_res, r_res);
    }

public:
    // 线段树维护数组 a
    SegmentTree(const vector<int>& a, int k) : k(k), n(a.size()), tree(2 << bit_width(a.size() - 1)) {
        build(a, 1, 0, n - 1);
    }

    // 更新 a[i] 为 new_val(val)
    // 时间复杂度 O(log n)
    void update(int i, int val) {
        update(1, 0, n - 1, i, val);
    }

    // 返回用 merge_val 合并所有 a[i] 的计算结果，其中 i 在闭区间 [ql, qr] 中
    // 时间复杂度 O(log n)
    T query(int ql, int qr) const {
        return query(1, 0, n - 1, ql, qr);
    }
};

class Solution {
public:
    vector<int> resultArray(vector<int>& nums, int k, vector<vector<int>>& queries) {
        SegmentTree t(nums, k);
        int n = nums.size();
        vector<int> ans;
        for (auto& q : queries) {
            t.update(q[0], q[1]);
            auto [_, cnt] = t.query(q[2], n - 1);
            ans.push_back(cnt[q[3]]);
        }
        return ans;
    }
};
```

```go [sol-Go]
// 完整线段树模板见数据结构题单
var k int

type data struct {
	mul int
	cnt [5]int // 比 []int 快
}

type seg []data

func mergeData(a, b data) data {
	cnt := a.cnt
	for rx, c := range b.cnt {
		cnt[a.mul*rx%k] += c
	}
	return data{a.mul * b.mul % k, cnt}
}

func newData(val int) data {
	mul := val % k
	cnt := [5]int{}
	cnt[mul] = 1
	return data{mul, cnt}
}

func (t seg) maintain(o int) {
	t[o] = mergeData(t[o<<1], t[o<<1|1])
}

func (t seg) build(a []int, o, l, r int) {
	if l == r {
		t[o] = newData(a[l])
		return
	}
	m := (l + r) >> 1
	t.build(a, o<<1, l, m)
	t.build(a, o<<1|1, m+1, r)
	t.maintain(o)
}

func (t seg) update(o, l, r, i, val int) {
	if l == r {
		t[o] = newData(val)
		return
	}
	m := (l + r) >> 1
	if i <= m {
		t.update(o<<1, l, m, i, val)
	} else {
		t.update(o<<1|1, m+1, r, i, val)
	}
	t.maintain(o)
}

func (t seg) query(o, l, r, ql, qr int) data {
	if ql <= l && r <= qr {
		return t[o]
	}
	m := (l + r) / 2
	if qr <= m {
		return t.query(o*2, l, m, ql, qr)
	}
	if ql > m {
		return t.query(o*2+1, m+1, r, ql, qr)
	}
	lRes := t.query(o*2, l, m, ql, qr)
	rRes := t.query(o*2+1, m+1, r, ql, qr)
	return mergeData(lRes, rRes)
}

func newSegmentTreeWithArray(a []int) seg {
	n := len(a)
	t := make(seg, 2<<bits.Len(uint(n-1)))
	t.build(a, 1, 0, n-1)
	return t
}

func resultArray(nums []int, K int, queries [][]int) []int {
	k = K
	t := newSegmentTreeWithArray(nums)
	n := len(nums)
	ans := make([]int, len(queries))
	for qi, q := range queries {
		t.update(1, 0, n-1, q[0], q[1])
		res := t.query(1, 0, n-1, q[2], n-1)
		ans[qi] = res.cnt[q[3]]
	}
	return ans
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}((n+q\log n)k)$，其中 $n$ 是 $\textit{nums}$ 的长度，$q$ 是 $\textit{queries}$ 的长度。
- 空间复杂度：$\mathcal{O}(nk)$。返回值不计入。

更多相似题目，见下面数据结构题单的「**§8.3 线段树（无区间更新）**」。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

## 本地原创解析

### 1. 题意重述

本题来自 `八、树状数组和线段树 / §8.3 线段树（无区间更新）`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

### 2. 暴力思路与瓶颈

直接枚举所有候选并逐个重新计算属性，通常会产生 $O(nk)$、$O(n^2)$ 或更高复杂度。瓶颈在于相邻候选之间有大量重复计算。

### 3. 关键观察

相邻状态通常只差少量元素或一个转移边界。只要把重复计算沉淀为可增量维护的统计量、单调结构、状态转移或图搜索标记，就能显著降低复杂度。

### 4. 算法设计

1. 根据题目约束确定窗口、前缀、二分、栈、图搜索、动态规划或数学变换的核心状态。
2. 初始化边界状态。
3. 按来源分类的套路推进枚举或转移，并在状态合法时更新答案。
4. 对边界不足、空状态、重复元素、负数、溢出、取模和不可达状态单独处理。

### 5. 正确性说明

枚举或转移过程覆盖所有合法候选；维护量在每一步与当前候选状态保持一致；答案只在候选合法或状态最优性成立时更新，因此最终结果等于所有合法候选的最优值、计数或可行性判断。

### 6. 复杂度分析

- 时间复杂度：依据具体题解正文确认；常见为 $O(n)$、$O(n\log n)$、$O(nm)$ 或状态数乘转移数。
- 空间复杂度：依据维护状态确认；常见为 $O(1)$、$O(k)$、$O(n)$ 或 DP/图状态规模。

### 7. C++17 实现

```cpp
#include <bits/stdc++.h>
using namespace std;

class Solution {
public:
    // TODO: 根据题目签名补全。当前批次先建立详解结构，代码需按题面签名复核。
};
```

### 8. 样例推演

当前本地层不复制题面样例。导入授权题解正文后，应结合正文中的示例或手工构造小样例，列出状态变化和答案更新时机。

### 9. 易错点

- 更新答案前必须确认当前状态已经合法。
- 删除、回退或转移状态时不要漏更新计数、和、频率表、访问标记或单调结构。
- 若题目含负数、重复值、空集合、取模、长整型溢出或特殊图结构，需单独核对边界。

### 10. 扩展解析

同一分类下的题目通常共享维护框架，差异主要在状态定义和合法性条件。复盘时应总结“状态是什么、何时合法、如何转移、答案如何更新”。

### 11. 同类题迁移

回到来源分类 `八、树状数组和线段树 / §8.3 线段树（无区间更新）`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
