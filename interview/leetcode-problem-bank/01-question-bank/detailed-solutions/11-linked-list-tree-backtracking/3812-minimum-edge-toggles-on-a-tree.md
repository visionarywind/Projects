# 3812. 翻转树上最少边

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-edge-toggles-on-a-tree/
- 题目 slug：`minimum-edge-toggles-on-a-tree`
- 来源专题：链表、树与回溯
- 来源分类路径：三、一般树 / §3.3 自底向上 DFS
- 难度分：2179
- 外部题解来源：https://leetcode.cn/problems/minimum-edge-toggles-on-a-tree/solutions/3883163/dfspythonjavacgo-by-endlesscheng-5m1i/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[树 DFS，O(n) 优化（Python/Java/C++/Go）](https://leetcode.cn/problems/minimum-edge-toggles-on-a-tree/solutions/3883163/dfspythonjavacgo-by-endlesscheng-5m1i/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`dfspythonjavacgo-by-endlesscheng-5m1i`
- topic id：`3883163`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:49:15 +0800

从这棵树的叶子开始思考。注意只有一条边与叶子相连。

- 如果叶子 $x$ 的 $\textit{start}[x]\ne \textit{target}[x]$，那么与 $x$ 相连的这条边，一定要翻转，从而让 $\textit{start}[x] = \textit{target}[x]$。如果不翻转，那么 $\textit{start}[x]\ne \textit{target}[x]$，无法满足题目要求。
- 如果叶子 $x$ 的 $\textit{start}[x] = \textit{target}[x]$，已经满足要求，那么与 $x$ 相连的这条边，一定不能翻转。

处理完所有叶子（以及连接叶子的边）后，我们把叶子去掉（剥洋葱），得到了一个规模更小的子问题。最终我们会把所有边都去掉。

按照这个流程，**哪些边要翻转，哪些边不能翻转，是完全确定的**。

如何实现这个流程？可以写拓扑排序，也可以写自底向上的 DFS，都可以实现「先处理叶子，再处理更上层节点」的目标。

