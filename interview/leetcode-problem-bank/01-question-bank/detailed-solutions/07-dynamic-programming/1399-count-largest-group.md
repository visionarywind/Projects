# 1399. 统计最大组的数目

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/count-largest-group/
- 题目 slug：`count-largest-group`
- 来源专题：动态规划
- 来源分类路径：十、数位 DP / §10.1 统计合法元素的数目
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/count-largest-group/solutions/3648666/liang-chong-fang-fa-bao-li-mei-ju-shu-we-g00u/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：暴力枚举 / 数位 DP（Python/Java/C++/Go）](https://leetcode.cn/problems/count-largest-group/solutions/3648666/liang-chong-fang-fa-bao-li-mei-ju-shu-we-g00u/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`liang-chong-fang-fa-bao-li-mei-ju-shu-we-g00u`
- topic id：`3648666`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 方法一：暴力枚举

枚举 $[1,n]$ 中的整数 $i$，计算 $i$ 的数位和 $\textit{ds}$，用一个哈希表（或者数组）$\textit{cnt}$ 统计 $\textit{ds}$ 及其出现次数。

比如示例 1 的 `cnt = {1: 2, 2: 2, 3: 2, 4: 2, 5: 1, 6: 1, 7: 1, 8: 1, 9: 1}`。

统计 $\textit{cnt}$ 中的最大 value（出现次数）的出现次数。比如示例 1 的最大 value 为 $2$，我们需要统计 $\textit{cnt}$ 中有多少个 value 等于 $2$，这一共有 $4$ 个。

```py [sol-Python3]
class Solution:
    def countLargestGroup(self, n: int) -> int:
        cnt = defaultdict(int)
        max_cnt = ans = 0
        for i in range(1, n + 1):
            ds = sum(map(int, str(i)))
            cnt[ds] += 1
            # 维护 max_cnt 以及 max_cnt 的出现次数
            if cnt[ds] > max_cnt:
                max_cnt = cnt[ds]
                ans = 1
            elif cnt[ds] == max_cnt:
                ans += 1
        return ans
```

```py [sol-Python3 列表]
class Solution:
    def countLargestGroup(self, n: int) -> int:
        m = len(str(n))
        cnt = [0] * (m * 9 + 1)  # 数位和 <= 9m
        max_cnt = ans = 0
        for i in range(1, n + 1):
            ds = sum(map(int, str(i)))
            cnt[ds] += 1
            # 维护 max_cnt 以及 max_cnt 的出现次数
            if cnt[ds] > max_cnt:
                max_cnt = cnt[ds]
                ans = 1
            elif cnt[ds] == max_cnt:
                ans += 1
        return ans
```

```java [sol-Java]
class Solution {
    public int countLargestGroup(int n) {
        int m = String.valueOf(n).length();
        int[] cnt = new int[m * 9 + 1]; // 数位和 <= 9m
        int maxCnt = 0;
        int ans = 0;
        for (int i = 1; i <= n; i++) {
            int ds = calcDigitSum(i);
            cnt[ds]++;
            // 维护 maxCnt 以及 maxCnt 的出现次数
            if (cnt[ds] > maxCnt) {
                maxCnt = cnt[ds];
                ans = 1;
            } else if (cnt[ds] == maxCnt) {
                ans++;
            }
        }
        return ans;
    }

    private int calcDigitSum(int num) {
        int ds = 0;
        while (num > 0) {
            ds += num % 10;
            num /= 10;
        }
        return ds;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int countLargestGroup(int n) {
        int m = to_string(n).size();
        vector<int> cnt(m * 9 + 1); // 数位和 <= 9m
        int max_cnt = 0, ans = 0;
        for (int i = 1; i <= n; i++) {
            int ds = 0;
            for (int x = i; x > 0; x /= 10) {
                ds += x % 10;
            }
            cnt[ds]++;
            // 维护 max_cnt 以及 max_cnt 的出现次数
            if (cnt[ds] > max_cnt) {
                max_cnt = cnt[ds];
                ans = 1;
            } else if (cnt[ds] == max_cnt) {
                ans++;
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func countLargestGroup(n int) (ans int) {
	m := len(strconv.Itoa(n))
	cnt := make([]int, m*9+1) // 数位和 <= 9m
	maxCnt := 0
	for i := 1; i <= n; i++ {
		ds := 0
		for x := i; x > 0; x /= 10 {
			ds += x % 10
		}
		cnt[ds]++
		// 维护 maxCnt 以及 maxCnt 的出现次数
		if cnt[ds] > maxCnt {
			maxCnt = cnt[ds]
			ans = 1
		} else if cnt[ds] == maxCnt {
			ans++
		}
	}
	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n)$。每个数字需要 $\mathcal{O}(\log n)$ 的时间计算数位和。
- 空间复杂度：$\mathcal{O}(D\log n)$，其中 $D=9$。有 $\mathcal{O}(D\log n)$ 个不同的数位和。

## 方法二：数位 DP

如果 $n = 10^{18}$，方法一就超时了，怎么办？

**前置知识**：

[数位 DP v1.0 视频讲解](https://www.bilibili.com/video/BV1rS4y1s721/)（上界数位 DP，本题只需要 v1.0 写法）

[数位 DP v2.0 视频讲解](https://www.bilibili.com/video/BV1Fg4y1Q7wv/)（上下界数位 DP，本题不需要）

设 $n$ 的十进制字符串为 $s$，设 $s$ 的长度为 $m$。那么数位和至多为 $9m$（每一位都填 $9$）。

枚举**目标数位和** $\textit{target}=1,2,\ldots,9m$，计算 $[1,n]$ 中有多少个整数的数位和**恰好**等于 $\textit{target}$。这正是方法一哈希表（数组）中的 value 值，即 $\textit{cnt}[\textit{target}]$。

**状态定义**：$\textit{dfs}(i, \textit{left}, \textit{limitHigh})$ 表示构造第 $i$ 位及其之后数位的合法方案数，其余参数的含义为：

- $\textit{left}$ 表示剩余数位的数位和，也就是 $i$ 到 $m-1$ 填的数位之和必须**恰好**等于 $\textit{left}$。
- $\textit{limitHigh}$ 表示当前是否受到了 $n$ 的约束（我们要构造的数字不能超过 $n$）。若为真，则第 $i$ 位填入的数字至多为 $s[i]$，否则至多为 $9$，这个数记作 $\textit{hi}$。如果在受到约束的情况下填了 $\textit{hi}$，那么后续填入的数字仍会受到 $n$ 的约束。例如 $n=123$，那么 $i=0$ 填的是 $1$ 的话，$i=1$ 的这一位至多填 $2$。

> 注 1：为什么要把 $\textit{left}$ 定义成剩余的数位和，而不是已填数位之和？因为这样可以让 $\textit{target}=1,2,\ldots,9m$ 复用同一个记忆化搜索的结果。如果反过来，从 $0$ 递归到 $\textit{target}$，记忆化的内容就和 $\textit{target}$ 有关了，无法复用。
> 
> 注 2：本题前导零不影响答案，因为任何数加上（减去）$0$ 是不变的。

**状态转移**。枚举第 $i$ 位填数字 $d=0,1,\ldots,\min(\textit{hi},\textit{left})$，把 $i$ 加一，把 $\textit{left}$ 减少 $d$，继续向后递归（枚举右边的数位）。

**递归终点**：$i=n$ 时，如果 $\textit{left}=0$，说明我们成功构造出一个数位和**恰好**等于 $\textit{target}$ 的整数，返回 $1$，否则返回 $0$。（注意 $\textit{left}$ 是从 $\textit{target}$ 开始减少的）

**递归入口**：$\textit{dfs}(0, \textit{target}, \texttt{true})$，表示：

- 从最高位开始。
- 目标数位和为 $\textit{target}$。
- 一开始要受到 $n$ 的约束（否则就可以随意填了，这肯定不行）。

```py [sol-Python3]
class Solution:
    def countLargestGroup(self, n: int) -> int:
        s = list(map(int, str(n)))  # 避免在 dfs 中频繁调用 int()
        m = len(s)

        @cache
        def dfs(i: int, left: int, limit_high: bool) -> int:
            if i == m:
                return 1 if left == 0 else 0
            hi = s[i] if limit_high else 9  # 当前数位至多填 hi
            res = 0
            for d in range(min(hi, left) + 1):  # 枚举当前数位填 d
                res += dfs(i + 1, left - d, limit_high and d == hi)
            return res

        max_cnt = ans = 0
        for target in range(1, m * 9 + 1):  # 枚举目标数位和
            cnt = dfs(0, target, True)
            if cnt > max_cnt:
                max_cnt = cnt
                ans = 1
            elif cnt == max_cnt:
                ans += 1
        return ans
```

```java [sol-Java]
class Solution {
    public int countLargestGroup(int n) {
        char[] s = String.valueOf(n).toCharArray();
        int m = s.length;
        int[][] memo = new int[m][m * 9 + 1];
        for (int[] row : memo) {
            Arrays.fill(row, -1);
        }

        int maxCnt = 0;
        int ans = 0;
        for (int target = 1; target <= m * 9; target++) { // 枚举目标数位和
            int cnt = dfs(0, target, true, s, memo);
            if (cnt > maxCnt) {
                maxCnt = cnt;
                ans = 1;
            } else if (cnt == maxCnt) {
                ans++;
            }
        }
        return ans;
    }

    private int dfs(int i, int left, boolean limitHigh, char[] s, int[][] memo) {
        if (i == s.length) {
            return left == 0 ? 1 : 0;
        }
        if (!limitHigh && memo[i][left] != -1) {
            return memo[i][left];
        }

        int hi = limitHigh ? s[i] - '0' : 9; // 当前数位至多填 hi
        int res = 0;
        for (int d = 0; d <= Math.min(hi, left); d++) { // 枚举当前数位填 d
            res += dfs(i + 1, left - d, limitHigh && d == hi, s, memo);
        }

        if (!limitHigh) {
            memo[i][left] = res;
        }
        return res;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int countLargestGroup(int n) {
        string s = to_string(n);
        int m = s.size();
        vector memo(m, vector<int>(m * 9 + 1, -1));
        auto dfs = [&](this auto&& dfs, int i, int left, bool limit_high) -> int {
            if (i == m) {
                return left == 0;
            }
            if (!limit_high && memo[i][left] != -1) {
                return memo[i][left];
            }

            int hi = limit_high ? s[i] - '0' : 9; // 当前数位至多填 hi
            int res = 0;
            for (int d = 0; d <= min(hi, left); d++) { // 枚举当前数位填 d
                res += dfs(i + 1, left - d, limit_high && d == hi);
            }

            if (!limit_high) {
                memo[i][left] = res;
            }
            return res;
        };

        int max_cnt = 0, ans = 0;
        for (int target = 1; target <= m * 9; target++) { // 枚举目标数位和
            int cnt = dfs(0, target, true);
            if (cnt > max_cnt) {
                max_cnt = cnt;
                ans = 1;
            } else if (cnt == max_cnt) {
                ans++;
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func countLargestGroup(n int) (ans int) {
	s := strconv.Itoa(n)
	m := len(s)
	memo := make([][]int, m)
	for i := range memo {
		memo[i] = make([]int, m*9+1)
		for j := range memo[i] {
			memo[i][j] = -1
		}
	}

	var dfs func(i, left int, limitHigh bool) int
	dfs = func(i, left int, limitHigh bool) (res int) {
		if i == m {
			if left == 0 {
				return 1
			}
			return
		}

		if !limitHigh {
			p := &memo[i][left]
			if *p != -1 {
				return *p
			}
			defer func() { *p = res }()
		}

		// 当前数位至多填 hi
		hi := 9
		if limitHigh {
			hi = int(s[i] - '0')
		}

		for d := 0; d <= min(hi, left); d++ { // 枚举当前数位填 d
			res += dfs(i+1, left-d, limitHigh && d == hi)
		}
		return
	}

	maxCnt := 0
	for target := 1; target <= m*9; target++ { // 枚举目标数位和
		cnt := dfs(0, target, true)
		if cnt > maxCnt {
			maxCnt = cnt
			ans = 1
		} else if cnt == maxCnt {
			ans++
		}
	}
	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(D^2\log ^2 n)$，其中 $D=10$。由于每个状态只会计算一次，动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题 $i$ 有 $\mathcal{O}(\log n)$ 个，$\textit{left}$ 有 $\mathcal{O}(D\log n)$ 个，所以状态个数为 $\mathcal{O}(D\log^2 n)$，单个状态的计算时间为 $\mathcal{O}(D)$，所以总的时间复杂度为 $\mathcal{O}(D^2\log ^2 n)$。
- 空间复杂度：$\mathcal{O}(D\log ^2 n)$。保存多少状态，就需要多少空间。

## 相似题目

- [2719. 统计整数数目](https://leetcode.cn/problems/count-of-integers/)

更多相似题目，见下面动态规划题单的「**十、数位 DP**」。

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
