# 3473. 长度至少为 M 的 K 个子数组之和

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/sum-of-k-subarrays-with-length-at-least-m/
- 题目 slug：`sum-of-k-subarrays-with-length-at-least-m`
- 来源专题：动态规划
- 来源分类路径：五、划分型 DP / §5.3 约束划分个数
- 难度分：2274
- 外部题解来源：https://leetcode.cn/problems/sum-of-k-subarrays-with-length-at-least-m/solutions/3591733/hua-fen-xing-dp-qian-zhui-he-shi-zi-bian-3k0w/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[划分型 DP + 前缀和 + 式子变形（Python/Java/C++/Go）](https://leetcode.cn/problems/sum-of-k-subarrays-with-length-at-least-m/solutions/3591733/hua-fen-xing-dp-qian-zhui-he-shi-zi-bian-3k0w/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`hua-fen-xing-dp-qian-zhui-he-shi-zi-bian-3k0w`
- topic id：`3591733`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

按照**划分型 DP** 的套路（见 [动态规划题单](https://leetcode.cn/circle/discuss/tXLS3i/) §5.3 节），定义 $f[i][j]$ 表示从长为 $j$ 的前缀 $\textit{nums}[0]$ 到 $\textit{nums}[j-1]$ 取出 $i$ 个连续子数组所得到的最大和。

> 注：这里用左闭右开区间 $[0,j)$ 表示前缀，与前缀和定义相匹配，方便后面做公式变形。

分类讨论：

- 不选 $\textit{nums}[j-1]$，问题变成从长为 $j-1$ 的前缀 $\textit{nums}[0]$ 到 $\textit{nums}[j-2]$ 取出 $i$ 个连续子数组所得到的最大和，即 $f[i][j-1]$。
- 选 $\textit{nums}[j-1]$，也就是子数组 $\textit{nums}[L]$ 到 $\textit{nums}[j-1]$（$L$ 是我们枚举的值），问题变成从长为 $L$ 的前缀 $\textit{nums}[0]$ 到 $\textit{nums}[L-1]$ 取出 $i-1$ 个连续子数组所得到的最大和，即 $f[i-1][L]$。

二者取最大值，得

$$
f[i][j] = \max\left(f[i][j-1], \max_{L=(i-1)\cdot m}^{j-m} f[i-1][L] + s[j] - s[L]\right)
$$

其中 $s$ 是 $\textit{nums}$ 的前缀和数组（长为 $n+1$），原理和定义请看 [前缀和讲解](https://leetcode.cn/problems/range-sum-query-immutable/solution/qian-zhui-he-ji-qi-kuo-zhan-fu-ti-dan-py-vaar/)。

其中 $L$ 最小是 $(i-1)\cdot m$，因为左边有 $i-1$ 个长度至少为 $m$ 的子数组。

**初始值**：

- $f[0][j] = 0$，不选任何子数组，元素和为 $0$。
- 如果 $j < i\cdot m$，那么 $f[i][j]=-\infty$，因为没有足够的数选取 $i$ 个长度至少为 $m$ 的子数组。这里用 $-\infty$ 表示不合法的状态，这样计算 $\max$ 不会取到不合法的状态。实际上只需要初始化 $f[i][i\cdot m-1] = -\infty$，因为更前面的状态不会被使用。

**答案**：$f[k][n]$。

这样做的时间复杂度为 $\mathcal{O}(n^2k)$，会超时。

由于

$$
\max_{L=(i-1)\cdot m}^{j-m} f[i-1][L] + s[j] - s[L] = s[j] + \max_{L=(i-1)\cdot m}^{j-m} f[i-1][L] - s[L]
$$

故定义 $d[L] = f[i-1][L] - s[L]$，并用一个变量 $\textit{mx}$ 在遍历 $j$ 的同时维护从 $L=(i-1)\cdot m$ 到 $j-m$ 的最大 $d[L]$，那么转移方程优化成

$$
f[i][j] = \max(f[i][j-1], \textit{mx} + s[j])
$$

这样就可以做到 $\mathcal{O}(nk)$ 时间了。

具体请看 [视频讲解](https://www.bilibili.com/video/BV1QP9bY3EL6/?t=16m57s)，欢迎点赞关注~

```py [sol-Python3]
class Solution:
    def maxSum(self, nums: List[int], k: int, m: int) -> int:
        n = len(nums)
        s = list(accumulate(nums, initial=0))  # nums 的前缀和
        f = [[0] * (n + 1) for _ in range(k + 1)]
        for i in range(1, k + 1):
            f[i][i * m - 1] = mx = -inf
            # 左右两边留出足够空间给其他子数组
            for j in range(i * m, n - (k - i) * m + 1):
                # mx 表示最大的 f[i-1][L]-s[L]，其中 L 在区间 [(i-1)*m, j-m] 中
                mx = max(mx, f[i - 1][j - m] - s[j - m])
                f[i][j] = max(f[i][j - 1], mx + s[j])  # 不选 vs 选
        return f[k][n]
```

```java [sol-Java]
class Solution {
    public int maxSum(int[] nums, int k, int m) {
        int n = nums.length;
        int[] s = new int[n + 1];
        for (int i = 0; i < n; i++) {
            s[i + 1] = s[i] + nums[i]; // nums 的前缀和
        }

        int[][] f = new int[k + 1][n + 1];
        for (int i = 1; i <= k; i++) {
            f[i][i * m - 1] = Integer.MIN_VALUE;
            int mx = Integer.MIN_VALUE;
            // 左右两边留出足够空间给其他子数组
            for (int j = i * m; j <= n - (k - i) * m; j++) {
                // mx 表示最大的 f[i-1][L]-s[L]，其中 L 在区间 [(i-1)*m, j-m] 中
                mx = Math.max(mx, f[i - 1][j - m] - s[j - m]);
                f[i][j] = Math.max(f[i][j - 1], mx + s[j]); // 不选 vs 选
            }
        }
        return f[k][n];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxSum(vector<int>& nums, int k, int m) {
        int n = nums.size();
        vector<int> s(n + 1);
        partial_sum(nums.begin(), nums.end(), s.begin() + 1); // nums 的前缀和

        vector f(k + 1, vector<int>(n + 1));
        for (int i = 1; i <= k; i++) {
            f[i][i * m - 1] = INT_MIN;
            int mx = INT_MIN;
            // 左右两边留出足够空间给其他子数组
            for (int j = i * m; j <= n - (k - i) * m; j++) {
                // mx 表示最大的 f[i-1][L]-s[L]，其中 L 在区间 [(i-1)*m, j-m] 中
                mx = max(mx, f[i - 1][j - m] - s[j - m]);
                f[i][j] = max(f[i][j - 1], mx + s[j]); // 不选 vs 选
            }
        }
        return f[k][n];
    }
};
```

```go [sol-Go]
func maxSum(nums []int, k, m int) int {
	n := len(nums)
	s := make([]int, n+1)
	for i, x := range nums {
		s[i+1] = s[i] + x // nums 的前缀和
	}

	f := make([][]int, k+1)
	f[0] = make([]int, n+1)
	for i := 1; i <= k; i++ {
		f[i] = make([]int, n+1)
		f[i][i*m-1] = math.MinInt
		mx := math.MinInt
		// 左右两边留出足够空间给其他子数组
		for j := i * m; j <= n-(k-i)*m; j++ {
			// mx 表示最大的 f[i-1][L]-s[L]，其中 L 在区间 [(i-1)*m, j-m] 中
			mx = max(mx, f[i-1][j-m]-s[j-m])
			f[i][j] = max(f[i][j-1], mx+s[j]) // 不选 vs 选
		}
	}
	return f[k][n]
}
```

## 滚动数组

```py [sol-Python3]
class Solution:
    def maxSum(self, nums: List[int], k: int, m: int) -> int:
        n = len(nums)
        s = list(accumulate(nums, initial=0))
        f = [0] * (n + 1)
        g = [0] * (n + 1)
        for i in range(1, k + 1):
            g[i * m - 1] = mx = -inf
            for j in range(i * m, n - (k - i) * m + 1):
                mx = max(mx, f[j - m] - s[j - m])
                g[j] = max(g[j - 1], mx + s[j])
            f, g = g, f
        return f[n]
```

```java [sol-Java]
class Solution {
    public int maxSum(int[] nums, int k, int m) {
        int n = nums.length;
        int[] s = new int[n + 1];
        for (int i = 0; i < n; i++) {
            s[i + 1] = s[i] + nums[i];
        }

        int[] f = new int[n + 1];
        int[] g = new int[n + 1];
        for (int i = 1; i <= k; i++) {
            g[i * m - 1] = Integer.MIN_VALUE;
            int mx = Integer.MIN_VALUE;
            for (int j = i * m; j <= n - (k - i) * m; j++) {
                mx = Math.max(mx, f[j - m] - s[j - m]);
                g[j] = Math.max(g[j - 1], mx + s[j]);
            }
            int[] tmp = f;
            f = g;
            g = tmp;
        }
        return f[n];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxSum(vector<int>& nums, int k, int m) {
        int n = nums.size();
        vector<int> s(n + 1);
        partial_sum(nums.begin(), nums.end(), s.begin() + 1);

        vector<int> f(n + 1), g(n + 1);
        for (int i = 1; i <= k; i++) {
            g[i * m - 1] = INT_MIN;
            int mx = INT_MIN;
            for (int j = i * m; j <= n - (k - i) * m; j++) {
                mx = max(mx, f[j - m] - s[j - m]);
                g[j] = max(g[j - 1], mx + s[j]);
            }
            swap(f, g);
        }
        return f[n];
    }
};
```

```go [sol-Go]
func maxSum(nums []int, k, m int) int {
	n := len(nums)
	s := make([]int, n+1)
	for i, x := range nums {
		s[i+1] = s[i] + x
	}

	f := make([]int, n+1)
	g := make([]int, n+1)
	for i := 1; i <= k; i++ {
		g[i*m-1] = math.MinInt
		mx := math.MinInt
		for j := i * m; j <= n-(k-i)*m; j++ {
			mx = max(mx, f[j-m]-s[j-m])
			g[j] = max(g[j-1], mx+s[j])
		}
		f, g = g, f
	}
	return f[n]
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}((n-km)k)$，其中 $n$ 是 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

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

## 滚动数组

```py [sol-Python3]
class Solution:
    def maxSum(self, nums: List[int], k: int, m: int) -> int:
        n = len(nums)
        s = list(accumulate(nums, initial=0))
        f = [0] * (n + 1)
        g = [0] * (n + 1)
        for i in range(1, k + 1):
            g[i * m - 1] = mx = -inf
            for j in range(i * m, n - (k - i) * m + 1):
                mx = max(mx, f[j - m] - s[j - m])
                g[j] = max(g[j - 1], mx + s[j])
            f, g = g, f
        return f[n]
```

```java [sol-Java]
class Solution {
    public int maxSum(int[] nums, int k, int m) {
        int n = nums.length;
        int[] s = new int[n + 1];
        for (int i = 0; i < n; i++) {
            s[i + 1] = s[i] + nums[i];
        }

        int[] f = new int[n + 1];
        int[] g = new int[n + 1];
        for (int i = 1; i <= k; i++) {
            g[i * m - 1] = Integer.MIN_VALUE;
            int mx = Integer.MIN_VALUE;
            for (int j = i * m; j <= n - (k - i) * m; j++) {
                mx = Math.max(mx, f[j - m] - s[j - m]);
                g[j] = Math.max(g[j - 1], mx + s[j]);
            }
            int[] tmp = f;
            f = g;
            g = tmp;
        }
        return f[n];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxSum(vector<int>& nums, int k, int m) {
        int n = nums.size();
        vector<int> s(n + 1);
        partial_sum(nums.begin(), nums.end(), s.begin() + 1);

        vector<int> f(n + 1), g(n + 1);
        for (int i = 1; i <= k; i++) {
            g[i * m - 1] = INT_MIN;
            int mx = INT_MIN;
            for (int j = i * m; j <= n - (k - i) * m; j++) {
                mx = max(mx, f[j - m] - s[j - m]);
                g[j] = max(g[j - 1], mx + s[j]);
            }
            swap(f, g);
        }
        return f[n];
    }
};
```

```go [sol-Go]
func maxSum(nums []int, k, m int) int {
	n := len(nums)
	s := make([]int, n+1)
	for i, x := range nums {
		s[i+1] = s[i] + x
	}

	f := make([]int, n+1)
	g := make([]int, n+1)
	for i := 1; i <= k; i++ {
		g[i*m-1] = math.MinInt
		mx := math.MinInt
		for j := i * m; j <= n-(k-i)*m; j++ {
			mx = max(mx, f[j-m]-s[j-m])
			g[j] = max(g[j-1], mx+s[j])
		}
		f, g = g, f
	}
	return f[n]
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}((n-km)k)$，其中 $n$ 是 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

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

本题来自 `五、划分型 DP / §5.3 约束划分个数`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `五、划分型 DP / §5.3 约束划分个数`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
