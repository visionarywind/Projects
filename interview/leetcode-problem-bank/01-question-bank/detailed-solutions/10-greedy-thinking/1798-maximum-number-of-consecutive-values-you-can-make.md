# 1798. 你能构造出连续值的最大数目

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-number-of-consecutive-values-you-can-make/
- 题目 slug：`maximum-number-of-consecutive-values-you-can-make`
- 来源专题：贪心与思维
- 来源分类路径：四、数学贪心 / §4.6 归纳法
- 难度分：1931
- 外部题解来源：https://leetcode.cn/problems/maximum-number-of-consecutive-values-you-can-make/solutions/2091580/mei-xiang-ming-bai-yi-zhang-tu-miao-dong-7xlx/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[没想明白？一张图秒懂！（Python/Java/C++/Go）](https://leetcode.cn/problems/maximum-number-of-consecutive-values-you-can-make/solutions/2091580/mei-xiang-ming-bai-yi-zhang-tu-miao-dong-7xlx/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`mei-xiang-ming-bai-yi-zhang-tu-miao-dong-7xlx`
- topic id：`2091580`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:07:12 +0800

### 提示

**有序**是一个强大的性质，如果对数组排序不影响答案的话，可以尝试将数组排序后，再重新思考，看看能否发现新的思路。

![1798.png](https://pic.leetcode.cn/1675089725-wbrBLD-1798.png)

```py [sol1-Python3]
class Solution:
    def getMaximumConsecutive(self, coins: List[int]) -> int:
        m = 0  # 一开始只能构造出 0
        coins.sort()
        for c in coins:
            if c > m + 1:  # coins 已排序，后面没有比 c 更小的数了
                break  # 无法构造出 m+1，继续循环没有意义
            m += c  # 可以构造出区间 [0,m+c] 中的所有整数
        return m + 1  # [0,m] 中一共有 m+1 个整数
```

```java [sol1-Java]
class Solution {
    public int getMaximumConsecutive(int[] coins) {
        int m = 0; // 一开始只能构造出 0
        Arrays.sort(coins);
        for (int c : coins) {
            if (c > m + 1) // coins 已排序，后面没有比 c 更小的数了
                break; // 无法构造出 m+1，继续循环没有意义
            m += c; // 可以构造出区间 [0,m+c] 中的所有整数
        }
        return m + 1; // [0,m] 中一共有 m+1 个整数
    }
}
```

```cpp [sol1-C++]
class Solution {
public:
    int getMaximumConsecutive(vector<int> &coins) {
        int m = 0; // 一开始只能构造出 0
        sort(coins.begin(), coins.end());
        for (int c : coins) {
            if (c > m + 1) // coins 已排序，后面没有比 c 更小的数了
                break; // 无法构造出 m+1，继续循环没有意义
            m += c; // 可以构造出区间 [0,m+c] 中的所有整数
        }
        return m + 1; // [0,m] 中一共有 m+1 个整数
    }
};
```

```go [sol1-Go]
func getMaximumConsecutive(coins []int) int {
    m := 0 // 一开始只能构造出 0
    sort.Ints(coins)
    for _, c := range coins {
        if c > m+1 { // coins 已排序，后面没有比 c 更小的数了
            break // 无法构造出 m+1，继续循环没有意义
        }
        m += c // 可以构造出区间 [0,m+c] 中的所有整数
    }
    return m + 1 // [0,m] 中一共有 m+1 个整数
}
```

### 复杂度分析

- 时间复杂度：$O(n\log n)$，其中 $n$ 为 $\textit{coins}$ 的长度。瓶颈在排序上。
- 空间复杂度：$O(1)$。忽略排序时的栈空间，仅用到若干额外变量。

---

如果你觉得自己的思维能力有些薄弱，可以做做 [从周赛中学算法 - 2022 年周赛题目总结（下篇）](https://leetcode.cn/circle/discuss/WR1MJP/) 中的「思维题」这节，所有题目我都写了题解。

最后，欢迎关注【bilibili@灵茶山艾府】，每周更新算法教学视频~

## 本地原创解析

### 1. 题意重述

本题来自 `四、数学贪心 / §4.6 归纳法`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `四、数学贪心 / §4.6 归纳法`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
