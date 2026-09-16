# 1040. 移动石子直到连续 II

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/moving-stones-until-consecutive-ii/
- 题目 slug：`moving-stones-until-consecutive-ii`
- 来源专题：滑动窗口与双指针
- 来源分类路径：二、不定长滑动窗口 / §2.1 越短越合法/求最长/最大 / §2.1.2 进阶（选做）
- 难度分：2456
- 外部题解来源：https://leetcode.cn/problems/moving-stones-until-consecutive-ii/solutions/2212638/tu-jie-xia-tiao-qi-pythonjavacgo-by-endl-r1eb/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【图解】下跳棋（Python/Java/C++/Go）](https://leetcode.cn/problems/moving-stones-until-consecutive-ii/solutions/2212638/tu-jie-xia-tiao-qi-pythonjavacgo-by-endl-r1eb/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tu-jie-xia-tiao-qi-pythonjavacgo-by-endl-r1eb`
- topic id：`2212638`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

![1040-cut.png](https://pic.leetcode.cn/1680696212-AUVzBz-1040-cut.png)

### 答疑

**问**：如何使用滑动窗口？如何避免下标算错？

**答**：请看 [滑动窗口【基础算法精讲 03】](https://www.bilibili.com/video/BV1hd4y1r7Gq/)。

**问**：在窗口右边界 $s[\textit{right}]$ 比较小时（比如 $\textit{right}=0$），窗口内部左边都是空的，无法把石子移过去，为什么代码中没有判断这种不合法的情况呢？

**答**：无需考虑这种情况。如果窗口内部左边都是空的，那么继续向右滑动窗口，在窗口左边界有石子之前，窗口内的石子个数是不会减少的，后面算出来的 $\textit{maxCnt}$ 必然更大，所以无需考虑窗口内部左边都是空的情况。代码实现时，在 $\textit{right}$ 比较小时也做了计算，因为在取 $\max$ 的情况下，这对最终的 $\textit{maxCnt}$ 是没有影响的。

**问**：对于最小移动次数，除了图中的「特殊情况」外，是否还存在其它的特殊情况？你能构造出一个具体的移动方式吗？

**答**：没有其它的特殊情况了，因为一定可以通过如下方式完成移动：

- 首先，按照算法流程，窗口右边界 $s[\textit{right}]$ 一定可以位于一颗石子上。（因为在窗口滑到下一颗石子**之前**，窗口内的石子不会增加，所以只需要考虑窗口右边界在石子上的情况。）
- 情况一：窗口左边界也有石子。那么窗口内任意空位的左右两侧都有石子，窗口外的石子怎么移动都行。
  - 注意：窗口左边界是 $s[\textit{right}]-n+1$，它 $\le s[\textit{left}]$。
- 情况二：窗口左边界没有石子，且窗口右边界的右侧还有石子。那么把右端点石子移到窗口的左边界上，就能转换到情况一。
- 情况三：窗口左边界没有石子，且窗口右边界的右侧也没有石子（即窗口右边界的石子就是右端点石子）。由于不是特殊情况，窗口左侧必然有至少 $2$ 颗石子，那么左端点石子可以移到窗口的左边界上，这样就转换到了情况一。（具体见「最小移动次数：讨论」的第三幅图。）

**问**：为什么不在滑动窗口内判断特殊情况？

**答**：不需要，在滑动窗口之前判断就行。这是因为如果不是特殊情况，窗口内至少有 $2$ 个空位，所以在窗口滑动时，是无论如何都不会出现特殊情况的。

**问**：你是如何想到本题的做法的？是否有一些通用的思考方式？

**答**：个人觉得这题有点构造的味道（想算出答案，要大致知道怎么移动石子）。对于构造题，通常是先从最基本的情况开始思考，比如本题就是从 $n=3$ 开始思考。在纸上多画一画，比较不同的移动方案，猜想出一个大致的结论。接着思考 $n=4,5,\cdots$ 的情况，验证/修正你的结论。这就是「**从特殊到一般**」。如果你想做更多的构造题，可以去 Codeforces 搜索 tag：constructive algorithms。

```py [sol-Python3]
class Solution:
    def numMovesStonesII(self, s: List[int]) -> List[int]:
        s.sort()
        n = len(s)
        e1 = s[-2] - s[0] - n + 2
        e2 = s[-1] - s[1] - n + 2  # 计算空位
        max_move = max(e1, e2)
        if e1 == 0 or e2 == 0:  # 特殊情况：没有空位
            return [min(2, max_move), max_move]
        max_cnt = left = 0
        for right, sr in enumerate(s):  # 滑动窗口：枚举右端点所在石子
            while sr - s[left] + 1 > n:  # 窗口长度大于 n
                left += 1  # 缩小窗口长度
            max_cnt = max(max_cnt, right - left + 1)  # 维护窗口内的最大石子数
        return [n - max_cnt, max_move]
```

```java [sol-Java]
class Solution {
    public int[] numMovesStonesII(int[] s) {
        Arrays.sort(s);
        int n = s.length;
        int e1 = s[n - 2] - s[0] - n + 2;
        int e2 = s[n - 1] - s[1] - n + 2; // 计算空位
        int maxMove = Math.max(e1, e2);
        if (e1 == 0 || e2 == 0) // 特殊情况：没有空位
            return new int[]{Math.min(2, maxMove), maxMove};
        int maxCnt = 0, left = 0;
        for (int right = 0; right < n; ++right) { // 滑动窗口：枚举右端点所在石子
            while (s[right] - s[left] + 1 > n) // 窗口长度大于 n
                ++left; // 缩小窗口长度
            maxCnt = Math.max(maxCnt, right - left + 1); // 维护窗口内的最大石子数
        }
        return new int[]{n - maxCnt, maxMove};
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> numMovesStonesII(vector<int> &s) {
        ranges::sort(s);
        int n = s.size();
        int e1 = s[n - 2] - s[0] - n + 2;
        int e2 = s[n - 1] - s[1] - n + 2; // 计算空位
        int max_move = max(e1, e2);
        if (e1 == 0 || e2 == 0) // 特殊情况：没有空位
            return {min(2, max_move), max_move};
        int max_cnt = 0, left = 0;
        for (int right = 0; right < n; ++right) { // 滑动窗口：枚举右端点所在石子
            while (s[right] - s[left] + 1 > n) // 窗口长度大于 n
                ++left; // 缩小窗口长度
            max_cnt = max(max_cnt, right - left + 1); // 维护窗口内的最大石子数
        }
        return {n - max_cnt, max_move};
    }
};
```

```go [sol-Go]
func numMovesStonesII(s []int) []int {
    slices.Sort(s)
    n := len(s)
    e1 := s[n-2] - s[0] - n + 2
    e2 := s[n-1] - s[1] - n + 2 // 计算空位
    maxMove := max(e1, e2)
    if e1 == 0 || e2 == 0 { // 特殊情况：没有空位
        return []int{min(2, maxMove), maxMove}
    }
    maxCnt, left := 0, 0
    for right, sr := range s { // 滑动窗口：枚举右端点所在石子
        for sr-s[left]+1 > n { // 窗口长度大于 n
            left++ // 缩小窗口长度
        }
        maxCnt = max(maxCnt, right-left+1) // 维护窗口内的最大石子数
    }
    return []int{n - maxCnt, maxMove}
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n)$，其中 $n$ 为 $\textit{stones}$ 的长度。瓶颈在排序上。
- 空间复杂度：$\mathcal{O}(1)$。忽略排序时的栈开销，仅用到若干额外变量。

## 分类题单

1. [滑动窗口（定长/不定长/多指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（矩形系列/字典序最小/贡献法）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/贪心/脑筋急转弯）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)

更多题单，点我个人主页 - 讨论发布。

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

[往期题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

## 本地原创解析

### 1. 题意重述

本题来自 `二、不定长滑动窗口 / §2.1 越短越合法/求最长/最大 / §2.1.2 进阶（选做）`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、不定长滑动窗口 / §2.1 越短越合法/求最长/最大 / §2.1.2 进阶（选做）`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
