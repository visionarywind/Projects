# 871. 最低加油次数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-number-of-refueling-stops/
- 题目 slug：`minimum-number-of-refueling-stops`
- 来源专题：常用数据结构
- 来源分类路径：五、堆（优先队列） / §5.5 反悔堆
- 难度分：2074
- 外部题解来源：https://leetcode.cn/problems/minimum-number-of-refueling-stops/solutions/2921064/zui-da-dui-tan-xin-pythonjavacgojsrust-b-yldp/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[最大堆贪心（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/minimum-number-of-refueling-stops/solutions/2921064/zui-da-dui-tan-xin-pythonjavacgojsrust-b-yldp/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`zui-da-dui-tan-xin-pythonjavacgojsrust-b-yldp`
- topic id：`2921064`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 思路

当汽车行驶到第 $i$ 个加油站时，视作**获取了一个装有** $\textit{fuel}_i$ **升汽油的油桶**。

在后续的行驶过程中，可以在没油时，把油桶中的油加到汽车中。

选哪个（哪些）油桶？

为了让加油次数尽量少，贪心地选油量多的油桶。

由于有添加和删除操作，用**最大堆**维护这些油桶。

## 细节

可以把终点 $\textit{target}$ 视作一个虚拟加油站，加到 $\textit{stations}$ 末尾，这样可以用同样的代码处理最后一段路程（从最后一个加油站到终点）。

可以提前判断 $\textit{startFuel}\ge \textit{target}$ 的情况，油量足以把车开到终点，直接返回 $0$。或者，在循环中判断 $\textit{curFuel}\ge \textit{target} - \textit{position}_i$ 的情况，此时可以退出循环。不过考虑到对实际运行时间没有影响，代码中没有写这些优化。

## 写法一

```py [sol-Python3]
class Solution:
    def minRefuelStops(self, target: int, startFuel: int, stations: List[List[int]]) -> int:
        stations.append((target, 0))
        ans = pre_position = 0
        cur_fuel = startFuel
        fuel_heap = []  # 下面把堆中元素取反，当作最大堆用
        for position, fuel in stations:
            cur_fuel -= position - pre_position  # 每行驶 1 英里用掉 1 升汽油
            while fuel_heap and cur_fuel < 0:  # 没油了
                cur_fuel -= heappop(fuel_heap)  # 选油量最多的油桶
                ans += 1
            if cur_fuel < 0:  # 无法到达
                return -1
            heappush(fuel_heap, -fuel)  # 留着后面加油
            pre_position = position
        return ans
```

```java [sol-Java]
class Solution {
    public int minRefuelStops(int target, int startFuel, int[][] stations) {
        int n = stations.length;
        int ans = 0;
        int prePosition = 0;
        int curFuel = startFuel;
        PriorityQueue<Integer> fuelHeap = new PriorityQueue<>((a, b) -> b - a); // 最大堆
        for (int i = 0; i <= n; i++) {
            int position = i < n ? stations[i][0] : target;
            curFuel -= position - prePosition; // 每行驶 1 英里用掉 1 升汽油
            while (!fuelHeap.isEmpty() && curFuel < 0) { // 没油了
                curFuel += fuelHeap.poll(); // 选油量最多的油桶
                ans++;
            }
            if (curFuel < 0) { // 无法到达
                return -1;
            }
            fuelHeap.offer(i < n ? stations[i][1] : 0); // 留着后面加油
            prePosition = position;
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minRefuelStops(int target, int startFuel, vector<vector<int>>& stations) {
        stations.push_back({target, 0});
        int ans = 0, pre_position = 0, cur_fuel = startFuel;
        priority_queue<int> fuel_heap;
        for (auto& station : stations) {
            int position = station[0];
            cur_fuel -= position - pre_position; // 每行驶 1 英里用掉 1 升汽油
            while (!fuel_heap.empty() && cur_fuel < 0) { // 没油了
                cur_fuel += fuel_heap.top(); // 选油量最多的油桶
                fuel_heap.pop();
                ans++;
            }
            if (cur_fuel < 0) { // 无法到达
                return -1;
            }
            fuel_heap.push(station[1]); // 留着后面加油
            pre_position = position;
        }
        return ans;
    }
};
```

