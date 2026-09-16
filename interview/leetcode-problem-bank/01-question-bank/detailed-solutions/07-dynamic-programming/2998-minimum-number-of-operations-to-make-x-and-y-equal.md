# 2998. 使 X 和 Y 相等的最少操作次数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-number-of-operations-to-make-x-and-y-equal/
- 题目 slug：`minimum-number-of-operations-to-make-x-and-y-equal`
- 来源专题：动态规划
- 来源分类路径：其他
- 难度分：1795
- 外部题解来源：https://leetcode.cn/problems/minimum-number-of-operations-to-make-x-and-y-equal/solutions/2594112/liang-chong-fang-fa-bfsji-yi-hua-sou-suo-djba/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：BFS/记忆化搜索（Python/Java/C++/Go）](https://leetcode.cn/problems/minimum-number-of-operations-to-make-x-and-y-equal/solutions/2594112/liang-chong-fang-fa-bfsji-yi-hua-sou-suo-djba/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`liang-chong-fang-fa-bfsji-yi-hua-sou-suo-djba`
- topic id：`2594112`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 方法一：BFS

每个操作都可以理解成：从 $x$ 向操作后的数连边。

在这张图上跑 BFS，求出从 $x$ 到 $y$ 的最短路，即为答案。

注意，如果 $x<y$ 那么只能用加一操作，此时可以直接算出操作次数。

