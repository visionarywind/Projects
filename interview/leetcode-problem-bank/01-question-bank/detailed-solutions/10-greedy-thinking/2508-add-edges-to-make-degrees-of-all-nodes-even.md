# 2508. 添加边使所有节点度数都为偶数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/add-edges-to-make-degrees-of-all-nodes-even/
- 题目 slug：`add-edges-to-make-degrees-of-all-nodes-even`
- 来源专题：贪心与思维
- 来源分类路径：五、思维题 / §5.8 分类讨论
- 难度分：2060
- 外部题解来源：https://leetcode.cn/problems/add-edges-to-make-degrees-of-all-nodes-even/solutions/2024528/fen-lei-tao-lun-by-endlesscheng-z71j/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[分类讨论（Python/Java/C++/Go）](https://leetcode.cn/problems/add-edges-to-make-degrees-of-all-nodes-even/solutions/2024528/fen-lei-tao-lun-by-endlesscheng-z71j/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`fen-lei-tao-lun-by-endlesscheng-z71j`
- topic id：`2024528`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:13:14 +0800

[视频讲解](https://www.bilibili.com/video/BV1LW4y1T7if/) 已出炉，欢迎点赞三连，在评论区分享你对这场周赛的看法~

---

把度数为奇数的节点记到 $\textit{odd}$ 中，记 $m$ 为 $\textit{odd}$ 的长度，分类讨论：

- 如果 $m=0$，那么已经符合要求。
- 如果 $m=2$，记 $x=\textit{odd}[0],y=\textit{odd}[1]$：
    - 如果 $x$ 和 $y$ 之间没有边，那么连边之后就符合要求了。
    - 如果 $x$ 和 $y$ 之间有边，那么枚举 $[1,n]$ 的所有不为 $x$ 和 $y$ 的点 $i$，由于 $i$ 的度数一定是偶数，如果 $i$ 和 $x$ 以及 $i$ 和 $y$ 之间没有边，那么连边之后就符合要求了。
- 如果 $m=4$，记 $a=\textit{odd}[0],b=\textit{odd}[1],c=\textit{odd}[2],d=\textit{odd}[3]$：
    - 如果 $a$ 和 $b$ 以及 $c$ 和 $d$ 之间没有边，那么连边之后就符合要求了。
    - 如果 $a$ 和 $c$ 以及 $b$ 和 $d$ 之间没有边，那么连边之后就符合要求了。
    - 如果 $a$ 和 $d$ 以及 $b$ 和 $c$ 之间没有边，那么连边之后就符合要求了。
- 其余情况无法满足要求。

```py [sol1-Python3]
class Solution:
    def isPossible(self, n: int, edges: List[List[int]]) -> bool:
        g = defaultdict(set)
        for x, y in edges:
            g[x].add(y)
            g[y].add(x)
        odd = [i for i, nb in g.items() if len(nb) % 2]
        m = len(odd)
        if m == 0: return True
        if m == 2:
            x, y = odd
            return x not in g[y] or any(
                i != x and i != y and x not in g[i] and y not in g[i]
                for i in range(1, n + 1))
        if m == 4:
            a, b, c, d = odd
            return b not in g[a] and d not in g[c] or \
                   c not in g[a] and d not in g[b] or \
                   d not in g[a] and c not in g[b]
        return False
```

```java [sol1-Java]
class Solution {
    public boolean isPossible(int n, List<List<Integer>> edges) {
        var g = new Set[n + 1];
        Arrays.setAll(g, e -> new HashSet<Integer>());
        for (var e : edges) {
            int x = e.get(0), y = e.get(1);
            g[x].add(y);
            g[y].add(x);
        }
        var odd = new ArrayList<Integer>();
        for (var i = 1; i <= n; ++i)
            if (g[i].size() % 2 > 0) odd.add(i);
        var m = odd.size();
        if (m == 0) return true;
        if (m == 2) {
            int x = odd.get(0), y = odd.get(1);
            if (!g[x].contains(y)) return true;
            for (var i = 1; i <= n; ++i)
                if (i != x && i != y && !g[i].contains(x) && !g[i].contains(y))
                    return true;
            return false;
        }
        if (m == 4) {
            int a = odd.get(0), b = odd.get(1), c = odd.get(2), d = odd.get(3);
            return !g[a].contains(b) && !g[c].contains(d) ||
                    !g[a].contains(c) && !g[b].contains(d) ||
                    !g[a].contains(d) && !g[b].contains(c);
        }
        return false;
    }
}
```

```cpp [sol1-C++]
class Solution {
public:
    bool isPossible(int n, vector<vector<int>> &edges) {
        unordered_set<int> g[n + 1];
        for (auto &e : edges) {
            int x = e[0], y = e[1];
            g[x].insert(y);
            g[y].insert(x);
        }
        vector<int> odd;
        for (int i = 1; i <= n; ++i)
            if (g[i].size() % 2) odd.push_back(i);
        int m = odd.size();
        if (m == 0) return true;
        if (m == 2) {
            int x = odd[0], y = odd[1];
            if (!g[x].count(y)) return true;
            for (int i = 1; i <= n; ++i)
                if (i != x && i != y && !g[i].count(x) && !g[i].count(y))
                    return true;
            return false;
        }
        if (m == 4) {
            int a = odd[0], b = odd[1], c = odd[2], d = odd[3];
            return !g[a].count(b) && !g[c].count(d) ||
                   !g[a].count(c) && !g[b].count(d) ||
                   !g[a].count(d) && !g[b].count(c);
        }
        return false;
    }
};
```

```go [sol1-Go]
func isPossible(n int, edges [][]int) bool {
	g := map[int]map[int]bool{}
	for _, e := range edges {
		x, y := e[0], e[1]
		if g[x] == nil {
			g[x] = map[int]bool{}
		}
		g[x][y] = true
		if g[y] == nil {
			g[y] = map[int]bool{}
		}
		g[y][x] = true
	}
	odd := []int{}
	for i, nb := range g {
		if len(nb)%2 > 0 {
			odd = append(odd, i)
		}
	}
	m := len(odd)
	if m == 0 {
		return true
	}
	if m == 2 {
		x, y := odd[0], odd[1]
		if !g[x][y] {
			return true
		}
		for i := 1; i <= n; i++ {
			if i != x && i != y && !g[i][x] && !g[i][y] {
				return true
			}
		}
		return false
	}
	if m == 4 {
		a, b, c, d := odd[0], odd[1], odd[2], odd[3]
		return !g[a][b] && !g[c][d] || !g[a][c] && !g[b][d] || !g[a][d] && !g[b][c]
	}
	return false
}
```

#### 复杂度分析

- 时间复杂度：$O(n+m)$，其中 $m$ 为 $\textit{edges}$ 的长度。
- 空间复杂度：$O(n+m)$。

## 本地原创解析

### 1. 题意重述

本题来自 `五、思维题 / §5.8 分类讨论`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `五、思维题 / §5.8 分类讨论`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
