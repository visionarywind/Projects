# 815. 公交路线

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/bus-routes/
- 题目 slug：`bus-routes`
- 来源专题：图论算法
- 来源分类路径：一、图的遍历 / §1.2 广度优先搜索（BFS）
- 难度分：1964
- 外部题解来源：https://leetcode.cn/problems/bus-routes/solutions/2916806/tu-jie-bfspythonjavacgojsrust-by-endless-t7oc/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【图解】BFS 线性做法（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/bus-routes/solutions/2916806/tu-jie-bfspythonjavacgojsrust-by-endless-t7oc/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tu-jie-bfspythonjavacgojsrust-by-endless-t7oc`
- topic id：`2916806`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 思路

![lc815-c.png](https://pic.leetcode.cn/1726294233-KCZYIg-lc815-c.png)

代码实现时，为方便起见，车站编号是一个一个出队的。

## 细节

**有哪些公交车会经过车站 $x$？**

创建一个哈希表 $\textit{stopToBuses}$，key 为车站编号，value 为经过该车站的公交车编号列表。

遍历第 $i$ 辆公交车的路线 $\textit{routes}[i]$，对于车站 $x=\textit{routes}[i][j]$，把公交车编号 $i$ 加到 $\textit{stopToBuses}[x]$ 列表中。

**在 BFS 中，如何保证每辆公交车的路线只遍历一次？**

可以创建一个 $\textit{vis}$ 数组。更简单的办法是，当公交车路线 $\textit{routes}[i]$ 遍历结束后，把 $\textit{routes}[i]$ 置为空。

**在 BFS 中，如何保证每个车站只入队一次？**

为了记录起点到每个站的最短路（最少乘坐的公交车数量），创建一个哈希表 $\textit{dis}$，key 为车站编号，value 为起点到该车站的最短路。

我们可以利用 $\textit{dis}$ 来知道车站 $x$ 是否入队过：看 $x$ 是否在 $\textit{dis}$ 中即可。

**小优化**

如果没有公交车经过起点或终点，直接返回：

- 如果 $\textit{source}\ne \textit{target}$，无法从起点到达终点，返回 $-1$。
- 如果 $\textit{source}=\textit{target}$，返回 $0$。

```py [sol-Python3]
class Solution:
    def numBusesToDestination(self, routes: List[List[int]], source: int, target: int) -> int:
        # 记录经过车站 x 的公交车编号
        stop_to_buses = defaultdict(list)
        for i, route in enumerate(routes):
            for x in route:
                stop_to_buses[x].append(i)

        # 小优化：如果没有公交车经过起点或终点，直接返回
        if source not in stop_to_buses or target not in stop_to_buses:
            # 注意原地 TP 的情况
            return -1 if source != target else 0

        # BFS
        dis = {source: 0}
        q = deque([source])
        while q:
            x = q.popleft()  # 当前在车站 x
            dis_x = dis[x]
            for i in stop_to_buses[x]:  # 遍历所有经过车站 x 的公交车 i
                if routes[i]:
                    for y in routes[i]:  # 遍历公交车 i 的路线
                        if y not in dis:  # 没有访问过车站 y
                            dis[y] = dis_x + 1  # 从 x 站上车然后在 y 站下车
                            q.append(y)
                    routes[i] = None  # 标记 routes[i] 遍历过

        return dis.get(target, -1)
```

```java [sol-Java]
class Solution {
    public int numBusesToDestination(int[][] routes, int source, int target) {
        // 记录经过车站 x 的公交车编号
        Map<Integer, List<Integer>> stopToBuses = new HashMap<>();
        for (int i = 0; i < routes.length; i++) {
            for (int x : routes[i]) {
                stopToBuses.computeIfAbsent(x, k -> new ArrayList<>()).add(i);
            }
        }

        // 小优化：如果没有公交车经过起点或终点，直接返回
        if (!stopToBuses.containsKey(source) || !stopToBuses.containsKey(target)) {
            // 注意原地 TP 的情况
            return source != target ? -1 : 0;
        }

        // BFS
        Map<Integer, Integer> dis = new HashMap<>();
        dis.put(source, 0);
        Queue<Integer> q = new ArrayDeque<>();
        q.add(source);
        while (!q.isEmpty()) {
            int x = q.poll(); // 当前在车站 x
            int disX = dis.get(x);
            for (int i : stopToBuses.get(x)) { // 遍历所有经过车站 x 的公交车 i
                if (routes[i] != null) {
                    for (int y : routes[i]) { // 遍历公交车 i 的路线
                        if (!dis.containsKey(y)) { // 没有访问过车站 y
                            dis.put(y, disX + 1); // 从 x 站上车然后在 y 站下车
                            q.add(y);
                        }
                    }
                    routes[i] = null; // 标记 routes[i] 遍历过
                }
            }
        }

        return dis.getOrDefault(target, -1);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int numBusesToDestination(vector<vector<int>>& routes, int source, int target) {
        // 记录经过车站 x 的公交车编号
        unordered_map<int, vector<int>> stop_to_buses;
        for (int i = 0; i < routes.size(); i++) {
            for (int x : routes[i]) {
                stop_to_buses[x].push_back(i);
            }
        }

        // 小优化：如果没有公交车经过起点或终点，直接返回
        if (!stop_to_buses.contains(source) || !stop_to_buses.contains(target)) {
            // 注意原地 TP 的情况
            return source != target ? -1 : 0;
        }

        // BFS
        unordered_map<int, int> dis;
        dis[source] = 0;
        queue<int> q;
        q.push(source);
        while (!q.empty()) {
            int x = q.front(); // 当前在车站 x
            q.pop();
            int dis_x = dis[x];
            for (int i : stop_to_buses[x]) { // 遍历所有经过车站 x 的公交车 i
                for (int y : routes[i]) { // 遍历公交车 i 的路线
                    if (!dis.contains(y)) { // 没有访问过车站 y
                        dis[y] = dis_x + 1; // 从 x 站上车然后在 y 站下车
                        q.push(y);
                    }
                }
                routes[i].clear(); // 标记 routes[i] 遍历过
            }
        }

        return dis.contains(target) ? dis[target] : -1;
    }
};
```

```go [sol-Go]
func numBusesToDestination(routes [][]int, source, target int) int {
    // 记录经过车站 x 的公交车编号
    stopToBuses := map[int][]int{}
    for i, route := range routes {
        for _, x := range route {
            stopToBuses[x] = append(stopToBuses[x], i)
        }
    }

    // 小优化：如果没有公交车经过起点或终点，直接返回
    if stopToBuses[source] == nil || stopToBuses[target] == nil {
        if source != target {
            return -1
        }
        // 注意原地 TP 的情况
        return 0
    }

    // BFS
    dis := map[int]int{source: 0}
    q := []int{source}
    for len(q) > 0 {
        x := q[0] // 当前在车站 x
        q = q[1:]
        disX := dis[x]
        for _, i := range stopToBuses[x] { // 遍历所有经过车站 x 的公交车 i
            for _, y := range routes[i] { // 遍历公交车 i 的路线
                if _, ok := dis[y]; !ok { // 没有访问过车站 y
                    dis[y] = disX + 1 // 从 x 站上车然后在 y 站下车
                    q = append(q, y)
                }
            }
            routes[i] = nil // 标记 routes[i] 遍历过
        }
    }

    if d, ok := dis[target]; ok {
        return d
    }
    return -1
}
```

```js [sol-JavaScript]
var numBusesToDestination = function(routes, source, target) {
    // 记录经过车站 x 的公交车编号
    const stopToBuses = new Map();
    for (let i = 0; i < routes.length; i++) {
        for (const x of routes[i]) {
            if (!stopToBuses.has(x)) {
                stopToBuses.set(x, [i]);
            } else {
                stopToBuses.get(x).push(i);
            }
        }
    }

    // 小优化：如果没有公交车经过起点或终点，直接返回
    if (!stopToBuses.has(source) || !stopToBuses.has(target)) {
        // 注意原地 TP 的情况
        return source !== target ? -1 : 0;
    }

    // BFS
    const dis = new Map();
    dis.set(source, 0);
    const q = Array(stopToBuses.size); // 用数组模拟队列
    q[0] = source;
    let ql = 0, qr = 1; // 队首队尾，左闭右开区间
    while (ql < qr) {
        const x = q[ql++]; // 当前在车站 x
        const disX = dis.get(x);
        for (const i of stopToBuses.get(x)) { // 遍历所有经过车站 x 的公交车 i
            if (routes[i]) {
                for (const y of routes[i]) { // 遍历公交车 i 的路线
                    if (!dis.has(y)) { // 没有访问过车站 y
                        dis.set(y, disX + 1); // 从 x 站上车然后在 y 站下车
                        q[qr++] = y;
                    }
                }
                routes[i] = null; // 标记 routes[i] 遍历过
            }
        }
    }

    return dis.get(target) ?? -1;
};
```

```rust [sol-Rust]
use std::collections::{HashMap, VecDeque};

impl Solution {
    pub fn num_buses_to_destination(mut routes: Vec<Vec<i32>>, source: i32, target: i32) -> i32 {
        // 记录经过车站 x 的公交车编号
        let mut stop_to_buses: HashMap<i32, Vec<usize>> = HashMap::new();
        for (i, route) in routes.iter().enumerate() {
            for &x in route {
                stop_to_buses.entry(x).or_default().push(i);
            }
        }

        // 小优化：如果没有公交车经过起点或终点，直接返回
        if !stop_to_buses.contains_key(&source) || !stop_to_buses.contains_key(&target) {
            // 注意原地 TP 的情况
            return if source != target { -1 } else { 0 };
        }

        // BFS
        let mut dis = HashMap::new();
        dis.insert(source, 0);
        let mut q = VecDeque::new();
        q.push_back(source);
        while let Some(x) = q.pop_front() {
            let dis_x = dis[&x]; // 当前在车站 x
            for &i in &stop_to_buses[&x] { // 遍历所有经过车站 x 的公交车 i
                for &y in &routes[i] { // 遍历公交车 i 的路线
                    if !dis.contains_key(&y) { // 没有访问过车站 y
                        dis.insert(y, dis_x + 1); // 从 x 站上车然后在 y 站下车
                        q.push_back(y);
                    }
                }
                routes[i].clear(); // 标记 routes[i] 遍历过
            }
        }

        dis.get(&target).copied().unwrap_or(-1)
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(S)$，其中 $S$ 是所有 $\textit{routes}[i]$ 的长度之和。每条线路至多遍历一次，每个车站至多入队一次。
- 空间复杂度：$\mathcal{O}(S)$。

## 思考题

把乘坐的公交车的编号，即 $\textit{routes}$ 的下标，按乘坐顺序记录下来，得到一个列表 $\textit{order}$。

输出 $\textit{order}$。

如果从起点到终点有多条最短路，输出字典序最小的 $\textit{order}$。

欢迎在评论区分享你的思路/代码。

## 专题训练

见下面图论题单的「**BFS**」。

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

本题来自 `一、图的遍历 / §1.2 广度优先搜索（BFS）`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、图的遍历 / §1.2 广度优先搜索（BFS）`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
