# 3363. 最多可收集的水果数目

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/find-the-maximum-number-of-fruits-collected/
- 题目 slug：`find-the-maximum-number-of-fruits-collected`
- 来源专题：动态规划
- 来源分类路径：二、网格图 DP / §2.2 进阶
- 难度分：2200
- 外部题解来源：https://leetcode.cn/problems/find-the-maximum-number-of-fruits-collected/solutions/2998667/nao-jin-ji-zhuan-wan-wang-ge-tu-dppython-gjcm/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[脑筋急转弯 + 网格图 DP（Python/Java/C++/Go）](https://leetcode.cn/problems/find-the-maximum-number-of-fruits-collected/solutions/2998667/nao-jin-ji-zhuan-wan-wang-ge-tu-dppython-gjcm/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`nao-jin-ji-zhuan-wan-wang-ge-tu-dppython-gjcm`
- topic id：`2998667`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

由于从左上角出发的小朋友只能移动 $n-1$ 次，所以他的走法有且仅有一种：主对角线。其余走法一定会超过 $n-1$ 步。

对于从右上角出发的小朋友，他不能穿过主对角线走到另一侧（不然就没法走到右下角），且同一个格子的水果不能重复收集。于是问题变成：

- 从右上角 $(0,n-1)$ 出发，在不访问主对角线的情况下，走到 $(n-2,n-1)$，也就是右下角的上面那个格子，所能收集到的水果总数的最大值。

