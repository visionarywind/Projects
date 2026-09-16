# 878. 第 N 个神奇数字

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/nth-magical-number/
- 题目 slug：`nth-magical-number`
- 来源专题：数学算法
- 来源分类路径：二、组合数学 / §2.3 容斥原理
- 难度分：1897
- 外部题解来源：https://leetcode.cn/problems/nth-magical-number/solutions/1984641/er-fen-da-an-rong-chi-yuan-li-by-endless-9j34/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[二分答案+容斥原理（Python/Java/C++/Go）](https://leetcode.cn/problems/nth-magical-number/solutions/1984641/er-fen-da-an-rong-chi-yuan-li-by-endless-9j34/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`er-fen-da-an-rong-chi-yuan-li-by-endless-9j34`
- topic id：`1984641`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

![878-2.png](https://pic.leetcode.cn/1669032532-GjXsyF-878-2.png)

## 补充

下面代码采用开区间二分，这仅仅是二分的一种写法，使用闭区间或者半闭半开区间都是可以的。

- 开区间左端点初始值：$1$。一定无法满足要求。注意 $a$ 和 $b$ 都 $\ge 2$。
- 开区间左端点初始值（优化）：$\min(a,b)+n-2$。神奇数字最小是 $\min(a,b)$，第 $n$ 个神奇数字至少是 $\min(a,b)+n-1$，再减一就一定无法满足要求了。
- 开区间右端点初始值：$\min(a,b)\cdot n$。一定满足要求。

## 答疑

**问**：能否讲解一下二分的原理和代码中的细节？

**答**：见 [二分查找又死循环了？一个视频讲透二分本质！](https://www.bilibili.com/video/BV1AP41137w7/)

**问**：为什么二分循环结束时，得到的一定是一个神奇数字？

**答**：设答案为 $x$，循环结束时，$\le x$ 的神奇数字有 $n$ 个，而 $\le x-1$ 的神奇数字不足 $n$ 个（可结合视频中的红蓝染色来理解）。**只有当 $x$ 是一个神奇数字时，才会出现这种情况。**

这也同时说明，在二分循环中，我们**不能在计算结果恰好等于 $n$ 的时候，直接返回答案**，而是要继续二分。

**问**：最小公倍数是怎么算的？

**答**：利用最小公倍数与最大公约数的关系 $\text{lcm}(a,b) = \dfrac{a\cdot b}{\text{gcd}(a,b)}$。计算 $\text{gcd}(a,b)$ 可以用辗转相除法。

```py [sol-Python3]
class Solution:
    def nthMagicalNumber(self, n: int, a: int, b: int) -> int:
        lcm = math.lcm(a, b)
        left = min(a, b) + n - 2
        right = min(a, b) * n  # 开区间 (left, right)
        while left + 1 < right:  # 开区间不为空
            mid = (left + right) // 2
            if mid // a + mid // b - mid // lcm >= n:
                right = mid  # 范围缩小到 (left, mid)
            else:
                left = mid  # 范围缩小到 (mid, right)
        return right % 1_000_000_007
```

```py [sol-Python3 库函数]
class Solution:
    def nthMagicalNumber(self, n: int, a: int, b: int) -> int:
        lcm = math.lcm(a, b)
        left = min(a, b) + n - 1  # 左闭右开
        right = min(a, b) * n
        check = lambda x: x // a + x // b - x // lcm >= n
        return bisect_left(range(right), True, lo=left, key=check) % 1_000_000_007
```

```java [sol-Java]
class Solution {
    public int nthMagicalNumber(int n, int a, int b) {
        final int MOD = 1_000_000_007;
        long lcm = a / gcd(a, b) * b;
        long left = Math.min(a, b) + n - 2;
        long right = (long) Math.min(a, b) * n; // 开区间 (left, right)
        while (left + 1 < right) { // 开区间不为空
            long mid = (left + right) >>> 1;
            if (mid / a + mid / b - mid / lcm >= n) {
                right = mid; // 范围缩小到 (left, mid)
            } else {
                left = mid; // 范围缩小到 (mid, right)
            }
        }
        return (int) (right % MOD);
    }

    private int gcd(int a, int b) {
        while (a != 0) {
            int tmp = a;
            a = b % a;
            b = tmp;
        }
        return b;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int nthMagicalNumber(int n, int a, int b) {
        const int MOD = 1'000'000'007;
        long long lcm = std::lcm(a, b);
        long long left = min(a, b) + n - 2;
        long long right = 1LL * min(a, b) * n; // 开区间 (left, right)
        while (left + 1 < right) { // 开区间不为空
            long long mid = left + (right - left) / 2;
            (mid / a + mid / b - mid / lcm >= n ? right : left) = mid;
        }
        return right % MOD;
    }
};
```

```go [sol-Go]
func nthMagicalNumber(n, a, b int) int {
    lcm := a / gcd(a, b) * b
    left, right := min(a, b)+n-2, min(a, b)*n // 开区间 (left, right)
    for left+1 < right { // 开区间不为空
        mid := left + (right-left)/2
        if mid/a+mid/b-mid/lcm >= n {
            right = mid // 范围缩小到 (left, mid)
        } else {
            left = mid // 范围缩小到 (mid, right)
        }
    }
    return right % 1_000_000_007
}

func gcd(a, b int) int {
    for a != 0 {
        a, b = b%a, a
    }
    return b
}
```

```go [sol-Go 库函数]
func nthMagicalNumber(n, a, b int) int {
    lcm := a / gcd(a, b) * b
    left, right := min(a, b)+n-1, min(a, b)*n // 左闭右开区间 [left, right)
    ans := left + sort.Search(right-left, func(x int) bool {
        x += left
        return x/a+x/b-x/lcm >= n
    })
    return ans % 1_000_000_007
}

func gcd(a, b int) int {
    for a != 0 {
        a, b = b%a, a
    }
    return b
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(\log(\min(a,b)\cdot n)))$。
- 空间复杂度：$\mathcal{O}(1)$。

## 相似题目

- [1201. 丑数 III](https://leetcode.cn/problems/ugly-number-iii/)

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

本题来自 `二、组合数学 / §2.3 容斥原理`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、组合数学 / §2.3 容斥原理`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
