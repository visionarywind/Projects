# 3887. 增量偶权环查询

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/incremental-even-weighted-cycle-queries/
- 题目 slug：`incremental-even-weighted-cycle-queries`
- 来源专题：常用数据结构
- 来源分类路径：七、并查集 / §7.6 带权并查集（边权并查集）
- 难度分：2128
- 外部题解来源：https://leetcode.cn/problems/incremental-even-weighted-cycle-queries/solutions/3939570/dai-quan-bing-cha-ji-pythonjavacgo-by-en-3ka2/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[带权并查集（Python/Java/C++/Go）](https://leetcode.cn/problems/incremental-even-weighted-cycle-queries/solutions/3939570/dai-quan-bing-cha-ji-pythonjavacgo-by-en-3ka2/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`dai-quan-bing-cha-ji-pythonjavacgo-by-en-3ka2`
- topic id：`3939570`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

在边权只有 $0$ 和 $1$ 的情况下，环的边权和是偶数，等价于有偶数个 $1$，等价于边权的**异或和**等于 $0$。

设 $x$ 和 $y$ 是某个环上的两个节点，我们可以把这个环拆分成从 $x$ 到 $y$ 的两条不同的简单路径。

由于 $s\oplus s = 0$，所以**当且仅当两条路径的异或和相同，环的边权和是偶数**。

如何验证是否满足要求？

- 如果添加的边连接了图的两个连通块，那么没有形成新的环，直接连边。
- 如果添加的边连接了同一个连通块中的两个点 $x$ 和 $y$，那么当且仅当添加的边的边权等于从 $x$ 到 $y$ 的路径的异或和（根据上面的结论，路径异或和是唯一的），我们才能添加这条边。

如何快速求出从 $x$ 到 $y$ 的路径的异或和？这可以用**带权并查集**维护，具体请看 [本题视频讲解](https://www.bilibili.com/video/BV11UXSB7EGz/)，欢迎点赞关注~

我们可以维护节点到其代表元 $\textit{root}$ 的路径异或和。从 $x$ 到 $y$ 的路径，可以拆分成先从 $x$ 到 $\textit{root}$，再从 $\textit{root}$ 到 $y$（重复走的边权会被异或抵消掉），这两条路径的异或和再计算异或，即为从 $x$ 到 $y$ 的路径异或和。

```py [sol-Python3]
# 完整模板见 https://leetcode.cn/circle/discuss/mOr1u6/
class UnionFind:
    def __init__(self, n: int):
        self.fa = list(range(n))  # fa[x] 是 x 的代表元
        self.dis = [0] * n  # dis[x] = 从 x 到 fa[x] 的路径异或和

    def find(self, x: int) -> int:
        fa = self.fa
        if fa[x] != x:
            root = self.find(fa[x])
            self.dis[x] ^= self.dis[fa[x]]
            fa[x] = root
        return fa[x]

    def merge(self, from_: int, to: int, value: int) -> bool:
        x, y = self.find(from_), self.find(to)
        dis = self.dis
        if x == y:
            return dis[from_] ^ dis[to] == value
        dis[x] = value ^ dis[to] ^ dis[from_]
        self.fa[x] = y
        return True


class Solution:
    def numberOfEdgesAdded(self, n: int, edges: List[List[int]]) -> int:
        uf = UnionFind(n)
        ans = 0
        for x, y, w in edges:
            if uf.merge(x, y, w):
                ans += 1
        return ans
```

```java [sol-Java]
// 完整模板见 https://leetcode.cn/circle/discuss/mOr1u6/
class UnionFind {
    private final int[] fa; // fa[x] 是 x 的代表元
    private final int[] dis; // dis[x] = 从 x 到 fa[x] 的路径异或和

    public UnionFind(int n) {
        fa = new int[n];
        dis = new int[n];
        for (int i = 0; i < n; i++) {
            fa[i] = i;
        }
    }

    public int find(int x) {
        if (fa[x] != x) {
            int root = find(fa[x]);
            dis[x] ^= dis[fa[x]];
            fa[x] = root;
        }
        return fa[x];
    }

    public boolean merge(int from, int to, int value) {
        int x = find(from), y = find(to);
        if (x == y) {
            return (dis[from] ^ dis[to]) == value;
        }
        dis[x] = value ^ dis[to] ^ dis[from];
        fa[x] = y;
        return true;
    }
}

class Solution {
    public int numberOfEdgesAdded(int n, int[][] edges) {
        UnionFind uf = new UnionFind(n);
        int ans = 0;
        for (int[] e : edges) {
            if (uf.merge(e[0], e[1], e[2])) {
                ans++;
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
// 完整模板见 https://leetcode.cn/circle/discuss/mOr1u6/
// 根据题目用 UnionFind<int> uf(n) 或者 UnionFind<long long> uf(n) 初始化
template<typename T>
class UnionFind {
public:
    vector<int> fa; // fa[x] 是 x 的代表元
    vector<T> dis; // dis[x] = 从 x 到 fa[x] 的路径异或和

    UnionFind(int n) : fa(n), dis(n) {
        ranges::iota(fa, 0); // iota(fa.begin(), fa.end(), 0);
    }

    int find(int x) {
        if (fa[x] != x) {
            int root = find(fa[x]);
            dis[x] ^= dis[fa[x]];
            fa[x] = root;
        }
        return fa[x];
    }

    bool merge(int from, int to, T value) {
        int x = find(from), y = find(to);
        if (x == y) {
            return (dis[from] ^ dis[to]) == value;
        }
        dis[x] = value ^ dis[to] ^ dis[from];
        fa[x] = y;
        return true;
    }
};

class Solution {
public:
    int numberOfEdgesAdded(int n, vector<vector<int>>& edges) {
        int ans = 0;
        UnionFind<int> uf(n);
        for (auto& e : edges) {
            if (uf.merge(e[0], e[1], e[2])) {
                ans++;
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
// 完整模板见 https://leetcode.cn/circle/discuss/mOr1u6/
type unionFind struct {
	fa  []int // fa[x] 是 x 的代表元
	dis []int // dis[x] = 从 x 到 fa[x] 的路径异或和
}

func newUnionFind(n int) unionFind {
	fa := make([]int, n)
	dis := make([]int, n)
	for i := range fa {
		fa[i] = i
	}
	return unionFind{fa, dis}
}

func (u unionFind) find(x int) int {
	if u.fa[x] != x {
		root := u.find(u.fa[x])
		u.dis[x] ^= u.dis[u.fa[x]]
		u.fa[x] = root
	}
	return u.fa[x]
}

func (u unionFind) merge(from, to, value int) bool {
	x, y := u.find(from), u.find(to)
	if x == y {
		return u.dis[from]^u.dis[to] == value
	}
	u.dis[x] = value ^ u.dis[to] ^ u.dis[from]
	u.fa[x] = y
	return true
}

func numberOfEdgesAdded(n int, edges [][]int) (ans int) {
	uf := newUnionFind(n)
	for _, e := range edges {
		if uf.merge(e[0], e[1], e[2]) {
			ans++
		}
	}
	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n)$。
- 空间复杂度：$\mathcal{O}(n)$。

## 相似题目

[399. 除法求值](https://leetcode.cn/problems/evaluate-division/)

## 专题训练

见下面数据结构题单的「**§7.6 带权并查集（边权并查集）**」。

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

## 本地原创解析

### 1. 题意重述

本题来自 `七、并查集 / §7.6 带权并查集（边权并查集）`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `七、并查集 / §7.6 带权并查集（边权并查集）`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
