# 3599. 划分数组得到最小 XOR

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/partition-array-to-minimize-xor/
- 题目 slug：`partition-array-to-minimize-xor`
- 来源专题：图论算法
- 来源分类路径：三、最短路 / §3.1 单源最短路：Dijkstra 算法
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/partition-array-to-minimize-xor/solutions/3710966/hua-fen-xing-dp-de-tong-yong-tao-lu-pyth-lmcm/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[划分型 DP 的通用套路，附二分 / Dijkstra 做法（Python/Java/C++/Go）](https://leetcode.cn/problems/partition-array-to-minimize-xor/solutions/3710966/hua-fen-xing-dp-de-tong-yong-tao-lu-pyth-lmcm/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`hua-fen-xing-dp-de-tong-yong-tao-lu-pyth-lmcm`
- topic id：`3710966`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

⚠**注意**：本题可以二分答案，但由于异或和没有单调性，二分答案仍然要写 DP，那还不如直接写 DP 呢。

和 [动态规划题单](https://leetcode.cn/circle/discuss/tXLS3i/) 中的「**§5.3 约束划分个数**」是一个套路：

把数组分成恰好 $k$ 个连续子数组，计算与这些子数组有关的最优值。

一般定义 $f[i][j]$ 表示把长为 $j$ 的前缀 $\textit{nums}[:j]$ 分成 $i$ 个连续子数组所得到的最优解，本题是最大子数组异或和的最小值。

枚举 $\textit{nums}[:j]$ 中的最后一个子数组的左端点 $L$，从 $f[i-1][L]$ 转移到 $f[i][j]$，并考虑 $\textit{nums}[L:j]$ 对最优解的影响。

对于本题，计算 $f[i][j]$ 时，我们可以倒着枚举最后一个子数组的左端点 $L=j-1,j-2,\ldots,i-1$（$i-1$ 是因为前面还有 $i-1$ 个长度至少为 $1$ 的子数组），为什么要倒着枚举？这样可以一边枚举，一边 $\mathcal{O}(1)$ 计算子数组异或和。

分割出这个子数组后，问题变成把长为 $L$ 的前缀 $\textit{nums}[:L]$ 分成 $i-1$ 个连续子数组所得到的最优解（最大子数组异或和的最小值），即 $f[i-1][L]$。

设子数组 $[L,j)$ 的异或和为 $s(L,j)$，那么前缀 $\textit{nums}[:j]$ 的子数组异或和的最大值就是 

$$
\max(f[i-1][L], s(L,j))
$$

枚举 $L$，所有情况取最小值，**状态转移方程**为

$$
f[i][j] = \min_{L=i-1}^{j-1} \max(f[i-1][L], s(L,j))
$$

**初始值**：$f[0][0] = 0$，$f[0][j>0] = \infty$。后者是因为无法把一个非空数组分成 $0$ 个子数组。

**答案**：$f[k][n]$。

具体请看 [视频讲解](https://www.bilibili.com/video/BV1j6gZzqEdc/?t=17m37s)，欢迎点赞关注~

## 优化前

```py [sol-Python3]
# 手写 min max 更快
min = lambda a, b: b if b < a else a
max = lambda a, b: b if b > a else a

class Solution:
    def minXor(self, nums: List[int], k: int) -> int:
        n = len(nums)
        f = [[inf] * (n + 1) for _ in range(k + 1)]
        f[0][0] = 0
        for i in range(1, k + 1):
            # 前后每个子数组长度至少是 1，预留空间给这些子数组
            for j in range(i, n - (k - i) + 1):
                s = 0
                # 枚举所有分割方案，取最小值
                for l in range(j - 1, i - 2, -1):
                    s ^= nums[l]
                    # 对于单个分割方案，子数组异或和要取最大值
                    f[i][j] = min(f[i][j], max(f[i - 1][l], s))
        return f[k][n]
```

```py [sol-Python3 记忆化搜索]
# 手写 min max 更快
min = lambda a, b: b if b < a else a
max = lambda a, b: b if b > a else a

class Solution:
    def minXor(self, nums: List[int], k: int) -> int:
        # 把 nums[0] 到 nums[j] 分割成 i 个子数组，返回所有分割方案的 max(子数组异或和) 的最小值
        @cache
        def dfs(i: int, j: int) -> int:
            if i == 0:
                return 0 if j < 0 else inf
            res = inf
            s = 0
            # 枚举所有分割方案，取最小值
            # 前面还有 i-1 个子数组，每个子数组长度至少是 1，所以至少留 i-1 个元素给前面
            for l in range(j, i - 2, -1):
                s ^= nums[l]
                if s >= res:
                    continue  # 最优性剪枝：res 不可能变小
                # 对于单个分割方案，子数组异或和要取最大值
                res = min(res, max(dfs(i - 1, l - 1), s))
            return res
        return dfs(k, len(nums) - 1)
```

```java [sol-Java]
class Solution {
    public int minXor(int[] nums, int k) {
        int n = nums.length;
        int[][] f = new int[k + 1][n + 1];
        Arrays.fill(f[0], Integer.MAX_VALUE);
        f[0][0] = 0;
        for (int i = 1; i <= k; i++) {
            // 前后每个子数组长度至少是 1，预留空间给这些子数组
            for (int j = i; j <= n - (k - i); j++) {
                int res = Integer.MAX_VALUE;
                int s = 0;
                // 枚举所有分割方案，取最小值
                for (int l = j - 1; l >= i - 1; l--) {
                    s ^= nums[l];
                    // 对于单个分割方案，子数组异或和要取最大值
                    res = Math.min(res, Math.max(f[i - 1][l], s));
                }
                f[i][j] = res;
            }
        }
        return f[k][n];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minXor(vector<int>& nums, int k) {
        int n = nums.size();
        vector f(k + 1, vector<int>(n + 1, INT_MAX));
        f[0][0] = 0;
        for (int i = 1; i <= k; i++) {
            // 前后每个子数组长度至少是 1，预留空间给这些子数组
            for (int j = i; j <= n - (k - i); j++) {
                int s = 0;
                // 枚举所有分割方案，取最小值
                for (int l = j - 1; l >= i - 1; l--) {
                    s ^= nums[l];
                    // 对于单个分割方案，子数组异或和要取最大值
                    f[i][j] = min(f[i][j], max(f[i - 1][l], s));
                }
            }
        }
        return f[k][n];
    }
};
```

```go [sol-Go]
func minXor(nums []int, k int) int {
	n := len(nums)
	f := make([][]int, k+1)
	for i := range f {
		f[i] = make([]int, n+1)
	}
	for i := 1; i <= n; i++ {
		f[0][i] = math.MaxInt
	}
	for i := 1; i <= k; i++ {
		// 前后每个子数组长度至少是 1，预留空间给这些子数组
		for j := i; j <= n-(k-i); j++ {
			res := math.MaxInt
			s := 0
			// 枚举所有分割方案，取最小值
			for l := j - 1; l >= i-1; l-- {
				s ^= nums[l]
				// 对于单个分割方案，子数组异或和要取最大值
				res = min(res, max(f[i-1][l], s))
			}
			f[i][j] = res
		}
	}
	return f[k][n]
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(k(n-k)^2)$，其中 $n$ 是 $\textit{nums}$ 的长度。求导可知，当 $k=\dfrac{n}{3}$ 时取到最大值 $\dfrac{4n^3}{27}$。
- 空间复杂度：$\mathcal{O}(kn)$。

## 空间优化

和 0-1 背包一样，把第一维去掉，内层循环倒着计算，避免状态被覆盖。原理见 [0-1 背包 完全背包【基础算法精讲 18】](https://www.bilibili.com/video/BV16Y411v7Y6/)。

```py [sol-Python3]
# 另见【Python3 更快写法】，避免函数调用，减少赋值次数
min = lambda a, b: b if b < a else a
max = lambda a, b: b if b > a else a

class Solution:
    def minXor(self, nums: List[int], k: int) -> int:
        n = len(nums)
        f = [0] + [inf] * n
        for i in range(1, k + 1):
            for j in range(n - (k - i), i - 1, -1):
                res = inf
                s = 0
                for l in range(j - 1, i - 2, -1):
                    s ^= nums[l]
                    res = min(res, max(f[l], s))
                f[j] = res
        return f[n]
```

```py [sol-Python3 更快写法]
class Solution:
    def minXor(self, nums: List[int], k: int) -> int:
        n = len(nums)
        f = [0] + [inf] * n
        for i in range(1, k + 1):
            for j in range(n - (k - i), i - 1, -1):
                res = inf
                s = 0
                for l in range(j - 1, i - 2, -1):
                    s ^= nums[l]
                    v = f[l]
                    if s > v: v = s
                    if v < res: res = v
                f[j] = res
        return f[n]
```

```java [sol-Java]
class Solution {
    public int minXor(int[] nums, int k) {
        int n = nums.length;
        int[] f = new int[n + 1];
        Arrays.fill(f, Integer.MAX_VALUE);
        f[0] = 0;
        for (int i = 1; i <= k; i++) {
            for (int j = n - (k - i); j >= i; j--) {
                int res = Integer.MAX_VALUE;
                int s = 0;
                for (int l = j - 1; l >= i - 1; l--) {
                    s ^= nums[l];
                    res = Math.min(res, Math.max(f[l], s));
                }
                f[j] = res;
            }
        }
        return f[n];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minXor(vector<int>& nums, int k) {
        int n = nums.size();
        vector<int> f(n + 1, INT_MAX);
        f[0] = 0;
        for (int i = 1; i <= k; i++) {
            for (int j = n - (k - i); j >= i; j--) {
                int res = INT_MAX;
                int s = 0;
                for (int l = j - 1; l >= i - 1; l--) {
                    s ^= nums[l];
                    res = min(res, max(f[l], s));
                }
                f[j] = res;
            }
        }
        return f[n];
    }
};
```

```go [sol-Go]
func minXor(nums []int, k int) int {
	n := len(nums)
	f := make([]int, n+1)
	for i := 1; i <= n; i++ {
		f[i] = math.MaxInt
	}
	for i := 1; i <= k; i++ {
		for j := n - (k - i); j >= i; j-- {
			res := math.MaxInt
			s := 0
			for l := j - 1; l >= i-1; l-- {
				s ^= nums[l]
				res = min(res, max(f[l], s))
			}
			f[j] = res
		}
	}
	return f[n]
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(k(n-k)^2)$，其中 $n$ 是 $\textit{nums}$ 的长度。求导可知，当 $k=\dfrac{n}{3}$ 时取到最大值 $\dfrac{4n^3}{27}$。
- 空间复杂度：$\mathcal{O}(n)$。

## 附：二分做法

设 $\textit{nums}$ 的前缀异或和数组为 $s$。

二分子数组异或和的上界 $\textit{upper}$，转化成一个有向无环图（DAG）上的恰好移动 $k$ 步问题：

- 如果 $s[i]\oplus s[j]\le \textit{upper}$，连一条从 $i$ 到 $j$ 的有向边。
- 问：是否存在一条从 $n$ 到 $0$ 的路径，恰好有 $k$ 条边？

DP，定义 $\textit{dfs}(i)$ 表示从 $i$ 到 $0$ 的移动步数**集合**。比如从 $5$ 到 $0$ 可以移动 $1,3,4$ 步，那么 $\textit{dfs}(5) = \{1,3,4\}$。

如果可以从 $i$ 移动到 $j$，那么把 $\textit{dfs}(j)$ 中的每个数加一，并到 $\textit{dfs}(i)$ 中。

递归边界：$\textit{dfs}(0)=\{0\}$。

递归入口：$\textit{dfs}(n)$。如果 $k$ 在集合 $\textit{dfs}(n)$ 中，则说明存在一条从 $n$ 到 $0$ 的路径，恰好有 $k$ 条边。

代码实现时，集合用二进制表示，集合的运算用位运算表示，具体请看 [从集合论到位运算，常见位运算技巧分类总结！](https://leetcode.cn/circle/discuss/CaOJ45/)

```py [sol-Python3]
class Solution:
    def minXor(self, nums: List[int], k: int) -> int:
        s = list(accumulate(nums, xor, initial=0))  # 前缀异或和

        def check(upper: int) -> bool:
            @cache
            def dfs(i: int) -> int:
                if i == 0:
                    return 1
                res = 0
                for j in range(i):
                    if s[i] ^ s[j] <= upper:
                        res |= dfs(j) << 1  # 走一步
                return res
            return dfs(len(nums)) >> k & 1 > 0  # 可以走恰好 k 步

        m = max(nums).bit_length()
        return bisect_left(range((1 << m) - 1), True, key=check)
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(\frac{n^3}{w}\log U)$，其中 $n$ 是 $\textit{nums}$ 的长度，$w=32$ 或 $64$，$U=\max(\textit{nums})$。
- 空间复杂度：$\mathcal{O}(\frac{n^2}{w})$。

## 附：Dijkstra 做法

计算从 $(n,k)$ 到 $(0,0)$ 的最短路，建图方式同上，边权为子数组异或和。

这里的「最短路」不是基于加法，而是基于 $\max$ 计算。

```py [sol-Python3]
max = lambda a, b: b if b > a else a

class Solution:
    def minXor(self, nums: List[int], k: int) -> int:
        s = list(accumulate(nums, xor, initial=0))  # 前缀异或和

        h = [(0, len(nums), k)]
        dis = [[inf] * (k + 1) for _ in s]
        while True:
            d, i, k = heappop(h)
            if d > dis[i][k]:
                continue
            if k == 0:
                if i == 0:
                    return d
                continue
            for j in range(i):
                new_d = max(d, s[i] ^ s[j])
                if new_d < dis[j][k - 1]:
                    dis[j][k - 1] = new_d
                    heappush(h, (new_d, j, k - 1))
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(M\log M)$，其中 $M=n^2k$。
- 空间复杂度：$\mathcal{O}(M)$。

## 专题训练

见下面动态规划题单的「**§5.3 约束划分个数**」。

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

本题来自 `三、最短路 / §3.1 单源最短路：Dijkstra 算法`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `三、最短路 / §3.1 单源最短路：Dijkstra 算法`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
