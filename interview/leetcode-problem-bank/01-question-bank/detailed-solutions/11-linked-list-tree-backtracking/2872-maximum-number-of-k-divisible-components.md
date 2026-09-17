# 2872. 可以被 K 整除连通块的最大数目

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-number-of-k-divisible-components/
- 题目 slug：`maximum-number-of-k-divisible-components`
- 来源专题：链表、树与回溯
- 来源分类路径：三、一般树 / §3.3 自底向上 DFS
- 难度分：1968
- 外部题解来源：https://leetcode.cn/problems/maximum-number-of-k-divisible-components/solutions/2464687/pan-duan-zi-shu-dian-quan-he-shi-fou-wei-uvsg/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[判断子树点权和是否为 k 的倍数（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/maximum-number-of-k-divisible-components/solutions/2464687/pan-duan-zi-shu-dian-quan-he-shi-fou-wei-uvsg/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`pan-duan-zi-shu-dian-quan-he-shi-fou-wei-uvsg`
- topic id：`2464687`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:49:15 +0800

最大化连通块的数目，等价于最大化删除的边数加一。

## 什么样的边可以删除？

如果 $x$ 和 $y$ 都是 $k$ 的倍数，那么 $x+y$ 也是 $k$ 的倍数。比如 $3$ 和 $6$ 都是 $3$ 的倍数，那么 $3+6=9$ 也是 $3$ 的倍数。

反过来说（逆否命题），如果 $x+y$ 不是 $k$ 的倍数，那么 $x$ 和 $y$ 不全是 $k$ 的倍数。不是 $k$ 的倍数的数，继续拆分，始终存在一个不是 $k$ 的倍数的数。

对应到删边上，删除一条边后，我们把一个连通块分成了两个连通块。如果其中一个连通块的点权和不是 $k$ 的倍数，那么这个连通块无论如何分割，始终存在一个点权和不是 $k$ 的倍数的连通块。所以当且仅当这两个连通块的点权和都是 $k$ 的倍数，这条边才能删除。

删除后，由于分割出的连通块点权和仍然是 $k$ 的倍数，所以可以继续分割，直到无法分割为止。换句话说，只要有能删除的边，就删除。

## 如何找到可以删除的边？

删除一条边后，我们把一个连通块分成了两个连通块。由于题目保证整棵树的点权和是 $k$ 的倍数，所以只需看其中一个连通块的点权和是否为 $k$ 的倍数。

从任意点出发 DFS 这棵树。计算子树 $x$ 的点权和 $s$，如果 $s$ 是 $k$ 的倍数，那么可以删除 $x$ 到其父节点这条边。注意根节点没有父节点。

连通块的数目等于删除的边数加一。可以把根节点到其父节点这条边（虽然不存在）也算上，这样答案就是删除的边数。

```py [sol-Python3]
class Solution:
    def maxKDivisibleComponents(self, n: int, edges: List[List[int]], values: List[int], k: int) -> int:
        g = [[] for _ in range(n)]
        for x, y in edges:
            g[x].append(y)
            g[y].append(x)

        # 返回子树 x 的点权和
        def dfs(x: int, fa: int) -> int:
            s = values[x]
            for y in g[x]:
                if y != fa:  # 避免访问父节点
                    # 加上子树 y 的点权和，得到子树 x 的点权和
                    s += dfs(y, x)  
            nonlocal ans
            ans += s % k == 0
            return s

        ans = 0
        dfs(0, -1)
        return ans
```

```java [sol-Java]
class Solution {
    private int ans;

    public int maxKDivisibleComponents(int n, int[][] edges, int[] values, int k) {
        List<Integer>[] g = new ArrayList[n];
        Arrays.setAll(g, _ -> new ArrayList<>());
        for (int[] e : edges) {
            int x = e[0];
            int y = e[1];
            g[x].add(y);
            g[y].add(x);
        }

        dfs(0, -1, g, values, k);
        return ans;
    }

    // 返回子树 x 的点权和
    private long dfs(int x, int fa, List<Integer>[] g, int[] values, int k) {
        long sum = values[x];
        for (int y : g[x]) {
            if (y != fa) { // 避免访问父节点
                // 加上子树 y 的点权和，得到子树 x 的点权和
                sum += dfs(y, x, g, values, k);
            }
        }
        ans += sum % k == 0 ? 1 : 0;
        return sum;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxKDivisibleComponents(int n, vector<vector<int>>& edges, vector<int>& values, int k) {
        vector<vector<int>> g(n);
        for (auto& e : edges) {
            int x = e[0], y = e[1];
            g[x].push_back(y);
            g[y].push_back(x);
        }

        int ans = 0;

        // 返回子树 x 的点权和
        auto dfs = [&](this auto&& dfs, int x, int fa) -> long long {
            long long sum = values[x];
            for (int y : g[x]) {
                if (y != fa) { // 避免访问父节点
                    // 加上子树 y 的点权和，得到子树 x 的点权和
                    sum += dfs(y, x);
                }
            }
            ans += sum % k == 0;
            return sum;
        };

        dfs(0, -1);
        return ans;
    }
};
```

```go [sol-Go]
func maxKDivisibleComponents(n int, edges [][]int, values []int, k int) (ans int) {
	g := make([][]int, n)
	for _, e := range edges {
		x, y := e[0], e[1]
		g[x] = append(g[x], y)
		g[y] = append(g[y], x)
	}

	// 返回子树 x 的点权和
	var dfs func(int, int) int
	dfs = func(x, fa int) int {
		s := values[x]
		for _, y := range g[x] {
			if y != fa { // 避免访问父节点
				// 加上子树 y 的点权和，得到子树 x 的点权和
				s += dfs(y, x)
			}
		}
		if s%k == 0 {
			ans++
		}
		return s
	}

	dfs(0, -1)
	return
}
```

```js [sol-JavaScript]
var maxKDivisibleComponents = function(n, edges, values, k) {
    const g = Array.from({ length: n }, () => []);
    for (const [x, y] of edges) {
        g[x].push(y);
        g[y].push(x);
    }

    let ans = 0;

    // 返回子树 x 的点权和
    function dfs(x, fa) {
        let sum = values[x];
        for (const y of g[x]) {
            if (y !== fa) { // 避免访问父节点
                // 加上子树 y 的点权和，得到子树 x 的点权和
                sum += dfs(y, x);
            }
        }
        ans += sum % k === 0 ? 1 : 0;
        return sum;
    }

    dfs(0, -1);
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn max_k_divisible_components(n: i32, edges: Vec<Vec<i32>>, values: Vec<i32>, k: i32) -> i32 {
        let n = n as usize;
        let mut g = vec![vec![]; n];
        for e in edges {
            let x = e[0] as usize;
            let y = e[1] as usize;
            g[x].push(y);
            g[y].push(x);
        }

        // 返回子树 x 的点权和
        fn dfs(x: usize, fa: usize, g: &[Vec<usize>], values: &[i32], k: i64, ans: &mut i32) -> i64 {
            let mut sum = values[x] as i64;
            for &y in &g[x] {
                if y != fa { // 避免访问父节点
                    // 加上子树 y 的点权和，得到子树 x 的点权和
                    sum += dfs(y, x, g, values, k, ans);
                }
            }
            if sum % k == 0 {
                *ans += 1;
            }
            sum
        }

        let mut ans = 0;
        dfs(0, 0, &g, &values, k as i64, &mut ans);
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$。
- 空间复杂度：$\mathcal{O}(n)$。

## 相似题目

[2440. 创建价值相同的连通块](https://leetcode.cn/problems/create-components-with-same-value/)

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
