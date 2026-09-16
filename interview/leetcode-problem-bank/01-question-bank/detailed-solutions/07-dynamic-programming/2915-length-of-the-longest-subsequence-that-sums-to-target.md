# 2915. 和为目标值的最长子序列的长度

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/length-of-the-longest-subsequence-that-sums-to-target/
- 题目 slug：`length-of-the-longest-subsequence-that-sums-to-target`
- 来源专题：动态规划
- 来源分类路径：三、背包 / §3.1 0-1 背包
- 难度分：1659
- 外部题解来源：https://leetcode.cn/problems/length-of-the-longest-subsequence-that-sums-to-target/solutions/2502839/mo-ban-qia-hao-zhuang-man-xing-0-1-bei-b-0nca/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[恰好装满型 0-1 背包：从记忆化搜索到递推（Python/Java/C++/Go）](https://leetcode.cn/problems/length-of-the-longest-subsequence-that-sums-to-target/solutions/2502839/mo-ban-qia-hao-zhuang-man-xing-0-1-bei-b-0nca/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`mo-ban-qia-hao-zhuang-man-xing-0-1-bei-b-0nca`
- topic id：`2502839`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 前置知识

请看视频讲解[【基础算法精讲 18】](https://www.bilibili.com/video/BV16Y411v7Y6/)。

## 一、记忆化搜索

本题是恰好装满型 0-1 背包，定义 $\textit{dfs}(i,j)$ 表示在 $\textit{nums}[0]$ 到 $\textit{nums}[i]$ 中的，元素和恰好等于 $j$ 的子序列的最长长度。如果不存在这样的子序列，则 $\textit{dfs}(i,j)=-\infty$。

考虑 $\textit{nums}[i]$ 选或不选：

- 不选：问题变成在 $\textit{nums}[0]$ 到 $\textit{nums}[i-1]$ 中的，元素和恰好等于 $j$ 的子序列的最长长度，即 $\textit{dfs}(i-1,j)$。
- 选，前提是 $\textit{nums}[i]\le j$：问题变成在 $\textit{nums}[0]$ 到 $\textit{nums}[i-1]$ 中的，元素和恰好等于 $j-\textit{nums}[i]$ 的子序列的最长长度，即 $\textit{dfs}(i-1,j-\textit{nums}[i])$。在这一子序列的末尾加上 $\textit{nums}[i]$，就是 $\textit{dfs}(i,j)$。

两种情况取最大值，得

$$
\textit{dfs}(i,j) = \max(\textit{dfs}(i-1,j),\textit{dfs}(i-1,j-\textit{nums}[i]) + 1)
$$

**递归边界**：

- $\textit{dfs}(-1,0)=0$。注意我们是从 $j=\textit{target}$ 开始倒着减小的，当 $j$ 减成 $0$，就表示找到了和为 $\textit{target}$ 的子序列。
- 其余 $\textit{dfs}(-1,j)=-\infty$。用 $-\infty$ 表示不合法的状态，从而保证 $\max$ 不会取到不合法的状态。

**递归入口**：$\textit{dfs}(n-1,\textit{target})$，这是原问题，也是答案。

> **注**：也可以在 $j=0$ 时直接返回 $0$，因为本题元素值都是正数。

```py [sol-Python3]
class Solution:
    def lengthOfLongestSubsequence(self, nums: List[int], target: int) -> int:
        @cache  # 缓存装饰器，避免重复计算 dfs（一行代码实现记忆化）
        def dfs(i: int, j: int) -> int:
            if i < 0:
                return 0 if j == 0 else -inf
            if nums[i] > j:
                return dfs(i - 1, j)
            return max(dfs(i - 1, j), dfs(i - 1, j - nums[i]) + 1)

        ans = dfs(len(nums) - 1, target)
        dfs.cache_clear()  # 防止爆内存
        return ans if ans > 0 else -1
```

```java [sol-Java]
class Solution {
    public int lengthOfLongestSubsequence(List<Integer> nums, int target) {
        Integer[] a = nums.toArray(Integer[]::new); // 转成数组处理，更快
        int n = a.length;
        int[][] memo = new int[n][target + 1];
        for (int[] row : memo) {
            Arrays.fill(row, -1); // -1 表示没有计算过
        }

        int ans = dfs(n - 1, target, a, memo);
        return ans > 0 ? ans : -1;
    }

    private int dfs(int i, int j, Integer[] nums, int[][] memo) {
        if (i < 0) {
            return j == 0 ? 0 : Integer.MIN_VALUE;
        }
        if (memo[i][j] != -1) { // 之前计算过
            return memo[i][j];
        }

        int res = dfs(i - 1, j, nums, memo); // 不选 nums[i]
        if (j >= nums[i]) {
            res = Math.max(res, dfs(i - 1, j - nums[i], nums, memo) + 1); // 选 nums[i]
        }
        return memo[i][j] = res; // 记忆化
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int lengthOfLongestSubsequence(vector<int>& nums, int target) {
        int n = nums.size();
        vector memo(n, vector<int>(target + 1, -1)); // -1 表示没有计算过
        auto dfs = [&](this auto&& dfs, int i, int j) -> int {
            if (i < 0) {
                return j == 0 ? 0 : INT_MIN;
            }
            int& res = memo[i][j]; // 注意这里是引用
            if (res != -1) { // 之前计算过
                return res;
            }

            res = dfs(i - 1, j); // 不选 nums[i]
            if (j >= nums[i]) {
                res = max(res, dfs(i - 1, j - nums[i]) + 1); // 选 nums[i]
            }
            return res;
        };

        int ans = dfs(n - 1, target);
        return ans > 0 ? ans : -1;
    }
};
```

```go [sol-Go]
func lengthOfLongestSubsequence(nums []int, target int) int {
	n := len(nums)
	memo := make([][]int, n)
	for i := range memo {
		memo[i] = make([]int, target+1)
		for j := range memo[i] {
			memo[i][j] = -1 // -1 表示没有计算过
		}
	}

	var dfs func(int, int) int
	dfs = func(i, j int) (res int) {
		if i < 0 {
			if j == 0 {
				return 0
			}
			return math.MinInt
		}
		p := &memo[i][j]
		if *p != -1 { // 之前计算过
			return *p
		}
		defer func() { *p = res }() // 记忆化
		if j < nums[i] {
			return dfs(i-1, j)
		}
		return max(dfs(i-1, j), dfs(i-1, j-nums[i])+1)
	}

	ans := dfs(n-1, target)
	if ans > 0 {
		return ans
	}
	return -1
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\cdot\textit{target})$，其中 $n$ 为 $\textit{nums}$ 的长度。由于每个状态只会计算一次，动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题状态个数等于 $\mathcal{O}(n\cdot\textit{target})$，单个状态的计算时间为 $\mathcal{O}(1)$，所以总的时间复杂度为 $\mathcal{O}(n\cdot\textit{target})$。
- 空间复杂度：$\mathcal{O}(n\cdot\textit{target})$。保存多少状态，就需要多少空间。

## 二、1:1 翻译成递推。

我们可以去掉递归中的「递」，只保留「归」的部分，即自底向上计算。

具体来说，$f[i+1][j]$ 的定义和 $\textit{dfs}(i,j)$ 的定义是一样的，都表示在 $\textit{nums}[0]$ 到 $\textit{nums}[i]$ 中的，元素和为 $j$ 的子序列的最长长度。这里 $+1$ 是为了把 $\textit{dfs}(-1,j)$ 这个状态也翻译过来，这样我们可以把 $f[0]$ 作为初始值。

相应的递推式（状态转移方程）也和 $\textit{dfs}$ 一样：

$$
f[i+1][j] = \max(f[i][j],f[i][j-\textit{nums}[i]] + 1)
$$

初始值 $f[0][0]=0$，其余为 $f[0][j] = -\infty$，翻译自递归边界 $\textit{dfs}(-1,0)=0$ 和 $\textit{dfs}(-1,j)=-\infty$。

答案为 $f[n][\textit{target}]$，翻译自递归入口 $\textit{dfs}(n-1,\textit{target})$。

```py [sol-Python3]
class Solution:
    def lengthOfLongestSubsequence(self, nums: List[int], target: int) -> int:
        n = len(nums)
        f = [[-inf] * (target + 1) for _ in range(n + 1)]
        f[0][0] = 0
        for i, x in enumerate(nums):
            for j in range(target + 1):
                if j < x:
                    f[i + 1][j] = f[i][j]
                else:
                    f[i + 1][j] = max(f[i][j], f[i][j - x] + 1)
        return f[n][-1] if f[n][-1] > 0 else -1
```

```java [sol-Java]
class Solution {
    public int lengthOfLongestSubsequence(List<Integer> nums, int target) {
        int n = nums.size();
        int[][] f = new int[n + 1][target + 1];
        Arrays.fill(f[0], Integer.MIN_VALUE);
        f[0][0] = 0;

        for (int i = 0; i < n; i++) {
            int x = nums.get(i);
            for (int j = 0; j <= target; j++) {
                if (j < x) {
                    f[i + 1][j] = f[i][j];
                } else {
                    f[i + 1][j] = Math.max(f[i][j], f[i][j - x] + 1);
                }
            }
        }

        return f[n][target] > 0 ? f[n][target] : -1;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int lengthOfLongestSubsequence(vector<int>& nums, int target) {
        int n = nums.size();
        vector f(n + 1, vector<int>(target + 1, INT_MIN));
        f[0][0] = 0;
        for (int i = 0; i < n; i++) {
            int x = nums[i];
            for (int j = 0; j <= target; j++) {
                if (j < x) {
                    f[i + 1][j] = f[i][j];
                } else {
                    f[i + 1][j] = max(f[i][j], f[i][j - x] + 1);
                }
            }
        }
        return f[n][target] > 0 ? f[n][target] : -1;
    }
};
```

```go [sol-Go]
func lengthOfLongestSubsequence(nums []int, target int) int {
	n := len(nums)
	f := make([][]int, n+1)
	for i := range f {
		f[i] = make([]int, target+1)
	}
	for j := 1; j <= target; j++ {
		f[0][j] = math.MinInt
	}

	for i, x := range nums {
		for j := range target + 1 {
			if j < x {
				f[i+1][j] = f[i][j]
			} else {
				f[i+1][j] = max(f[i][j], f[i][j-x]+1)
			}
		}
	}

	if f[n][target] > 0 {
		return f[n][target]
	}
	return -1
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\cdot\textit{target})$，其中 $n$ 为 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(n\cdot\textit{target})$。

## 三、空间优化

```py [sol-Python3]
class Solution:
    def lengthOfLongestSubsequence(self, nums: List[int], target: int) -> int:
        f = [0] + [-inf] * target
        for x in nums:
            for j in range(target, x - 1, -1):
                f[j] = max(f[j], f[j - x] + 1)
        return f[-1] if f[-1] > 0 else -1
```

```java [sol-Java]
class Solution {
    public int lengthOfLongestSubsequence(List<Integer> nums, int target) {
        int[] f = new int[target + 1];
        Arrays.fill(f, Integer.MIN_VALUE);
        f[0] = 0;
        for (int x : nums) {
            for (int j = target; j >= x; j--) {
                f[j] = Math.max(f[j], f[j - x] + 1);
            }
        }
        return f[target] > 0 ? f[target] : -1;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int lengthOfLongestSubsequence(vector<int>& nums, int target) {
        vector<int> f(target + 1, INT_MIN);
        f[0] = 0;
        for (int x : nums) {
            for (int j = target; j >= x; j--) {
                f[j] = max(f[j], f[j - x] + 1);
            }
        }
        return f[target] > 0 ? f[target] : -1;
    }
};
```

```go [sol-Go]
func lengthOfLongestSubsequence(nums []int, target int) int {
	f := make([]int, target+1)
	for i := 1; i <= target; i++ {
		f[i] = math.MinInt
	}
	for _, x := range nums {
		for j := target; j >= x; j-- {
			f[j] = max(f[j], f[j-x]+1)
		}
	}
	if f[target] > 0 {
		return f[target]
	}
	return -1
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\cdot\textit{target})$，其中 $n$ 为 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(\textit{target})$。

## 四、时间优化

例如 $\textit{nums}$ 前两个数的和是 $5$，那么枚举 $j$ 的时候，最大只需要枚举 $5$，而不是 $\textit{target}$。因为前两个数的和不可能比 $5$ 大，所以 $f[6],f[7],\cdots,f[\textit{target}]$ 一定都是 $-\infty$，无需计算。

```py [sol-Python3]
class Solution:
    def lengthOfLongestSubsequence(self, nums: List[int], target: int) -> int:
        f = [0] + [-inf] * target
        s = 0
        for x in nums:
            s = min(s + x, target)
            for j in range(s, x - 1, -1):
                t = f[j - x] + 1
                if t > f[j]:  # 手写 max 效率更高
                    f[j] = t
        return f[-1] if f[-1] > 0 else -1
```

```java [sol-Java]
class Solution {
    public int lengthOfLongestSubsequence(List<Integer> nums, int target) {
        int[] f = new int[target + 1];
        Arrays.fill(f, Integer.MIN_VALUE);
        f[0] = 0;
        int s = 0;
        for (int x : nums) {
            s = Math.min(s + x, target);
            for (int j = s; j >= x; j--) {
                f[j] = Math.max(f[j], f[j - x] + 1);
            }
        }
        return f[target] > 0 ? f[target] : -1;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int lengthOfLongestSubsequence(vector<int>& nums, int target) {
        vector<int> f(target + 1, INT_MIN);
        f[0] = 0;
        int s = 0;
        for (int x : nums) {
            s = min(s + x, target);
            for (int j = s; j >= x; j--) {
                f[j] = max(f[j], f[j - x] + 1);
            }
        }
        return f[target] > 0 ? f[target] : -1;
    }
};
```

```go [sol-Go]
func lengthOfLongestSubsequence(nums []int, target int) int {
	f := make([]int, target+1)
	for i := 1; i <= target; i++ {
		f[i] = math.MinInt
	}
	s := 0
	for _, x := range nums {
		s = min(s+x, target)
		for j := s; j >= x; j-- {
			f[j] = max(f[j], f[j-x]+1)
		}
	}
	if f[target] > 0 {
		return f[target]
	}
	return -1
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\cdot\textit{target})$，其中 $n$ 为 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(\textit{target})$。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与一般树（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/circle/discuss/K0n2gO/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `三、背包 / §3.1 0-1 背包`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `三、背包 / §3.1 0-1 背包`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
