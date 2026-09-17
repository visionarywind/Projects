# 3592. 硬币面值还原

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/inverse-coin-change/
- 题目 slug：`inverse-coin-change`
- 来源专题：贪心与思维
- 来源分类路径：六、构造题
- 难度分：1701
- 外部题解来源：https://leetcode.cn/problems/inverse-coin-change/solutions/3705647/wan-quan-bei-bao-pythonjavacgo-by-endles-y6oq/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[完全背包：反向构造题（Python/Java/C++/Go）](https://leetcode.cn/problems/inverse-coin-change/solutions/3705647/wan-quan-bei-bao-pythonjavacgo-by-endles-y6oq/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`wan-quan-bei-bao-pythonjavacgo-by-endles-y6oq`
- topic id：`3705647`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:22:48 +0800

**前置知识**：[0-1 背包与完全背包【基础算法精讲 18】](https://www.bilibili.com/video/BV16Y411v7Y6/)

本题是 [518. 零钱兑换 II](https://leetcode.cn/problems/coin-change-ii/) 的反向构造题：给你完全背包的 DP 数组 $\textit{numWays}$，已知 $\textit{numWays}$ 是由数组 $a$ 算出来的（算方案数），请你还原数组 $a$。

想一想，$\textit{numWays}$ 中最小非零元素，意味着什么？

看示例 1 中，$\textit{numWays} = [0,1,0,2,0,3,0,4,0,5]$。

其中最小非零元素为 $\textit{numWays}[2]=1$（注意下标从 $1$ 开始），说明 $a$ 中没有比 $2$ 小的数（否则最小非零元素的下标比 $2$ 小），且元素和为 $2$ 的方案数有一种，说明 $a$ 中有 $2$，这个 $2$ 可以单独一个数，贡献一个和为 $2$ 的方案。

用这个 $2$ 去计算一个新的完全背包数组 $f$（下标从 $0$ 开始），我们得到 $f = [1,0,1,0,1,0,1,0,1,0,1]$，其中 $f[i]$ 对应和为 $i$ 的方案数。特别地，$f[0]=1$ 对应和为 $0$ 的方案数。

继续向后遍历 $\textit{numWays}$：

- 如果 $\textit{numWays}[i] = f[i]$，说明所有和为 $i$ 的方案均由小于 $i$ 的元素组成，$a$ 中没有 $i$。
- 如果 $\textit{numWays}[i] = f[i]+1$，意味着 $i$ 可以单独一个数，贡献 $1$ 个和为 $i$ 的方案，所以 $a$ 中一定有 $i$。这里 $i=4$ 符合要求。把 $i$ 加入答案，用 $i$ 继续更新 $f$（见后文）。
- 其他情况：$\textit{numWays}$ 不可能是某个数组 $a$ 的完全背包 DP 数组，返回空列表。

用这个 $4$ 去更新 $f$，得到 $f =[1,0,1,0,2,0,2,0,3,0,3]$。

继续向后遍历 $\textit{numWays}$，发现 $\textit{numWays}[6] = f[6]+1$，这意味着 $6$ 可以单独一个数，贡献 $1$ 个和为 $6$ 的方案，所以 $a$ 中一定有 $6$。

用这个 $6$ 去更新 $f$，得到 $f =[1,0,1,0,2,0,3,0,4,0,5]$。

继续向后遍历 $\textit{numWays}$，没有 $\textit{numWays}[i] = f[i]+1$。

最终 $a=[2,4,6]$。

具体请看 [视频讲解](https://www.bilibili.com/video/BV1GCNRzgEYp/?t=9m14s)，欢迎点赞关注~

## 答疑

**问**：代码中的 `f[j] += f[j - i]` 是否会导致 $f[j]$ 溢出？

**答**：不会。本题 $n\le 100$，在最坏情况下，数组 $a=[1,2,3,\ldots,100]$ 生成的 $f$ 的最大值 $190569292 < 2\cdot 10^8$。

**问**：如果数据范围扩大到 $n\le 1000$ 呢？如何避免溢出？

**答**：在算出 $f[j] > \max(\textit{numWays})$ 时，返回空列表。

```py [sol-Python3]
class Solution:
    def findCoins(self, numWays: List[int]) -> List[int]:
        n = len(numWays)
        f = [1] + [0] * n
        ans = []
        for i, ways in enumerate(numWays, 1):
            if ways == f[i]:
                continue
            if ways - 1 != f[i]:
                return []
            ans.append(i)
            # 现在得到了一个大小为 i 的物品，用 i 计算完全背包（空间优化写法）
            for j in range(i, n + 1):
                f[j] += f[j - i]
        return ans
```

```java [sol-Java]
class Solution {
    public List<Integer> findCoins(int[] numWays) {
        int n = numWays.length;
        int[] f = new int[n + 1];
        f[0] = 1;
        List<Integer> ans = new ArrayList<>();
        for (int i = 1; i <= n; i++) {
            int ways = numWays[i - 1];
            if (ways == f[i]) {
                continue;
            }
            if (ways - 1 != f[i]) {
                return List.of();
            }
            ans.add(i);
            // 现在得到了一个大小为 i 的物品，用 i 计算完全背包（空间优化写法）
            for (int j = i; j <= n; j++) {
                f[j] += f[j - i];
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> findCoins(vector<int>& numWays) {
        int n = numWays.size();
        vector<int> f(n + 1);
        f[0] = 1;
        vector<int> ans;
        for (int i = 1; i <= n; i++) {
            int ways = numWays[i - 1];
            if (ways == f[i]) {
                continue;
            }
            if (ways - 1 != f[i]) {
                return {};
            }
            ans.push_back(i);
            // 现在得到了一个大小为 i 的物品，用 i 计算完全背包（空间优化写法）
            for (int j = i; j <= n; j++) {
                f[j] += f[j - i];
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func findCoins(numWays []int) (ans []int) {
	n := len(numWays)
	f := make([]int, n+1)
	f[0] = 1
	for i := 1; i <= n; i++ {
		ways := numWays[i-1]
		if ways == f[i] {
			continue
		}
		if ways-1 != f[i] {
			return nil
		}
		ans = append(ans, i)
		// 现在得到了一个大小为 i 的物品，用 i 计算完全背包
		for j := i; j <= n; j++ {
			f[j] += f[j-i]
		}
	}
	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2)$，其中 $n$ 是 $\textit{numWays}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 相似题目

1. 动态规划题单的「**§3.2 完全背包**」。
2. 贪心与思维题单的「**六、构造题**」。

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

本题来自 `六、构造题`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `六、构造题`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
