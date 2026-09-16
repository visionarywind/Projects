# 2662. 前往目标的最小代价

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-cost-of-a-path-with-special-roads/
- 题目 slug：`minimum-cost-of-a-path-with-special-roads`
- 来源专题：图论算法
- 来源分类路径：三、最短路 / §3.1 单源最短路：Dijkstra 算法
- 难度分：2154
- 外部题解来源：https://leetcode.cn/problems/minimum-cost-of-a-path-with-special-roads/solutions/2251336/zhi-jie-qiu-zui-duan-lu-wu-xu-jian-tu-by-i8h7/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[无需建图，直接朴素 Dijkstra 求最短路（Python/Java/C++/Go）](https://leetcode.cn/problems/minimum-cost-of-a-path-with-special-roads/solutions/2251336/zhi-jie-qiu-zui-duan-lu-wu-xu-jian-tu-by-i8h7/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`zhi-jie-qiu-zui-duan-lu-wu-xu-jian-tu-by-i8h7`
- topic id：`2251336`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

### 本题视频讲解

见[【力扣周赛 343】](https://www.bilibili.com/video/BV1QX4y1m71X/)第三题。

### 思路

把起点、终点和所有特殊路径的终点看成是图上的点。

定义 $\textit{dis}[i]$ 表示从 $\textit{start}$ 到 $i$ 的最短路的长度。初始时 $\textit{dis}[\textit{start}]=0$，其余 $\textit{dis}[i]$ 为 $\infty$。

首先，从 $\textit{start}$ 出发，更新邻居的最短路：

- 更新到终点的最短路：直接到终点，距离为曼哈顿距离。
- 更新到所有特殊路径的终点的最短路：要么走曼哈顿距离直接过去，要么先走曼哈顿距离到特殊路径的起点，再走特殊路径。这两者取最小值。但实际上只需要考虑走特殊路径的情况，连续的曼哈顿走法可以合并成一个曼哈顿走法。

下一步，寻找除去 $\textit{start}$ 的 $\textit{dis}$ 的最小值，设这个点为 $x$，那么 $\textit{dis}[x]$ 就已经是从 $\textit{start}$ 到 $x$ 的最短路的长度了。

证明：由于除去起点的其余 $\textit{dis}[i]$ 都不低于 $\textit{dis}[x]$，且图中边权都非负，那么从另外一个点 $y$ 去更新 $\textit{dis}[x]$ 时，是无法让 $\textit{dis}[x]$ 变得更小的（因为 $\textit{dis}[x]$ 是当前最小），因此 $\textit{dis}[x]$ 已经是从 $\textit{start}$ 到 $x$ 的最短路的长度了。

由于在寻找 $\textit{dis}$ 的最小值时，需要排除在前面的循环中找到的 $x$（因为已经更新 $x$ 到其它点的最短路了，无需反复更新），可以用一个 $\textit{vis}$ 数组标记这些 $x$。

以上，通过**数学归纳法**，可以证明每次找到的未被标记的 $\textit{dis}$ 的最小值就是最短路。

由于图是**稠密图**，所以用朴素 Dijkstra 求最短路。

```py [sol1-Python3]
class Solution:
    def minimumCost(self, start: List[int], target: List[int], specialRoads: List[List[int]]) -> int:
        t = tuple(target)
        dis = defaultdict(lambda: inf)
        dis[tuple(start)] = 0
        vis = set()
        while True:
            v, dv = None, -1
            for p, d in dis.items():
                if p not in vis and (dv < 0 or d < dv):
                    v, dv = p, d
            if v == t: return dv  # 到终点的最短路已确定
            vis.add(v)
            vx, vy = v
            dis[t] = min(dis[t], dv + t[0] - vx + t[1] - vy)  # 更新到终点的最短路
            for x1, y1, x2, y2, cost in specialRoads:
                w = (x2, y2)
                dis[w] = min(dis[w], dv + abs(x1 - vx) + abs(y1 - vy) + cost)
```

```java [sol1-Java]
class Solution {
    public int minimumCost(int[] start, int[] target, int[][] specialRoads) {
        long t = (long) target[0] << 32 | target[1];
        var dis = new HashMap<Long, Integer>();
        dis.put(t, Integer.MAX_VALUE);
        dis.put((long) start[0] << 32 | start[1], 0);
        var vis = new HashSet<Long>();
        for (;;) {
            long v = -1;
            int dv = -1;
            for (var e : dis.entrySet())
                if (!vis.contains(e.getKey()) && (dv < 0 || e.getValue() < dv)) {
                    v = e.getKey();
                    dv = e.getValue();                
                }
            if (v == t) return dv; // 到终点的最短路已确定
            vis.add(v);
            int vx = (int) (v >> 32), vy = (int) (v & Integer.MAX_VALUE);
            // 更新到终点的最短路
            dis.merge(t, dv + target[0] - vx + target[1] - vy, Math::min);
            for (var r : specialRoads) {
                int d = dv + Math.abs(r[0] - vx) + Math.abs(r[1] - vy) + r[4];
                long w = (long) r[2] << 32 | r[3];
                if (d < dis.getOrDefault(w, Integer.MAX_VALUE))
                    dis.put(w, d);
            }
        }
    }
}
```

```cpp [sol1-C++]
class Solution {
public:
    int minimumCost(vector<int> &start, vector<int> &target, vector <vector<int>> &specialRoads) {
        using LL = long long;
        LL t = (LL) target[0] << 32 | target[1];
        unordered_map<LL, int> dis = {{(LL) start[0] << 32 | start[1], 0}, {t, INT_MAX}};
        unordered_set<LL> vis;
        for (;;) {
            LL v = -1;
            int dv = -1;
            for (auto &[p, d]: dis)
                if (!vis.count(p) && (dv < 0 || d < dv))
                    v = p, dv = d;
            if (v == t) return dv; // 到终点的最短路已确定
            vis.insert(v);
            int vx = v >> 32, vy = v & UINT32_MAX;
            // 更新到终点的最短路
            dis[t] = min(dis[t], dv + target[0] - vx + target[1] - vy);
            for (auto &r: specialRoads) {
                int d = dv + abs(r[0] - vx) + abs(r[1] - vy) + r[4];
                LL w = (LL) r[2] << 32 | r[3];
                if (!dis.count(w) || d < dis[w])
                    dis[w] = d;
            }
        }
    }
};
```

```go [sol1-Go]
func minimumCost(start, target []int, specialRoads [][]int) int {
	type pair struct{ x, y int }
	t := pair{target[0], target[1]}
	dis := make(map[pair]int, len(specialRoads)+2)
	dis[t] = math.MaxInt
	dis[pair{start[0], start[1]}] = 0
	vis := make(map[pair]bool, len(specialRoads)+1) // 终点不用记
	for {
		v, dv := pair{}, -1
		for p, d := range dis {
			if !vis[p] && (dv < 0 || d < dv) {
				v, dv = p, d
			}
		}
		if v == t { // 到终点的最短路已确定
			return dv
		}
		vis[v] = true
		dis[t] = min(dis[t], dv+t.x-v.x+t.y-v.y) // 更新到终点的最短路
		for _, r := range specialRoads {
			w := pair{r[2], r[3]}
			d := dv + abs(r[0]-v.x) + abs(r[1]-v.y) + r[4]
			if dw, ok := dis[w]; !ok || d < dw {
				dis[w] = d
			}
		}
	}
}

func abs(x int) int { if x < 0 { return -x }; return x }
func min(a, b int) int { if b < a { return b }; return a }
```

### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2)$，其中 $n$ 为 $\textit{specialRoads}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 本地原创解析

### 1. 题意重述

本题来自 `三、最短路 / §3.1 单源最短路：Dijkstra 算法`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `三、最短路 / §3.1 单源最短路：Dijkstra 算法`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
