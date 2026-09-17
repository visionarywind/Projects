# 354. 俄罗斯套娃信封问题

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/russian-doll-envelopes/
- 题目 slug：`russian-doll-envelopes`
- 来源专题：动态规划
- 来源分类路径：四、经典线性 DP / §4.2 最长递增子序列（LIS） / §4.2.2 进阶
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/russian-doll-envelopes/solutions/3785353/qiao-miao-pai-xu-zhuan-hua-cheng-yi-wei-25mb0/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[巧妙排序，转化成一维 LIS 问题（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/russian-doll-envelopes/solutions/3785353/qiao-miao-pai-xu-zhuan-hua-cheng-yi-wei-25mb0/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`qiao-miao-pai-xu-zhuan-hua-cheng-yi-wei-25mb0`
- topic id：`3785353`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 16:46:02 +0800

## 从特殊到一般

先考虑原问题的一个简单版本：所有宽度 $w_i$ 互不相同。

如果按照宽度 $w_i$ 从小到大排序，比如

$$
\textit{envelopes} = [[1,3],[2,1],[3,4],[4,5]]
$$

那么只能把左边的信封装在右边的信封中。这样排序后，就只需关注 $h_i$ 的大小关系啦！也就是从

$$
h = [3,1,4,5]
$$

中选一个子序列，满足左边信封的高度小于右边信封的高度，即**严格递增子序列**。

题目要选**最多**的信封，即 [300. 最长递增子序列](https://leetcode.cn/problems/longest-increasing-subsequence/)。

## 回到原问题

对于相同的 $w_i$，应该怎么处理？

相同的 $w_i$，至多选一个信封。对于相同的 $w_i$，如果按照 $h_i$ **降序排序**，就可以保证至多有一个信封出现在最长递增子序列中了。比如

$$
\textit{envelopes} = [[1,3],[2,4],[2,3],[2,2],[3,5]]
$$

按照上述规则排序后，得到的 

$$
h = [3,4,3,2,5]
$$

其中的 $4,3,2$ 是降序，至多有一个在最长递增子序列中。这符合题目要求，因为信封 $4,3,2$ 的宽度都是 $2$，至多能选一个。

对比来看，如果相同的 $w_i$，按照 $h_i$ 升序排序，那么

$$
\textit{envelopes} = [[1,3],[2,2],[2,3],[2,4],[3,5]]
$$

得到

$$
h = [3,2,3,4,5]
$$

最长递增子序列是 $[2,3,4,5]$，这就搞错了，选了多个相同宽度的信封。

## 总结

先按照 $w_i$ 从小到大排序，相同的 $w_i$ 按照 $h_i$ 从大到小排序。

排序后，对 $h_i$ 计算 [300. 最长递增子序列](https://leetcode.cn/problems/longest-increasing-subsequence/)。

本题 $n\le 10^5$，需要用二分查找优化，请看[【基础算法精讲 20】](https://www.bilibili.com/video/BV1ub411Q7sB/)。

```py [sol-Python3]
class Solution:
    def maxEnvelopes(self, envelopes: List[List[int]]) -> int:
        # 双关键字排序：宽度升序，高度降序
        envelopes.sort(key=lambda e: (e[0], -e[1]))

        # 300. 最长递增子序列
        g = []
        for _, h in envelopes:
            j = bisect_left(g, h)
            if j < len(g):
                g[j] = h
            else:
                g.append(h)
        return len(g)
```

```java [sol-Java]
class Solution {
    public int maxEnvelopes(int[][] envelopes) {
        // 双关键字排序：宽度升序，高度降序
        Arrays.sort(envelopes, (a, b) -> {
            if (a[0] == b[0]) {
                return b[1] - a[1];
            }
            return a[0] - b[0];
        });

        // 300. 最长递增子序列
        List<Integer> g = new ArrayList<>();
        for (int[] e : envelopes) {
            int h = e[1];
            int j = lowerBound(g, h);
            if (j < g.size()) {
                g.set(j, h);
            } else {
                g.add(h);
            }
        }
        return g.size();
    }

    private int lowerBound(List<Integer> g, int target) {
        int left = -1, right = g.size(); // 开区间 (left, right)
        while (left + 1 < right) { // 区间不为空
            // 循环不变量：
            // nums[left] < target
            // nums[right] >= target
            int mid = left + (right - left) / 2;
            if (g.get(mid) >= target) {
                right = mid; // 范围缩小到 (left, mid)
            } else {
                left = mid; // 范围缩小到 (mid, right)
            }
        }
        return right; // 或者 left+1
    }
}
```

```java [sol-Java 库函数]
class Solution {
    public int maxEnvelopes(int[][] envelopes) {
        // 双关键字排序：宽度升序，高度降序
        Arrays.sort(envelopes, (a, b) -> {
            if (a[0] == b[0]) {
                return b[1] - a[1];
            }
            return a[0] - b[0];
        });

        // 300. 最长递增子序列
        List<Integer> g = new ArrayList<>();
        for (int[] e : envelopes) {
            int h = e[1];
            int j = Collections.binarySearch(g, h);
            if (j < 0) {
                j = ~j;
            }
            if (j < g.size()) {
                g.set(j, h);
            } else {
                g.add(h);
            }
        }
        return g.size();
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxEnvelopes(vector<vector<int>>& envelopes) {
        // 双关键字排序：宽度升序，高度降序
        ranges::sort(envelopes, {}, [](auto& e) { return pair(e[0], -e[1]); });

        // 300. 最长递增子序列
        vector<int> g;
        for (auto& e : envelopes) {
            int h = e[1];
            auto it = ranges::lower_bound(g, h);
            if (it != g.end()) {
                *it = h;
            } else {
                g.push_back(h);
            }
        }
        return g.size();
    }
};
```

```go [sol-Go]
func maxEnvelopes(envelopes [][]int) int {
    // 双关键字排序：宽度升序，高度降序
    slices.SortFunc(envelopes, func(a, b []int) int {
        return cmp.Or(a[0]-b[0], b[1]-a[1]) 
    })

    // 300. 最长递增子序列
    g := []int{}
    for _, e := range envelopes {
        h := e[1]
        j := sort.SearchInts(g, h)
        if j < len(g) {
            g[j] = h
        } else {
            g = append(g, h)
        }
    }
    return len(g)
}
```

```js [sol-JavaScript]
var maxEnvelopes = function(envelopes) {
    // 双关键字排序：宽度升序，高度降序
    envelopes.sort((a, b) => a[0] - b[0] || b[1] - a[1]);

    // 300. 最长递增子序列
    const g = [];
    for (const [, h] of envelopes) {
        // 二分查找 g 中第一个 >= h 的位置
        const j = _.sortedIndex(g, h);
        if (j < g.length) {
            g[j] = h;
        } else {
            g.push(h);
        }
    }
    return g.length;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn max_envelopes(mut envelopes: Vec<Vec<i32>>) -> i32 {
        // 双关键字排序：宽度升序，高度降序
        envelopes.sort_unstable_by_key(|e| (e[0], -e[1]));

        // 300. 最长递增子序列
        let mut g = vec![];
        for e in envelopes {
            let h = e[1];
            let j = g.partition_point(|&x| x < h);
            if j < g.len() {
                g[j] = h;
            } else {
                g.push(h);
            }
        }
        g.len() as _
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n)$，其中 $n$ 是 $\textit{envelopes}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 专题训练

见下面动态规划题单的「**§4.2 最长递增子序列（LIS）**」。

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

本题来自 `四、经典线性 DP / §4.2 最长递增子序列（LIS） / §4.2.2 进阶`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `四、经典线性 DP / §4.2 最长递增子序列（LIS） / §4.2.2 进阶`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
