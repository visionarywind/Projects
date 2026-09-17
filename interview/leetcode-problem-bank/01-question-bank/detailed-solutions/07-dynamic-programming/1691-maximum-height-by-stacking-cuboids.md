# 1691. 堆叠长方体的最大高度

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-height-by-stacking-cuboids/
- 题目 slug：`maximum-height-by-stacking-cuboids`
- 来源专题：动态规划
- 来源分类路径：四、经典线性 DP / §4.2 最长递增子序列（LIS） / §4.2.2 进阶
- 难度分：2172
- 外部题解来源：https://leetcode.cn/problems/maximum-height-by-stacking-cuboids/solutions/2014514/tu-jie-suan-fa-you-hua-xiang-xi-zheng-mi-b6fq/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【图解】算法优化+详细证明（Python/Java/C++/Go）](https://leetcode.cn/problems/maximum-height-by-stacking-cuboids/solutions/2014514/tu-jie-suan-fa-you-hua-xiang-xi-zheng-mi-b6fq/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tu-jie-suan-fa-you-hua-xiang-xi-zheng-mi-b6fq`
- topic id：`2014514`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 16:46:02 +0800

![1691-2-4-cut.png](https://pic.leetcode.cn/1670597058-EzWyyA-1691-2-4-cut.png)

#### 答疑

**问**：为什么要对 $\textit{cuboids}$ 排序？

**答**：排序后，答案对应的长方体子序列可直接按照从左到右的顺序堆叠，从而保证了状态转移的正确性，即计算 $f[i]$ 时，可按照 $i$ 从小到大的顺序计算，且只用考虑 $j<i$ 的状态转移来源 $f[j]$。如果不排序，则无法保证这些。

**问**：为什么当 $A$ 可以堆在 $B$ 上时，可以直接把最长的边作为高？

**答**：我可以换一个角度来解释。把长方体看成盒子，把 $A$ **装进** $B$ 中（三条边短对短，中对中，长对长）。由于无论 $B$ 怎么旋转，$A$ 始终在 $B$ 中，那么直接把 $B$ 按照最长的边竖着放置，$A$ 也会按照最长的边竖着放置。

**问**：是否有时间复杂度比 $O(n^2)$ 更低的做法？

**答**：可以用动态开点二维线段树做到 $O(n\log^2 n)$。

```py [sol1-Python3]
class Solution:
    def maxHeight(self, cuboids: List[List[int]]) -> int:
        for c in cuboids:
            c.sort()
        cuboids.sort()
        f = [0] * len(cuboids)
        for i, (_, l2, h2) in enumerate(cuboids):
            for j, (_, l1, h1) in enumerate(cuboids[:i]):
                if l1 <= l2 and h1 <= h2:  # 排序后，w1 <= w2 恒成立
                    f[i] = max(f[i], f[j])  # cuboids[j] 可以堆在 cuboids[i] 上
            f[i] += h2
        return max(f)
```

```java [sol1-Java]
class Solution {
    public int maxHeight(int[][] cuboids) {
        for (int[] c : cuboids)
            Arrays.sort(c);
        Arrays.sort(cuboids, (a, b) -> a[0] != b[0] ? a[0] - b[0] : a[1] != b[1] ? a[1] - b[1] : a[2] - b[2]);
        int ans = 0, n = cuboids.length;
        int[] f = new int[n];
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < i; ++j)
                // 排序后，cuboids[j][0] <= cuboids[i][0] 恒成立
                if (cuboids[j][1] <= cuboids[i][1] && cuboids[j][2] <= cuboids[i][2])
                    f[i] = Math.max(f[i], f[j]); // cuboids[j] 可以堆在 cuboids[i] 上
            f[i] += cuboids[i][2];
            ans = Math.max(ans, f[i]);
        }
        return ans;
    }
}
```

```cpp [sol1-C++]
class Solution {
public:
    int maxHeight(vector<vector<int>> &cuboids) {
        for (auto &c : cuboids)
            sort(c.begin(), c.end());
        sort(cuboids.begin(), cuboids.end());
        int n = cuboids.size(), f[n];
        for (int i = 0; i < n; ++i) {
            f[i] = 0;
            for (int j = 0; j < i; ++j)
                // 排序后，cuboids[j][0] <= cuboids[i][0] 恒成立
                if (cuboids[j][1] <= cuboids[i][1] && cuboids[j][2] <= cuboids[i][2])
                    f[i] = max(f[i], f[j]); // cuboids[j] 可以堆在 cuboids[i] 上
            f[i] += cuboids[i][2];
        }
        return *max_element(f, f + n);
    }
};
```

```go [sol1-Go]
func maxHeight(cuboids [][]int) (ans int) {
    for _, c := range cuboids {
        sort.Ints(c)
    }
    sort.Slice(cuboids, func(i, j int) bool {
        a, b := cuboids[i], cuboids[j]
        return a[0] < b[0] || a[0] == b[0] && (a[1] < b[1] || a[1] == b[1] && a[2] < b[2])
    })
    f := make([]int, len(cuboids))
    for i, c2 := range cuboids {
        for j, c1 := range cuboids[:i] {
            if c1[1] <= c2[1] && c1[2] <= c2[2] { // 排序后，c1[0] <= c2[0] 恒成立
                f[i] = max(f[i], f[j]) // c1 可以堆在 c2 上
            }
        }
        f[i] += c2[2]
        ans = max(ans, f[i])
    }
    return
}

func max(a, b int) int { if b > a { return b }; return a }
```

#### 复杂度分析

- 时间复杂度：$O(n^2)$，其中 $n$ 为 $\textit{cuboids}$ 的长度。
- 空间复杂度：$O(n)$。

#### 相似题目

- [300. 最长递增子序列](https://leetcode.cn/problems/longest-increasing-subsequence/)
- [354. 俄罗斯套娃信封问题](https://leetcode.cn/problems/russian-doll-envelopes/)

如果想加深对 LIS 的理解，还可以做做 [2407. 最长递增子序列 II](https://leetcode.cn/problems/longest-increasing-subsequence-ii/)。

---

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)，最近正在连载 [【基础算法精讲】](https://www.bilibili.com/video/BV1AP41137w7/)，欢迎点赞~

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
