# LCP 71. 集水器

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/kskhHQ/
- 题目 slug：`kskhHQ`
- 来源专题：常用数据结构
- 来源分类路径：七、并查集 / §7.2 进阶
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/kskhHQ/solutions/1876216/jiantu-by-endlesscheng-rkra/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[建图技巧 + 并查集（Python/Java/C++/Go）](https://leetcode.cn/problems/kskhHQ/solutions/1876216/jiantu-by-endlesscheng-rkra/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`jiantu-by-endlesscheng-rkra`
- topic id：`1876216`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

[视频讲解](https://www.bilibili.com/video/BV1rT411P7NA) 已出炉，欢迎点赞三连，在评论区分享你对这场力扣杯的看法~

记录关键思路，详细的说明见视频讲解。

1. 判断能否容纳水：第 $i$ 行的水，在不走到第 $i-1$ 行的前提下，无法触及左、右、下边界。
2. 转换成连通性问题，用并查集处理。
3. 为了方便判断是否触及边界，还需要在左、右、下各加一条格子。
4. 根据 1，我们可以从最后一行往上计算。
5. 用并查集合并当前行各个区域，以及边界。
6. 如果合并完了，枚举当前行的各个区域，如果不和边界相连，那么该区域能容纳水。
7. 边界、最上面一层和超级汇点 $0$ 相连。
8. 最后判断的时候，如果该区域能容纳水，且和超级汇点相连，那么不是闭合区域，就真的有水。
9. 统计真的有水的格子，即为答案。
10. 代码实现时，把每个格子划分四个区域，相对两个区域，代码写起来要容易一些。

```py [sol1-Python3]
class Solution:
    def reservoir(self, shape: List[str]) -> int:
        n, m = len(shape), len(shape[0])
        # 每个格子分成四个区域（上下左右），标上序号，方便用并查集连通
        # 假设左右下还有一圈格子，直接连到超级汇点 0
        u = [[0] * (m + 2) for _ in range(n + 1)]
        d = [[0] * (m + 2) for _ in range(n + 1)]
        l = [[0] * (m + 2) for _ in range(n + 1)]
        r = [[0] * (m + 2) for _ in range(n + 1)]
        c = 1
        for i in range(n):
            for j in range(1, m + 1):  # 假设格子的列号从 1 开始，这样方便表示左右边界
                u[i][j] = c; c += 1
                d[i][j] = c; c += 1
                l[i][j] = c; c += 1
                r[i][j] = c; c += 1

        # 并查集模板
        fa = list(range(c))
        def find(x: int) -> int:
            if fa[x] != x:
                fa[x] = find(fa[x])
            return fa[x]
        def merge(x: int, y: int):
            fa[find(x)] = find(y)

        ok = [False] * c  # 能否容纳水
        # 倒着判断每一行，寻找可能有水的区域
        for i in range(n - 1, -1, -1):
            for j in range(m + 1):
                merge(r[i][j], l[i][j + 1])  # 连通左右
            for j, type in enumerate(shape[i], 1):
                merge(d[i][j], u[i + 1][j])  # 连通下
                # 根据格子的类型连接格子内部四个区域
                if type == '.':
                    merge(l[i][j], u[i][j])
                    merge(l[i][j], d[i][j])
                    merge(l[i][j], r[i][j])
                elif type == 'l':
                    merge(l[i][j], d[i][j])
                    merge(r[i][j], u[i][j])
                else:
                    merge(l[i][j], u[i][j])
                    merge(r[i][j], d[i][j])
            for j in range(1, m + 1):
                # 在没有连接第 i-1 行的情况下，无法到达左右下边界 => 能容纳水
                ok[l[i][j]] = find(l[i][j]) != find(0)
                ok[r[i][j]] = find(r[i][j]) != find(0)
                ok[u[i][j]] = find(u[i][j]) != find(0)
                ok[d[i][j]] = find(d[i][j]) != find(0)

        # 第一行连上超级汇点，方便后面统一判断是否在闭合区域里面
        for j in range(1, m + 1):
            merge(u[0][j], 0)

        ans = 0
        for i, b in enumerate(ok):
            if b and find(i) == find(0):  # 能容纳水，且不在闭合区域里面
                ans += 1
        return ans // 2
```

```java [sol1-Java]
class Solution {
    private int[] fa;

    public int reservoir(String[] shape) {
        int n = shape.length, m = shape[0].length(), c = 1;
        // 每个格子分成四个区域（上下左右），标上序号，方便用并查集连通
        // 假设左右下还有一圈格子，直接连到超级汇点 0
        int[][] u = new int[n + 1][m + 2], d = new int[n + 1][m + 2], l = new int[n + 1][m + 2], r = new int[n + 1][m + 2];
        for (var i = 0; i < n; ++i)
            for (var j = 1; j <= m; ++j) { // 假设格子的列号从 1 开始，这样方便表示左右边界
                u[i][j] = c++;
                d[i][j] = c++;
                l[i][j] = c++;
                r[i][j] = c++;
            }

        fa = new int[c];
        for (var i = 0; i < c; i++) fa[i] = i;

        var ok = new boolean[c]; // 能否容纳水
        // 倒着判断每一行，寻找可能有水的区域
        for (var i = n - 1; i >= 0; --i) {
            for (var j = 0; j <= m; j++)
                merge(r[i][j], l[i][j + 1]); // 连通左右
            for (var j = 1; j <= m; j++) {
                merge(d[i][j], u[i + 1][j]); // 连通下
                // 根据格子的类型连接格子内部四个区域
                var type = shape[i].charAt(j - 1);
                if (type == '.') {
                    merge(l[i][j], u[i][j]);
                    merge(l[i][j], d[i][j]);
                    merge(l[i][j], r[i][j]);
                } else if (type == 'l') {
                    merge(l[i][j], d[i][j]);
                    merge(r[i][j], u[i][j]);
                } else {
                    merge(l[i][j], u[i][j]);
                    merge(r[i][j], d[i][j]);
                }
            }
            for (var j = 1; j <= m; j++) {
                // 在没有连接第 i-1 行的情况下，无法到达左右下边界 => 能容纳水
                ok[l[i][j]] = find(l[i][j]) != find(0);
                ok[r[i][j]] = find(r[i][j]) != find(0);
                ok[u[i][j]] = find(u[i][j]) != find(0);
                ok[d[i][j]] = find(d[i][j]) != find(0);
            }
        }

        // 第一行连上超级汇点，方便后面统一判断是否在闭合区域里面
        for (var j = 1; j <= m; j++)
            merge(u[0][j], 0);

        var ans = 0;
        for (var i = 0; i < c; i++)
            if (ok[i] && find(i) == find(0))
                ++ans; // 能容纳水，且不在闭合区域里面
        return ans / 2;
    }

    private int find(int x) {
        if (fa[x] != x) fa[x] = find(fa[x]);
        return fa[x];
    }

    private void merge(int x, int y) {
        fa[find(x)] = find(y);
    }
}
```

```cpp [sol1-C++]
class Solution {
public:
    int reservoir(vector<string> &shape) {
        int n = shape.size(), m = shape[0].size(), c = 1;
        // 每个格子分成四个区域（上下左右），标上序号，方便用并查集连通
        // 假设左右下还有一圈格子，直接连到超级汇点 0
        int u[n + 1][m + 2], d[n + 1][m + 2], l[n + 1][m + 2], r[n + 1][m + 2];
        memset(u, 0, sizeof(u)); memset(d, 0, sizeof(d)); memset(l, 0, sizeof(l)); memset(r, 0, sizeof(r));
        for (int i = 0; i < n; ++i)
            for (int j = 1; j <= m; ++j) // 假设格子的列号从 1 开始，这样方便表示左右边界
                u[i][j] = c++, d[i][j] = c++, l[i][j] = c++, r[i][j] = c++;

        // 并查集模板
        int fa[c];
        iota(fa, fa + c, 0);
        function<int(int)> find = [&](int x) -> int { return fa[x] == x ? x : fa[x] = find(fa[x]); };
        auto merge = [&](int x, int y) { fa[find(x)] = find(y); };

        bool ok[c]; // 能否容纳水
        memset(ok, 0, sizeof(ok));
        // 倒着判断每一行，寻找可能有水的区域
        for (int i = n - 1; i >= 0; --i) {
            for (int j = 0; j <= m; j++) merge(r[i][j], l[i][j + 1]); // 连通左右
            for (int j = 1; j <= m; j++) {
                merge(d[i][j], u[i + 1][j]); // 连通下
                // 根据格子的类型连接格子内部四个区域
                if (shape[i][j - 1] == '.') merge(l[i][j], u[i][j]), merge(l[i][j], d[i][j]), merge(l[i][j], r[i][j]);
                else if (shape[i][j - 1] == 'l') merge(l[i][j], d[i][j]), merge(r[i][j], u[i][j]);
                else merge(l[i][j], u[i][j]), merge(r[i][j], d[i][j]);
            }
            for (int j = 1; j <= m; j++) {
                // 在没有连接第 i-1 行的情况下，无法到达左右下边界 => 能容纳水
                ok[l[i][j]] = find(l[i][j]) != find(0);
                ok[r[i][j]] = find(r[i][j]) != find(0);
                ok[u[i][j]] = find(u[i][j]) != find(0);
                ok[d[i][j]] = find(d[i][j]) != find(0);
            }
        }

        // 第一行连上超级汇点，方便后面统一判断是否在闭合区域里面
        for (int j = 1; j <= m; j++) merge(u[0][j], 0);

        int ans = 0;
        for (int i = 0; i < c; i++)
            ans += ok[i] && find(i) == find(0); // 能容纳水，且不在闭合区域里面
        return ans / 2;
    }
};
```

```go [sol1-Go]
func reservoir(shape []string) int {
	n, m := len(shape), len(shape[0])
	// 每个格子分成四个区域（上下左右），标上序号，方便用并查集连通
	// 假设左右下还有一圈格子，直接连到超级汇点 0
	u := make([][]int, n+1)
	d := make([][]int, n+1)
	l := make([][]int, n+1)
	r := make([][]int, n+1)
	for i := range u {
		u[i] = make([]int, m+2)
		d[i] = make([]int, m+2)
		l[i] = make([]int, m+2)
		r[i] = make([]int, m+2)
	}
	c := 1
	for i := 0; i < n; i++ {
		for j := 1; j <= m; j++ { // 假设格子的列号从 1 开始，这样方便表示左右边界
			u[i][j] = c; c++
			d[i][j] = c; c++
			l[i][j] = c; c++
			r[i][j] = c; c++
		}
	}

	// 并查集模板
	fa := make([]int, c)
	for i := range fa {
		fa[i] = i
	}
	var find func(int) int
	find = func(x int) int {
		if fa[x] != x {
			fa[x] = find(fa[x])
		}
		return fa[x]
	}
	merge := func(x, y int) { fa[find(x)] = find(y) }

	ok := make([]bool, c) // 能否容纳水
	// 倒着判断每一行，寻找可能有水的区域
	for i := n - 1; i >= 0; i-- {
		for j := 0; j <= m; j++ {
			merge(r[i][j], l[i][j+1]) // 连通左右
		}
		for j := 1; j <= m; j++ {
			merge(d[i][j], u[i+1][j]) // 连通下
			// 根据格子的类型连接格子内部四个区域
			switch shape[i][j-1] {
			case '.':
				merge(l[i][j], u[i][j])
				merge(l[i][j], d[i][j])
				merge(l[i][j], r[i][j])
			case 'l':
				merge(l[i][j], d[i][j])
				merge(r[i][j], u[i][j])
			default:
				merge(l[i][j], u[i][j])
				merge(r[i][j], d[i][j])
			}
		}
		for j := 1; j <= m; j++ {
			// 在没有连接第 i-1 行的情况下，无法到达左右下边界 => 能容纳水
			ok[l[i][j]] = find(l[i][j]) != find(0)
			ok[r[i][j]] = find(r[i][j]) != find(0)
			ok[u[i][j]] = find(u[i][j]) != find(0)
			ok[d[i][j]] = find(d[i][j]) != find(0)
		}
	}

	// 第一行连上超级汇点，方便后面统一判断是否在闭合区域里面
	for j := 1; j <= m; j++ {
		merge(u[0][j], 0)
	}

	ans := 0
	for i, b := range ok {
		if b && find(i) == find(0) { // 能容纳水，且不在闭合区域里面
			ans++
		}
	}
	return ans / 2
}
```

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口（定长/不定长/多指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心算法（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `七、并查集 / §7.2 进阶`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `七、并查集 / §7.2 进阶`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
