# 2517. 礼盒的最大甜蜜度

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-tastiness-of-candy-basket/
- 题目 slug：`maximum-tastiness-of-candy-basket`
- 来源专题：二分算法
- 来源分类路径：二、二分答案 / §2.5 最大化最小值
- 难度分：2021
- 外部题解来源：https://leetcode.cn/problems/maximum-tastiness-of-candy-basket/solutions/2031994/er-fen-da-an-by-endlesscheng-r418/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[二分答案（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/maximum-tastiness-of-candy-basket/solutions/2031994/er-fen-da-an-by-endlesscheng-r418/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`er-fen-da-an-by-endlesscheng-r418`
- topic id：`2031994`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 16:46:02 +0800

「任意两种糖果价格绝对差的**最小值**」等价于「排序后，任意两种**相邻**糖果价格绝对差的最小值」。

如果题目要求「最大化最小值」或者「最小化最大值」，一般是二分答案。为什么？对于本题来说，甜蜜度越大，能选择的糖果越少，有**单调性**，所以可以二分答案。关于二分的原理，请看[【基础算法精讲 04】](https://www.bilibili.com/video/BV1AP41137w7/)。

定义 $f(d)$ 表示甜蜜度**至少**为 $d$ 时，**最多**能选多少类糖果。（注意是至少，不是恰好）

二分答案 $d$：

- 如果 $f(d)\ge k$，说明答案至少为 $d$。
- 如果 $f(d)< k$，说明答案至多为 $d-1$。
- 二分结束后，设答案为 $\textit{d}_0$，那么 $f(d_0)\ge k$ 且 $f(d_0+1)< k$。

如何计算 $f(d)$？对 $\textit{price}$ 从小到大排序，贪心地计算 $f(d)$：

- 第一个数 $\textit{price}[0]$ 一定可以选。如果有其他不选 $\textit{price}[0]$ 的方案，那么把该方案中的第一个数改成 $\textit{price}[0]$，也满足要求。或者说，选 $\textit{price}[0]$，后面可以选的数比不选 $\textit{price}[0]$ 更多。
- 假设上一个选的数是 $\textit{pre}$，那么当 $\textit{price}[i] \ge \textit{pre}+d$ 时，才可以选 $\textit{price}[i]$。

## 细节

下面代码采用开区间二分，这仅仅是二分的一种写法，使用闭区间或者半闭半开区间都是可以的。

- 开区间左端点初始值：$0$。此时我们算的是 $f(0)$，表示甜蜜度**至少**为 $0$ 时，最多能选多少类糖果。由于绝对值一定 $\ge 0$，所以所有糖果都可以选，一定可以满足要求。注意题目保证 $k\le n$。
- 开区间右端点初始值：$\left\lfloor\dfrac{\textit{price}[n-1]-\textit{price}[0]}{k-1}\right\rfloor+1$。假设可以每隔 $d$ 就选一类糖果，那么第一个糖果和第 $k$ 个糖果的间隔至少为 $(k-1)\cdot d$，必须满足 $\textit{price}[0] + (k-1)\cdot d \le \textit{price}[n-1]$ 才可能选 $k$ 个糖果，解得 $d\le \left\lfloor\dfrac{\textit{price}[n-1]-\textit{price}[0]}{k-1}\right\rfloor$。加一就一定无法满足要求了，也就是当 $d=\left\lfloor\dfrac{\textit{price}[n-1]-\textit{price}[0]}{k-1}\right\rfloor+1$ 时，一定无法选 $k$ 类糖果。

## 答疑

**问**：为什么二分出来的答案，一定来自数组中价格的差？有没有可能，二分出来的答案不是任何价格的差？

**答**：反证法。如果答案 $d$ 不是任何价格的差，也就是说，礼盒中任意两种糖果的价格的绝对差都大于 $d$，也就是大于等于 $d+1$。那么对于 $d+1$ 来说，它也满足 $f(d + 1) \ge k$，这与循环不变量相矛盾。所以原命题成立。

```py [sol-Python3]
class Solution:
    def maximumTastiness(self, price: List[int], k: int) -> int:
        def f(d: int) -> int:
            cnt = 1
            pre = price[0]  # 先选一个价格最小的糖果
            for p in price:
                if p - pre >= d:  # 可以选 p
                    cnt += 1
                    pre = p
            return cnt

        price.sort()
        left = 0
        right = (price[-1] - price[0]) // (k - 1) + 1
        while left + 1 < right:  # 开区间不为空
            # 循环不变量：
            # f(left) >= k
            # f(right) < k
            mid = (left + right) // 2
            if f(mid) >= k:
                left = mid  # 下一轮二分 (mid, right)
            else:
                right = mid  # 下一轮二分 (left, mid)
        return left  # 最大的满足 f(left) >= k 的数
```

```py [sol-Python3 库函数]
class Solution:
    def maximumTastiness(self, price: List[int], k: int) -> int:
        def check(d: int) -> bool:
            # 二分最小的 f(d+1) < k，从而知道最大的 f(d) >= k
            d += 1
            cnt = 1
            pre = price[0]  # 先选一个价格最小的糖果
            for p in price:
                if p - pre >= d:  # 可以选 p
                    cnt += 1
                    pre = p
            return cnt < k

        price.sort()
        right = (price[-1] - price[0]) // (k - 1)
        return bisect_left(range(right), True, key=check)
```

```java [sol-Java]
class Solution {
    public int maximumTastiness(int[] price, int k) {
        Arrays.sort(price);
        int left = 0;
        int right = (price[price.length - 1] - price[0]) / (k - 1) + 1;
        while (left + 1 < right) { // 开区间不为空
            // 循环不变量：
            // f(left) >= k
            // f(right) < k
            int mid = left + (right - left) / 2;
            if (f(price, mid) >= k) {
                left = mid; // 下一轮二分 (mid, right)
            } else {
                right = mid; // 下一轮二分 (left, mid)
            }
        }
        return left; // 最大的满足 f(left) >= k 的数
    }

    private int f(int[] price, int d) {
        int cnt = 1;
        int pre = price[0]; // 先选一个价格最小的糖果
        for (int p : price) {
            if (p - pre >= d) { // 可以选 p
                cnt++;
                pre = p;
            }
        }
        return cnt;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maximumTastiness(vector<int>& price, int k) {
        auto f = [&](int d) -> int {
            int cnt = 1, pre = price[0]; // 先选一个价格最小的糖果
            for (int p : price) {
                if (p - pre >= d) { // 可以选 p
                    cnt++;
                    pre = p;
                }
            }
            return cnt;
        };

        ranges::sort(price);
        int left = 0;
        int right = (price.back() - price[0]) / (k - 1) + 1;
        while (left + 1 < right) { // 开区间不为空
            // 循环不变量：
            // f(left) >= k
            // f(right) < k
            int mid = left + (right - left) / 2;
            (f(mid) >= k ? left : right) = mid;
        }
        return left; // 最大的满足 f(left) >= k 的数
    }
};
```

```c [sol-C]
int cmp(const void* a, const void* b) {
    return *(int*)a - *(int*)b;
}

int maximumTastiness(int* price, int priceSize, int k) {
    int f(int d) {
        int cnt = 1, pre = price[0]; // 先选一个价格最小的糖果
        for (int i = 1; i < priceSize; i++) {
            if (price[i] - pre >= d) { // 可以选 p
                cnt++;
                pre = price[i];
            }
        }
        return cnt;
    }

    qsort(price, priceSize, sizeof(int), cmp);
    int left = 0;
    int right = (price[priceSize - 1] - price[0]) / (k - 1) + 1;
    while (left + 1 < right) { // 开区间不为空
        // 循环不变量：
        // f(left) >= k
        // f(right) < k
        int mid = left + (right - left) / 2;
        if (f(mid) >= k) {
            left = mid;
        } else {
            right = mid;
        }
    }
    return left; // 最大的满足 f(left) >= k 的数
}
```

```go [sol-Go]
func maximumTastiness(price []int, k int) int {
    slices.Sort(price)
    return sort.Search((price[len(price)-1]-price[0])/(k-1), func(d int) bool {
        d++ // 二分最小的 f(d+1) < k，从而知道最大的 f(d) >= k
        cnt, pre := 1, price[0]
        for _, p := range price[1:] {
            if p-pre >= d {
                cnt++
                pre = p
            }
        }
        return cnt < k
    })
}
```

```js [sol-JavaScript]
var maximumTastiness = function(price, k) {
    function f(d) {
        let cnt = 1, pre = price[0]; // 先选一个价格最小的糖果
        for (const p of price) {
            if (p - pre >= d) { // 可以选 p
                cnt++;
                pre = p;
            }
        }
        return cnt;
    }

    price.sort((a, b) => a - b);
    let left = 0;
    let right = Math.floor((price[price.length - 1] - price[0]) / (k - 1)) + 1;
    while (left + 1 < right) { // 开区间不为空
        // 循环不变量：
        // f(left) >= k
        // f(right) < k
        const mid = Math.floor((left + right) / 2);
        if (f(mid) >= k) {
            left = mid;
        } else {
            right = mid;
        }
    }
    return left; // 最大的满足 f(left) >= k 的数
};
```

```rust [sol-Rust]
impl Solution {
    pub fn maximum_tastiness(mut price: Vec<i32>, k: i32) -> i32 {
        price.sort_unstable();

        let f = |d: i32| -> i32 {
            let mut cnt = 1;
            let mut pre = price[0]; // 先选一个价格最小的糖果
            for &p in &price {
                if p - pre >= d { // 可以选 p
                    cnt += 1;
                    pre = p;
                }
            }
            cnt
        };

        let mut left = 0;
        let mut right = (price.last().unwrap() - price[0]) / (k - 1) + 1;
        while left + 1 < right { // 开区间不为空
            // 循环不变量：
            // f(left) >= k
            // f(right) < k
            let mid = left + (right - left) / 2;
            if f(mid) >= k {
                left = mid;
            } else {
                right = mid;
            }
        }
        left // 最大的满足 f(left) >= k 的数
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n + n\log U)$，其中 $n$ 为 $\textit{price}$ 的长度，$U=\dfrac{\max(\textit{price})-\min(\textit{price})}{k-1}$。
- 空间复杂度：$\mathcal{O}(1)$。忽略排序的栈开销。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/circle/discuss/0viNMK/)
2. 【本题相关】[二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
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

本题来自 `二、二分答案 / §2.5 最大化最小值`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、二分答案 / §2.5 最大化最小值`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
