# 3621. 位计数深度为 K 的整数数目 I

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/number-of-integers-with-popcount-depth-equal-to-k-i/
- 题目 slug：`number-of-integers-with-popcount-depth-equal-to-k-i`
- 来源专题：动态规划
- 来源分类路径：十、数位 DP / §10.1 统计合法元素的数目
- 难度分：3352
- 外部题解来源：https://leetcode.cn/problems/number-of-integers-with-popcount-depth-equal-to-k-i/solutions/3728362/shu-wei-dpzuo-fa-tong-3352-ti-pythonjava-fjti/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[数位 DP，做法同 3352 题（Python/Java/C++/Go）](https://leetcode.cn/problems/number-of-integers-with-popcount-depth-equal-to-k-i/solutions/3728362/shu-wei-dpzuo-fa-tong-3352-ti-pythonjava-fjti/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`shu-wei-dpzuo-fa-tong-3352-ti-pythonjava-fjti`
- topic id：`3728362`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

本题和 [3352. 统计小于 N 的 K 可约简整数](https://leetcode.cn/problems/count-k-reducible-numbers-less-than-n/) 几乎一样。

如果 $k=0$，那么只有 $x=1$ 满足要求，返回 $1$。

如果 $k=1$，那么只有 $x=2^1,2^2,\ldots,2^{m-1}$ 满足要求，其中 $m$ 是 $n$ 的二进制长度。这一共有 $m-1$ 个数。

如果 $k\ge 2$，计算方法同 3352 题，区别：

- 3352 题是小于 $n$，本题是小于等于 $n$。DFS 中无需判断是否严格小于 $n$。递归入口 $i$ 可以等于 $m$。
- 3352 题是小于等于 $k$，本题是恰好等于 $k$。递归入口的判断改成恰好等于 $k$。
- 本题无需取模。

```py [sol-Python3]
class Solution:
    def popcountDepth(self, n: int, k: int) -> int:
        if k == 0:
            return 1

        # 注：也可以不转成字符串，下面 dfs 用位运算取出 n 的第 i 位
        # 但转成字符串的通用性更好
        s = list(map(int, bin(n)[2:]))
        m = len(s)
        if k == 1:
            return m - 1

        @cache
        def dfs(i: int, left1: int, is_limit: bool) -> int:
            if i == m:
                return 0 if left1 else 1
            up = s[i] if is_limit else 1
            res = 0
            for d in range(min(up, left1) + 1):
                res += dfs(i + 1, left1 - d, is_limit and d == up)
            return res

        ans = 0
        f = [0] * (m + 1)
        for i in range(1, m + 1):
            f[i] = f[i.bit_count()] + 1
            if f[i] == k:
                # 计算有多少个二进制数恰好有 i 个 1
                ans += dfs(0, i, True)
        return ans
```

```java [sol-Java]
class Solution {
    public long popcountDepth(long n, int k) {
        if (k == 0) {
            return 1;
        }

        // 注：也可以不转成字符串，下面 dfs 用位运算取出 n 的第 i 位
        // 但转成字符串的通用性更好
        char[] s = Long.toBinaryString(n).toCharArray();
        int m = s.length;
        if (k == 1) {
            return m - 1;
        }

        long[][] memo = new long[m][m + 1];
        for (long[] row : memo) {
            Arrays.fill(row, -1);
        }

        long ans = 0;
        int[] f = new int[m + 1];
        for (int i = 1; i <= m; i++) {
            f[i] = f[Integer.bitCount(i)] + 1;
            if (f[i] == k) {
                // 计算有多少个二进制数恰好有 i 个 1
                ans += dfs(0, i, true, s, memo);
            }
        }
        return ans;
    }

    private long dfs(int i, int left1, boolean isLimit, char[] s, long[][] memo) {
        if (i == s.length) {
            return left1 == 0 ? 1 : 0;
        }
        if (!isLimit && memo[i][left1] != -1) {
            return memo[i][left1];
        }

        int up = isLimit ? s[i] - '0' : 1;
        long res = 0;
        for (int d = 0; d <= Math.min(up, left1); d++) {
            res += dfs(i + 1, left1 - d, isLimit && d == up, s, memo);
        }

        if (!isLimit) {
            memo[i][left1] = res;
        }
        return res;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long popcountDepth(long long n, int k) {
        if (k == 0) {
            return 1;
        }

        int m = bit_width((uint64_t) n);
        if (k == 1) {
            return m - 1;
        }

        vector memo(m, vector<long long>(m + 1, -1));
        auto dfs = [&](this auto& dfs, int i, int left1, bool is_limit) -> long long {
            if (i < 0) {
                return left1 == 0;
            }
            if (!is_limit && memo[i][left1] != -1) {
                return memo[i][left1];
            }

            // 直接用位运算取出 n 的第 i 位
            int up = is_limit ? n >> i & 1 : 1;
            long long res = 0;
            for (int d = 0; d <= min(up, left1); d++) {
                res += dfs(i - 1, left1 - d, is_limit && d == up);
            }

            if (!is_limit) {
                memo[i][left1] = res;
            }
            return res;
        };

        long long ans = 0;
        vector<int> f(m + 1);
        for (uint32_t i = 1; i <= m; i++) {
            f[i] = f[popcount(i)] + 1;
            if (f[i] == k) {
                // 计算有多少个二进制数恰好有 i 个 1
                ans += dfs(m - 1, i, true);
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func popcountDepth(n int64, k int) (ans int64) {
	if k == 0 {
		return 1
	}

	// 注：也可以不转成字符串，下面 dfs 用位运算取出 n 的第 i 位 
	// 但转成字符串的通用性更好
	s := strconv.FormatInt(n, 2)
	m := len(s)
	if k == 1 {
		return int64(m - 1)
	}

	memo := make([][]int64, m)
	for i := range memo {
		memo[i] = make([]int64, m+1)
		for j := range memo[i] {
			memo[i][j] = -1
		}
	}

	var dfs func(int, int, bool) int64
	dfs = func(i, left1 int, isLimit bool) (res int64) {
		if i == m {
			if left1 == 0 {
				return 1
			}
			return
		}
		if !isLimit {
			p := &memo[i][left1]
			if *p >= 0 {
				return *p
			}
			defer func() { *p = res }()
		}

		up := 1
		if isLimit {
			up = int(s[i] - '0')
		}
		for d := 0; d <= min(up, left1); d++ {
			res += dfs(i+1, left1-d, isLimit && d == up)
		}
		return
	}

	f := make([]int, m+1)
	for i := 1; i <= m; i++ {
		f[i] = f[bits.OnesCount(uint(i))] + 1
		if f[i] == k {
			// 计算有多少个二进制数恰好有 i 个 1
			ans += dfs(0, i, true)
		}
	}
	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(\log^2 n)$。
- 空间复杂度：$\mathcal{O}(\log^2 n)$。

## 专题训练

见下面动态规划题单的「**十、数位 DP**」。

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

本题来自 `十、数位 DP / §10.1 统计合法元素的数目`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `十、数位 DP / §10.1 统计合法元素的数目`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
