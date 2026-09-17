# 3434. 子数组操作后的最大频率

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-frequency-after-subarray-operation/
- 题目 slug：`maximum-frequency-after-subarray-operation`
- 来源专题：动态规划
- 来源分类路径：六、状态机 DP / §6.3 进阶
- 难度分：2094
- 外部题解来源：https://leetcode.cn/problems/maximum-frequency-after-subarray-operation/solutions/3057702/mei-ju-zhuang-tai-ji-dp-by-endlesscheng-qpt0/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[枚举 + 状态机 DP + O(n) 优化（Python/Java/C++/Go）](https://leetcode.cn/problems/maximum-frequency-after-subarray-operation/solutions/3057702/mei-ju-zhuang-tai-ji-dp-by-endlesscheng-qpt0/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`mei-ju-zhuang-tai-ji-dp-by-endlesscheng-qpt0`
- topic id：`3057702`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 16:46:02 +0800

## 方法一：枚举 + 状态机 DP

考虑把子数组中等于 $\textit{target}$ 的元素都变成 $k$。

由于 $\textit{nums}$ 至多有 $50$ 种不同元素，可以枚举 $\textit{target}=1,2,3,\ldots,50$（或者 $\textit{nums}$ 中的不同元素）。

$\textit{nums}$ 可以分为三段：

1. 左：被修改的子数组的左边。
2. 中：被修改的子数组。
3. 右：被修改的子数组的右边。

用状态机 DP 计算 $\textit{nums}[0]$ 到 $\textit{nums}[i]$ 最多有多少个数可以等于 $k$：

- $f[i+1][0]$ 表示左，或者说 $\textit{nums}[i]$ 在被修改的子数组的左侧，此时只能统计等于 $k$ 的元素个数。
- $f[i+1][1]$ 表示左+中，或者说 $\textit{nums}[i]$ 在被修改的子数组中，此时只能统计等于 $\textit{target}$ 的元素个数，这些数被修改成 $k$。注意不能统计子数组中的 $k$ 的个数，因为子数组中的 $k$ 会被修改成别的数。
- $f[i+1][2]$ 表示左+中+右，或者说 $\textit{nums}[i]$ 在被修改的子数组的右侧，此时只能统计等于 $k$ 的元素个数。

从左到右遍历 $\textit{nums}$，设 $x=\textit{nums}[i]$，考虑转移来源：

- 「左」只能从「左」转移过来。如果 $x=k$，那么不选白不选，问题变成 $\textit{nums}[0]$ 到 $\textit{nums}[i-1]$ 最多有多少个数可以等于 $k$，即状态 $f[i][0]$，所以有转移方程 $f[i+1][0] = f[i][0] + 1$。如果 $x\ne k$，那么 $f[i+1][0] = f[i][0]$。
- 「左+中」可以从「左+中」或者「左」转移过来。同上，问题变成 $\textit{nums}[0]$ 到 $\textit{nums}[i-1]$ 最多有多少个数可以等于 $k$。如果 $x=\textit{target}$，那么 $f[i+1][1] = \max(f[i][1], f[i][0]) + 1$，否则 $f[i+1][1] = \max(f[i][1], f[i][0])$。这里从 $f[i][1]$ 转移过来，表示 $\textit{nums}[i-1]$ 也在被修改的子数组中；从 $f[i][0]$ 转移过来，表示 $\textit{nums}[i]$ 是被修改的子数组的第一个数。
- 「左+中+右」可以从「左+中+右」或者「左+中」转移过来。同上，问题变成 $\textit{nums}[0]$ 到 $\textit{nums}[i-1]$ 最多有多少个数可以等于 $k$。如果 $x=k$，那么 $f[i+1][2] = \max(f[i][2], f[i][1]) + 1$，否则 $f[i+1][2] = \max(f[i][2], f[i][1])$。这里从 $f[i][2]$ 转移过来，表示 $\textit{nums}[i-1]$ 也在被修改的子数组的右边；从 $f[i][1]$ 转移过来，表示 $\textit{nums}[i-1]$ 是被修改的子数组的最后一个数。

