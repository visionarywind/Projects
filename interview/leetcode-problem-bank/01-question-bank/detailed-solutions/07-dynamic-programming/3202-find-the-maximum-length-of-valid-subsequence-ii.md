# 3202. 找出有效子序列的最大长度 II

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/find-the-maximum-length-of-valid-subsequence-ii/
- 题目 slug：`find-the-maximum-length-of-valid-subsequence-ii`
- 来源专题：动态规划
- 来源分类路径：七、其他线性 DP / §7.4 合法子序列 DP
- 难度分：1974
- 外部题解来源：https://leetcode.cn/problems/find-the-maximum-length-of-valid-subsequence-ii/solutions/2826591/deng-jie-zhuan-huan-dong-tai-gui-hua-pyt-z2fs/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[分析式子+动态规划，两种方法（Python/Java/C++/Go）](https://leetcode.cn/problems/find-the-maximum-length-of-valid-subsequence-ii/solutions/2826591/deng-jie-zhuan-huan-dong-tai-gui-hua-pyt-z2fs/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`deng-jie-zhuan-huan-dong-tai-gui-hua-pyt-z2fs`
- topic id：`2826591`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 分析

对于等式

$$
(a+b)\bmod k = (b+c)\bmod k
$$ 

根据 [模运算的世界：当加减乘除遇上取模](https://leetcode.cn/circle/discuss/mDfnkW/)，可以移项，得

$$
(a+b-(b+c)) \bmod k = 0
$$

化简得

$$
(a-c)\bmod k = 0
$$

这意味着 $a$ 与 $c$ 关于模 $k$ **同余**。即题目式子中的 $\textit{sub}[i]$ 与 $\textit{sub}[i+2]$ 关于模 $k$ **同余**。换句话说，有效子序列的偶数项 $\textit{sub}[0],\textit{sub}[2],\textit{sub}[4],\ldots$ 都关于模 $k$ 同余，奇数项 $\textit{sub}[1],\textit{sub}[3],\textit{sub}[5],\ldots$ 都关于模 $k$ 同余。

如果把每个 $\textit{nums}[i]$ 都改成 $\textit{nums}[i]\bmod k$，问题等价于：

- 求最长子序列的长度，该子序列的奇数项都相同，偶数项都相同。

在模 $k$ 意义下，如果确定了子序列的**最后两项**，就确定了整个子序列。

## 方法一：考察子序列的最后两项

下面讨论的内容，假定所有 $\textit{nums}[i]$ 已更新为 $\textit{nums}[i]\bmod k$。

比如 $\textit{nums}=[1,2,1,2,1,2]$：

- 遍历到 $1$ 的时候，在「末尾为 $1,2$ 的子序列」的末尾添加一个 $1$，得到末尾为 $1,2,1$ 的子序列，只看最后两个数，就是「末尾为 $2,1$ 的子序列」。只看最后两个数是因为确定了子序列的最后两项，就确定了整个子序列。
- 遍历到 $2$ 的时候，在「末尾为 $2,1$ 的子序列」的末尾添加一个 $2$，得到末尾为 $2,1,2$ 的子序列，只看最后两个数，就是「末尾为 $1,2$ 的子序列」。

⚠**注意**：对于 $\textit{nums}[0]=1$ 来说，虽然前面没有 $2$，但子序列总得有个头，可以认为这个 $1$ 在「末尾为 $2,1$ 的子序列」中，此时子序列只有 $1$ 一个数。

从左到右遍历 $\textit{nums}$，遍历的同时，维护（实时更新）一个二维数组 $f[y][x]$，表示对于目前已经遍历过的数，最后两项为 $y$ 和 $x$ 的子序列的最长长度。

对于 $x=\textit{nums}[i]$，我们可以在「最后两项为 $x$ 和 $y$ 的子序列」的末尾添加 $x$，得到末尾为 $x,y,x$ 的子序列，只看最后两个数，就是「最后两项为 $y$ 和 $x$ 的子序列」，因为添加了一个元素，子序列的长度增加 $1$，即

$$
f[y][x] = f[x][y] + 1
$$

> 注意上式和 $i$ 是无关的，我们在三维 DP 数组 $f[i][y][x]$ 的基础上，优化掉了第一维度。

最后答案为 $f$ 中的最大值。

### 答疑

**问**：如何理解这个递推？它和记忆化搜索的区别是什么？

**答**：对比二者的**计算顺序**。如果用记忆化搜索来做，需要单独计算「最左（或者最右）两项模 $k$ 分别为 $x$ 和 $y$ 的子序列」的长度，这是「单线程」，必须**查找下一个元素的位置**。而递推的计算顺序是，（假设我们先遍历到了元素 $2$，然后遍历到了元素 $4$，两个元素属于不同的子序列）一会计算一下「最后两项模 $k$ 分别为 $y$ 和 $2$ 的子序列」，一会又计算一下「最后两项模 $k$ 分别为 $y$ 和 $4$ 的子序列」，这是「多线程」，**没有查找元素位置的过程，遇到谁就处理谁**。

具体请看 [视频讲解](https://www.bilibili.com/video/BV16w4m1e7y3/) 第三题，欢迎点赞关注~

```py [sol-Python3]
class Solution:
    def maximumLength(self, nums: List[int], k: int) -> int:
        f = [[0] * k for _ in range(k)]
        for x in nums:
            x %= k
            for y, fxy in enumerate(f[x]):
                f[y][x] = fxy + 1
        return max(map(max, f))
```

```py [sol-Python3 写法二]
class Solution:
    def maximumLength(self, nums: List[int], k: int) -> int:
        f = [0] * (k * k)
        for x in nums:
            x %= k
            # f[x * k: (x + 1) * k] 是二维写法的第 x 行
            # f[x::k] 是二维写法的第 x 列
            f[x::k] = [v + 1 for v in f[x * k: (x + 1) * k]]
        return max(f)
```

```java [sol-Java]
class Solution {
    public int maximumLength(int[] nums, int k) {
        int ans = 0;
        int[][] f = new int[k][k];
        for (int x : nums) {
            x %= k;
            for (int y = 0; y < k; y++) {
                f[y][x] = f[x][y] + 1;
                ans = Math.max(ans, f[y][x]);
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maximumLength(vector<int>& nums, int k) {
        int ans = 0;
        vector f(k, vector<int>(k));
        for (int x : nums) {
            x %= k;
            for (int y = 0; y < k; y++) {
                f[y][x] = f[x][y] + 1;
                ans = max(ans, f[y][x]);
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func maximumLength(nums []int, k int) (ans int) {
	f := make([][]int, k)
	for i := range f {
		f[i] = make([]int, k)
	}
	for _, x := range nums {
		x %= k
		for y, fxy := range f[x] {
			f[y][x] = fxy + 1
			ans = max(ans, f[y][x])
		}
	}
	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(k^2 + nk)$，其中 $n$ 是 $\textit{nums}$ 的长度。注意创建大小为 $k^2$ 的二维数组需要 $\mathcal{O}(k^2)$ 的时间。
- 空间复杂度：$\mathcal{O}(k^2)$。

## 方法二：枚举余数，考察子序列的最后一项

下面讨论的内容，假定所有 $\textit{nums}[i]$ 已更新为 $\textit{nums}[i]\bmod k$。

枚举子序列相邻两项之和模 $k$ 的结果为 $m=0,1,2,\ldots, k-1$。

如果知道了子序列的最后一项（假设是 $x$），那么子序列的倒数第二项也就确定了，根据 [模运算的世界：当加减乘除遇上取模](https://leetcode.cn/circle/discuss/mDfnkW/)，倒数第二项为

$$
(m - x\bmod k + k) \bmod k
$$

> 加 $k$ 再模 $k$ 是为了在 $m < x\bmod k$ 时，保证计算结果非负。

类似方法一，从左到右遍历 $\textit{nums}$ 的同时，维护（实时更新）一个数组 $f[x]$，表示对于目前已经遍历过的数，最后一项为 $x$ 的子序列的最长长度。

对于 $x=\textit{nums}[i]$，我们可以在「最后一项为 $(m - x\bmod k + k) \bmod k$ 的子序列」的末尾添加 $x$，那么「最后一项为 $x$ 的子序列」的长度会增加 $1$，即

$$
f[x] = f[(m - x\bmod k + k) \bmod k] + 1
$$

> Python 取模更简单，由于允许负数下标，可以直接用 $f[m-x\bmod k]$ 作为转移来源。

遍历结束后（或者遍历中），用 $f[i]$ 更新答案的最大值。

```py [sol-Python3]
class Solution:
    def maximumLength(self, nums: List[int], k: int) -> int:
        ans = 0
        for m in range(k):
            f = [0] * k
            for x in nums:
                x %= k
                f[x] = f[m - x] + 1
            ans = max(ans, max(f))
        return ans
```

```java [sol-Java]
class Solution {
    public int maximumLength(int[] nums, int k) {
        int ans = 0;
        for (int m = 0; m < k; m++) {
            int[] f = new int[k];
            for (int x : nums) {
                x %= k;
                f[x] = f[(m - x + k) % k] + 1;
                ans = Math.max(ans, f[x]);
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maximumLength(vector<int>& nums, int k) {
        int ans = 0;
        for (int m = 0; m < k; m++) {
            vector<int> f(k);
            for (int x : nums) {
                x %= k;
                f[x] = f[(m - x + k) % k] + 1;
                ans = max(ans, f[x]);
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func maximumLength(nums []int, k int) (ans int) {
	f := make([]int, k)
	for m := 0; m < k; m++ {
		clear(f)
		for _, x := range nums {
			x %= k
			f[x] = f[(m-x+k)%k] + 1
			ans = max(ans, f[x])
		}
	}
	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(k(k+n))$，其中 $n$ 是 $\textit{nums}$ 的长度。注意创建大小为 $k$ 的数组需要 $\mathcal{O}(k)$ 的时间。
- 空间复杂度：$\mathcal{O}(k)$。

## 专题训练

见动态规划题单的「**§7.4 合法子序列 DP**」。

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

本题来自 `七、其他线性 DP / §7.4 合法子序列 DP`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `七、其他线性 DP / §7.4 合法子序列 DP`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
