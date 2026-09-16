# 3524. 求出数组的 X 值 I

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/find-x-value-of-array-i/
- 题目 slug：`find-x-value-of-array-i`
- 来源专题：动态规划
- 来源分类路径：七、其他线性 DP / §7.3 子数组 DP
- 难度分：2008
- 外部题解来源：https://leetcode.cn/problems/find-x-value-of-array-i/solutions/3656580/zi-shu-zu-dp-shua-biao-fa-pythonjavacgo-8lonk/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[子数组 DP + 刷表法（Python/Java/C++/Go）](https://leetcode.cn/problems/find-x-value-of-array-i/solutions/3656580/zi-shu-zu-dp-shua-biao-fa-pythonjavacgo-8lonk/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`zi-shu-zu-dp-shua-biao-fa-pythonjavacgo-8lonk`
- topic id：`3656580`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 题意

对于 $x=0,1,2,\ldots,k-1$，计算有多少个非空连续子数组的元素积模 $k$ 等于 $x$。

## 初步想法

**枚举**子数组右端点，计算右端点为 $0,1,2,\ldots,n-1$ 时，元素积模 $k$ 等于 $x$ 的子数组有多少个。

## 寻找子问题

回想一下 [53. 最大子数组和](https://leetcode.cn/problems/maximum-subarray/)，要计算右端点为 $i$ 的最大子数组和，我们需要知道右端点为 $i-1$ 的最大子数组和。换句话说，右端点为 $i-1$ 的子数组，拼接上 $\textit{nums}[i]$，就得到了右端点为 $i$ 的子数组。

本题也一样，要计算右端点为 $i$ 的、元素积模 $k$ 等于 $x$ 的子数组的个数，需要知道右端点为 $i-1$ 的、元素积模 $k$ 等于 $y$ 的子数组的个数。

比如 $k=5$，右端点为 $i-1$ 的元素积模 $5$ 等于 $2$ 的子数组有 $100$ 个。假设 $\textit{nums}[i]=4$，那么计算 $2\cdot 4\bmod 5 = 3$，我们就得到了 $100$ 个右端点为 $i$ 的元素积模 $5$ 等于 $3$ 的子数组。

此外，我们只需要知道元素积模 $k$ 的结果，至于原始元素积是多少，我们并不关心。比如 $k=5$，原始元素积等于 $10$ 还是 $15$，取模后都是 $0$，这些数（无论是原始元素积还是取模后的元素积）乘以其他数，模 $k$ 后都是 $0$。换句话说，我们用到的是如下恒等式

$$
(a\cdot b) \bmod k=((a\bmod k)\cdot  b) \bmod k
$$

对于模运算，可以在计算过程中取模，而不是等到计算完毕后再取模。详见 [模运算的世界：当加减乘除遇上取模](https://leetcode.cn/circle/discuss/mDfnkW/)。

## 动态规划

一般地，定义 $f[i+1][x]$ 表示右端点为 $i$ 的、元素积模 $k$ 等于 $x$ 的子数组的个数。$+1$ 是为了方便用 $f[0]$ 表示初始值。 

设 $v=\textit{nums}[i]$。采用**刷表法**转移：

- 对于右端点为 $i-1$ 的子数组，枚举子数组元素积模 $k$ 等于 $y=0,1,2,\ldots,k-1$，更新右端点为 $i$ 的子数组的状态值，也就是把 $f[i+1][y\cdot v\bmod k]$ 增加 $f[i][y]$。
- 此外，$v$ 这个数也可以单独作为一个子数组（长度等于 $1$），把 $f[i+1][v\bmod k]$ 加一。

> **刷表法**：如果枚举 $x$，倒推 $x = y \cdot v\bmod k$ 中的 $y$，是困难的。但枚举 $y$，计算 $y\cdot v \bmod k$，是简单的。也就是说，对于状态 $f[i+1][x]$ 而言，其转移来源是谁不好计算，但从 $f[i][y]$ 转移到的目标状态 $f[i+1][y\cdot v\bmod k]$ 是好计算的。在动态规划中，根据转移来源计算状态叫查表法，用当前状态更新其他状态叫刷表法。

**初始值**：$f[0][0] = 0$，一开始没有任何元素。 

**答案**：$\textit{ans}[x] = \displaystyle\sum\limits_{i=1}^{n}f[i][x]$。

具体请看 [视频讲解](https://www.bilibili.com/video/BV1NALczNERr/?t=18m15s)，欢迎点赞关注~

## 空间优化前

```py [sol-Python3]
class Solution:
    def resultArray(self, nums: List[int], k: int) -> List[int]:
        ans = [0] * k
        f = [[0] * k for _ in range(len(nums) + 1)]
        for i, v in enumerate(nums):
            f[i + 1][v % k] = 1
            for y, c in enumerate(f[i]):
                f[i + 1][y * v % k] += c  # 刷表法
            for x, c in enumerate(f[i + 1]):
                ans[x] += c
        return ans
```

```java [sol-Java]
class Solution {
    public long[] resultArray(int[] nums, int k) {
        int n = nums.length;
        long[] ans = new long[k];
        int[][] f = new int[n + 1][k];
        for (int i = 0; i < n; i++) {
            int v = nums[i] % k; // 避免下面乘法溢出
            f[i + 1][v] = 1;
            for (int y = 0; y < k; y++) {
                f[i + 1][y * v % k] += f[i][y]; // 刷表法
            }
            for (int x = 0; x < k; x++) {
                ans[x] += f[i + 1][x];
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<long long> resultArray(vector<int>& nums, int k) {
        int n = nums.size();
        vector<long long> ans(k);
        vector f(n + 1, vector<int>(k));
        for (int i = 0; i < n; i++) {
            int v = nums[i] % k; // 避免下面乘法溢出
            f[i + 1][v] = 1;
            for (int y = 0; y < k; y++) {
                f[i + 1][y * v % k] += f[i][y]; // 刷表法
            }
            for (int x = 0; x < k; x++) {
                ans[x] += f[i + 1][x];
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func resultArray(nums []int, k int) []int64 {
	ans := make([]int64, k)
	f := make([][]int, len(nums)+1)
	for i := range f {
		f[i] = make([]int, k)
	}
	for i, v := range nums {
		f[i+1][v%k] = 1
		for y, c := range f[i] {
			f[i+1][y*v%k] += c // 刷表法
		}
		for x, c := range f[i+1] {
			ans[x] += int64(c)
		}
	}
	return ans
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(nk)$，其中 $n$ 是 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(nk)$。

## 空间优化

```py [sol-Python3]
class Solution:
    def resultArray(self, nums: List[int], k: int) -> List[int]:
        ans = [0] * k
        f = [0] * k
        for v in nums:
            nf = [0] * k
            nf[v % k] = 1
            for y, c in enumerate(f):
                nf[y * v % k] += c
            f = nf
            for x, c in enumerate(f):
                ans[x] += c
        return ans
```

```java [sol-Java]
class Solution {
    public long[] resultArray(int[] nums, int k) {
        long[] ans = new long[k];
        int[] f = new int[k];
        for (int v : nums) {
            v %= k; // 避免下面乘法溢出
            int[] nf = new int[k];
            nf[v] = 1;
            for (int y = 0; y < k; y++) {
                nf[y * v % k] += f[y];
            }
            f = nf;
            for (int x = 0; x < k; x++) {
                ans[x] += f[x];
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<long long> resultArray(vector<int>& nums, int k) {
        vector<long long> ans(k);
        vector<int> f(k); // 更快的写法见【C++ array】
        for (int v : nums) {
            v %= k; // 避免下面乘法溢出
            vector<int> nf(k);
            nf[v] = 1;
            for (int y = 0; y < k; y++) {
                nf[y * v % k] += f[y];
            }
            f = move(nf);
            for (int x = 0; x < k; x++) {
                ans[x] += f[x];
            }
        }
        return ans;
    }
};
```

```cpp [sol-C++ array]
class Solution {
public:
    vector<long long> resultArray(vector<int>& nums, int k) {
        vector<long long> ans(k);
        array<int, 5> f{};
        for (int v : nums) {
            v %= k; // 避免下面乘法溢出
            array<int, 5> nf{};
            nf[v] = 1;
            for (int y = 0; y < k; y++) {
                nf[y * v % k] += f[y];
            }
            f = move(nf);
            for (int x = 0; x < k; x++) {
                ans[x] += f[x];
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func resultArray(nums []int, k int) []int64 {
	ans := make([]int64, k)
	f := make([]int, k)
	for _, v := range nums {
		nf := make([]int, k)
		nf[v%k] = 1
		for y, c := range f {
			nf[y*v%k] += c
		}
		f = nf
		for x, c := range f {
			ans[x] += int64(c)
		}
	}
	return ans
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(nk)$，其中 $n$ 是 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(k)$。

## 思考题

本题去掉的是数组前缀和后缀，如果改成去掉中间的子数组呢？

欢迎在评论区分享你的思路/代码。

## 相似题目（刷表法）

- [2140. 解决智力问题](https://leetcode.cn/problems/solving-questions-with-brainpower/) 1709
- [3448. 统计可以被最后一个数位整除的子字符串数目](https://leetcode.cn/problems/count-substrings-divisible-by-last-digit/) 2387 这题和本题比较像

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

本题来自 `七、其他线性 DP / §7.3 子数组 DP`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `七、其他线性 DP / §7.3 子数组 DP`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
