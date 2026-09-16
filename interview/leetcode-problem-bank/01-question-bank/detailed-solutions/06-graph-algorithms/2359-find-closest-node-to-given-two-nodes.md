# 2359. 找到离给定两个节点最近的节点

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/find-closest-node-to-given-two-nodes/
- 题目 slug：`find-closest-node-to-given-two-nodes`
- 来源专题：图论算法
- 来源分类路径：二、拓扑排序 / §2.3 基环树
- 难度分：1715
- 外部题解来源：https://leetcode.cn/problems/find-closest-node-to-given-two-nodes/solutions/1710829/ji-suan-dao-mei-ge-dian-de-ju-chi-python-gr2u/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：最短路 / 我吹过你吹过的晚风（Python/Java/C++/Go）](https://leetcode.cn/problems/find-closest-node-to-given-two-nodes/solutions/1710829/ji-suan-dao-mei-ge-dian-de-ju-chi-python-gr2u/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`ji-suan-dao-mei-ge-dian-de-ju-chi-python-gr2u`
- topic id：`1710829`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 方法一：计算最短路

我们需要知道 $\textit{node}_1$ 到每个点的最短路长度 $\textit{dis}_1[i]$，以及 $\textit{node}_2$ 到每个点的最短路长度 $\textit{dis}_2[i]$。

题目要我们计算的，是 $\max(\textit{dis}_1[i],\textit{dis}_2[i])$ 的最小值对应的节点编号 $i$。若没有这样的节点，返回 $-1$。

