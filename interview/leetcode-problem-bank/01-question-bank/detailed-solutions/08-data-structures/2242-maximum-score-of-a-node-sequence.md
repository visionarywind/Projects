# 2242. 节点序列的最大得分

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-score-of-a-node-sequence/
- 题目 slug：`maximum-score-of-a-node-sequence`
- 来源专题：常用数据结构
- 来源分类路径：零、常用枚举技巧 / §0.2 枚举中间 / §0.2.2 进阶
- 难度分：2304
- 外部题解来源：https://leetcode.cn/problems/maximum-score-of-a-node-sequence/solutions/1426715/by-endlesscheng-dt8h/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[一步步提示如何做出此题（Python/Java/C++/Go）](https://leetcode.cn/problems/maximum-score-of-a-node-sequence/solutions/1426715/by-endlesscheng-dt8h/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`by-endlesscheng-dt8h`
- topic id：`1426715`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

#### 提示 1-1

试试枚举可不可以。（做题时优先考虑最简单的算法）

#### 提示 1-2

枚举谁呢？可以枚举点，也可以枚举边。

#### 提示 2-1

**简化问题**可以帮助我们找到思路。

如果序列长度为 $3$，要如何枚举？

#### 提示 2-2

只要 $3$ 个节点的话，我们可以枚举端点，可以枚举中间的节点，还可以枚举边。

这几种方案都试着想一想，哪一种是最方便的呢？

#### 提示 2-3

枚举**中间**的点是最方便的，算出与其相邻的分数最大的两个点即可。

相比枚举端点，**枚举中间的效率也要更高**。

顺着这个思路去思考原问题。

#### 提示 3-1

设序列为 $a-x-y-b$（$-$ 表示边），枚举 $\textit{edges}$ 中的每条边，作为序列**正中间**的那条边，即 $x-y$。

#### 提示 3-2

我们需要把与 $x$ 相邻的点中，分数最大且不同于 $y$ 和 $b$ 的点作为 $a$；把与 $y$ 相邻的点中，分数最大且不同于 $x$ 和 $a$ 的点作为 $b$。

#### 提示 3-3

与 $x$ 相邻的点中，由于只需要与 $y$ 和 $b$ 不一样，我们仅需要保留分数最大的三个点，$a$ 必定在这三个点中。

#### 提示 3-4

剩下要做的，就是在枚举 $\textit{edges}$ 前，预处理出这三个点。

代码实现时，可以用排序（见 Go 和 Java）、堆（见 Python `nlargest`）、快速选择（见 C++ `nth_element`）或者手动维护求前三大。最优的时间复杂度为 $O(n+m)$。

```Python [sol1-Python3]
class Solution:
    def maximumScore(self, scores: List[int], edges: List[List[int]]) -> int:
        g = [[] for _ in range(len(scores))]
        for x, y in edges:
            g[x].append((scores[y], y))
            g[y].append((scores[x], x))
        for i, vs in enumerate(g):
            if len(vs) > 3: 
                g[i] = nlargest(3, vs)

        # 下面这一段可以简写成一行，为了可读性这里就不写了
        ans = -1
        for x, y in edges:
            for score_a, a in g[x]:
                for score_b, b in g[y]:
                    if y != a != b != x:
                        ans = max(ans, score_a + scores[x] + scores[y] + score_b)
        return ans
```

```java [sol1-Java]
class Solution {
    public int maximumScore(int[] scores, int[][] edges) {
        var n = scores.length;
        List<int[]>[] g = new ArrayList[n];
        Arrays.setAll(g, e -> new ArrayList<>());
        for (var e : edges) {
            int x = e[0], y = e[1];
            g[x].add(new int[]{scores[y], y});
            g[y].add(new int[]{scores[x], x});
        }
        for (var i = 0; i < n; i++)
            if (g[i].size() > 3) {
                g[i].sort((a, b) -> (b[0] - a[0]));
                g[i] = new ArrayList<>(g[i].subList(0, 3));
            }

        var ans = -1;
        for (var e : edges) {
            int x = e[0], y = e[1];
            for (var p : g[x]) {
                var a = p[1];
                for (var q : g[y]) {
                    var b = q[1];
                    if (a != y && b != x && a != b)
                        ans = Math.max(ans, p[0] + scores[x] + scores[y] + q[0]);
                }
            }
        }
        return ans;
    }
}
```

```C++ [sol1-C++]
class Solution {
public:
    int maximumScore(vector<int> &scores, vector<vector<int>> &edges) {
        int n = scores.size();
        vector<vector<pair<int, int>>> g(n);
        for (auto &e : edges) {
            int x = e[0], y = e[1];
            g[x].emplace_back(-scores[y], y);
            g[y].emplace_back(-scores[x], x);
        }
        for (auto &vs : g)
            if (vs.size() > 3) {
                nth_element(vs.begin(), vs.begin() + 3, vs.end());
                vs.resize(3);
            }

        int ans = -1;
        for (auto &e : edges) {
            int x = e[0], y = e[1];
            for (auto &[score_a, a] : g[x])
                for (auto &[score_b, b] : g[y])
                    if (a != y && b != x && a != b)
                        ans = max(ans, -score_a + scores[x] + scores[y] - score_b);
        }
        return ans;
    }
};
```

```go [sol1-Go]
func maximumScore(scores []int, edges [][]int) int {
	type nb struct{ to, s int }
	g := make([][]nb, len(scores))
	for _, e := range edges {
		x, y := e[0], e[1]
		g[x] = append(g[x], nb{y, scores[y]})
		g[y] = append(g[y], nb{x, scores[x]})
	}
	for i, vs := range g {
		if len(vs) > 3 {
			sort.Slice(vs, func(i, j int) bool { return vs[i].s > vs[j].s })
			g[i] = vs[:3]
		}
	}

	ans := -1
	for _, e := range edges {
		x, y := e[0], e[1]
		for _, p := range g[x] {
			for _, q := range g[y] {
				if p.to != y && q.to != x && p.to != q.to {
					ans = max(ans, p.s+scores[x]+scores[y]+q.s)
				}
			}
		}
	}
	return ans
}

func max(a, b int) int { if b > a { return b }; return a }
```

#### 相似题目

- [Codeforces 666B. World Tour](https://codeforces.com/problemset/problem/666/B)

#### 最后

欢迎关注我的B站频道：[灵茶山艾府](https://space.bilibili.com/206214)，定期更新算法讲解视频哦~

## 本地原创解析

### 1. 题意重述

本题来自 `零、常用枚举技巧 / §0.2 枚举中间 / §0.2.2 进阶`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `零、常用枚举技巧 / §0.2 枚举中间 / §0.2.2 进阶`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