做法类似 [931. 下降路径最小和](https://leetcode.cn/problems/minimum-falling-path-sum/)，请看 [我的题解](https://leetcode.cn/problems/minimum-falling-path-sum/solutions/2341851/cong-di-gui-dao-di-tui-jiao-ni-yi-bu-bu-2cwkb/)。

对于从左下角出发的小朋友，我们可以把矩阵按照主对角线翻转，就可以复用同一套代码逻辑了。

代码实现时，由于我们是倒着走的（为了方便翻译成递推），小朋友不能一直往左上走，不然没法走到右上角。所以要限制小朋友不能太靠左，即保证 $j\ge n-1-i$。这是因为从 $(0,n-1)$ 往左下的这条线满足 $i+j=n-1$，不能越过这条线，即 $i+j\ge n-1$，也就是 $j\ge n-1-i$。

本题由于元素值均非负，可以在出界时返回 $0$。

具体请看 [视频讲解](https://www.bilibili.com/video/BV1uzBxYoEJC/?t=13m12s)，欢迎点赞关注~

## 一、记忆化搜索

```py [sol-Python3]
class Solution:
    def maxCollectedFruits(self, fruits: List[List[int]]) -> int:
        n = len(fruits)

        @cache
        def dfs(i: int, j: int) -> int:
            if not (n - 1 - i <= j < n):
                return -inf
            if i == 0:
                return fruits[i][j]
            return max(dfs(i - 1, j - 1), dfs(i - 1, j), dfs(i - 1, j + 1)) + fruits[i][j]

        # 从 (0, 0) 出发的小朋友
        ans = sum(row[i] for i, row in enumerate(fruits))

        # 从 (0, n - 1) 出发的小朋友（倒着走）
        ans += dfs(n - 2, n - 1)  # 从下往上走，方便 1:1 翻译成递推
        dfs.cache_clear()

        # 从 (n - 1, 0) 出发的小朋友（按照主对角线翻转，然后倒着走）
        fruits = list(zip(*fruits))  # 按照主对角线翻转
        return ans + dfs(n - 2, n - 1)
```

```java [sol-Java]
class Solution {
    public int maxCollectedFruits(int[][] fruits) {
        int n = fruits.length;
        int[][] memo = new int[n][n];
        for (int[] row : memo) {
            Arrays.fill(row, -1); // -1 表示没有计算过
        }

        int ans = 0;
        // 从 (0, 0) 出发的小朋友
        for (int i = 0; i < n; i++) {
            ans += fruits[i][i];
        }

        // 从 (0, n - 1) 出发的小朋友（倒着走）
        // 从下往上走，方便 1:1 翻译成递推
        ans += dfs(n - 2, n - 1, fruits, memo);

        // 从 (n - 1, 0) 出发的小朋友（按照主对角线翻转，然后倒着走）
        // 把下三角形中的数据填到上三角形中
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < i; j++) {
                fruits[j][i] = fruits[i][j];
            }
        }
        for (int[] row : memo) {
            Arrays.fill(row, -1); // -1 表示没有计算过
        }
        return ans + dfs(n - 2, n - 1, fruits, memo);
    }

    private int dfs(int i, int j, int[][] fruits, int[][] memo) {
        int n = fruits.length;
        if (j < n - 1 - i || j >= n) {
            return 0;
        }
        if (i == 0) {
            return fruits[i][j];
        }
        if (memo[i][j] != -1) { // 之前计算过
            return memo[i][j];
        }
        int res1 = dfs(i - 1, j - 1, fruits, memo);
        int res2 = dfs(i - 1, j, fruits, memo);
        int res3 = dfs(i - 1, j + 1, fruits, memo);
        int res = Math.max(Math.max(res1, res2), res3) + fruits[i][j];
        return memo[i][j] = res; // 记忆化
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxCollectedFruits(vector<vector<int>>& fruits) {
        int n = fruits.size();
        vector memo(n, vector<int>(n, -1)); // -1 表示没有计算过
        auto dfs = [&](this auto&& dfs, int i, int j) -> int {
            if (j < n - 1 - i || j >= n) {
                return 0;
            }
            if (i == 0) {
                return fruits[i][j];
            }
            int& res = memo[i][j]; // 注意这里是引用
            if (res != -1) { // 之前计算过
                return res;
            }
            return res = max({dfs(i - 1, j - 1), dfs(i - 1, j), dfs(i - 1, j + 1)}) + fruits[i][j];
        };

        int ans = 0;
        // 从 (0, 0) 出发的小朋友
        for (int i = 0; i < n; i++) {
            ans += fruits[i][i];
        }

        // 从 (0, n - 1) 出发的小朋友（倒着走）
        ans += dfs(n - 2, n - 1); // 从下往上走，方便 1:1 翻译成递推

        // 从 (n - 1, 0) 出发的小朋友（按照主对角线翻转，然后倒着走）
        // 把下三角形中的数据填到上三角形中
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < i; j++) {
                fruits[j][i] = fruits[i][j];
            }
        }
        ranges::fill(memo, vector(n, -1));
        return ans + dfs(n - 2, n - 1);
    }
};
```

```go [sol-Go]
func maxCollectedFruits(fruits [][]int) (ans int) {
	n := len(fruits)
	memo := make([][]int, n)
	for i := range memo {
		memo[i] = make([]int, n)
		for j := range memo[i] {
			memo[i][j] = -1 // -1 表示没有计算过
		}
	}

	var dfs func(int, int) int
	dfs = func(i, j int) int {
		if j < n-1-i || j >= n {
			return 0
		}
		if i == 0 {
			return fruits[i][j]
		}
		p := &memo[i][j]
		if *p < 0 { // 没有计算过
			*p = max(dfs(i-1, j-1), dfs(i-1, j), dfs(i-1, j+1)) + fruits[i][j]
		}
		return *p
	}

	// 从 (0, 0) 出发的小朋友
	for i, row := range fruits {
		ans += row[i]
	}

	// 从 (0, n-1) 出发的小朋友（倒着走）
	ans += dfs(n-2, n-1) // 从下往上走，方便 1:1 翻译成递推

	// 从 (n-1, 0) 出发的小朋友（按照主对角线翻转，然后倒着走）
	// 把下三角形中的数据填到上三角形中
	for i := range fruits {
		for j := range i {
			fruits[j][i] = fruits[i][j]
		}
	}
	for i := range memo {
		for j := range memo[i] {
			memo[i][j] = -1 // -1 表示没有计算过
		}
	}
	return ans + dfs(n-2, n-1)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2)$，其中 $n$ 是 $\textit{fruits}$ 的长度。
- 空间复杂度：$\mathcal{O}(n^2)$。

## 二、1:1 翻译成递推

由于起点是 $(0,n-1)$，即使每一步都往左下走，$i+j$ 也不会低于 $n-1$，所以 $j\ge n-1-i$。

由于终点是 $(n-2,n-1)$，即使从终点倒着，每一步都往左上走，$j$ 也始终大于 $i$。

所以 $j$ 可以从

$$
\max(n-1-i,i+1)
$$

开始枚举。

```py [sol-Python3]
class Solution:
    def maxCollectedFruits(self, fruits: List[List[int]]) -> int:
        def dp(fruits: List[List[int]]) -> int:
            n = len(fruits)
            f = [[0] * (n + 1) for _ in range(n - 1)]
            f[0][n - 1] = fruits[0][-1]
            for i in range(1, n - 1):
                for j in range(max(n - 1 - i, i + 1), n):
                    f[i][j] = max(f[i - 1][j - 1], f[i - 1][j], f[i - 1][j + 1]) + fruits[i][j]
            return f[-1][n - 1]

        ans = sum(row[i] for i, row in enumerate(fruits))
        return ans + dp(fruits) + dp(list(zip(*fruits)))
```

```java [sol-Java]
class Solution {
    public int maxCollectedFruits(int[][] fruits) {
        int n = fruits.length;
        int ans = 0;
        // 从 (0, 0) 出发的小朋友
        for (int i = 0; i < n; i++) {
            ans += fruits[i][i];
        }

        // 从 (0, n - 1) 出发的小朋友
        ans += dp(fruits);

        // 从 (n - 1, 0) 出发的小朋友（按照主对角线翻转）
        // 把下三角形中的数据填到上三角形中
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < i; j++) {
                fruits[j][i] = fruits[i][j];
            }
        }
        return ans + dp(fruits);
    }

    int dp(int[][] fruits) {
        int n = fruits.length;
        int[][] f = new int[n - 1][n + 1];
        f[0][n - 1] = fruits[0][n - 1];
        for (int i = 1; i < n - 1; i++) {
            for (int j = Math.max(n - 1 - i, i + 1); j < n; j++) {
                f[i][j] = Math.max(Math.max(f[i - 1][j - 1], f[i - 1][j]), f[i - 1][j + 1]) + fruits[i][j];
            }
        }
        return f[n - 2][n - 1];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxCollectedFruits(vector<vector<int>>& fruits) {
        int n = fruits.size();
        auto dp = [&]() -> int {
            vector f(n - 1, vector<int>(n + 1));
            f[0][n - 1] = fruits[0][n - 1];
            for (int i = 1; i < n - 1; i++) {
                for (int j = max(n - 1 - i, i + 1); j < n; j++) {
                    f[i][j] = max({f[i - 1][j - 1], f[i - 1][j], f[i - 1][j + 1]}) + fruits[i][j];
                }
            }
            return f[n - 2][n - 1];
        };

        int ans = 0;
        // 从 (0, 0) 出发的小朋友
        for (int i = 0; i < n; i++) {
            ans += fruits[i][i];
        }

        // 从 (0, n - 1) 出发的小朋友
        ans += dp();

        // 从 (n - 1, 0) 出发的小朋友（按照主对角线翻转）
        // 把下三角形中的数据填到上三角形中
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < i; j++) {
                fruits[j][i] = fruits[i][j];
            }
        }
        return ans + dp();
    }
};
```

```go [sol-Go]
func maxCollectedFruits(fruits [][]int) (ans int) {
	n := len(fruits)
	f := make([][]int, n-1)
	for i := range f {
		f[i] = make([]int, n+1)
	}
	dp := func() int {
		f[0][n-1] = fruits[0][n-1]
		for i := 1; i < n-1; i++ {
			for j := max(n-1-i, i+1); j < n; j++ {
				f[i][j] = max(f[i-1][j-1], f[i-1][j], f[i-1][j+1]) + fruits[i][j]
			}
		}
		return f[n-2][n-1]
	}

	// 从 (0, 0) 出发的小朋友
	for i, row := range fruits {
		ans += row[i]
	}

	// 从 (0, n - 1) 出发的小朋友
	ans += dp()

	// 从 (n - 1, 0) 出发的小朋友（按照主对角线翻转）
	// 把下三角形中的数据填到上三角形中
	for i := range fruits {
		for j := range i {
			fruits[j][i] = fruits[i][j]
		}
	}
	return ans + dp()
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2)$，其中 $n$ 是 $\textit{fruits}$ 的长度。
- 空间复杂度：$\mathcal{O}(n^2)$。

**注**：用滚动数组可以做到 $\mathcal{O}(n)$ 的空间复杂度。也可以原地修改 $\textit{fruits}$，做到 $\mathcal{O}(1)$ 空间复杂度。

## 专题训练

见下面动态规划题单的「**二、网格图 DP**」。

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

## 本地原创解析

### 1. 题意重述

本题来自 `二、网格图 DP / §2.2 进阶`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、网格图 DP / §2.2 进阶`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