初始值 $f[0][0] = f[0][1] = f[0][2] = 0$。本题子数组所有数都增加 $0$ 相当于没有操作，这也等价于子数组可以是空的。既然允许空子数组，那么初始化成 $0$ 也可以。

答案为 $\max(f[n][1], f[n][2])$。最后一个数可以在「中」也可以在「右」。

代码实现时，第一个维度可以优化掉，三个状态按照 $f_2\to f_1\to f_0$ 的顺序倒着更新，理由同 [0-1 背包](https://www.bilibili.com/video/BV16Y411v7Y6/)。

具体请看 [视频讲解](https://www.bilibili.com/video/BV15sFNewEia/?t=13m50s)，欢迎点赞关注~

```py [sol-Python3]
class Solution:
    def maxFrequency(self, nums: List[int], k: int) -> int:
        ans = 0
        for target in set(nums):
            f0 = f1 = f2 = 0
            for x in nums:
                f2 = max(f2, f1) + (x == k)
                f1 = max(f1, f0) + (x == target)
                f0 += (x == k)
            ans = max(ans, f1, f2)
        return ans
```

```java [sol-Java]
class Solution {
    public int maxFrequency(int[] nums, int k) {
        Set<Integer> set = new HashSet<>();
        for (int x : nums) {
            set.add(x);
        }

        int ans = 0;
        for (int target : set) {
            int f0 = 0, f1 = 0, f2 = 0;
            for (int x : nums) {
                f2 = Math.max(f2, f1) + (x == k ? 1 : 0);
                f1 = Math.max(f1, f0) + (x == target ? 1 : 0);
                f0 += (x == k ? 1 : 0);
            }
            ans = Math.max(ans, Math.max(f1, f2));
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxFrequency(vector<int>& nums, int k) {
        unordered_set<int> st(nums.begin(), nums.end());
        int ans = 0;
        for (int target : st) {
            int f0 = 0, f1 = 0, f2 = 0;
            for (int x : nums) {
                f2 = max(f2, f1) + (x == k);
                f1 = max(f1, f0) + (x == target);
                f0 += (x == k);
            }
            ans = max({ans, f1, f2});
        }
        return ans;
    }
};
```

```go [sol-Go]
func maxFrequency(nums []int, k int) (ans int) {
	set := map[int]struct{}{}
	for _, x := range nums {
		set[x] = struct{}{}
	}

	for target := range set {
		var f0, f1, f2 int
		for _, x := range nums {
			f2 = max(f2, f1) + b2i(x == k)
			f1 = max(f1, f0) + b2i(x == target)
			f0 += b2i(x == k)
		}
		ans = max(ans, f1, f2)
	}
	return
}

func b2i(b bool) int { if b { return 1 }; return 0 }
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(nU)$，其中 $n$ 是 $\textit{nums}$ 的长度，$U$ 是 $\textit{nums}$ 中的不同元素个数。
- 空间复杂度：$\mathcal{O}(U)$。

## 方法二：状态机 DP 优化（一次遍历）

$f_0$ 和 $f_2$ 的定义不变。

设 $x=\textit{nums}[i]$。用 $f_1[x]$ 存储方法一中的 $\textit{target}=x$ 时的 $f_1$ 状态，转移方程为

$$
f_1[x] = \max(f_1[x], f_0) + 1
$$

其余 $f_1[y]\ (y\ne x)$ 不变（懒更新）。 

额外定义 $\textit{maxF}_1$ 表示 $f_1[x]$ 的最大值。

那么 $f_2$ 的转移方程为

$$
f_2 = \max(f_2, \textit{maxF}_1) + [x=k]
$$

### 写法一

```py [sol-Python3]
max = lambda a, b: a if a > b else b  # 手写 max 更快

class Solution:
    def maxFrequency(self, nums: List[int], k: int) -> int:
        f0 = max_f1 = f2 = 0
        f1 = [0] * 51  # 或者用 defaultdict(int)
        for x in nums:
            f2 = max(f2, max_f1) + (x == k)
            f1[x] = max(f1[x], f0) + 1
            f0 += (x == k)
            max_f1 = max(max_f1, f1[x])
        return max(max_f1, f2)
```

```java [sol-Java]
class Solution {
    public int maxFrequency(int[] nums, int k) {
        int f0 = 0, maxF1 = 0, f2 = 0;
        int[] f1 = new int[51];
        for (int x : nums) {
            f2 = Math.max(f2, maxF1) + (x == k ? 1 : 0);
            f1[x] = Math.max(f1[x], f0) + 1;
            f0 += (x == k ? 1 : 0);
            maxF1 = Math.max(maxF1, f1[x]);
        }
        return Math.max(maxF1, f2);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxFrequency(vector<int>& nums, int k) {
        int f0 = 0, f1[51]{}, max_f1 = 0, f2 = 0;
        for (int x : nums) {
            f2 = max(f2, max_f1) + (x == k);
            f1[x] = max(f1[x], f0) + 1;
            f0 += (x == k);
            max_f1 = max(max_f1, f1[x]);
        }
        return max(max_f1, f2);
    }
};
```

```go [sol-Go]
func maxFrequency(nums []int, k int) int {
	var f0, maxF1, f2 int
	f1 := [51]int{}
	for _, x := range nums {
		f2 = max(f2, maxF1)
		f1[x] = max(f1[x], f0) + 1
		if x == k {
			f2++
			f0++
		}
		maxF1 = max(maxF1, f1[x])
	}
	return max(maxF1, f2)
}
```

### 写法二

把 $\textit{maxF}_1$ 和 $f_2$ 合并成一个变量 $\textit{maxF}_{12}$。当 $x=k$ 的时候，把 $\textit{maxF}_{12}$ 加一。转移方程中的 $\max(f_2, \textit{maxF}_1)$ 无需计算，因为两个变量已经合二为一。

此外，$x=k$ 的时候不需要计算 $f_1[x]$，因为这个状态等价于统计 $k$ 的个数，这也是 $\textit{maxF}_{12}$ 统计的内容。

```py [sol-Python3]
max = lambda a, b: a if a > b else b  # 手写 max 更快

class Solution:
    def maxFrequency(self, nums: List[int], k: int) -> int:
        f0 = max_f12 = 0
        f1 = [0] * 51  # 或者用 defaultdict(int)
        for x in nums:
            if x == k:
                max_f12 += 1
                f0 += 1
            else:
                f1[x] = max(f1[x], f0) + 1
                max_f12 = max(max_f12, f1[x])
        return max_f12
```

```java [sol-Java]
class Solution {
    public int maxFrequency(int[] nums, int k) {
        int f0 = 0, maxF12 = 0;
        int[] f1 = new int[51];
        for (int x : nums) {
            if (x == k) {
                maxF12++;
                f0++;
            } else {
                f1[x] = Math.max(f1[x], f0) + 1;
                maxF12 = Math.max(maxF12, f1[x]);
            }
        }
        return maxF12;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxFrequency(vector<int>& nums, int k) {
        int f0 = 0, f1[51]{}, max_f12 = 0;
        for (int x : nums) {
            if (x == k) {
                max_f12++;
                f0++;
            } else {
                f1[x] = max(f1[x], f0) + 1;
                max_f12 = max(max_f12, f1[x]);
            }
        }
        return max_f12;
    }
};
```

```go [sol-Go]
func maxFrequency(nums []int, k int) int {
	f0, maxF12 := 0, 0
	f1 := [51]int{}
	for _, x := range nums {
		if x == k {
			maxF12++
			f0++
		} else {
			f1[x] = max(f1[x], f0) + 1
			maxF12 = max(maxF12, f1[x])
		}
	}
	return maxF12
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n+U)$，其中 $n$ 是 $\textit{nums}$ 的长度，$U$ 是 $\textit{nums}$ 中的不同元素个数。注意创建数组需要 $\mathcal{O}(U)$ 的时间。
- 空间复杂度：$\mathcal{O}(U)$。

## 变形题

改成可以修改**两个**不相交的子数组呢？

在 [视频讲解](https://www.bilibili.com/video/BV15sFNewEia/?t=13m50s) 第三题的最后讲了怎么做。

更多相似题目，见下面动态规划题单中的「**五、状态机 DP**」。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. 【本题相关】[动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

## 本地原创解析

### 1. 题意重述

本题来自 `六、状态机 DP / §6.3 进阶`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `六、状态机 DP / §6.3 进阶`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
