# 3624. 位计数深度为 K 的整数数目 II

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/number-of-integers-with-popcount-depth-equal-to-k-ii/
- 题目 slug：`number-of-integers-with-popcount-depth-equal-to-k-ii`
- 来源专题：常用数据结构
- 来源分类路径：八、树状数组和线段树 / §8.1 树状数组
- 难度分：2086
- 外部题解来源：https://leetcode.cn/problems/number-of-integers-with-popcount-depth-equal-to-k-ii/solutions/3728547/6-ge-shu-zhuang-shu-zu-pythonjavacgo-by-klqxt/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[6 个树状数组（Python/Java/C++/Go）](https://leetcode.cn/problems/number-of-integers-with-popcount-depth-equal-to-k-ii/solutions/3728547/6-ge-shu-zhuang-shu-zu-pythonjavacgo-by-klqxt/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`6-ge-shu-zhuang-shu-zu-pythonjavacgo-by-klqxt`
- topic id：`3728547`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

**前置知识**：[带你发明树状数组！附数学证明](https://leetcode.cn/problems/range-sum-query-mutable/solution/dai-ni-fa-ming-shu-zhuang-shu-zu-fu-shu-lyfll/)

由于 $0\le k \le 5$，用 $6$ 棵树状数组维护。

比如 $k=3$ 的树状数组维护的是一个 $0\text{-}1$ 数组 $a$ 的前缀和，其中：

- $a[i] = 0$ 表示 $\textit{nums}[i]$ 的 popcount-depth 值不等于 $3$。
- $a[i] = 1$ 表示 $\textit{nums}[i]$ 的 popcount-depth 值等于 $3$。

注：在本题的数据范围下，popcount-depth 值 $\le 4$，所以无需判断 popcount-depth 值是否下标越界。也可以用 $5$ 棵树状数组。

具体请看 [视频讲解](https://www.bilibili.com/video/BV1tbg8z3EaP/?t=23m26s)，欢迎点赞关注~

## 优化前

```py [sol-Python3]
class FenwickTree:
    def __init__(self, n: int):
        self.tree = [0] * (n + 1)  # 使用下标 1 到 n

    # a[i] 增加 val
    # 1 <= i <= n
    # 时间复杂度 O(log n)
    def update(self, i: int, val: int) -> None:
        while i < len(self.tree):
            self.tree[i] += val
            i += i & -i

    # 计算前缀和 a[1] + ... + a[i]
    # 1 <= i <= n
    # 时间复杂度 O(log n)
    def pre(self, i: int) -> int:
        res = 0
        while i > 0:
            res += self.tree[i]
            i &= i - 1
        return res

    # 计算区间和 a[l] + ... + a[r]
    # 1 <= l <= r <= n
    # 时间复杂度 O(log n)
    def query(self, l: int, r: int) -> int:
        return self.pre(r) - self.pre(l - 1)

# 不写记忆化，直接迭代
def pop_depth(x: int) -> int:
    res = 0
    while x > 1:
        res += 1
        x = x.bit_count()
    return res

class Solution:
    def popcountDepth(self, nums: List[int], queries: List[List[int]]) -> List[int]:
        n = len(nums)
        f = [FenwickTree(n) for _ in range(6)]

        def update(i: int, delta: int) -> None:
            d = pop_depth(nums[i])
            f[d].update(i + 1, delta)

        for i in range(n):
            update(i, 1)  # 添加

        ans = []
        for q in queries:
            if q[0] == 1:
                ans.append(f[q[3]].query(q[1] + 1, q[2] + 1))
            else:
                i = q[1]
                update(i, -1)  # 撤销旧的
                nums[i] = q[2]
                update(i, 1)  # 添加新的
        return ans
```

```java [sol-Java]
class FenwickTree {
    private final int[] tree; // 如果计算结果没有超出 int 范围，可以改成 int

    public FenwickTree(int n) {
        tree = new int[n + 1]; // 使用下标 1 到 n
    }

    // a[i] 增加 val
    // 1 <= i <= n
    // 时间复杂度 O(log n)
    public void update(int i, int val) {
        for (; i < tree.length; i += i & -i) {
            tree[i] += val;
        }
    }

    // 求前缀和 a[1] + ... + a[i]
    // 1 <= i <= n
    // 时间复杂度 O(log n)
    public int pre(int i) {
        int res = 0;
        for (; i > 0; i &= i - 1) {
            res += tree[i];
        }
        return res;
    }

    // 求区间和 a[l] + ... + a[r]
    // 1 <= l <= r <= n
    // 时间复杂度 O(log n)
    public int query(int l, int r) {
        return pre(r) - pre(l - 1);
    }
}

class Solution {
    public int[] popcountDepth(long[] nums, long[][] queries) {
        int n = nums.length;
        FenwickTree[] f = new FenwickTree[6];
        Arrays.setAll(f, _ -> new FenwickTree(n));

        for (int i = 0; i < n; i++) {
            update(i, nums[i], 1, f); // 添加
        }

        int ansSize = 0;
        for (long[] q : queries) {
            ansSize += q[0] == 1 ? 1 : 0;
        }

        int[] ans = new int[ansSize];
        int idx = 0;
        for (long[] q : queries) {
            if (q[0] == 1) {
                ans[idx++] = f[(int) q[3]].query((int) q[1] + 1, (int) q[2] + 1);
            } else {
                int i = (int) q[1];
                update(i, nums[i], -1, f); // 撤销旧的
                nums[i] = q[2];
                update(i, nums[i], 1, f); // 添加新的
            }
        }
        return ans;
    }

    private void update(int i, long x, int delta, FenwickTree[] f) {
        int d = popDepth(x);
        f[d].update(i + 1, delta);
    }

    // 不写记忆化，直接迭代
    private int popDepth(long x) {
        int res = 0;
        while (x > 1) {
            res++;
            x = Long.bitCount(x);
        }
        return res;
    }
}
```

```cpp [sol-C++]
template<typename T>
class FenwickTree {
    vector<T> tree;

public:
    // 使用下标 1 到 n
    FenwickTree(int n) : tree(n + 1) {}

    // a[i] 增加 val
    // 1 <= i <= n
    // 时间复杂度 O(log n)
    void update(int i, T val) {
        for (; i < tree.size(); i += i & -i) {
            tree[i] += val;
        }
    }

    // 求前缀和 a[1] + ... + a[i]
    // 1 <= i <= n
    // 时间复杂度 O(log n)
    T pre(int i) const {
        T res = 0;
        for (; i > 0; i &= i - 1) {
            res += tree[i];
        }
        return res;
    }

    // 求区间和 a[l] + ... + a[r]
    // 1 <= l <= r <= n
    // 时间复杂度 O(log n)
    T query(int l, int r) const {
        return pre(r) - pre(l - 1);
    }
};

class Solution {
    // 不写记忆化，直接迭代
    int pop_depth(uint64_t x) {
        int res = 0;
        while (x > 1) {
            res++;
            x = popcount(x);
        }
        return res;
    }

public:
    vector<int> popcountDepth(vector<long long>& nums, vector<vector<long long>>& queries) {
        int n = nums.size();
        vector f(6, FenwickTree<int>(n));
        auto update = [&](int i, int delta) -> void {
            int d = pop_depth(nums[i]);
            f[d].update(i + 1, delta);
        };

        for (int i = 0; i < n; i++) {
            update(i, 1); // 添加
        }

        vector<int> ans;
        for (auto& q : queries) {
            if (q[0] == 1) {
                ans.push_back(f[q[3]].query(q[1] + 1, q[2] + 1));
            } else {
                int i = q[1];
                update(i, -1); // 撤销旧的
                nums[i] = q[2];
                update(i, 1); // 添加新的
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
type fenwick []int

func newFenwickTree(n int) fenwick {
    return make(fenwick, n+1) // 使用下标 1 到 n
}

// a[i] 增加 val
// 1 <= i <= n
// 时间复杂度 O(log n)
func (f fenwick) update(i int, val int) {
    for ; i < len(f); i += i & -i {
        f[i] += val
    }
}

// 求前缀和 a[1] + ... + a[i]
// 1 <= i <= n
// 时间复杂度 O(log n)
func (f fenwick) pre(i int) (res int) {
    for ; i > 0; i &= i - 1 {
        res += f[i]
    }
    return
}

// 求区间和 a[l] + ... + a[r]
// 1 <= l <= r <= n
// 时间复杂度 O(log n)
func (f fenwick) query(l, r int) int {
    return f.pre(r) - f.pre(l-1)
}

// 不写记忆化更快，直接迭代
func popDepth(x uint64) (res int) {
	for x > 1 {
		res++
		x = uint64(bits.OnesCount64(x))
	}
	return
}

func popcountDepth(nums []int64, queries [][]int64) (ans []int) {
	n := len(nums)
	f := [6]fenwick{}
	for i := range f {
		f[i] = newFenwickTree(n)
	}
	update := func(i, delta int) {
		d := popDepth(uint64(nums[i]))
		f[d].update(i+1, delta)
	}

	for i := range n {
		update(i, 1) // 添加
	}

	for _, q := range queries {
		if q[0] == 1 {
			ans = append(ans, f[q[3]].query(int(q[1])+1, int(q[2])+1))
		} else {
			i := int(q[1])
			update(i, -1) // 撤销旧的
			nums[i] = q[2]
			update(i, 1) // 添加新的
		}
	}
	return
}
```

## 优化

$10^{15}$ 的二进制长度是 $50$，所以任意元素计算一次 popcount-depth 后数值都会变成 $\le 50$。我们可以预处理 $\le 50$ 的数的 popcount-depth 值，从而减少计算次数。

```py [sol-Python3]
class FenwickTree:
    def __init__(self, n: int):
        self.tree = [0] * (n + 1)  # 使用下标 1 到 n

    # a[i] 增加 val
    # 1 <= i <= n
    # 时间复杂度 O(log n)
    def update(self, i: int, val: int) -> None:
        while i < len(self.tree):
            self.tree[i] += val
            i += i & -i

    # 计算前缀和 a[1] + ... + a[i]
    # 1 <= i <= n
    # 时间复杂度 O(log n)
    def pre(self, i: int) -> int:
        res = 0
        while i > 0:
            res += self.tree[i]
            i &= i - 1
        return res

    # 计算区间和 a[l] + ... + a[r]
    # 1 <= l <= r <= n
    # 时间复杂度 O(log n)
    def query(self, l: int, r: int) -> int:
        return self.pre(r) - self.pre(l - 1)


pop_depth_list = [0] * 51
for i in range(2, len(pop_depth_list)):
    pop_depth_list[i] = pop_depth_list[i.bit_count()] + 1

def pop_depth(x: int) -> int:
    if x == 1:
        return 0
    return pop_depth_list[x.bit_count()] + 1


class Solution:
    def popcountDepth(self, nums: List[int], queries: List[List[int]]) -> List[int]:
        n = len(nums)
        f = [FenwickTree(n) for _ in range(6)]

        def update(i: int, delta: int) -> None:
            d = pop_depth(nums[i])
            f[d].update(i + 1, delta)

        for i in range(n):
            update(i, 1)  # 添加

        ans = []
        for q in queries:
            if q[0] == 1:
                ans.append(f[q[3]].query(q[1] + 1, q[2] + 1))
            else:
                i = q[1]
                update(i, -1)  # 撤销旧的
                nums[i] = q[2]
                update(i, 1)  # 添加新的
        return ans
```

```java [sol-Java]
class FenwickTree {
    private final int[] tree; // 如果计算结果没有超出 int 范围，可以改成 int

    public FenwickTree(int n) {
        tree = new int[n + 1]; // 使用下标 1 到 n
    }

    // a[i] 增加 val
    // 1 <= i <= n
    // 时间复杂度 O(log n)
    public void update(int i, int val) {
        for (; i < tree.length; i += i & -i) {
            tree[i] += val;
        }
    }

    // 求前缀和 a[1] + ... + a[i]
    // 1 <= i <= n
    // 时间复杂度 O(log n)
    public int pre(int i) {
        int res = 0;
        for (; i > 0; i &= i - 1) {
            res += tree[i];
        }
        return res;
    }

    // 求区间和 a[l] + ... + a[r]
    // 1 <= l <= r <= n
    // 时间复杂度 O(log n)
    public int query(int l, int r) {
        return pre(r) - pre(l - 1);
    }
}

class Solution {
    private static final int[] popDepthList = new int[51];

    static {
        for (int i = 2; i < popDepthList.length; i++) {
            popDepthList[i] = popDepthList[Integer.bitCount(i)] + 1;
        }
    }

    public int[] popcountDepth(long[] nums, long[][] queries) {
        int n = nums.length;
        FenwickTree[] f = new FenwickTree[6];
        Arrays.setAll(f, _ -> new FenwickTree(n));

        for (int i = 0; i < n; i++) {
            update(i, nums[i], 1, f); // 添加
        }

        int ansSize = 0;
        for (long[] q : queries) {
            ansSize += q[0] == 1 ? 1 : 0;
        }

        int[] ans = new int[ansSize];
        int idx = 0;
        for (long[] q : queries) {
            if (q[0] == 1) {
                ans[idx++] = f[(int) q[3]].query((int) q[1] + 1, (int) q[2] + 1);
            } else {
                int i = (int) q[1];
                update(i, nums[i], -1, f); // 撤销旧的
                nums[i] = q[2];
                update(i, nums[i], 1, f); // 添加新的
            }
        }
        return ans;
    }

    private void update(int i, long x, int delta, FenwickTree[] f) {
        int d = popDepth(x);
        f[d].update(i + 1, delta);
    }

    private int popDepth(long x) {
        return x == 1 ? 0 : popDepthList[Long.bitCount(x)] + 1;
    }
}
```

```cpp [sol-C++]
template<typename T>
class FenwickTree {
    vector<T> tree;

public:
    // 使用下标 1 到 n
    FenwickTree(int n) : tree(n + 1) {}

    // a[i] 增加 val
    // 1 <= i <= n
    // 时间复杂度 O(log n)
    void update(int i, T val) {
        for (; i < tree.size(); i += i & -i) {
            tree[i] += val;
        }
    }

    // 求前缀和 a[1] + ... + a[i]
    // 1 <= i <= n
    // 时间复杂度 O(log n)
    T pre(int i) const {
        T res = 0;
        for (; i > 0; i &= i - 1) {
            res += tree[i];
        }
        return res;
    }

    // 求区间和 a[l] + ... + a[r]
    // 1 <= l <= r <= n
    // 时间复杂度 O(log n)
    T query(int l, int r) const {
        return pre(r) - pre(l - 1);
    }
};

const int MX = 51;
int pop_depth_list[MX];

int init = [] {
    for (size_t i = 2; i < MX; i++) {
        pop_depth_list[i] = pop_depth_list[popcount(i)] + 1;
    }
    return 0;
}();

class Solution {
    int pop_depth(uint64_t x) {
        return x == 1 ? 0 : pop_depth_list[popcount(x)] + 1;
    }

public:
    vector<int> popcountDepth(vector<long long>& nums, vector<vector<long long>>& queries) {
        int n = nums.size();
        vector f(6, FenwickTree<int>(n));
        auto update = [&](int i, int delta) -> void {
            int d = pop_depth(nums[i]);
            f[d].update(i + 1, delta);
        };

        for (int i = 0; i < n; i++) {
            update(i, 1); // 添加
        }

        vector<int> ans;
        for (auto& q : queries) {
            if (q[0] == 1) {
                ans.push_back(f[q[3]].query(q[1] + 1, q[2] + 1));
            } else {
                int i = q[1];
                update(i, -1); // 撤销旧的
                nums[i] = q[2];
                update(i, 1); // 添加新的
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
type fenwick []int

func newFenwickTree(n int) fenwick {
    return make(fenwick, n+1) // 使用下标 1 到 n
}

// a[i] 增加 val
// 1 <= i <= n
// 时间复杂度 O(log n)
func (f fenwick) update(i int, val int) {
    for ; i < len(f); i += i & -i {
        f[i] += val
    }
}

// 求前缀和 a[1] + ... + a[i]
// 1 <= i <= n
// 时间复杂度 O(log n)
func (f fenwick) pre(i int) (res int) {
    for ; i > 0; i &= i - 1 {
        res += f[i]
    }
    return
}

// 求区间和 a[l] + ... + a[r]
// 1 <= l <= r <= n
// 时间复杂度 O(log n)
func (f fenwick) query(l, r int) int {
    return f.pre(r) - f.pre(l-1)
}

var popDepthList [51]int

func init() {
	for i := 2; i < len(popDepthList); i++ {
		popDepthList[i] = popDepthList[bits.OnesCount(uint(i))] + 1
	}
}

func popDepth(x uint64) int {
	if x == 1 {
		return 0
	}
	return popDepthList[bits.OnesCount64(x)] + 1
}

func popcountDepth(nums []int64, queries [][]int64) (ans []int) {
	n := len(nums)
	f := [6]fenwick{}
	for i := range f {
		f[i] = newFenwickTree(n)
	}
	update := func(i, delta int) {
		d := popDepth(uint64(nums[i]))
		f[d].update(i+1, delta)
	}

	for i := range n {
		update(i, 1) // 添加
	}

	for _, q := range queries {
		if q[0] == 1 {
			ans = append(ans, f[q[3]].query(int(q[1])+1, int(q[2])+1))
		} else {
			i := int(q[1])
			update(i, -1) // 撤销旧的
			nums[i] = q[2]
			update(i, 1) // 添加新的
		}
	}
	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(nK + (n+q)\log n)$，其中 $n$ 是 $\textit{nums}$ 的长度，$q$ 是 $\textit{queries}$ 的长度，$K=6$。
- 空间复杂度：$\mathcal{O}(nK)$。返回值不计入。

**注**：可以用 $\mathcal{O}(n)$ 初始化树状数组的技巧，做到 $\mathcal{O}(nK + q\log n)$ 时间。

## 专题训练

见下面数据结构题单的「**§8.1 树状数组**」。

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

## 本地原创解析

### 1. 题意重述

本题来自 `八、树状数组和线段树 / §8.1 树状数组`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `八、树状数组和线段树 / §8.1 树状数组`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
