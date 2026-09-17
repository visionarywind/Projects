# 3203. 合并两棵树后的最小直径

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/find-minimum-diameter-after-merging-two-trees/
- 题目 slug：`find-minimum-diameter-after-merging-two-trees`
- 来源专题：链表、树与回溯
- 来源分类路径：三、一般树 / §3.5 树的直径
- 难度分：2266
- 外部题解来源：https://leetcode.cn/problems/find-minimum-diameter-after-merging-two-trees/solutions/2826587/lian-jie-zhi-jing-zhong-dian-pythonjavac-0e1c/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[连接直径中点（Python/Java/C++/Go）](https://leetcode.cn/problems/find-minimum-diameter-after-merging-two-trees/solutions/2826587/lian-jie-zhi-jing-zhong-dian-pythonjavac-0e1c/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`lian-jie-zhi-jing-zhong-dian-pythonjavac-0e1c`
- topic id：`2826587`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:49:15 +0800

**前置知识**：[树形 DP：树的直径【基础算法精讲 23】](https://www.bilibili.com/video/BV17o4y187h1/)

设 $d_1,d_2$ 分别为两棵树的直径长度。答案有三种情况：

- 第一棵树的直径特别长。那么连边后，新树的直径仍然为第一棵树的直径，答案为 $d_1$。
- 第二棵树的直径特别长。那么连边后，新树的直径仍然为第二棵树的直径，答案为 $d_2$。
- 新树的直径经过添加的边。假设我们连接了第一棵树的节点 $x_1$ 与第二棵树的节点 $x_2$，那么新树的直径相当于以下三部分之和：
  - $x_1$ 到第一棵树最远点的距离。由直径的定义可知，选 $x_1$ 为第一棵树的直径中点，可以让 $x_1$ 到第一棵树最远点的距离**最小**。
  - $x_2$ 到第二棵树最远点的距离。由直径的定义可知，选 $x_2$ 为第二棵树的直径中点，可以让 $x_2$ 到第二棵树最远点的距离**最小**。
  - $x_1\text{-}x_2$ 这条边的长度，即 $1$。
  - 三部分之和为

$$
\begin{align}
    & \left\lceil\dfrac{d_1}{2}\right\rceil + \left\lceil\dfrac{d_2}{2}\right\rceil + 1   \\
={} & \left\lfloor\dfrac{d_1+1}{2}\right\rfloor + \left\lfloor\dfrac{d_2+1}{2}\right\rfloor + 1       \\
\end{align}
$$

![w404d.png](https://pic.leetcode.cn/1719719529-GSlAHr-w404d.png)

三种情况取最大值，答案为

$$
\max\left\{d_1,d_2, \left\lfloor\dfrac{d_1+1}{2}\right\rfloor + \left\lfloor\dfrac{d_2+1}{2}\right\rfloor + 1  \right\}
$$

具体请看 [视频讲解](https://www.bilibili.com/video/BV16w4m1e7y3/) 第四题，包括更详细的证明，欢迎点赞关注！

```py [sol-Python3]
class Solution:
    def diameter(self, edges: List[List[int]]) -> int:
        g = [[] for _ in range(len(edges) + 1)]
        for x, y in edges:
            g[x].append(y)
            g[y].append(x)

        res = 0
        def dfs(x: int, fa: int) -> int:
            nonlocal res
            max_len = 0
            for y in g[x]:
                if y != fa:
                    sub_len = dfs(y, x) + 1
                    res = max(res, max_len + sub_len)
                    max_len = max(max_len, sub_len)
            return max_len
        dfs(0, -1)
        return res

    def minimumDiameterAfterMerge(self, edges1: List[List[int]], edges2: List[List[int]]) -> int:
        d1 = self.diameter(edges1)
        d2 = self.diameter(edges2)
        return max(d1, d2, (d1 + 1) // 2 + (d2 + 1) // 2 + 1)
```

```java [sol-Java]
class Solution {
    public int minimumDiameterAfterMerge(int[][] edges1, int[][] edges2) {
        int d1 = diameter(edges1);
        int d2 = diameter(edges2);
        return Math.max(Math.max(d1, d2), (d1 + 1) / 2 + (d2 + 1) / 2 + 1);
    }

    private int res;

    private int diameter(int[][] edges) {
        List<Integer>[] g = new ArrayList[edges.length + 1];
        Arrays.setAll(g, i -> new ArrayList<>());
        for (int[] e : edges) {
            int x = e[0];
            int y = e[1];
            g[x].add(y);
            g[y].add(x);
        }

        res = 0;
        dfs(0, -1, g);
        return res;
    }

    private int dfs(int x, int fa, List<Integer>[] g) {
        int maxLen = 0;
        for (int y : g[x]) {
            if (y != fa) {
                int subLen = dfs(y, x, g) + 1;
                res = Math.max(res, maxLen + subLen);
                maxLen = Math.max(maxLen, subLen);
            }
        }
        return maxLen;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int diameter(vector<vector<int>>& edges) {
        vector<vector<int>> g(edges.size() + 1);
        for (auto& e : edges) {
            int x = e[0], y = e[1];
            g[x].push_back(y);
            g[y].push_back(x);
        }

        int res = 0;
        auto dfs = [&](auto&& dfs, int x, int fa) -> int {
            int max_len = 0;
            for (auto y : g[x]) {
                if (y != fa) {
                    int sub_len = dfs(dfs, y, x) + 1;
                    res = max(res, max_len + sub_len);
                    max_len = max(max_len, sub_len);
                }
            }
            return max_len;
        };
        dfs(dfs, 0, -1);
        return res;
    }

    int minimumDiameterAfterMerge(vector<vector<int>>& edges1, vector<vector<int>>& edges2) {
        int d1 = diameter(edges1);
        int d2 = diameter(edges2);
        return max({d1, d2, (d1 + 1) / 2 + (d2 + 1) / 2 + 1});
    }
};
```

```go [sol-Go]
func diameter(edges [][]int) (res int) {
	g := make([][]int, len(edges)+1)
	for _, e := range edges {
		x, y := e[0], e[1]
		g[x] = append(g[x], y)
		g[y] = append(g[y], x)
	}
	var dfs func(int, int) int
	dfs = func(x, fa int) (maxLen int) {
		for _, y := range g[x] {
			if y != fa {
				subLen := dfs(y, x) + 1
				res = max(res, maxLen+subLen)
				maxLen = max(maxLen, subLen)
			}
		}
		return
	}
	dfs(0, -1)
	return
}

func minimumDiameterAfterMerge(edges1, edges2 [][]int) int {
	d1 := diameter(edges1)
	d2 := diameter(edges2)
	return max(d1, d2, (d1+1)/2+(d2+1)/2+1)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n+m)$，其中 $n$ 是 $\textit{edges}_1$ 的长度，$m$ 是 $\textit{edges}_2$ 的长度。
- 空间复杂度：$\mathcal{O}(n+m)$。

#### 相似题目

见 [动态规划题单](https://leetcode.cn/circle/discuss/tXLS3i/) 中的「**§12.1 树的直径**」。

## 分类题单

以下题单没有特定的顺序，可以按照个人喜好刷题。

1. [滑动窗口（定长/不定长/多指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/贪心/脑筋急转弯）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心算法（基本贪心策略/反悔/区间/字典序/数学/思维/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

## 本地原创解析

### 1. 题意重述

本题来自 `三、一般树 / §3.5 树的直径`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `三、一般树 / §3.5 树的直径`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