[本题视频讲解](https://www.bilibili.com/video/BV1MVkxBZE4D/?t=24m17s)，欢迎点赞关注~

## 写法一

```py [sol-Python3]
class Solution:
    def minimumFlips(self, n: int, edges: List[List[int]], start: str, target: str) -> List[int]:
        g = [[] for _ in range(n)]
        for i, (x, y) in enumerate(edges):
            g[x].append((y, i))
            g[y].append((x, i))

        ans = []

        # 返回是否需要翻转 x-fa 这条边
        def dfs(x: int, fa: int) -> bool:
            rev = start[x] != target[x]  # x-fa 是否要翻转
            for y, i in g[x]:
                if y != fa and dfs(y, x):
                    ans.append(i)  # 需要翻转 y-x
                    rev = not rev  # x 被翻转了
            return rev

        if dfs(0, -1):  # 只剩下一个根节点需要翻转，无法操作
            return [-1]

        ans.sort()
        return ans
```

```java [sol-Java]
class Solution {
    public List<Integer> minimumFlips(int n, int[][] edges, String start, String target) {
        List<int[]>[] g = new ArrayList[n];
        Arrays.setAll(g, _ -> new ArrayList<>());
        for (int i = 0; i < edges.length; i++) {
            int x = edges[i][0];
            int y = edges[i][1];
            g[x].add(new int[]{y, i});
            g[y].add(new int[]{x, i});
        }

        char[] s = start.toCharArray();
        char[] t = target.toCharArray();
        List<Integer> ans = new ArrayList<>();

        if (dfs(0, -1, g, s, t, ans)) { // 只剩下一个根节点需要翻转，无法操作
            return List.of(-1);
        }

        Collections.sort(ans);
        return ans;
    }

    // 返回是否需要翻转 x-fa 这条边
    private boolean dfs(int x, int fa, List<int[]>[] g, char[] s, char[] t, List<Integer> ans) {
        boolean rev = s[x] != t[x]; // x-fa 是否要翻转
        for (int[] e : g[x]) {
            int y = e[0];
            if (y != fa && dfs(y, x, g, s, t, ans)) {
                ans.add(e[1]); // 需要翻转 y-x
                rev = !rev; // x 被翻转了
            }
        }
        return rev;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> minimumFlips(int n, vector<vector<int>>& edges, string start, string target) {
        vector<vector<pair<int, int>>> g(n);
        for (int i = 0; i < edges.size(); i++) {
            int x = edges[i][0], y = edges[i][1];
            g[x].emplace_back(y, i);
            g[y].emplace_back(x, i);
        }

        vector<int> ans;

        // 返回是否需要翻转 x-fa 这条边
        auto dfs = [&](this auto&& dfs, int x, int fa) -> bool {
            bool rev = start[x] != target[x]; // x-fa 是否要翻转
            for (auto& [y, i] : g[x]) {
                if (y != fa && dfs(y, x)) {
                    ans.push_back(i); // 需要翻转 y-x
                    rev = !rev; // x 被翻转了
                }
            }
            return rev;
        };

        if (dfs(0, -1)) { // 只剩下一个根节点需要翻转，无法操作
            return {-1};
        }

        ranges::sort(ans);
        return ans;
    }
};
```

```go [sol-Go]
func minimumFlips(n int, edges [][]int, start, target string) (ans []int) {
	type edge struct{ to, i int }
	g := make([][]edge, n)
	for i, e := range edges {
		x, y := e[0], e[1]
		g[x] = append(g[x], edge{y, i})
		g[y] = append(g[y], edge{x, i})
	}

	// 返回是否需要翻转 x-fa 这条边
	var dfs func(int, int) bool
	dfs = func(x, fa int) bool {
		rev := start[x] != target[x] // x-fa 是否要翻转
		for _, e := range g[x] {
			y := e.to
			if y != fa && dfs(y, x) {
				ans = append(ans, e.i) // 需要翻转 y-x
				rev = !rev // x 被翻转了
			}
		}
		return rev
	}

	if dfs(0, -1) { // 只剩下一个根节点需要翻转，无法操作
		return []int{-1}
	}

	slices.Sort(ans)
	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n)$。瓶颈在排序上。
- 空间复杂度：$\mathcal{O}(n)$。

## 写法二

用一个布尔数组标记哪些边需要反转，然后从左到右遍历布尔数组，把标记的下标放入答案。这样可以避免排序。

```py [sol-Python3]
class Solution:
    def minimumFlips(self, n: int, edges: List[List[int]], start: str, target: str) -> List[int]:
        g = [[] for _ in range(n)]
        for i, (x, y) in enumerate(edges):
            g[x].append((y, i))
            g[y].append((x, i))

        revs = [False] * (n - 1)
        # 返回是否需要翻转 x-fa 这条边
        def dfs(x: int, fa: int) -> bool:
            rev = start[x] != target[x]  # x-fa 是否要翻转
            for y, i in g[x]:
                if y != fa and dfs(y, x):
                    revs[i] = True  # 需要翻转 y-x
                    rev = not rev  # x 被翻转了
            return rev

        if dfs(0, -1):  # 只剩下一个根节点需要翻转，无法操作
            return [-1]

        return [i for i, rev in enumerate(revs) if rev]
```

```java [sol-Java]
class Solution {
    public List<Integer> minimumFlips(int n, int[][] edges, String start, String target) {
        List<int[]>[] g = new ArrayList[n];
        Arrays.setAll(g, _ -> new ArrayList<>());
        for (int i = 0; i < edges.length; i++) {
            int x = edges[i][0];
            int y = edges[i][1];
            g[x].add(new int[]{y, i});
            g[y].add(new int[]{x, i});
        }

        char[] s = start.toCharArray();
        char[] t = target.toCharArray();
        boolean[] revs = new boolean[n - 1];
        if (dfs(0, -1, g, s, t, revs)) { // 只剩下一个根节点需要翻转，无法操作
            return List.of(-1);
        }

        List<Integer> ans = new ArrayList<>();
        for (int i = 0; i < n - 1; i++) {
            if (revs[i]) {
                ans.add(i);
            }
        }
        return ans;
    }

    // 返回是否需要翻转 x-fa 这条边
    private boolean dfs(int x, int fa, List<int[]>[] g, char[] s, char[] t, boolean[] revs) {
        boolean rev = s[x] != t[x]; // x-fa 是否要翻转
        for (int[] e : g[x]) {
            int y = e[0];
            if (y != fa && dfs(y, x, g, s, t, revs)) {
                revs[e[1]] = true; // 需要翻转 y-x
                rev = !rev; // x 被翻转了
            }
        }
        return rev;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> minimumFlips(int n, vector<vector<int>>& edges, string start, string target) {
        vector<vector<pair<int, int>>> g(n);
        for (int i = 0; i < edges.size(); i++) {
            int x = edges[i][0], y = edges[i][1];
            g[x].emplace_back(y, i);
            g[y].emplace_back(x, i);
        }

        vector<int8_t> revs(n - 1);
        // 返回是否需要翻转 x-fa 这条边
        auto dfs = [&](this auto&& dfs, int x, int fa) -> bool {
            bool rev = start[x] != target[x]; // x-fa 是否要翻转
            for (auto& [y, i] : g[x]) {
                if (y != fa && dfs(y, x)) {
                    revs[i] = true; // 需要翻转 y-x
                    rev = !rev; // x 被翻转了
                }
            }
            return rev;
        };

        if (dfs(0, -1)) { // 只剩下一个根节点需要翻转，无法操作
            return {-1};
        }

        vector<int> ans;
        for (int i = 0; i < n - 1; i++) {
            if (revs[i]) {
                ans.push_back(i);
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func minimumFlips(n int, edges [][]int, start, target string) (ans []int) {
	type edge struct{ to, i int }
	g := make([][]edge, n)
	for i, e := range edges {
		x, y := e[0], e[1]
		g[x] = append(g[x], edge{y, i})
		g[y] = append(g[y], edge{x, i})
	}

	revs := make([]bool, n-1)
	// 返回是否需要翻转 x-fa 这条边
	var dfs func(int, int) bool
	dfs = func(x, fa int) bool {
		rev := start[x] != target[x] // x-fa 是否要翻转
		for _, e := range g[x] {
			y := e.to
			if y != fa && dfs(y, x) {
				revs[e.i] = true // 需要翻转 y-x
				rev = !rev // x 被翻转了
			}
		}
		return rev
	}

	if dfs(0, -1) { // 只剩下一个根节点需要翻转，无法操作
		return []int{-1}
	}

	for i, rev := range revs {
		if rev {
			ans = append(ans, i)
		}
	}
	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$。
- 空间复杂度：$\mathcal{O}(n)$。

## 专题训练

见下面树题单的「**§3.3 自底向上 DFS**」和「**§3.6 树的拓扑排序**」。

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

本题来自 `三、一般树 / §3.3 自底向上 DFS`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `三、一般树 / §3.3 自底向上 DFS`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
