# 407. 接雨水 II

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/trapping-rain-water-ii/
- 题目 slug：`trapping-rain-water-ii`
- 来源专题：常用数据结构
- 来源分类路径：五、堆（优先队列） / §5.2 进阶
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/trapping-rain-water-ii/solutions/2998212/duan-ban-xiao-ying-pythonjavacgojsrust-b-39mp/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[短板效应（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/trapping-rain-water-ii/solutions/2998212/duan-ban-xiao-ying-pythonjavacgojsrust-b-39mp/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`duan-ban-xiao-ying-pythonjavacgojsrust-b-39mp`
- topic id：`2998212`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

**前置题目**：[42. 接雨水](https://leetcode.cn/problems/trapping-rain-water/)。建议先掌握那题的**双指针**写法，再来做本题。

![Buckets-effect-1024x937.webp](https://pic.leetcode.cn/1732354284-EMYCNx-Buckets-effect-1024x937.webp){:width=400}

哪个格子的接水量，在一开始就能确定？

- 最外面一圈的格子是无法接水的。
- 假设 $(0,1)$ 的高度是最外面一圈的格子中最小的，且高度等于 $5$，那么和它相邻的 $(1,1)$，我们能知道：
    - $(1,1)$ 的水位不能超过 $5$，否则水会从 $(0,1)$ 流出去。
    - $(1,1)$ 的水位一定可以等于 $5$，这是因为 $(0,1)$ 的高度是最外面一圈的格子中最小的，$(1,1)$ 的水不会从其他地方流出去。

我们从最外面一圈的格子开始。想象成一个木桶，最外面一圈格子的高度视作木板的高度。

接着上面的讨论：

- 如果 $(1,1)$ 的高度 $\ge 5$，那么 $(0,1)$ 这块木板就没用了，我们去掉 $(0,1)$ 这块木板，改用 $(1,1)$ 这块木板。
- 如果 $(1,1)$ 的高度 $< 5$，假设我们接的不是水，是**水泥**。那么把 $(1,1)$ 的高度填充为 $5$，仍然可以去掉 $(0,1)$ 这块木板，改用 $(1,1)$ 这块（填充水泥后）高为 $5$ 的~~木板~~水泥板。

继续，从当前木板中，找到一根最短的木板。假设 $(1,1)$ 是当前所有木板中最短的，那么其邻居 $(1,2)$ 和 $(2,1)$ 的水位就是 $(1,1)$ 的高度，因为超过 $(1,1)$ 高度的水会流出去。然后，去掉 $(1,1)$ 这块木板，改用 $(1,2)$ 和 $(2,1)$ 这两块木板。依此类推。

由于每次都要找最短的木板，所以用一个**最小堆**维护木板的高度。按照上述做法，不断循环，直到堆为空。

> 为方便实现，代码在初始化堆的时候，直接遍历了整个矩阵。只遍历最外面一圈的写法可以参考 Python3 的写法二。

### 答疑

**问**：这种思路和 [42. 接雨水](https://leetcode.cn/problems/trapping-rain-water/) 的双指针做法的联系是什么？

**答**：42 那题只需要维护左右两个指针，本题相当于维护了“一圈”指针。42 那题每次取左右最小的指针，然后移动到相邻位置上；本题也是取最小的指针（出堆），往周围的邻居移动（入堆）。

```py [sol-Python3]
class Solution:
    def trapRainWater(self, heightMap: List[List[int]]) -> int:
        m, n = len(heightMap), len(heightMap[0])
        h = []
        for i, row in enumerate(heightMap):
            for j, height in enumerate(row):
                if i == 0 or i == m - 1 or j == 0 or j == n - 1:
                    h.append((height, i, j))
                    row[j] = -1  # 标记 (i,j) 访问过
        heapify(h)

        ans = 0
        while h:
            min_height, i, j = heappop(h)  # min_height 是木桶的短板
            for x, y in (i, j - 1), (i, j + 1), (i - 1, j), (i + 1, j):
                if 0 <= x < m and 0 <= y < n and heightMap[x][y] >= 0:  # (x,y) 没有访问过
                    # 如果 (x,y) 的高度小于 min_height，那么接水量为 min_height - heightMap[x][y]
                    ans += max(min_height - heightMap[x][y], 0)
                    # 给木桶新增一块高为 max(min_height, heightMap[x][y]) 的木板
                    heappush(h, (max(min_height, heightMap[x][y]), x, y))
                    heightMap[x][y] = -1  # 标记 (x,y) 访问过
        return ans
```

```py [sol-Python3 写法二]
class Solution:
    def trapRainWater(self, heightMap: List[List[int]]) -> int:
        m, n = len(heightMap), len(heightMap[0])
        h = []
        for j in range(n):
            h.append((heightMap[0][j], 0, j))  # 上边
            h.append((heightMap[-1][j], m - 1, j))  # 下边
            heightMap[0][j] = heightMap[-1][j] = -1
        for i in range(1, m - 1):
            h.append((heightMap[i][0], i, 0))  # 左边
            h.append((heightMap[i][-1], i, n - 1))  # 右边
            heightMap[i][0] = heightMap[i][-1] = -1
        heapify(h)

        ans = 0
        while h:
            min_height, i, j = heappop(h)  # min_height 是木桶的短板
            for x, y in (i, j - 1), (i, j + 1), (i - 1, j), (i + 1, j):
                if 0 <= x < m and 0 <= y < n and heightMap[x][y] >= 0:  # (x,y) 没有访问过
                    # 如果 (x,y) 的高度小于 min_height，那么接水量为 min_height - heightMap[x][y]
                    ans += max(min_height - heightMap[x][y], 0)
                    # 给木桶新增一块高为 max(min_height, heightMap[x][y]) 的木板
                    heappush(h, (max(min_height, heightMap[x][y]), x, y))
                    heightMap[x][y] = -1  # 标记 (x,y) 访问过
        return ans
```

```java [sol-Java]
class Solution {
    private static final int[][] DIRS = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};

    public int trapRainWater(int[][] heightMap) {
        int m = heightMap.length, n = heightMap[0].length;
        PriorityQueue<int[]> pq = new PriorityQueue<>((a, b) -> a[0] - b[0]);
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                if (i == 0 || i == m - 1 || j == 0 || j == n - 1) {
                    pq.add(new int[]{heightMap[i][j], i, j});
                    heightMap[i][j] = -1; // 标记 (i,j) 访问过
                }
            }
        }

        int ans = 0;
        while (!pq.isEmpty()) {
            int[] t = pq.poll(); // 去掉短板
            int minHeight = t[0], i = t[1], j = t[2]; // minHeight 是木桶的短板
            for (int[] d : DIRS) {
                int x = i + d[0], y = j + d[1]; // (i,j) 的邻居
                if (0 <= x && x < m && 0 <= y && y < n && heightMap[x][y] >= 0) { // (x,y) 没有访问过
                    // 如果 (x,y) 的高度小于 minHeight，那么接水量为 minHeight - heightMap[x][y]
                    ans += Math.max(minHeight - heightMap[x][y], 0);
                    // 给木桶新增一块高为 max(minHeight, heightMap[x][y]) 的木板
                    pq.add(new int[]{Math.max(minHeight, heightMap[x][y]), x, y});
                    heightMap[x][y] = -1; // 标记 (x,y) 访问过
                }
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
    static constexpr int DIRS[4][2] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};
public:
    int trapRainWater(vector<vector<int>>& heightMap) {
        int m = heightMap.size(), n = heightMap[0].size();
        priority_queue<tuple<int, int, int>, vector<tuple<int, int, int>>, greater<>> pq;
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                if (i == 0 || i == m - 1 || j == 0 || j == n - 1) {
                    pq.emplace(heightMap[i][j], i, j);
                    heightMap[i][j] = -1; // 标记 (i,j) 访问过
                }
            }
        }

        int ans = 0;
        while (!pq.empty()) {
            auto [min_height, i, j] = pq.top(); // min_height 是木桶的短板
            pq.pop(); // 去掉短板
            for (auto& [dx, dy] : DIRS) {
                int x = i + dx, y = j + dy; // (i,j) 的邻居
                if (0 <= x && x < m && 0 <= y && y < n && heightMap[x][y] >= 0) { // (x,y) 没有访问过
                    // 如果 (x,y) 的高度小于 min_height，那么接水量为 min_height - heightMap[x][y]
                    ans += max(min_height - heightMap[x][y], 0);
                    // 给木桶新增一块高为 max(min_height, heightMap[x][y]) 的木板
                    pq.emplace(max(min_height, heightMap[x][y]), x, y);
                    heightMap[x][y] = -1; // 标记 (x,y) 访问过
                }
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
var dir4 = []struct{ x, y int }{{0, -1}, {0, 1}, {-1, 0}, {1, 0}}

func trapRainWater(heightMap [][]int) (ans int) {
    m, n := len(heightMap), len(heightMap[0])
    h := hp{}
    for i, row := range heightMap {
        for j, height := range row {
            if i == 0 || i == m-1 || j == 0 || j == n-1 {
                h = append(h, cell{height, i, j})
                row[j] = -1 // 标记 (i,j) 访问过
            }
        }
    }
    heap.Init(&h)

    for len(h) > 0 {
        c := heap.Pop(&h).(cell) // 去掉短板
        minHeight, i, j := c.height, c.x, c.y // minHeight 是木桶的短板
        for _, d := range dir4 {
            x, y := i+d.x, j+d.y // (i,j) 的邻居
            if 0 <= x && x < m && 0 <= y && y < n && heightMap[x][y] >= 0 { // (x,y) 没有访问过
                // 如果 (x,y) 的高度小于 minHeight，那么接水量为 minHeight - heightMap[x][y]
                ans += max(minHeight-heightMap[x][y], 0)
                // 给木桶新增一块高为 max(minHeight, heightMap[x][y]) 的木板
                heap.Push(&h, cell{max(minHeight, heightMap[x][y]), x, y})
                heightMap[x][y] = -1 // 标记 (x,y) 访问过
            }
        }
    }
    return
}

type cell struct{ height, x, y int }
type hp []cell
func (h hp) Len() int           { return len(h) }
func (h hp) Less(i, j int) bool { return h[i].height < h[j].height }
func (h hp) Swap(i, j int)      { h[i], h[j] = h[j], h[i] }
func (h *hp) Push(v any)        { *h = append(*h, v.(cell)) }
func (h *hp) Pop() any          { a := *h; v := a[len(a)-1]; *h = a[:len(a)-1]; return v }
```

```js [sol-JavaScript]
var trapRainWater = function(heightMap) {
    const m = heightMap.length, n = heightMap[0].length;
    const pq = new MinPriorityQueue(e => e[0]);
    for (let i = 0; i < m; i++) {
        for (let j = 0; j < n; j++) {
            if (i === 0 || i === m - 1 || j === 0 || j === n - 1) {
                pq.enqueue([heightMap[i][j], i, j]);
                heightMap[i][j] = -1; // 标记 (i,j) 访问过
            }
        }
    }

    let ans = 0;
    while (!pq.isEmpty()) {
        const [minHeight, i, j] = pq.dequeue(); // 去掉短板
        for (const [x, y] of [[i, j - 1], [i, j + 1], [i - 1, j], [i + 1, j]]) {
            if (0 <= x && x < m && 0 <= y && y < n && heightMap[x][y] >= 0) { // (x,y) 没有访问过
                // 如果 (x,y) 的高度小于 minHeight，那么接水量为 minHeight - heightMap[x][y]
                ans += Math.max(minHeight - heightMap[x][y], 0);
                // 给木桶新增一块高为 max(minHeight, heightMap[x][y]) 的木板
                pq.enqueue([Math.max(minHeight, heightMap[x][y]), x, y]);
                heightMap[x][y] = -1; // 标记 (x,y) 访问过
            }
        }
    }
    return ans;
};
```

```rust [sol-Rust]
use std::collections::BinaryHeap;

impl Solution {
    pub fn trap_rain_water(mut height_map: Vec<Vec<i32>>) -> i32 {
        let m = height_map.len();
        let n = height_map[0].len();
        let mut h = BinaryHeap::new();
        for (i, row) in height_map.iter_mut().enumerate() {
            for (j, height) in row.iter_mut().enumerate() {
                if i == 0 || i == m - 1 || j == 0 || j == n - 1 {
                    h.push((-*height, i, j)); // 取相反数变成最小堆
                    *height = -1; // 标记 (i,j) 访问过
                }
            }
        }

        let mut ans = 0;
        while let Some((min_height, i, j)) = h.pop() { // 去掉短板
            let min_height = -min_height; // min_height 是木桶的短板
            for (x, y) in [(i, j - 1), (i, j + 1), (i - 1, j), (i + 1, j)] {
                if x < m && y < n && height_map[x][y] >= 0 { // (x,y) 没有访问过
                    // 如果 (x,y) 的高度小于 min_height，那么接水量为 min_height - heightMap[x][y]
                    ans += 0.max(min_height - height_map[x][y]);
                    // 给木桶新增一块高为 max(min_height, heightMap[x][y]) 的木板
                    h.push((-min_height.max(height_map[x][y]), x, y));
                    height_map[x][y] = -1; // 标记 (x,y) 访问过
                }
            }
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(mn\log (mn))$，其中 $m$ 和 $n$ 分别为 $\textit{heightMap}$ 的行数和列数。每次出堆入堆需要 $\mathcal{O}(\log (mn))$ 的时间。
- 空间复杂度：$\mathcal{O}(mn)$。

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

本题来自 `五、堆（优先队列） / §5.2 进阶`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `五、堆（优先队列） / §5.2 进阶`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
