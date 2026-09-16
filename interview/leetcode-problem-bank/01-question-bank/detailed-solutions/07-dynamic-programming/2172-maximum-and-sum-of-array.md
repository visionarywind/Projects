# 2172. 数组的最大与和

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-and-sum-of-array/
- 题目 slug：`maximum-and-sum-of-array`
- 来源专题：动态规划
- 来源分类路径：九、状态压缩 DP（状压 DP） / §9.1 排列型状压 DP ① 相邻无关
- 难度分：2392
- 外部题解来源：https://leetcode.cn/problems/maximum-and-sum-of-array/solutions/1262392/zhuang-tai-ya-suo-dp-by-endlesscheng-5eqn/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[状态压缩 DP（附最小费用最大流做法）](https://leetcode.cn/problems/maximum-and-sum-of-array/solutions/1262392/zhuang-tai-ya-suo-dp-by-endlesscheng-5eqn/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`zhuang-tai-ya-suo-dp-by-endlesscheng-5eqn`
- topic id：`1262392`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

#### 提示 1

转换一下：视作有 $2\cdot\textit{numSlots}$ 个篮子，每个篮子至多可以放 $1$ 个整数。

#### 提示 2

数据范围很小，考虑状压 DP。

#### 提示 3

以谁作为状态定义的对象呢？注意篮子编号是不能变的，而 $\textit{nums}$ 中元素的位置信息是不重要的。

---

由于每个篮子至多可以放 $2$ 个整数，我们可以视作有 $2\cdot\textit{numSlots}$ 个篮子，每个篮子至多可以放 $1$ 个整数。由于篮子个数很少，我们可以用二进制数 $x$ 表示这 $2\cdot\textit{numSlots}$ 个篮子中放了数字的篮子集合，其中 $x$ 从低到高的第 $i$ 位为 $1$ 表示第 $i$ 个篮子放了数字，为 $0$ 表示第 $i$ 个篮子为空。

设 $i$ 的二进制中的 $1$ 的个数为 $c$，定义 $f[i]$ 表示将 $\textit{nums}$ 的前 $c$ 个数字放到篮子中，且放了数字的篮子集合为 $i$ 时的最大与和。初始值 $f[0]=0$。

考虑将 $\textit{nums}[c]$ 放到一个空篮子时的状态转移方程（下标从 $0$ 开始，此时 $\textit{nums}[c]$ 还没被放入篮中），我们可以枚举 $i$ 中的 $0$，即空篮子的位置 $j$，该空篮子对应的编号为 $\dfrac{j}{2}+1$，则有

$$
f[i+2^j] = \max(f[i+2^j],\ f[i] + (\dfrac{j}{2}+1)\&\textit{nums}[c])
$$

设 $\textit{nums}$ 的长度为 $n$，最后答案为 $\max_{c=n}(f)$。

由于篮子装满肯定比有空篮子优，所以 $\max_{c=n}(f) = \max(f)$。

代码实现时需要注意，若 $c\ge n$ 则 $f[i]$ 无法转移，需要跳过。

相似题目：

