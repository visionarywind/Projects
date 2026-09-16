# 2603. 收集树中金币

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/collect-coins-in-a-tree/
- 题目 slug：`collect-coins-in-a-tree`
- 来源专题：图论算法
- 来源分类路径：二、拓扑排序 / §2.1 拓扑排序
- 难度分：2712
- 外部题解来源：https://leetcode.cn/problems/collect-coins-in-a-tree/solutions/2191371/tuo-bu-pai-xu-ji-lu-ru-dui-shi-jian-pyth-6uli/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[拓扑排序（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/collect-coins-in-a-tree/solutions/2191371/tuo-bu-pai-xu-ji-lu-ru-dui-shi-jian-pyth-6uli/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tuo-bu-pai-xu-ji-lu-ru-dui-shi-jian-pyth-6uli`
- topic id：`2191371`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 提示 1

定义一个点的**度数**为其邻居个数。如果一个点的度数为 $1$，那么这个点叫做**叶子节点**，例如示例 2 的 $3,4,6,7$ 都是叶子节点。

如果叶子节点没有金币，我们有必要移动到叶子节点吗？没有必要。

那么可以先把这些没有金币的叶子节点去掉。如果去掉后又产生了新的没有金币的叶子节点，就继续去掉。

怎么实现？**拓扑排序**。一开始，把没有金币的叶子节点都加到队列中。然后不断循环直到队列为空。每次循环，弹出队首的节点 $x$，并删除 $x$ 及其邻居之间的边。我们并不需要实际删除边，只需要把邻居的度数减少 $1$。如果一个邻居的度数减少为 $1$ 且没有金币，就加到队列中，继续拓扑排序。

## 提示 2

看示例 2，在去掉节点 $6$ 之后，现在每个叶子节点上都有金币。

由于可以「收集距离当前节点距离为 $2$ 以内的所有金币」，我们没有必要移动到叶子节点再收集，而是移动到叶子节点的父节点的父节点，就能收集到叶子节点上的金币。

那么，去掉所有叶子，然后再去掉新产生的叶子，剩余节点就是必须要访问的节点。

## 提示 3

由于题目要求最后回到出发点，无论从哪个点出发，每条边都必须走两次。这是因为把出发点作为树根，递归遍历这棵树，那么往下「递」是一次，往上「归」又是一次，每条边都会经过两次。

所以答案就是剩余边数乘 $2$。当我们删除节点时，也可以看成是删除这个点到其父节点的边。

特别地，如果所有点都要被删除，那么当剩下两个点时，这两个点之间的边我们会删除两次，这会导致剩余边数等于 $-1$，而此时答案应该是 $0$。所以最后答案要和 $0$ 取最大值。

**代码实现时，由于我们不需要得到一个严格的拓扑序，所以简单地用栈或者数组代替队列，也是可以的。**

```py [sol-Python3]
class Solution:
    def collectTheCoins(self, coins: List[int], edges: List[List[int]]) -> int:
        n = len(coins)
        g = [[] for _ in range(n)]
        for x, y in edges:
            g[x].append(y)
            g[y].append(x)  # 建图
        deg = list(map(len, g))  # 每个节点的度数（邻居个数）

        left_edges = n - 1  # 剩余边数
        # 拓扑排序，去掉没有金币的子树
        q = []
        for i, (d, c) in enumerate(zip(deg, coins)):
            if d == 1 and c == 0:  # 没有金币的叶子
                q.append(i)
        while q:
            left_edges -= 1  # 删除节点到其父节点的边
            for y in g[q.pop()]:
                deg[y] -= 1
                if deg[y] == 1 and coins[y] == 0:  # 没有金币的叶子
                    q.append(y)

        # 再次拓扑排序
        for i, (d, c) in enumerate(zip(deg, coins)):
            if d == 1 and c:  # 有金币的叶子（判断 c 是避免把没有金币的叶子也算进来）
                q.append(i)
        left_edges -= len(q)  # 删除所有叶子（到其父节点的边）
        for x in q:  # 遍历所有叶子
            for y in g[x]:
                deg[y] -= 1
                if deg[y] == 1:  # y 现在是叶子了
                    left_edges -= 1  # 删除 y（到其父节点的边）
        return max(left_edges * 2, 0)
```

