# 2931. 购买物品的最大开销

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-spending-after-buying-items/
- 题目 slug：`maximum-spending-after-buying-items`
- 来源专题：贪心与思维
- 来源分类路径：四、数学贪心 / §4.3 排序不等式
- 难度分：1822
- 外部题解来源：https://leetcode.cn/problems/maximum-spending-after-buying-items/solutions/2522953/liang-chong-fang-fa-pai-xu-zui-xiao-dui-bzedc/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：排序/最小堆（Python/Java/C++/Go）](https://leetcode.cn/problems/maximum-spending-after-buying-items/solutions/2522953/liang-chong-fang-fa-pai-xu-zui-xiao-dui-bzedc/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`liang-chong-fang-fa-pai-xu-zui-xiao-dui-bzedc`
- topic id：`2522953`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:01:04 +0800

[视频讲解](https://www.bilibili.com/video/BV1Ww411T7JP/) 第四题。

## 思路

根据 [排序不等式](https://baike.baidu.com/item/%E6%8E%92%E5%BA%8F%E4%B8%8D%E7%AD%89%E5%BC%8F/7775728)，$\textit{values}[i][j]$ 越小的数，应该越早购买。

最小的数在哪？

$\textit{values}$ 的最后一列一定包含最小的数，因为其余列的数都不会比最后一列的这 $m$ 个数小。

次小的数在哪？

去掉最小的数，然后考虑每行的最后一个数，这 $m$ 个数中一定包含次小的数，因为和上面一样，其余数字都不会比这 $m$ 个数小。

依此类推，所以我们一定可以按照 $\textit{values}[i][j]$ 从小到大的顺序取到所有元素。

那么把所有数合并到一个数组中，然后排序计算。

## 写法一：排序

```py [sol-Python3]
class Solution:
    def maxSpending(self, values: List[List[int]]) -> int:
        a = sorted(x for row in values for x in row)
        return sum(x * i for i, x in enumerate(a, 1))
```

```java [sol-Java]
class Solution {
    public long maxSpending(int[][] values) {
        int m = values.length;
        int n = values[0].length;
        int[] a = new int[m * n];
        for (int i = 0; i < m; i++) {
            System.arraycopy(values[i], 0, a, i * n, n);
        }
        Arrays.sort(a);

        long ans = 0;
        for (int i = 0; i < a.length; i++) {
            ans += (long) a[i] * (i + 1);
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long maxSpending(vector<vector<int>>& values) {
        int m = values.size(), n = values[0].size();
        vector<int> a;
        a.reserve(m * n); // 预分配空间
        for (auto& row : values) {
            a.insert(a.end(), row.begin(), row.end());
        }
        ranges::sort(a);

        long long ans = 0;
        for (int i = 0; i < a.size(); i++) {
            ans += (long long) a[i] * (i + 1);
        }
        return ans;
    }
};
```

```go [sol-Go]
func maxSpending(values [][]int) (ans int64) {
    m, n := len(values), len(values[0])
    a := make([]int, 0, m*n) // 预分配空间
    for _, row := range values {
        a = append(a, row...)
    }
    slices.Sort(a)

    for i, x := range a {
        ans += int64(x) * int64(i+1)
    }
    return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(mn\log (mn))$，其中 $m$ 和 $n$ 分别为 $\textit{values}$ 的行数和列数。
- 空间复杂度：$\mathcal{O}(mn)$。

## 写法二：最小堆

也可以用最小堆模拟取数的流程。

```py [sol-Python3]
class Solution:
    def maxSpending(self, values: List[List[int]]) -> int:
        h = [(a[-1], i) for i, a in enumerate(values)]
        heapify(h)
        ans = 0
        for d in range(1, len(values) * len(values[0]) + 1):
            v, i = heappop(h)
            ans += v * d
            values[i].pop()
            if values[i]:
                heappush(h, (values[i][-1], i))
        return ans
```

```java [sol-Java]
class Solution {
    public long maxSpending(int[][] values) {
        int m = values.length;
        int n = values[0].length;
        PriorityQueue<int[]> pq = new PriorityQueue<>((a, b) -> values[a[0]][a[1]] - values[b[0]][b[1]]);
        for (int i = 0; i < m; i++) {
            pq.offer(new int[]{i, n - 1});
        }

        long ans = 0;
        for (int d = 1; d <= m * n; d++) {
            int[] p = pq.poll();
            int i = p[0];
            int j = p[1];
            ans += (long) values[i][j] * d;
            if (j > 0) {
                pq.offer(new int[]{i, j - 1});
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long maxSpending(vector<vector<int>>& values) {
        int m = values.size(), n = values[0].size();
        priority_queue<pair<int, int>, vector<pair<int, int>>, greater<>> pq;
        for (int i = 0; i < m; i++) {
            pq.emplace(values[i].back(), i);
        }

        long long ans = 0;
        for (int d = 1; d <= m * n; d++) {
            auto [v, i] = pq.top();
            pq.pop();
            ans += (long long) v * d;
            values[i].pop_back();
            if (!values[i].empty()) {
                pq.emplace(values[i].back(), i);
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func maxSpending(values [][]int) (ans int64) {
    m, n := len(values), len(values[0])
    idx := make([]int, m)
    for i := range idx {
        idx[i] = i
    }
    h := &hp{idx, values}
    heap.Init(h)

    for d := 1; d <= m*n; d++ {
        a := values[idx[0]]
        ans += int64(a[len(a)-1]) * int64(d)
        if len(a) > 1 {
            values[idx[0]] = a[:len(a)-1]
            heap.Fix(h, 0)
        } else {
            heap.Pop(h)
        }
    }
    return
}

type hp struct {
    sort.IntSlice
    values [][]int
}

func (h hp) Less(i, j int) bool {
    a, b := h.values[h.IntSlice[i]], h.values[h.IntSlice[j]]
    return a[len(a)-1] < b[len(b)-1]
}
func (hp) Push(any) {}
func (h *hp) Pop() (_ any) { a := h.IntSlice; h.IntSlice = a[:len(a)-1]; return }
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(mn\log m)$，其中 $m$ 和 $n$ 分别为 $\textit{values}$ 的行数和列数。
- 空间复杂度：$\mathcal{O}(m)$。

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
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `四、数学贪心 / §4.3 排序不等式`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `四、数学贪心 / §4.3 排序不等式`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