- [1879. 两个数组最小的异或值之和](https://leetcode.cn/problems/minimum-xor-sum-of-two-arrays/)

```Python [sol1-Python3]
class Solution:
    def maximumANDSum(self, nums: List[int], numSlots: int) -> int:
        f = [0] * (1 << (numSlots * 2))
        for i, fi in enumerate(f):
            c = i.bit_count()
            if c >= len(nums): continue
            for j in range(numSlots * 2):
                if (i & (1 << j)) == 0:  # 枚举空篮子 j
                    s = i | (1 << j)
                    f[s] = max(f[s], fi + ((j // 2 + 1) & nums[c]))
        return max(f)
```

```java [sol1-Java]
class Solution {
    public int maximumANDSum(int[] nums, int numSlots) {
        var ans = 0;
        var f = new int[1 << (numSlots * 2)];
        for (var i = 0; i < f.length; i++) {
            var c = Integer.bitCount(i);
            if (c >= nums.length) continue;
            for (var j = 0; j < numSlots * 2; ++j) {
                if ((i & (1 << j)) == 0) { // 枚举空篮子 j
                    var s = i | (1 << j);
                    f[s] = Math.max(f[s], f[i] + ((j / 2 + 1) & nums[c]));
                    ans = Math.max(ans, f[s]);
                }
            }
        }
        return ans;
    }
}
```

```C++ [sol1-C++]
class Solution {
public:
    int maximumANDSum(vector<int> &nums, int numSlots) {
        int ans = 0;
        vector<int> f(1 << (numSlots * 2));
        for (int i = 0; i < f.size(); ++i) {
            int c = __builtin_popcount(i);
            if (c >= nums.size()) continue;
            for (int j = 0; j < numSlots * 2; ++j) {
                if ((i & (1 << j)) == 0) { // 枚举空篮子 j
                    int s = i | (1 << j);
                    f[s] = max(f[s], f[i] + ((j / 2 + 1) & nums[c]));
                    ans = max(ans, f[s]);
                }
            }
        }
        return ans;
    }
};
```

```go [sol1-Go]
func maximumANDSum(nums []int, numSlots int) (ans int) {
	f := make([]int, 1<<(numSlots*2))
	for i, fi := range f {
		c := bits.OnesCount(uint(i))
		if c >= len(nums) {
			continue
		}
		for j := 0; j < numSlots*2; j++ {
			if i>>j&1 == 0 { // 枚举空篮子 j
				s := i | 1<<j
				f[s] = max(f[s], fi+(j/2+1)&nums[c])
				ans = max(ans, f[s])
			}
		}
	}
	return
}

func max(a, b int) int { if b > a { return b }; return a }
```

---

顺便说一下这题怎么用最小费用最大流解决，这其实是一种比较典型的建图技巧。

设集合 $A$ 为数字，集合 $B$ 为篮子，额外建立超级源点和超级汇点：

- 从源点连容量为 $1$ 费用为 $0$ 的边到 $A$ 中各点；
- 从 $B$ 中各点连容量为 $2$ 费用为 $0$ 的边到汇点；
- 从 $A$ 的每个数字 $\textit{nums}[i]$ 向 $B$ 的每个篮子 $j$ 连边，容量为 $+\infty$，费用为 $-\textit{nums}[i]\& j$，取负号是为了求最小费用最大流。

这样跑最小费用最大流得到的结果的**相反数**就是匹配 $A$ 中所有数字的最大花费，即最大与和。

时间复杂度 $O(nm(n+m))$，实际运行时间 $0$ ms。

贴个 Go 的实现：

```go
func maximumANDSum(nums []int, numSlots int) (ans int) {
	const inf int = 1e9

	// 集合 A 和 B 的大小
	n, m := len(nums), numSlots

	// 建图
	type neighbor struct{ to, rid, cap, cost int } // 相邻节点、反向边下标、容量、费用
	g := make([][]neighbor, n+m+2)
	addEdge := func(from, to, cap, cost int) {
		g[from] = append(g[from], neighbor{to, len(g[to]), cap, cost})
		g[to] = append(g[to], neighbor{from, len(g[from]) - 1, 0, -cost})
	}
	start := n + m   // 超级源点
	end := start + 1 // 超级汇点
	for i, num := range nums {
		addEdge(start, i, 1, 0)
		for j := 1; j <= m; j++ {
			addEdge(i, n+j-1, inf, -(num & j))
		}
	}
	for i := 0; i < m; i++ {
		addEdge(n+i, end, 2, 0)
	}

	// 下面为最小费用最大流模板
	dist := make([]int, len(g))
	type vi struct{ v, i int }
	fa := make([]vi, len(g))
	spfa := func() bool {
		for i := range dist {
			dist[i] = inf
		}
		dist[start] = 0
		inQ := make([]bool, len(g))
		inQ[start] = true
		q := []int{start}
		for len(q) > 0 {
			v := q[0]
			q = q[1:]
			inQ[v] = false
			for i, e := range g[v] {
				if e.cap == 0 {
					continue
				}
				w := e.to
				if newD := dist[v] + e.cost; newD < dist[w] {
					dist[w] = newD
					fa[w] = vi{v, i}
					if !inQ[w] {
						q = append(q, w)
						inQ[w] = true
					}
				}
			}
		}
		return dist[end] < inf
	}
	for spfa() {
		// 沿 start-end 的最短路尽量增广
		minFlow := inf
		for v := end; v != start; {
			p := fa[v]
			if c := g[p.v][p.i].cap; c < minFlow {
				minFlow = c
			}
			v = p.v
		}
		for v := end; v != start; {
			p := fa[v]
			e := &g[p.v][p.i]
			e.cap -= minFlow
			g[v][e.rid].cap += minFlow
			v = p.v
		}
		ans -= dist[end] * minFlow
	}
	return
}
```

## 本地原创解析

### 1. 题意重述

本题来自 `九、状态压缩 DP（状压 DP） / §9.1 排列型状压 DP ① 相邻无关`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `九、状态压缩 DP（状压 DP） / §9.1 排列型状压 DP ① 相邻无关`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
