# 564. 寻找最近的回文数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/find-the-closest-palindrome/
- 题目 slug：`find-the-closest-palindrome`
- 来源专题：数学算法
- 来源分类路径：七、杂项 / §7.1 回文数
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/find-the-closest-palindrome/solutions/3855597/zhi-xu-kao-lu-5-ge-shu-zi-pythonjavacgo-3td25/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[只需考虑 5 个数（Python/Java/C++/Go）](https://leetcode.cn/problems/find-the-closest-palindrome/solutions/3855597/zhi-xu-kao-lu-5-ge-shu-zi-pythonjavacgo-3td25/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`zhi-xu-kao-lu-5-ge-shu-zi-pythonjavacgo-3td25`
- topic id：`3855597`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:37:43 +0800

对于连续的回文数，例如 $232,242,252,262,\ldots$ 这些数的左半边（包含回文中心）是连续的整数 $23,24,25,26,\ldots$

设 $n$ 的左半（奇数长度时包含中心）为 $\textit{left}$。一般地，最近回文数的左半只会在 $\textit{left}-1, \textit{left}, \textit{left}+1$ 中，其余回文数一定比这三个更远。枚举这三个回文数。

注意本题不允许回文数与 $n$ 相同，可能出现 $n=999$，答案为 $1001$，或者 $n=101$，答案为 $99$ 的情况。虽然可以在枚举 $\textit{left}-1, \textit{left}, \textit{left}+1$ 时处理，但特判是最方便的。

设 $m$ 是 $n$ 的十进制长度，额外枚举两种情况：

- 十进制长为 $m-1$ 的最大回文数 $10^{m-1}-1$。
- 十进制长为 $m+1$ 的最小回文数 $10^m+1$。

> 注：代码实现时，无需考虑 $\textit{left}-1$ 生成的回文数的十进制长度小于 $m$ 的情况，按照我们的规则，这种回文数的十进制长度是 $m-2$，远远小于我们需要考虑的数。同理，无需考虑 $\textit{left}+1$ 生成的回文数的十进制长度大于 $m$ 的情况。

```py [sol-Python3]
class Solution:
    def nearestPalindromic(self, n: str) -> str:
        num = int(n)
        ans = (inf, 0)

        def update(pal: int) -> None:
            diff = abs(pal - num)
            if diff > 0:
                nonlocal ans
                ans = min(ans, (diff, pal))

        m = len(n)  # num 的十进制长度
        update(10 ** (m - 1) - 1)  # 十进制长为 m-1 的最大回文数
        update(10 ** m + 1)  # 十进制长为 m+1 的最小回文数

        left = int(n[:(m + 1) // 2])
        # 枚举十进制长为 m 的邻近回文数
        for l in range(left - 1, left + 2):
            s = str(l)
            update(int(s + s[::-1][m % 2:]))

        return str(ans[1])
```

```java [sol-Java]
class Solution {
    private long minD = Long.MAX_VALUE;
    private long ans;

    public String nearestPalindromic(String n) {
        long num = Long.parseLong(n);
        int m = n.length(); // num 的十进制长度
        update((long) Math.pow(10, m - 1) - 1, num); // 十进制长为 m-1 的最大回文数
        update((long) Math.pow(10, m) + 1, num);     // 十进制长为 m+1 的最小回文数

        int left = Integer.parseInt(n.substring(0, (m + 1) / 2));
        // 枚举十进制长为 m 的邻近回文数
        for (int l = left - 1; l <= left + 1; l++) {
            long pal = l;
            for (int x = m % 2 > 0 ? l / 10 : l; x > 0; x /= 10) {
                pal = pal * 10 + x % 10;
            }
            update(pal, num);
        }

        return Long.toString(ans);
    }

    private void update(long pal, long num) {
        long d = Math.abs(pal - num);
        if (d > 0 && (d < minD || d == minD && pal < ans)) {
            minD = d;
            ans = pal;
        }
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    string nearestPalindromic(string n) {
        long long num = stoll(n);
        pair<long long, long long> ans = {LLONG_MAX, 0};

        auto update = [&](long long pal) -> void {
            long long diff = abs(pal - num);
            if (diff > 0) {
                ans = min(ans, pair(diff, pal));
            }
        };

        int m = n.size(); // num 的十进制长度
        // 注意这里要转一下类型，因为浮点数 1e18 + 1 == 1e18
        update((long long) pow(10, m - 1) - 1); // 十进制长为 m-1 的最大回文数
        update((long long) pow(10, m) + 1);     // 十进制长为 m+1 的最小回文数

        int left = stoi(n.substr(0, (m + 1) / 2));
        // 枚举十进制长为 m 的邻近回文数
        for (int l = left - 1; l <= left + 1; l++) {
            long long pal = l;
            for (int x = m % 2 ? l / 10 : l; x > 0; x /= 10) {
                pal = pal * 10 + x % 10;
            }
            update(pal);
        }

        return to_string(ans.second);
    }
};
```

```go [sol-Go]
func nearestPalindromic(n string) string {
	minD, ans := math.MaxInt, 0
	num, _ := strconv.Atoi(n)

	update := func(pal int) {
		d := abs(pal - num)
		if d > 0 && (d < minD || d == minD && pal < ans) {
			minD, ans = d, pal
		}
	}

	m := len(n) // num 的十进制长度
	update(int(math.Pow10(m-1)) - 1) // 十进制长为 m-1 的最大回文数
	update(int(math.Pow10(m)) + 1)   // 十进制长为 m+1 的最小回文数

	left, _ := strconv.Atoi(n[:(m+1)/2])
	// 枚举十进制长为 m 的邻近回文数
	for l := left - 1; l <= left+1; l++ {
		pal := l
		x := l
		if m%2 > 0 {
			x /= 10
		}
		for ; x > 0; x /= 10 {
			pal = pal*10 + x%10
		}
		update(pal)
	}

	return strconv.Itoa(ans)
}

func abs(x int) int { if x < 0 { return -x }; return x }
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(\log n)$。$n$ 的十进制长度为 $\mathcal{O}(\log n)$。
- 空间复杂度：$\mathcal{O}(\log n)$ 或 $\mathcal{O}(1)$。返回值不计入。

## 专题训练

见下面数学题单的「**§7.1 回文数**」。

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

本题来自 `七、杂项 / §7.1 回文数`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `七、杂项 / §7.1 回文数`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
