# 207. 课程表

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/course-schedule/
- 题目 slug：`course-schedule`
- 来源专题：图论算法
- 来源分类路径：一、图的遍历 / §1.1 深度优先搜索（DFS）
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/course-schedule/solutions/2992884/san-se-biao-ji-fa-pythonjavacgojsrust-by-pll7/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[DFS 三色标记法（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/course-schedule/solutions/2992884/san-se-biao-ji-fa-pythonjavacgojsrust-by-pll7/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`san-se-biao-ji-fa-pythonjavacgojsrust-by-pll7`
- topic id：`2992884`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 题意

给你一个有向图，判断图中是否有环。

## 核心思路

如果在递归过程中，发现下一个节点在递归栈中（正在访问中），则找到了环。

![lc207.svg](https://pic.leetcode.cn/1782190022-GeUZCu-lc207.svg){:width=300}

如上图，我们 DFS 访问 $0\to 3\to 4\to 5\to 3$。走到 $5$ 的时候，发现下一个节点 $3$ 在递归栈中（正在访问中），那么就找到了环。

> 注：说节点 $x$「正在访问中」，是说我们正在递归处理节点 $x$ 的邻居，$\textit{dfs}(x)$ 尚未结束。

## 具体思路

对于每个节点 $x$，都定义三种颜色值（状态值）：

$0$：节点 $x$ 尚未被访问到。
$1$：节点 $x$ 正在访问中，$\textit{dfs}(x)$ 尚未结束。
$2$：节点 $x$ 已经完全访问完毕。注意这还说明从 $x$ 出发无法找到环。所以当我们遇到状态值为 $2$ 的节点 $x$ 时，无需递归 $x$。

⚠**误区**：不能只用两种状态表示节点「没有访问过」和「访问过」。如上图，我们先 DFS 访问 $0\to 1\to 2$，再访问 $0\to 2$，此时 $0$ 的邻居 $2$ 已经访问过，但这并不能表示此时就找到了环。

算法流程：

1. 建图：把每个 $\textit{prerequisites}[i]=[a,b]$ 看成一条有向边 $b\to a$，构建一个有向图 $g$。
2. 创建长为 $\textit{numCourses}$ 的颜色数组 $\textit{colors}$，所有元素值初始化成 $0$。
3. 遍历 $\textit{colors}$，如果 $\textit{colors}[i]=0$，则调用递归函数 $\textit{dfs}(i)$。
4. 执行 $\textit{dfs}(x)$：
    1. 首先标记 $\textit{colors}[x]=1$，表示节点 $x$ 正在访问中。
    2. 然后遍历 $x$ 的邻居 $y$。如果 $\textit{colors}[y]=1$，则找到环，返回 $\texttt{true}$。如果 $\textit{colors}[y]=0$（没有访问过）且 $\textit{dfs}(y)$ 返回了 $\texttt{true}$，那么 $\textit{dfs}(x)$ 也返回 $\texttt{true}$。
    3. 如果没有找到环，那么先标记 $\textit{colors}[x]=2$，表示 $x$ 已经完全访问完毕，然后返回 $\texttt{false}$。
5. 如果 $\textit{dfs}(i)$ 返回 $\texttt{true}$，那么找到了环，返回 $\texttt{false}$。
6. 如果遍历完所有节点也没有找到环，返回 $\texttt{true}$。

```py [sol-Python3]
class Solution:
    def canFinish(self, numCourses: int, prerequisites: List[List[int]]) -> bool:
        g = [[] for _ in range(numCourses)]
        for a, b in prerequisites:
            g[b].append(a)

        colors = [0] * numCourses
        # 返回 True 表示找到了环
        def dfs(x: int) -> bool:
            colors[x] = 1  # x 正在访问中
            for y in g[x]:
                # 情况一：colors[y] == 1，表示发生循环依赖，找到了环
                # 情况二：colors[y] == 0，没有访问过 y，继续递归 y 获取信息
                # 情况三：colors[y] == 2，重复访问 y 只会重蹈覆辙，和之前一样无法找到环，跳过
                if colors[y] == 1 or colors[y] == 0 and dfs(y):
                    return True  # 找到了环
            colors[x] = 2  # x 完全访问完毕，从 x 出发无法找到环
            return False  # 没有找到环

        for i, c in enumerate(colors):
            if c == 0 and dfs(i):
                return False  # 有环
        return True  # 没有环
```

```java [sol-Java]
class Solution {
    public boolean canFinish(int numCourses, int[][] prerequisites) {
        List<Integer>[] g = new ArrayList[numCourses];
        Arrays.setAll(g, i -> new ArrayList<>());
        for (int[] p : prerequisites) {
            g[p[1]].add(p[0]);
        }

        int[] colors = new int[numCourses];
        for (int i = 0; i < numCourses; i++) {
            if (colors[i] == 0 && dfs(i, g, colors)) {
                return false; // 有环
            }
        }
        return true; // 没有环
    }

    // 返回 true 表示找到了环
    private boolean dfs(int x, List<Integer>[] g, int[] colors) {
        colors[x] = 1; // x 正在访问中
        for (int y : g[x]) {
            // 情况一：colors[y] == 1，表示发生循环依赖，找到了环
            // 情况二：colors[y] == 0，没有访问过 y，继续递归 y 获取信息
            // 情况三：colors[y] == 2，重复访问 y 只会重蹈覆辙，和之前一样无法找到环，跳过
            if (colors[y] == 1 || colors[y] == 0 && dfs(y, g, colors)) {
                return true; // 找到了环
            }
        }
        colors[x] = 2; // x 完全访问完毕，从 x 出发无法找到环
        return false; // 没有找到环
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    bool canFinish(int numCourses, vector<vector<int>>& prerequisites) {
        vector<vector<int>> g(numCourses);
        for (auto& p : prerequisites) {
            g[p[1]].push_back(p[0]);
        }

        vector<int> colors(numCourses);
        // 返回 true 表示找到了环
        auto dfs = [&](this auto&& dfs, int x) -> bool {
            colors[x] = 1; // x 正在访问中
            for (int y : g[x]) {
                // 情况一：colors[y] == 1，表示发生循环依赖，找到了环
                // 情况二：colors[y] == 0，没有访问过 y，继续递归 y 获取信息
                // 情况三：colors[y] == 2，重复访问 y 只会重蹈覆辙，和之前一样无法找到环，跳过
                if (colors[y] == 1 || colors[y] == 0 && dfs(y)) {
                    return true; // 找到了环
                }
            }
            colors[x] = 2; // x 完全访问完毕，从 x 出发无法找到环
            return false; // 没有找到环
        };

        for (int i = 0; i < numCourses; i++) {
            if (colors[i] == 0 && dfs(i)) {
                return false; // 有环
            }
        }
        return true; // 没有环
    }
};
```

```go [sol-Go]
func canFinish(numCourses int, prerequisites [][]int) bool {
    g := make([][]int, numCourses)
    for _, p := range prerequisites {
        g[p[1]] = append(g[p[1]], p[0])
    }

    colors := make([]int, numCourses)
    // 返回 true 表示找到了环
    var dfs func(int) bool
    dfs = func(x int) bool {
        colors[x] = 1 // x 正在访问中
        for _, y := range g[x] {
            // 情况一：colors[y] == 1，表示发生循环依赖，找到了环
            // 情况二：colors[y] == 0，没有访问过 y，继续递归 y 获取信息
            // 情况三：colors[y] == 2，重复访问 y 只会重蹈覆辙，和之前一样无法找到环，跳过
            if colors[y] == 1 || colors[y] == 0 && dfs(y) {
                return true // 找到了环
            }
        }
        colors[x] = 2 // x 完全访问完毕，从 x 出发无法找到环
        return false // 没有找到环
    }

    for i, c := range colors {
        if c == 0 && dfs(i) {
            return false // 有环
        }
    }
    return true // 没有环
}
```

```js [sol-JavaScript]
var canFinish = function(numCourses, prerequisites) {
    const g = Array.from({ length: numCourses }, () => []);
    for (const [a, b] of prerequisites) {
        g[b].push(a);
    }

    const colors = Array(numCourses).fill(0);
    // 返回 true 表示找到了环
    function dfs(x) {
        colors[x] = 1; // x 正在访问中
        for (const y of g[x]) {
            // 情况一：colors[y] === 1，表示发生循环依赖，找到了环
            // 情况二：colors[y] === 0，没有访问过 y，继续递归 y 获取信息
            // 情况三：colors[y] === 2，重复访问 y 只会重蹈覆辙，和之前一样无法找到环，跳过
            if (colors[y] === 1 || colors[y] === 0 && dfs(y)) {
                return true; // 找到了环
            }
        }
        colors[x] = 2; // x 完全访问完毕，从 x 出发无法找到环
        return false; // 没有找到环
    }

    for (let i = 0; i < numCourses; i++) {
        if (colors[i] === 0 && dfs(i)) {
            return false; // 有环
        }
    }
    return true; // 没有环
};
```

```rust [sol-Rust]
impl Solution {
    pub fn can_finish(num_courses: i32, prerequisites: Vec<Vec<i32>>) -> bool {
        let n = num_courses as usize;
        let mut g = vec![vec![]; n];
        for p in prerequisites {
            g[p[1] as usize].push(p[0] as usize);
        }

        let mut colors = vec![0; n];
        for i in 0..n {
            if colors[i] == 0 && Self::dfs(i, &g, &mut colors) {
                return false; // 有环
            }
        }
        true // 没有环
    }

    // 返回 true 表示找到了环
    fn dfs(x: usize, g: &[Vec<usize>], colors: &mut [i32]) -> bool {
        colors[x] = 1; // x 正在访问中
        for &y in &g[x] {
            // 情况一：colors[y] == 1，表示发生循环依赖，找到了环
            // 情况二：colors[y] == 0，没有访问过 y，继续递归 y 获取信息
            // 情况三：colors[y] == 2，重复访问 y 只会重蹈覆辙，和之前一样无法找到环，跳过
            if colors[y] == 1 || colors[y] == 0 && Self::dfs(y, g, colors) {
                return true; // 找到了环
            }
        }
        colors[x] = 2; // x 完全访问完毕，从 x 出发无法找到环
        false // 没有找到环
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n+m)$，其中 $n$ 是 $\textit{numCourses}$，$m$ 是 $\textit{prerequisites}$ 的长度。每个节点至多递归访问一次，每条边至多遍历一次。
- 空间复杂度：$\mathcal{O}(n+m)$。存储 $g$ 需要 $\mathcal{O}(n+m)$ 的空间。

## 相似题目

- [802. 找到最终的安全状态](https://leetcode.cn/problems/find-eventual-safe-states/)

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

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

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
