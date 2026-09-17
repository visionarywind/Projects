# 2044. 统计按位或能得到最大值的子集数目

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/count-number-of-maximum-bitwise-or-subsets/
- 题目 slug：`count-number-of-maximum-bitwise-or-subsets`
- 来源专题：动态规划
- 来源分类路径：九、状态压缩 DP（状压 DP） / §9.6 SOS DP
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/count-number-of-maximum-bitwise-or-subsets/solutions/1051495/o2n-zi-ji-huo-xie-fa-by-endlesscheng-bczp/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[子集型回溯 + 剪枝（Python/Java/C++/Go）](https://leetcode.cn/problems/count-number-of-maximum-bitwise-or-subsets/solutions/1051495/o2n-zi-ji-huo-xie-fa-by-endlesscheng-bczp/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`o2n-zi-ji-huo-xie-fa-by-endlesscheng-bczp`
- topic id：`1051495`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 16:46:02 +0800

首先计算出 OR 运算结果的最大值。由于 OR 的性质是，参与 OR 的元素越多，结果越大，所以最大是 $\textit{nums}$ 所有元素的 OR，记作 $\textit{totalOr}$。

然后枚举 $\textit{nums}$ 的所有 [78. 子集](https://leetcode.cn/problems/subsets/)，在枚举子集的过程中，用一个参数 $\textit{subsetOr}$ 表示子集的 OR 值。递归到终点时，如果 $\textit{subsetOr}=\textit{totalOr}$，那么答案加一。

## 答疑

**问**：为什么不需要写恢复现场？

**答**：$\textit{subsetOr}$ 是 $\texttt{int}$ 类型，每次往下递归都会复制一份，每个 DFS 各自持有一份独立的 $\textit{subsetOr}$。在当前 DFS 中修改 $\textit{subsetOr}$ 并不会影响上层 DFS 中的 $\textit{subsetOr}$ 值。但是，如果把 $\textit{subsetOr}$ 写在 DFS 外面（而不是作为参数），就需要写恢复现场了。

## 优化前

```py [sol-Python3]
class Solution:
    def countMaxOrSubsets(self, nums: List[int]) -> int:
        total_or = reduce(or_, nums)
        n = len(nums)
        ans = 0

        def dfs(i: int, subset_or: int) -> None:
            if i == n:
                if subset_or == total_or:
                    nonlocal ans
                    ans += 1
                return
            dfs(i + 1, subset_or) # 不选 nums[i]
            dfs(i + 1, subset_or | nums[i])  # 选 nums[i]

        dfs(0, 0)
        return ans
```

```java [sol-Java]
class Solution {
    public int countMaxOrSubsets(int[] nums) {
        int totalOr = 0;
        for (int x : nums) {
            totalOr |= x;
        }

        dfs(0, 0, nums, totalOr);
        return ans;
    }

    private int ans = 0;

    private void dfs(int i, int subsetOr, int[] nums, int totalOr) {
        if (i == nums.length) {
            if (subsetOr == totalOr) {
                ans++;
            }
            return;
        }
        dfs(i + 1, subsetOr, nums, totalOr); // 不选 nums[i]
        dfs(i + 1, subsetOr | nums[i], nums, totalOr); // 选 nums[i]
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int countMaxOrSubsets(vector<int>& nums) {
        int total_or = reduce(nums.begin(), nums.end(), 0, bit_or());
        int ans = 0;

        auto dfs = [&](this auto&& dfs, int i, int subset_or) -> void {
            if (i == nums.size()) {
                if (subset_or == total_or) {
                    ans++;
                }
                return;
            }
            dfs(i + 1, subset_or); // 不选 nums[i]
            dfs(i + 1, subset_or | nums[i]); // 选 nums[i]
        };

        dfs(0, 0);
        return ans;
    }
};
```

```go [sol-Go]
func countMaxOrSubsets(nums []int) (ans int) {
	totalOr := 0
	for _, x := range nums {
		totalOr |= x
	}

	var dfs func(int, int)
	dfs = func(i, subsetOr int) {
		if i == len(nums) {
			if subsetOr == totalOr {
				ans++
			}
			return
		}
		dfs(i+1, subsetOr)         // 不选 nums[i]
		dfs(i+1, subsetOr|nums[i]) // 选 nums[i]
	}

	dfs(0, 0)
	return
}
```

## 优化

递归到 $\textit{nums}[i]$ 时，如果发现 $\textit{subsetOr}$ 已经等于 $\textit{totalOr}$，那么 $\textit{subsetOr}$ 不可能再变大。所以下标在 $[i,n-1]$ 中的每个数无论是选还是不选，对应的子集 OR 值都等于 $\textit{totalOr}$。这一共有 $2^{n-i}$ 种方案，直接加入答案，不继续递归。

```py [sol-Python3]
class Solution:
    def countMaxOrSubsets(self, nums: List[int]) -> int:
        total_or = reduce(or_, nums)
        n = len(nums)
        ans = 0

        def dfs(i: int, subset_or: int) -> None:
            if subset_or == total_or:
                nonlocal ans
                ans += 1 << (n - i)
                return
            if i == n:
                return
            dfs(i + 1, subset_or) # 不选 nums[i]
            dfs(i + 1, subset_or | nums[i])  # 选 nums[i]

        dfs(0, 0)
        return ans
```

```java [sol-Java]
class Solution {
    public int countMaxOrSubsets(int[] nums) {
        int totalOr = 0;
        for (int x : nums) {
            totalOr |= x;
        }

        dfs(0, 0, nums, totalOr);
        return ans;
    }

    private int ans = 0;

    private void dfs(int i, int subsetOr, int[] nums, int totalOr) {
        if (subsetOr == totalOr) {
            ans += 1 << (nums.length - i);
            return;
        }
        if (i == nums.length) {
            return;
        }
        dfs(i + 1, subsetOr, nums, totalOr); // 不选 nums[i]
        dfs(i + 1, subsetOr | nums[i], nums, totalOr); // 选 nums[i]
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int countMaxOrSubsets(vector<int>& nums) {
        int total_or = reduce(nums.begin(), nums.end(), 0, bit_or());
        int n = nums.size();
        int ans = 0;

        auto dfs = [&](this auto&& dfs, int i, int subset_or) -> void {
            if (subset_or == total_or) {
                ans += 1 << (n - i);
                return;
            }
            if (i == n) {
                return;
            }
            dfs(i + 1, subset_or); // 不选 nums[i]
            dfs(i + 1, subset_or | nums[i]); // 选 nums[i]
        };

        dfs(0, 0);
        return ans;
    }
};
```

```go [sol-Go]
func countMaxOrSubsets(nums []int) (ans int) {
	n := len(nums)
	totalOr := 0
	for _, x := range nums {
		totalOr |= x
	}

	var dfs func(int, int)
	dfs = func(i, subsetOr int) {
		if subsetOr == totalOr {
			ans += 1 << (n - i)
			return
		}
		if i == n {
			return
		}
		dfs(i+1, subsetOr)         // 不选 nums[i]
		dfs(i+1, subsetOr|nums[i]) // 选 nums[i]
	}

	dfs(0, 0)
	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(2^n)$，其中 $n$ 是 $\textit{nums}$ 的长度。搜索树是一棵高为 $\mathcal{O}(n)$ 的满二叉树，有 $\mathcal{O}(2^n)$ 个节点，遍历这棵搜索树需要 $\mathcal{O}(2^n)$ 的时间。
- 空间复杂度：$\mathcal{O}(n)$。递归需要 $\mathcal{O}(n)$ 的栈空间。

**注**：本题可以归约为 set cover 计数问题，是 NP-hard。

## 专题训练

见下面回溯题单的「**§4.2 子集型回溯**」。

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

本题来自 `九、状态压缩 DP（状压 DP） / §9.6 SOS DP`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `九、状态压缩 DP（状压 DP） / §9.6 SOS DP`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
