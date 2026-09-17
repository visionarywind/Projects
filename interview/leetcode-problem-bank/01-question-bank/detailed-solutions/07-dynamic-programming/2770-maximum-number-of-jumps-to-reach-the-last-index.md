# 2770. 达到末尾下标所需的最大跳跃次数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-number-of-jumps-to-reach-the-last-index/
- 题目 slug：`maximum-number-of-jumps-to-reach-the-last-index`
- 来源专题：动态规划
- 来源分类路径：十一、优化 DP / §11.4 树状数组/线段树优化 DP
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/maximum-number-of-jumps-to-reach-the-last-index/solutions/2336752/dong-tai-gui-hua-cong-ji-yi-hua-sou-suo-2ptkg/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：普通 DP / 值域线段树优化 DP（Python/Java/C++/Go）](https://leetcode.cn/problems/maximum-number-of-jumps-to-reach-the-last-index/solutions/2336752/dong-tai-gui-hua-cong-ji-yi-hua-sou-suo-2ptkg/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`dong-tai-gui-hua-cong-ji-yi-hua-sou-suo-2ptkg`
- topic id：`2336752`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 16:46:02 +0800

## 一、寻找子问题

想一想，最后一步发生了什么？

最后一步，我们从某个满足条件的下标 $i$ 跳到了下标 $n-1$。

枚举满足条件的 $i$，问题变成：

- 从下标 $0$ 到达下标 $i$ 所需的最大跳跃次数。

这是**和原问题相似的、规模更小的子问题**，可以用**递归**解决。

> 注：从右往左思考，主要是方便把递归翻译成从左往右的递推。从左往右思考也是可以的。

## 二、状态定义与状态转移方程

根据上面的讨论，定义 $\textit{dfs}(j)$ 表示从下标 $0$ 到达下标 $j$ 所需的最大跳跃次数。

枚举满足 $0\le i<j$ 且 $|\textit{nums}[i]-\textit{nums}[j]|\le \textit{target}$ 的下标 $i$，问题变成从下标 $0$ 到达下标 $i$ 所需的最大跳跃次数，再加上从 $i$ 跳到 $j$ 的一次。

取最大值，得

$$
\textit{dfs}(j) = \max_{i} \textit{dfs}(i) + 1
$$

其中 $0\le i<j$ 且 $|\textit{nums}[i]-\textit{nums}[j]|\le \textit{target}$。

**递归边界**：

- $\textit{dfs}(0)=0$。从 $0$ 到 $0$ 不用跳。
- 如果没有满足条件的 $i$，那么 $\textit{dfs}(j) = -\infty$。

**递归入口**：$\textit{dfs}(n-1)$，这是原问题，也是答案。

## 三、递归搜索 + 保存递归返回值 = 记忆化搜索

考虑到整个递归过程中有大量重复递归调用（递归入参相同）。由于递归函数没有副作用，同样的入参无论计算多少次，算出来的结果都是一样的，因此可以用**记忆化搜索**来优化：

- 如果一个状态（递归入参）是第一次遇到，那么可以在返回前，把状态及其结果记到一个 $\textit{memo}$ 数组中。
- 如果一个状态不是第一次遇到（$\textit{memo}$ 中保存的结果不等于 $\textit{memo}$ 的初始值），那么可以直接返回 $\textit{memo}$ 中保存的结果。

> Python 用户可以无视上面这段，直接用 `@cache` 装饰器。

关于记忆化搜索的原理，请看视频讲解 [动态规划入门：从记忆化搜索到递推【基础算法精讲 17】](https://www.bilibili.com/video/BV1Xj411K7oF/)，其中包含把记忆化搜索 1:1 翻译成递推的技巧。

```py [sol-Python3]
class Solution:
    def maximumJumps(self, nums: List[int], target: int) -> int:
        @cache  # 缓存装饰器，避免重复计算 dfs（一行代码实现记忆化）
        def dfs(j: int) -> int:
            if j == 0:  # 起点
                return 0
            res = -inf
            for i in range(j):
                if abs(nums[i] - nums[j]) <= target:  # 可以从 i 跳到 j
                    res = max(res, dfs(i) + 1)
            return res

        ans = dfs(len(nums) - 1)  # 终点
        return -1 if ans < 0 else ans
```

```java [sol-Java]
class Solution {
    public int maximumJumps(int[] nums, int target) {
        int n = nums.length;
        int[] memo = new int[n];
        int ans = dfs(n - 1, nums, target, memo);
        return ans < 0 ? -1 : ans;
    }

    private int dfs(int j, int[] nums, int target, int[] memo) {
        if (j == 0) { // 起点
            return 0;
        }

        if (memo[j] != 0) { // 之前计算过
            return memo[j];
        }

        int res = Integer.MIN_VALUE;
        for (int i = 0; i < j; i++) {
            if (Math.abs(nums[i] - nums[j]) <= target) { // 可以从 i 跳到 j
                res = Math.max(res, dfs(i, nums, target, memo) + 1);
            }
        }
        memo[j] = res; // 记忆化
        return res;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maximumJumps(vector<int>& nums, int target) {
        int n = nums.size();
        vector<int> memo(n);

        auto dfs = [&](this auto&& dfs, int j) -> int {
            if (j == 0) { // 起点
                return 0;
            }

            int& res = memo[j]; // 注意这里是引用
            if (res) { // 之前计算过
                return res;
            }

            res = INT_MIN;
            for (int i = 0; i < j; i++) {
                if (abs(nums[i] - nums[j]) <= target) { // 可以从 i 跳到 j
                    res = max(res, dfs(i) + 1);
                }
            }
            return res;
        };

        int ans = dfs(n - 1); // 终点
        return ans < 0 ? -1 : ans;
    }
};
```

```go [sol-Go]
func maximumJumps(nums []int, target int) int {
	n := len(nums)
	memo := make([]int, n)

	var dfs func(int) int
	dfs = func(j int) int {
		if j == 0 { // 起点
			return 0
		}

		p := &memo[j]
		if *p != 0 { // 之前计算过
			return *p
		}

		res := math.MinInt
		for i, x := range nums[:j] {
			if abs(x-nums[j]) <= target { // 可以从 i 跳到 j
				res = max(res, dfs(i)+1)
			}
		}
		*p = res // 记忆化
		return res
	}

	ans := dfs(n - 1) // 终点
	if ans < 0 {
		return -1
	}
	return ans
}

func abs(x int) int {
	if x < 0 {
		return -x
	}
	return x
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2)$，其中 $n$ 是 $\textit{nums}$ 的长度。由于每个状态只会计算一次，动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题状态个数等于 $\mathcal{O}(n)$，单个状态的计算时间为 $\mathcal{O}(n)$，所以总的时间复杂度为 $\mathcal{O}(n^2)$。
- 空间复杂度：$\mathcal{O}(n)$。保存多少状态，就需要多少空间。

## 四、1:1 翻译成递推

我们可以去掉递归中的「递」，只保留「归」的部分，即自底向上计算。

具体来说，$f[j]$ 的定义和 $\textit{dfs}(j)$ 的定义是完全一样的，都表示从下标 $0$ 到达下标 $j$ 所需的最大跳跃次数。

相应的递推式（状态转移方程）也和 $\textit{dfs}$ 一样：

$$
f[j] = \max_{i} f[i] + 1
$$

其中 $0\le i<j$ 且 $|\textit{nums}[i]-\textit{nums}[j]|\le \textit{target}$。

如果没有满足条件的 $i$，那么 $f[j] = -\infty$。

初始值 $f[0]=0$，翻译自递归边界 $\textit{dfs}(0)=0$。

答案为 $f[n-1]$，翻译自递归入口 $\textit{dfs}(n-1)$。

```py [sol-Python3]
class Solution:
    def maximumJumps(self, nums: List[int], target: int) -> int:
        n = len(nums)
        f = [-inf] * n
        f[0] = 0
        for j in range(1, n):
            for i in range(j):
                if abs(nums[i] - nums[j]) <= target:  # 可以从 i 跳到 j
                    f[j] = max(f[j], f[i] + 1)
        return -1 if f[-1] < 0 else f[-1]
```

```java [sol-Java]
class Solution {
    public int maximumJumps(int[] nums, int target) {
        int n = nums.length;
        int[] f = new int[n];
        for (int j = 1; j < n; j++) {
            f[j] = Integer.MIN_VALUE;
            for (int i = 0; i < j; i++) {
                if (Math.abs(nums[i] - nums[j]) <= target) { // 可以从 i 跳到 j
                    f[j] = Math.max(f[j], f[i] + 1);
                }
            }
        }
        return f[n - 1] < 0 ? -1 : f[n - 1];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maximumJumps(vector<int>& nums, int target) {
        int n = nums.size();
        vector<int> f(n, INT_MIN);
        f[0] = 0;
        for (int j = 1; j < n; j++) {
            for (int i = 0; i < j; i++) {
                if (abs(nums[i] - nums[j]) <= target) { // 可以从 i 跳到 j
                    f[j] = max(f[j], f[i] + 1);
                }
            }
        }
        return f[n - 1] < 0 ? -1 : f[n - 1];
    }
};
```

```go [sol-Go]
func maximumJumps(nums []int, target int) int {
	n := len(nums)
	f := make([]int, n)

	for j := 1; j < n; j++ {
		f[j] = math.MinInt
		for i, x := range nums[:j] {
			if abs(x-nums[j]) <= target { // 可以从 i 跳到 j
				f[j] = max(f[j], f[i]+1)
			}
		}
	}

	if f[n-1] < 0 {
		return -1
	}
	return f[n-1]
}

func abs(x int) int {
	if x < 0 {
		return -x
	}
	return x
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2)$，其中 $n$ 是 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 五、值域线段树优化 DP

如果 $n=10^5$，上面的做法就超时了。

遍历到 $\textit{nums}[j]$ 时，我们需要知道满足 $\textit{nums}[j]-\textit{target} \le \textit{nums}[i] \le \textit{nums}[j]+\textit{target}$ 的最大的 $f[i]$。

这可以用一棵**值域线段树**维护。线段树的区间是值域区间，例如区间 $[20,23]$ 指的是 $\textit{nums}$ 中的元素 $20,21,22,23$。线段树的每个节点保存的是值域区间对应的最大的 $f[i]$。例如 $\textit{nums}[4]=20$ 且 $f[4] = 3$，那么线段树维护的位置 $20$ 更新为 $3$。

如此一来，满足 $\textit{nums}[j]-\textit{target} \le \textit{nums}[i] \le \textit{nums}[j]+\textit{target}$ 的最大的 $f[i]$，可以通过线段树的**区间最值查询**得到。

```py [sol-Python3]
# 完整的线段树模板见 https://leetcode.cn/circle/discuss/mOr1u6/
class SegmentTree:
    def __init__(self, n: int) -> None:
        self.t = [-inf] * (2 << (n - 1).bit_length())

    def update(self, node: int, l: int, r: int, i: int, val: int) -> None:
        if l == r:  # 叶子
            self.t[node] = val
            return
        m = (l + r) // 2
        if i <= m:  # i 在左子树
            self.update(node * 2, l, m, i, val)
        else:  # i 在右子树
            self.update(node * 2 + 1, m + 1, r, i, val)
        self.t[node] = max(self.t[node * 2], self.t[node * 2 + 1])

    def query(self, node: int, l: int, r: int, ql: int, qr: int) -> int:
        if ql <= l and r <= qr:  # 当前子树完全在 [ql, qr] 内
            return self.t[node]
        m = (l + r) // 2
        if qr <= m:  # [ql, qr] 在左子树
            return self.query(node * 2, l, m, ql, qr)
        if ql > m:  # [ql, qr] 在右子树
            return self.query(node * 2 + 1, m + 1, r, ql, qr)
        return max(self.query(node * 2, l, m, ql, qr), self.query(node * 2 + 1, m + 1, r, ql, qr))


class Solution:
    def maximumJumps(self, nums: List[int], target: int) -> int:
        # 去重排序，便于离散化
        sorted_nums = sorted(set(nums))

        n = len(nums)
        m = len(sorted_nums)
        t = SegmentTree(m)  # 值域线段树

        # nums[0] 对应的 f[0] = 0
        t.update(1, 0, m - 1, bisect_left(sorted_nums, nums[0]), 0)

        for j in range(1, n):
            l = bisect_left(sorted_nums, nums[j] - target)       # >= nums[j]-target 的第一个数
            r = bisect_right(sorted_nums, nums[j] + target) - 1  # <= nums[j]+target 的最后一个数
            # t.query 返回满足 nums[j]-target <= nums[i] <= nums[j]+target 的最大的 f[i]
            fj = t.query(1, 0, m - 1, l, r) + 1
            t.update(1, 0, m - 1, bisect_left(sorted_nums, nums[j]), fj)

        return -1 if fj < 0 else fj
```

```java [sol-Java]
class Solution {
    // 完整的线段树模板见 https://leetcode.cn/circle/discuss/mOr1u6/
    private int[] tree;

    private void update(int node, int l, int r, int i, int val) {
        if (l == r) { // 叶子
            tree[node] = val;
            return;
        }
        int m = (l + r) / 2;
        if (i <= m) { // i 在左子树
            update(node * 2, l, m, i, val);
        } else { // i 在右子树
            update(node * 2 + 1, m + 1, r, i, val);
        }
        tree[node] = Math.max(tree[node * 2], tree[node * 2 + 1]);
    }

    private int query(int node, int l, int r, int ql, int qr) {
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
        return Math.max(query(node * 2, l, m, ql, qr), query(node * 2 + 1, m + 1, r, ql, qr));
    }

    public int maximumJumps(int[] nums, int target) {
        int n = nums.length;
        int[] sorted = nums.clone(); // 用于离散化
        Arrays.sort(sorted);

        tree = new int[2 << (32 - Integer.numberOfLeadingZeros(n - 1))];
        Arrays.fill(tree, Integer.MIN_VALUE);

        // nums[0] 对应的 f[0] = 0
        update(1, 0, n - 1, lowerBound(sorted, nums[0]), 0);

        for (int j = 1; ; j++) {
            int l = lowerBound(sorted, (long) nums[j] - target);         // >= nums[j]-target 的第一个数
            int r = lowerBound(sorted, (long) nums[j] + target + 1) - 1; // <= nums[j]+target 的最后一个数
            // query 返回满足 nums[j]-target <= nums[i] <= nums[j]+target 的最大的 f[i]
            int fj = query(1, 0, n - 1, l, r) + 1;
            if (j == n - 1) {
                return fj < 0 ? -1 : fj;
            }
            update(1, 0, n - 1, lowerBound(sorted, nums[j]), fj);
        }
    }

    // 见 https://www.bilibili.com/video/BV1AP41137w7/
    private int lowerBound(int[] nums, long target) {
        int left = -1;
        int right = nums.length;
        while (left + 1 < right) {
            int mid = (left + right) >>> 1;
            if (nums[mid] >= target) {
                right = mid;
            } else {
                left = mid;
            }
        }
        return right;
    }
}
```

```cpp [sol-C++]
// 完整的线段树模板见 https://leetcode.cn/circle/discuss/mOr1u6/
class SegmentTree {
    vector<int> tree;

public:
    SegmentTree(int n) : tree(2 << bit_width(n - 1u), INT_MIN) {}

    void update(int node, int l, int r, int i, int val) {
        if (l == r) { // 叶子
            tree[node] = val;
            return;
        }
        int m = (l + r) / 2;
        if (i <= m) { // i 在左子树
            update(node * 2, l, m, i, val);
        } else { // i 在右子树
            update(node * 2 + 1, m + 1, r, i, val);
        }
        tree[node] = max(tree[node * 2], tree[node * 2 + 1]);
    }

    int query(int node, int l, int r, int ql, int qr) const {
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
        return max(query(node * 2, l, m, ql, qr), query(node * 2 + 1, m + 1, r, ql, qr));
    }
};

class Solution {
public:
    int maximumJumps(vector<int>& nums, int target) {
        // 排序去重，便于离散化
        auto sorted = nums;
        ranges::sort(sorted);
        sorted.erase(ranges::unique(sorted).begin(), sorted.end());

        int n = nums.size();
        int m = sorted.size();

        SegmentTree t(m); // 值域线段树

        // nums[0] 对应的 f[0] = 0
        int pos = ranges::lower_bound(sorted, nums[0]) - sorted.begin();
        t.update(1, 0, m - 1, pos, 0);

        long long tar = target;
        for (int j = 1; ; j++) {
            int l = ranges::lower_bound(sorted, nums[j] - tar) - sorted.begin();     // >= nums[j]-target 的第一个数
            int r = ranges::upper_bound(sorted, nums[j] + tar) - sorted.begin() - 1; // <= nums[j]+target 的最后一个数
            // t.query 返回满足 nums[j]-target <= nums[i] <= nums[j]+target 的最大的 f[i]
            int fj = t.query(1, 0, m - 1, l, r) + 1;
            if (j == n - 1) {
                return fj < 0 ? -1 : fj;
            }
            pos = ranges::lower_bound(sorted, nums[j]) - sorted.begin();
            t.update(1, 0, m - 1, pos, fj);
        }
    }
};
```

```go [sol-Go]
// 完整的线段树模板见 https://leetcode.cn/circle/discuss/mOr1u6/
type seg []int

func (t seg) update(node, l, r, i, val int) {
	if l == r { // 叶子
		t[node] = val
		return
	}
	m := (l + r) / 2
	if i <= m { // i 在左子树
		t.update(node*2, l, m, i, val)
	} else { // i 在右子树
		t.update(node*2+1, m+1, r, i, val)
	}
	t[node] = max(t[node*2], t[node*2+1])
}

func (t seg) query(node, l, r, ql, qr int) int {
	if ql <= l && r <= qr { // 当前子树完全在 [ql, qr] 内
		return t[node]
	}
	m := (l + r) / 2
	if qr <= m { // [ql, qr] 在左子树
		return t.query(node*2, l, m, ql, qr)
	}
	if ql > m { // [ql, qr] 在右子树
		return t.query(node*2+1, m+1, r, ql, qr)
	}
	return max(t.query(node*2, l, m, ql, qr), t.query(node*2+1, m+1, r, ql, qr))
}

func maximumJumps(nums []int, target int) int {
	// 排序去重，便于离散化
	sorted := slices.Clone(nums)
	slices.Sort(sorted)
	sorted = slices.Compact(sorted)

	n := len(nums)
	m := len(sorted)

	t := make(seg, 2<<bits.Len(uint(m-1))) // 值域线段树
	for i := range t {
		t[i] = math.MinInt
	}

	// nums[0] 对应的 f[0] = 0
	t.update(1, 0, m-1, sort.SearchInts(sorted, nums[0]), 0)

	for j := 1; ; j++ {
		l := sort.SearchInts(sorted, nums[j]-target)       // >= nums[j]-target 的第一个数
		r := sort.SearchInts(sorted, nums[j]+target+1) - 1 // <= nums[j]+target 的最后一个数
		// t.query 返回满足 nums[j]-target <= nums[i] <= nums[j]+target 的最大的 f[i]
		fj := t.query(1, 0, m-1, l, r) + 1
		if j == n-1 {
			if fj < 0 {
				return -1
			}
			return fj
		}
		t.update(1, 0, m-1, sort.SearchInts(sorted, nums[j]), fj)
	}
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n)$，其中 $n$ 是 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 相似题目

[2407. 最长递增子序列 II](https://leetcode.cn/problems/longest-increasing-subsequence-ii/)

更多相似题目，见下面动态规划题单的「**§11.4 树状数组/线段树优化 DP**」和「**专题：跳跃游戏**」。

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
11. [链表、树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `十一、优化 DP / §11.4 树状数组/线段树优化 DP`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `十一、优化 DP / §11.4 树状数组/线段树优化 DP`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
