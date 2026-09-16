# 1871. 跳跃游戏 VII

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/jump-game-vii/
- 题目 slug：`jump-game-vii`
- 来源专题：滑动窗口与双指针
- 来源分类路径：三、单序列双指针 / §3.3 同向双指针
- 难度分：1896
- 外部题解来源：https://leetcode.cn/problems/jump-game-vii/solutions/791314/qian-zhui-he-you-hua-dp-by-endlesscheng-k9d2/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[三种方法：DP/差分数组/同向双指针（Python/Java/C++/Go）](https://leetcode.cn/problems/jump-game-vii/solutions/791314/qian-zhui-he-you-hua-dp-by-endlesscheng-k9d2/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`qian-zhui-he-you-hua-dp-by-endlesscheng-k9d2`
- topic id：`791314`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 方法一：前缀和优化 DP

从下标 $i$ 跳到下标 $j$，需要满足 $s[j]=\texttt{0}$，并且

$$
i + \textit{minJump}\le j \le \min(i+\textit{maxJump},n-1)
$$

换句话说，能跳到 $j$ 的 $i$ 必须满足

$$
\max(j-\textit{maxJump}, 0) \le i \le j-\textit{minJump}
$$

想一想，最后一步发生了什么？

- 最后一步跳到 $n-1$。
- 枚举起跳位置为 $i$，问题变成能否从 $0$ 跳到 $i$，这是规模更小的子问题。

定义 $f[j]$ 表示能否从下标 $0$ 跳到下标 $j$。

枚举上一步的位置 $i$，如果在区间 $[\max(j-\textit{maxJump}, 0), j-\textit{minJump}]$ 中存在下标 $i$ 满足 $f[i] = \texttt{true}$，那么 $\textit{f}[j] = \texttt{true}$。

初始值：$f[0]=\texttt{true}$。注意题目保证 $s[0] = \texttt{0}$。

答案：$f[n-1]$。

这样做的时间复杂度为 $\mathcal{O}(n(\textit{maxJump}-\textit{minJump}))$，最坏情况下是 $\mathcal{O}(n^2)$ 的，太慢了。

## 优化

