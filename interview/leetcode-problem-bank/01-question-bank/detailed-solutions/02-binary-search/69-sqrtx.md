# 69. x 的平方根

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/sqrtx/
- 题目 slug：`sqrtx`
- 来源专题：二分算法
- 来源分类路径：四、其他
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/sqrtx/solutions/2942682/kai-qu-jian-er-fen-jian-ji-xie-fa-python-v4fb/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[开区间二分，不用 long 类型（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/sqrtx/solutions/2942682/kai-qu-jian-er-fen-jian-ji-xie-fa-python-v4fb/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`kai-qu-jian-er-fen-jian-ji-xie-fa-python-v4fb`
- topic id：`2942682`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 题意

输入非负整数 $x$，返回最大的非负整数 $m$，满足 $m^2\le x$。

## 思路

对于非负整数 $m$ 来说，由于 $m$ 越小越满足 $m^2\le x$，$m$ 越大越不满足 $m^2\le x$，有单调性，可以二分答案。

二分 $m$：

- 如果 $m^2\le x$ 成立，那么最终答案大于等于 $m$，更新 $\textit{left}$。
- 如果 $m^2 > x$ 成立，那么最终答案小于 $m$，更新 $\textit{right}$。

关于二分的原理，请看视频[【基础算法精讲 04】](https://www.bilibili.com/video/BV1AP41137w7/)。

最后，讨论二分的上下界。**本文用开区间二分，其他二分写法也是可以的。**

开区间二分左边界：$0$。题目保证 $x\ge 0$，所以 $0^2\le x$ 一定成立。

开区间二分右边界：$x+1$。题目保证 $x\ge 0$，所以 $(x+1)^2 > x$ 一定成立。

开区间二分右边界（优化）：$\min(x, 46340) + 1$，其中 $46340 = \left\lfloor\sqrt {2^{31}-1}\right\rfloor$。因为题目保证 $x\le 2^{31}-1 = 2147483647$，所以 $46341^2 = 2147488281 > x$ 一定成立。

## 答疑

**问**：为什么代码用 $\texttt{int}$ 类型就够了？为什么不会溢出？

**答**：在循环中，$m$ 的值**始终在二分区间范围内**，也就是说 $0 < m < 46341$，所以 $m^2 \le 46340^2 = 2147395600$，在 $32$ 位有符号整数的范围内，所以不会溢出。

**问**：为什么代码返回的是 $\textit{left}$？

**答**：在练习二分时，请注意「求最小」和「求最大」的二分写法上的区别。

「求最小」的题目和二分查找求「排序数组中某元素的第一个位置」是类似的，按照 [红蓝染色法](https://www.bilibili.com/video/BV1AP41137w7/)，左边是不满足要求的（红色），右边则是满足要求的（蓝色）。

「求最大」的题目（例如本题）则相反，左边是满足要求的（蓝色），右边是不满足要求的（红色）。这会导致二分写法和「求最小」有一些区别。

以开区间二分为例：

- 求最小：`check(mid) == true` 时更新 `right = mid`，反之更新 `left = mid`，最后返回 `right`。
- 求最大：`check(mid) == true` 时更新 `left = mid`，反之更新 `right = mid`，最后返回 `left`。

对于开区间写法，简单来说 `check(mid) == true` 时更新的是谁，最后就返回谁。相比其他二分写法，开区间写法不需要思考加一减一等细节，**推荐使用开区间写二分**。

```py [sol-Python3]
SQRT_MAX = 46340  # isqrt(2 ** 31 - 1)

class Solution:
    def mySqrt(self, x: int) -> int:
        # 开区间 (left, right)
        left, right = 0, min(x, SQRT_MAX) + 1
        while left + 1 < right:  # 开区间不为空
            # 循环不变量：left^2 <= x
            # 循环不变量：right^2 > x
            m = (left + right) // 2
            if m * m <= x:
                left = m
            else:
                right = m
        # 循环结束时 left+1 == right
        # 此时 left^2 <= x 且 right^2 > x
        # 所以 left 最大的满足 m^2 <= x 的数
        return left
```

```java [sol-Java]
class Solution {
    private static final int SQRT_INT_MAX = 46340; // sqrt(Integer.MAX_VALUE)

    public int mySqrt(int x) {
        // 开区间 (left, right)
        int left = 0;
        int right = Math.min(x, SQRT_INT_MAX) + 1;
        while (left + 1 < right) { // 开区间不为空
            // 循环不变量：left^2 <= x
            // 循环不变量：right^2 > x
            int m = (left + right) >>> 1;
            if (m * m <= x) {
                left = m;
            } else {
                right = m;
            }
        }
        // 循环结束时 left+1 == right
        // 此时 left^2 <= x 且 right^2 > x
        // 所以 left 最大的满足 m^2 <= x 的数
        return left;
    }
}
```

```cpp [sol-C++]
class Solution {
    const int SQRT_INT_MAX = 46340; // sqrt(INT_MAX);
public:
    int mySqrt(int x) {
        // 开区间 (left, right)
        int left = 0, right = min(x, SQRT_INT_MAX) + 1;
        while (left + 1 < right) { // 开区间不为空
            // 循环不变量：left^2 <= x
            // 循环不变量：right^2 > x
            int m = (left + right) / 2;
            (m * m <= x ? left : right) = m;
        }
        // 循环结束时 left+1 == right
        // 此时 left^2 <= x 且 right^2 > x
        // 所以 left 最大的满足 m^2 <= x 的数
        return left;
    }
};
```

```c [sol-C]
// (int) sqrt(INT_MAX)
#define SQRT_INT_MAX 46340

int mySqrt(int x) {
    // 开区间 (left, right)
    int left = 0, right = MIN(x, SQRT_INT_MAX) + 1;
    while (left + 1 < right) { // 开区间不为空
        // 循环不变量：left^2 <= x
        // 循环不变量：right^2 > x
        int m = (left + right) / 2;
        if (m * m <= x) {
            left = m;
        } else {
            right = m;
        }
    }
    // 循环结束时 left+1 == right
    // 此时 left^2 <= x 且 right^2 > x
    // 所以 left 最大的满足 m^2 <= x 的数
    return left;
}
```

```go [sol-Go]
const sqrtMaxInt32 = 46340 // Sqrt(math.MaxInt32)

func mySqrt(x int) int {
    // 开区间 (left, right)
    left, right := 0, min(x, sqrtMaxInt32)+1
    for left+1 < right { // 开区间不为空
        // 循环不变量：left^2 <= x
        // 循环不变量：right^2 > x
        m := (left + right) / 2
        if m*m <= x {
            left = m
        } else {
            right = m
        }
    }
    // 循环结束时 left+1 == right
    // 此时 left^2 <= x 且 right^2 > x
    // 所以 left 最大的满足 m^2 <= x 的数
    return left
}
```

```js [sol-JavaScript]
const SQRT_MAX = 46340; // floor(sqrt((1 << 31) - 1));

var mySqrt = function(x) {
    // 开区间 (left, right)
    let left = 0, right = Math.min(x, SQRT_MAX) + 1;
    while (left + 1 < right) { // 开区间不为空
        // 循环不变量：left^2 <= x
        // 循环不变量：right^2 > x
        let m = Math.floor((left + right) / 2);
        if (m * m <= x) {
            left = m;
        } else {
            right = m;
        }
    }
    // 循环结束时 left+1 == right
    // 此时 left^2 <= x 且 right^2 > x
    // 所以 left 最大的满足 m^2 <= x 的数
    return left;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn my_sqrt(x: i32) -> i32 {
        const sqrt_i32_max: i32 = 46340; // i32::MAX 开根号

        // 开区间 (left, right)
        let mut left = 0;
        let mut right = x.min(sqrt_i32_max) + 1;
        while left + 1 < right { // 开区间不为空
            // 循环不变量：left^2 <= x
            // 循环不变量：right^2 > x
            let m = (left + right) / 2;
            if m * m <= x {
                left = m;
            } else {
                right = m;
            }
        }
        // 循环结束时 left+1 == right
        // 此时 left^2 <= x 且 right^2 > x
        // 所以 left 最大的满足 m^2 <= x 的数
        left
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(\log \min(x, U))$，其中 $U=46340$ 为二分区间的最大长度。
- 空间复杂度：$\mathcal{O}(1)$。

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

本题来自 `四、其他`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `四、其他`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
