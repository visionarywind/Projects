# 3615. 图中的最长回文路径

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/longest-palindromic-path-in-graph/
- 题目 slug：`longest-palindromic-path-in-graph`
- 来源专题：字符串
- 来源分类路径：三、Manacher 算法（回文串）
- 难度分：2463
- 外部题解来源：https://leetcode.cn/problems/longest-palindromic-path-in-graph/solutions/3722469/zhong-xin-kuo-zhan-fa-zhuang-ya-dp-by-en-ai9s/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[中心扩展法 + 状压 DP + 优化（Python/Java/C++/Go）](https://leetcode.cn/problems/longest-palindromic-path-in-graph/solutions/3722469/zhong-xin-kuo-zhan-fa-zhuang-ya-dp-by-en-ai9s/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`zhong-xin-kuo-zhan-fa-zhuang-ya-dp-by-en-ai9s`
- topic id：`3722469`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 12:05:27 +0800

直接从左到右暴搜回文路径？写记忆化？不幸的是，记忆化需要保存后续路径的完整顺序信息（因为要判断是否回文）。考虑完全图，本质在枚举所有排列，有 $\mathcal{O}(n!)$ 条不同的路径，这太多了。

**核心思路**：枚举回文中心，从中心向左右两边扩展（中心扩展法）。

设访问过的节点集合为 $S$。对于已经访问过的点，其访问顺序是不重要的，我们只需要知道访问过哪些点。比如回文路径的左半边扩展到节点 $3$ 时，怎么来的不重要，无论是 $1\to 2 \to 3$ 还是 $2\to 1 \to 3$，都等同于回文路径的左半边已经包含 $1,2,3$ 这三个节点了，且左半边当前在节点 $3$（重叠子问题）。这样就无需暴力枚举节点访问顺序的排列了，只需维护节点无序集合的信息，可以用状压 DP 解决。

我们需要知道三个信息：

- 路径的左右端点 $x$ 和 $y$。
- 已经访问过的节点（包括 $x$ 和 $y$）集合 $S$。

定义 $\textit{dfs}(x,y,S)$ 表示在路径的左右端点为 $x$ 和 $y$，访问过的节点集合为 $S$ 的情况下，从 $x$ 和 $y$ 向两侧扩展，最多还能访问多少个节点（不算 $x$ 和 $y$）。

枚举 $x$ 的邻居 $v$，枚举 $y$ 的邻居 $w$，如果 $v$ 和 $w$ 都没访问过，且 $v\ne w$ 且 $\textit{label}[v] = \textit{label}[w]$，那么可以扩展，问题变成在路径的左右端点为 $v$ 和 $w$，访问过的节点集合为 $S \cup \{v,w\}$ 的情况下，从 $v$ 和 $w$ 向两侧扩展，最多还能访问多少个节点，即 $\textit{dfs}(v,w, S \cup \{v,w\})$，用该返回值加二（加上 $v$ 和 $w$），更新 $\textit{dfs}(x,y,S)$ 的返回值的最大值，即

$$
\textit{dfs}(x,y,S) = \max_{v,w} \textit{dfs}(v,w, S \cup \{v,w\}) + 2
$$

其中 $v$ 是 $x$ 的邻居，$w$ 是 $y$ 的邻居，$v\notin S$，$w\notin S$，$v\ne w$，$\textit{label}[v] = \textit{label}[w]$。

**递归边界**：无需判断。

**递归入口**：

- 奇回文串：$\textit{dfs}(x,x,\{x\})+1$。
- 偶回文串：$\textit{dfs}(x,y,\{x,y\})+2$。其中 $x$ 和 $y$ 是邻居且 $\textit{label}[x] = \textit{label}[y]$。

