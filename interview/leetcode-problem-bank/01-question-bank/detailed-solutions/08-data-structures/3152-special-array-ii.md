# 3152. 特殊数组 II

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/special-array-ii/
- 题目 slug：`special-array-ii`
- 来源专题：常用数据结构
- 来源分类路径：一、前缀和 / §1.1 基础
- 难度分：1523
- 外部题解来源：https://leetcode.cn/problems/special-array-ii/solutions/2782830/qian-zhui-he-wei-yun-suan-pythonjavacgo-3pvya/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：前缀和 / 奇偶性相同的位置（Python/Java/C++/Go）](https://leetcode.cn/problems/special-array-ii/solutions/2782830/qian-zhui-he-wei-yun-suan-pythonjavacgo-3pvya/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`qian-zhui-he-wei-yun-suan-pythonjavacgo-3pvya`
- topic id：`2782830`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 方法一：前缀和

如果子数组存在一对相邻元素，它们的奇偶性相同，那么这个子数组就不是特殊数组。

怎么快速知道是否有奇偶性相同的相邻元素？

考虑这样一个问题：给你一个只包含 $0$ 和 $1$ 的数组，如何快速判断一个子数组是否全为 $0$？

解答：如果子数组的**元素和**等于 $0$，那么子数组一定全为 $0$；如果子数组的**元素和**大于 $0$，那么子数组一定包含 $1$。如何快速计算子数组元素和？这可以用 [前缀和](https://leetcode.cn/problems/range-sum-query-immutable/solution/qian-zhui-he-ji-qi-kuo-zhan-fu-ti-dan-py-vaar/) 解决。

对于本题，定义长为 $n-1$ 的数组 $a$，其中

$$
a[i] =
\begin{cases} 
0, & \textit{nums}[i]\bmod 2 \ne \textit{nums}[i+1]\bmod 2   \\
1, & \textit{nums}[i]\bmod 2 = \textit{nums}[i+1]\bmod 2     \\
\end{cases}
$$

如果 $a$ 的下标从 $\textit{from}$ 到 $\textit{to}-1$ 的子数组和等于 $0$，就说明 $\textit{nums}$ 的下标从 $\textit{from}$ 到 $\textit{to}$ 的这个子数组，其所有相邻元素的奇偶性都不同，该子数组为特殊数组。

计算 $a$ 的**前缀和** $s$，可以快速判断子数组和是否为 $0$，也就是判断

$$
s[\textit{to}] - s[\textit{from}] = 0
$$

即

$$
s[\textit{from}] = s[\textit{to}]
$$

具体请看 [视频讲解](https://www.bilibili.com/video/BV19D421G7mw/) 第二题，欢迎点赞关注！

```py [sol-Python3]
class Solution:
    def isArraySpecial(self, nums: List[int], queries: List[List[int]]) -> List[bool]:
        s = list(accumulate((x % 2 == y % 2 for x, y in pairwise(nums)), initial=0))
        return [s[from_] == s[to] for from_, to in queries]
```

```java [sol-Java]
class Solution {
    public boolean[] isArraySpecial(int[] nums, int[][] queries) {
        int[] s = new int[nums.length];
        for (int i = 1; i < nums.length; i++) {
            s[i] = s[i - 1] + (nums[i - 1] % 2 == nums[i] % 2 ? 1 : 0);
        }
        boolean[] ans = new boolean[queries.length];
        for (int i = 0; i < queries.length; i++) {
            int[] q = queries[i];
            ans[i] = s[q[0]] == s[q[1]];
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<bool> isArraySpecial(vector<int>& nums, vector<vector<int>>& queries) {
        vector<int> s(nums.size());
        for (int i = 1; i < nums.size(); i++) {
            s[i] = s[i - 1] + (nums[i - 1] % 2 == nums[i] % 2);
        }
        vector<bool> ans(queries.size());
        for (int i = 0; i < queries.size(); i++) {
            auto& q = queries[i];
            ans[i] = s[q[0]] == s[q[1]];
        }
        return ans;
    }
};
```

```go [sol-Go]
func isArraySpecial(nums []int, queries [][]int) []bool {
	s := make([]int, len(nums))
	for i := 1; i < len(nums); i++ {
		s[i] = s[i-1]
		if nums[i-1]%2 == nums[i]%2 {
			s[i]++
		}
	}
	ans := make([]bool, len(queries))
	for i, q := range queries {
		ans[i] = s[q[0]] == s[q[1]]
	}
	return ans
}
```

也可以用**位运算**计算 $a[i]$：相邻两数的异或和，其最低位取反即为 $a[i]$。

```py [sol-Python3]
class Solution:
    def isArraySpecial(self, nums: List[int], queries: List[List[int]]) -> List[bool]:
        s = list(accumulate(((x ^ y ^ 1) & 1 for x, y in pairwise(nums)), initial=0))
        return [s[from_] == s[to] for from_, to in queries]
```

```java [sol-Java]
class Solution {
    public boolean[] isArraySpecial(int[] nums, int[][] queries) {
        int[] s = new int[nums.length];
        for (int i = 1; i < nums.length; i++) {
            s[i] = s[i - 1] + ((nums[i] ^ nums[i - 1] ^ 1) & 1);
        }
        boolean[] ans = new boolean[queries.length];
        for (int i = 0; i < queries.length; i++) {
            int[] q = queries[i];
            ans[i] = s[q[0]] == s[q[1]];
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<bool> isArraySpecial(vector<int>& nums, vector<vector<int>>& queries) {
        vector<int> s(nums.size());
        for (int i = 1; i < nums.size(); i++) {
            s[i] = s[i - 1] + ((nums[i] ^ nums[i - 1] ^ 1) & 1);
        }
        vector<bool> ans(queries.size());
        for (int i = 0; i < queries.size(); i++) {
            auto& q = queries[i];
            ans[i] = s[q[0]] == s[q[1]];
        }
        return ans;
    }
};
```

```go [sol-Go]
func isArraySpecial(nums []int, queries [][]int) []bool {
	s := make([]int, len(nums))
	for i := 1; i < len(nums); i++ {
		s[i] = s[i-1] + (nums[i]^nums[i-1]^1)&1
	}
	ans := make([]bool, len(queries))
	for i, q := range queries {
		ans[i] = s[q[0]] == s[q[1]]
	}
	return ans
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n+q)$，其中 $n$ 是 $\textit{nums}$ 的长度，其中 $q$ 是 $\textit{queries}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。返回值不计入。

## 方法二：记录最近一次奇偶性相同的位置

考虑对于每个位置（下标）$i$，记录最近一次奇偶性相同的位置。

定义 $\textit{lastSame}[i]$ 为 $\le i$ 的最大下标 $j$，满足 $\textit{nums}[j-1]\bmod 2 = \textit{nums}[j]\bmod 2$。如果没有这样的 $j$，则 $\textit{lastSame}[i]=0$。

例如 $\textit{nums}= [1,1,2,1,1,2]$，对应的 $\textit{lastSame} = [0,1,1,1,4,4]$。

对于这个例子，回答询问 $\textit{from}=2,\ \textit{to}=3$，因为 $\textit{lastSame}[3]=1\le \textit{from}$，所以子数组 $[\textit{from},\textit{to}]$ 内没有奇偶性相同的相邻元素。又例如 $\textit{from}=0,\ \textit{to}=3$，因为 $\textit{lastSame}[3]=1 > \textit{from}$，所以子数组 $[\textit{from},\textit{to}]$ 包含奇偶性相同的相邻元素。

⚠**注意**：当 $\textit{lastSame}[to]=\textit{from}$ 时，由于定义是比较 $j-1$ 和 $j$ 这两个位置，而 $j-1 < \textit{from}$ 在子数组范围外，所以在这种情况下，子数组内没有奇偶性相同的相邻元素。

怎么计算 $\textit{lastSame}[i]$？

分类讨论：

- 如果 $\textit{nums}[i-1]\bmod 2 = \textit{nums}[i]\bmod 2$，那么根据定义，$\textit{lastSame}[i]=i$。
- 如果 $\textit{nums}[i-1]\bmod 2 \ne \textit{nums}[i]\bmod 2$，说明定义中的 $j\le i-1$，所以 $\textit{lastSame}[i]=\textit{lastSame}[i-1]$。

所以可以用**递推**的方法计算出 $\textit{lastSame}$。

```py [sol-Python3]
class Solution:
    def isArraySpecial(self, nums: List[int], queries: List[List[int]]) -> List[bool]:
        n = len(nums)
        last_same = [0] * n
        for i in range(1, n):
            last_same[i] = i if nums[i - 1] % 2 == nums[i] % 2 else last_same[i - 1]
        return [last_same[to] <= from_ for from_, to in queries]
```

```java [sol-Java]
class Solution {
    public boolean[] isArraySpecial(int[] nums, int[][] queries) {
        int n = nums.length;
        int[] lastSame = new int[n];
        for (int i = 1; i < n; i++) {
            lastSame[i] = nums[i - 1] % 2 == nums[i] % 2 ? i : lastSame[i - 1];
        }
        boolean[] ans = new boolean[queries.length];
        for (int i = 0; i < queries.length; i++) {
            int[] q = queries[i];
            ans[i] = lastSame[q[1]] <= q[0];
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<bool> isArraySpecial(vector<int>& nums, vector<vector<int>>& queries) {
        int n = nums.size();
        vector<int> last_same(n);
        for (int i = 1; i < n; i++) {
            last_same[i] = nums[i - 1] % 2 == nums[i] % 2 ? i : last_same[i - 1];
        }
        vector<bool> ans(queries.size());
        for (int i = 0; i < queries.size(); i++) {
            auto& q = queries[i];
            ans[i] = last_same[q[1]] <= q[0];
        }
        return ans;
    }
};
```

```go [sol-Go]
func isArraySpecial(nums []int, queries [][]int) []bool {
	n := len(nums)
	lastSame := make([]int, n)
	for i := 1; i < n; i++ {
		if nums[i-1]%2 == nums[i]%2 {
			lastSame[i] = i
		} else {
			lastSame[i] = lastSame[i-1]
		}
	}
	ans := make([]bool, len(queries))
	for i, q := range queries {
		ans[i] = lastSame[q[1]] <= q[0]
	}
	return ans
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n+q)$，其中 $n$ 是 $\textit{nums}$ 的长度，其中 $q$ 是 $\textit{queries}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。返回值不计入。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口（定长/不定长/多指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心算法（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与一般树（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/circle/discuss/K0n2gO/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `一、前缀和 / §1.1 基础`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、前缀和 / §1.1 基础`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
