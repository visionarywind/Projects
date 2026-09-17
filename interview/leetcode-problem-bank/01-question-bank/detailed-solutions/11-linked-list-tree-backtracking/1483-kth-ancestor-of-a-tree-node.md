# 1483. 树节点的第 K 个祖先

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/kth-ancestor-of-a-tree-node/
- 题目 slug：`kth-ancestor-of-a-tree-node`
- 来源专题：链表、树与回溯
- 来源分类路径：三、一般树 / §3.8 最近公共祖先（LCA）、倍增算法
- 难度分：2115
- 外部题解来源：https://leetcode.cn/problems/kth-ancestor-of-a-tree-node/solutions/2305895/mo-ban-jiang-jie-shu-shang-bei-zeng-suan-v3rw/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【模板讲解】树上倍增算法（以及最近公共祖先）Python/Java/C++/Go](https://leetcode.cn/problems/kth-ancestor-of-a-tree-node/solutions/2305895/mo-ban-jiang-jie-shu-shang-bei-zeng-suan-v3rw/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`mo-ban-jiang-jie-shu-shang-bei-zeng-suan-v3rw`
- topic id：`2305895`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:57:36 +0800

## 一、思考

最暴力的做法是，从 $\textit{node}$ 出发，一步一步地往上跳，即 

$$
\textit{node} \rightarrow \textit{parent}[\textit{node}] \rightarrow \textit{parent}[\textit{parent}[\textit{node}]] \rightarrow \cdots
$$

需要跳 $k$ 次才能到达 $\textit{node}$ 的第 $k$ 个祖先节点，时间复杂度为 $\mathcal{O}(k)$。

如何优化这个暴力算法呢？

一个初步的想法是，预处理出每个节点的「爷爷节点」，即父节点的父节点，那么就可以两步两步地往上跳，从而减少一半的跳跃次数（循环次数）。

进一步地，再预处理出爷爷节点的爷爷节点，就可以四步四步地往上跳。

请你思考：一般地，要预处理出哪些节点呢？如何利用这些预处理出的节点，快速地找到第 $k$ 个祖先节点？

## 二、解惑

预处理出**每个节点**的第 $2^i$ 个祖先节点，即第 $1,2,4,8,\cdots$ 个祖先节点（其中 $x$ 的第 $1$ 个祖先节点就是 $\textit{parent}[x]$）。由于任意 $k$ 可以分解为若干不同的 $2$ 的幂（例如 $13=8+4+1$），所以只需要预处理出这些 $2^i$ 祖先节点，就可以快速地到达**任意**第 $k$ 个祖先节点。

例如 $k=13=8+4+1=1101_{(2)}$，我们可以先往上跳 $8$ 步，再往上跳 $4$ 步和 $1$ 步；也可以先往上跳 $1$ 步，再往上跳 $4$ 步和 $8$ 步。无论如何跳，都只需要跳 $3$ 次就能到达第 $13$ 个祖先节点。

据此，可以得到下面的算法。

## 三、算法

在构造函数 $\texttt{TreeAncestor}$ 中，预处理出每个节点 $\textit{x}$ 的第 $2^i$ 个祖先节点，记作 $\textit{pa}[x][i]$（若第 $2^i$ 个祖先节点不存在，则 $\textit{pa}[x][i]=-1$）。计算方式如下：
- 先枚举 $i$，再枚举 $x$。相当于先算出所有爷爷节点，再算出所有爷爷节点的爷爷节点，依此类推。
- $\textit{pa}[x][0]=\textit{parent}[x]$，即父节点。
- $\textit{pa}[x][1]=\textit{pa}[\textit{pa}[x][0]][0]$，即爷爷节点。
- 依此类推，$\textit{pa}[x][i+1]=\textit{pa}[\textit{pa}[x][i]][i]$，表示 $x$ 的第 $2^i$ 个祖先节点的第 $2^i$ 个祖先节点就是 $x$ 的第 $2^{i+1}$ 个祖先节点。特别地，如果 $\textit{pa}[x][i]=-1$ 则 $\textit{pa}[x][i+1]=-1$。
- 这里 $i+1$ 至多为 $\left\lfloor\log_2 n \right\rfloor$。例如 $n=13$ 时，$\left\lfloor\log_2 13 \right\rfloor=3$，至多需要预处理到第 $2^3$ 个祖先节点。（当然，你也可以先把树高，或者每个节点的深度求出来，再据此做精细地计算。）

