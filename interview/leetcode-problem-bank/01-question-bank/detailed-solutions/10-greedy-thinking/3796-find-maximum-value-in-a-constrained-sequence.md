# 3796. 找到带限制序列的最大值

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/find-maximum-value-in-a-constrained-sequence/
- 题目 slug：`find-maximum-value-in-a-constrained-sequence`
- 来源专题：贪心与思维
- 来源分类路径：五、思维题 / §5.6 两次扫描
- 难度分：1833
- 外部题解来源：https://leetcode.cn/problems/find-maximum-value-in-a-constrained-sequence/solutions/3872153/liang-ci-sao-miao-fa-pythonjavacgo-by-en-p7qc/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两次扫描，传播约束（Python/Java/C++/Go）](https://leetcode.cn/problems/find-maximum-value-in-a-constrained-sequence/solutions/3872153/liang-ci-sao-miao-fa-pythonjavacgo-by-en-p7qc/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`liang-ci-sao-miao-fa-pythonjavacgo-by-en-p7qc`
- topic id：`3872153`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:13:14 +0800

## 核心思路

每个 $a[i]$ 的最大值会受到三个约束：

- $i$ 处的 $\textit{maxVal}$（如果有）。
- 来自左侧元素的约束。需要**从左到右**扫描一遍算出来。
- 来自右侧元素的约束。需要**从右到左**扫描一遍算出来。

## 具体思路

设 $i$ 处的值不得超过 $\textit{maxVal}[i]$。如果此处没有限制，那么 $\textit{maxVal}[i]=\infty$。

第一次扫描，从左到右。如果没有 $\textit{maxVal}[i]$ 的限制，那么拉满，即 $a[i+1] = a[i] + \textit{diff}[i]$ 是最优的。但这不能超过 $\textit{maxVal}[i+1]$，所以有

$$
a[i+1] = \min(a[i] + \textit{diff}[i], \textit{maxVal}[i+1])
$$

第一次扫描后，每个 $a[i]$ 都尽量拉满。但和 $\textit{maxVal}$ 计算 $\min$ 后，相邻元素可能不满足 $\textit{diff}$ 的约束，可能前一个数比较大，后一个数被 $\textit{maxVal}$ 突然拉低，导致两个相邻元素之差超过 $\textit{diff}$ 值。怎么办？拉低后，再反向修正一下。但先别急，遍历到最后一个元素再说。

注意到，对于最后一个数 $a[n-1]$，它的最大值只取决于左边的元素大小，以及 $\textit{maxVal}[n-1]$。第一次扫描已经把 $0$ 到 $n-2$ 的所有限制传播到了 $n-1$，所以第一次扫描结束时，$a[n-1]$ 就已经是它能达到的最大值了。

第二次扫描，从右到左。从已确定的值 $a[n-1]$ 出发：

- 用 $a[n-1] + \textit{diff}[n-2]$ 更新 $a[n-2]$ 的最小值。
- 用 $a[n-2] + \textit{diff}[n-3]$ 更新 $a[n-3]$ 的最小值。
- ……
- 用 $a[2] + \textit{diff}[1]$ 更新 $a[1]$ 的最小值。

注意 $a[0]=0$ 是固定值，无需修正。 

⚠**注意**：这里取的是 $\min$，只会把 $a[i]$ 变小，没有破坏在第一次扫描时建立的 $a[i+1]\le a[i] + \textit{diff}[i]$ 的约束。所以两次扫描后，每个 $a[i]$ 都满足 $\textit{diff}$ 的约束。我们得到了最终的 $a$。

最后返回 $\max(a)$。

[本题视频讲解](https://www.bilibili.com/video/BV1mUijBnEda/?t=4m32s)，欢迎点赞关注~

```py [sol-Python3]
class Solution:
    def findMaxVal(self, n: int, restrictions: List[List[int]], diff: List[int]) -> int:
        max_val = [inf] * n
        for i, mx in restrictions:
            max_val[i] = mx

        a = [0] * n
        for i, d in enumerate(diff):
            a[i + 1] = min(a[i] + d, max_val[i + 1])
        for i in range(n - 2, 0, -1):
            a[i] = min(a[i], a[i + 1] + diff[i])
        return max(a)
```

```java [sol-Java]
class Solution {
    public int findMaxVal(int n, int[][] restrictions, int[] diff) {
        int[] maxVal = new int[n];
        Arrays.fill(maxVal, Integer.MAX_VALUE);
        for (int[] r : restrictions) {
            maxVal[r[0]] = r[1];
        }

        int[] a = new int[n];
        for (int i = 0; i < diff.length; i++) {
            a[i + 1] = Math.min(a[i] + diff[i], maxVal[i + 1]);
        }
        int ans = a[n - 1];
        for (int i = n - 2; i > 0; i--) {
            a[i] = Math.min(a[i], a[i + 1] + diff[i]);
            ans = Math.max(ans, a[i]);
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int findMaxVal(int n, vector<vector<int>>& restrictions, vector<int>& diff) {
        vector<int> max_val(n, INT_MAX);
        for (auto& r : restrictions) {
            max_val[r[0]] = r[1];
        }

        vector<int> a(n);
        for (int i = 0; i < n - 1; i++) {
            a[i + 1] = min(a[i] + diff[i], max_val[i + 1]);
        }
        for (int i = n - 2; i > 0; i--) {
            a[i] = min(a[i], a[i + 1] + diff[i]);
        }
        return ranges::max(a);
    }
};
```

```go [sol-Go]
func findMaxVal(n int, restrictions [][]int, diff []int) int {
	maxVal := make([]int, n)
	for i := range maxVal {
		maxVal[i] = math.MaxInt
	}
	for _, r := range restrictions {
		maxVal[r[0]] = r[1]
	}

	a := make([]int, n)
	for i, d := range diff {
		a[i+1] = min(a[i]+d, maxVal[i+1])
	}
	for i := n - 2; i > 0; i-- {
		a[i] = min(a[i], a[i+1]+diff[i])
	}
	return slices.Max(a)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$。
- 空间复杂度：$\mathcal{O}(n)$。

## 相似题目

如果改成 $\textit{diff}[i] = 1$，但 $n$ 最大是 $10^9$，怎么做？

这题是 [1840. 最高建筑高度](https://leetcode.cn/problems/maximum-building-height/)。

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

## 本地原创解析

### 1. 题意重述

本题来自 `五、思维题 / §5.6 两次扫描`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `五、思维题 / §5.6 两次扫描`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