代码采用双数组实现 BFS，具体请看[【基础算法精讲 13】](https://www.bilibili.com/video/BV1hG4y1277i/)。

[本题视频讲解](https://www.bilibili.com/video/BV1Fg4y1Q7wv/)（第三题）

```py [sol-Python3]
class Solution:
    def minimumOperationsToMakeEqual(self, x: int, y: int) -> int:
        if x <= y:
            return y - x
        ans = x - y  # 总操作次数不会超过 x-y
        vis = [False] * (x + ans + 1)  # +1 操作至多执行 x-y 次
        q = []
        step = 0

        def add(v: int) -> None:
            if v < y:
                nonlocal ans
                ans = min(ans, step + 1 + y - v)  # 只能执行 +1 操作
            elif not vis[v]:
                vis[v] = True
                q.append(v)

        add(x)
        while True:
            tmp = q
            q = []
            for v in tmp:
                if v == y:
                    return min(ans, step)
                if v % 11 == 0:
                    add(v // 11)
                if v % 5 == 0:
                    add(v // 5)
                add(v - 1)
                add(v + 1)
            step += 1
```

```java [sol-Java]
class Solution {
    public int minimumOperationsToMakeEqual(int x, int y) {
        if (x <= y) {
            return y - x;
        }
        int ans = x - y; // 总操作次数不会超过 x-y
        boolean[] vis = new boolean[x + ans + 1]; // +1 操作至多执行 x-y 次
        vis[x] = true;
        List<Integer> q = List.of(x);
        int step = 0;
        while (true) {
            List<Integer> tmp = q;
            q = new ArrayList<>();
            for (int v : tmp) {
                if (v == y) {
                    return Math.min(ans, step);
                }
                if (v < y) {
                    ans = Math.min(ans, step + y - v);
                    continue;
                }
                if (v % 11 == 0 && !vis[v / 11]) {
                    vis[v / 11] = true;
                    q.add(v / 11);
                }
                if (v % 5 == 0 && !vis[v / 5]) {
                    vis[v / 5] = true;
                    q.add(v / 5);
                }
                if (!vis[v - 1]) {
                    vis[v - 1] = true;
                    q.add(v - 1);
                }
                if (!vis[v + 1]) {
                    vis[v + 1] = true;
                    q.add(v + 1);
                }
            }
            step++;
        }
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minimumOperationsToMakeEqual(int x, int y) {
        if (x <= y) {
            return y - x;
        }
        int ans = x - y; // 总操作次数不会超过 x-y
        vector<int> vis(x + ans + 1); // +1 操作至多执行 x-y 次
        vector<int> q;
        int step = 0;

        auto add = [&](int v) {
            if (v < y) {
                ans = min(ans, step + 1 + y - v); // 只能执行 +1 操作
            } else if (!vis[v]) {
                vis[v] = true;
                q.push_back(v);
            }
        };

        add(x);
        while (true) {
            auto tmp = move(q); // move 后 q 为空
            for (int v : tmp) {
                if (v == y) {
                    return min(ans, step);
                }
                if (v % 11 == 0) {
                    add(v / 11);
                }
                if (v % 5 == 0) {
                    add(v / 5);
                }
                add(v - 1);
                add(v + 1);
            }
            step++;
        }
    }
};
```

```go [sol-Go]
func minimumOperationsToMakeEqual(x, y int) int {
	if x <= y {
		return y - x
	}
	ans := x - y // 总操作次数不会超过 x-y
	vis := make([]bool, x+ans+1) // +1 操作至多执行 x-y 次
	q := []int{}
	step := 0
	add := func(v int) {
		if v < y {
			ans = min(ans, step+1+y-v) // 只能执行 +1 操作
		} else if !vis[v] {
			vis[v] = true
			q = append(q, v)
		}
	}
	add(x)
	for {
		tmp := q
		q = nil
		for _, v := range tmp {
			if v == y {
				return min(ans, step)
			}
			if v%11 == 0 {
				add(v / 11)
			}
			if v%5 == 0 {
				add(v / 5)
			}
			add(v - 1)
			add(v + 1)
		}
		step++
	}
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(x)$。每个元素至多访问一次。
- 空间复杂度：$\mathcal{O}(x)$。

## 方法二：记忆化搜索

从 $x$ 到 $y$ 有哪些方式？

- 如果 $x= y$，无需操作。
- 如果 $x< y$，只能用加一操作。
- 如果 $x> y$，可以只用减一操作到达 $y$，或者：
- 通过减一操作到达 $x'=x - x\bmod 11$，此时 $x'$ 是 $11$ 的倍数，除以 $11$ 后，问题变成从 $x'/11$ 到 $y$ 的最少操作次数。注意，继续减少到 $x'-11$ 再除以 $11$，不如先把 $x'$ 除以 $11$ 再减一，后者可以到达同一个数，且操作次数更小。所以通过减一到达到 $x'$ 后，就无需再减一了。
- 通过加一操作到达 $x'=x + 11 - x\bmod 11$，此时 $x'$ 是 $11$ 的倍数，除以 $11$ 后，问题变成从 $x'/11$ 到 $y$ 的最少操作次数。
- 通过减一操作到达 $x'=x - x\bmod 5$，此时 $x'$ 是 $5$ 的倍数，除以 $5$ 后，问题变成从 $x'/5$ 到 $y$ 的最少操作次数。
- 通过加一操作到达 $x'=x + 5 - x\bmod 5$，此时 $x'$ 是 $5$ 的倍数，除以 $5$ 后，问题变成从 $x'/5$ 到 $y$ 的最少操作次数。

上述方式取最小值。

```py [sol-Python3]
class Solution:
    @cache
    def minimumOperationsToMakeEqual(self, x: int, y: int) -> int:
        if x <= y:
            return y - x
        return min(x - y,
                   self.minimumOperationsToMakeEqual(x // 11, y) + x % 11 + 1,
                   self.minimumOperationsToMakeEqual(x // 11 + 1, y) + 11 - x % 11 + 1,
                   self.minimumOperationsToMakeEqual(x // 5, y) + x % 5 + 1,
                   self.minimumOperationsToMakeEqual(x // 5 + 1, y) + 5 - x % 5 + 1)
```

```java [sol-Java]
class Solution {
    private final Map<Integer, Integer> memo = new HashMap<>();

    public int minimumOperationsToMakeEqual(int x, int y) {
        if (x <= y) {
            return y - x;
        }
        if (memo.containsKey(x)) {
            return memo.get(x);
        }
        int ans = x - y;
        ans = Math.min(ans, minimumOperationsToMakeEqual(x / 11, y) + x % 11 + 1);
        ans = Math.min(ans, minimumOperationsToMakeEqual(x / 11 + 1, y) + 11 - x % 11 + 1);
        ans = Math.min(ans, minimumOperationsToMakeEqual(x / 5, y) + x % 5 + 1);
        ans = Math.min(ans, minimumOperationsToMakeEqual(x / 5 + 1, y) + 5 - x % 5 + 1);
        memo.put(x, ans);
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
    unordered_map<int, int> memo;

public:
    int minimumOperationsToMakeEqual(int x, int y) {
        if (x <= y) {
            return y - x;
        }
        auto it = memo.find(x);
        if (it != memo.end()) {
            return it->second;
        }
        return memo[x] = min({x - y,
            minimumOperationsToMakeEqual(x / 11, y) + x % 11 + 1,
            minimumOperationsToMakeEqual(x / 11 + 1, y) + 11 - x % 11 + 1,
            minimumOperationsToMakeEqual(x / 5, y) + x % 5 + 1,
            minimumOperationsToMakeEqual(x / 5 + 1, y) + 5 - x % 5 + 1});
    }
};
```

```go [sol-Go]
func minimumOperationsToMakeEqual(x, y int) int {
	memo := map[int]int{}
	var dfs func(int) int
	dfs = func(x int) int {
		if x <= y {
			return y - x
		}
		if v, ok := memo[x]; ok {
			return v
		}
		res := min(x-y,
			dfs(x/11)+x%11+1,
			dfs(x/11+1)+11-x%11+1,
			dfs(x/5)+x%5+1,
			dfs(x/5+1)+5-x%5+1)
		memo[x] = res
		return res
	}
	return dfs(x)
}
```

#### 复杂度分析

由于除法对 $x$ 的影响远大于加减，可以认为，每次递归都把 $x$ 的规模变成 $x/5$ 和 $x/11$，当 $x\le y$ 时结束递归。故这两种操作都至多执行 $\mathcal{O}(\log (x/y))$ 次。从 $x$ 到 $y$ 的过程，$x$ 的规模会变成 $\dfrac{x}{5^p 11^q}$，其中 $p$ 和 $q$ 各有 $\mathcal{O}(\log (x/y))$ 个，所以状态个数为 $\mathcal{O}(\log^2 (x/y))$。

- 时间复杂度：$\mathcal{O}(\log^2 (x/y))$。动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题状态个数等于 $\mathcal{O}(\log^2 (x/y))$，单个状态的计算时间为 $\mathcal{O}(1)$，所以动态规划的时间复杂度为 $\mathcal{O}(\log^2 (x/y))$。
- 空间复杂度：$\mathcal{O}(\log^2 (x/y))$。保存状态所需的空间等于状态个数。

## 相似题目

- 方法一：[2059. 转化数字的最小运算数](https://leetcode.cn/problems/minimum-operations-to-convert-number/) 1850
- 方法二：[1553. 吃掉 N 个橘子的最少天数](https://leetcode.cn/problems/minimum-number-of-days-to-eat-n-oranges/) 2048 

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

## 本地原创解析

### 1. 题意重述

本题来自 `其他`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `其他`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
