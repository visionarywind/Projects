# 2440. 创建价值相同的连通块

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/create-components-with-same-value/
- 题目 slug：`create-components-with-same-value`
- 来源专题：链表、树与回溯
- 来源分类路径：三、一般树 / §3.3 自底向上 DFS
- 难度分：2460
- 外部题解来源：https://leetcode.cn/problems/create-components-with-same-value/solutions/1895302/by-endlesscheng-u03q/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[枚举答案 + DFS（Python/Java/C++/Go）](https://leetcode.cn/problems/create-components-with-same-value/solutions/1895302/by-endlesscheng-u03q/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`by-endlesscheng-u03q`
- topic id：`1895302`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:49:15 +0800

## 提示 1

枚举连通块的个数 $i$，则删除的边数为 $i-1$。

设 $\textit{total}$ 为整棵树的点权和（即 $\textit{nums}$ 的元素和），如果 $\textit{total}$ 能被 $i$ 整除（$i$ 是 $\textit{total}$ 的因子），那么每个连通块的点权和都应等于 $\dfrac{\textit{total}}{i}$，记作 $\textit{target}$。

什么样的边可以删除呢？

## 提示 2

如果一条边左右两侧的点权和都是 $\textit{target}$ 的倍数，那么这条边就可以删除。由于要使删除的边数最多，这条边**必须**删除。

由于 $\textit{total}$ 可以被 $\textit{target}$ 整除，我们只需要看一侧的点权和是否为 $\textit{target}$ 的倍数。

换言之，可以从任意点出发 DFS，只要发现子树的点权和是 $\textit{target}$ 的倍数，就说明子树到上面父节点的这条边是可以删除的。

具体来说，不妨以 $0$ 为根，DFS 这棵树，统计子树的点权和：

- 如果点权和超过 $\textit{target}$，说明当前删边方案不合法，返回 $-1$。
- 如果点权和等于 $\textit{target}$，这条边必须删除，返回 $0$。
- 如果点权和小于 $\textit{target}$，尚未找到一个完整的连通块，返回点权和。

如果 DFS 最终没有返回 $-1$，则当前删边方案合法。

如果我们从大到小枚举连通块的个数，则此时删除的边数是最多的，直接返回 $i-1$。

## 答疑

**问**：为什么这样做可以保证分出**恰好** $i$ 个连通块？

**答**：第一，不会超过 $i$ 个连通块，因为我们的做法相当于用水杯接水，每次接满 $\textit{target}$ 水就换下一杯继续接水。总共就 $\textit{total}$ 的水，至多可以接 $i$ 杯水。

第二，不会低于 $i$ 个连通块，如果出现这样的情况，说明至少有一个连通块的点权和超过 $\textit{target}$，此时 DFS 会返回 $-1$。

## 优化

代码实现时，由于点权至少为 $mx=\max(\textit{nums})$，所以连通块的个数至多为 $\left\lfloor\dfrac{\textit{total}}{mx}\right\rfloor$。由于 $\left\lfloor\dfrac{\textit{total}}{mx}\right\rfloor\le n$，因此可以从 $\left\lfloor\dfrac{\textit{total}}{mx}\right\rfloor$ 开始枚举连通块的个数。

[视频讲解](https://www.bilibili.com/video/BV1cV4y157BY) 第四题。

```py [sol-Python3]
class Solution:
    def componentValue(self, nums: List[int], edges: List[List[int]]) -> int:
        g = [[] for _ in nums]
        for x, y in edges:
            g[x].append(y)
            g[y].append(x)

        def dfs(x: int, fa: int) -> int:
            s = nums[x]
            for y in g[x]:
                if y != fa:
                    res = dfs(y, x)
                    if res < 0: return -1
                    s += res
            if s > target: return -1
            return s if s < target else 0

        total = sum(nums)
        for i in range(total // max(nums), 1, -1):
            if total % i == 0:
                target = total // i
                if dfs(0, -1) == 0: return i - 1
        return 0
```

```java [sol-Java]
class Solution {
    private List<Integer>[] g;
    private int[] nums;
    private int target;

    public int componentValue(int[] nums, int[][] edges) {
        g = new ArrayList[nums.length];
        Arrays.setAll(g, e -> new ArrayList<>());
        for (int[] e : edges) {
            int x = e[0], y = e[1];
            g[x].add(y);
            g[y].add(x);
        }
        this.nums = nums;

        int total = Arrays.stream(nums).sum();
        int max = Arrays.stream(nums).max().getAsInt();
        for (int i = total / max; ; i--) {
            if (total % i == 0) {
                target = total / i;
                if (dfs(0, -1) == 0) {
                    return i - 1;
                }
            }
        }
    }

    private int dfs(int x, int fa) {
        int sum = nums[x];
        for (int y : g[x]) {
            if (y != fa) {
                var res = dfs(y, x);
                if (res < 0) return -1;
                sum += res;
            }
        }
        if (sum > target) {
            return -1;
        }
        return sum < target ? sum : 0;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int componentValue(vector<int>& nums, vector<vector<int>>& edges) {
        vector<vector<int>> g(nums.size());
        for (auto& e : edges) {
            int x = e[0], y = e[1];
            g[x].push_back(y);
            g[y].push_back(x);
        }

        int target;
        auto dfs = [&](this auto&& dfs, int x, int fa) -> int {
            int sum = nums[x];
            for (int y : g[x]) {
                if (y != fa) {
                    int res = dfs(y, x);
                    if (res < 0) {
                        return -1;
                    }
                    sum += res;
                }
            }
            if (sum > target) {
                return -1;
            }
            return sum < target ? sum : 0;
        };

        int total = reduce(nums.begin(), nums.end());
        int mx = ranges::max(nums);
        for (int i = total / mx; ; i--) {
            if (total % i == 0) {
                target = total / i;
                if (dfs(0, -1) == 0) {
                    return i - 1;
                }
            }
        }
    }
};
```

```go [sol-Go]
func componentValue(nums []int, edges [][]int) int {
	g := make([][]int, len(nums))
	for _, e := range edges {
		x, y := e[0], e[1]
		g[x] = append(g[x], y)
		g[y] = append(g[y], x)
	}

	var target int
	var dfs func(int, int) int
	dfs = func(x, fa int) int {
		sum := nums[x]
		for _, y := range g[x] {
			if y != fa {
				res := dfs(y, x)
				if res < 0 {
					return -1
				}
				sum += res
			}
		}
		if sum > target {
			return -1
		}
		if sum == target {
			return 0
		}
		return sum
	}

	total, mx := 0, 0
	for _, x := range nums {
		total += x
		mx = max(mx, x)
	}
	for i := total / mx; ; i-- {
		if total%i == 0 {
			target = total / i
			if dfs(0, -1) == 0 {
				return i - 1
			}
		}
	}
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\cdot d(s))$，其中 $n$ 为 $\textit{nums}$ 的长度，$s$ 为所有 $\textit{nums}[i]$ 的和，$d(s)$ 为 $s$ 的因子个数。根据本题的数据范围，$d(s)\le 240$，例如 $s=720720$ 时可以取到等号。
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
