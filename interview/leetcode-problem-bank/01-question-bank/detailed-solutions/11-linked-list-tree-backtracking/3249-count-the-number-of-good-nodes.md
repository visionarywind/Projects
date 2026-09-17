# 3249. 统计好节点的数目

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/count-the-number-of-good-nodes/
- 题目 slug：`count-the-number-of-good-nodes`
- 来源专题：链表、树与回溯
- 来源分类路径：三、一般树 / §3.3 自底向上 DFS
- 难度分：1566
- 外部题解来源：https://leetcode.cn/problems/count-the-number-of-good-nodes/solutions/2876198/dfs-ji-suan-zi-shu-da-xiao-pythonjavacgo-9atl/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[DFS 计算子树大小，附相似题目（Python/Java/C++/Go）](https://leetcode.cn/problems/count-the-number-of-good-nodes/solutions/2876198/dfs-ji-suan-zi-shu-da-xiao-pythonjavacgo-9atl/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`dfs-ji-suan-zi-shu-da-xiao-pythonjavacgo-9atl`
- topic id：`2876198`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:49:15 +0800

建树，然后从根节点 $0$ 开始 DFS 这棵树。

DFS 返回子树大小。

对于节点 $x$，如果其是叶子节点，或者其所有儿子子树大小都一样，那么答案加一。

具体请看 [视频讲解](https://www.bilibili.com/video/BV1Cf421v7Ky/) 第二题，欢迎点赞关注~

```py [sol-Python3]
class Solution:
    def countGoodNodes(self, edges: List[List[int]]) -> int:
        n = len(edges) + 1
        g = [[] for _ in range(n)]
        for x, y in edges:
            g[x].append(y)
            g[y].append(x)

        ans = 0
        def dfs(x: int, fa: int) -> int:
            size, sz0, ok = 1, 0, True
            for y in g[x]:
                if y == fa:
                    continue  # 不能递归到父节点
                sz = dfs(y, x)
                if sz0 == 0:
                    sz0 = sz  # 记录第一个儿子子树的大小
                elif sz != sz0:  # 存在大小不一样的儿子子树
                    ok = False  # 注意不能 break，其他子树 y 仍然要递归
                size += sz
            nonlocal ans
            ans += ok
            return size
        dfs(0, -1)
        return ans
```

```java [sol-Java]
class Solution {
    public int countGoodNodes(int[][] edges) {
        int n = edges.length + 1;
        List<Integer>[] g = new ArrayList[n];
        Arrays.setAll(g, i -> new ArrayList<>());
        for (int[] e : edges) {
            int x = e[0];
            int y = e[1];
            g[x].add(y);
            g[y].add(x);
        }
        dfs(0, -1, g);
        return ans;
    }

    private int ans;

    private int dfs(int x, int fa, List<Integer>[] g) {
        int size = 1;
        int sz0 = 0;
        boolean ok = true;
        for (int y : g[x]) {
            if (y == fa) {
                continue; // 不能递归到父节点
            }
            int sz = dfs(y, x, g);
            if (sz0 == 0) {
                sz0 = sz; // 记录第一个儿子子树的大小
            } else if (sz != sz0) { // 存在大小不一样的儿子子树
                ok = false; // 注意不能 break，其他子树 y 仍然要递归
            }
            size += sz;
        }
        ans += ok ? 1 : 0;
        return size;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int countGoodNodes(vector<vector<int>>& edges) {
        int n = edges.size() + 1;
        vector<vector<int>> g(n);
        for (auto& e : edges) {
            int x = e[0], y = e[1];
            g[x].push_back(y);
            g[y].push_back(x);
        }

        int ans = 0;
        auto dfs = [&](auto&& dfs, int x, int fa) -> int {
            int size = 1, sz0 = 0;
            bool ok = true;
            for (int y : g[x]) {
                if (y == fa) {
                    continue; // 不能递归到父节点
                }
                int sz = dfs(dfs, y, x);
                if (sz0 == 0) {
                    sz0 = sz; // 记录第一个儿子子树的大小
                } else if (sz != sz0) { // 存在大小不一样的儿子子树
                    ok = false; // 注意不能 break，其他子树 y 仍然要递归
                }
                size += sz;
            }
            ans += ok;
            return size;
        };
        dfs(dfs, 0, -1);
        return ans;
    }
};
```

```go [sol-Go]
func countGoodNodes(edges [][]int) (ans int) {
	n := len(edges) + 1
	g := make([][]int, n)
	for _, e := range edges {
		x, y := e[0], e[1]
		g[x] = append(g[x], y)
		g[y] = append(g[y], x)
	}

	var dfs func(int, int) int
	dfs = func(x, fa int) int {
		size, sz0, ok := 1, 0, true
		for _, y := range g[x] {
			if y == fa {
				continue
			}
			sz := dfs(y, x)
			if sz0 == 0 {
				sz0 = sz // 记录第一个儿子子树的大小
			} else if sz != sz0 { // 存在大小不一样的儿子子树
				ok = false // 注意不能 break，其他子树 y 仍然要递归
			}
			size += sz
		}
		if ok {
			ans++
		}
		return size
	}
	dfs(0, -1)
	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{edges}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

更多相似题目，见 [一般树题单](https://leetcode.cn/circle/discuss/K0n2gO/) 中的「**§3.3 自底向上 DFS**」。

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