```go [sol-Go]
func minRefuelStops(target, startFuel int, stations [][]int) (ans int) {
    stations = append(stations, []int{target, 0})
    prePosition, curFuel := 0, startFuel
    fuelHeap := &hp{}
    for _, station := range stations {
        position, fuel := station[0], station[1]
        curFuel -= position - prePosition       // 每行驶 1 英里用掉 1 升汽油
        for fuelHeap.Len() > 0 && curFuel < 0 { // 没油了
            curFuel += heap.Pop(fuelHeap).(int) // 选油量最多的油桶
            ans++
        }
        if curFuel < 0 { // 无法到达
            return -1
        }
        heap.Push(fuelHeap, fuel) // 留着后面加油
        prePosition = position
    }
    return
}

type hp struct{ sort.IntSlice }
func (h hp) Less(i, j int) bool { return h.IntSlice[i] > h.IntSlice[j] } // 最大堆
func (h *hp) Push(v any)        { h.IntSlice = append(h.IntSlice, v.(int)) }
func (h *hp) Pop() any          { a := h.IntSlice; v := a[len(a)-1]; h.IntSlice = a[:len(a)-1]; return v }
```

```js [sol-JavaScript]
var minRefuelStops = function(target, startFuel, stations) {
    stations.push([target, 0]);
    let ans = 0, prePosition = 0, curFuel = startFuel;
    const fuelHeap = new MaxPriorityQueue();
    for (const [position, fuel] of stations) {
        curFuel -= position - prePosition; // 每行驶 1 英里用掉 1 升汽油
        while (!fuelHeap.isEmpty() && curFuel < 0) { // 没油了
            curFuel += fuelHeap.dequeue(); // 选油量最多的油桶
            ans++;
        }
        if (curFuel < 0) { // 无法到达
            return -1;
        }
        fuelHeap.enqueue(fuel); // 留着后面加油
        prePosition = position;
    }
    return ans;
};
```

```rust [sol-Rust]
use std::collections::BinaryHeap;

impl Solution {
    pub fn min_refuel_stops(target: i32, start_fuel: i32, mut stations: Vec<Vec<i32>>) -> i32 {
        stations.push(vec![target, 0]);
        let mut ans = 0;
        let mut pre_position = 0;
        let mut cur_fuel = start_fuel; // 当前油量
        let mut fuel_heap = BinaryHeap::new(); // 最大堆
        for station in stations {
            let position = station[0];
            cur_fuel -= position - pre_position; // 每行驶 1 英里用掉 1 升汽油
            while cur_fuel < 0 && !fuel_heap.is_empty() { // 没油了
                cur_fuel += fuel_heap.pop().unwrap(); // 选油量最多的油桶
                ans += 1;
            }
            if cur_fuel < 0 { // 无法到达
                return -1;
            }
            fuel_heap.push(station[1]); // 留着后面加油
            pre_position = position;
        }
        ans
    }
}
```

## 写法二

定义 $\textit{miles}$ 表示可以行驶的距离，初始值为 $\textit{startFuel}$。

如果循环中发现 $\textit{miles} < \textit{position}_i$，就从把油桶中的油加到 $\textit{miles}$ 中，直到 $\textit{miles} \ge \textit{position}_i$。

```py [sol-Python3]
class Solution:
    def minRefuelStops(self, target: int, startFuel: int, stations: List[List[int]]) -> int:
        stations.append((target, 0))
        ans, miles = 0, startFuel
        fuel_heap = []  # 下面把堆中元素取反，当作最大堆用
        for position, fuel in stations:
            while fuel_heap and miles < position:  # 没有足够的油到达 position
                miles -= heappop(fuel_heap)  # 选油量最多的油桶
                ans += 1
            if miles < position:  # 无法到达
                return -1
            heappush(fuel_heap, -fuel)  # 留着后面加油
        return ans
```

