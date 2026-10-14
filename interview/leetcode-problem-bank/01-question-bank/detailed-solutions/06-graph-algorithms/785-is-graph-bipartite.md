# 785. 判断二分图

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/is-graph-bipartite/
- 题目 slug：`is-graph-bipartite`
- 来源专题：图论算法
- 来源分类路径：七、二分图染色
- 难度分：1625
- 外部题解来源：https://leetcode.cn/problems/is-graph-bipartite/solutions/3803670/tu-jie-jiao-ti-ran-se-fa-pythonjavaccgoj-ov27/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【图解】交替染色法（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/is-graph-bipartite/solutions/3803670/tu-jie-jiao-ti-ran-se-fa-pythonjavaccgoj-ov27/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tu-jie-jiao-ti-ran-se-fa-pythonjavaccgoj-ov27`
- topic id：`3803670`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

![lc785-c.png](https://pic.leetcode.cn/1760224846-iPvDLZ-lc785-c.png){:width=650px}

[本题视频讲解](https://www.bilibili.com/video/BV16E4uzLEdK/)，欢迎点赞关注~

```py [sol-Python3]
class Solution:
    def isBipartite(self, graph: List[List[int]]) -> bool:
        # colors[i] = 0  表示未访问节点 i
        # colors[i] = 1  表示节点 i 为红色
        # colors[i] = -1 表示节点 i 为蓝色
        colors = [0] * len(graph)

        def dfs(x: int, c: int) -> bool:
            colors[x] = c  # 节点 x 染成颜色 c
            for y in graph[x]:
                # 邻居 y 的颜色与 x 的相同，说明不是二分图，返回 False
                # 或者继续递归，发现不是二分图，返回 False
                if colors[y] == c or \
                   colors[y] == 0 and not dfs(y, -c):  # 取相反数，实现交替染色
                    return False
            return True

        # 可能有多个连通块
        for i, c in enumerate(colors):
            if c == 0 and not dfs(i, 1):
                # 从节点 i 开始递归，发现 i 所在连通块不是二分图
                return False
        return True
```

```java [sol-Java]
class Solution {
    public boolean isBipartite(int[][] graph) {
        // colors[i] = 0  表示未访问节点 i
        // colors[i] = 1  表示节点 i 为红色
        // colors[i] = -1 表示节点 i 为蓝色
        int[] colors = new int[graph.length];

        for (int i = 0; i < graph.length; i++) {
            if (colors[i] == 0 && !dfs(i, 1, graph, colors)) {
                // 从节点 i 开始递归，发现 i 所在连通块不是二分图
                return false;
            }
        }
        return true;
    }

    private boolean dfs(int x, int c, int[][] graph, int[] colors) {
        colors[x] = c; // 节点 x 染成颜色 c
        for (int y : graph[x]) {
            // 邻居 y 的颜色与 x 的相同，说明不是二分图，返回 false
            // 或者继续递归，发现不是二分图，返回 false
            if (colors[y] == c || 
                colors[y] == 0 && !dfs(y, -c, graph, colors)) { // 取相反数，实现交替染色
                return false;
            }
        }
        return true;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    bool isBipartite(vector<vector<int>>& graph) {
        // colors[i] = 0  表示未访问节点 i
        // colors[i] = 1  表示节点 i 为红色
        // colors[i] = -1 表示节点 i 为蓝色
        vector<int8_t> colors(graph.size());

        // lambda 递归
        auto dfs = [&](this auto&& dfs, int x, int8_t c) -> bool {
            colors[x] = c; // 节点 x 染成颜色 c
            for (int y : graph[x]) {
                // 邻居 y 的颜色与 x 的相同，说明不是二分图，返回 false
                // 或者继续递归，发现不是二分图，返回 false
                if (colors[y] == c || 
                    colors[y] == 0 && !dfs(y, -c)) { // 取相反数，实现交替染色
                    return false;
                }
            }
            return true;
        };

        // 可能有多个连通块
        for (int i = 0; i < colors.size(); i++) {
            if (colors[i] == 0 && !dfs(i, 1)) {
                // 从节点 i 开始递归，发现 i 所在连通块不是二分图
                return false;
            }
        }
        return true;
    }
};
```

```c [sol-C]
bool isBipartite(int** graph, int graphSize, int* graphColSize) {
    // colors[i] = 0  表示未访问节点 i
    // colors[i] = 1  表示节点 i 为红色
    // colors[i] = -1 表示节点 i 为蓝色
    int* colors = calloc(graphSize, sizeof(int));

    bool dfs(int x, int c) {
        colors[x] = c; // 节点 x 染成颜色 c
        for (int i = 0; i < graphColSize[x]; i++) {
            int y = graph[x][i];
            // 邻居 y 的颜色与 x 的相同，说明不是二分图，返回 false
            // 或者继续递归，发现不是二分图，返回 false
            if (colors[y] == c || 
                colors[y] == 0 && !dfs(y, -c)) { // 取相反数，实现交替染色
                return false;
            }
        }
        return true;
    }

    // 可能有多个连通块
    for (int i = 0; i < graphSize; i++) {
        if (colors[i] == 0 && !dfs(i, 1)) {
            // 从节点 i 开始递归，发现 i 所在连通块不是二分图
            free(colors);
            return false;
        }
    }

    free(colors);
    return true;
}
```

```go [sol-Go]
func isBipartite(graph [][]int) bool {
    // colors[i] = 0  表示未访问节点 i
    // colors[i] = 1  表示节点 i 为红色
    // colors[i] = -1 表示节点 i 为蓝色
    colors := make([]int8, len(graph))

    var dfs func(int, int8) bool
    dfs = func(x int, c int8) bool {
        colors[x] = c // 节点 x 染成颜色 c
        for _, y := range graph[x] {
            // 邻居 y 的颜色与 x 的相同，说明不是二分图，返回 false
            // 或者继续递归，发现不是二分图，返回 false
            if colors[y] == c ||
                colors[y] == 0 && !dfs(y, -c) { // 取相反数，实现交替染色
                return false
            }
        }
        return true
    }

    // 可能有多个连通块
    for i, c := range colors {
        if c == 0 && !dfs(i, 1) {
            // 从节点 i 开始递归，发现 i 所在连通块不是二分图
            return false
        }
    }
    return true
}
```

```js [sol-JavaScript]
var isBipartite = function(graph) {
    // colors[i] = 0  表示未访问节点 i
    // colors[i] = 1  表示节点 i 为红色
    // colors[i] = -1 表示节点 i 为蓝色
    const colors = Array(graph.length).fill(0);

    function dfs(x, c) {
        colors[x] = c; // 节点 x 染成颜色 c
        for (const y of graph[x]) {
            // 邻居 y 的颜色与 x 的相同，说明不是二分图，返回 false
            // 或者继续递归，发现不是二分图，返回 false
            if (colors[y] === c || 
                colors[y] === 0 && !dfs(y, -c)) { // 取相反数，实现交替染色
                return false;
            }
        }
        return true;
    }

    // 可能有多个连通块
    for (let i = 0; i < colors.length; i++) {
        if (colors[i] === 0 && !dfs(i, 1)) {
            // 从节点 i 开始递归，发现 i 所在连通块不是二分图
            return false;
        }
    }
    return true;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn is_bipartite(graph: Vec<Vec<i32>>) -> bool {
        fn dfs(x: usize, c: i8, graph: &[Vec<i32>], colors: &mut [i8]) -> bool {
            colors[x] = c; // 节点 x 染成颜色 c
            for &y in &graph[x] {
                let y = y as usize;
                // 邻居 y 的颜色与 x 的相同，说明不是二分图，返回 false
                // 或者继续递归，发现不是二分图，返回 false
                if colors[y] == c || 
                   colors[y] == 0 && !dfs(y, -c, graph, colors) { // 取相反数，实现交替染色
                    return false;
                }
            }
            true
        }

        let n = graph.len();
        // colors[i] = 0  表示未访问节点 i
        // colors[i] = 1  表示节点 i 为红色
        // colors[i] = -1 表示节点 i 为蓝色
        let mut colors = vec![0; n];

        // 可能有多个连通块
        for i in 0..n {
            if colors[i] == 0 && !dfs(i, 1, &graph, &mut colors) {
                // 从节点 i 开始递归，发现 i 所在连通块不是二分图
                return false;
            }
        }
        true
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n+m)$，其中 $n$ 是图的节点个数，$m$ 是图的边数，即 $\textit{graph}[i]$ 的长度之和。染色过的节点不会再递归，所以每个节点只会递归访问一次。
- 空间复杂度：$\mathcal{O}(n)$。

## 专题训练

见下面图论题单的「**七、二分图染色**」。

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

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)
