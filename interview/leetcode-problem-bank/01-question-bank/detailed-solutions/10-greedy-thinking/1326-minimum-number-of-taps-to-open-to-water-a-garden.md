# 1326. 灌溉花园的最少水龙头数目

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-number-of-taps-to-open-to-water-a-garden/
- 题目 slug：`minimum-number-of-taps-to-open-to-water-a-garden`
- 来源专题：贪心与思维
- 来源分类路径：二、区间贪心 / §2.4 区间覆盖
- 难度分：1885
- 外部题解来源：https://leetcode.cn/problems/minimum-number-of-taps-to-open-to-water-a-garden/solutions/2123855/yi-zhang-tu-miao-dong-pythonjavacgo-by-e-wqry/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[一张图秒懂！（Python/Java/C++/Go）](https://leetcode.cn/problems/minimum-number-of-taps-to-open-to-water-a-garden/solutions/2123855/yi-zhang-tu-miao-dong-pythonjavacgo-by-e-wqry/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`yi-zhang-tu-miao-dong-pythonjavacgo-by-e-wqry`
- topic id：`2123855`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:54:06 +0800

首先解释下示例 2 为什么要输出 $-1$：因为「整个花园」包含不是整点的位置，例如 $0.5$ 这种小数位置也要被灌溉到，但输入只能灌溉 $0,1,2,3$ 这 $4$ 个整点。

![1326.png](https://pic.leetcode.cn/1676903975-zvMFSg-1326.png)

## 答疑

**问**：我能想出这题的思路，就是代码实现总是写不对，有没有什么建议？

**答**：清晰的变量名以及一些必要的注释，会对理清代码逻辑有帮助。在出现错误时，可以用一些小数据去运行你的代码，通过 print 或者打断点的方式，查看这些关键变量的值，看看是否与预期结果一致。

**问**：为什么没有初始化 $\textit{rightMost}[i]=i$？

**答**：如果可以走到 $i$，那么 $\textit{nextRight}\ge i$，此时 $\textit{rightMost}[i]=i$ 还是 $=0$ 都不会影响 $\textit{nextRight}$ 的值，也就没必要初始化 $\textit{rightMost}[i]=i$ 了。

```py [sol-Python3]
class Solution:
    def minTaps(self, n: int, ranges: List[int]) -> int:
        right_most = [0] * (n + 1)
        for i, r in enumerate(ranges):
            left = max(i - r, 0)
            right_most[left] = max(right_most[left], i + r)

        ans = 0
        cur_right = 0  # 已建造的桥的右端点
        next_right = 0  # 下一座桥的右端点的最大值
        for i in range(n):  # 如果走到 n-1 时没有返回 -1，那么必然可以到达 n
            next_right = max(next_right, right_most[i])
            if i == cur_right:  # 到达已建造的桥的右端点
                if i == next_right:  # 无论怎么造桥，都无法从 i 到 i+1
                    return -1
                cur_right = next_right  # 造一座桥
                ans += 1
        return ans
```

```java [sol-Java]
class Solution {
    public int minTaps(int n, int[] ranges) {
        int[] rightMost = new int[n + 1];
        for (int i = 0; i <= n; i++) {
            int r = ranges[i];
            int left = Math.max(i - r, 0);
            rightMost[left] =  Math.max(rightMost[left], i + r);
        }

        int ans = 0;
        int curRight = 0; // 已建造的桥的右端点
        int nextRight = 0; // 下一座桥的右端点的最大值
        for (int i = 0; i < n; i++) { // 如果走到 n-1 时没有返回 -1，那么必然可以到达 n
            nextRight = Math.max(nextRight, rightMost[i]);
            if (i == curRight) { // 到达已建造的桥的右端点
                if (i == nextRight) {
                    return -1; // 无论怎么造桥，都无法从 i 到 i+1
                }
                curRight = nextRight; // 造一座桥
                ans++;
            }
        }
        return ans;
    }
}
```

```java [sol-Java 更快写法]
class Solution {
    public int minTaps(int n, int[] ranges) {
        int[] rightMost = new int[n + 1];
        for (int i = 0; i <= n; i++) {
            int r = ranges[i];
            // 这样写可以在 i > r 时少写一个 max
            if (i > r) {
                rightMost[i - r] = i + r; // 由于 i 在不断变大，对于 i-r 来说，i+r 必然是它目前的最大值
            } else {
                rightMost[0] = Math.max(rightMost[0], i + r);
            }
        }

        int ans = 0;
        int curRight = 0; // 已建造的桥的右端点
        int nextRight = 0; // 下一座桥的右端点的最大值
        for (int i = 0; i < n; i++) { // 如果走到 n-1 时没有返回 -1，那么必然可以到达 n
            nextRight = Math.max(nextRight, rightMost[i]);
            if (i == curRight) { // 到达已建造的桥的右端点
                if (i == nextRight) {
                    return -1; // 无论怎么造桥，都无法从 i 到 i+1
                }
                curRight = nextRight; // 造一座桥
                ans++;
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minTaps(int n, vector<int>& ranges) {
        vector<int> right_most(n + 1);
        for (int i = 0; i <= n; i++) {
            int r = ranges[i];
            int left = max(i - r, 0);
            right_most[left] = max(right_most[left], i + r);
        }

        int ans = 0;
        int cur_right = 0; // 已建造的桥的右端点
        int next_right = 0; // 下一座桥的右端点的最大值
        for (int i = 0; i < n; i++) { // 如果走到 n-1 时没有返回 -1，那么必然可以到达 n
            next_right = max(next_right, right_most[i]);
            if (i == cur_right) { // 到达已建造的桥的右端点
                if (i == next_right) {
                    return -1; // 无论怎么造桥，都无法从 i 到 i+1
                }
                cur_right = next_right; // 造一座桥
                ans++;
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func minTaps(n int, ranges []int) (ans int) {
    rightMost := make([]int, n+1)
    for i, r := range ranges {
        left := max(i-r, 0)
        rightMost[left] = max(rightMost[left], i+r)
    }

    curRight := 0 // 已建造的桥的右端点
    nextRight := 0 // 下一座桥的右端点的最大值
    for i, r := range rightMost[:n] { // 如果走到 n-1 时没有返回 -1，那么必然可以到达 n
        nextRight = max(nextRight, r)
        if i == curRight { // 到达已建造的桥的右端点
            if i == nextRight { // 无论怎么造桥，都无法从 i 到 i+1
                return -1
            }
            curRight = nextRight // 造一座桥
            ans++
        }
    }
    return
}
```

## 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$。
- 空间复杂度：$\mathcal{O}(n)$。

## 相似题目

- [55. 跳跃游戏](https://leetcode.cn/problems/jump-game/)
- [45. 跳跃游戏 II](https://leetcode.cn/problems/jump-game-ii/)
- [1024. 视频拼接](https://leetcode.cn/problems/video-stitching/)

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/circle/discuss/0viNMK/)
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

本题来自 `二、区间贪心 / §2.4 区间覆盖`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、区间贪心 / §2.4 区间覆盖`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
