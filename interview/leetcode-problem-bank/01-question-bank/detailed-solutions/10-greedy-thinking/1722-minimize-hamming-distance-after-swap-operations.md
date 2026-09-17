# 1722. 执行交换操作后的最小汉明距离

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimize-hamming-distance-after-swap-operations/
- 题目 slug：`minimize-hamming-distance-after-swap-operations`
- 来源专题：贪心与思维
- 来源分类路径：五、思维题 / §5.7 交换元素
- 难度分：1892
- 外部题解来源：https://leetcode.cn/problems/minimize-hamming-distance-after-swap-operations/solutions/3951185/jian-tu-dfspythonjavacgo-by-endlesscheng-qmkq/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[建图 + DFS（Python/Java/C++/Go）](https://leetcode.cn/problems/minimize-hamming-distance-after-swap-operations/solutions/3951185/jian-tu-dfspythonjavacgo-by-endlesscheng-qmkq/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`jian-tu-dfspythonjavacgo-by-endlesscheng-qmkq`
- topic id：`3951185`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:13:14 +0800

在 $a_i$ 和 $b_i$ 之间连边，得到一个无向图 $G$。

$G$ 的同一个连通块中的节点（下标）可以随意交换。

[证明（具体交换方案）](https://zhuanlan.zhihu.com/p/2027293665970721660)

例如 $\textit{source}$ 在某个连通块中的元素为 $1,1,2,3$，$\textit{target}$ 在这个连通块中的元素为 $2,2,4,1$，二者有一个共同的 $1$ 和一个共同的 $2$。但 $\textit{source}$ 多了一个 $1$ 和一个 $3$，$\textit{target}$ 多了一个 $2$ 和一个 $4$，每对多出来的元素，无法匹配，会贡献 $1$ 个汉明距离。所以这个连通块贡献了 $2$ 个汉明距离。

用哈希表（或者数组）统计 $\textit{source}$ 和 $\textit{target}$ 各自多出来的元素个数。总个数除以 $2$，即为无法配对的元素对的个数，即汉明距离。

```py [sol-Python3]
class Solution:
    def minimumHammingDistance(self, source: List[int], target: List[int], allowedSwaps: List[List[int]]) -> int:
        n = len(source)
        g = [[] for _ in range(n)]
        for i, j in allowedSwaps:
            g[i].append(j)  # 建图
            g[j].append(i)

        def dfs(x: int) -> None:
            vis[x] = True  # 避免重复访问
            # 抵消相同的元素，最终剩下 source 和 target 各自多出来的元素（对称差）
            diff[source[x]] += 1
            diff[target[x]] -= 1
            for y in g[x]:
                if not vis[y]:
                    dfs(y)

        vis = [False] * n
        ans = 0
        for x in range(n):
            if not vis[x]:
                diff = defaultdict(int)
                dfs(x)
                ans += sum(map(abs, diff.values()))
        return ans // 2  # 有 ans // 2 对多出来的元素
```

```java [sol-Java]
class Solution {
    public int minimumHammingDistance(int[] source, int[] target, int[][] allowedSwaps) {
        int n = source.length;
        List<Integer>[] g = new ArrayList[n];
        Arrays.setAll(g, _ -> new ArrayList<>());
        for (int[] e : allowedSwaps) {
            int i = e[0];
            int j = e[1];
            g[i].add(j); // 建图
            g[j].add(i);
        }

        boolean[] vis = new boolean[n];
        int ans = 0;
        for (int x = 0; x < n; x++) {
            if (!vis[x]) {
                Map<Integer, Integer> diff = new HashMap<>();
                dfs(x, source, target, g, vis, diff);
                for (int c : diff.values()) {
                    ans += Math.abs(c);
                }
            }
        }
        return ans / 2; // 有 ans / 2 对多出来的元素
    }

    private void dfs(int x, int[] source, int[] target, List<Integer>[] g, boolean[] vis, Map<Integer, Integer> diff) {
        vis[x] = true; // 避免重复访问
        // 抵消相同的元素，最终剩下 source 和 target 各自多出来的元素（对称差）
        diff.merge(source[x], 1, Integer::sum);  // diff[source[x]]++;
        diff.merge(target[x], -1, Integer::sum); // diff[target[x]]--;
        for (int y : g[x]) {
            if (!vis[y]) {
                dfs(y, source, target, g, vis, diff);
            }
        }
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minimumHammingDistance(vector<int>& source, vector<int>& target, vector<vector<int>>& allowedSwaps) {
        int n = source.size();
        vector<vector<int>> g(n);
        for (auto& e : allowedSwaps) {
            int i = e[0], j = e[1];
            g[i].push_back(j); // 建图
            g[j].push_back(i);
        }

        vector<int8_t> vis(n);
        unordered_map<int, int> diff;

        auto dfs = [&](this auto&& dfs, int x) -> void {
            vis[x] = true; // 避免重复访问
            // 抵消相同的元素，最终剩下 source 和 target 各自多出来的元素（对称差）
            diff[source[x]]++;
            diff[target[x]]--;
            for (int y : g[x]) {
                if (!vis[y]) {
                    dfs(y);
                }
            }
        };

        int ans = 0;
        for (int x = 0; x < n; x++) {
            if (!vis[x]) {
                diff.clear();
                dfs(x);
                for (auto& [_, c] : diff) {
                    ans += abs(c);
                }
            }
        }
        return ans / 2; // 有 ans / 2 对多出来的元素
    }
};
```

```go [sol-Go]
func minimumHammingDistance(source []int, target []int, allowedSwaps [][]int) (ans int) {
	n := len(source)
	g := make([][]int, n)
	for _, e := range allowedSwaps {
		i, j := e[0], e[1]
		g[i] = append(g[i], j) // 建图
		g[j] = append(g[j], i)
	}

	vis := make([]bool, n)
	diff := map[int]int{}

	var dfs func(int)
	dfs = func(x int) {
		vis[x] = true // 避免重复访问
		// 抵消相同的元素，最终剩下 source 和 target 各自多出来的元素（对称差）
		diff[source[x]]++
		diff[target[x]]--
		for _, y := range g[x] {
			if !vis[y] {
				dfs(y)
			}
		}
	}

	for x, b := range vis {
		if !b {
			clear(diff)
			dfs(x)
			for _, c := range diff {
				ans += abs(c)
			}
		}
	}
	return ans / 2 // 有 ans / 2 对多出来的元素
}

func abs(x int) int {
	if x < 0 {
		return -x
	}
	return x
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n + m)$，其中 $n$ 是 $\textit{source}$ 的长度，$m$ 是 $\textit{allowedSwaps}$ 的长度。
- 空间复杂度：$\mathcal{O}(n + m)$。

## 相似题目

- [1202. 交换字符串中的元素](https://leetcode.cn/problems/smallest-string-with-swaps/)
- [1722. 执行交换操作后的最小汉明距离](https://leetcode.cn/problems/minimize-hamming-distance-after-swap-operations/)
- [3695. 交换元素后的最大交替和](https://leetcode.cn/problems/maximize-alternating-sum-using-swaps/)

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

本题来自 `五、思维题 / §5.7 交换元素`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `五、思维题 / §5.7 交换元素`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
