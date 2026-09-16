# 2478. 完美分割的方案数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/number-of-beautiful-partitions/
- 题目 slug：`number-of-beautiful-partitions`
- 来源专题：动态规划
- 来源分类路径：五、划分型 DP / §5.3 约束划分个数
- 难度分：2344
- 外部题解来源：https://leetcode.cn/problems/number-of-beautiful-partitions/solutions/1981346/dong-tai-gui-hua-jian-ji-xie-fa-xun-huan-xyw3/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[动态规划 + 简洁写法 + 循环优化（Python/Java/C++/Go）](https://leetcode.cn/problems/number-of-beautiful-partitions/solutions/1981346/dong-tai-gui-hua-jian-ji-xie-fa-xun-huan-xyw3/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`dong-tai-gui-hua-jian-ji-xie-fa-xun-huan-xyw3`
- topic id：`1981346`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

[视频讲解](https://www.bilibili.com/video/BV1A3411f7H3/) 已出炉，欢迎点赞三连，在评论区分享你对这场周赛的看法~

---

定义 $f[i][j]$ 表示把 $s$ 的前 $j$ 个字符分割成 $i$ 段的方案数（每段需要满足题目的后两个要求）。

定义 $j$ 为分割点，等价于 $s[j]$ 不是质数且 $s[j+1]$ 是质数。

如果 $j$ 是分割点，那么可以考虑枚举第 $i-1$ 段与第 $i$ 段的分割点 $j'$，需满足 $j-j'\ge \textit{minLength}$。

累加所有 $f[i-1][j']$，记作 $\textit{sum}$，那么 $f[i][j]=\textit{sum}$。

每个 $f[i][j]$ 都要这样累加就太慢了，需要优化。

我们可以从小到大同时遍历 $j'$ 和 $j$，一边更新 $\textit{sum}$，一边计算 $f[i][j]$，具体见代码。

为方便计算，定义初始值 $f[0][0] = 1$，表示空串的 $0$ 个分割算作一种方案。**因为这个原因，要把所有下标 $j$ 向后移动一位。**

答案为 $f[k][n]$，这里 $n$ 为 $s$ 的长度。

还有一些剪枝和循环次数优化的小技巧，具体见代码。

```py [sol1-Python3]
class Solution:
    def beautifulPartitions(self, s: str, k: int, l: int) -> int:
        MOD = 10 ** 9 + 7
        def is_prime(c: str) -> bool:
            return c in "2357"
        # 判断是否可以在 j-1 和 j 之间分割（开头和末尾也算）
        def can_partition(j: int) -> bool:
            return j == 0 or j == n or not is_prime(s[j - 1]) and is_prime(s[j])

        n = len(s)
        if k * l > n or not is_prime(s[0]) or is_prime(s[-1]):  # 剪枝
            return 0
        f = [[0] * (n + 1) for _ in range(k + 1)]
        f[0][0] = 1
        for i in range(1, k + 1):
            sum = 0
            # 优化：枚举的起点和终点需要给前后的子串预留出足够的长度
            for j in range(i * l, n - (k - i) * l + 1):
                if can_partition(j - l): sum = (sum + f[i - 1][j - l]) % MOD  # j'=j-l 双指针
                if can_partition(j): f[i][j] = sum
        return f[k][n]
```

```java [sol1-Java]
class Solution {
    private static final int MOD = (int) 1e9 + 7;

    public int beautifulPartitions(String S, int k, int l) {
        var s = S.toCharArray();
        var n = s.length;
        if (k * l > n || !isPrime(s[0]) || isPrime(s[n - 1])) // 剪枝
            return 0;
        var f = new int[k + 1][n + 1];
        f[0][0] = 1;
        for (var i = 1; i <= k; ++i) {
            var sum = 0;
            // 优化：枚举的起点和终点需要给前后的子串预留出足够的长度
            for (var j = i * l; j + (k - i) * l <= n; j++) {
                if (canPartition(s, j - l)) sum = (sum + f[i - 1][j - l]) % MOD; // j'=j-l 双指针
                if (canPartition(s, j)) f[i][j] = sum;
            }
        }
        return f[k][n];
    }

    private boolean isPrime(char c) {
        return c == '2' || c == '3' || c == '5' || c == '7';
    }

    // 判断是否可以在 j-1 和 j 之间分割（开头和末尾也算）
    private boolean canPartition(char[] s, int j) {
        return j == 0 || j == s.length || !isPrime(s[j - 1]) && isPrime(s[j]);
    }
}
```

```cpp [sol1-C++]
class Solution {
    const int MOD = 1e9 + 7;

    bool is_prime(char c) {
        return c == '2' || c == '3' || c == '5' || c == '7';
    }

    // 判断是否可以在 j-1 和 j 之间分割（开头和末尾也算）
    bool can_partition(string &s, int j) {
        return j == 0 || j == s.length() || !is_prime(s[j - 1]) && is_prime(s[j]);
    }

public:
    int beautifulPartitions(string &s, int k, int l) {
        int n = s.length();
        if (k * l > n || !is_prime(s[0]) || is_prime(s[n - 1])) // 剪枝
            return 0;
        int f[k + 1][n + 1]; memset(f, 0, sizeof(f));
        f[0][0] = 1;
        for (int i = 1; i <= k; ++i) {
            int sum = 0;
            // 优化：枚举的起点和终点需要给前后的子串预留出足够的长度
            for (int j = i * l; j + (k - i) * l <= n; j++) {
                if (can_partition(s, j - l)) sum = (sum + f[i - 1][j - l]) % MOD; // j'=j-l 双指针
                if (can_partition(s, j)) f[i][j] = sum;
            }
        }
        return f[k][n];
    }
};
```

```go [sol1-Go]
func beautifulPartitions(s string, k, l int) (ans int) {
	const mod int = 1e9 + 7
	isPrime := func(c byte) bool { return strings.IndexByte("2357", c) >= 0 }
	n := len(s)
	if k*l > n || !isPrime(s[0]) || isPrime(s[n-1]) { // 剪枝
		return
	}
	// 判断是否可以在 j-1 和 j 之间分割（开头和末尾也算）
	canPartition := func(j int) bool { return j == 0 || j == n || !isPrime(s[j-1]) && isPrime(s[j]) }
	f := make([][]int, k+1)
	for i := range f {
		f[i] = make([]int, n+1)
	}
	f[0][0] = 1
	for i := 1; i <= k; i++ {
		sum := 0
		// 优化：枚举的起点和终点需要给前后的子串预留出足够的长度
		for j := i * l; j+(k-i)*l <= n; j++ {
			if canPartition(j - l) { // j'=j-l 双指针
				sum = (sum + f[i-1][j-l]) % mod
			}
			if canPartition(j) {
				f[i][j] = sum
			}
		}
	}
	return f[k][n]
}
```

#### 复杂度分析

- 时间复杂度：$O(k(n-kl))$，其中 $n$ 为 $s$ 的长度。
- 空间复杂度：$O(kn)$。注：也可以用滚动数组优化至 $O(n)$。

#### 相似题目

- [1977. 划分数字的方案数](https://leetcode.cn/problems/number-of-ways-to-separate-numbers/)

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
