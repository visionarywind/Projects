# 3331. 修改后子树的大小

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/find-subtree-sizes-after-changes/
- 题目 slug：`find-subtree-sizes-after-changes`
- 来源专题：链表、树与回溯
- 来源分类路径：三、一般树 / §3.4 有递有归
- 难度分：2046
- 外部题解来源：https://leetcode.cn/problems/find-subtree-sizes-after-changes/solutions/2966800/liang-ci-dfszi-ding-xiang-xia-zi-di-xian-k4zj/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：两次 DFS / 一次 DFS（Python/Java/C++/Go）](https://leetcode.cn/problems/find-subtree-sizes-after-changes/solutions/2966800/liang-ci-dfszi-ding-xiang-xia-zi-di-xian-k4zj/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`liang-ci-dfszi-ding-xiang-xia-zi-di-xian-k4zj`
- topic id：`2966800`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:49:15 +0800

## 方法一：两次 DFS

不了解递归的同学请先看[【基础算法精讲 09】](https://www.bilibili.com/video/BV1UD4y1Y769/)。

### 第一次 DFS

写一个自顶向下的 DFS，同时维护路径上的每个字母对应的最深节点 $\textit{ancestor}$，初始化成一个全为 $-1$ 的数组（或者哈希表）。递归到节点 $x$ 时，更新 $\textit{ancestor}[s[x]]=x$。

例如示例 1，递归到节点 $1$ 时，有 $\textit{ancestor}[\texttt{a}]=0,\ \textit{ancestor}[\texttt{b}]=1$。

对于节点 $x$ 及其儿子 $y$，如果 $\textit{ancestor}[s[y]]\ne -1$，则把 $y$ 加到 $\textit{ancestor}[s[y]]$ 的子节点列表中，同时把 $x$ 的儿子 $y$ 改成 $-1$，在第二次 DFS 中，不去递归等于 $-1$ 的儿子。

注意要写 [回溯](https://www.bilibili.com/video/BV1mG4y1A7Gu/)，递归返回前，要恢复 $\textit{ancestor}[s[x]]$ 的原始值（递归到 $x$ 之前的值）。

### 第二次 DFS

写一个自底向上的 DFS，返回子树大小。

当前子树大小为 $1$（自己）加上所有儿子子树的大小之和。

⚠**注意**：在遍历列表的同时，往列表中加入数据，会导致我们遍历到新加入列表的数据。可以考虑固定遍历次数为 $x$ 的子节点列表的长度。

> Go 语言不用考虑这个问题。

```py [sol-Python3]
class Solution:
    def findSubtreeSizes(self, parent: List[int], s: str) -> List[int]:
        n = len(parent)
        g = [[] for _ in range(n)]
        for i in range(1, n):
            g[parent[i]].append(i)

        ancestor = defaultdict(lambda: -1)
        def rebuild(x: int) -> None:
            old = ancestor[s[x]]
            ancestor[s[x]] = x
            for i in range(len(g[x])):
                y = g[x][i]
                if (anc := ancestor[s[y]]) != -1:
                    g[anc].append(y)
                    g[x][i] = -1  # -1 表示删除 y
                rebuild(y)
            ancestor[s[x]] = old  # 恢复现场
        rebuild(0)

        size = [1] * n  # 注意这里已经把 1 算进去了
        def dfs(x: int) -> None:
            for y in g[x]:
                if y != -1:  # y 没被删除
                    dfs(y)
                    size[x] += size[y]
        dfs(0)
        return size
```

```java [sol-Java]
class Solution {
    public int[] findSubtreeSizes(int[] parent, String s) {
        int n = parent.length;
        List<Integer>[] g = new ArrayList[n];
        Arrays.setAll(g, i -> new ArrayList<>());
        for (int i = 1; i < n; i++) {
            g[parent[i]].add(i);
        }

        int[] ancestor = new int[26];
        Arrays.fill(ancestor, -1);
        rebuild(0, g, s.toCharArray(), ancestor);

        int[] size = new int[n];
        dfs(0, g, size);
        return size;
    }

    private void rebuild(int x, List<Integer>[] g, char[] s, int[] ancestor) {
        int sx = s[x] - 'a';
        int old = ancestor[sx];
        ancestor[sx] = x;
        for (int i = g[x].size() - 1; i >= 0; i--) {
            int y = g[x].get(i);
            int anc = ancestor[s[y] - 'a'];
            if (anc != -1) {
                g[anc].add(y);
                g[x].set(i, -1); // -1 表示删除 y
            }
            rebuild(y, g, s, ancestor);
        }
        ancestor[sx] = old; // 恢复现场
    }

    private void dfs(int x, List<Integer>[] g, int[] size) {
        size[x] = 1;
        for (int y : g[x]) {
            if (y != -1) { // y 没被删除
                dfs(y, g, size);
                size[x] += size[y];
            }
        }
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> findSubtreeSizes(vector<int>& parent, string s) {
        int n = parent.size();
        vector<vector<int>> g(n);
        for (int i = 1; i < n; i++) {
            g[parent[i]].push_back(i);
        }

        int ancestor[26];
        ranges::fill(ancestor, -1);
        auto rebuild = [&](auto&& rebuild, int x) -> void {
            int sx = s[x] - 'a';
            int old = ancestor[sx];
            ancestor[sx] = x;
            for (int i = g[x].size() - 1; i >= 0; i--) {
                int y = g[x][i];
                int anc = ancestor[s[y] - 'a'];
                if (anc != -1) {
                    g[anc].push_back(y);
                    g[x][i] = -1; // -1 表示删除 y
                }
                rebuild(rebuild, y);
            }
            ancestor[sx] = old; // 恢复现场
        };
        rebuild(rebuild, 0);

        vector<int> size(n, 1); // 注意这里已经把 1 算进去了
        auto dfs = [&](auto&& dfs, int x) -> void {
            for (int y : g[x]) {
                if (y != -1) { // y 没被删除
                    dfs(dfs, y);
                    size[x] += size[y];
                }
            }
        };
        dfs(dfs, 0);
        return size;
    }
};
```

```go [sol-Go]
func findSubtreeSizes(parent []int, s string) []int {
	n := len(parent)
	g := make([][]int, n)
	for i := 1; i < n; i++ {
		p := parent[i]
		g[p] = append(g[p], i)
	}

	ancestor := [26]int{}
	for i := range ancestor {
		ancestor[i] = -1
	}
	var rebuild func(int)
	rebuild = func(x int) {
		sx := s[x] - 'a'
		old := ancestor[sx]
		ancestor[sx] = x
		for i, y := range g[x] {
			if anc := ancestor[s[y]-'a']; anc != -1 {
				g[anc] = append(g[anc], y)
				g[x][i] = -1 // -1 表示删除 y
			}
			rebuild(y)
		}
		ancestor[sx] = old // 恢复现场
	}
	rebuild(0)

	size := make([]int, n)
	var dfs func(int)
	dfs = func(x int) {
		size[x] = 1
		for _, y := range g[x] {
			if y != -1 { // y 没被删除
				dfs(y)
				size[x] += size[y]
			}
		}
	}
	dfs(0)
	return size
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n+|\Sigma|)$，其中 $n$ 是 $s$ 的长度，$|\Sigma|=26$ 是字符集合的大小。
- 空间复杂度：$\mathcal{O}(n)$。

## 方法二：一次 DFS

把两个 DFS 结合起来。在 $\textit{dfs}(y)$ 结束后，分类讨论：

- 如果 $y$ 没有对应的祖先，把 $\textit{size}[x]$ 增加 $\textit{size}[y]$。
- 如果 $y$ 有对应的祖先 $\textit{anc}$，把 $\textit{size}[\textit{anc}]$ 增加 $\textit{size}[y]$。

具体请看 [视频讲解](https://www.bilibili.com/video/BV13J1MYwEGM/)，欢迎点赞关注~

```py [sol-Python3]
class Solution:
    def findSubtreeSizes(self, parent: List[int], s: str) -> List[int]:
        n = len(parent)
        g = [[] for _ in range(n)]
        for i in range(1, n):
            g[parent[i]].append(i)

        size = [1] * n
        ancestor = defaultdict(lambda: -1)
        def dfs(x: int) -> None:
            old = ancestor[s[x]]
            ancestor[s[x]] = x
            for y in g[x]:
                dfs(y)
                anc = ancestor[s[y]]
                size[x if anc < 0 else anc] += size[y]
            ancestor[s[x]] = old  # 恢复现场
        dfs(0)
        return size
```

```java [sol-Java]
class Solution {
    public int[] findSubtreeSizes(int[] parent, String s) {
        int n = parent.length;
        List<Integer>[] g = new ArrayList[n];
        Arrays.setAll(g, i -> new ArrayList<>());
        for (int i = 1; i < n; i++) {
            g[parent[i]].add(i);
        }

        int[] size = new int[n];
        int[] ancestor = new int[26];
        Arrays.fill(ancestor, -1);
        dfs(0, g, s.toCharArray(), ancestor, size);
        return size;
    }

    private void dfs(int x, List<Integer>[] g, char[] s, int[] ancestor, int[] size) {
        size[x] = 1;
        int sx = s[x] - 'a';
        int old = ancestor[sx];
        ancestor[sx] = x;
        for (int y : g[x]) {
            dfs(y, g, s, ancestor, size);
            int anc = ancestor[s[y] - 'a'];
            size[anc < 0 ? x : anc] += size[y];
        }
        ancestor[sx] = old; // 恢复现场
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> findSubtreeSizes(vector<int>& parent, string s) {
        int n = parent.size();
        vector<vector<int>> g(n);
        for (int i = 1; i < n; i++) {
            g[parent[i]].push_back(i);
        }

        vector<int> size(n, 1);
        int ancestor[26];
        ranges::fill(ancestor, -1);
        auto dfs = [&](auto&& dfs, int x) -> void {
            int sx = s[x] - 'a';
            int old = ancestor[sx];
            ancestor[sx] = x;
            for (int y : g[x]) {
                dfs(dfs, y);
                int anc = ancestor[s[y] - 'a'];
                size[anc < 0 ? x : anc] += size[y];
            }
            ancestor[sx] = old; // 恢复现场
        };
        dfs(dfs, 0);
        return size;
    }
};
```

```go [sol-Go]
func findSubtreeSizes(parent []int, s string) []int {
	n := len(parent)
	g := make([][]int, n)
	for i := 1; i < n; i++ {
		p := parent[i]
		g[p] = append(g[p], i)
	}

	size := make([]int, n)
	ancestor := [26]int{}
	for i := range ancestor {
		ancestor[i] = -1
	}
	var dfs func(int)
	dfs = func(x int) {
		size[x] = 1
		sx := s[x] - 'a'
		old := ancestor[sx]
		ancestor[sx] = x
		for _, y := range g[x] {
			dfs(y)
			anc := ancestor[s[y]-'a']
			if anc < 0 {
				anc = x
			}
			size[anc] += size[y]
		}
		ancestor[sx] = old // 恢复现场
	}
	dfs(0)
	return size
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n+|\Sigma|)$，其中 $n$ 是 $s$ 的长度，$|\Sigma|=26$ 是字符集合的大小。
- 空间复杂度：$\mathcal{O}(n)$。

更多相似题目，见 [链表、二叉树与一般树](https://leetcode.cn/circle/discuss/K0n2gO/) 中的「**§2.2 自顶向下 DFS**」和「**§2.3 自底向上 DFS**」。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与一般树（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

## 本地原创解析

### 1. 题意重述

本题来自 `三、一般树 / §3.4 有递有归`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `三、一般树 / §3.4 有递有归`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
