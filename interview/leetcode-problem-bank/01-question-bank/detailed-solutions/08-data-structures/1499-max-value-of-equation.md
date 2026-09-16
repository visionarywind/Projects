# 1499. 满足不等式的最大值

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/max-value-of-equation/
- 题目 slug：`max-value-of-equation`
- 来源专题：常用数据结构
- 来源分类路径：四、队列 / §4.4 单调队列
- 难度分：2456
- 外部题解来源：https://leetcode.cn/problems/max-value-of-equation/solutions/2352457/on-dan-diao-dui-lie-fu-ti-dan-pythonjava-hhrr/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[O(n) 单调队列，附题单！（Python/Java/C++/Go/JS）](https://leetcode.cn/problems/max-value-of-equation/solutions/2352457/on-dan-diao-dui-lie-fu-ti-dan-pythonjava-hhrr/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`on-dan-diao-dui-lie-fu-ti-dan-pythonjava-hhrr`
- topic id：`2352457`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 前置知识

不了解单调队列的同学，请看：

[两张图秒懂单调队列](https://leetcode.cn/problems/shortest-subarray-with-sum-at-least-k/solution/liang-zhang-tu-miao-dong-dan-diao-dui-li-9fvh/)

## 思路

变形：

$$
\begin{aligned}
&y_i + y_j + |x_i - x_j|\\
=\ &y_i + y_j + x_j - x_i\\
=\ &(x_j + y_j) + (y_i - x_i)
\end{aligned}
$$

枚举 $j$，问题变成计算 $y_i - x_i$ 的最大值，其中 $i<j$ 且 $x_i\ge x_j-k$。

用单调队列优化：

- 单调队列存储二元组 $(x_i,y_i-x_i)$。
- 首先把队首的超出范围的数据出队，即 $x_i<x_j-k$ 的数据。
- 然后把 $(x_j,y_j - x_j)$ 入队，入队前如果发现 $y_j - x_j$ 不低于队尾的数据，那么直接弹出队尾。
- 这样维护后，单调队列的 $y_i-x_i$ 从队首到队尾是严格递减的，$y_i - x_i$ 的最大值即为队首的最大值。

形象一点的说法是，老员工的能力必须比新来的强。~~否则就淘汰~~

```py [sol-Python3]
class Solution:
    def findMaxValueOfEquation(self, points: List[List[int]], k: int) -> int:
        ans = -inf
        q = deque()
        for x, y in points:
            while q and q[0][0] < x - k:  # 队首超出范围
                q.popleft()  # 弹它！
            if q:
                ans = max(ans, x + y + q[0][1])  # 加上最大的 yi-xi
            while q and q[-1][1] <= y - x:  # 队尾不如新来的强
                q.pop()  # 弹它！
            q.append((x, y - x))
        return ans
```

```java [sol-Java]
class Solution {
    public int findMaxValueOfEquation(int[][] points, int k) {
        int ans = Integer.MIN_VALUE;
        var q = new ArrayDeque<int[]>();
        for (var p : points) {
            int x = p[0], y = p[1];
            while (!q.isEmpty() && q.peekFirst()[0] < x - k) // 队首超出范围
                q.pollFirst(); // 弹它！
            if (!q.isEmpty())
                ans = Math.max(ans, x + y + q.peekFirst()[1]); // 加上最大的 yi-xi
            while (!q.isEmpty() && q.peekLast()[1] <= y - x) // 队尾不如新来的强
                q.pollLast(); // 弹它！
            q.addLast(new int[]{x, y - x});
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int findMaxValueOfEquation(vector<vector<int>> &points, int k) {
        int ans = INT_MIN;
        deque<pair<int, int>> q;
        for (auto &p: points) {
            int x = p[0], y = p[1];
            while (!q.empty() && q.front().first < x - k) // 队首超出范围
                q.pop_front(); // 弹它！
            if (!q.empty())
                ans = max(ans, x + y + q.front().second); // 加上最大的 yi-xi
            while (!q.empty() && q.back().second <= y - x) // 队尾不如新来的强
                q.pop_back(); // 弹它！
            q.emplace_back(x, y - x);
        }
        return ans;
    }
};
```

```go [sol-Go]
func findMaxValueOfEquation(points [][]int, k int) int {
    ans := math.MinInt
    type pair struct{ x, yx int }
    q := []pair{}
    for _, p := range points {
        x, y := p[0], p[1]
        for len(q) > 0 && q[0].x < x-k { // 队首超出范围
            q = q[1:] // 弹它！
        }
        if len(q) > 0 {
            ans = max(ans, x+y+q[0].yx) // 加上最大的 yi-xi
        }
        for len(q) > 0 && q[len(q)-1].yx <= y-x { // 队尾不如新来的强
            q = q[:len(q)-1] // 弹它！
        }
        q = append(q, pair{x, y - x})
    }
    return ans
}

func max(a, b int) int { if b > a { return b }; return a }
```

```js [sol-JavaScript]
var findMaxValueOfEquation = function (points, k) {
    let ans = Number.MIN_SAFE_INTEGER;
    let q = Array(points.length); // 用数组模拟双端队列
    let left = 0, right = 0; // 实际元素下标在左闭右开区间 [left,right) 内
    for (const [x, y] of points) {
        while (left < right && q[left][0] < x - k) // 队首超出范围
            left++; // 弹它！
        if (left < right)
            ans = Math.max(ans, x + y + q[left][1]); // 加上最大的 yi-xi
        while (left < right && q[right - 1][1] <= y - x) // 队尾不如新来的强
            right--; // 弹它！
        q[right++] = [x, y - x];
    }
    return ans;
};
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{points}$ 的长度。每个点至多入队出队各一次，所以时间复杂度是 $\mathcal{O}(n)$。
- 空间复杂度：$\mathcal{O}(n)$。

## 相似题目

- [面试题 59-II. 队列的最大值](https://leetcode.cn/problems/dui-lie-de-zui-da-zhi-lcof/)（单调队列模板题）
- [239. 滑动窗口最大值](https://leetcode.cn/problems/sliding-window-maximum/)
- [862. 和至少为 K 的最短子数组](https://leetcode.cn/problems/shortest-subarray-with-sum-at-least-k/)
- [1438. 绝对差不超过限制的最长连续子数组](https://leetcode.cn/problems/longest-continuous-subarray-with-absolute-diff-less-than-or-equal-to-limit/)

[往期每日一题题解（按 tag 分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `四、队列 / §4.4 单调队列`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `四、队列 / §4.4 单调队列`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