```java [sol-Java]
class Solution {
    public int collectTheCoins(int[] coins, int[][] edges) {
        int n = coins.length;
        List<Integer>[] g = new ArrayList[n];
        Arrays.setAll(g, e -> new ArrayList<>());
        int[] deg = new int[n];
        for (int[] e : edges) {
            int x = e[0], y = e[1];
            g[x].add(y);
            g[y].add(x); // 建图
            deg[x]++;
            deg[y]++; // 统计每个节点的度数（邻居个数）
        }

        int leftEdges = n - 1; // 剩余边数
        // 拓扑排序，去掉没有金币的子树
        Queue<Integer> q = new ArrayDeque<>();
        for (int i = 0; i < n; i++) {
            if (deg[i] == 1 && coins[i] == 0) { // 没有金币的叶子
                q.add(i);
            }
        }
        while (!q.isEmpty()) {
            leftEdges--; // 删除节点到其父节点的边
            for (int y : g[q.poll()]) {
                if (--deg[y] == 1 && coins[y] == 0) { // 没有金币的叶子
                    q.add(y);
                }
            }
        }

        // 再次拓扑排序
        for (int i = 0; i < n; i++) {
            if (deg[i] == 1 && coins[i] == 1) { // 有金币的叶子（判断 coins[i] 是避免把没有金币的叶子也算进来）
                q.add(i);
            }
        }
        leftEdges -= q.size(); // 删除所有叶子（到其父节点的边）
        for (int x : q) { // 遍历所有叶子
            for (int y : g[x]) {
                if (--deg[y] == 1) { // y 现在是叶子了
                    leftEdges--; // 删除 y（到其父节点的边）
                }
            }
        }
        return Math.max(leftEdges * 2, 0);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int collectTheCoins(vector<int>& coins, vector<vector<int>>& edges) {
        int n = coins.size();
        vector<vector<int>> g(n);
        vector<int> deg(n);
        for (auto& e : edges) {
            int x = e[0], y = e[1];
            g[x].push_back(y);
            g[y].push_back(x); // 建图
            deg[x]++;
            deg[y]++; // 统计每个节点的度数（邻居个数）
        }

        int left_edges = n - 1; // 剩余边数
        // 拓扑排序，去掉没有金币的子树
        vector<int> q;
        for (int i = 0; i < n; i++) {
            if (deg[i] == 1 && coins[i] == 0) { // 没有金币的叶子
                q.push_back(i);
            }
        }
        while (!q.empty()) {
            left_edges--; // 删除节点 x（到其父节点的边）
            int x = q.back(); q.pop_back();
            for (int y : g[x]) {
                if (--deg[y] == 1 && coins[y] == 0) { // 没有金币的叶子
                    q.push_back(y);
                }
            }
        }

        // 再次拓扑排序
        for (int i = 0; i < n; i++) {
            if (deg[i] == 1 && coins[i]) { // 有金币的叶子（判断 coins[i] 是避免把没有金币的叶子也算进来）
                q.push_back(i);
            }
        }
        left_edges -= q.size(); // 删除所有叶子（到其父节点的边）
        for (int x : q) { // 遍历所有叶子
            for (int y : g[x]) {
                if (--deg[y] == 1) { // y 现在是叶子了
                    left_edges--; // 删除 y（到其父节点的边）
                }
            }
        }
        return max(left_edges * 2, 0);
    }
};
```

```go [sol-Go]
func collectTheCoins(coins []int, edges [][]int) int {
	n := len(coins)
	g := make([][]int, n)
	deg := make([]int, n)
	for _, e := range edges {
		x, y := e[0], e[1]
		g[x] = append(g[x], y)
		g[y] = append(g[y], x) // 建图
		deg[x]++
		deg[y]++ // 统计每个节点的度数（邻居个数）
	}

	leftEdges := n - 1 // 剩余边数
	// 拓扑排序，去掉没有金币的子树
	q := []int{}
	for i, d := range deg {
		if d == 1 && coins[i] == 0 { // 没有金币的叶子
			q = append(q, i)
		}
	}
	for len(q) > 0 {
		x := q[len(q)-1]
		q = q[:len(q)-1]
		leftEdges-- // 删除节点 x 到其父节点的边
		for _, y := range g[x] {
			deg[y]--
			if deg[y] == 1 && coins[y] == 0 { // 没有金币的叶子
				q = append(q, y)
			}
		}
	}

	// 再次拓扑排序
	for i, d := range deg {
		if d == 1 && coins[i] > 0 { // 有金币的叶子（判断 coins[i] 是避免把没有金币的叶子也算进来）
			q = append(q, i)
		}
	}
	leftEdges -= len(q) // 删除所有叶子（到其父节点的边）
	for _, x := range q { // 遍历所有叶子
		for _, y := range g[x] {
			deg[y]--
			if deg[y] == 1 { // y 现在是叶子了
				leftEdges-- // 删除 y（到其父节点的边）
			}
		}
	}
	return max(leftEdges*2, 0)
}

func max(a, b int) int { if b > a { return b }; return a }
```

