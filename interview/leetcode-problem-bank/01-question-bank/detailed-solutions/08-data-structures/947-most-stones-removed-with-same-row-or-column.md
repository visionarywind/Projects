# 947. 移除最多的同行或同列石头

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/most-stones-removed-with-same-row-or-column/
- 题目 slug：`most-stones-removed-with-same-row-or-column`
- 来源专题：常用数据结构
- 来源分类路径：七、并查集 / §7.3 中介并查集
- 难度分：2035
- 外部题解来源：https://leetcode.cn/problems/most-stones-removed-with-same-row-or-column/solutions/3931977/zhong-jie-bing-cha-ji-pythonjavacgo-by-e-ulrj/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[中介并查集（Python/Java/C++/Go）](https://leetcode.cn/problems/most-stones-removed-with-same-row-or-column/solutions/3931977/zhong-jie-bing-cha-ji-pythonjavacgo-by-e-ulrj/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`zhong-jie-bing-cha-ji-pythonjavacgo-by-e-ulrj`
- topic id：`3931977`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 题意

如果一对点（石头），横坐标相同或者纵坐标相同，那么在这两个点之间连一条边，可以得到一个无向图。

这个图的大小为 $k$ 的连通块，可以移除其中 $k-1$ 块石头。移除后，剩下的 $1$ 块石头由于没有同行或同列的其他石头，所以不能移除。

每个连通块都剩下 $1$ 块石头，所以最终剩下「图的连通块个数」块石头。

移除的石头个数，就是 $n$ 减去图的连通块个数。

## 引入

如果 $\mathcal{O}(n^2)$ 枚举所有点对，连边，就太慢了。

对于 $x$ 相同的点，我们可以把这些点都连到同一个代表（或者说中介）上。对于 $y$ 相同的点，同理，把这些点都连到同一个代表（或者说中介）上。

如果两个点可以通过中介互相到达，那么这两个点属于同一个连通块。

这样只需要连 $2n$ 条边，大大地提高了效率。

进一步地，直接把中介 $x$ 和中介 $y$ 相连，这样只需要连 $n$ 条边。

## 思路

在行和列之间连边，**相连的行列中介可以被一个坐标点直接或间接地访问到**，连接行列中介相当于连接了同行同列的点。具体地，在节点 $x$ 和节点 $y + \textit{offset}$ 之间连边，其中 $\textit{offset} = 10^4+1$。增加 $\textit{offset}$ 是为了区分行列。

这可以用**并查集**解决。我们可以用哈希表记录代表元，这样即使坐标范围改成 $[-10^9,10^9]$ 也能做。

不同代表元的个数，就是连通块的个数。

[视频讲解](https://www.bilibili.com/video/BV1DvwTzbE1n/?t=27m34s)（一道类似的题），欢迎点赞关注~

```py [sol-Python3]
class Solution:
    def removeStones(self, stones: List[List[int]]) -> int:
        # 哈希表并查集
        fa = {}

        def find(x: int) -> int:
            if x not in fa:
                return x
            if fa[x] != x:
                fa[x] = find(fa[x])
            return fa[x]

        OFFSET = 10001
        for x, y in stones:
            fa[find(x)] = find(y + OFFSET)

        # 记录不同的代表元
        st = {find(p[0]) for p in stones}

        return len(stones) - len(st)
```

```java [sol-Java]
class Solution {
    public int removeStones(int[][] stones) {
        // 哈希表并查集
        Map<Integer, Integer> fa = new HashMap<>();

        final int OFFSET = 10001;
        for (int[] p : stones) {
            int fx = find(p[0], fa);
            int fy = find(p[1] + OFFSET, fa);
            fa.put(fx, fy);
        }

        // 记录不同的代表元
        Set<Integer> set = new HashSet<>();
        for (int[] p : stones) {
            set.add(find(p[0], fa));
        }

        return stones.length - set.size();
    }

    private int find(int x, Map<Integer, Integer> fa) {
        Integer fx = fa.get(x);
        if (fx == null) {
            return x;
        }
        if (fx != x) {
            int root = find(fx, fa);
            fa.put(x, root);
            return root;
        }
        return x;
    }
}
```

```cpp [sol-C++]
class Solution {
    // 哈希表并查集
    unordered_map<int, int> fa;

    int find(int x) {
        auto it = fa.find(x);
        if (it == fa.end()) {
            return x;
        }
        auto& fx = it->second;
        if (fx != x) {
            fx = find(fx);
        }
        return fx;
    }

public:
    int removeStones(vector<vector<int>>& stones) {
        constexpr int OFFSET = 10001;
        for (auto& p : stones) {
            fa[find(p[0])] = find(p[1] + OFFSET);
        }

        // 记录不同的代表元
        unordered_set<int> st;
        for (auto& p : stones) {
            st.insert(find(p[0]));
        }

        return stones.size() - st.size();
    }
};
```

```go [sol-Go]
func removeStones(stones [][]int) int {
	// 哈希表并查集
	fa := map[int]int{}
	var find func(int) int
	find = func(x int) int {
		fx, ok := fa[x]
		if !ok {
			return x
		}
		if fx != x {
			fa[x] = find(fx)
			return fa[x]
		}
		return x
	}

	const offset = 10001
	for _, p := range stones {
		fa[find(p[0])] = find(p[1] + offset)
	}

	// 记录不同的代表元
	set := map[int]struct{}{}
	for _, p := range stones {
		set[find(p[0])] = struct{}{}
	}

	return len(stones) - len(set)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n)$，其中 $n$ 是 $\textit{stones}$ 的长度。并查集我只写了路径压缩，单次操作的均摊复杂度是 $\mathcal{O}(\log n)$。
- 空间复杂度：$\mathcal{O}(n)$。

注：如果用建图 + DFS 实现，可以做到 $\mathcal{O}(n)$ 时间。

## 专题训练

见下面数据结构题单的「**§7.3 中介并查集**」。

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

本题来自 `七、并查集 / §7.3 中介并查集`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `七、并查集 / §7.3 中介并查集`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
