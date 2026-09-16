# 3639. 变为活跃状态的最小时间

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-time-to-activate-string/
- 题目 slug：`minimum-time-to-activate-string`
- 来源专题：二分算法
- 来源分类路径：二、二分答案 / §2.1 求最小 / 答疑
- 难度分：1853
- 外部题解来源：https://leetcode.cn/problems/minimum-time-to-activate-string/solutions/3741028/er-fen-da-an-pythonjavacgo-by-endlessche-6s8n/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：二分答案 / 逆向思维+双向链表（Python/Java/C++/Go）](https://leetcode.cn/problems/minimum-time-to-activate-string/solutions/3741028/er-fen-da-an-pythonjavacgo-by-endlessche-6s8n/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`er-fen-da-an-pythonjavacgo-by-endlessche-6s8n`
- topic id：`3741028`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 方法一：二分答案

由于答案（时间）越大，$s$ 中的星号越多，有效子串越多，越能够 $\ge k$；反之，$s$ 中的星号越少，有效子串越少，越无法 $\ge k$。据此，可以**二分猜答案**。关于二分算法的原理，请看 [二分查找 红蓝染色法【基础算法精讲 04】](https://www.bilibili.com/video/BV1AP41137w7/)。

现在问题转化成一个判定性问题：

- 给定 $t=m$，把 $\textit{order}$ 的前 $m+1$ 个下标对应的字母改成星号，有效子串的个数能否 $\ge k$？

如果可以，说明答案 $\le m$，否则答案 $>m$。

### 计算有效子串个数

**枚举有效子串的右端点**。

从左到右遍历 $s$，同时维护最近遍历到的星号的下标 $\textit{last}$。初始值 $\textit{last}=-1$。

设当前枚举到 $s[i]$，那么如下子串均包含至少一个星号：

$$
[0,i],[1,i],[2,i],\ldots,[\textit{last},i]
$$

这一共有 $\textit{last}+1$ 个，加入有效子串的个数。

### 细节

#### 1)

如果全部改成星号都无法满足要求，返回 $-1$。

全部改成星号时，所有子串都是有效子串，一共有 $\dfrac{n(n+1)}{2}$ 个。

#### 2)

下面代码采用开区间二分，这仅仅是二分的一种写法，使用闭区间或者半闭半开区间都是可以的，喜欢哪种写法就用哪种。

- 开区间左端点初始值：$-1$。没有星号，一定无法满足要求。
- 开区间右端点初始值：$n-1$。一定满足要求。注意在二分之前，我们特判了全部改成星号的情况。

对于开区间写法，简单来说 `check(mid) == true` 时更新的是谁，最后就返回谁。相比其他二分写法，开区间写法不需要思考加一减一等细节，更简单。推荐使用开区间写二分。

#### 3)

注意我们会二分多次，不能直接修改 $s$。

可以用一个 $\textit{star}$ 布尔数组标记哪些位置改成了星号。

为避免在二分内部反复创建/初始化数组，可以改成整数数组，记录二分的 $\textit{m}$ 值，如果 $\textit{star}[i]=m$ 则表示标记成星号。

具体请看 [视频讲解](https://www.bilibili.com/video/BV1BEh3zZEoM/?t=16m47s)，欢迎点赞关注~

```py [sol-Python3]
class Solution:
    def minTime(self, s: str, order: List[int], k: int) -> int:
        n = len(s)
        if n * (n + 1) // 2 < k:
            return -1

        star = [0] * n  # 避免在二分内部反复创建/初始化列表

        def check(m: int) -> bool:
            m += 1
            for j in range(m):
                star[order[j]] = m
            cnt = 0
            last = -1  # 上一个 '*' 的位置
            for i, x in enumerate(star):
                if x == m:  # s[i] 是 '*'
                    last = i
                cnt += last + 1
                if cnt >= k:  # 提前退出循环
                    return True
            return False

        left, right = -1, n - 1
        while left + 1 < right:
            mid = (left + right) // 2
            if check(mid):
                right = mid
            else:
                left = mid
        return right
```

```py [sol-Python3 库函数]
class Solution:
    def minTime(self, s: str, order: List[int], k: int) -> int:
        n = len(s)
        if n * (n + 1) // 2 < k:
            return -1

        star = [0] * n  # 避免在二分内部反复创建/初始化列表

        def check(m: int) -> bool:
            m += 1
            for j in range(m):
                star[order[j]] = m
            cnt = 0
            last = -1  # 上一个 '*' 的位置
            for i, x in enumerate(star):
                if x == m:  # s[i] 是 '*'
                    last = i
                cnt += last + 1
                if cnt >= k:  # 提前退出循环
                    return True
            return False

        return bisect_left(range(n - 1), True, key=check)
```

```java [sol-Java]
class Solution {
    public int minTime(String s, int[] order, int k) {
        int n = s.length();
        if ((long) n * (n + 1) / 2 < k) {
            return -1;
        }

        int[] star = new int[n]; // 避免在二分内部反复创建/初始化列表
        int left = -1;
        int right = n - 1;
        while (left + 1 < right) {
            int mid = left + (right - left) / 2;
            if (check(mid, k, order, star)) {
                right = mid;
            } else {
                left = mid;
            }
        }
        return right;
    }

    private boolean check(int m, int k, int[] order, int[] star) {
        m++;
        for (int j = 0; j < m; j++) {
            star[order[j]] = m;
        }
        int cnt = 0;
        int last = -1; // 上一个 '*' 的位置
        for (int i = 0; i < star.length; i++) {
            if (star[i] == m) { // s[i] 是 '*'
                last = i;
            }
            cnt += last + 1;
            if (cnt >= k) { // 提前退出循环
                return true;
            }
        }
        return false;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minTime(string s, vector<int>& order, int k) {
        int n = s.size();
        if (1LL * n * (n + 1) / 2 < k) {
            return -1;
        }

        vector<int> star(n); // 避免在二分内部反复创建/初始化列表

        auto check = [&](int m) -> bool {
            m++;
            for (int j = 0; j < m; j++) {
                star[order[j]] = m;
            }
            int cnt = 0;
            int last = -1; // 上一个 '*' 的位置
            for (int i = 0; i < n; i++) {
                if (star[i] == m) { // s[i] 是 '*'
                    last = i;
                }
                cnt += last + 1;
                if (cnt >= k) { // 提前退出循环
                    return true;
                }
            }
            return false;
        };

        int left = -1, right = n - 1;
        while (left + 1 < right) {
            int mid = left + (right - left) / 2;
            (check(mid) ? right : left) = mid;
        }
        return right;
    }
};
```

```go [sol-Go]
func minTime(s string, order []int, k int) int {
	n := len(s)
	if n*(n+1)/2 < k { // 全改成星号也无法满足要求
		return -1
	}

	star := make([]int, n) // 避免在二分内部反复创建/初始化列表
	ans := sort.Search(n-1, func(m int) bool {
		m++
		for _, j := range order[:m] {
			star[j] = m
		}
		cnt := 0
		last := -1 // 上一个 '*' 的位置
		for i, x := range star {
			if x == m { // s[i] 是 '*'
				last = i
			}
			cnt += last + 1
			if cnt >= k { // 提前退出循环
				return true
			}
		}
		return false
	})
	return ans
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n)$，其中 $n$ 是 $s$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 方法二：逆向思维+双向链表

> **注**：直接正向思维也可以做，但需要用有序集合，方便找左右最近星号位置，不如逆向思维的做法。

我们从全为星号的情况开始，倒着遍历 $\textit{order}$，把星号还原回字母。

一开始有 $\textit{cnt}=\dfrac{n(n+1)}{2}$ 个有效子串。

把 $s[i]$ 还原回字母，设 $i$ 左侧最近星号为 $L$（若不存在则 $L=-1$），$i$ 右侧最近星号为 $R$（若不存在则 $R=n$）。

那么原本左端点在 $[L+1,i]$，右端点在 $[i,R-1]$ 中的有效子串就变成非有效子串了，根据乘法原理，这一共有

$$
(i-L)(R-i)
$$

个，从 $\textit{cnt}$ 中减去。

如果减去后 $\textit{cnt}<k$，那么答案就是当前时刻。

代码实现时，用数组模拟双向链表，以及链表的节点删除操作，从而快速知道上文中的 $L$ 和 $R$ 的值。可以参考 [图解 LRU](https://leetcode.cn/problems/lru-cache/solutions/2456294/tu-jie-yi-zhang-tu-miao-dong-lrupythonja-czgt/) 中的图。

```py [sol-Python3]
class Solution:
    def minTime(self, s: str, order: List[int], k: int) -> int:
        n = len(s)
        cnt = n * (n + 1) // 2
        if cnt < k:  # 全改成星号也无法满足要求
            return -1

        # 数组模拟双向链表
        pre = list(range(-1, n))
        nxt = list(range(1, n + 2))

        for t in range(n - 1, -1, -1):
            i = order[t]
            l, r = pre[i], nxt[i]
            cnt -= (i - l) * (r - i)
            if cnt < k:
                return t
            # 删除链表中的 i
            nxt[l] = r
            pre[r] = l
```

```java [sol-Java]
class Solution {
    public int minTime(String s, int[] order, int k) {
        int n = s.length();
        long cnt = (long) n * (n + 1) / 2;
        if (cnt < k) { // 全改成星号也无法满足要求
            return -1;
        }

        // 数组模拟双向链表
        int[] prev = new int[n + 1];
        int[] next = new int[n];
        for (int i = 0; i < n; i++) {
            prev[i] = i - 1;
            next[i] = i + 1;
        }

        for (int t = n - 1; ; t--) {
            int i = order[t];
            int l = prev[i];
            int r = next[i];
            cnt -= (long) (i - l) * (r - i);
            if (cnt < k) {
                return t;
            }
            // 删除链表中的 i
            if (l >= 0) {
                next[l] = r;
            }
            prev[r] = l;
        }
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minTime(string s, vector<int>& order, int k) {
        int n = s.size();
        long long cnt = 1LL * n * (n + 1) / 2;
        if (cnt < k) { // 全改成星号也无法满足要求
            return -1;
        }

        // 数组模拟双向链表
        vector<int> prev(n + 1), next(n);
        ranges::iota(prev, -1);
        ranges::iota(next, 1);

        for (int t = n - 1; ; t--) {
            int i = order[t];
            int l = prev[i], r = next[i];
            cnt -= 1LL * (i - l) * (r - i);
            if (cnt < k) {
                return t;
            }
            // 删除链表中的 i
            if (l >= 0) {
                next[l] = r;
            }
            prev[r] = l;
        }
    }
};
```

```go [sol-Go]
func minTime(s string, order []int, k int) int {
	n := len(s)
	cnt := n * (n + 1) / 2
	if cnt < k { // 全改成星号也无法满足要求
		return -1
	}

	// 数组模拟双向链表
	prev := make([]int, n+1)
	next := make([]int, n)
	for i := range n {
		prev[i] = i - 1
		next[i] = i + 1
	}

	for t := n - 1; ; t-- {
		i := order[t]
		l, r := prev[i], next[i]
		cnt -= (i - l) * (r - i)
		if cnt < k {
			return t
		}
		// 删除链表中的 i
		if l >= 0 {
			next[l] = r
		}
		prev[r] = l
	}
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $s$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 专题训练

见下面二分题单的「**§2.1 求最小**」。

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

本题来自 `二、二分答案 / §2.1 求最小 / 答疑`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、二分答案 / §2.1 求最小 / 答疑`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
