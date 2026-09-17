# 2410. 运动员和训练师的最大匹配数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-matching-of-players-with-trainers/
- 题目 slug：`maximum-matching-of-players-with-trainers`
- 来源专题：贪心与思维
- 来源分类路径：一、贪心策略 / §1.3 双序列配对
- 难度分：1381
- 外部题解来源：https://leetcode.cn/problems/maximum-matching-of-players-with-trainers/solutions/1830866/pai-xu-shuang-zhi-zhen-by-endlesscheng-h54k/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[排序 + 双指针，两种写法（Python/Java/C++/Go）](https://leetcode.cn/problems/maximum-matching-of-players-with-trainers/solutions/1830866/pai-xu-shuang-zhi-zhen-by-endlesscheng-h54k/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`pai-xu-shuang-zhi-zhen-by-endlesscheng-h54k`
- topic id：`1830866`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:46:10 +0800

## 运动员的视角

对于任意一种匹配方案，总是可以把其中的运动员改成能力值更小的没有匹配的运动员，仍然满足题目要求。

所以可以只考虑 $\textit{players}$ 中的能力值最小的一部分人。

为方便计算，把 $\textit{players}$ 和 $\textit{trainers}$ 从小到大排序。参与匹配的运动员是 $\textit{players}$ 的前缀。

从能力值最小的运动员 $\textit{player}[0]$ 开始思考，他应当匹配训练能力值大于等于 $\textit{player}[0]$ 且最接近 $\textit{player}[0]$ 的训练师（如果选了一个训练能力值更大的，可能会导致能力值更大的运动员无法匹配）。找到训练师 $\textit{trainers}[j]$ 后，下一个与 $\textit{players}[1]$ 匹配的训练师在 $j$ 右边，所以我们可以用双指针做。

```py [sol-Python3]
class Solution:
    def matchPlayersAndTrainers(self, players: List[int], trainers: List[int]) -> int:
        players.sort()
        trainers.sort()
        j, m = 0, len(trainers)
        for i, p in enumerate(players):
            while j < m and trainers[j] < p:
                j += 1
            if j == m:  # 无法找到匹配的训练师
                return i
            j += 1  # 匹配一位训练师
        return len(players)  # 所有运动员都有匹配的训练师
```

```java [sol-Java]
class Solution {
    public int matchPlayersAndTrainers(int[] players, int[] trainers) {
        Arrays.sort(players);
        Arrays.sort(trainers);
        int n = players.length;
        int m = trainers.length;
        int j = 0;
        for (int i = 0; i < n; i++) {
            int p = players[i];
            while (j < m && trainers[j] < p) {
                j++;
            }
            if (j == m) { // 无法找到匹配的训练师
                return i;
            }
            j++; // 匹配一位训练师
        }
        return n; // 所有运动员都有匹配的训练师
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int matchPlayersAndTrainers(vector<int>& players, vector<int>& trainers) {
        ranges::sort(players);
        ranges::sort(trainers);
        int n = players.size(), m = trainers.size();
        int j = 0;
        for (int i = 0; i < n; i++) {
            while (j < m && trainers[j] < players[i]) {
                j++;
            }
            if (j == m) { // 无法找到匹配的训练师
                return i;
            }
            j++; // 匹配一位训练师
        }
        return n; // 所有运动员都有匹配的训练师
    }
};
```

```go [sol-Go]
func matchPlayersAndTrainers(players, trainers []int) int {
	slices.Sort(players)
	slices.Sort(trainers)
	j, m := 0, len(trainers)
	for i, p := range players {
		for j < m && trainers[j] < p {
			j++
		}
		if j == m { // 无法找到匹配的训练师
			return i
		}
		j++ // 匹配一位训练师
	}
	return len(players) // 所有运动员都有匹配的训练师
}
```

## 训练师的视角

也可以遍历训练师 $\textit{trainers}$，去找对应的运动员 $\textit{players}$：

- 初始化 $i=0$，$j=0$。
- 如果 $\textit{trainers}[i] < \textit{players}[j]$，不匹配，把 $i$ 加一，考虑下一个训练师能否匹配（训练能力值更大）。
- 如果 $\textit{trainers}[i] \ge  \textit{players}[j]$，匹配，把 $i$ 和 $j$ 都加一。

**注**：把 $\textit{players}$ 视作子序列，这个做法类似在 $\textit{trainers}$ 中寻找子序列 $\textit{players}$。读者可以对比下面的代码和 [392. 判断子序列](https://leetcode.cn/problems/is-subsequence/) 的代码。

```py [sol-Python3]
class Solution:
    def matchPlayersAndTrainers(self, players: List[int], trainers: List[int]) -> int:
        players.sort()
        trainers.sort()
        j, m = 0, len(players)
        for t in trainers:
            if j < m and players[j] <= t:
                j += 1
        return j
```

```java [sol-Java]
class Solution {
    public int matchPlayersAndTrainers(int[] players, int[] trainers) {
        Arrays.sort(players);
        Arrays.sort(trainers);
        int m = players.length;
        int j = 0;
        for (int t : trainers) {
            if (j < m && players[j] <= t) {
                j++;
            }
        }
        return j;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int matchPlayersAndTrainers(vector<int>& players, vector<int>& trainers) {
        ranges::sort(players);
        ranges::sort(trainers);
        int m = players.size();
        int j = 0;
        for (int t : trainers) {
            if (j < m && players[j] <= t) {
                j++;
            }
        }
        return j;
    }
};
```

```go [sol-Go]
func matchPlayersAndTrainers(players, trainers []int) int {
	slices.Sort(players)
	slices.Sort(trainers)
	j, m := 0, len(players)
	for _, t := range trainers {
		if j < m && players[j] <= t {
			j++
		}
	}
	return j
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n + m\log m)$，其中 $n$ 为 $\textit{players}$ 的长度，$m$ 为 $\textit{trainers}$ 的长度。瓶颈在排序上。
- 空间复杂度：$\mathcal{O}(1)$，忽略排序时的栈开销。

## 总结

训练师的视角不需要写二重循环，更简单。

这类问题类似「子序列匹配」，我们要在 $\textit{trainers}$ 中寻找子序列 $\textit{players}$，通常的子序列匹配是 $\textit{trainers}[i]=\textit{players}[j]$，本题是 $\textit{trainers}[i]\ge \textit{players}[j]$。对于这类问题，我的经验是外层循环遍历相对来说更长的 $\textit{trainers}$：不匹配就继续循环，匹配就把 $j$ 加一，继续循环。如果外层循环遍历更短的子序列 $\textit{players}$，就需要写二重循环了。

## 专题训练

贪心题单的「**§1.3 双序列配对**」。

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

本题来自 `一、贪心策略 / §1.3 双序列配对`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、贪心策略 / §1.3 双序列配对`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
