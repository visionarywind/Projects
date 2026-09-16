# 2192. 有向无环图中一个节点的所有祖先

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/all-ancestors-of-a-node-in-a-directed-acyclic-graph/
- 题目 slug：`all-ancestors-of-a-node-in-a-directed-acyclic-graph`
- 来源专题：图论算法
- 来源分类路径：一、图的遍历 / §1.1 深度优先搜索（DFS）
- 难度分：1788
- 外部题解来源：https://leetcode.cn/problems/all-ancestors-of-a-node-in-a-directed-acyclic-graph/solutions/2723203/liang-chong-fang-fa-ni-xiang-zheng-xiang-rwjs/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：逆向/正向（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/all-ancestors-of-a-node-in-a-directed-acyclic-graph/solutions/2723203/liang-chong-fang-fa-ni-xiang-zheng-xiang-rwjs/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`liang-chong-fang-fa-ni-xiang-zheng-xiang-rwjs`
- topic id：`2723203`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 方法一：逆向 DFS

把边**反向**，从点 $i$ 出发 DFS，能访问到的点就是 $\textit{answer}[i]$。

例如示例 1，把边反向后，从 $6$ 出发 DFS，可以访问到 $0,1,2,3,4$。

![lc2192.png](https://pic.leetcode.cn/1711720529-hapVMv-lc2192.png)

从点 $i$ 出发 DFS，为避免重复访问节点，可以用 $\textit{vis}$ 数组标记访问过的节点，这样 DFS 结束后，有标记的节点就组成 $\textit{answer}[i]$ 了。

注：本题输入的是一个有向无环图，但该方法并不需要这个条件，即使图中有环，我们也可以找到所有能访问到 $i$ 的节点。

```py [sol-Python3]
class Solution:
    def getAncestors(self, n: int, edges: List[List[int]]) -> List[List[int]]:
        g = [[] for _ in range(n)]
        for x, y in edges:
            g[y].append(x)  # 反向建图

        def dfs(x: int) -> None:
            vis[x] = True  # 避免重复访问
            for y in g[x]:
                if not vis[y]:
                    dfs(y)  # 只递归没有访问过的点

        ans = [None] * n
        for i in range(n):
            vis = [False] * n
            dfs(i)  # 从 i 开始 DFS
            vis[i] = False  # ans[i] 不含 i
            ans[i] = [j for j, b in enumerate(vis) if b]
        return ans
```

```java [sol-Java]
class Solution {
    public List<List<Integer>> getAncestors(int n, int[][] edges) {
        List<Integer>[] g = new ArrayList[n];
        Arrays.setAll(g, i -> new ArrayList<>());
        for (int[] e : edges) {
            g[e[1]].add(e[0]); // 反向建图
        }

        List<Integer>[] ans = new ArrayList[n];
        Arrays.setAll(ans, i -> new ArrayList<>());
        boolean[] vis = new boolean[n];
        for (int i = 0; i < n; i++) {
            Arrays.fill(vis, false);
            dfs(i, g, vis); // 从 i 开始 DFS
            vis[i] = false; // ans[i] 不含 i
            for (int j = 0; j < n; j++) {
                if (vis[j]) {
                    ans[i].add(j);
                }
            }
        }
        return Arrays.asList(ans);
    }

    private void dfs(int x, List<Integer>[] g, boolean[] vis) {
        vis[x] = true; // 避免重复访问
        for (int y : g[x]) {
            if (!vis[y]) {
                dfs(y, g, vis); // 只递归没有访问过的点
            }
        }
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<vector<int>> getAncestors(int n, vector<vector<int>> &edges) {
        vector<vector<int>> g(n);
        for (auto &e : edges) {
            g[e[1]].push_back(e[0]); // 反向建图
        }

        vector<vector<int>> ans(n);
        vector<int> vis(n);
        function<void(int)> dfs = [&](int x) {
            vis[x] = true; // 避免重复访问
            for (int y : g[x]) {
                if (!vis[y]) {
                    dfs(y); // 只递归没有访问过的点
                }
            }
        };
        for (int i = 0; i < n; i++) {
            ranges::fill(vis, false);
            dfs(i); // 从 i 开始 DFS
            vis[i] = false; // ans[i] 不含 i
            for (int j = 0; j < n; j++) {
                if (vis[j]) {
                    ans[i].push_back(j);
                }
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func getAncestors(n int, edges [][]int) [][]int {
    g := make([][]int, n)
    for _, e := range edges {
        x, y := e[0], e[1]
        g[y] = append(g[y], x) // 反向建图
    }

    vis := make([]bool, n)
    var dfs func(int)
    dfs = func(x int) {
        vis[x] = true // 避免重复访问
        for _, y := range g[x] {
            if !vis[y] {
                dfs(y) // 只递归没有访问过的点
            }
        }
    }
    ans := make([][]int, n)
    for i := range ans {
        clear(vis)
        dfs(i) // 从 i 开始 DFS
        vis[i] = false // ans[i] 不含 i
        for j, b := range vis {
            if b {
                ans[i] = append(ans[i], j)
            }
        }
    }
    return ans
}
```

```js [sol-JavaScript]
var getAncestors = function(n, edges) {
    const g = Array.from({length: n}, () => []);
    for (const [x, y] of edges) {
        g[y].push(x); // 反向建图
    }

    function dfs(x) {
        vis[x] = true; // 避免重复访问
        for (const y of g[x]) {
            if (!vis[y]) {
                dfs(y, g, vis); // 只递归没有访问过的点
            }
        }
    }
    const ans = Array.from({length: n}, () => []);
    const vis = Array(n);
    for (let i = 0; i < n; i++) {
        vis.fill(false);
        dfs(i); // 从 i 开始 DFS
        vis[i] = false; // ans[i] 不含 i
        for (let j = 0; j < n; j++) {
            if (vis[j]) {
                ans[i].push(j);
            }
        }
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn get_ancestors(n: i32, edges: Vec<Vec<i32>>) -> Vec<Vec<i32>> {
        let n = n as usize;
        let mut g = vec![vec![]; n];
        for e in &edges {
            g[e[1] as usize].push(e[0] as usize); // 反向建图
        }

        fn dfs(x: usize, g: &Vec<Vec<usize>>, vis: &mut Vec<bool>) {
            vis[x] = true; // 避免重复访问
            for &y in &g[x] {
                if !vis[y] {
                    dfs(y, g, vis); // 只递归没有访问过的点
                }
            }
        }
        let mut ans = vec![vec![]; n];
        let mut vis = vec![false; n];
        for i in 0..n {
            vis.fill(false);
            dfs(i, &g, &mut vis); // 从 i 开始 DFS
            vis[i] = false; // ans[i] 不含 i
            for (j, &b) in vis.iter().enumerate() {
                if b {
                    ans[i].push(j as i32);
                }
            }
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n(n+m))$。其中 $m$ 是 $\textit{edges}$ 的长度。对于每个起点 $i$，跑一次 DFS 的时间复杂度为 $\mathcal{O}(n+m)$。
- 空间复杂度：$\mathcal{O}(n+m)$。返回值不计入。

## 方法二：正向 DFS

例如示例 1，从 $2$ 出发 DFS，可以访问到 $4,6,7$，那么把 $2$ 加到 $\textit{answer}[4], \textit{answer}[6], \textit{answer}[7]$ 中。

![lc2192.png](https://pic.leetcode.cn/1711720529-hapVMv-lc2192.png)

依次从起点 $\textit{start}=0,1,2,\cdots,n-1$ 出发 DFS，途中把 $\textit{start}$ 加到能访问到的点的 $\textit{answer}$ 中。由于 $\textit{start}$ 从小到大枚举，所以 $\textit{answer}[i]$ 列表自然就是有序的了。

例如：

- 从 $0$ 出发访问到 $5$，把 $0$ 加到 $\textit{answer}[5]$ 中，现在 $\textit{answer}[5]=[0]$。
- 从 $1$ 出发访问到 $5$，把 $1$ 加到 $\textit{answer}[5]$ 中，现在 $\textit{answer}[5]=[0,1]$。
- 从 $3$ 出发访问到 $5$，把 $3$ 加到 $\textit{answer}[5]$ 中，现在 $\textit{answer}[5]=[0,1,3]$。

**小技巧**：无需每次 DFS 前都重新初始化 $\textit{vis}$ 数组。我们会跑 $n$ 个 DFS，每个 DFS 的 $\textit{start}$ 都是不同的。利用这一条件，当访问到节点 $x$ 时，标记 $\textit{vis}[x] = \textit{start}$，表示 $x$ 是本轮 DFS 中访问到的节点。当我们访问到某个节点 $y$ 时，如果发现 $\textit{vis}[y] = \textit{start}$，就表示 $y$ 访问过了，否则没有访问过。

注：本题输入的是一个有向无环图，但该方法并不需要这个条件，即使图中有环，我们也可以找到所有能访问到 $i$ 的节点。

```py [sol-Python3]
class Solution:
    def getAncestors(self, n: int, edges: List[List[int]]) -> List[List[int]]:
        g = [[] for _ in range(n)]
        for x, y in edges:
            g[x].append(y)

        def dfs(x: int) -> None:
            vis[x] = start  # 避免重复访问
            for y in g[x]:
                if vis[y] != start:
                    ans[y].append(start)  # start 是访问到的点的祖先
                    dfs(y)  # 只递归没有访问过的点

        ans = [[] for _ in range(n)]
        vis = [-1] * n
        for start in range(n):
            dfs(start)  # 从 start 开始 DFS
        return ans
```

```java [sol-Java]
class Solution {
    public List<List<Integer>> getAncestors(int n, int[][] edges) {
        List<Integer>[] g = new ArrayList[n];
        Arrays.setAll(g, i -> new ArrayList<>());
        for (int[] e : edges) {
            g[e[0]].add(e[1]);
        }

        List<Integer>[] ans = new ArrayList[n];
        Arrays.setAll(ans, i -> new ArrayList<>());
        int[] vis = new int[n];
        Arrays.fill(vis, -1);
        for (int start = 0; start < n; start++) {
            dfs(start, start, g, vis, ans); // 从 start 开始 DFS
        }
        return Arrays.asList(ans);
    }

    private void dfs(int x, int start, List<Integer>[] g, int[] vis, List<Integer>[] ans) {
        vis[x] = start; // 避免重复访问
        for (int y : g[x]) {
            if (vis[y] != start) {
                ans[y].add(start); // start 是访问到的点的祖先
                dfs(y, start, g, vis, ans); // 只递归没有访问过的点
            }
        }
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<vector<int>> getAncestors(int n, vector<vector<int>> &edges) {
        vector<vector<int>> g(n);
        for (auto &e : edges) {
            g[e[0]].push_back(e[1]);
        }

        vector<vector<int>> ans(n);
        vector<int> vis(n, -1);
        int start;
        function<void(int)> dfs = [&](int x) {
            vis[x] = start; // 避免重复访问
            for (int y : g[x]) {
                if (vis[y] != start) {
                    ans[y].push_back(start); // start 是访问到的点的祖先
                    dfs(y); // 只递归没有访问过的点
                }
            }
        };
        for (start = 0; start < n; start++) {
            dfs(start); // 从 start 开始 DFS
        }
        return ans;
    }
};
```

```go [sol-Go]
func getAncestors(n int, edges [][]int) [][]int {
    g := make([][]int, n)
    for _, e := range edges {
        x, y := e[0], e[1]
        g[x] = append(g[x], y)
    }

    ans := make([][]int, n)
    vis := make([]int, n)
    start := 0
    var dfs func(int)
    dfs = func(x int) {
        vis[x] = start + 1 // 避免重复访问
        for _, y := range g[x] {
            if vis[y] != start+1 {
                ans[y] = append(ans[y], start) // start 是访问到的点的祖先
                dfs(y) // 只递归没有访问过的点
            }
        }
    }
    for ; start < n; start++ {
        dfs(start) // 从 start 开始 DFS
    }
    return ans
}
```

```js [sol-JavaScript]
var getAncestors = function(n, edges) {
    const g = Array.from({length: n}, () => []);
    for (const [x, y] of edges) {
        g[x].push(y);
    }

    function dfs(x) {
        vis[x] = start; // 避免重复访问
        for (const y of g[x]) {
            if (vis[y] !== start) {
                ans[y].push(start); // start 是访问到的点的祖先
                dfs(y); // 只递归没有访问过的点
            }
        }
    }
    const ans = Array.from({length: n}, () => []);
    const vis = Array(n).fill(-1);
    let start = 0;
    for (; start < n; start++) {
        dfs(start); // 从 start 开始 DFS
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn get_ancestors(n: i32, edges: Vec<Vec<i32>>) -> Vec<Vec<i32>> {
        let n = n as usize;
        let mut g = vec![vec![]; n];
        for e in &edges {
            g[e[0] as usize].push(e[1] as usize);
        }

        fn dfs(x: usize, start: usize, g: &Vec<Vec<usize>>, vis: &mut Vec<usize>, ans: &mut Vec<Vec<i32>>) {
            vis[x] = start; // 避免重复访问
            for &y in &g[x] {
                if vis[y] != start {
                    ans[y].push(start as i32); // start 是访问到的点的祖先
                    dfs(y, start, g, vis, ans); // 只递归没有访问过的点
                }
            }
        }
        let mut ans = vec![vec![]; n];
        let mut vis = vec![n; n];
        for start in 0..n {
            dfs(start, start, &g, &mut vis, &mut ans); // 从 start 开始 DFS
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n(n+m))$。其中 $m$ 是 $\textit{edges}$ 的长度。对于每个起点 $\textit{start}$，跑一次 DFS 的时间复杂度为 $\mathcal{O}(n+m)$。
- 空间复杂度：$\mathcal{O}(n+m)$。返回值不计入。

## 分类题单

1. [滑动窗口（定长/不定长/多指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（矩形系列/字典序最小/贡献法）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/贪心/脑筋急转弯）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)

更多题单，点我个人主页 - 讨论发布。

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

[往期题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

## 本地原创解析

### 1. 题意重述

本题来自 `一、图的遍历 / §1.1 深度优先搜索（DFS）`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、图的遍历 / §1.1 深度优先搜索（DFS）`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
