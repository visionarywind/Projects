# 3530. 有向无环图中合法拓扑排序的最大利润

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-profit-from-valid-topological-order-in-dag/
- 题目 slug：`maximum-profit-from-valid-topological-order-in-dag`
- 来源专题：动态规划
- 来源分类路径：九、状态压缩 DP（状压 DP） / §9.1 排列型状压 DP ① 相邻无关
- 难度分：2353
- 外部题解来源：https://leetcode.cn/problems/maximum-profit-from-valid-topological-order-in-dag/solutions/3663054/pai-lie-xing-zhuang-ya-dpcong-ji-yi-hua-z67rp/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[排列型状压 DP：从记忆化搜索到递推（Python/Java/C++/Go）](https://leetcode.cn/problems/maximum-profit-from-valid-topological-order-in-dag/solutions/3663054/pai-lie-xing-zhuang-ya-dpcong-ji-yi-hua-z67rp/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`pai-lie-xing-zhuang-ya-dpcong-ji-yi-hua-z67rp`
- topic id：`3663054`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

如果你没做过状压 DP，请先完成 [526. 优美的排列](https://leetcode.cn/problems/beautiful-arrangement/)，并阅读我的题解 [教你一步步思考状压 DP：从记忆化搜索到递推](https://leetcode.cn/problems/beautiful-arrangement/solution/jiao-ni-yi-bu-bu-si-kao-zhuang-ya-dpcong-c6kd/)。

**关键思路**：把拓扑序理解为先修课关系，在学习课程 $j$ 之前，$j$ 的所有先修课（直接前驱）必须全部学完。

定义 $\textit{dfs}(S)$ 表示在已学课程集合为 $S$ 的情况下，学完**剩余未学课程**可以获得的最大利润。

考虑下一门课程学哪个：

- 枚举下一门要学习的课程为 $j=0,1,2,\ldots,n-1$，要求满足 $j\notin S$ 且 $\textit{pre}[j] \subseteq S$。其中 $\textit{pre}[j]$ 表示 $j$ 的先修课集合。
- 要解决的问题变成：在已学课程集合为 $S \cup \{j\}$ 的情况下，学完剩余未学课程可以获得的最大利润，即 $\textit{dfs}(S \cup \{j\})$。

取最大值，得

$$
\textit{dfs}(S) = \max_{j=0}^{n-1} \textit{dfs}(S \cup \{j\}) + \textit{score}[j] \cdot (|S|+1)
$$

其中 $j\notin S$ 且 $\textit{pre}[j] \subseteq S$，$|S|$ 表示集合 $S$ 的大小。

递归边界：$\textit{dfs}(U)=0$，其中全集 $U=\{0,1,2,\ldots,n-1\}$。递归到 $S=U$ 的状态，表示所有课程全部学完，没有剩余未学课程，可以获得的最大利润为 $0$。

递归入口：$\textit{dfs}(\varnothing)$，也就是答案。其中 $\varnothing$ 表示空集，因为一开始什么课程也没有学。

**代码实现时，用二进制表示集合，用位运算实现集合操作，具体请看** [从集合论到位运算，常见位运算技巧分类总结](https://leetcode.cn/circle/discuss/CaOJ45/)。

**优化**：如果 $\textit{edges}$ 是空的（没有边），那么根据 [排序不等式](https://baike.baidu.com/item/%E6%8E%92%E5%BA%8F%E4%B8%8D%E7%AD%89%E5%BC%8F/7775728)，按照 $\textit{score}$ 从小到大选课是最优的。所以把 $\textit{score}$ 从小到大排序后，累加 $\textit{score}[i]\cdot(i+1)$，即为答案。

[本题视频讲解](https://www.bilibili.com/video/BV1J2jAziENo/?t=15m46s)，欢迎点赞关注~

## 写法一：记忆化搜索

```py [sol-Python3]
class Solution:
    def maxProfit(self, n: int, edges: List[List[int]], score: List[int]) -> int:
        if not edges:
            score.sort()
            return sum(s * i for i, s in enumerate(score, 1))

        # 记录每个节点的先修课（直接前驱）
        pre = [0] * n
        for x, y in edges:
            pre[y] |= 1 << x

        @cache  # 缓存装饰器，避免重复计算 dfs（一行代码实现记忆化）
        def dfs(s: int) -> int:
            res = 0
            i = s.bit_count() + 1  # 已学课程数加一
            # 枚举还没学过的课程 j，且 j 的所有先修课都学完了
            for j, p in enumerate(pre):
                if (s >> j & 1) == 0 and (s | p) == s:
                    r = dfs(s | 1 << j) + score[j] * i
                    if r > res:  # 手写 max
                        res = r
            return res

        return dfs(0)
```

```java [sol-Java]
class Solution {
    public int maxProfit(int n, int[][] edges, int[] score) {
        if (edges.length == 0) {
            Arrays.sort(score);
            int ans = 0;
            for (int i = 0; i < n; i++) {
                ans += score[i] * (i + 1);
            }
            return ans;
        }

        // 记录每个节点的先修课（直接前驱）
        int[] pre = new int[n];
        for (int[] e : edges) {
            pre[e[1]] |= 1 << e[0];
        }

        int[] memo = new int[1 << n];
        return dfs(0, pre, score, memo);
    }

    private int dfs(int s, int[] pre, int[] score, int[] memo) {
        if (memo[s] > 0) { // 之前计算过
            return memo[s];
        }
        int res = 0;
        int i = Integer.bitCount(s); // 已学课程数
        // 枚举还没学过的课程 j，且 j 的所有先修课都学完了
        for (int j = 0; j < pre.length; j++) {
            if ((s >> j & 1) == 0 && (s | pre[j]) == s) {
                res = Math.max(res, dfs(s | 1 << j, pre, score, memo) + score[j] * (i + 1));
            }
        }
        return memo[s] = res; // 记忆化
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxProfit(int n, vector<vector<int>>& edges, vector<int>& score) {
        if (edges.empty()) {
            ranges::sort(score);
            int ans = 0;
            for (int i = 0; i < n; i++) {
                ans += score[i] * (i + 1);
            }
            return ans;
        }

        // 记录每个节点的先修课（直接前驱）
        vector<int> pre(n);
        for (auto& e : edges) {
            pre[e[1]] |= 1 << e[0];
        }

        vector<int> memo(1 << n);
        auto dfs = [&](this auto&& dfs, int s) -> int {
            int& res = memo[s]; // 注意这里是引用
            if (res) { // 之前计算过
                return res;
            }
            int i = popcount((uint32_t) s); // 已学课程数
            // 枚举还没学过的课程 j，且 j 的所有先修课都学完了
            for (int j = 0; j < n; j++) {
                if ((s >> j & 1) == 0 && (s | pre[j]) == s) {
                    res = max(res, dfs(s | 1 << j) + score[j] * (i + 1));
                }
            }
            return res;
        };
        return dfs(0);
    }
};
```

```go [sol-Go]
func maxProfit(n int, edges [][]int, score []int) int {
	if len(edges) == 0 {
		slices.Sort(score)
		for i, s := range score {
			ans += s * (i + 1)
		}
		return
	}

	// 记录每个节点的先修课（直接前驱）
	pre := make([]int, n)
	for _, e := range edges {
		pre[e[1]] |= 1 << e[0]
	}

	memo := make([]int, 1<<n)
	var dfs func(s int) int
	dfs = func(s int) (res int) {
		m := &memo[s]
		if *m > 0 { // 之前计算过
			return *m
		}
		defer func() { *m = res }() // 记忆化
		i := bits.OnesCount(uint(s)) // 已学课程数
		// 枚举还没学过的课程 j，且 j 的所有先修课都学完了
		for j, p := range pre {
			if s>>j&1 == 0 && s|p == s {
				res = max(res, dfs(s|1<<j)+score[j]*(i+1))
			}
		}
		return
	}
	return dfs(0)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(m + n2^n)$，其中 $m$ 是 $\textit{edges}$ 的长度。由于每个状态只会计算一次，动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题状态个数等于 $\mathcal{O}(2^n)$，单个状态的计算时间为 $\mathcal{O}(n)$，所以记忆化搜索的时间复杂度为 $\mathcal{O}(n2^n)$。
- 空间复杂度：$\mathcal{O}(2^n)$。保存多少状态，就需要多少空间。

## 写法二：记忆化搜索 1:1 翻译成递推（倒序）

我们可以去掉递归中的「递」，只保留「归」的部分，即自底向上计算。

具体来说，$f[S]$ 的定义和 $\textit{dfs}(S)$ 的定义是完全一样的，都表示在已学课程集合为 $S$ 的情况下，学完**剩余未学课程**可以获得的最大利润。

相应的递推式（状态转移方程）也和 $\textit{dfs}$ 一样：

$$
f[S] = \max_{j=0}^{n-1} f[S \cup \{j\}] + \textit{score}[j] \cdot (|S|+1)
$$

初始值 $f[U]=0$，翻译自递归边界 $\textit{dfs}(U)=0$。

答案为 $f[\varnothing]$，翻译自递归入口 $\textit{dfs}(\varnothing)$。

⚠**注意**：下面的写法可能会超时，请看后面的写法三。

```py [sol-Python3]
# 超时了！请看写法三！
class Solution:
    def maxProfit(self, n: int, edges: List[List[int]], score: List[int]) -> int:
        if not edges:
            score.sort()
            return sum(s * i for i, s in enumerate(score, 1))

        # 记录每个节点的先修课（直接前驱）
        pre = [0] * n
        for x, y in edges:
            pre[y] |= 1 << x

        f = [0] * (1 << n)
        for s in range((1 << n) - 2, -1, -1):
            res = 0
            i = s.bit_count() + 1  # 已学课程数加一
            # 枚举还没学过的课程 j，且 j 的所有先修课都学完了
            for j, p in enumerate(pre):
                if (s >> j & 1) == 0 and (s | p) == s:
                    r = f[s | 1 << j] + score[j] * i
                    if r > res:  # 手写 max
                        res = r
            f[s] = res
        return f[0]
```

```java [sol-Java]
class Solution {
    public int maxProfit(int n, int[][] edges, int[] score) {
        if (edges.length == 0) {
            Arrays.sort(score);
            int ans = 0;
            for (int i = 0; i < n; i++) {
                ans += score[i] * (i + 1);
            }
            return ans;
        }

        // 记录每个节点的先修课（直接前驱）
        int[] pre = new int[n];
        for (int[] e : edges) {
            pre[e[1]] |= 1 << e[0];
        }

        int u = 1 << n;
        int[] f = new int[u];

        for (int s = u - 2; s >= 0; s--) {
            int i = Integer.bitCount(s); // 已学课程数
            // 枚举还没学过的课程 j，且 j 的所有先修课都学完了
            for (int j = 0; j < n; j++) {
                if ((s >> j & 1) == 0 && (s | pre[j]) == s) {
                    f[s] = Math.max(f[s], f[s | 1 << j] + score[j] * (i + 1));
                }
            }
        }
        return f[0];
    }
}
```

```cpp [sol-C++]
// 超时了！请看写法三！
class Solution {
public:
    int maxProfit(int n, vector<vector<int>>& edges, vector<int>& score) {
        if (edges.empty()) {
            ranges::sort(score);
            int ans = 0;
            for (int i = 0; i < n; i++) {
                ans += score[i] * (i + 1);
            }
            return ans;
        }

        // 记录每个节点的先修课（直接前驱）
        vector<int> pre(n);
        for (auto& e : edges) {
            pre[e[1]] |= 1 << e[0];
        }

        int u = 1 << n;
        vector<int> f(u);

        for (int s = u - 2; s >= 0; s--) {
            int i = popcount((uint32_t) s); // 已学课程数
            // 枚举还没学过的课程 j，且 j 的所有先修课都学完了
            for (int j = 0; j < n; j++) {
                if ((s >> j & 1) == 0 && (s | pre[j]) == s) {
                    f[s] = max(f[s], f[s | 1 << j] + score[j] * (i + 1));
                }
            }
        }
        return f[0];
    }
};
```

```go [sol-Go]
// 超时了！请看写法三！
func maxProfit(n int, edges [][]int, score []int) (ans int) {
	if len(edges) == 0 {
		slices.Sort(score)
		for i, s := range score {
			ans += s * (i + 1)
		}
		return
	}

	// 记录每个节点的先修课（直接前驱）
	pre := make([]int, n)
	for _, e := range edges {
		pre[e[1]] |= 1 << e[0]
	}

	u := 1 << n
	f := make([]int, u)

	for s := u - 2; s >= 0; s-- {
		i := bits.OnesCount(uint(s)) // 已学课程数
		// 枚举还没学过的课程 j，且 j 的所有先修课都学完了
		for j, p := range pre {
			if s>>j&1 == 0 && s|p == s {
				f[s] = max(f[s], f[s|1<<j]+score[j]*(i+1))
			}
		}
	}
	return f[0]
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(m + n2^n)$，其中 $m$ 是 $\textit{edges}$ 的长度。
- 空间复杂度：$\mathcal{O}(2^n)$。

## 写法三：刷表法（正序）

> **注**：在动态规划中，用转移来源更新当前状态叫**查表法**（写法二），用当前状态更新其他状态叫**刷表法**（写法三）。

定义 $f[S]$ 表示已学课程集合为 $S$ 时，从**已学课程**中可以获得的最大利润。

枚举不在 $S$ 中的课程 $j$，如果 $\textit{pre}[j] \subseteq S$ 用 $f[S] + \textit{score}[j]\cdot (|S|+1)$ 更新 $f[S\cup\{j\}]$ 的最大值。

这样写的好处是，如果一个状态从未被更新过，说明这个状态不合法（比如已经学完后面的课程，但前面的课程还没学），无需执行内层循环。

```py [sol-Python3]
class Solution:
    def maxProfit(self, n: int, edges: List[List[int]], score: List[int]) -> int:
        if not edges:
            score.sort()
            return sum(s * i for i, s in enumerate(score, 1))

        # 记录每个节点的先修课（直接前驱）
        pre = [0] * n
        for x, y in edges:
            pre[y] |= 1 << x

        f = [-1] * (1 << n)
        f[0] = 0
        for s, fs in enumerate(f):
            if fs < 0:  # 不合法状态，比如已经学完后面的课程，但前面的课程还没学
                continue
            i = s.bit_count() + 1  # 已学课程数加一
            # 枚举还没学过的课程 j，且 j 的所有先修课都学完了
            for j, p in enumerate(pre):
                if (s >> j & 1) == 0 and (s | p) == s:
                    new_s = s | 1 << j
                    r = f[s] + score[j] * i
                    if r > f[new_s]:  # 手写 max
                        f[new_s] = r
        return f[-1]
```

```java [sol-Java]
class Solution {
    public int maxProfit(int n, int[][] edges, int[] score) {
        if (edges.length == 0) {
            Arrays.sort(score);
            int ans = 0;
            for (int i = 0; i < n; i++) {
                ans += score[i] * (i + 1);
            }
            return ans;
        }

        // 记录每个节点的先修课（直接前驱）
        int[] pre = new int[n];
        for (int[] e : edges) {
            pre[e[1]] |= 1 << e[0];
        }

        int u = 1 << n;
        int[] f = new int[u];
        Arrays.fill(f, -1);
        f[0] = 0;

        for (int s = 0; s < u - 1; s++) {
            if (f[s] < 0) { // 不合法状态，比如已经学完后面的课程，但前面的课程还没学
                continue;
            }
            int i = Integer.bitCount(s); // 已学课程数
            // 枚举还没学过的课程 j，且 j 的所有先修课都学完了
            for (int j = 0; j < n; j++) {
                if ((s >> j & 1) == 0 && (s | pre[j]) == s) {
                    int newS = s | 1 << j;
                    f[newS] = Math.max(f[newS], f[s] + score[j] * (i + 1));
                }
            }
        }
        return f[u - 1];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxProfit(int n, vector<vector<int>>& edges, vector<int>& score) {
        if (edges.empty()) {
            ranges::sort(score);
            int ans = 0;
            for (int i = 0; i < n; i++) {
                ans += score[i] * (i + 1);
            }
            return ans;
        }

        // 记录每个节点的先修课（直接前驱）
        vector<int> pre(n);
        for (auto& e : edges) {
            pre[e[1]] |= 1 << e[0];
        }

        uint32_t u = 1 << n;
        vector<int> f(u, -1);
        f[0] = 0;

        for (uint32_t s = 0; s < u - 1; s++) {
            if (f[s] < 0) { // 不合法状态，比如已经学完后面的课程，但前面的课程还没学
                continue;
            }
            int i = popcount(s); // 已学课程数
            // 枚举还没学过的课程 j，且 j 的所有先修课都学完了
            for (int j = 0; j < n; j++) {
                if ((s >> j & 1) == 0 && (s | pre[j]) == s) {
                    int new_s = s | 1 << j;
                    f[new_s] = max(f[new_s], f[s] + score[j] * (i + 1));
                }
            }
        }
        return f[u - 1];
    }
};
```

```go [sol-Go]
func maxProfit(n int, edges [][]int, score []int) (ans int) {
	if len(edges) == 0 {
		slices.Sort(score)
		for i, s := range score {
			ans += s * (i + 1)
		}
		return
	}

	// 记录每个节点的先修课（直接前驱）
	pre := make([]int, n)
	for _, e := range edges {
		pre[e[1]] |= 1 << e[0]
	}

	u := 1 << n
	f := make([]int, u)
	for s := 1; s < u; s++ {
		f[s] = -1
	}

	for s, fs := range f {
		if fs < 0 { // 不合法状态，比如已经学完后面的课程，但前面的课程还没学
			continue
		}
		i := bits.OnesCount(uint(s)) // 已学课程数
		// 枚举还没学过的课程 j，且 j 的所有先修课都学完了
		for j, p := range pre {
			if s>>j&1 == 0 && s|p == s {
				newS := s | 1<<j
				f[newS] = max(f[newS], fs+score[j]*(i+1))
			}
		}
	}
	return f[u-1]
}
```

```go [sol-Go 写法二]
func maxProfit(n int, edges [][]int, score []int) (ans int) {
	if len(edges) == 0 {
		slices.Sort(score)
		for i, s := range score {
			ans += s * (i + 1)
		}
		return
	}

	// 记录每个节点的先修课（直接前驱）
	pre := make([]int, n)
	for _, e := range edges {
		pre[e[1]] |= 1 << e[0]
	}

	u := 1 << n
	f := make([]int, u)
	for s := 1; s < u; s++ {
		f[s] = -1
	}

	for s, fs := range f {
		if fs < 0 { // 不合法状态，比如已经学完后面的课程，但前面的课程还没学
			continue
		}
		i := bits.OnesCount(uint(s)) // 已学课程数
		// 枚举还没学过的课程 j，且 j 的所有先修课都学完了
		for cus, lb := u-1^s, 0; cus > 0; cus ^= lb {
			lb = cus & -cus
			j := bits.TrailingZeros(uint(lb))
			if s|pre[j] == s {
				newS := s | lb
				f[newS] = max(f[newS], fs+score[j]*(i+1))
			}
		}
	}
	return f[u-1]
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(m + n2^n)$，其中 $m$ 是 $\textit{edges}$ 的长度。
- 空间复杂度：$\mathcal{O}(2^n)$。

更多相似题目，见 [动态规划题单](https://leetcode.cn/circle/discuss/tXLS3i/) 的「**§9.1 排列型 ① 相邻无关**」。

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

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `九、状态压缩 DP（状压 DP） / §9.1 排列型状压 DP ① 相邻无关`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `九、状态压缩 DP（状压 DP） / §9.1 排列型状压 DP ① 相邻无关`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