```js [sol-JavaScript]
var collectTheCoins = function(coins, edges) {
    const n = coins.length;
    const g = Array(n).fill(null).map(() => []);
    for (const [x, y] of edges) {
        g[x].push(y);
        g[y].push(x); // 建图
    }
    const deg = g.map((neighbors) => neighbors.length); // 每个节点的度数（邻居个数）

    let leftEdges = n - 1; // 剩余边数
    // 拓扑排序，去掉没有金币的子树
    const q = [];
    for (let i = 0; i < n; i++) {
        if (deg[i] === 1 && coins[i] === 0) { // 没有金币的叶子
            q.push(i);
        }
    }
    while (q.length) {
        leftEdges--; // 删除节点到其父节点的边
        for (const x of g[q.pop()]) {
            if (--deg[x] === 1 && coins[x] === 0) { // 没有金币的叶子
                q.push(x);
            }
        }
    }

    // 再次拓扑排序
    for (let i = 0; i < n; i++) {
        if (deg[i] === 1 && coins[i]) { // 有金币的叶子（判断 coins[i] 是避免把没有金币的叶子也算进来）
            q.push(i);
        }
    }
    leftEdges -= q.length; // 删除所有叶子（到其父节点的边）
    for (const x of q) { // 遍历所有叶子
        for (const y of g[x]) {
            if (--deg[y] === 1) { // y 现在是叶子了
                leftEdges--; // 删除 y（到其父节点的边）
            }
        }
    }
    return Math.max(leftEdges * 2, 0);
};
```

```rust [sol-Rust]
impl Solution {
    pub fn collect_the_coins(coins: Vec<i32>, edges: Vec<Vec<i32>>) -> i32 {
        let n = coins.len();
        let mut g = vec![vec![]; n];
        let mut deg = vec![0; n];
        for e in &edges {
            let x = e[0] as usize;
            let y = e[1] as usize;
            g[x].push(y);
            g[y].push(x); // 建图
            deg[x] += 1;
            deg[y] += 1; // 统计每个节点的度数（邻居个数）
        }

        let mut left_edges = n as i32 - 1; // 剩余边数
        // 拓扑排序，去掉没有金币的子树
        let mut q = Vec::new();
        for i in 0..n {
            if deg[i] == 1 && coins[i] == 0 { // 没有金币的叶子
                q.push(i);
            }
        }
        while !q.is_empty() {
            left_edges -= 1; // 删除节点到其父节点的边
            for &y in &g[q.pop().unwrap()] {
                deg[y] -= 1;
                if deg[y] == 1 && coins[y] == 0 { // 没有金币的叶子
                    q.push(y);
                }
            }
        }

        // 再次拓扑排序
        for i in 0..n {
            if deg[i] == 1 && coins[i] == 1 { // 有金币的叶子（判断 coins[i] 是避免把没有金币的叶子也算进来）
                q.push(i);
            }
        }
        left_edges -= q.len() as i32; // 删除所有叶子（到其父节点的边）
        for &x in &q { // 遍历所有叶子
            for &y in &g[x] {
                deg[y] -= 1;
                if deg[y] == 1 { // y 现在是叶子了
                    left_edges -= 1; // 删除 y（到其父节点的边）
                }
            }
        }
        0.max(left_edges * 2)
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{coins}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

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

本题来自 `二、拓扑排序 / §2.1 拓扑排序`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、拓扑排序 / §2.1 拓扑排序`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
