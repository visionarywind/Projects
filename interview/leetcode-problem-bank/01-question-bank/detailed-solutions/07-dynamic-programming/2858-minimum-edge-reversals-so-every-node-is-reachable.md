# 2858. 可以到达每一个节点的最少边反转次数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-edge-reversals-so-every-node-is-reachable/
- 题目 slug：`minimum-edge-reversals-so-every-node-is-reachable`
- 来源专题：动态规划
- 来源分类路径：十二、树形 DP / §12.4 换根 DP
- 难度分：2295
- 外部题解来源：https://leetcode.cn/problems/minimum-edge-reversals-so-every-node-is-reachable/solutions/2445681/mo-ban-huan-gen-dppythonjavacgojs-by-end-8qiu/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【模板】换根 DP（Python/Java/C++/Go/JS）](https://leetcode.cn/problems/minimum-edge-reversals-so-every-node-is-reachable/solutions/2445681/mo-ban-huan-gen-dppythonjavacgojs-by-end-8qiu/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`mo-ban-huan-gen-dppythonjavacgojs-by-end-8qiu`
- topic id：`2445681`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

[视频讲解](https://www.bilibili.com/video/BV1PV411N76R/) 第四题。

换根 DP 原理：[【图解】一张图秒懂换根 DP！](https://leetcode.cn/problems/sum-of-distances-in-tree/solution/tu-jie-yi-zhang-tu-miao-dong-huan-gen-dp-6bgb/)

本题可以先计算出以 $0$ 为根时的答案：在建图的时候，对于一条 $x\rightarrow y$ 的边，把 $(y,1)$ 加到 $x$ 的邻居，把 $(x,-1)$ 加到 $y$ 的邻居，从而可以在递归过程中统计有多少条边是需要反向的。

然后换根，假设 $y$ 是 $x$ 的儿子节点，从 $x$ 换根到 $y$，只会影响 $x$ 和 $y$ 的父子关系，其余节点不受影响，所以

$$
\textit{ans}[y] = \textit{ans}[x] + \textit{direction}
$$

如果从 $x$ 到 $y$ 不需要反向，则换根后需要反向，$\textit{direction}=1$，否则等于 $-1$，这正好就是建图时我们添加的 $1$ 和 $-1$。

```py [sol-Python3]
class Solution:
    def minEdgeReversals(self, n: int, edges: List[List[int]]) -> List[int]:
        g = [[] for _ in range(n)]
        for x, y in edges:
            g[x].append((y, 1))
            g[y].append((x, -1))  # 从 y 到 x 需要反向

        ans = [0] * n
        def dfs(x: int, fa: int) -> None:
            for y, dir in g[x]:
                if y != fa:
                    ans[0] += dir < 0
                    dfs(y, x)
        dfs(0, -1)

        def reroot(x: int, fa: int) -> None:
            for y, dir in g[x]:
                if y != fa:
                    ans[y] = ans[x] + dir  # dir 就是从 x 换到 y 的「变化量」
                    reroot(y, x)
        reroot(0, -1)
        return ans
```

```java [sol-Java]
class Solution {
    public int[] minEdgeReversals(int n, int[][] edges) {
        List<int[]>[] g = new ArrayList[n];
        Arrays.setAll(g, e -> new ArrayList<>());
        for (var e : edges) {
            int x = e[0], y = e[1];
            g[x].add(new int[]{y, 1});
            g[y].add(new int[]{x, -1}); // 从 y 到 x 需要反向
        }

        var ans = new int[n];
        dfs(0, -1, g, ans);
        reroot(0, -1, g, ans);
        return ans;
    }

    private void dfs(int x, int fa, List<int[]>[] g, int[] ans) {
        for (var e : g[x]) {
            int y = e[0], dir = e[1];
            if (y != fa) {
                if (dir < 0) {
                    ans[0]++;
                }
                dfs(y, x, g, ans);
            }
        }
    }

    private void reroot(int x, int fa, List<int[]>[] g, int[] ans) {
        for (var e : g[x]) {
            int y = e[0], dir = e[1];
            if (y != fa) {
                ans[y] = ans[x] + dir; // dir 就是从 x 换到 y 的「变化量」
                reroot(y, x, g, ans);
            }
        }
    }
}
```

```cpp [sol-C++]
class Solution {
    vector<vector<pair<int, int>>> g;
    vector<int> ans;

    void dfs(int x, int fa) {
        for (auto &[y, dir] : g[x]) {
            if (y != fa) {
                ans[0] += dir < 0;
                dfs(y, x);
            }
        }
    }

    void reroot(int x, int fa) {
        for (auto &[y, dir] : g[x]) {
            if (y != fa) {
                ans[y] = ans[x] + dir; // dir 就是从 x 换到 y 的「变化量」
                reroot(y, x);
            }
        }
    }

public:
    vector<int> minEdgeReversals(int n, vector<vector<int>> &edges) {
        g.resize(n);
        for (auto &e : edges) {
            int x = e[0], y = e[1];
            g[x].emplace_back(y, 1);
            g[y].emplace_back(x, -1); // 从 y 到 x 需要反向
        }

        ans.resize(n);
        dfs(0, -1);
        reroot(0, -1);
        return ans;
    }
};
```

```go [sol-Go]
func minEdgeReversals(n int, edges [][]int) (ans []int) {
	type pair struct{ to, dir int }
	g := make([][]pair, n)
	for _, e := range edges {
		x, y := e[0], e[1]
		g[x] = append(g[x], pair{y, 1})
		g[y] = append(g[y], pair{x, -1}) // 从 y 到 x 需要反向
	}

	ans = make([]int, n)
	var dfs func(int, int)
	dfs = func(x, fa int) {
		for _, e := range g[x] {
			y := e.to
			if y != fa {
				if e.dir < 0 {
					ans[0]++
				}
				dfs(y, x)
			}
		}
	}
	dfs(0, -1)

	var reroot func(int, int)
	reroot = func(x, fa int) {
		for _, e := range g[x] {
			y := e.to
			if y != fa {
				ans[y] = ans[x] + e.dir // e.dir 就是从 x 换到 y 的「变化量」
				reroot(y, x)
			}
		}
	}
	reroot(0, -1)
	return ans
}
```

```js [sol-JavaScript]
var minEdgeReversals = function (n, edges) {
    const g = new Array(n).fill(null).map(() => []);
    for (const [x, y] of edges) {
        g[x].push([y, 1]);
        g[y].push([x, -1]); // 从 y 到 x 需要反向
    }

    const ans = new Array(n).fill(0);
    function dfs(x, fa) {
        for (const [y, dir] of g[x]) {
            if (y !== fa) {
                ans[0] += dir < 0;
                dfs(y, x);
            }
        }
    }
    dfs(0, -1);

    function reroot(x, fa) {
        for (const [y, dir] of g[x]) {
            if (y !== fa) {
                ans[y] = ans[x] + dir; // dir 就是从 x 换到 y 的「变化量」
                reroot(y, x);
            }
        }
    }
    reroot(0, -1);
    return ans;
};
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$。
- 空间复杂度：$\mathcal{O}(n)$。

## 和本题非常像的题目

- [2581. 统计可能的树根数目](https://leetcode.cn/problems/count-number-of-possible-root-nodes/)

## 本地原创解析

### 1. 题意重述

本题来自 `十二、树形 DP / §12.4 换根 DP`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `十二、树形 DP / §12.4 换根 DP`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
