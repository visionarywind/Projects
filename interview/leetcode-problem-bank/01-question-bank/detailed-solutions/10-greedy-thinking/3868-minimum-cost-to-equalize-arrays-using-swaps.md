# 3868. 通过交换使数组相等的最小花费

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-cost-to-equalize-arrays-using-swaps/
- 题目 slug：`minimum-cost-to-equalize-arrays-using-swaps`
- 来源专题：贪心与思维
- 来源分类路径：五、思维题 / §5.3 等价转化
- 难度分：1579
- 外部题解来源：https://leetcode.cn/problems/minimum-cost-to-equalize-arrays-using-swaps/solutions/3925649/deng-jie-zhuan-hua-pythonjavacgo-by-endl-hbly/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[等价转化（Python/Java/C++/Go）](https://leetcode.cn/problems/minimum-cost-to-equalize-arrays-using-swaps/solutions/3925649/deng-jie-zhuan-hua-pythonjavacgo-by-endl-hbly/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`deng-jie-zhuan-hua-pythonjavacgo-by-endl-hbly`
- topic id：`3925649`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:07:12 +0800

由于在同一个数组内交换是免费的，我们可以把数组视作**无序集合**，里面的元素是什么顺序都可以。

于是问题转化成：

- 给你两个大小都为 $n$ 的集合 $A$ 和 $B$。每次操作，从两个集合中各选一个数，交换。最少交换多少次，可以让 $A=B$？

首先，每种元素的**总**出现次数必须是**偶数**，不然无法均分。

否则可以均分。比如元素 $x$ 在集合 $A$ 中出现 $8$ 次，在集合 $B$ 中出现 $2$ 次，一共有 $10$ 个 $x$。交换后，需要满足两个集合各有 $5$ 个 $x$，也就是把 $A$ 中的 $3$ 个 $x$ 与 $B$ 中的另外 $3$ 个数交换（交换的数是集合 $A$ 需要的数），交换 $3$ 次。

定义 $\textit{diff}[x]$ 表示元素 $x$ 在集合 $A$ 中的出现次数，减去 $x$ 在集合 $B$ 中的出现次数。

我们需要把 $\textit{diff}[x]$ 变成 $0$，这样两个集合中的元素 $x$ 的个数就相等了。

由于两个集合的大小都是 $n$，根据 $\textit{diff}$ 的定义，$\textit{diff}$ 的总和（出现次数之差的总和）是 $n-n=0$。换句话说，$\textit{diff}$ 中的正数之和等于负数之和的绝对值。**集合 $A$ 多出的数，恰好也是集合 $B$ 多出的数**。把这些数交换，即可让 $A=B$。由于交换一次，可以让 $\textit{diff}[x]$ 中的正数之和减少 $1-(-1) = 2$，所以交换次数等于 $\textit{diff}$ 中的正数之和除以 $2$。在上面的例子中，$\textit{diff}[x]=6$，需要交换 $\dfrac{6}{2} = 3$ 次。

[本题视频讲解](https://www.bilibili.com/video/BV111wTzQEbp/)，欢迎点赞关注~

```py [sol-Python3]
class Solution:
    def minCost(self, nums1: List[int], nums2: List[int]) -> int:
        diff = Counter(nums1)
        diff.subtract(nums2)

        ans = 0
        for d in diff.values():
            if d % 2:
                return -1
            if d > 0:
                ans += d
        return ans // 2
```

```java [sol-Java]
class Solution {
    public int minCost(int[] nums1, int[] nums2) {
        Map<Integer, Integer> diff = new HashMap<>(); // 更快的写法见【Java 数组】
        for (int x : nums1) {
            diff.merge(x, 1, Integer::sum); // diff[x]++
        }
        for (int x : nums2) {
            diff.merge(x, -1, Integer::sum); // diff[x]--
        }

        int ans = 0;
        for (int d : diff.values()) {
            if (d % 2 != 0) {
                return -1;
            }
            if (d > 0) {
                ans += d;
            }
        }
        return ans / 2;
    }
}
```

```java [sol-Java 数组]
class Solution {
    public int minCost(int[] nums1, int[] nums2) {
        int mx = 0;
        for (int x : nums1) {
            mx = Math.max(mx, x);
        }
        for (int x : nums2) {
            mx = Math.max(mx, x);
        }

        int[] diff = new int[mx + 1];
        for (int x : nums1) {
            diff[x]++;
        }
        for (int x : nums2) {
            diff[x]--;
        }

        int ans = 0;
        for (int d : diff) {
            if (d % 2 != 0) {
                return -1;
            }
            if (d > 0) {
                ans += d;
            }
        }
        return ans / 2;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minCost(vector<int>& nums1, vector<int>& nums2) {
        unordered_map<int, int> diff;
        for (int x : nums1) {
            diff[x]++;
        }
        for (int x : nums2) {
            diff[x]--;
        }

        int ans = 0;
        for (auto& [_, d] : diff) {
            if (d % 2) {
                return -1;
            }
            if (d > 0) {
                ans += d;
            }
        }
        return ans / 2;
    }
};
```

```go [sol-Go]
func minCost(nums1, nums2 []int) (ans int) {
	diff := map[int]int{}
	for _, x := range nums1 {
		diff[x]++
	}
	for _, x := range nums2 {
		diff[x]--
	}

	for _, d := range diff {
		if d%2 != 0 {
			return -1
		}
		if d > 0 {
			ans += d
		}
	}
	return ans / 2
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{nums}_1$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

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

## 本地原创解析

### 1. 题意重述

本题来自 `五、思维题 / §5.3 等价转化`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `五、思维题 / §5.3 等价转化`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
