# 3310. 移除可疑的方法

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/remove-methods-from-project/
- 题目 slug：`remove-methods-from-project`
- 来源专题：图论算法
- 来源分类路径：一、图的遍历 / §1.1 深度优先搜索（DFS）
- 难度分：1711
- 外部题解来源：https://leetcode.cn/problems/remove-methods-from-project/solutions/2940460/liang-ci-dfspythonjavacgo-by-endlesschen-cjat/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[DFS 标记可疑方法（Python/Java/C++/Go）](https://leetcode.cn/problems/remove-methods-from-project/solutions/2940460/liang-ci-dfspythonjavacgo-by-endlesschen-cjat/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`liang-ci-dfspythonjavacgo-by-endlesschen-cjat`
- topic id：`2940460`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 题意

一个项目有 $n$ 个方法，其中第 $k$ 个方法有 bug。可能是第 $k$ 个方法自己的 bug，也可能是第 $k$ 个方法调用的更底层的方法有 bug。

你需要删除所有可能有 bug 的方法。如果删除后无法编译（剩余的方法调用了删除的方法），那么返回数组 $[0,1,2,\cdots,n-1]$。

如果可以正常删除，返回剩余的方法编号。

## 思路

1. 从 $k$ 开始 DFS 图，标记所有可能有 bug 的方法（节点）。题目把这些方法叫做**可疑方法**。
2. 遍历 $\textit{invocations}$，如果存在从「非可疑方法」到「可疑方法」的边，则删除后无法编译，返回数组 $[0,1,2,\cdots,n-1]$。
3. 否则可以正常删除，把非可疑方法加入答案。

⚠**注意**：图中可能有环，为避免 DFS 无限递归下去，只需 DFS 没有访问过（没有被标记）的节点。

```py [sol-Python3 哈希集合]
class Solution:
    def remainingMethods(self, n: int, k: int, invocations: List[List[int]]) -> List[int]:
        g = [[] for _ in range(n)]
        for x, y in invocations:
            g[x].append(y)

        # 收集所有可疑方法
        suspicious = set()
        def dfs(x: int) -> None:
            suspicious.add(x)
            for y in g[x]:
                if y not in suspicious:  # 避免无限递归
                    dfs(y)
        dfs(k)

        # 检查是否有【非可疑方法】->【可疑方法】的边
        for x, y in invocations:
            if x not in suspicious and y in suspicious:
                # 无法移除可疑方法
                return list(range(n))

        # 移除所有可疑方法
        return list(set(range(n)) - suspicious)
```

```py [sol-Python3 数组]
class Solution:
    def remainingMethods(self, n: int, k: int, invocations: List[List[int]]) -> List[int]:
        g = [[] for _ in range(n)]
        for x, y in invocations:
            g[x].append(y)

        # 标记所有可疑方法
        is_suspicious = [False] * n
        def dfs(x: int) -> None:
            is_suspicious[x] = True
            for y in g[x]:
                if not is_suspicious[y]:  # 避免无限递归
                    dfs(y)
        dfs(k)

        # 检查是否有【非可疑方法】->【可疑方法】的边
        for x, y in invocations:
            if not is_suspicious[x] and is_suspicious[y]:
                # 无法移除可疑方法
                return list(range(n))

        # 移除所有可疑方法
        return [i for i, b in enumerate(is_suspicious) if not b]
```

```java [sol-Java]
class Solution {
    public List<Integer> remainingMethods(int n, int k, int[][] invocations) {
        List<Integer>[] g = new ArrayList[n];
        Arrays.setAll(g, i -> new ArrayList<>());
        for (int[] e : invocations) {
            g[e[0]].add(e[1]);
        }

        // 标记所有可疑方法
        boolean[] isSuspicious = new boolean[n];
        dfs(k, g, isSuspicious);

        // 检查是否有【非可疑方法】->【可疑方法】的边
        for (int[] e : invocations) {
            if (!isSuspicious[e[0]] && isSuspicious[e[1]]) {
                // 无法移除可疑方法
                List<Integer> ans = new ArrayList<>(n);
                for (int i = 0; i < n; i++) {
                    ans.add(i);
                }
                return ans;
            }
        }

        // 移除所有可疑方法
        List<Integer> ans = new ArrayList<>();
        for (int i = 0; i < n; i++) {
            if (!isSuspicious[i]) {
                ans.add(i);
            }
        }
        return ans;
    }

    private void dfs(int x, List<Integer>[] g, boolean[] isSuspicious) {
        isSuspicious[x] = true;
        for (int y : g[x]) {
            if (!isSuspicious[y]) { // 避免无限递归
                dfs(y, g, isSuspicious);
            }
        }
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> remainingMethods(int n, int k, vector<vector<int>>& invocations) {
        vector<vector<int>> g(n);
        for (auto& e : invocations) {
            g[e[0]].push_back(e[1]);
        }

        // 标记所有可疑方法
        vector<int> is_suspicious(n);
        auto dfs = [&](auto&& dfs, int x) -> void {
            is_suspicious[x] = true;
            for (int y : g[x]) {
                if (!is_suspicious[y]) { // 避免无限递归
                    dfs(dfs, y);
                }
            }
        };
        dfs(dfs, k);

        // 检查是否有【非可疑方法】->【可疑方法】的边
        for (auto& e : invocations) {
            if (!is_suspicious[e[0]] && is_suspicious[e[1]]) {
                // 无法移除可疑方法
                vector<int> ans(n);
                iota(ans.begin(), ans.end(), 0);
                return ans;
            }
        }

        // 移除所有可疑方法
        vector<int> ans;
        for (int i = 0; i < n; i++) {
            if (!is_suspicious[i]) {
                ans.push_back(i);
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func remainingMethods(n, k int, invocations [][]int) (ans []int) {
	g := make([][]int, n)
	for _, e := range invocations {
		g[e[0]] = append(g[e[0]], e[1])
	}

	// 标记所有可疑方法
	isSuspicious := make([]bool, n)
	var dfs func(int)
	dfs = func(x int) {
		isSuspicious[x] = true
		for _, y := range g[x] {
			if !isSuspicious[y] { // 避免无限递归
				dfs(y)
			}
		}
	}
	dfs(k)

	// 检查是否有【非可疑方法】->【可疑方法】的边
	for _, e := range invocations {
		if !isSuspicious[e[0]] && isSuspicious[e[1]] {
			// 无法移除可疑方法
			for i := range n {
				ans = append(ans, i)
			}
			return
		}
	}

	// 移除所有可疑方法
	for i, b := range isSuspicious {
		if !b {
			ans = append(ans, i)
		}
	}
	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n+m)$，其中 $m$ 是 $\textit{invocations}$ 的长度。
- 空间复杂度：$\mathcal{O}(n+m)$。

更多相似题目，见下面图论题单中的「**DFS**」。

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

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

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
