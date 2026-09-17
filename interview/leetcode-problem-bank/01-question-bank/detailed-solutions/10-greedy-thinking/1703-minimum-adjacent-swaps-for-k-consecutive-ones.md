# 1703. 得到连续 K 个 1 的最少相邻交换次数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-adjacent-swaps-for-k-consecutive-ones/
- 题目 slug：`minimum-adjacent-swaps-for-k-consecutive-ones`
- 来源专题：贪心与思维
- 来源分类路径：四、数学贪心 / §4.5 中位数贪心
- 难度分：2467
- 外部题解来源：https://leetcode.cn/problems/minimum-adjacent-swaps-for-k-consecutive-ones/solutions/2024387/tu-jie-zhuan-huan-cheng-zhong-wei-shu-ta-iz4v/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【图解】转换成中位数贪心+空间优化（Python/Java/C++/Go）](https://leetcode.cn/problems/minimum-adjacent-swaps-for-k-consecutive-ones/solutions/2024387/tu-jie-zhuan-huan-cheng-zhong-wei-shu-ta-iz4v/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tu-jie-zhuan-huan-cheng-zhong-wei-shu-ta-iz4v`
- topic id：`2024387`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:07:12 +0800

![1703-2.png](https://pic.leetcode.cn/1671286198-JctUsi-1703-2.png)

#### 算法小课堂：前缀和

定义前缀和 $s[0]=0$，$s[i+1] = \sum\limits_{j=0}^{i}p[j]$。

例如 $p=[2,3,3,5]$，对应的前缀和数组为 $s=[0, 2, 5, 8, 13]$。

通过前缀和，我们可以把**子数组的和转换成两个前缀和的差**，即

$$
\sum_{j=\textit{left}}^{\textit{right}}p[j] = \sum\limits_{j=0}^{\textit{right}}p[j] - \sum\limits_{j=0}^{\textit{left}-1}p[j] = s[\textit{right}+1] - s[\textit{left}]
$$

例如 $p$ 的子数组 $[3,3,5]$ 的和就可以用 $s[4]-s[1]=13-2=11$ 算出来。

> 注：为方便计算，常用左闭右开区间 $[\textit{left},\textit{right})$ 来表示子数组，此时子数组的和为 $s[\textit{right}]-s[\textit{left}]$。

#### 答疑

**问**：为什么图中的 $x$ 取在中位数上是最优的？

**答**：首先，如果 $x$ 取在区间 $[p[0],p[k-1]]$ 之外，那么 $x$ 向区间方向移动可以使距离和变小；同时，如果 $x$ 取在区间 $[p[0],p[k-1]]$ 之内，无论如何移动 $x$，它到 $p[0]$ 和 $p[k-1]$ 的距离和都是一个定值 $p[k-1]-p[0]$，那么去掉 $p[0]$ 和 $p[k-1]$ 这两个最左最右的数，问题规模缩小。不断缩小问题规模，如果最后剩下 $1$ 个数，那么 $x$ 就取它；如果最后剩下 $2$ 个数，那么 $x$ 取这两个数之间的任意值都可以（包括这两个数）。因此中位数可以取 $p[k/2]$。

```py [sol1-Python3]
class Solution:
    def minMoves(self, nums: List[int], k: int) -> int:
        p = [q - i for i, q in enumerate(i for i, x in enumerate(nums) if x)]
        s = list(accumulate(p, initial=0))  # p 的前缀和
        return min(s[i] + s[i + k] - s[i + k // 2] * 2 - p[i + k // 2] * (k % 2)
                   for i in range(len(p) - k + 1))  # p[i:i+k] 中所有数到 p[i+k//2] 的距离之和，取最小值
```

```java [sol1-Java]
class Solution {
    public int minMoves(int[] nums, int k) {
        var p = new ArrayList<Integer>();
        for (int i = 0; i < nums.length; ++i)
            if (nums[i] != 0) p.add(i - p.size());
        int m = p.size();
        int[] s = new int[m + 1]; // p 的前缀和
        for (int i = 0; i < m; i++)
            s[i + 1] = s[i] + p.get(i);
        int ans = Integer.MAX_VALUE;
        for (int i = 0; i <= m - k; ++i) // p[i] 到 p[i+k-1] 中所有数到 p[i+k/2] 的距离之和，取最小值
            ans = Math.min(ans, s[i] + s[i + k] - s[i + k / 2] * 2 - p.get(i + k / 2) * (k % 2));
        return ans;
    }
}
```

```cpp [sol1-C++]
class Solution {
public:
    int minMoves(vector<int> &nums, int k) {
        vector<int> p;
        for (int i = 0; i < nums.size(); ++i)
            if (nums[i]) p.push_back(i - p.size());
        int m = p.size(), s[m + 1];
        s[0] = 0;
        partial_sum(p.begin(), p.end(), s + 1); // p 的前缀和
        int ans = INT_MAX;
        for (int i = 0; i <= m - k; ++i) // p[i] 到 p[i+k-1] 中所有数到 p[i+k/2] 的距离之和，取最小值
            ans = min(ans, s[i] + s[i + k] - s[i + k / 2] * 2 - p[i + k / 2] * (k % 2));
        return ans;
    }
};
```

```go [sol1-Go]
func minMoves(nums []int, k int) int {
	p := []int{}
	for i, v := range nums {
		if v != 0 {
			p = append(p, i-len(p))
		}
	}
	m := len(p)
	s := make([]int, m+1) // p 的前缀和
	for i, v := range p {
		s[i+1] = s[i] + v
	}
	ans := math.MaxInt
	for i, v := range s[:m-k+1] { // p[i] 到 p[i+k-1] 中所有数到 p[i+k/2] 的距离之和，取最小值
		ans = min(ans, v+s[i+k]-s[i+k/2]*2-p[i+k/2]*(k%2))
	}
	return ans
}

func min(a, b int) int { if b < a { return b }; return a }
```

#### 优化

在遍历 $\textit{nums}$ 的同时，把 $p$ 直接存到 $\textit{nums}$ 中，并累加计算 $s[i],s[i+k/2],s[i+k]$，这样可以做到 $O(1)$ 额外空间。

> 注：还可以用三指针在不修改 $\textit{nums}$ 的情况下做到 $O(1)$ 额外空间，感兴趣的读者可以试试。

```py [sol2-Python3]
class Solution:
    def minMoves(self, nums: List[int], k: int) -> int:
        m = 0
        for i, p in enumerate(i for i, x in enumerate(nums) if x):
            nums[i] = p - i
            m += 1
        if m == len(nums): return 0  # 全部都是 1
        ans, p = inf, nums
        sl, sm, sr = 0, sum(p[:k // 2]), sum(p[:k])  # s[i] s[i+k//2] s[i+k] 忽略切片开销
        for i in range(m - k + 1):
            ans = min(ans, sl + sr - sm * 2 - p[i + k // 2] * (k % 2))
            sl += p[i]
            sm += p[i + k // 2]
            sr += p[i + k]
        return ans
```

```java [sol2-Java]
class Solution {
    public int minMoves(int[] nums, int k) {
        int n = nums.length, m = 0;
        for (int i = 0; i < n; ++i)
            if (nums[i] != 0) {
                nums[m] = i - m;
                ++m;
            }
        if (m == n) return 0; // 全部都是 1
        int[] p = nums;
        int sl = 0, sm = 0, sr = 0; // s[i] s[i+k/2] s[i+k]
        for (int i = 0; i < k; ++i) {
            if (i < k / 2) sm += p[i];
            sr += p[i];
        }
        int ans = Integer.MAX_VALUE;
        for (int i = 0; i <= m - k; ++i) {
            ans = Math.min(ans, sl + sr - sm * 2 - p[i + k / 2] * (k % 2));
            sl += p[i];
            sm += p[i + k / 2];
            sr += p[i + k];
        }
        return ans;
    }
}
```

```cpp [sol2-C++]
class Solution {
public:
    int minMoves(vector<int> &nums, int k) {
        int n = nums.size(), m = 0;
        for (int i = 0; i < n; ++i)
            if (nums[i]) {
                nums[m] = i - m;
                ++m;
            }
        if (m == n) return 0; // 全部都是 1
        auto &p = nums;
        int sl = 0; // s[i]
        int sm = accumulate(p.begin(), p.begin() + k / 2, 0); // s[i+k/2]
        int sr = accumulate(p.begin(), p.begin() + k, 0); // s[i+k]
        int ans = INT_MAX;
        for (int i = 0; i <= m - k; ++i) {
            ans = min(ans, sl + sr - sm * 2 - p[i + k / 2] * (k % 2));
            sl += p[i];
            sm += p[i + k / 2];
            sr += p[i + k];
        }
        return ans;
    }
};
```

```go [sol2-Go]
func minMoves(nums []int, k int) int {
	n, m := len(nums), 0
	for i, p := range nums {
		if p != 0 {
			nums[m] = i - m
			m++
		}
	}
	if m == n { // 全部都是 1
		return 0
	}
	p := nums
	var sl, sm, sr int // s[i] s[i+k/2] s[i+k]
	for i, v := range p[:k] {
		if i < k/2 {
			sm += v
		}
		sr += v
	}
	ans := math.MaxInt
	for i, v := range p[:m-k+1] {
		ans = min(ans, sl+sr-sm*2-p[i+k/2]*(k%2))
		sl += v
		sm += p[i+k/2]
		sr += p[i+k]
	}
	return ans
}

func min(a, b int) int { if b < a { return b }; return a }
```

#### 复杂度分析

- 时间复杂度：$O(n)$，其中 $n$ 为 $\textit{nums}$ 的长度。
- 空间复杂度：$O(1)$，仅用到若干额外变量。

#### 相似题目（中位数贪心）

- [462. 最小操作次数使数组元素相等 II](https://leetcode.cn/problems/minimum-moves-to-equal-array-elements-ii/)
- [2033. 获取单值网格的最小操作数](https://leetcode.cn/problems/minimum-operations-to-make-a-uni-value-grid/)
- [2448. 使数组相等的最小开销](https://leetcode.cn/problems/minimum-cost-to-make-array-equal/)

---

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)，最近正在连载 [【基础算法精讲】](https://www.bilibili.com/video/BV1AP41137w7/)，欢迎点赞~

## 本地原创解析

### 1. 题意重述

本题来自 `四、数学贪心 / §4.5 中位数贪心`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `四、数学贪心 / §4.5 中位数贪心`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
