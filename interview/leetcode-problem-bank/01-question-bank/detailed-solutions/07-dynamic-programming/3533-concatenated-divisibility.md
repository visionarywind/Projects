# 3533. 判断连接可整除性

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/concatenated-divisibility/
- 题目 slug：`concatenated-divisibility`
- 来源专题：动态规划
- 来源分类路径：九、状态压缩 DP（状压 DP） / §9.1 排列型状压 DP ① 相邻无关
- 难度分：2257
- 外部题解来源：https://leetcode.cn/problems/concatenated-divisibility/solutions/3663246/quan-pai-lie-bao-sou-pythonjavacgo-by-en-l4zv/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[全排列暴搜（Python/Java/C++/Go）](https://leetcode.cn/problems/concatenated-divisibility/solutions/3663246/quan-pai-lie-bao-sou-pythonjavacgo-by-en-l4zv/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`quan-pai-lie-bao-sou-pythonjavacgo-by-en-l4zv`
- topic id：`3663246`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

首先读清楚题目，要计算的是字典序最小的**数组**，不是拼接后的数。

为了让字典序最小，我们要从小到大枚举。把 $\textit{nums}$ 从小到大排序，然后写一个类似 [46. 全排列](https://leetcode.cn/problems/permutations/) 的暴搜：

- 枚举答案的第一个位置填 $\textit{nums}[0],\textit{nums}[1],\ldots,\textit{nums}[n-1]$。
- 枚举答案的第二个位置填 $\textit{nums}[0],\textit{nums}[1],\ldots,\textit{nums}[n-1]$。但不能填之前填过的数字。
- 依此类推。

在枚举的过程中，维护拼接的数字模 $k$ 的结果。为什么可以在中途取模，请看 [模运算的世界：当加减乘除遇上取模](https://leetcode.cn/circle/discuss/mDfnkW/)。

一旦我们找到了答案（拼接的 $n$ 个数模 $k$ 等于 $0$），就立刻返回 $\texttt{true}$，不再继续递归搜索。

注意有重复的状态，比如 $k=2$，先填 $2$ 再填 $4$，$24\bmod k = 0$；先填 $4$ 再填 $2$，$42\bmod k = 0$，都会递归到「选了 $2$ 和 $4$，且当前拼接的数字模 $k$ 为 $0$」的状态。

为了避免重复访问同样的状态，用一个 $\textit{vis}$ 数组记录访问过的状态。由于我们找到了答案就立刻返回 $\texttt{true}$，如果重复访问同样的状态，那么继续递归一定无法找到答案，应该返回 $\texttt{false}$。

## 细节

用集合 $S$ 表示剩余未填（没有选过的）数字的**下标**，$x$ 表示当前拼接的数字模 $k$ 的结果。

枚举 $S$ 中的下标 $i$，填入 $\textit{nums}[i]$，那么：

- $S$ 变成 $S\setminus \{i\}$。
- $x$ 变成 $(x\cdot 10^L + \textit{nums}[i])\bmod k$，其中 $L$ 是 $\textit{nums}[i]$ 的十进制长度。

递归入口：$\textit{dfs}(U,0)$，其中全集 $U=\{0,1,2,\ldots,n-1\}$。

递归边界：$\textit{dfs}(\varnothing,0)=\texttt{true}$，其余 $\textit{dfs}(\varnothing,x)=\texttt{false}$。

**代码实现时，用二进制表示集合，用位运算实现集合操作，具体请看** [从集合论到位运算，常见位运算技巧分类总结](https://leetcode.cn/circle/discuss/CaOJ45/)。

具体请看 [视频讲解](https://www.bilibili.com/video/BV1BgjAzcE7k/?t=10m43s)，欢迎点赞关注~

```py [sol-Python3]
class Solution:
    def concatenatedDivisibility(self, nums: List[int], k: int) -> List[int]:
        nums.sort()
        pow10 = [10 ** len(str(x)) for x in nums]

        ans = []
        @cache  # 充当 vis
        def dfs(s: int, x: int) -> bool:
            if s == 0:
                return x == 0
            # 枚举在 s 中的下标 i
            for i, (p10, v) in enumerate(zip(pow10, nums)):
                if s & (1 << i) and dfs(s ^ (1 << i), (x * p10 + v) % k):
                    ans.append(v)
                    return True
            return False

        if not dfs((1 << len(nums)) - 1, 0):
            return []
        ans.reverse()  # nums[i] 是倒序加入答案的，所以要反转
        return ans
```

```java [sol-Java]
class Solution {
    public int[] concatenatedDivisibility(int[] nums, int k) {
        Arrays.sort(nums);
        int n = nums.length;
        int[] pow10 = new int[n];
        for (int i = 0; i < n; i++) {
            pow10[i] = (int) Math.pow(10, Integer.toString(nums[i]).length());
        }

        int[] ans = new int[n];
        boolean[][] vis = new boolean[1 << n][k];
        if (!dfs((1 << n) - 1, 0, nums, pow10, k, vis, ans)) {
            return new int[]{};
        }
        return ans;
    }

    private boolean dfs(int s, int x, int[] nums, int[] pow10, int k, boolean[][] vis, int[] ans) {
        if (s == 0) {
            return x == 0;
        }
        if (vis[s][x]) {
            return false;
        }
        vis[s][x] = true;
        // 枚举在 s 中的下标 i
        for (int i = 0; i < nums.length; i++) {
            if ((s & (1 << i)) > 0 && dfs(s ^ (1 << i), (x * pow10[i] + nums[i]) % k, nums, pow10, k, vis, ans)) {
                ans[nums.length - Integer.bitCount(s)] = nums[i];
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
    vector<int> concatenatedDivisibility(vector<int>& nums, int k) {
        ranges::sort(nums);
        int n = nums.size();
        vector<int> pow10(n);
        for (int i = 0; i < n; i++) {
            pow10[i] = pow(10, to_string(nums[i]).size());
        }

        vector<int> ans;
        vector vis(1 << n, vector<uint8_t>(k));
        auto dfs = [&](this auto&& dfs, int s, int x) -> bool {
            if (s == 0) {
                return x == 0;
            }
            if (vis[s][x]) {
                return false;
            }
            vis[s][x] = true;
            // 枚举在 s 中的下标 i
            for (int i = 0; i < n; i++) {
                if (s & (1 << i) && dfs(s ^ (1 << i), (x * pow10[i] + nums[i]) % k)) {
                    ans.push_back(nums[i]);
                    return true;
                }
            }
            return false;
        };
        if (!dfs((1 << n) - 1, 0)) {
            return {};
        }
        ranges::reverse(ans); // nums[i] 是倒序加入答案的，所以要反转
        return ans;
    }
};
```

```go [sol-Go]
func concatenatedDivisibility(nums []int, k int) []int {
	slices.Sort(nums)
	n := len(nums)
	pow10 := make([]int, n)
	for i, x := range nums {
		pow10[i] = int(math.Pow10(len(strconv.Itoa(x))))
	}

	ans := make([]int, 0, n)
	vis := make([][]bool, 1<<n)
	for i := range vis {
		vis[i] = make([]bool, k)
	}
	var dfs func(int, int) bool
	dfs = func(s, x int) bool {
		if s == 0 {
			return x == 0
		}
		if vis[s][x] {
			return false
		}
		vis[s][x] = true
		// 枚举在 s 中的下标 i
		for t := uint(s); t > 0; t &= t - 1 {
			i := bits.TrailingZeros(t)
			if dfs(s^1<<i, (x*pow10[i]+nums[i])%k) {
				ans = append(ans, nums[i])
				return true
			}
		}
		return false
	}
	if !dfs(1<<n-1, 0) {
		return nil
	}
	slices.Reverse(ans) // nums[i] 是倒序加入答案的，所以要反转
	return ans
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(nk2^n)$，其中 $n$ 是 $\textit{nums}$ 的长度。$s$ 有 $\mathcal{O}(2^n)$ 个，$x$ 有 $\mathcal{O}(k)$ 个，所以总共有 $\mathcal{O}(k2^n)$ 个状态。每个状态至多访问一次。每个状态需要花费 $\mathcal{O}(n)$ 的时间枚举下标 $i$。
- 空间复杂度：$\mathcal{O}(k2^n)$。

## 思考题

如果要计算的是「拼接数字」的最小字典序呢？

欢迎在评论区分享你的思路/代码。

## 相似题目（字典序+暴搜）

- [3260. 找出最大的 N 位 K 回文数](https://leetcode.cn/problems/find-the-largest-palindrome-divisible-by-k/) 2370
- [3348. 最小可整除数位乘积 II](https://leetcode.cn/problems/smallest-divisible-digit-product-ii/) 3101

更多相似题目，见下面动态规划题单的「**§9.1 排列型 ① 相邻无关**」。虽然本题不是 DP，但思路是类似的。

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

## 细节

用集合 $S$ 表示剩余未填（没有选过的）数字的**下标**，$x$ 表示当前拼接的数字模 $k$ 的结果。

枚举 $S$ 中的下标 $i$，填入 $\textit{nums}[i]$，那么：

- $S$ 变成 $S\setminus \{i\}$。
- $x$ 变成 $(x\cdot 10^L + \textit{nums}[i])\bmod k$，其中 $L$ 是 $\textit{nums}[i]$ 的十进制长度。

递归入口：$\textit{dfs}(U,0)$，其中全集 $U=\{0,1,2,\ldots,n-1\}$。

递归边界：$\textit{dfs}(\varnothing,0)=\texttt{true}$，其余 $\textit{dfs}(\varnothing,x)=\texttt{false}$。

**代码实现时，用二进制表示集合，用位运算实现集合操作，具体请看** [从集合论到位运算，常见位运算技巧分类总结](https://leetcode.cn/circle/discuss/CaOJ45/)。

具体请看 [视频讲解](https://www.bilibili.com/video/BV1BgjAzcE7k/?t=10m43s)，欢迎点赞关注~

```py [sol-Python3]
class Solution:
    def concatenatedDivisibility(self, nums: List[int], k: int) -> List[int]:
        nums.sort()
        pow10 = [10 ** len(str(x)) for x in nums]

        ans = []
        @cache  # 充当 vis
        def dfs(s: int, x: int) -> bool:
            if s == 0:
                return x == 0
            # 枚举在 s 中的下标 i
            for i, (p10, v) in enumerate(zip(pow10, nums)):
                if s & (1 << i) and dfs(s ^ (1 << i), (x * p10 + v) % k):
                    ans.append(v)
                    return True
            return False

        if not dfs((1 << len(nums)) - 1, 0):
            return []
        ans.reverse()  # nums[i] 是倒序加入答案的，所以要反转
        return ans
```

```java [sol-Java]
class Solution {
    public int[] concatenatedDivisibility(int[] nums, int k) {
        Arrays.sort(nums);
        int n = nums.length;
        int[] pow10 = new int[n];
        for (int i = 0; i < n; i++) {
            pow10[i] = (int) Math.pow(10, Integer.toString(nums[i]).length());
        }

        int[] ans = new int[n];
        boolean[][] vis = new boolean[1 << n][k];
        if (!dfs((1 << n) - 1, 0, nums, pow10, k, vis, ans)) {
            return new int[]{};
        }
        return ans;
    }

    private boolean dfs(int s, int x, int[] nums, int[] pow10, int k, boolean[][] vis, int[] ans) {
        if (s == 0) {
            return x == 0;
        }
        if (vis[s][x]) {
            return false;
        }
        vis[s][x] = true;
        // 枚举在 s 中的下标 i
        for (int i = 0; i < nums.length; i++) {
            if ((s & (1 << i)) > 0 && dfs(s ^ (1 << i), (x * pow10[i] + nums[i]) % k, nums, pow10, k, vis, ans)) {
                ans[nums.length - Integer.bitCount(s)] = nums[i];
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
    vector<int> concatenatedDivisibility(vector<int>& nums, int k) {
        ranges::sort(nums);
        int n = nums.size();
        vector<int> pow10(n);
        for (int i = 0; i < n; i++) {
            pow10[i] = pow(10, to_string(nums[i]).size());
        }

        vector<int> ans;
        vector vis(1 << n, vector<uint8_t>(k));
        auto dfs = [&](this auto&& dfs, int s, int x) -> bool {
            if (s == 0) {
                return x == 0;
            }
            if (vis[s][x]) {
                return false;
            }
            vis[s][x] = true;
            // 枚举在 s 中的下标 i
            for (int i = 0; i < n; i++) {
                if (s & (1 << i) && dfs(s ^ (1 << i), (x * pow10[i] + nums[i]) % k)) {
                    ans.push_back(nums[i]);
                    return true;
                }
            }
            return false;
        };
        if (!dfs((1 << n) - 1, 0)) {
            return {};
        }
        ranges::reverse(ans); // nums[i] 是倒序加入答案的，所以要反转
        return ans;
    }
};
```

```go [sol-Go]
func concatenatedDivisibility(nums []int, k int) []int {
	slices.Sort(nums)
	n := len(nums)
	pow10 := make([]int, n)
	for i, x := range nums {
		pow10[i] = int(math.Pow10(len(strconv.Itoa(x))))
	}

	ans := make([]int, 0, n)
	vis := make([][]bool, 1<<n)
	for i := range vis {
		vis[i] = make([]bool, k)
	}
	var dfs func(int, int) bool
	dfs = func(s, x int) bool {
		if s == 0 {
			return x == 0
		}
		if vis[s][x] {
			return false
		}
		vis[s][x] = true
		// 枚举在 s 中的下标 i
		for t := uint(s); t > 0; t &= t - 1 {
			i := bits.TrailingZeros(t)
			if dfs(s^1<<i, (x*pow10[i]+nums[i])%k) {
				ans = append(ans, nums[i])
				return true
			}
		}
		return false
	}
	if !dfs(1<<n-1, 0) {
		return nil
	}
	slices.Reverse(ans) // nums[i] 是倒序加入答案的，所以要反转
	return ans
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(nk2^n)$，其中 $n$ 是 $\textit{nums}$ 的长度。$s$ 有 $\mathcal{O}(2^n)$ 个，$x$ 有 $\mathcal{O}(k)$ 个，所以总共有 $\mathcal{O}(k2^n)$ 个状态。每个状态至多访问一次。每个状态需要花费 $\mathcal{O}(n)$ 的时间枚举下标 $i$。
- 空间复杂度：$\mathcal{O}(k2^n)$。

## 思考题

如果要计算的是「拼接数字」的最小字典序呢？

欢迎在评论区分享你的思路/代码。

## 相似题目（字典序+暴搜）

- [3260. 找出最大的 N 位 K 回文数](https://leetcode.cn/problems/find-the-largest-palindrome-divisible-by-k/) 2370
- [3348. 最小可整除数位乘积 II](https://leetcode.cn/problems/smallest-divisible-digit-product-ii/) 3101

更多相似题目，见下面动态规划题单的「**§9.1 排列型 ① 相邻无关**」。虽然本题不是 DP，但思路是类似的。

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

本题来自 `九、状态压缩 DP（状压 DP） / §9.1 排列型状压 DP ① 相邻无关`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `九、状态压缩 DP（状压 DP） / §9.1 排列型状压 DP ① 相邻无关`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