对于 $\texttt{getKthAncestor}$，需要找到 $k$ 的二进制表示中的所有 $1$（相当于把 $k$ 分解为若干 $2^i$）。可以从小到大枚举 $i$，如果 $k$ 右移 $i$ 位后的最低位为 $1$，就说明 $k$ 的二进制从低到高第 $i$ 位是 $1$，那么往上跳 $2^i$ 步，将 $\textit{node}$ 更新为 $\textit{pa}[\textit{node}][i]$。如果 $\textit{node}=-1$ 则说明第 $k$ 个祖先节点不存在。

代码中用到了一些位运算技巧，具体请看 [从集合论到位运算，常见位运算技巧分类总结！](https://leetcode.cn/circle/discuss/CaOJ45/)

```py [sol-Python3]
class TreeAncestor:
    def __init__(self, n: int, parent: List[int]):
        m = n.bit_length() - 1
        pa = [[p] + [-1] * m for p in parent]
        for i in range(m):
            for x in range(n):
                if (p := pa[x][i]) != -1:
                    pa[x][i + 1] = pa[p][i]
        self.pa = pa

    def getKthAncestor(self, node: int, k: int) -> int:
        for i in range(k.bit_length()):
            if k >> i & 1:  # k 的二进制从低到高第 i 位是 1
                node = self.pa[node][i]
                if node < 0:
                    break
        return node

    # 另一种写法，不断去掉 k 的最低位的 1
    def getKthAncestor2(self, node: int, k: int) -> int:
        while k and node != -1:
            lb = k & -k
            node = self.pa[node][lb.bit_length() - 1]
            k ^= lb
        return node
```

```java [sol-Java]
class TreeAncestor {
    private final int[][] pa;

    public TreeAncestor(int n, int[] parent) {
        int m = 32 - Integer.numberOfLeadingZeros(n); // n 的二进制长度
        pa = new int[n][m];
        for (int i = 0; i < n; i++) {
            pa[i][0] = parent[i];
        }
        for (int i = 0; i < m - 1; i++) {
            for (int x = 0; x < n; x++) {
                int p = pa[x][i];
                pa[x][i + 1] = p < 0 ? -1 : pa[p][i];
            }
        }
    }

    public int getKthAncestor(int node, int k) {
        int m = 32 - Integer.numberOfLeadingZeros(k); // k 的二进制长度
        for (int i = 0; i < m; i++) {
            if ((k >> i & 1) > 0) { // k 的二进制从低到高第 i 位是 1
                node = pa[node][i];
                if (node < 0) break;
            }
        }
        return node;
    }

    // 另一种写法，不断去掉 k 的最低位的 1
    public int getKthAncestor2(int node, int k) {
        for (; k > 0 && node != -1; k &= k - 1) {
            node = pa[node][Integer.numberOfTrailingZeros(k)];
        }
        return node;
    }
}
```

```cpp [sol-C++]
class TreeAncestor {
    vector<vector<int>> pa;

public:
    TreeAncestor(int n, vector<int>& parent) {
        int m = bit_width((unsigned) n); // n 的二进制长度
        pa.resize(n, vector<int>(m, -1));
        for (int i = 0; i < n; i++) {
            pa[i][0] = parent[i];
        }
        for (int i = 0; i < m - 1; i++) {
            for (int x = 0; x < n; x++) {
                if (int p = pa[x][i]; p != -1) {
                    pa[x][i + 1] = pa[p][i];
                }
            }
        }
    }

    int getKthAncestor(int node, int k) {
        int m = bit_width((unsigned) k); // k 的二进制长度
        for (int i = 0; i < m; i++) {
            if (k >> i & 1) { // k 的二进制从低到高第 i 位是 1
                node = pa[node][i];
                if (node < 0) break;
            }
        }
        return node;
    }

    // 另一种写法，不断去掉 k 的最低位的 1
    int getKthAncestor2(int node, int k) {
        for (; k && node != -1; k &= k - 1) {
            node = pa[node][countr_zero((unsigned) k)];
        }
        return node;
    }
};
```

```go [sol-Go]
type TreeAncestor [][]int

func Constructor(n int, parent []int) TreeAncestor {
    m := bits.Len(uint(n))
    pa := make([][]int, n)
    for i, p := range parent {
        pa[i] = make([]int, m)
        pa[i][0] = p
    }
    for i := range m - 1 {
        for x := range n {
            if p := pa[x][i]; p != -1 {
                pa[x][i+1] = pa[p][i]
            } else {
                pa[x][i+1] = -1
            }
        }
    }
    return pa
}

func (pa TreeAncestor) GetKthAncestor(node, k int) int {
    m := bits.Len(uint(k))
    for i := range m {
        if k>>i&1 > 0 { // k 的二进制从低到高第 i 位是 1
            node = pa[node][i]
            if node < 0 {
                break
            }
        }
    }
    return node
}

// 另一种写法，不断去掉 k 的最低位的 1
func (pa TreeAncestor) GetKthAncestor2(node, k int) int {
    for ; k > 0 && node != -1; k &= k - 1 {
        node = pa[node][bits.TrailingZeros(uint(k))]
    }
    return node
}
```

#### 复杂度分析

- 时间复杂度：预处理 $\mathcal{O}(n\log n)$，回答每个询问 $\mathcal{O}(\log k)$。
- 空间复杂度：预处理需要 $\mathcal{O}(n\log n)$ 的空间。

> 注：利用**长链剖分**，可以做到预处理 $\mathcal{O}(n\log n)$，回答每个询问 $\mathcal{O}(1)$ 的时间复杂度。

## 四、【模板】最近公共祖先

下面是本题的扩展内容。

如何计算树上任意两点 $x$ 和 $y$ 的最近公共祖先 $\textit{lca}$ 呢？

设节点 $i$ 的深度为 $\textit{depth}[i]$。这可以通过一次 DFS 预处理出来。

假设 $\textit{depth}[x]\le \textit{depth}[y]$（否则交换两点）。我们可以先把更靠下的 $y$ 更新为 $y$ 的第 $\textit{depth}[y]- \textit{depth}[x]$ 个祖先节点，这样 $x$ 和 $y$ 就处在同一深度了。

如果此时 $x=y$，那么 $x$ 就是 $\textit{lca}$。否则说明 $\textit{lca}$ 在更上面，那么就把 $x$ 和 $y$ **一起往上跳**。

由于不知道 $\textit{lca}$ 的具体位置，只能不断尝试，先尝试大步跳，再尝试小步跳。设 $i=\left\lfloor\log_2 n \right\rfloor$，循环直到 $i<0$。每次循环：

- 如果 $x$ 的第 $2^i$ 个祖先节点不存在，即 $\textit{pa}[x][i]=-1$，说明步子迈大了，将 $i$ 减 $1$，继续循环。
- 如果 $x$ 的第 $2^i$ 个祖先节点存在，且 $\textit{pa}[x][i]\ne \textit{pa}[y][i]$，说明 $\textit{lca}$ 在 $\textit{pa}[x][i]$ 的上面，那么更新 $x$ 为 $\textit{pa}[x][i]$，更新 $y$ 为 $\textit{pa}[y][i]$，将 $i$ 减 $1$，继续循环。否则，若 $\textit{pa}[x][i]=\textit{pa}[y][i]$，那么 $\textit{lca}$ 可能在 $\textit{pa}[x][i]$ 下面，由于无法向下跳，只能将 $i$ 减 $1$，继续循环。

上述做法能跳就尽量跳，不会错过任何可以上跳的机会。所以循环结束时，$x$ 与 $\textit{lca}$ 只有一步之遥，即 $\textit{lca}=\textit{pa}[x][0]$。

> 注：你也可以用二分来理解上述算法。在 $x$ 到根节点的这条路径上**猜一个点** $z$ 当作 $\textit{lca}$，且 $x$ 与 $z$ 相距 $2^i$ 步。那么把 $x$ 和 $y$ 同时向上跳 $2^i$ 步，如果 $x\ne y$，就说明 $\textit{lca}$ 在 $z$ 的上面，否则 $\textit{lca}$ 要么是 $z$，要么在 $z$ 的下面。这样一种二段性既说明了二分的正确性，又说明了每次上跳之后，步长一定要减半（类比二分查找，把搜索的区间长度减半）。

考虑到通常题目是用 $\textit{edges}$ 的方式输入的，所以下面的模板先用 $\textit{edges}$ 建图，再用 DFS 预处理。

```py [sol-Python3]
class TreeAncestor:
    def __init__(self, edges: List[List[int]]):
        n = len(edges) + 1
        m = n.bit_length()
        g = [[] for _ in range(n)]
        for x, y in edges:  # 节点编号从 0 开始
            g[x].append(y)
            g[y].append(x)

        depth = [0] * n
        pa = [[-1] * m for _ in range(n)]
        def dfs(x: int, fa: int) -> None:
            pa[x][0] = fa
            for y in g[x]:
                if y != fa:
                    depth[y] = depth[x] + 1
                    dfs(y, x)
        dfs(0, -1)

        for i in range(m - 1):
            for x in range(n):
                if (p := pa[x][i]) != -1:
                    pa[x][i + 1] = pa[p][i]
        self.depth = depth
        self.pa = pa

    def get_kth_ancestor(self, node: int, k: int) -> int:
        for i in range(k.bit_length()):
            if k >> i & 1:  # k 二进制从低到高第 i 位是 1
                node = self.pa[node][i]
        return node

    # 返回 x 和 y 的最近公共祖先（节点编号从 0 开始）
    def get_lca(self, x: int, y: int) -> int:
        if self.depth[x] > self.depth[y]:
            x, y = y, x
        # 使 y 和 x 在同一深度
        y = self.get_kth_ancestor(y, self.depth[y] - self.depth[x])
        if y == x:
            return x
        for i in range(len(self.pa[x]) - 1, -1, -1):
            px, py = self.pa[x][i], self.pa[y][i]
            if px != py:
                x, y = px, py  # 同时往上跳 2**i 步
        return self.pa[x][0]
```

```java [sol-Java]
class TreeAncestor {
    private final int[] depth;
    private final int[][] pa;

    public TreeAncestor(int[][] edges) {
        int n = edges.length + 1;
        int m = 32 - Integer.numberOfLeadingZeros(n); // n 的二进制长度
        List<Integer>[] g = new ArrayList[n];
        Arrays.setAll(g, e -> new ArrayList<>());
        for (int[] e : edges) {
            int x = e[0], y = e[1]; // 节点编号从 0 开始
            g[x].add(y);
            g[y].add(x);
        }

        depth = new int[n];
        pa = new int[n][m];
        dfs(g, 0, -1);

        for (int i = 0; i < m - 1; i++) {
            for (int x = 0; x < n; x++) {
                int p = pa[x][i];
                pa[x][i + 1] = p < 0 ? -1 : pa[p][i];
            }
        }
    }

    private void dfs(List<Integer>[] g, int x, int fa) {
        pa[x][0] = fa;
        for (int y : g[x]) {
            if (y != fa) {
                depth[y] = depth[x] + 1;
                dfs(g, y, x);
            }
        }
    }

    public int getKthAncestor(int node, int k) {
        for (; k > 0; k &= k - 1) {
            node = pa[node][Integer.numberOfTrailingZeros(k)];
        }
        return node;
    }

    public int getLCA(int x, int y) {
        if (depth[x] > depth[y]) {
            int tmp = y;
            y = x;
            x = tmp;
        }
        y = getKthAncestor(y, depth[y] - depth[x]); // 使 y 和 x 在同一深度
        if (y == x) {
            return x;
        }
        for (int i = pa[x].length - 1; i >= 0; i--) {
            int px = pa[x][i], py = pa[y][i];
            if (px != py) {
                x = px;
                y = py; // 同时往上跳 2^i 步
            }
        }
        return pa[x][0];
    }
}
```

```cpp [sol-C++]
class TreeAncestor {
    vector<int> depth;
    vector<vector<int>> pa;

public:
    TreeAncestor(vector<pair<int, int>>& edges) {
        int n = edges.size() + 1;
        int m = bit_width((unsigned) n); // n 的二进制长度
        vector<vector<int>> g(n);
        for (auto& [x, y] : edges) { // 节点编号从 0 开始
            g[x].push_back(y);
            g[y].push_back(x);
        }

        depth.resize(n);
        pa.resize(n, vector<int>(m, -1));
        auto dfs = [&](this auto&& dfs, int x, int fa) -> void {
            pa[x][0] = fa;
            for (int y : g[x]) {
                if (y != fa) {
                    depth[y] = depth[x] + 1;
                    dfs(y, x);
                }
            }
        };
        dfs(0, -1);

        for (int i = 0; i < m - 1; i++) {
            for (int x = 0; x < n; x++) {
                if (int p = pa[x][i]; p != -1) {
                    pa[x][i + 1] = pa[p][i];
                }
            }
        }
    }

    int get_kth_ancestor(int node, int k) {
        for (; k; k &= k - 1) {
            node = pa[node][countr_zero((unsigned) k)];
        }
        return node;
    }

    // 返回 x 和 y 的最近公共祖先（节点编号从 0 开始）
    int get_lca(int x, int y) {
        if (depth[x] > depth[y]) {
            swap(x, y);
        }
        y = get_kth_ancestor(y, depth[y] - depth[x]); // 使 y 和 x 在同一深度
        if (y == x) {
            return x;
        }
        for (int i = pa[x].size() - 1; i >= 0; i--) {
            int px = pa[x][i], py = pa[y][i];
            if (px != py) {
                x = px;
                y = py; // 同时往上跳 2^i 步
            }
        }
        return pa[x][0];
    }
};
```

```go [sol-Go]
type TreeAncestor struct {
    depth []int
    pa    [][]int
}

func Constructor(edges [][]int) *TreeAncestor {
    n := len(edges) + 1
    m := bits.Len(uint(n))
    g := make([][]int, n)
    for _, e := range edges {
        x, y := e[0], e[1] // 节点编号从 0 开始
        g[x] = append(g[x], y)
        g[y] = append(g[y], x)
    }

    depth := make([]int, n)
    pa := make([][]int, n)
    var dfs func(int, int)
    dfs = func(x, fa int) {
        pa[x] = make([]int, m)
        pa[x][0] = fa
        for _, y := range g[x] {
            if y != fa {
                depth[y] = depth[x] + 1
                dfs(y, x)
            }
        }
    }
    dfs(0, -1)

    for i := range m - 1 {
        for x := range n {
            if p := pa[x][i]; p != -1 {
                pa[x][i+1] = pa[p][i]
            } else {
                pa[x][i+1] = -1
            }
        }
    }
    return &TreeAncestor{depth, pa}
}

func (t *TreeAncestor) GetKthAncestor(node, k int) int {
    for ; k > 0; k &= k - 1 {
        node = t.pa[node][bits.TrailingZeros(uint(k))]
    }
    return node
}

// 返回 x 和 y 的最近公共祖先（节点编号从 0 开始）
func (t *TreeAncestor) GetLCA(x, y int) int {
    if t.depth[x] > t.depth[y] {
        x, y = y, x
    }
    y = t.GetKthAncestor(y, t.depth[y]-t.depth[x]) // 使 y 和 x 在同一深度
    if y == x {
        return x
    }
    for i := len(t.pa[x]) - 1; i >= 0; i-- {
        px, py := t.pa[x][i], t.pa[y][i]
        if px != py {
            x, y = px, py // 同时往上跳 2^i 步
        }
    }
    return t.pa[x][0]
}
```

#### 复杂度分析

- 时间复杂度：预处理 $\mathcal{O}(n\log n)$，回答每个询问 $\mathcal{O}(\log n)$。
- 空间复杂度：预处理需要 $\mathcal{O}(n\log n)$ 的空间。

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

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `三、一般树 / §3.8 最近公共祖先（LCA）、倍增算法`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `三、一般树 / §3.8 最近公共祖先（LCA）、倍增算法`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
