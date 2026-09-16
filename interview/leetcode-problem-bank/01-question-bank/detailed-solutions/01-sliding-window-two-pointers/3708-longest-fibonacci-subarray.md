# 3708. 最长斐波那契子数组

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/longest-fibonacci-subarray/
- 题目 slug：`longest-fibonacci-subarray`
- 来源专题：滑动窗口与双指针
- 来源分类路径：六、分组循环
- 难度分：1381
- 外部题解来源：https://leetcode.cn/problems/longest-fibonacci-subarray/solutions/3803679/liang-chong-fang-fa-bao-li-mei-ju-yi-ci-cpg4t/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：暴力枚举 / 一次遍历（Python/Java/C++/Go）](https://leetcode.cn/problems/longest-fibonacci-subarray/solutions/3803679/liang-chong-fang-fa-bao-li-mei-ju-yi-ci-cpg4t/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`liang-chong-fang-fa-bao-li-mei-ju-yi-ci-cpg4t`
- topic id：`3803679`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 方法一：暴力枚举

枚举斐波那契子数组的左端点，暴力找最远右端点。

```py [sol-Python3]
class Solution:
    def longestSubarray(self, nums: List[int]) -> int:
        n = len(nums)
        ans = 2
        for i in range(n):  # 枚举斐波那契子数组的左端点
            j = i + 2
            while j < n and nums[j] == nums[j - 1] + nums[j - 2]:
                j += 1
            ans = max(ans, j - i)  # [i,j-1] 是斐波那契子数组
        return ans
```

```java [sol-Java]
class Solution {
    public int longestSubarray(int[] nums) {
        int n = nums.length;
        int ans = 2;
        for (int i = 0; i < n; i++) { // 枚举斐波那契子数组的左端点
            int j = i + 2;
            while (j < n && nums[j] == nums[j - 1] + nums[j - 2]) {
                j++;
            }
            ans = Math.max(ans, j - i); // [i,j-1] 是斐波那契子数组
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int longestSubarray(vector<int>& nums) {
        int n = nums.size();
        int ans = 2;
        for (int i = 0; i < n; i++) { // 枚举斐波那契子数组的左端点
            int j = i + 2;
            while (j < n && nums[j] == nums[j - 1] + nums[j - 2]) {
                j++;
            }
            ans = max(ans, j - i); // [i,j-1] 是斐波那契子数组
        }
        return ans;
    }
};
```

```go [sol-Go]
func longestSubarray(nums []int) int {
	ans := 2
	for i := range nums { // 枚举斐波那契子数组的左端点
		j := i + 2
		for j < len(nums) && nums[j] == nums[j-1]+nums[j-2] {
			j++
		}
		ans = max(ans, j-i) // [i,j-1] 是斐波那契子数组
	}
	return ans
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log U)$，其中 $n$ 是 $\textit{nums}$ 的长度，$U=\max(\textit{nums})$。由于斐波那契序列的增长速度是指数级的，所以斐波那契子数组的长度（内层循环的循环次数）至多为 $\mathcal{O}(\log U)$，其中 $U=\max(\textit{nums})$。
- 空间复杂度：$\mathcal{O}(1)$。

## 方法二：一次遍历

例如 $\textit{nums}=[1, 1, 2, 3, 5, 1, 6, 7, 13]$。我们从 $\textit{nums}[0]$ 开始遍历，可以找到一个长为 $5$ 的斐波那契子数组 $[1, 1, 2, 3, 5]$。这还意味着，从 $\textit{nums}[1]$ 开始遍历，得到的斐波那契子数组一定是 $[1, 2, 3, 5]$，比答案小。所以不需要考虑从下标 $1,2,3$ 开始，而是直接从 $\textit{nums}[4]=5$ 开始继续向后寻找。

⚠**注意**：斐波那契子数组的最后一个数，可能是下一个斐波那契子数组的第一个数。在上面的例子中，$5$ 是 $[1, 1, 2, 3, 5]$ 的最后一个数，同时也是 $[5, 1, 6, 7, 13]$ 的第一个数。

[本题视频讲解](https://www.bilibili.com/video/BV16E4uzLEdK/?t=23m43s)，欢迎点赞关注~

```py [sol-Python3]
class Solution:
    def longestSubarray(self, nums: List[int]) -> int:
        n = len(nums)
        ans = 2
        start = 0
        for i in range(2, n):
            if nums[i] != nums[i - 1] + nums[i - 2]:
                ans = max(ans, i - start)  # [start,i-1] 是斐波那契子数组
                start = i - 1
        return max(ans, n - start)  # [start,n-1] 是斐波那契子数组
```

```java [sol-Java]
class Solution {
    public int longestSubarray(int[] nums) {
        int n = nums.length;
        int ans = 2;
        int start = 0;
        for (int i = 2; i < n; i++) {
            if (nums[i] != nums[i - 1] + nums[i - 2]) {
                ans = Math.max(ans, i - start); // [start,i-1] 是斐波那契子数组
                start = i - 1;
            }
        }
        return Math.max(ans, n - start); // [start,n-1] 是斐波那契子数组
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int longestSubarray(vector<int>& nums) {
        int n = nums.size();
        int ans = 2;
        int start = 0;
        for (int i = 2; i < n; i++) {
            if (nums[i] != nums[i - 1] + nums[i - 2]) {
                ans = max(ans, i - start); // [start,i-1] 是斐波那契子数组
                start = i - 1;
            }
        }
        return max(ans, n - start); // [start,n-1] 是斐波那契子数组
    }
};
```

```go [sol-Go]
func longestSubarray(nums []int) int {
	n := len(nums)
	ans := 2
	start := 0
	for i := 2; i < n; i++ {
		if nums[i] != nums[i-1]+nums[i-2] {
			ans = max(ans, i-start) // [start,i-1] 是斐波那契子数组
			start = i - 1
		}
	}
	return max(ans, n-start) // [start,n-1] 是斐波那契子数组
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

## 变形题

改成子序列，怎么做？

这题是 [873. 最长的斐波那契子序列的长度](https://leetcode.cn/problems/length-of-longest-fibonacci-subsequence/)。

## 专题训练

见下面双指针题单的「**六、分组循环**」。

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

本题来自 `六、分组循环`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `六、分组循环`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
