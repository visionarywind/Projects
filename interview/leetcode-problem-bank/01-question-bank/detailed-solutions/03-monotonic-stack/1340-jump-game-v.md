# 1340. 跳跃游戏 V

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/jump-game-v/
- 题目 slug：`jump-game-v`
- 来源专题：单调栈
- 来源分类路径：一、单调栈 / §1.2 进阶
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/jump-game-v/solutions/3963787/liang-chong-fang-fa-zheng-zhao-tiao-dao-pl5ep/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：正着跳 / 倒着跳，O(n) 时间（Python/Java/C++/Go）](https://leetcode.cn/problems/jump-game-v/solutions/3963787/liang-chong-fang-fa-zheng-zhao-tiao-dao-pl5ep/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`liang-chong-fang-fa-zheng-zhao-tiao-dao-pl5ep`
- topic id：`3963787`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 方法一：正着跳

写个暴力搜索，枚举所有情况。

- 从 $i$ 往左跳：枚举 $j=i-1,i-2,\ldots,\max(i-d,0)$。根据题目要求，遍历过的元素都必须严格小于 $\textit{arr}[i]$，如果 $\textit{arr}[j]\ge \textit{arr}[i]$ 就停止枚举。然后从 $j$ 开始继续跳。
- 从 $i$ 往右跳：枚举 $j=i+1,i+2,\ldots,\min(i+d,n-1)$。根据题目要求，遍历过的元素都必须严格小于 $\textit{arr}[i]$，如果 $\textit{arr}[j]\ge \textit{arr}[i]$ 就停止枚举。然后从 $j$ 开始继续跳。

考虑到整个递归过程中有大量重复递归调用（递归入参相同）。由于递归函数没有副作用，同样的入参无论计算多少次，算出来的结果都是一样的，因此可以用**记忆化搜索**来优化：

- 如果一个状态（递归入参）是第一次遇到，那么可以在返回前，把状态及其结果记到一个 $\textit{memo}$ 数组中。
- 如果一个状态不是第一次遇到（$\textit{memo}$ 中保存的结果不等于 $\textit{memo}$ 的初始值），那么可以直接返回 $\textit{memo}$ 中保存的结果。

关于记忆化搜索的原理，请看视频讲解 [动态规划入门：从记忆化搜索到递推【基础算法精讲 17】](https://www.bilibili.com/video/BV1Xj411K7oF/)。

```py [sol-Python3]
class Solution:
    def maxJumps(self, arr: List[int], d: int) -> int:
        n = len(arr)

        @cache  # 缓存装饰器，避免重复计算 dfs（一行代码实现记忆化）
        def dfs(i: int) -> int:
            res = 1

            # 往左跳
            for j in range(i - 1, max(i - d - 1, -1), -1):
                if arr[j] >= arr[i]:
                    break
                res = max(res, dfs(j) + 1)

            # 往右跳
            for j in range(i + 1, min(i + d + 1, n)):
                if arr[j] >= arr[i]:
                    break
                res = max(res, dfs(j) + 1)

            return res

        # 枚举起点
        return max(dfs(i) for i in range(n))
```

```java [sol-Java]
class Solution {
    public int maxJumps(int[] arr, int d) {
        int n = arr.length;
        int[] memo = new int[n];

        // 枚举起点
        int ans = 0;
        for (int i = 0; i < n; i++) {
            ans = Math.max(ans, dfs(i, arr, d, memo));
        }
        return ans;
    }

    private int dfs(int i, int[] arr, int d, int[] memo) {
        if (memo[i] > 0) { // 之前计算过
            return memo[i];
        }

        int res = 1;

        // 往左跳
        for (int j = i - 1; j >= Math.max(i - d, 0) && arr[j] < arr[i]; j--) {
            res = Math.max(res, dfs(j, arr, d, memo) + 1);
        }

        // 往右跳
        for (int j = i + 1; j <= Math.min(i + d, arr.length - 1) && arr[j] < arr[i]; j++) {
            res = Math.max(res, dfs(j, arr, d, memo) + 1);
        }

        memo[i] = res; // 记忆化
        return res;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxJumps(vector<int>& arr, int d) {
        int n = arr.size();
        vector<int> memo(n);

        auto dfs = [&](this auto&& dfs, int i) -> int {
            int& res = memo[i]; // 注意这里是引用
            if (res) { // 之前计算过
                return res;
            }

            res = 1;

            // 往左跳
            for (int j = i - 1; j >= max(i - d, 0) && arr[j] < arr[i]; j--) {
                res = max(res, dfs(j) + 1);
            }

            // 往右跳
            for (int j = i + 1; j <= min(i + d, n - 1) && arr[j] < arr[i]; j++) {
                res = max(res, dfs(j) + 1);
            }

            return res;
        };

        // 枚举起点
        int ans = 0;
        for (int i = 0; i < n; i++) {
            ans = max(ans, dfs(i));
        }
        return ans;
    }
};
```

```go [sol-Go]
func maxJumps(arr []int, d int) (ans int) {
	n := len(arr)
	memo := make([]int, n)

	var dfs func(int) int
	dfs = func(i int) (res int) {
		p := &memo[i]
		if *p > 0 { // 之前计算过
			return *p
		}
		defer func() { *p = res }() // 记忆化

		// 往左跳
		for j := i - 1; j >= max(i-d, 0) && arr[j] < arr[i]; j-- {
			res = max(res, dfs(j))
		}

		// 往右跳
		for j := i + 1; j <= min(i+d, n-1) && arr[j] < arr[i]; j++ {
			res = max(res, dfs(j))
		}

		return res + 1 // +1 提到循环外面
	}

	// 枚举起点
	for i := range n {
		ans = max(ans, dfs(i))
	}
	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(nd)$，其中 $n$ 是 $\textit{arr}$ 的长度。由于每个状态只会计算一次，记忆化搜索的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题状态个数等于 $\mathcal{O}(n)$，单个状态的计算时间为 $\mathcal{O}(d)$，所以总的时间复杂度为 $\mathcal{O}(nd)$。
- 空间复杂度：$\mathcal{O}(n)$。保存多少状态，就需要多少空间。

## 方法二：倒着跳

能不能从小到大跳？

![lc1340.jpeg](https://pic.leetcode.cn/1778028230-jGCYlD-lc1340.jpeg)

逆向思维，倒着跳，从下标 $i$ 跳到下标 $j$，需要满足：

- $i$ 到 $j$ 之间（包含 $i$ 不包含 $j$）的每个数都严格小于 $\textit{arr}[j]$。

上图中的元素 $7$ 往左可以跳到元素 $9,13,14$，往右可以跳到元素 $10,12$。由于 $7$ 往左能跳到的元素，$9$ 往左也可以跳到（都满足要求），所以 $7\to 9\to 13$ 一定比 $7\to 13$ 更优。

一般地，**跳到左右最近的更大元素是最优的**。

所以无需枚举 $[i-d,i+d]$ 中的 $j$，只需考虑两个下标 $j$：

- $\textit{arr}[i]$ 左边最近的更大元素 $\textit{arr}[j]$。
- $\textit{arr}[i]$ 右边最近的更大元素 $\textit{arr}[j]$。

这是**单调栈**的标准应用，原理见 [单调栈【基础算法精讲 26】](https://www.bilibili.com/video/BV1VN411J7S7/)。

```py [sol-Python3]
class Solution:
    def maxJumps(self, arr: List[int], d: int) -> int:
        n = len(arr)

        # 计算 arr[i] 左边最近的更大元素 arr[left[i]]
        left = [0] * n
        st = []
        for i, x in enumerate(arr):
            while st and arr[st[-1]] <= x:
                st.pop()
            # 如果左边没有更大的数，或者跳跃距离超过 d，都标记为 -1
            left[i] = st[-1] if st and i - st[-1] <= d else -1
            st.append(i)

        # 计算 arr[i] 右边最近的更大元素 arr[right[i]]
        right = [0] * n
        st.clear()
        for i in range(n - 1, -1, -1):
            x = arr[i]
            while st and arr[st[-1]] <= x:
                st.pop()
            # 如果右边没有更大的数，或者跳跃距离超过 d，都标记为 -1
            right[i] = st[-1] if st and st[-1] - i <= d else -1
            st.append(i)

        @cache  # 缓存装饰器，避免重复计算 dfs（一行代码实现记忆化）
        def dfs(i: int) -> int:
            if i < 0:  # 没有更大的数，或者跳跃距离超过 d
                return 0
            # 往左跳 vs 往右跳
            return max(dfs(left[i]), dfs(right[i])) + 1

        # 枚举终点，倒着跳
        return max(dfs(i) for i in range(n))
```

```java [sol-Java]
class Solution {
    public int maxJumps(int[] arr, int d) {
        int n = arr.length;

        // 计算 arr[i] 左边最近的更大元素 arr[left[i]]
        int[] left = new int[n];
        int[] st = new int[n];
        int top = -1; // 栈顶下标
        for (int i = 0; i < n; i++) {
            int x = arr[i];
            while (top >= 0 && arr[st[top]] <= x) {
                top--; // 出栈
            }
            // 如果左边没有更大的数，或者跳跃距离超过 d，都标记为 -1
            left[i] = top < 0 || i - st[top] > d ? -1 : st[top];
            st[++top] = i; // 入栈
        }

        // 计算 arr[i] 右边最近的更大元素 arr[right[i]]
        int[] right = new int[n];
        top = -1;
        for (int i = n - 1; i >= 0; i--) {
            int x = arr[i];
            while (top >= 0 && arr[st[top]] <= x) {
                top--;
            }
            // 如果右边没有更大的数，或者跳跃距离超过 d，都标记为 -1
            right[i] = top < 0 || st[top] - i > d ? -1 : st[top];
            st[++top] = i;
        }

        int[] memo = new int[n];

        // 枚举终点，倒着跳
        int ans = 0;
        for (int i = 0; i < n; i++) {
            ans = Math.max(ans, dfs(i, left, right, memo));
        }
        return ans;
    }

    private int dfs(int i, int[] left, int[] right, int[] memo) {
        if (i < 0) { // 没有更大的数，或者跳跃距离超过 d
            return 0;
        }
        if (memo[i] == 0) { // 没有计算过
            // 往左跳 vs 往右跳
            memo[i] = Math.max(dfs(left[i], left, right, memo), dfs(right[i], left, right, memo)) + 1;
        }
        return memo[i];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxJumps(vector<int>& arr, int d) {
        int n = arr.size();

        // 计算 arr[i] 左边最近的更大元素 arr[left[i]]
        vector<int> left(n);
        vector<int> st;
        for (int i = 0; i < n; i++) {
            int x = arr[i];
            while (!st.empty() && arr[st.back()] <= x) {
                st.pop_back();
            }
            // 如果左边没有更大的数，或者跳跃距离超过 d，都标记为 -1
            left[i] = st.empty() || i - st.back() > d ? -1 : st.back();
            st.push_back(i);
        }

        // 计算 arr[i] 右边最近的更大元素 arr[right[i]]
        vector<int> right(n);
        st.clear();
        for (int i = n - 1; i >= 0; i--) {
            int x = arr[i];
            while (!st.empty() && arr[st.back()] <= x) {
                st.pop_back();
            }
            // 如果右边没有更大的数，或者跳跃距离超过 d，都标记为 -1
            right[i] = st.empty() || st.back() - i > d ? -1 : st.back();
            st.push_back(i);
        }

        vector<int> memo(n);

        auto dfs = [&](this auto&& dfs, int i) -> int {
            if (i < 0) { // 没有更大的数，或者跳跃距离超过 d
                return 0;
            }
            int& res = memo[i];
            if (res == 0) { // 没有计算过
                // 往左跳 vs 往右跳
                res = max(dfs(left[i]), dfs(right[i])) + 1;
            }
            return res;
        };

        // 枚举终点，倒着跳
        int ans = 0;
        for (int i = 0; i < n; i++) {
            ans = max(ans, dfs(i));
        }
        return ans;
    }
};
```

```go [sol-Go]
func maxJumps(arr []int, d int) (ans int) {
	n := len(arr)
	// 计算 arr[i] 左边最近的更大元素 arr[left[i]]
	left := make([]int, n)
	st := []int{}
	for i, x := range arr {
		for len(st) > 0 && arr[st[len(st)-1]] <= x {
			st = st[:len(st)-1]
		}
		if len(st) > 0 && i-st[len(st)-1] <= d {
			left[i] = st[len(st)-1]
		} else {
			left[i] = -1 // 左边没有更大的数，或者跳跃距离超过 d，都标记为 -1
		}
		st = append(st, i)
	}

	// 计算 arr[i] 右边最近的更大元素 arr[right[i]]
	right := make([]int, n)
	st = st[:0]
	for i, x := range slices.Backward(arr) {
		for len(st) > 0 && arr[st[len(st)-1]] <= x {
			st = st[:len(st)-1]
		}
		if len(st) > 0 && st[len(st)-1]-i <= d {
			right[i] = st[len(st)-1]
		} else {
			right[i] = -1 // 右边没有更大的数，或者跳跃距离超过 d，都标记为 -1
		}
		st = append(st, i)
	}

	memo := make([]int, n)

	var dfs func(int) int
	dfs = func(i int) int {
		if i < 0 { // 没有更大的数，或者跳跃距离超过 d
			return 0
		}
		p := &memo[i]
		if *p == 0 { // 没有计算过
			// 往左跳 vs 往右跳
			*p = max(dfs(left[i]), dfs(right[i])) + 1
		}
		return *p
	}

	// 枚举终点，倒着跳
	for i := range n {
		ans = max(ans, dfs(i))
	}
	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{arr}$ 的长度。对于单调栈的二重循环，由于每个下标至多入栈出栈各一次（在一个二重循环中），所以二重循环的时间复杂度为 $\mathcal{O}(n)$。对于记忆化搜索，由于每个状态只会计算一次，记忆化搜索的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题状态个数等于 $\mathcal{O}(n)$，单个状态的计算时间为 $\mathcal{O}(1)$，所以记忆化搜索的时间复杂度为 $\mathcal{O}(n)$。
- 空间复杂度：$\mathcal{O}(n)$。

## 专题训练

1. 动态规划题单的「**专题：跳跃游戏**」。
2. 单调栈题单。

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
11. [链表、树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `一、单调栈 / §1.2 进阶`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、单调栈 / §1.2 进阶`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