```java [sol-Java]
class Solution {
    public int minRefuelStops(int target, int startFuel, int[][] stations) {
        int n = stations.length;
        int ans = 0;
        int miles = startFuel;
        PriorityQueue<Integer> fuelHeap = new PriorityQueue<>((a, b) -> b - a); // 最大堆
        for (int i = 0; i <= n; i++) {
            int position = i < n ? stations[i][0] : target;
            while (!fuelHeap.isEmpty() && miles < position) { // 没有足够的油到达 position
                miles += fuelHeap.poll(); // 选油量最多的油桶
                ans++;
            }
            if (miles < position) { // 无法到达
                return -1;
            }
            fuelHeap.offer(i < n ? stations[i][1] : 0); // 留着后面加油
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minRefuelStops(int target, int startFuel, vector<vector<int>>& stations) {
        stations.push_back({target, 0});
        int ans = 0, miles = startFuel;
        priority_queue<int> fuel_heap;
        for (auto& station : stations) {
            int position = station[0];
            while (!fuel_heap.empty() && miles < position) { // 没有足够的油到达 position
                miles += fuel_heap.top(); // 选油量最多的油桶
                fuel_heap.pop();
                ans++;
            }
            if (miles < position) { // 无法到达
                return -1;
            }
            fuel_heap.push(station[1]); // 留着后面加油
        }
        return ans;
    }
};
```

```go [sol-Go]
func minRefuelStops(target, startFuel int, stations [][]int) (ans int) {
    stations = append(stations, []int{target, 0})
    miles := startFuel
    fuelHeap := &hp{}
    for _, station := range stations {
        position, fuel := station[0], station[1]
        for fuelHeap.Len() > 0 && miles < position { // 没有足够的油到达 position
            miles += heap.Pop(fuelHeap).(int) // 选油量最多的油桶
            ans++
        }
        if miles < position { // 无法到达
            return -1
        }
        heap.Push(fuelHeap, fuel) // 留着后面加油
    }
    return
}

type hp struct{ sort.IntSlice }
func (h hp) Less(i, j int) bool { return h.IntSlice[i] > h.IntSlice[j] } // 最大堆
func (h *hp) Push(v any)        { h.IntSlice = append(h.IntSlice, v.(int)) }
func (h *hp) Pop() any          { a := h.IntSlice; v := a[len(a)-1]; h.IntSlice = a[:len(a)-1]; return v }
```

```js [sol-JavaScript]
var minRefuelStops = function(target, startFuel, stations) {
    stations.push([target, 0]);
    let ans = 0, miles = startFuel;
    const fuelHeap = new MaxPriorityQueue();
    for (const [position, fuel] of stations) {
        while (!fuelHeap.isEmpty() && miles < position) { // 没有足够的油到达 position
            miles += fuelHeap.dequeue(); // 选油量最多的油桶
            ans++;
        }
        if (miles < position) { // 无法到达
            return -1;
        }
        fuelHeap.enqueue(fuel); // 留着后面加油
    }
    return ans;
};
```

```rust [sol-Rust]
use std::collections::BinaryHeap;

impl Solution {
    pub fn min_refuel_stops(target: i32, start_fuel: i32, mut stations: Vec<Vec<i32>>) -> i32 {
        stations.push(vec![target, 0]);
        let mut ans = 0;
        let mut miles = start_fuel;
        let mut fuel_heap = BinaryHeap::new();
        for station in stations {
            let position = station[0];
            while !fuel_heap.is_empty() && miles < position { // 没有足够的油到达 position
                miles += fuel_heap.pop().unwrap(); // 选油量最多的油桶
                ans += 1;
            }
            if miles < position { // 无法到达
                return -1;
            }
            fuel_heap.push(station[1]); // 留着后面加油
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n)$，其中 $n$ 是 $\textit{stations}$ 的长度。虽然写了个二重循环，但每个油量入堆一次，所以出堆也至多一次，所以二重循环只会循环 $\mathcal{O}(n)$ 次，每次操作堆的时间是 $\mathcal{O}(\log n)$，因此总的时间复杂度为 $\mathcal{O}(n\log n)$。
- 空间复杂度：$\mathcal{O}(n)$。

更多相似题目，见下面贪心题单中的「**§1.9 反悔贪心**」。

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

本题来自 `五、堆（优先队列） / §5.5 反悔堆`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `五、堆（优先队列） / §5.5 反悔堆`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