把 $\texttt{false}$ 视作 $0$，$\texttt{true}$ 视作 $1$，计算 $f$ 的**前缀和**数组 $\textit{sum}$。关于 $\textit{sum}$ 数组的定义，请看 [前缀和](https://leetcode.cn/problems/range-sum-query-immutable/solution/qian-zhui-he-ji-qi-kuo-zhan-fu-ti-dan-py-vaar/)。

区间 $[\max(j-\textit{maxJump}, 0), j-\textit{minJump}]$ 中存在下标 $i$ 满足 $f[i] = \texttt{true}$，等价于 $f$ 的子数组 $[\max(j-\textit{maxJump}, 0), j-\textit{minJump}]$ 的元素和大于 $0$。用前缀和表示为

$$
\textit{sum}[j-\textit{minJump} + 1] - \textit{sum}[\max(j-\textit{maxJump}, 0)] > 0
$$

即

$$
\textit{sum}[j-\textit{minJump} + 1] > \textit{sum}[\max(j-\textit{maxJump}, 0)]
$$

```py [sol-Python3]
class Solution:
    def canReach(self, s: str, minJump: int, maxJump: int) -> bool:
        n = len(s)
        f = [False] * n
        sum_ = [0] * (n + 1)  # f 的前缀和
        f[0] = True
        sum_[1] = 1

        for j in range(1, n):
            f[j] = j >= minJump and s[j] == '0' and sum_[j - minJump + 1] > sum_[max(j - maxJump, 0)]
            sum_[j + 1] = sum_[j] + f[j]

        return f[-1]
```

```java [sol-Java]
class Solution {
    public boolean canReach(String s, int minJump, int maxJump) {
        int n = s.length();
        int[] f = new int[n];
        int[] sum = new int[n + 1]; // f 的前缀和
        f[0] = sum[1] = 1;

        for (int j = 1; j < n; j++) {
            if (j >= minJump && s.charAt(j) == '0' && sum[j - minJump + 1] > sum[Math.max(j - maxJump, 0)]) {
                f[j] = 1;
            }
            sum[j + 1] = sum[j] + f[j];
        }

        return f[n - 1] == 1;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    bool canReach(string s, int minJump, int maxJump) {
        int n = s.size();
        vector<int8_t> f(n);
        vector<int> sum(n + 1); // f 的前缀和
        f[0] = true;
        sum[1] = 1;

        for (int j = 1; j < n; j++) {
            f[j] = j >= minJump && s[j] == '0' && sum[j - minJump + 1] > sum[max(j - maxJump, 0)];
            sum[j + 1] = sum[j] + f[j];
        }

        return f[n - 1];
    }
};
```

```go [sol-Go]
func canReach(s string, minJump, maxJump int) bool {
	n := len(s)
	f := make([]int, n)
	sum := make([]int, n+1) // f 的前缀和
	f[0] = 1
	sum[1] = 1
	for j := 1; j < n; j++ {
		if j >= minJump && s[j] == '0' && sum[j-minJump+1] > sum[max(j-maxJump, 0)] {
			f[j] = 1
		}
		sum[j+1] = sum[j] + f[j]
	}
	return f[n-1] == 1
}
```

$f$ 数组可以优化掉。

```py [sol-Python3]
class Solution:
    def canReach(self, s: str, minJump: int, maxJump: int) -> bool:
        n = len(s)
        sum_ = [0] * (n + 1)  # f 的前缀和
        sum_[1] = 1  # f[0] = True

        for j in range(1, n):
            sum_[j + 1] = sum_[j]
            if j >= minJump and s[j] == '0' and sum_[j - minJump + 1] > sum_[max(j - maxJump, 0)]:
                sum_[j + 1] += 1  # f[j] = True

        return sum_[n] > sum_[n - 1]  # f[n-1] == True
```

```java [sol-Java]
class Solution {
    public boolean canReach(String s, int minJump, int maxJump) {
        int n = s.length();
        int[] sum = new int[n + 1]; // f 的前缀和
        sum[1] = 1; // f[0] = true

        for (int j = 1; j < n; j++) {
            sum[j + 1] = sum[j];
            if (j >= minJump && s.charAt(j) == '0' && sum[j - minJump + 1] > sum[Math.max(j - maxJump, 0)]) {
                sum[j + 1]++; // f[j] = true
            }
        }

        return sum[n] > sum[n - 1]; // f[n-1] == true
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    bool canReach(string s, int minJump, int maxJump) {
        int n = s.size();
        vector<int> sum(n + 1); // f 的前缀和
        sum[1] = 1; // f[0] = true

        for (int j = 1; j < n; j++) {
            sum[j + 1] = sum[j];
            if (j >= minJump && s[j] == '0' && sum[j - minJump + 1] > sum[max(j - maxJump, 0)]) {
                sum[j + 1]++; // f[j] = true
            }
        }

        return sum[n] > sum[n - 1]; // f[n-1] == true
    }
};
```

```go [sol-Go]
func canReach(s string, minJump, maxJump int) bool {
	n := len(s)
	sum := make([]int, n+1) // f 的前缀和
	sum[1] = 1 // f[0] = true
	for j := 1; j < n; j++ {
		sum[j+1] = sum[j]
		if j >= minJump && s[j] == '0' && sum[j-minJump+1] > sum[max(j-maxJump, 0)] {
			sum[j+1]++ // f[j] = true
		}
	}
	return sum[n] > sum[n-1] // f[n-1] == true
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $s$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 方法二：差分数组

当我们跳到下标 $i$ 时，需要标记 $[i+\textit{minJump},i+\textit{maxJump}]$ 中的下标都可以跳到（都可以访问）。

用一个数组 $a$ 记录标记次数。当我们跳到下标 $i$ 时，把区间 $[i+\textit{minJump},i+\textit{maxJump}]$ 中的 $a[j]$ 都加一。如果 $a[j] > 0$ 说明 $j$ 可以跳到，如果 $a[j] = 0$ 说明 $j$ 无法跳到。

区间加一操作，可以用**差分数组**快速实现。请看 [差分数组原理讲解](https://leetcode.cn/problems/car-pooling/solution/suan-fa-xiao-ke-tang-chai-fen-shu-zu-fu-9d4ra/)。计算差分数组的前缀和，就得到了数组 $a$。

一开始我们在下标 $0$，要把区间 $[0,0]$ 加一，这样我们可以知道下标 $0$ 是一个可以访问的下标。

```py [sol-Python3]
class Solution:
    def canReach(self, s: str, minJump: int, maxJump: int) -> bool:
        n = len(s)
        diff = [0] * (n + 1)

        # 一开始在起点 0，把 [0, 0] 加一
        diff[0] = 1
        diff[1] = -1

        sum_d = 0
        for i, ch in enumerate(s):
            sum_d += diff[i]
            # 现在 sum_d 是下标 i 的标记次数
            if sum_d > 0 and ch == '0':  # i 可以跳到
                # [i+minJump, i+maxJump] 可以跳到
                diff[min(i + minJump, n)] += 1
                diff[min(i + maxJump + 1, n)] -= 1

        return sum_d > 0 and s[-1] == '0'  # n-1 可以跳到
```

```java [sol-Java]
class Solution {
    public boolean canReach(String s, int minJump, int maxJump) {
        int n = s.length();
        int[] diff = new int[n + 1];

        // 一开始在起点 0，把 [0, 0] 加一
        diff[0] = 1;
        diff[1] = -1;

        int sumD = 0;
        for (int i = 0; i < n; i++) {
            sumD += diff[i];
            // 现在 sumD 是下标 i 的标记次数
            if (sumD > 0 && s.charAt(i) == '0') { // i 可以跳到
                // [i+minJump, i+maxJump] 可以跳到
                diff[Math.min(i + minJump, n)]++;
                diff[Math.min(i + maxJump + 1, n)]--;
            }
        }

        return sumD > 0 && s.charAt(n - 1) == '0'; // n-1 可以跳到
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    bool canReach(string s, int minJump, int maxJump) {
        int n = s.size();
        vector<int> diff(n + 1);

        // 一开始在起点 0，把 [0, 0] 加一
        diff[0] = 1;
        diff[1] = -1;

        int sum_d = 0;
        for (int i = 0; i < n; i++) {
            char ch = s[i];
            sum_d += diff[i];
            // 现在 sum_d 是下标 i 的标记次数
            if (sum_d > 0 && ch == '0') { // i 可以跳到
                // [i+minJump, i+maxJump] 可以跳到
                diff[min(i + minJump, n)]++;
                diff[min(i + maxJump + 1, n)]--;
            }
        }

        return sum_d > 0 && s[n - 1] == '0'; // n-1 可以跳到
    }
};
```

```go [sol-Go]
func canReach(s string, minJump, maxJump int) bool {
	n := len(s)
	diff := make([]int, n+1)

	// 一开始在起点 0，把 [0, 0] 加一
	diff[0] = 1
	diff[1] = -1

	sumD := 0
	for i, ch := range s {
		sumD += diff[i]
		// 现在 sumD 是下标 i 的标记次数
		if sumD > 0 && ch == '0' { // i 可以跳到
			// [i+minJump, i+maxJump] 可以跳到
			diff[min(i+minJump, n)]++
			diff[min(i+maxJump+1, n)]--
		}
	}
	return sumD > 0 && s[n-1] == '0' // n-1 可以跳到
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $s$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 方法三：同向双指针

在不断往右跳的过程中，窗口 $[i+\textit{minJump},i+\textit{maxJump}]$ 也在向右滑动。窗口右边的还没有被我们访问到的下标，都在 $s$ 的后缀中。

于是，用一个下标 $j$ 表示：还没有访问到的下标为 $[j,n)$。初始值 $j=1$。

当我们跳到下标 $i$ 时，暴力标记 $[\max(j, i+\textit{minJump}),i+\textit{maxJump}]$ 中的下标都被访问到。然后更新 $j$ 为 $i+\textit{maxJump}+1$。

由于 $j$ 只会增大，不会减小，所以总体时间复杂度是 $\mathcal{O}(n)$。

```py [sol-Python3]
class Solution:
    def canReach(self, s: str, minJump: int, maxJump: int) -> bool:
        n = len(s)
        can_reaches = [False] * n
        can_reaches[0] = True

        j = 1
        for i, ch in enumerate(s):
            if ch == '0' and can_reaches[i]:
                # 注意 j 只会增大，不会减小，所以总体时间复杂度是 O(n)
                mx = min(i + maxJump, n - 1)
                j = max(j, i + minJump)
                while j <= mx:                    
                    can_reaches[j] = True  # 可以跳到 j
                    j += 1
                if j == n:
                    break

        return s[-1] == '0' and can_reaches[-1]
```

```java [sol-Java]
class Solution {
    public boolean canReach(String s, int minJump, int maxJump) {
        int n = s.length();
        boolean[] canReaches = new boolean[n];
        canReaches[0] = true;

        int j = 1;
        for (int i = 0; i < n && j < n; i++) {
            if (canReaches[i] && s.charAt(i) == '0') {
                // 注意 j 只会增大，不会减小，所以总体时间复杂度是 O(n)
                for (j = Math.max(j, i + minJump); j <= Math.min(i + maxJump, n - 1); j++) {
                    canReaches[j] = true; // 可以跳到 j
                }
            }
        }

        return canReaches[n - 1] && s.charAt(n - 1) == '0';
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    bool canReach(string s, int minJump, int maxJump) {
        int n = s.size();
        vector<int8_t> can_reaches(n);
        can_reaches[0] = true;

        int j = 1;
        for (int i = 0; i < n && j < n; i++) {
            if (s[i] == '0' && can_reaches[i]) {
                // 注意 j 只会增大，不会减小，所以总体时间复杂度是 O(n)
                for (j = max(j, i + minJump); j <= min(i + maxJump, n - 1); j++) {
                    can_reaches[j] = true; // 可以跳到 j
                }
            }
        }

        return s[n - 1] == '0' && can_reaches[n - 1];
    }
};
```

```go [sol-Go]
func canReach(s string, minJump, maxJump int) bool {
	n := len(s)
	canReaches := make([]bool, n)
	canReaches[0] = true
	j := 1
	for i, ch := range s {
		if ch == '0' && canReaches[i] {
			// 注意 j 只会增大，不会减小，所以总体时间复杂度是 O(n)
			for j = max(j, i+minJump); j <= min(i+maxJump, n-1); j++ {
				canReaches[j] = true // 可以跳到 j
			}
		}
	}
	return s[n-1] == '0' && canReaches[n-1]
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $s$ 的长度。由于 $j$ 只会增大，不会减小，所以二重循环的总循环次数是 $\mathcal{O}(n)$，所以总体时间复杂度是 $\mathcal{O}(n)$。
- 空间复杂度：$\mathcal{O}(n)$ 或 $\mathcal{O}(1)$。

## 专题训练

1. 动态规划题单的「**§11.1 前缀和优化 DP**」。
2. 数据结构题单的「**§2.1 一维差分**」。
3. 双指针题单的「**§3.3 同向双指针**」。

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

本题来自 `三、单序列双指针 / §3.3 同向双指针`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `三、单序列双指针 / §3.3 同向双指针`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
