# 1840. 最高建筑高度

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-building-height/
- 题目 slug：`maximum-building-height`
- 来源专题：贪心与思维
- 来源分类路径：五、思维题 / §5.6 两次扫描
- 难度分：2374
- 外部题解来源：https://leetcode.cn/problems/maximum-building-height/solutions/3981306/liang-ci-sao-miao-chuan-bo-yue-shu-pytho-kigb/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两次扫描，传播约束（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/maximum-building-height/solutions/3981306/liang-ci-sao-miao-chuan-bo-yue-shu-pytho-kigb/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`liang-ci-sao-miao-chuan-bo-yue-shu-pytho-kigb`
- topic id：`3981306`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:13:14 +0800

请先完成本题的简单版本 [3796. 找到带限制序列的最大值](https://leetcode.cn/problems/find-maximum-value-in-a-constrained-sequence/)，[我的题解](https://leetcode.cn/problems/find-maximum-value-in-a-constrained-sequence/solutions/3872153/liang-ci-sao-miao-fa-pythonjavacgo-by-en-p7qc/)。

本题相当于 3796 题中的 $\textit{diff}[i] = 1$，但 $n$ 高达 $10^9$，直接创建大小为 $n$ 的数组会超时/超内存。

## 核心思路

1. 用 3796 题的两次扫描法，算出每个编号为 $\textit{id}_i$ 的建筑的最大高度。注意建筑 $\textit{id}_i$ 不仅受到 $\textit{maxHeight}_i$ 的约束，还因为「相邻建筑的高度差不能超过 $1$」会受到其他 $\textit{maxHeight}_j$ 的约束。
2. 编号在 $\textit{id}_i$ 和 $\textit{id}_{i+1}$ 之间的建筑，最大高度**先递增再递减**，用数学方法可以 $\mathcal{O}(1)$ 算出峰顶的最大高度。

## 具体思路

定义 $H[i]$ 表示编号为 $\textit{id}_i$ 的建筑的最大高度。这可以用两次扫描法计算，见 3796 题。

设 $j$ 是 $[\textit{id}_i,\textit{id}_{i+1}]$ 中的一个整数。对于建筑 $j$，其高度 $h_j$ 受到 $a[i]$ 的约束，即 $h_j \le H[i] + j - \textit{id}_i$；同时也受到 $H[i+1]$ 的约束，即 $h_j \le H[i+1] + \textit{id}_{i+1} - j$。所以有

$$
h_j \le \min(H[i] + j - \textit{id}_i, H[i+1] + \textit{id}_{i+1} - j)
$$

哪个 $j$ 可以让 $h_j$ 的上界最大？

想象有两条斜率分别为 $1$ 和 $-1$ 的直线 $y = x - \textit{id}_i + H[i]$ 和 $y = -x + \textit{id}_{i+1} + H[i+1]$，交点横坐标（取整）即为最优的 $j$，即

$$
j = \left\lfloor\dfrac{\textit{id}_i + \textit{id}_{i+1} - H[i] + H[i+1]}{2}\right\rfloor
$$

代入斜率为 $1$ 的直线，得到最大高度为

$$
\begin{aligned}
    & H[i] - \textit{id}_i + \left\lfloor\dfrac{\textit{id}_i + \textit{id}_{i+1} - H[i] + H[i+1]}{2}\right\rfloor      \\
={} & \left\lfloor\dfrac{-\textit{id}_i + \textit{id}_{i+1} + H[i] + H[i+1]}{2}\right\rfloor        \\
\end{aligned}
$$

> **注 1**：也可以计算上取整，代入斜率为 $-1$ 的直线，计算结果是一样的。
> 
> **注 2**：另一种计算方法是，设峰顶编号为 $j$，从 $\textit{id}_i$ 到 $j$ 再到 $\textit{id}_{i+1}$ 是先递增再递减的（如果只是递增或递减，那么峰顶在端点）。从 $\textit{id}_i$ 到 $j$，建筑高度至多增加了 $h_j - H[i]$；从 $\textit{id}_{i+1}$ 到 $j$，建筑高度至多增加了 $h_j - H[i+1]$。这两个增量之和不超过 $\textit{id}_{i+1} - \textit{id}_i$，所以有
> 
> $$
> (h_j - H[i]) + (h_j - H[i+1]) \le \textit{id}_{i+1} - \textit{id}_i
> $$
> 
> 解得
> 
> $$
> h_j \le \left\lfloor\dfrac{\textit{id}_{i+1} - \textit{id}_i + H[i] + H[i+1]}{2}\right\rfloor
> $$

特别地，对于编号在 $1$ 和 $id_0$ 之间的建筑，最大高度为

$$
\left\lfloor\dfrac{\textit{id}_0 - 1 + H[0]}{2}\right\rfloor
$$

设 $m$ 是 $\textit{restrictions}$ 的长度。对于编号在 $id_{m-1}$ 和 $n$ 之间的建筑，编号为 $n$ 的建筑最高，高度为

$$
H[m-1] + n - id_{m-1}
$$

特殊情况：如果 $m=0$，返回建筑 $n$ 的最大高度 $n-1$。 

代码实现时，可以添加两个约束 $[1,0]$ 和 $[n,\infty]$，这样无需单独计算上述特殊情况。见 Python3 代码。

注意输入的 $\textit{id}_i$ 不是有序的，需要先排序。

```py [sol-Python3]
class Solution:
    def maxBuilding(self, n: int, restrictions: List[List[int]]) -> int:
        restrictions += [[1, 0], [n, inf]]
        restrictions.sort(key=lambda a: a[0])  # 按照 id 从小大到大排序

        m = len(restrictions)
        h = [0] * m  # h[i] 表示编号为 id[i] 的建筑的最大高度
        for i in range(1, m):
            h[i] = min(h[i - 1] + restrictions[i][0] - restrictions[i - 1][0], restrictions[i][1])
        for i in range(m - 2, -1, -1):
            h[i] = min(h[i], h[i + 1] + restrictions[i + 1][0] - restrictions[i][0])

        return max(restrictions[i + 1][0] - restrictions[i][0] + h[i] + h[i + 1] for i in range(m - 1)) // 2
```

```java [sol-Java]
class Solution {
    public int maxBuilding(int n, int[][] restrictions) {
        int m = restrictions.length;
        if (m == 0) {
            return n - 1;
        }

        Arrays.sort(restrictions, (a, b) -> a[0] - b[0]); // 按照 id 从小大到大排序

        // h[i] 表示编号为 id[i] 的建筑的最大高度
        int[] h = new int[m];
        h[0] = Math.min(restrictions[0][0] - 1, restrictions[0][1]);
        for (int i = 1; i < m; i++) {
            h[i] = Math.min(h[i - 1] + restrictions[i][0] - restrictions[i - 1][0], restrictions[i][1]);
        }
        for (int i = m - 2; i >= 0; i--) {
            h[i] = Math.min(h[i], h[i + 1] + restrictions[i + 1][0] - restrictions[i][0]);
        }

        int ans = Math.max((restrictions[0][0] - 1 + h[0]) / 2, h[m - 1] + n - restrictions[m - 1][0]);
        for (int i = 0; i < m - 1; i++) {
            ans = Math.max(ans, (restrictions[i + 1][0] - restrictions[i][0] + h[i] + h[i + 1]) / 2);
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxBuilding(int n, vector<vector<int>>& restrictions) {
        int m = restrictions.size();
        if (m == 0) {
            return n - 1;
        }

        ranges::sort(restrictions, {}, [](auto& a) { return a[0]; }); // 按照 id 从小大到大排序

        // h[i] 表示编号为 id[i] 的建筑的最大高度
        vector<int> h(m);
        h[0] = min(restrictions[0][0] - 1, restrictions[0][1]);
        for (int i = 1; i < m; i++) {
            h[i] = min(h[i - 1] + restrictions[i][0] - restrictions[i - 1][0], restrictions[i][1]);
        }
        for (int i = m - 2; i >= 0; i--) {
            h[i] = min(h[i], h[i + 1] + restrictions[i + 1][0] - restrictions[i][0]);
        }

        int ans = max((restrictions[0][0] - 1 + h[0]) / 2, h[m - 1] + n - restrictions[m - 1][0]);
        for (int i = 0; i < m - 1; i++) {
            ans = max(ans, (restrictions[i + 1][0] - restrictions[i][0] + h[i] + h[i + 1]) / 2);
        }
        return ans;
    }
};
```

```c [sol-C]
int cmp(const void* a, const void* b) {
    return (*(int**)a)[0] - (*(int**)b)[0];
}

int maxBuilding(int n, int** restrictions, int restrictionsSize, int* restrictionsColSize) {
    int m = restrictionsSize;
    if (m == 0) {
        return n - 1;
    }

    qsort(restrictions, m, sizeof(int*), cmp); // 按照 id 从小大到大排序

    // h[i] 表示编号为 id[i] 的建筑的最大高度
    int* h = malloc(m * sizeof(int));
    h[0] = MIN(restrictions[0][0] - 1, restrictions[0][1]);
    for (int i = 1; i < m; i++) {
        h[i] = MIN(h[i - 1] + restrictions[i][0] - restrictions[i - 1][0], restrictions[i][1]);
    }
    for (int i = m - 2; i >= 0; i--) {
        h[i] = MIN(h[i], h[i + 1] + restrictions[i + 1][0] - restrictions[i][0]);
    }

    int ans = MAX((restrictions[0][0] - 1 + h[0]) / 2, h[m - 1] + n - restrictions[m - 1][0]);
    for (int i = 0; i < m - 1; i++) {
        ans = MAX(ans, (restrictions[i + 1][0] - restrictions[i][0] + h[i] + h[i + 1]) / 2);
    }

    free(h);
    return ans;
}
```

```go [sol-Go]
func maxBuilding(n int, restrictions [][]int) int {
	m := len(restrictions)
	if m == 0 {
		return n - 1
	}

	// 按照 id 从小大到大排序
	slices.SortFunc(restrictions, func(a, b []int) int { return a[0] - b[0] })

	// h[i] 表示编号为 id[i] 的建筑的最大高度
	h := make([]int, m)
	h[0] = min(restrictions[0][0]-1, restrictions[0][1])
	for i := 1; i < m; i++ {
		h[i] = min(h[i-1]+restrictions[i][0]-restrictions[i-1][0], restrictions[i][1])
	}
	for i := m - 2; i >= 0; i-- {
		h[i] = min(h[i], h[i+1]+restrictions[i+1][0]-restrictions[i][0])
	}

	ans := max((restrictions[0][0]-1+h[0])/2, h[m-1]+n-restrictions[m-1][0])
	for i := range m - 1 {
		ans = max(ans, (restrictions[i+1][0]-restrictions[i][0]+h[i]+h[i+1])/2)
	}
	return ans
}
```

```js [sol-JavaScript]
var maxBuilding = function(n, restrictions) {
    const m = restrictions.length;
    if (m === 0) {
        return n - 1;
    }

    restrictions.sort((a, b) => a[0] - b[0]); // 按照 id 从小大到大排序

    // h[i] 表示编号为 id[i] 的建筑的最大高度
    const h = Array(m).fill(0);
    h[0] = Math.min(restrictions[0][0] - 1, restrictions[0][1]);
    for (let i = 1; i < m; i++) {
        h[i] = Math.min(h[i - 1] + restrictions[i][0] - restrictions[i - 1][0], restrictions[i][1]);
    }
    for (let i = m - 2; i >= 0; i--) {
        h[i] = Math.min(h[i], h[i + 1] + restrictions[i + 1][0] - restrictions[i][0]);
    }

    let ans = Math.max(Math.floor((restrictions[0][0] - 1 + h[0]) / 2), h[m - 1] + n - restrictions[m - 1][0]);
    for (let i = 0; i < m - 1; i++) {
        ans = Math.max(ans, Math.floor((restrictions[i + 1][0] - restrictions[i][0] + h[i] + h[i + 1]) / 2));
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn max_building(n: i32, mut restrictions: Vec<Vec<i32>>) -> i32 {
        let m = restrictions.len();
        if m == 0 {
            return n - 1;
        }

        restrictions.sort_unstable_by_key(|a| a[0]); // 按照 id 从小大到大排序

        // h[i] 表示编号为 id[i] 的建筑的最大高度
        let mut h = vec![0; m];
        h[0] = restrictions[0][1].min(restrictions[0][0] - 1);
        for i in 1..m {
            h[i] = restrictions[i][1].min(h[i - 1] + restrictions[i][0] - restrictions[i - 1][0]);
        }
        for i in (0..m - 1).rev() {
            h[i] = h[i].min(h[i + 1] + restrictions[i + 1][0] - restrictions[i][0]);
        }

        let mut ans = ((restrictions[0][0] - 1 + h[0]) / 2).max(h[m - 1] + n - restrictions[m - 1][0]);
        for i in 0..m - 1 {
            ans = ans.max((restrictions[i + 1][0] - restrictions[i][0] + h[i] + h[i + 1]) / 2);
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(m\log m)$，其中 $m$ 是 $\textit{restrictions}$ 的长度。瓶颈在排序上。
- 空间复杂度：$\mathcal{O}(m)$。

## 专题训练

见下面思维题单的「**§5.6 两次扫描**」。

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
11. [链表、树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `五、思维题 / §5.6 两次扫描`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `五、思维题 / §5.6 两次扫描`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
