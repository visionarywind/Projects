# 3515. 带权树中的最短路径

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/shortest-path-in-a-weighted-tree/
- 题目 slug：`shortest-path-in-a-weighted-tree`
- 来源专题：链表、树与回溯
- 来源分类路径：三、一般树 / §3.7 DFS 时间戳
- 难度分：2312
- 外部题解来源：https://leetcode.cn/problems/shortest-path-in-a-weighted-tree/solutions/3649372/dfs-shi-jian-chuo-chai-fen-shu-zhuang-sh-h8q3/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[DFS 时间戳 + 差分树状数组（Python/Java/C++/Go）](https://leetcode.cn/problems/shortest-path-in-a-weighted-tree/solutions/3649372/dfs-shi-jian-chuo-chai-fen-shu-zhuang-sh-h8q3/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`dfs-shi-jian-chuo-chai-fen-shu-zhuang-sh-h8q3`
- topic id：`3649372`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:57:36 +0800

**前置知识**：

1. [DFS 时间戳——处理树上问题的有力工具](https://leetcode.cn/problems/minimum-score-after-removals-on-a-tree/solutions/1625899/dfs-shi-jian-chuo-chu-li-shu-shang-wen-t-x1kk/)
2. [树状数组](https://leetcode.cn/problems/range-sum-query-mutable/solution/dai-ni-fa-ming-shu-zhuang-shu-zu-fu-shu-lyfll/)
3. [差分思想](https://leetcode.cn/problems/car-pooling/solution/suan-fa-xiao-ke-tang-chai-fen-shu-zu-fu-9d4ra/)（推荐和[【图解】从一维差分到二维差分](https://leetcode.cn/problems/stamping-the-grid/solution/wu-nao-zuo-fa-er-wei-qian-zhui-he-er-wei-zwiu/) 一起看）

**核心思路**：如果把 $x$ 到 $y$（$y$ 是 $x$ 的儿子）的边权增加 $d$，那么节点 $1$ 到子树 $y$ 中的所有节点的距离都会增加 $d$。

> **注**：本题是树，所以最短路就是距离。

我们需要把「子树 $y$ 中的所有节点」转化成区间，从而能够用数据结构做区间更新。根据前置知识，求出每个节点的进出时间戳，那么在子树 $y$ 中的节点的**进入**时间戳，就都在闭区间 $[\textit{in}[y],\textit{out}[y]]$ 中了。

> **注**：$\textit{out}$ 时间戳可以理解为子树的 $\textit{in}$ 时间戳的最大值。

- 询问 1：修改边权，转换成边权增加了 $d$，相当于把闭区间 $[\textit{in}[y],\textit{out}[y]]$ 中的数都增加了 $d$。（区间更新）
- 询问 2：查询下标 $\textit{in}[y]$ 对应的数。（单点查询）

> **注**：因为每个节点的父节点是唯一的，边权信息可以保存在节点 $y$ 中。这样可以把边权转换成点权，方便维护。

有三种**实现方法**：

1. **Lazy 线段树**。区间加，单点查询。
2. **差分树状数组**。最适合本题。
3. **重链剖分**。最全面。支持路径更新、路径查询、子树更新、子树查询。

下面代码用的差分树状数组。完整的树状数组模板，见 [数据结构题单](https://leetcode.cn/circle/discuss/mOr1u6/)。

- 区间更新转换成两个点的更新。
- 单点查询转换成前缀和。

具体请看 [视频讲解](https://www.bilibili.com/video/BV1dSdBYEEW6/?t=15m3s)，欢迎点赞关注~

```py [sol-Python3]
class FenwickTree:
    def __init__(self, n: int):
        self.tree = [0] * (n + 1)  # 使用下标 1 到 n

    # a[i] 增加 val
    # 1 <= i <= n
    def update(self, i: int, val: int) -> None:
        while i < len(self.tree):
            self.tree[i] += val
            i += i & -i

    # 计算前缀和 a[1] + ... + a[i]
    # 1 <= i <= n
    def pre(self, i: int) -> int:
        res = 0
        while i > 0:
            res += self.tree[i]
            i &= i - 1
        return res

class Solution:
    def treeQueries(self, n: int, edges: List[List[int]], queries: List[List[int]]) -> List[int]:
        g = [[] for _ in range(n + 1)]
        for x, y, _ in edges:
            g[x].append(y)
            g[y].append(x)

        in_ = [0] * (n + 1)
        out = [0] * (n + 1)
        clock = 0
        def dfs(x: int, fa: int) -> None:
            nonlocal clock
            clock += 1
            in_[x] = clock  # 进来的时间
            for y in g[x]:
                if y != fa:
                    dfs(y, x)
            out[x] = clock  # 离开的时间
        dfs(1, 0)

        weight = [0] * (n + 1)
        diff = FenwickTree(n)
        def update(x: int, y: int, w: int) -> None:
            # 保证 y 是 x 的儿子
            if in_[x] > in_[y]:
                y = x
            d = w - weight[y]  # 边权的增量
            weight[y] = w
            # 把子树 y 中的最短路长度都增加 d（用差分树状数组维护）
            diff.update(in_[y], d)
            diff.update(out[y] + 1, -d)

        for e in edges:
            update(*e)

        ans = []
        for q in queries:
            if q[0] == 1:
                update(*q[1:])
            else:
                ans.append(diff.pre(in_[q[1]]))
        return ans
```

```java [sol-Java]
class FenwickTree {
    private final int[] tree;

    public FenwickTree(int n) {
        tree = new int[n + 1]; // 使用下标 1 到 n
    }

    // a[i] 增加 val
    // 1 <= i <= n
    public void update(int i, int val) {
        for (; i < tree.length; i += i & -i) {
            tree[i] += val;
        }
    }

    // 求前缀和 a[1] + ... + a[i]
    // 1 <= i <= n
    public int pre(int i) {
        int res = 0;
        for (; i > 0; i &= i - 1) {
            res += tree[i];
        }
        return res;
    }
}

class Solution {
    public int[] treeQueries(int n, int[][] edges, int[][] queries) {
        List<Integer>[] g = new ArrayList[n + 1];
        Arrays.setAll(g, i -> new ArrayList<>());
        for (int[] e : edges) {
            int x = e[0];
            int y = e[1];
            g[x].add(y);
            g[y].add(x);
        }

        int[] in = new int[n + 1];
        int[] out = new int[n + 1];
        dfs(1, 0, g, in, out);

        int[] weight = new int[n + 1];
        FenwickTree diff = new FenwickTree(n);

        for (int[] e : edges) {
            update(e[0], e[1], e[2], in, out, weight, diff);
        }

        List<Integer> ans = new ArrayList<>();
        for (int[] q : queries) {
            if (q[0] == 1) {
                update(q[1], q[2], q[3], in, out, weight, diff);
            } else {
                ans.add(diff.pre(in[q[1]]));
            }
        }
        return ans.stream().mapToInt(i -> i).toArray();
    }

    private int clock = 0;

    private void dfs(int x, int fa, List<Integer>[] g, int[] in, int[] out) {
        in[x] = ++clock; // 进来的时间
        for (int y : g[x]) {
            if (y != fa) {
                dfs(y, x, g, in, out);
            }
        }
        out[x] = clock; // 离开的时间
    }

    private void update(int x, int y, int w, int[] in, int[] out, int[] weight, FenwickTree diff) {
        // 保证 y 是 x 的儿子
        if (in[x] > in[y]) {
            y = x;
        }
        int d = w - weight[y]; // 边权的增量
        weight[y] = w;
        // 把子树 y 中的最短路长度都增加 d（用差分树状数组维护）
        diff.update(in[y], d);
        diff.update(out[y] + 1, -d);
    }
}
```

```cpp [sol-C++]
template<typename T>
class FenwickTree {
    vector<T> tree;

public:
    // 使用下标 1 到 n
    FenwickTree(int n) : tree(n + 1) {}

    // a[i] 增加 val
    // 1 <= i <= n
    void update(int i, T val) {
        for (; i < tree.size(); i += i & -i) {
            tree[i] += val;
        }
    }

    // 求前缀和 a[1] + ... + a[i]
    // 1 <= i <= n
    T pre(int i) const {
        T res = 0;
        for (; i > 0; i &= i - 1) {
            res += tree[i];
        }
        return res;
    }
};

class Solution {
public:
    vector<int> treeQueries(int n, vector<vector<int>>& edges, vector<vector<int>>& queries) {
        vector<vector<int>> g(n + 1);
        for (auto& e : edges) {
            int x = e[0], y = e[1];
            g[x].push_back(y);
            g[y].push_back(x);
        }

        vector<int> in(n + 1), out(n + 1);
        int clock = 0;
        auto dfs = [&](this auto&& dfs, int x, int fa) -> void {
            in[x] = ++clock; // 进来的时间
            for (int y : g[x]) {
                if (y != fa) {
                    dfs(y, x);
                }
            }
            out[x] = clock; // 离开的时间
        };
        dfs(1, 0);

        // 对于一条边 x-y（y 是 x 的儿子），把边权保存在 weight[y] 中
        vector<int> weight(n + 1);
        FenwickTree<int> diff(n);
        auto update = [&](int x, int y, int w) {
            // 保证 y 是 x 的儿子
            if (in[x] > in[y]) {
                y = x;
            }
            int d = w - weight[y]; // 边权的增量
            weight[y] = w;
            // 把子树 y 中的最短路长度都增加 d（用差分树状数组维护）
            diff.update(in[y], d);
            diff.update(out[y] + 1, -d);
        };

        for (auto& e : edges) {
            update(e[0], e[1], e[2]);
        }

        vector<int> ans;
        for (auto& q : queries) {
            if (q[0] == 1) {
                update(q[1], q[2], q[3]);
            } else {
                ans.push_back(diff.pre(in[q[1]]));
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
type fenwick []int

func newFenwickTree(n int) fenwick {
	return make(fenwick, n+1) // 使用下标 1 到 n
}

// a[i] 增加 val
// 1 <= i <= n
func (f fenwick) update(i, val int) {
	for ; i < len(f); i += i & -i {
		f[i] += val
	}
}

// 求前缀和 a[1] + ... + a[i]
// 1 <= i <= n
func (f fenwick) pre(i int) (s int) {
	for ; i > 0; i &= i - 1 {
		s += f[i]
	}
	return
}

func treeQueries(n int, edges [][]int, queries [][]int) (ans []int) {
	g := make([][]int, n+1)
	for _, e := range edges {
		x, y := e[0], e[1]
		g[x] = append(g[x], y)
		g[y] = append(g[y], x)
	}

	in := make([]int, n+1)
	out := make([]int, n+1)
	clock := 0
	var dfs func(int, int)
	dfs = func(x, fa int) {
		clock++
		in[x] = clock // 进来的时间
		for _, y := range g[x] {
			if y != fa {
				dfs(y, x)
			}
		}
		out[x] = clock // 离开的时间
	}
	dfs(1, 0)

	// 对于一条边 x-y（y 是 x 的儿子），把边权保存在 weight[y] 中
	weight := make([]int, n+1)
	diff := newFenwickTree(n)
	update := func(x, y, w int) {
		// 保证 y 是 x 的儿子
		if in[x] > in[y] {
			y = x
		}
		d := w - weight[y] // 边权的增量
		weight[y] = w
		// 把子树 y 中的最短路长度都增加 d（用差分树状数组维护）
		diff.update(in[y], d)
		diff.update(out[y]+1, -d)
	}

	for _, e := range edges {
		update(e[0], e[1], e[2])
	}

	for _, q := range queries {
		if q[0] == 1 {
			update(q[1], q[2], q[3])
		} else {
			ans = append(ans, diff.pre(in[q[1]]))
		}
	}
	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}((n+q)\log n)$，其中 $q$ 是 $\textit{queries}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。返回值不计入。

**注**：也可以 $\mathcal{O}(n)$ 初始化树状数组，从而做到 $\mathcal{O}(n + q\log n)$ 的时间。

## 相似题目

- [LCP 05. 发 LeetCoin](https://leetcode.cn/problems/coin-bonus/)

更多相似题目，见下面数据结构题单的「**§2.1 一维差分**」「**§8.1 树状数组**」和树题单的「**§3.6 DFS 时间戳**」。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

## 本地原创解析

### 1. 题意重述

本题来自 `三、一般树 / §3.7 DFS 时间戳`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `三、一般树 / §3.7 DFS 时间戳`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
