# 2896. 执行操作使两个字符串相等

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/apply-operations-to-make-two-strings-equal/
- 题目 slug：`apply-operations-to-make-two-strings-equal`
- 来源专题：动态规划
- 来源分类路径：七、其他线性 DP / §7.1 一维 DP
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/apply-operations-to-make-two-strings-equal/solutions/2472122/ji-yi-hua-sou-suo-by-endlesscheng-64vq/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：从 O(n^2) 到 O(n)（Python/Java/C++/Go）](https://leetcode.cn/problems/apply-operations-to-make-two-strings-equal/solutions/2472122/ji-yi-hua-sou-suo-by-endlesscheng-64vq/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`ji-yi-hua-sou-suo-by-endlesscheng-64vq`
- topic id：`2472122`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 一个错误的贪心思路

对于两个需要反转的位置 $i$ 和 $j$，如果 $i+1$ 到 $j-1$ 都不需要反转，且 $j-i<x$，那么不断使用第二种操作直到 $i$ 和 $j$ 都反转，否则用第一种操作。

这是不对的，比如

```
s1=10000011000001
s2=00000000000000
```

更优的做法是反转首尾的 $1$。

## 方法一：O(n^2) DP

### 前置知识：动态规划入门

请看 [动态规划入门：从记忆化搜索到递推【基础算法精讲 17】](https://www.bilibili.com/video/BV1Xj411K7oF/)。

### 什么时候返回 -1？

注意到，反转两个字符：

- 如果都是 $0$ 或者都是 $1$，那么反转后 $1$ 的个数会加 $2$ 或者减 $2$；
- 如果一个是 $0$ 另一个是 $1$，那么反转后 $1$ 的个数不变。

所以，无论是哪种操作，都不会改变 $s_1$ 中的 $1$ 的个数的奇偶性。

那么只要 $s_1$ 和 $s_2$ 的 $1$ 的个数一个是奇数一个是偶数，就直接返回 $-1$。否则，哪怕只用第二种操作，都一定可以让 $s_1=s_2$。

### 然后呢？

做题时，可以先想 DP，再想贪心。毕竟 DP 就是对暴力搜索的优化，贪心还得思考它的正确性。（或者看题目的数据范围比较小，可以往 DP 上想。）

考虑 $s_1$ 和 $s_2$ 的最后一对字符（也可以考虑第一对字符）：

- 如果相同，那么无需修改。
- 如果不同：
   - 选择第一种操作，相当于后面可以「免费」反转一个字符。
   - 选择第二种操作，那么下一个字符要把 $0$ 看作 $1$，把 $1$ 看作 $0$。

所以除了知道当前下标 $i$，还需要知道免费反转次数 $j$，以及上一个字符是否选择了第二种操作 $\textit{preRev}$。

定义 $\textit{dfs}(i,j,\textit{preRev})$，参数含义如上，返回值是在这种状态下的最小操作代价之和。

分类讨论：

- 如果 `(s1[i] == s2[i]) == (not pre_rev)`，表示 $s_1[i]$ 和 $s_2[i]$ 是相等的，无需操作，返回 $\textit{dfs}(i - 1, j, \text{false})$。
- 否则：
    - 选择第一种操作：$\textit{dfs}(i - 1, j + 1, \text{false}) + x$。
    - 选择第二种操作：$\textit{dfs}(i - 1, j, \text{true}) + 1$。
    - 如果 $j>0$，免费反转一次：$\textit{dfs}(i - 1, j-1, \text{false})$。
    - 这三种情况取最小值。

递归边界，当 $i<0$ 时：

- 如果 $j>0$ 或者 $\textit{preRev}$ 为真，那么不合法，返回 $\infty$。
- 否则返回 $0$。

递归入口：$\textit{dfs}(n-1,0,\text{false})$，即答案。

请看 [视频讲解](https://www.bilibili.com/video/BV1e84y117R9/) 第三题。

```py [sol-Python3]
class Solution:
    def minOperations(self, s1: str, s2: str, x: int) -> int:
        if s1.count('1') % 2 != s2.count('1') % 2:
            return -1
        @cache
        def dfs(i: int, j: int, pre_rev: bool) -> int:
            if i < 0:
                return inf if j or pre_rev else 0
            if (s1[i] == s2[i]) == (not pre_rev):  # 无需反转
                return dfs(i - 1, j, False)
            res = min(dfs(i - 1, j + 1, False) + x, dfs(i - 1, j, True) + 1)
            if j:  # 可以免费反转
                res = min(res, dfs(i - 1, j - 1, False))
            return res
        return dfs(len(s1) - 1, 0, False)
```

```java [sol-Java]
class Solution {
    public int minOperations(String s1, String s2, int x) {
        char[] s = s1.toCharArray(), t = s2.toCharArray();
        int n = s.length, diff = 0;
        for (int i = 0; i < n; i++) {
            diff ^= s[i] ^ t[i];
        }
        if (diff != 0) {
            return -1;
        }
        int[][][] memo = new int[n][n + 1][2];
        for (int i = 0; i < n; i++) {
            for (int j = 0; j <= n; j++) {
                Arrays.fill(memo[i][j], -1);// -1 表示没有计算过
            }
        }
        return dfs(n - 1, 0, 0, memo, s, t, x);
    }

    private int dfs(int i, int j, int preRev, int[][][] memo, char[] s, char[] t, int x) {
        if (i < 0) { // 递归边界
            return j > 0 || preRev > 0 ? Integer.MAX_VALUE / 2 : 0;
        }
        if (memo[i][j][preRev] != -1) { // 之前计算过
            return memo[i][j][preRev];
        }
        if ((s[i] == t[i]) == (preRev == 0)) { // 无需反转
            return dfs(i - 1, j, 0, memo, s, t, x);
        }
        int res = Math.min(dfs(i - 1, j + 1, 0, memo, s, t, x) + x, dfs(i - 1, j, 1, memo, s, t, x) + 1);
        if (j > 0) { // 可以免费反转
            res = Math.min(res, dfs(i - 1, j - 1, 0, memo, s, t, x));
        }
        return memo[i][j][preRev] = res; // 记忆化
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minOperations(string s1, string s2, int x) {
        if (ranges::count(s1, '1') % 2 != ranges::count(s2, '1') % 2) {
            return -1;
        }
        int n = s1.length();
        int memo[n][n + 1][2];
        memset(memo, -1, sizeof(memo)); // -1 表示没有计算过
        auto dfs = [&](this auto&& dfs, int i, int j, bool pre_rev) -> int {
            if (i < 0) {
                return j || pre_rev ? INT_MAX / 2 : 0;
            }
            int &res = memo[i][j][pre_rev]; // 注意这里是引用
            if (res != -1) { // 之前计算过
                return res;
            }
            if ((s1[i] == s2[i]) == !pre_rev) { // 无需反转
                return dfs(i - 1, j, false);
            }
            res = min(dfs(i - 1, j + 1, false) + x, dfs(i - 1, j, true) + 1);
            if (j) { // 可以免费反转
                res = min(res, dfs(i - 1, j - 1, false));
            }
            return res;
        };
        return dfs(n - 1, 0, false);
    }
};
```

```go [sol-Go]
func minOperations(s1, s2 string, x int) int {
	if strings.Count(s1, "1")%2 != strings.Count(s2, "1")%2 {
		return -1
	}
	n := len(s1)
	memo := make([][][2]int, n)
	for i := range memo {
		memo[i] = make([][2]int, n+1)
		for j := range memo[i] {
			memo[i][j] = [2]int{-1, -1}
		}
	}
	var dfs func(int, int, int) int
	dfs = func(i, j, preRev int) int {
		if i < 0 {
			if j > 0 || preRev > 0 {
				return 1e9
			}
			return 0
		}
		p := &memo[i][j][preRev]
		if *p != -1 {
			return *p
		}
		if s1[i] == s2[i] == (preRev == 0) { // 无需反转
			return dfs(i-1, j, 0)
		}
		res := min(dfs(i-1, j+1, 0)+x, dfs(i-1, j, 1)+1)
		if j > 0 { // 可以免费反转
			res = min(res, dfs(i-1, j-1, 0))
		}
		*p = res
		return res
	}
	return dfs(n-1, 0, 0)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2)$，其中 $n$ 为 $s_1$ 的长度。由于每个状态只会计算一次，动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题中状态个数等于 $\mathcal{O}(n^2)$，单个状态的计算时间为 $\mathcal{O}(1)$，所以动态规划的时间复杂度为 $\mathcal{O}(n^2)$。
- 空间复杂度：$\mathcal{O}(n^2)$。

## 方法二：O(n) DP

把 $s_1[i]\ne s_2[i]$ 的下标统计到数组 $p$ 中。如果 $p$ 的长度是奇数，那么和上面一样，无法操作，返回 $-1$。

设 $f[i]$ 表示修改 $p$ 的前 $i$ 个位置的最小花费，那么对于 $p[i]$，有两种方案：

第一种操作：花费 $x$，那么对于 $p[i]$ 相当于花费 $\dfrac{x}{2}$，因此
   
$$
f[i] = f[i-1] + \dfrac{x}{2}
$$

注意这个转移一定会发生偶数次，因为 $p$ 的长度是偶数，并且第二种操作每次反转两个数，所以第一种操作一定会反转偶数个下标。

第二种操作：需要不断用相邻的位置操作，把 $p[i]$ 和 $p[i-1]$ 都反转，那么需要操作 $p[i]-p[i-1]$ 次，因此

$$
f[i] = f[i-2] + p[i]-p[i-1]
$$

两者取最小值，即

$$
f[i] = \min(f[i-1] + \dfrac{x}{2}, f[i-2] + p[i]-p[i-1])
$$

代码实现时，为了方便处理 $\dfrac{x}{2}$，可以先在计算过程中把所有数都乘 $2$，最后返回答案的时候再除以 $2$。所以转移方程就是

$$
f[i] = \min(f[i-1] + x, f[i-2] + (p[i]-p[i-1]) \cdot 2)
$$

初始值 $f[-1]=0$，$f[0]=x$，答案为 $\dfrac{f[m-1]}{2}$，这里 $m$ 是数组 $p$ 的长度。你可以把 $f$ 数组的下标都加一来避免负数，也可以用两个变量滚动计算。

```py [sol-Python3]
class Solution:
    def minOperations(self, s1: str, s2: str, x: int) -> int:
        if s1 == s2:
            return 0
        p = [i for i, (x, y) in enumerate(zip(s1, s2)) if x != y]
        if len(p) % 2:
            return -1
        f0, f1 = 0, x
        for i, j in pairwise(p):
            f0, f1 = f1, min(f1 + x, f0 + (j - i) * 2)
        return f1 // 2
```

```java [sol-Java]
class Solution {
    public int minOperations(String s1, String s2, int x) {
        if (s1.equals(s2)) {
            return 0;
        }
        List<Integer> p = new ArrayList<>();
        for (int i = 0; i < s1.length(); i++) {
            if (s1.charAt(i) != s2.charAt(i)) {
                p.add(i);
            }
        }
        if (p.size() % 2 != 0) {
            return -1;
        }
        int f0 = 0, f1 = x;
        for (int i = 1; i < p.size(); i++) {
            int newF = Math.min(f1 + x, f0 + (p.get(i) - p.get(i - 1)) * 2);
            f0 = f1;
            f1 = newF;
        }
        return f1 / 2;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minOperations(string s1, string s2, int x) {
        if (s1 == s2) return 0;
        vector<int> p;
        for (int i = 0; i < s1.size(); i++)
            if (s1[i] != s2[i])
                p.push_back(i);
        if (p.size() % 2) return -1;
        int f0 = 0, f1 = x;
        for (int i = 1; i < p.size(); i++) {
            int new_f = min(f1 + x, f0 + (p[i] - p[i - 1]) * 2);
            f0 = f1;
            f1 = new_f;
        }
        return f1 / 2;
    }
};
```

```go [sol-Go]
func minOperations(s1, s2 string, x int) int {
	if s1 == s2 {
		return 0
	}
	p := []int{}
	for i, c := range s1 {
		if byte(c) != s2[i] {
			p = append(p, i)
		}
	}
	if len(p)%2 > 0 {
		return -1
	}
	f0, f1 := 0, x
	for i := 1; i < len(p); i++ {
		f0, f1 = f1, min(f1+x, f0+(p[i]-p[i-1])*2)
	}
	return f1 / 2
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $s_1$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 思考题

如果第一种操作限制 $|i-j|\ge k$ 呢？

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

## 一个错误的贪心思路

对于两个需要反转的位置 $i$ 和 $j$，如果 $i+1$ 到 $j-1$ 都不需要反转，且 $j-i<x$，那么不断使用第二种操作直到 $i$ 和 $j$ 都反转，否则用第一种操作。

这是不对的，比如

```
s1=10000011000001
s2=00000000000000
```

更优的做法是反转首尾的 $1$。

## 方法一：O(n^2) DP

### 前置知识：动态规划入门

请看 [动态规划入门：从记忆化搜索到递推【基础算法精讲 17】](https://www.bilibili.com/video/BV1Xj411K7oF/)。

### 什么时候返回 -1？

注意到，反转两个字符：

- 如果都是 $0$ 或者都是 $1$，那么反转后 $1$ 的个数会加 $2$ 或者减 $2$；
- 如果一个是 $0$ 另一个是 $1$，那么反转后 $1$ 的个数不变。

所以，无论是哪种操作，都不会改变 $s_1$ 中的 $1$ 的个数的奇偶性。

那么只要 $s_1$ 和 $s_2$ 的 $1$ 的个数一个是奇数一个是偶数，就直接返回 $-1$。否则，哪怕只用第二种操作，都一定可以让 $s_1=s_2$。

### 然后呢？

做题时，可以先想 DP，再想贪心。毕竟 DP 就是对暴力搜索的优化，贪心还得思考它的正确性。（或者看题目的数据范围比较小，可以往 DP 上想。）

考虑 $s_1$ 和 $s_2$ 的最后一对字符（也可以考虑第一对字符）：

- 如果相同，那么无需修改。
- 如果不同：
   - 选择第一种操作，相当于后面可以「免费」反转一个字符。
   - 选择第二种操作，那么下一个字符要把 $0$ 看作 $1$，把 $1$ 看作 $0$。

所以除了知道当前下标 $i$，还需要知道免费反转次数 $j$，以及上一个字符是否选择了第二种操作 $\textit{preRev}$。

定义 $\textit{dfs}(i,j,\textit{preRev})$，参数含义如上，返回值是在这种状态下的最小操作代价之和。

分类讨论：

- 如果 `(s1[i] == s2[i]) == (not pre_rev)`，表示 $s_1[i]$ 和 $s_2[i]$ 是相等的，无需操作，返回 $\textit{dfs}(i - 1, j, \text{false})$。
- 否则：
    - 选择第一种操作：$\textit{dfs}(i - 1, j + 1, \text{false}) + x$。
    - 选择第二种操作：$\textit{dfs}(i - 1, j, \text{true}) + 1$。
    - 如果 $j>0$，免费反转一次：$\textit{dfs}(i - 1, j-1, \text{false})$。
    - 这三种情况取最小值。

递归边界，当 $i<0$ 时：

- 如果 $j>0$ 或者 $\textit{preRev}$ 为真，那么不合法，返回 $\infty$。
- 否则返回 $0$。

递归入口：$\textit{dfs}(n-1,0,\text{false})$，即答案。

请看 [视频讲解](https://www.bilibili.com/video/BV1e84y117R9/) 第三题。

```py [sol-Python3]
class Solution:
    def minOperations(self, s1: str, s2: str, x: int) -> int:
        if s1.count('1') % 2 != s2.count('1') % 2:
            return -1
        @cache
        def dfs(i: int, j: int, pre_rev: bool) -> int:
            if i < 0:
                return inf if j or pre_rev else 0
            if (s1[i] == s2[i]) == (not pre_rev):  # 无需反转
                return dfs(i - 1, j, False)
            res = min(dfs(i - 1, j + 1, False) + x, dfs(i - 1, j, True) + 1)
            if j:  # 可以免费反转
                res = min(res, dfs(i - 1, j - 1, False))
            return res
        return dfs(len(s1) - 1, 0, False)
```

```java [sol-Java]
class Solution {
    public int minOperations(String s1, String s2, int x) {
        char[] s = s1.toCharArray(), t = s2.toCharArray();
        int n = s.length, diff = 0;
        for (int i = 0; i < n; i++) {
            diff ^= s[i] ^ t[i];
        }
        if (diff != 0) {
            return -1;
        }
        int[][][] memo = new int[n][n + 1][2];
        for (int i = 0; i < n; i++) {
            for (int j = 0; j <= n; j++) {
                Arrays.fill(memo[i][j], -1);// -1 表示没有计算过
            }
        }
        return dfs(n - 1, 0, 0, memo, s, t, x);
    }

    private int dfs(int i, int j, int preRev, int[][][] memo, char[] s, char[] t, int x) {
        if (i < 0) { // 递归边界
            return j > 0 || preRev > 0 ? Integer.MAX_VALUE / 2 : 0;
        }
        if (memo[i][j][preRev] != -1) { // 之前计算过
            return memo[i][j][preRev];
        }
        if ((s[i] == t[i]) == (preRev == 0)) { // 无需反转
            return dfs(i - 1, j, 0, memo, s, t, x);
        }
        int res = Math.min(dfs(i - 1, j + 1, 0, memo, s, t, x) + x, dfs(i - 1, j, 1, memo, s, t, x) + 1);
        if (j > 0) { // 可以免费反转
            res = Math.min(res, dfs(i - 1, j - 1, 0, memo, s, t, x));
        }
        return memo[i][j][preRev] = res; // 记忆化
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minOperations(string s1, string s2, int x) {
        if (ranges::count(s1, '1') % 2 != ranges::count(s2, '1') % 2) {
            return -1;
        }
        int n = s1.length();
        int memo[n][n + 1][2];
        memset(memo, -1, sizeof(memo)); // -1 表示没有计算过
        auto dfs = [&](this auto&& dfs, int i, int j, bool pre_rev) -> int {
            if (i < 0) {
                return j || pre_rev ? INT_MAX / 2 : 0;
            }
            int &res = memo[i][j][pre_rev]; // 注意这里是引用
            if (res != -1) { // 之前计算过
                return res;
            }
            if ((s1[i] == s2[i]) == !pre_rev) { // 无需反转
                return dfs(i - 1, j, false);
            }
            res = min(dfs(i - 1, j + 1, false) + x, dfs(i - 1, j, true) + 1);
            if (j) { // 可以免费反转
                res = min(res, dfs(i - 1, j - 1, false));
            }
            return res;
        };
        return dfs(n - 1, 0, false);
    }
};
```

```go [sol-Go]
func minOperations(s1, s2 string, x int) int {
	if strings.Count(s1, "1")%2 != strings.Count(s2, "1")%2 {
		return -1
	}
	n := len(s1)
	memo := make([][][2]int, n)
	for i := range memo {
		memo[i] = make([][2]int, n+1)
		for j := range memo[i] {
			memo[i][j] = [2]int{-1, -1}
		}
	}
	var dfs func(int, int, int) int
	dfs = func(i, j, preRev int) int {
		if i < 0 {
			if j > 0 || preRev > 0 {
				return 1e9
			}
			return 0
		}
		p := &memo[i][j][preRev]
		if *p != -1 {
			return *p
		}
		if s1[i] == s2[i] == (preRev == 0) { // 无需反转
			return dfs(i-1, j, 0)
		}
		res := min(dfs(i-1, j+1, 0)+x, dfs(i-1, j, 1)+1)
		if j > 0 { // 可以免费反转
			res = min(res, dfs(i-1, j-1, 0))
		}
		*p = res
		return res
	}
	return dfs(n-1, 0, 0)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2)$，其中 $n$ 为 $s_1$ 的长度。由于每个状态只会计算一次，动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题中状态个数等于 $\mathcal{O}(n^2)$，单个状态的计算时间为 $\mathcal{O}(1)$，所以动态规划的时间复杂度为 $\mathcal{O}(n^2)$。
- 空间复杂度：$\mathcal{O}(n^2)$。

## 方法二：O(n) DP

把 $s_1[i]\ne s_2[i]$ 的下标统计到数组 $p$ 中。如果 $p$ 的长度是奇数，那么和上面一样，无法操作，返回 $-1$。

设 $f[i]$ 表示修改 $p$ 的前 $i$ 个位置的最小花费，那么对于 $p[i]$，有两种方案：

第一种操作：花费 $x$，那么对于 $p[i]$ 相当于花费 $\dfrac{x}{2}$，因此
   
$$
f[i] = f[i-1] + \dfrac{x}{2}
$$

注意这个转移一定会发生偶数次，因为 $p$ 的长度是偶数，并且第二种操作每次反转两个数，所以第一种操作一定会反转偶数个下标。

第二种操作：需要不断用相邻的位置操作，把 $p[i]$ 和 $p[i-1]$ 都反转，那么需要操作 $p[i]-p[i-1]$ 次，因此

$$
f[i] = f[i-2] + p[i]-p[i-1]
$$

两者取最小值，即

$$
f[i] = \min(f[i-1] + \dfrac{x}{2}, f[i-2] + p[i]-p[i-1])
$$

代码实现时，为了方便处理 $\dfrac{x}{2}$，可以先在计算过程中把所有数都乘 $2$，最后返回答案的时候再除以 $2$。所以转移方程就是

$$
f[i] = \min(f[i-1] + x, f[i-2] + (p[i]-p[i-1]) \cdot 2)
$$

初始值 $f[-1]=0$，$f[0]=x$，答案为 $\dfrac{f[m-1]}{2}$，这里 $m$ 是数组 $p$ 的长度。你可以把 $f$ 数组的下标都加一来避免负数，也可以用两个变量滚动计算。

```py [sol-Python3]
class Solution:
    def minOperations(self, s1: str, s2: str, x: int) -> int:
        if s1 == s2:
            return 0
        p = [i for i, (x, y) in enumerate(zip(s1, s2)) if x != y]
        if len(p) % 2:
            return -1
        f0, f1 = 0, x
        for i, j in pairwise(p):
            f0, f1 = f1, min(f1 + x, f0 + (j - i) * 2)
        return f1 // 2
```

```java [sol-Java]
class Solution {
    public int minOperations(String s1, String s2, int x) {
        if (s1.equals(s2)) {
            return 0;
        }
        List<Integer> p = new ArrayList<>();
        for (int i = 0; i < s1.length(); i++) {
            if (s1.charAt(i) != s2.charAt(i)) {
                p.add(i);
            }
        }
        if (p.size() % 2 != 0) {
            return -1;
        }
        int f0 = 0, f1 = x;
        for (int i = 1; i < p.size(); i++) {
            int newF = Math.min(f1 + x, f0 + (p.get(i) - p.get(i - 1)) * 2);
            f0 = f1;
            f1 = newF;
        }
        return f1 / 2;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minOperations(string s1, string s2, int x) {
        if (s1 == s2) return 0;
        vector<int> p;
        for (int i = 0; i < s1.size(); i++)
            if (s1[i] != s2[i])
                p.push_back(i);
        if (p.size() % 2) return -1;
        int f0 = 0, f1 = x;
        for (int i = 1; i < p.size(); i++) {
            int new_f = min(f1 + x, f0 + (p[i] - p[i - 1]) * 2);
            f0 = f1;
            f1 = new_f;
        }
        return f1 / 2;
    }
};
```

```go [sol-Go]
func minOperations(s1, s2 string, x int) int {
	if s1 == s2 {
		return 0
	}
	p := []int{}
	for i, c := range s1 {
		if byte(c) != s2[i] {
			p = append(p, i)
		}
	}
	if len(p)%2 > 0 {
		return -1
	}
	f0, f1 := 0, x
	for i := 1; i < len(p); i++ {
		f0, f1 = f1, min(f1+x, f0+(p[i]-p[i-1])*2)
	}
	return f1 / 2
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $s_1$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 思考题

如果第一种操作限制 $|i-j|\ge k$ 呢？

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

本题来自 `七、其他线性 DP / §7.1 一维 DP`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `七、其他线性 DP / §7.1 一维 DP`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