求最短路可以用 BFS 做。不过，由于本题输入是树或者 [内向基环树](https://leetcode.cn/problems/maximum-employees-to-be-invited-to-a-meeting/solution/nei-xiang-ji-huan-shu-tuo-bu-pai-xu-fen-c1i1b/)（森林），每个连通块至多有一个环，我们可以用一个简单的循环求出 $\textit{dis}_i$。

```py [sol-Python3]
class Solution:
    def closestMeetingNode(self, edges: List[int], node1: int, node2: int) -> int:
        n = len(edges)
        def calc_dis(x: int) -> List[int]:
            dis = [n] * n  # 初始化成 n，表示无法到达或者尚未访问的节点
            d = 0
            # 从 x 出发，直到无路可走（x=-1）或者重复访问节点（dis[x]<n）
            while x >= 0 and dis[x] == n:
                dis[x] = d
                d += 1
                x = edges[x]
            return dis

        dis1 = calc_dis(node1)
        dis2 = calc_dis(node2)

        min_dis, ans = n, -1
        for i, (d1, d2) in enumerate(zip(dis1, dis2)):
            d = max(d1, d2)
            if d < min_dis:
                min_dis, ans = d, i
        return ans
```

```java [sol-Java]
class Solution {
    public int closestMeetingNode(int[] edges, int node1, int node2) {
        int[] dis1 = calcDis(edges, node1);
        int[] dis2 = calcDis(edges, node2);

        int n = edges.length;
        int minDis = n;
        int ans = -1;
        for (int i = 0; i < n; i++) {
            int d = Math.max(dis1[i], dis2[i]);
            if (d < minDis) {
                minDis = d;
                ans = i;
            }
        }
        return ans;
    }

    private int[] calcDis(int[] edges, int x) {
        int n = edges.length;
        int[] dis = new int[n];
        Arrays.fill(dis, n); // n 表示无法到达或者尚未访问的节点
        // 从 x 出发，直到无路可走（x=-1）或者重复访问节点（dis[x]<n）
        for (int d = 0; x >= 0 && dis[x] == n; x = edges[x]) {
            dis[x] = d++;
        }
        return dis;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int closestMeetingNode(vector<int>& edges, int node1, int node2) {
        int n = edges.size();
        auto calc_dis = [&](int x) {
            vector<int> dis(n, n); // 初始化成 n，表示无法到达或者尚未访问的节点
            // 从 x 出发，直到无路可走（x=-1）或者重复访问节点（dis[x]<n）
            for (int d = 0; x >= 0 && dis[x] == n; x = edges[x]) {
                dis[x] = d++;
            }
            return dis;
        };

        vector<int> dis1 = calc_dis(node1);
        vector<int> dis2 = calc_dis(node2);

        int min_dis = n, ans = -1;
        for (int i = 0; i < n; i++) {
            int d = max(dis1[i], dis2[i]);
            if (d < min_dis) {
                min_dis = d;
                ans = i;
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func closestMeetingNode(edges []int, node1, node2 int) int {
	n := len(edges)
	calcDis := func(x int) []int {
		dis := make([]int, n)
		for i := range dis {
			dis[i] = n // n 表示无法到达或者尚未访问的节点
		}
		// 从 x 出发，直到无路可走（x=-1）或者重复访问节点（dis[x]<n）
		for d := 0; x >= 0 && dis[x] == n; x = edges[x] {
			dis[x] = d
			d++
		}
		return dis
	}

	dis1 := calcDis(node1)
	dis2 := calcDis(node2)

	minDis, ans := n, -1
	for i, d1 := range dis1 {
		d := max(d1, dis2[i])
		if d < minDis {
			minDis, ans = d, i
		}
	}
	return ans
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{edges}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 方法二：「我吹过你吹过的晚风」

为方便描述，把 $\textit{node}_1$ 和 $\textit{node}_2$ 分别视作两个人 $x$ 和 $y$。

两个人顺着 $\textit{edges}$ 移动，标记访问过的节点，即更新 $\textit{visX}[x]=\texttt{true}$ 以及 $\textit{visY}[y]=\texttt{true}$。

如果某个时刻，$x$ 到达 $y$ 走过的路，即 $\textit{visY}[x]$ 为 $\texttt{true}$，那么当前 $x$ 所在节点，就是两人都可以到达的节点，且距离最近。

如果某个时刻，$y$ 到达 $x$ 走过的路，即 $\textit{visX}[y]$ 为 $\texttt{true}$，那么当前 $y$ 所在节点，就是两人都可以到达的节点，且距离最近。

如果某个时刻上面两种情况同时发生，返回二者的最小值。

如果两个人都走到死路或者各自走过的路，返回 $-1$。

```py [sol-Python3]
class Solution:
    def closestMeetingNode(self, edges: List[int], x: int, y: int) -> int:
        ans = n = len(edges)
        vis_x = [False] * n
        vis_y = [False] * n

        while not vis_x[x] or not vis_y[y]:  # x 或 y 没有访问过
            vis_x[x] = vis_y[y] = True  # 标记访问过

            if vis_y[x]:  # 我吹过你吹过的晚风
                ans = x
            if vis_x[y]:
                ans = min(ans, y)  # 如果有多个答案，返回最小的节点编号
            if ans < n:
                return ans

            if edges[x] >= 0:
                x = edges[x]  # 继续走
            if edges[y] >= 0:
                y = edges[y]  # 继续走

        return -1
```

```java [sol-Java]
class Solution {
    public int closestMeetingNode(int[] edges, int x, int y) {
        int n = edges.length;
        int ans = n;
        boolean[] visX = new boolean[n];
        boolean[] visY = new boolean[n];

        while (!visX[x] || !visY[y]) { // x 或 y 没有访问过
            visX[x] = visY[y] = true; // 标记访问过

            if (visY[x]) { // 我吹过你吹过的晚风
                ans = x;
            }
            if (visX[y]) {
                ans = Math.min(ans, y); // 如果有多个答案，返回最小的节点编号
            }
            if (ans < n) {
                return ans;
            }

            if (edges[x] >= 0) {
                x = edges[x]; // 继续走
            }
            if (edges[y] >= 0) {
                y = edges[y]; // 继续走
            }
        }

        return -1;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int closestMeetingNode(vector<int>& edges, int x, int y) {
        int n = edges.size();
        int ans = n;
        vector<int8_t> vis_x(n), vis_y(n);

        while (!vis_x[x] || !vis_y[y]) { // x 或 y 没有访问过
            vis_x[x] = vis_y[y] = true; // 标记访问过

            if (vis_y[x]) { // 我吹过你吹过的晚风
                ans = x;
            }
            if (vis_x[y]) {
                ans = min(ans, y); // 如果有多个答案，返回最小的节点编号
            }
            if (ans < n) {
                return ans;
            }

            if (edges[x] >= 0) {
                x = edges[x]; // 继续走
            }
            if (edges[y] >= 0) {
                y = edges[y]; // 继续走
            }
        }

        return -1;
    }
};
```

```go [sol-Go]
func closestMeetingNode(edges []int, x, y int) int {
	n := len(edges)
	ans := n
	visX := make([]bool, n)
	visY := make([]bool, n)

	for !visX[x] || !visY[y] { // x 或 y 没有访问过
		visX[x] = true // 标记访问过
		visY[y] = true

		if visY[x] { // 我吹过你吹过的晚风
			ans = x
		}
		if visX[y] {
			ans = min(ans, y) // 如果有多个答案，返回最小的节点编号
		}
		if ans < n {
			return ans
		}

		if edges[x] >= 0 {
			x = edges[x] // 继续走
		}
		if edges[y] >= 0 {
			y = edges[y] // 继续走
		}
	}

	return -1
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{edges}$ 的长度。单看 $x$ 或 $y$，每次循环会标记一个没有访问过的节点，一共有 $n$ 个节点，所以至多循环 $n$ 次。
- 空间复杂度：$\mathcal{O}(n)$。

## 思考题

1. 如果输入的不止两个节点 $\textit{node}_1$ 和 $\textit{node}_2$，而是一个很长的 $\textit{nodes}$ 列表，要怎么做呢？
2. 如果输入的是 $\textit{queries}$ 询问数组，每个询问包含两个节点 $\textit{node}_1$ 和 $\textit{node}_2$，你需要快速计算 `closestMeetingNode(edges, node1, node2)`，要怎么做呢？

**解答**：见 [视频讲解](https://www.bilibili.com/video/BV1Ba411N78j/?t=22m01s) 第三题。

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

本题来自 `二、拓扑排序 / §2.3 基环树`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、拓扑排序 / §2.3 基环树`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
