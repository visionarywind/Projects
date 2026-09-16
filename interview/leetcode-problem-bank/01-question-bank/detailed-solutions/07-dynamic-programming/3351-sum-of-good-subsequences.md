# 3351. 好子序列的元素之和

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/sum-of-good-subsequences/
- 题目 slug：`sum-of-good-subsequences`
- 来源专题：动态规划
- 来源分类路径：七、其他线性 DP / §7.4 合法子序列 DP
- 难度分：2086
- 外部题解来源：https://leetcode.cn/problems/sum-of-good-subsequences/solutions/2983496/te-shu-zi-xu-lie-dppythonjavacgo-by-endl-vv7e/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[子序列计数 DP（Python/Java/C++/Go）](https://leetcode.cn/problems/sum-of-good-subsequences/solutions/2983496/te-shu-zi-xu-lie-dppythonjavacgo-by-endl-vv7e/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`te-shu-zi-xu-lie-dppythonjavacgo-by-endl-vv7e`
- topic id：`2983496`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

套路题，见 [动态规划题单](https://leetcode.cn/circle/discuss/tXLS3i/) 中的 §7.2 节。

这些题目，一般都定义 $f[x]$ 表示以元素 $x$ 结尾的子序列的 xxx 值（比如个数、元素和），并从子序列的倒数第二个数转移过来。

对于本题，定义 $f[x]$ 表示以元素 $x$ 结尾的子序列的元素之和。子序列的倒数第二个数是 $x-1$ 或 $x+1$。

那么 $x$ 可以加在所有以 $x-1$ 结尾的子序列之后，以及所有以 $x+1$ 结尾的子序列之后。

为此，我们还需要知道以 $x$ 结尾的子序列的个数，记作 $\textit{cnt}[x]$。

从左到右遍历 $\textit{nums}$，设 $x = \textit{nums}[i]$，考虑是否选 $x$（是否在子序列中），分类讨论：

- 不选 $x$，元素和为 $f[x]$。
- $x$ 加在所有以 $x-1$ 结尾的子序列之后，这会额外产生 $\textit{cnt}[x-1]$ 个子序列，相当于在 $f[x-1]$ 的基础上，额外增加了 $\textit{cnt}[x-1]$ 个 $x$，所以这些子序列的元素总和为 $f[x-1] + x\cdot \textit{cnt}[x-1]$。
- $x$ 加在所有以 $x+1$ 结尾的子序列之后，这会额外产生 $\textit{cnt}[x+1]$ 个子序列，相当于在 $f[x+1]$ 的基础上，额外增加了 $\textit{cnt}[x+1]$ 个 $x$，所以这些子序列的元素总和为 $f[x+1] + x\cdot \textit{cnt}[x+1]$。
- $x$ 单独作为一个子序列，元素和为 $x$。

所以有

$$
f[x] = f[x] + f[x-1] + f[x+1] + x\cdot (\textit{cnt}[x-1] + \textit{cnt}[x+1] + 1)
$$

同时，额外产生了 $\textit{cnt}[x-1] + \textit{cnt}[x+1] + 1$ 个以 $x$ 结尾的子序列，所以有

$$
\textit{cnt}[x] = \textit{cnt}[x] + \textit{cnt}[x-1] + \textit{cnt}[x+1] + 1
$$

记得取模。关于取模的知识点，见 [模运算的世界：当加减乘除遇上取模](https://leetcode.cn/circle/discuss/mDfnkW/)。

[本题视频讲解](https://www.bilibili.com/video/BV1JVmBYvEnD/?t=7m31s)，欢迎点赞关注~

```py [sol-Python3]
class Solution:
    def sumOfGoodSubsequences(self, nums: List[int]) -> int:
        MOD = 1_000_000_007
        f = defaultdict(int)
        cnt = defaultdict(int)
        for x in nums:
            c = cnt[x - 1] + cnt[x + 1] + 1
            f[x] = (f[x] + f[x - 1] + f[x + 1] + x * c) % MOD
            cnt[x] = (cnt[x] + c) % MOD
        return sum(f.values()) % MOD
```

```java [sol-Java]
class Solution {
    public int sumOfGoodSubsequences(int[] nums) {
        final int MOD = 1_000_000_007;
        Map<Integer, Integer> f = new HashMap<>();
        Map<Integer, Integer> cnt = new HashMap<>();
        for (int x : nums) {
            long c = cnt.getOrDefault(x - 1, 0) + cnt.getOrDefault(x + 1, 0) + 1;
            f.put(x, (int) ((x * c + f.getOrDefault(x, 0) + f.getOrDefault(x - 1, 0) + f.getOrDefault(x + 1, 0)) % MOD));
            cnt.put(x, (int) ((cnt.getOrDefault(x, 0) + c) % MOD));
        }

        long ans = 0;
        for (int s : f.values()) {
            ans += s;
        }
        return (int) (ans % MOD);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int sumOfGoodSubsequences(vector<int>& nums) {
        const int MOD = 1'000'000'007;
        unordered_map<int, int> f, cnt;
        for (int x : nums) {
            long long c = cnt[x - 1] + cnt[x + 1] + 1;
            f[x] = (x * c + f[x] + f[x - 1] + f[x + 1]) % MOD;
            cnt[x] = (cnt[x] + c) % MOD;
        }

        long long ans = 0;
        for (const auto& [_, s] : f) {
            ans += s;
        }
        return ans % MOD;
    }
};
```

```go [sol-Go]
func sumOfGoodSubsequences(nums []int) (ans int) {
	const mod = 1_000_000_007
	f := map[int]int{}
	cnt := map[int]int{}
	for _, x := range nums {
		c := cnt[x-1] + cnt[x+1] + 1
		f[x] = (f[x] + f[x-1] + f[x+1] + x*c) % mod
		cnt[x] = (cnt[x] + c) % mod
	}

	for _, s := range f {
		ans += s
	}
	return ans % mod
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

也可以用数组代替哈希表，效率更高。

```py [sol-Python3]
class Solution:
    def sumOfGoodSubsequences(self, nums: List[int]) -> int:
        MOD = 1_000_000_007
        mx = max(nums) + 2
        f = [0] * mx
        cnt = [0] * mx
        for x in nums:
            c = cnt[x - 1] + cnt[x + 1] + 1
            f[x] = (f[x] + f[x - 1] + f[x + 1] + x * c) % MOD
            cnt[x] = (cnt[x] + c) % MOD
        return sum(f) % MOD
```

```java [sol-Java]
class Solution {
    public int sumOfGoodSubsequences(int[] nums) {
        final int MOD = 1_000_000_007;
        int mx = 0;
        for (int x : nums) {
            mx = Math.max(mx, x);
        }

        int[] f = new int[mx + 3];
        int[] cnt = new int[mx + 3];
        for (int x : nums) {
            // 为避免出现 -1，所有下标加一
            long c = cnt[x] + cnt[x + 2] + 1;
            f[x + 1] = (int) ((x * c + f[x] + f[x + 1] + f[x + 2]) % MOD);
            cnt[x + 1] = (int) ((cnt[x + 1] + c) % MOD);
        }

        long ans = 0;
        for (int s : f) {
            ans += s;
        }
        return (int) (ans % MOD);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int sumOfGoodSubsequences(vector<int>& nums) {
        int MOD = 1'000'000'007;
        int mx = ranges::max(nums);
        vector<int> f(mx + 3);
        vector<int> cnt(mx + 3);
        for (int x : nums) {
            // 为避免出现 -1，所有下标加一
            long long c = cnt[x] + cnt[x + 2] + 1;
            f[x + 1] = (x * c + f[x] + f[x + 1] + f[x + 2]) % MOD;
            cnt[x + 1] = (cnt[x + 1] + c) % MOD;
        }
        return reduce(f.begin(), f.end(), 0LL) % MOD;
    }
};
```

```go [sol-Go]
func sumOfGoodSubsequences(nums []int) (ans int) {
	const mod = 1_000_000_007
	mx := slices.Max(nums)
	f := make([]int, mx+3)
	cnt := make([]int, mx+3)
	for _, x := range nums {
		// 为避免出现 -1，所有下标加一
		c := cnt[x] + cnt[x+2] + 1
		f[x+1] = (f[x] + f[x+1] + f[x+2] + x*c) % mod
		cnt[x+1] = (cnt[x+1] + c) % mod
	}

	for _, s := range f {
		ans += s
	}
	return ans % mod
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n+U)$，其中 $n$ 是 $\textit{nums}$ 的长度，$U=\max(\textit{nums})$。
- 空间复杂度：$\mathcal{O}(n+U)$。

更多相似题目，见下面 [动态规划题单](https://leetcode.cn/circle/discuss/tXLS3i/) 中的 §7.2 节。

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
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

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