代码实现时，用二进制表示集合，用位运算实现集合操作，具体请看 [从集合论到位运算，常见位运算技巧分类总结](https://leetcode.cn/circle/discuss/CaOJ45/)。

**优化 1**：我们计算的是从 $x$ 和 $y$ 出发继续扩展的节点个数，根据对称性，$\textit{dfs}(x,y,S)$ 计算出的结果和 $\textit{dfs}(y,x,S)$ 计算出的结果是一样的，没必要算两次。所以递归时，可以人为规定递归参数必须满足 $x\le y$，从而减少状态个数和计算量。

**优化 2**：特判完全图的情况，此时路径可以是任意节点的排列，问题等价于重排 $\textit{label}$ 中的字母可以得到的最长回文串。比如 $3$ 个 $\texttt{a}$ 和 $5$ 个 $\texttt{b}$，可以选 $2$ 个 $\texttt{a}$ 一左一右，$4$ 个 $\texttt{b}$ 左右各放 $2$ 个，多出的字母只能选一个放正中间。比如回文串为 $\texttt{abbabba}$。（谢谢 [@观铃 🔔](/u/kamio_misuzu) 补充）

**优化 3**：递归结束后，如果 $\textit{ans} = n$，可以直接返回 $n$。进一步地，利用优化 2 的想法，计算出理论最大值，如果 $\textit{ans}$ 等于理论最大值，返回 $\textit{ans}$。

具体请看 [视频讲解](https://www.bilibili.com/video/BV1xSuFzHEa1/?t=28m51s)，欢迎点赞关注~

```py [sol-Python3]
# 手写 max 更快
max = lambda a, b: b if b > a else a

class Solution:
    def maxLen(self, n: int, edges: List[List[int]], label: str) -> int:
        # 计算理论最大值
        odd = sum(c % 2 for c in Counter(label).values()) 
        theoretical_max = n - max(odd - 1, 0)  # 奇数选一个放正中心，其余全弃

        if len(edges) == n * (n - 1) // 2:  # 完全图，可以达到理论最大值
            return theoretical_max

        g = [[] for _ in range(n)]
        for x, y in edges:
            g[x].append(y)
            g[y].append(x)

        # 计算从 x 和 y 向两侧扩展，最多还能访问多少个节点（不算 x 和 y）
        @cache
        def dfs(x: int, y: int, vis: int) -> int:
            res = 0
            for v in g[x]:
                if vis >> v & 1:
                    continue
                for w in g[y]:
                    if vis >> w & 1 == 0 and v != w and label[w] == label[v]:
                        tv, tw = v, w  # 注意不能直接交换 v 和 w，否则下个循环的 v 就不是原来的 v 了
                        if tv > tw:  # 保证 tv < tw，减少状态个数和计算量
                            tv, tw = tw, tv
                        res = max(res, dfs(tv, tw, vis | 1 << v | 1 << w) + 2)
            return res

        ans = 0
        for x, to in enumerate(g):
            # 奇回文串，x 作为回文中心
            ans = max(ans, dfs(x, x, 1 << x) + 1)
            if ans == theoretical_max:
                return ans
            # 偶回文串，x 和 x 的邻居 y 作为回文中心
            for y in to:
                # 保证递归参数 x < y，减少状态个数和计算量
                if x < y and label[x] == label[y]:
                    ans = max(ans, dfs(x, y, 1 << x | 1 << y) + 2)
                    if ans == theoretical_max:
                        return ans
        return ans
```

```java [sol-Java]
class Solution {
    public int maxLen(int n, int[][] edges, String label) {
        char[] s = label.toCharArray();
        // 计算理论最大值
        int[] cnt = new int[26];
        for (char ch : s) {
            cnt[ch - 'a']++;
        }
        int odd = 0;
        for (int c : cnt) {
            odd += c % 2;
        }
        int theoreticalMax = n - Math.max(odd - 1, 0); // 奇数选一个放正中心，其余全弃

        if (edges.length == n * (n - 1) / 2) { // 完全图，可以达到理论最大值
            return theoreticalMax;
        }

        List<Integer>[] g = new ArrayList[n];
        Arrays.setAll(g, _ -> new ArrayList<>());
        for (int[] e : edges) {
            int x = e[0];
            int y = e[1];
            g[x].add(y);
            g[y].add(x);
        }

        int[][][] memo = new int[n][n][1 << n];
        for (int[][] mat : memo) {
            for (int[] row : mat) {
                Arrays.fill(row, -1);
            }
        }

        int ans = 0;
        for (int x = 0; x < n; x++) {
            // 奇回文串，x 作为回文中心
            ans = Math.max(ans, dfs(x, x, 1 << x, g, s, memo) + 1);
            if (ans == theoreticalMax) {
                return ans;
            }
            // 偶回文串，x 和 x 的邻居 y 作为回文中心
            for (int y : g[x]) {
                // 保证 x < y，减少状态个数和计算量
                if (x < y && s[x] == s[y]) {
                    ans = Math.max(ans, dfs(x, y, 1 << x | 1 << y, g, s, memo) + 2);
                    if (ans == theoreticalMax) {
                        return ans;
                    }
                }
            }
        }
        return ans;
    }

    // 计算从 x 和 y 向两侧扩展，最多还能访问多少个节点（不算 x 和 y）
    private int dfs(int x, int y, int vis, List<Integer>[] g, char[] label, int[][][] memo) {
        if (memo[x][y][vis] >= 0) { // 之前计算过
            return memo[x][y][vis];
        }
        int res = 0;
        for (int v : g[x]) {
            if ((vis >> v & 1) > 0) { // v 在路径中
                continue;
            }
            for (int w : g[y]) {
                if ((vis >> w & 1) == 0 && w != v && label[w] == label[v]) {
                    // 保证 v < w，减少状态个数和计算量
                    int r = dfs(Math.min(v, w), Math.max(v, w), vis | 1 << v | 1 << w, g, label, memo);
                    res = Math.max(res, r + 2);
                }
            }
        }
        return memo[x][y][vis] = res; // 记忆化
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxLen(int n, vector<vector<int>>& edges, string label) {
        // 计算理论最大值
        int cnt[26]{};
        for (char ch : label) {
            cnt[ch - 'a']++;
        }
        int odd = 0;
        for (int c : cnt) {
            odd += c % 2;
        }
        int theoretical_max = n - max(odd - 1, 0); // 奇数选一个放正中心，其余全弃

        if (edges.size() == n * (n - 1) / 2) { // 完全图，可以达到理论最大值
            return theoretical_max;
        }

        vector<vector<int>> g(n);
        for (auto& e : edges) {
            int x = e[0], y = e[1];
            g[x].push_back(y);
            g[y].push_back(x);
        }

        vector memo(n, vector(n, vector<int>(1 << n, -1)));
        // 计算从 x 和 y 向两侧扩展，最多还能访问多少个节点（不算 x 和 y）
        auto dfs = [&](this auto&& dfs, int x, int y, int vis) -> int {
            int& res = memo[x][y][vis]; // 注意这里是引用
            if (res >= 0) { // 之前计算过
                return res;
            }
            res = 0;
            for (int v : g[x]) {
                if (vis >> v & 1) { // v 在路径中
                    continue;
                }
                for (int w : g[y]) {
                    if ((vis >> w & 1) == 0 && w != v && label[w] == label[v]) {
                        // 保证 v < w，减少状态个数和计算量
                        int r = dfs(min(v, w), max(v, w), vis | 1 << v | 1 << w);
                        res = max(res, r + 2);
                    }
                }
            }
            return res;
        };

        int ans = 0;
        for (int x = 0; x < n; x++) {
            // 奇回文串，x 作为回文中心
            ans = max(ans, dfs(x, x, 1 << x) + 1);
            if (ans == theoretical_max) {
                return ans;
            }
            // 偶回文串，x 和 x 的邻居 y 作为回文中心
            for (int y : g[x]) {
                // 保证 x < y，减少状态个数和计算量
                if (x < y && label[x] == label[y]) {
                    ans = max(ans, dfs(x, y, 1 << x | 1 << y) + 2);
                    if (ans == theoretical_max) {
                        return ans;
                    }
                }
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func maxLen(n int, edges [][]int, label string) (ans int) {
	// 计算理论最大值
	cnt := [26]int{}
	for _, ch := range label {
		cnt[ch-'a']++
	}
	odd := 0
	for _, c := range cnt {
		odd += c % 2
	}
	theoreticalMax := n - max(odd-1, 0) // 奇数选一个放正中心，其余全弃

	if len(edges) == n*(n-1)/2 { // 完全图，可以达到理论最大值
		return theoreticalMax
	}

	g := make([][]int, n)
	for _, e := range edges {
		x, y := e[0], e[1]
		g[x] = append(g[x], y)
		g[y] = append(g[y], x)
	}

	memo := make([][][]int, n)
	for i := range memo {
		memo[i] = make([][]int, n)
		for j := range memo[i] {
			memo[i][j] = make([]int, 1<<n)
			for p := range memo[i][j] {
				memo[i][j][p] = -1
			}
		}
	}

	// 计算从 x 和 y 向两侧扩展，最多还能访问多少个节点（不算 x 和 y）
	var dfs func(int, int, int) int
	dfs = func(x, y, vis int) (res int) {
		p := &memo[x][y][vis]
		if *p >= 0 { // 之前计算过
			return *p
		}
		for _, v := range g[x] {
			if vis>>v&1 > 0 { // v 在路径中
				continue
			}
			for _, w := range g[y] {
				if vis>>w&1 == 0 && w != v && label[w] == label[v] {
					// 保证 v < w，减少状态个数和计算量
					r := dfs(min(v, w), max(v, w), vis|1<<v|1<<w)
					res = max(res, r+2)
				}
			}
		}
		*p = res // 记忆化
		return
	}

	for x, to := range g {
		// 奇回文串，x 作为回文中心
		ans = max(ans, dfs(x, x, 1<<x)+1)
		if ans == theoreticalMax {
			return
		}
		// 偶回文串，x 和 x 的邻居 y 作为回文中心
		for _, y := range to {
			// 保证 x < y，减少状态个数和计算量
			if x < y && label[x] == label[y] {
				ans = max(ans, dfs(x, y, 1<<x|1<<y)+2)
				if ans == theoreticalMax {
					return
				}
			}
		}
	}
	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^4 2^n)$。由于每个状态只会计算一次，动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题状态个数等于 $\mathcal{O}(n^2 2^n)$，最坏情况下（完全图）单个状态的计算时间为 $\mathcal{O}(n^2)$，所以总的时间复杂度为 $\mathcal{O}(n^4 2^n)$。
- 空间复杂度：$\mathcal{O}(n^2 2^n)$。保存多少状态，就需要多少空间。

## 专题训练

见下面动态规划题单的「**§9.2 排列型 ② 相邻相关**」。

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

本题来自 `三、Manacher 算法（回文串）`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `三、Manacher 算法（回文串）`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
