# 135. 分发糖果

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/candy/
- 题目 slug：`candy`
- 来源专题：贪心与思维
- 来源分类路径：五、思维题 / §5.6 两次扫描
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/candy/solutions/3691236/fen-zu-xun-huan-ba-kun-nan-ti-bian-cheng-fo15/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[分组循环，把困难题变成简单题，一次遍历（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/candy/solutions/3691236/fen-zu-xun-huan-ba-kun-nan-ti-bian-cheng-fo15/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`fen-zu-xun-huan-ba-kun-nan-ti-bian-cheng-fo15`
- topic id：`3691236`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:13:14 +0800

## 分析

题目要求，每个孩子至少分 $1$ 颗糖果。我们**先给每个孩子 $1$ 颗糖果，下面就不用考虑这个要求了，可以给孩子最少 $0$ 颗糖果**。

题目要求，相邻两个孩子评分更大的孩子会获得更多的糖果。如何理解这句话呢？

比如 $\textit{ratings}=[1,2,3,4,5,4,3,2]$，由严格递增段 $[1,2,3,4]$、峰顶 $5$ 和严格递减段 $[4,3,2]$ 组成。

- 对于严格递增段，最左边的孩子旁边没有评分比他小的孩子，所以给他 $0$ 颗糖果就行（注意我们在一开始给了每个孩子一颗糖果），下一个孩子的糖果必须比上一个孩子的多，贪心地，多 $1$ 就行。所以分给严格递增段的糖果数为 $0,1,2,3$。
- 对于严格递减段，最右边的孩子旁边没有评分比他小的孩子，所以给他 $0$ 颗糖果就行（注意我们在一开始给了每个孩子一颗糖果），往左看，左边孩子的糖果必须比右边孩子多，贪心地，多 $1$ 就行。所以分给严格递减段的糖果数为 $2,1,0$。
- 最后确定峰顶的糖果数，必须比严格递增段的最大值多，即 $\ge 4$；也必须比严格递减段的最大值多，即 $\ge 3$，取 $\max(4,3)=4$ 作为峰顶的糖果数。

把严格递增段 + 峰顶 + 严格递减段叫做一座山。

如果数组中只有一座山，计算糖果总数是简单的。但如果有多座山呢？如何找到每座山？

## 分组循环

**适用场景**：按照题目要求，数组会被分割成若干组，每一组的判断/处理逻辑是相同的。

**核心思想**：

- 外层循环负责遍历组之前的准备工作（记录开始位置），和遍历组之后的统计工作（更新答案）。
- 内层循环负责遍历组，找出这一组最远在哪结束。

这个写法的好处是，各个逻辑块分工明确，也不需要特判最后一组（易错点）。以我的经验，这个写法是所有写法中最不容易出 bug 的，推荐大家记住。

## 详细计算过程

以 $\textit{ratings}=[1,2,3,4,2,1,4,5,2]$ 为例说明：

- 第一组，先上坡再下坡，即 $[1,2,3,4,2,1]$。
  - 记录起始下标 $\textit{start}=i=0$。
  - 写一个循环找严格递增段，如果 $\textit{ratings}[i]<ratings[i+1]$ 就继续循环。循环结束时 $i$ 就是峰顶下标，记作 $\textit{top}$。这里 $\textit{top}=3$。
  - 写一个循环找严格递减段，如果 $\textit{ratings}[i]>ratings[i+1]$ 就继续循环。循环结束时 $i$ 就是严格递减段最后一个数的下标。这里 $i=5$。
  - 设 $\textit{inc}=\textit{top}-\textit{start}$。从 $\textit{start}$ 到 $\textit{top}-1$，分到的糖果数依次为 $0,1,2,\ldots,\textit{inc}-1$（注意我们在一开始给了每个孩子一颗糖果），根据等差数列求和公式，得糖果总数为 $\dfrac{\textit{inc}(\textit{inc}-1)}{2}$。这里 $\textit{inc}=3$，糖果总数为 $0+1+2=3$。
  - 设 $\textit{dec}=i-\textit{top}$。从 $\textit{top}+1$ 到 $i$ 到，分到的糖果数依次为 $\textit{dec}-1,\textit{dec}-2,\ldots,0$，根据等差数列求和公式，得糖果总数为 $\dfrac{\textit{dec}(\textit{dec}-1)}{2}$。这里 $\textit{dec}=2$，糖果总数为 $0+1=1$。
  - 峰顶的糖果数为 $\max(\textit{inc},\textit{dec})=3$。
  - 把 $i$ 加一，找下一组。
- 第二组，先上坡再下坡，即 $[1,4,5,2]$。做法同上。
  - 注意这里的 $1$ 也是前一组的最后一个数（**谷底会被两组共享**）。在记录 $\textit{start}$ 时，如果 $i>0$ 且 $\textit{ratings}[i-1] < \textit{ratings}[i]$，说明 $i-1$ 是谷底，记录 $\textit{start}=i-1$，否则记录 $\textit{start}=i$。
  - ⚠**注意**：我们在一开始给了每个孩子一颗糖果，在后面分组循环的计算过程中，不考虑一开始分的这颗糖果。第一组的最后一个数虽然也在第二组的第一个数中，但由于这个孩子在分组循环中分到的糖果数是 $0$，所以不会重复统计。

对于 $\textit{ratings}=[1,2,2,3]$ 这种包含相邻相同元素的情况，按照上文的规则，分成两组 $[1,2]$ 和 $[2,3]$。糖果数为 $0,1,0,1$。

特别地，如果一组的第一个数是峰顶，则 $\textit{inc}=0$；如果一组的最后一个数是峰顶，则 $\textit{dec}=0$。

示例 1 $\textit{ratings}=[1,0,2]$，分成两组 $[1,0]$ 和 $[0,2]$。答案为 $3$（先给每个孩子一颗糖果）加上 $1+0$ 加上 $0+1$ 等于 $5$。

示例 2 $\textit{ratings}=[1,2,2]$，分成两组 $[1,2]$ 和 $[2]$。答案为 $3$（先给每个孩子一颗糖果）加上 $0+1$ 加上 $0$ 等于 $4$。

## 总结

先给每个孩子一颗糖果，即初始化答案为数组长度 $n$。

然后用分组循环计算每一组（严格递增段 + 峰顶 + 严格递减段）的 $\textit{inc}$ 和 $\textit{dec}$，把答案增加

$$
\dfrac{\textit{inc}(\textit{inc}-1) + \textit{dec}(\textit{dec}-1)}{2} + \max(\textit{inc},\textit{dec})
$$

```py [sol-Python3]
class Solution:
    def candy(self, ratings: List[int]) -> int:
        ans = n = len(ratings)  # 先给每人分一个
        i = 0
        while i < n:
            start = i - 1 if i > 0 and ratings[i - 1] < ratings[i] else i

            # 找严格递增段
            while i + 1 < n and ratings[i] < ratings[i + 1]:
                i += 1
            top = i  # 峰顶

            # 找严格递减段
            while i + 1 < n and ratings[i] > ratings[i + 1]:
                i += 1

            inc = top - start  # start 到 top 严格递增
            dec = i - top      # top 到 i 严格递减
            ans += (inc * (inc - 1) + dec * (dec - 1)) // 2 + max(inc, dec)
            i += 1
        return ans
```

```java [sol-Java]
class Solution {
    public int candy(int[] ratings) {
        int n = ratings.length;
        int ans = n; // 先给每人分一个
        for (int i = 0; i < n; i++) {
            int start = i > 0 && ratings[i - 1] < ratings[i] ? i - 1 : i;

            // 找严格递增段
            while (i + 1 < n && ratings[i] < ratings[i + 1]) {
                i++;
            }
            int top = i; // 峰顶

            // 找严格递减段
            while (i + 1 < n && ratings[i] > ratings[i + 1]) {
                i++;
            }

            long inc = top - start; // start 到 top 严格递增
            long dec = i - top;     // top 到 i 严格递减
            ans += (inc * (inc - 1) + dec * (dec - 1)) / 2 + Math.max(inc, dec);
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int candy(vector<int>& ratings) {
        int n = ratings.size();
        int ans = n; // 先给每人分一个
        for (int i = 0; i < n; i++) {
            int start = i > 0 && ratings[i - 1] < ratings[i] ? i - 1 : i;

            // 找严格递增段
            while (i + 1 < n && ratings[i] < ratings[i + 1]) {
                i++;
            }
            int top = i; // 峰顶

            // 找严格递减段
            while (i + 1 < n && ratings[i] > ratings[i + 1]) {
                i++;
            }

            long long inc = top - start; // start 到 top 严格递增
            long long dec = i - top;     // top 到 i 严格递减
            ans += (inc * (inc - 1) + dec * (dec - 1)) / 2 + max(inc, dec);
        }
        return ans;
    }
};
```

```c [sol-C]
int candy(int* ratings, int ratingsSize) {
    int ans = ratingsSize; // 先给每人分一个
    for (int i = 0; i < ratingsSize; i++) {
        int start = i > 0 && ratings[i - 1] < ratings[i] ? i - 1 : i;

        // 找严格递增段
        while (i + 1 < ratingsSize && ratings[i] < ratings[i + 1]) {
            i++;
        }
        int top = i; // 峰顶

        // 找严格递减段
        while (i + 1 < ratingsSize && ratings[i] > ratings[i + 1]) {
            i++;
        }

        long long inc = top - start; // start 到 top 严格递增
        long long dec = i - top;     // top 到 i 严格递减
        ans += (inc * (inc - 1) + dec * (dec - 1)) / 2 + MAX(inc, dec);
    }
    return ans;
}
```

```go [sol-Go]
func candy(ratings []int) int {
    n := len(ratings)
    ans := n // 先给每人分一个
    for i := 0; i < n; i++ {
        start := i
        if i > 0 && ratings[i-1] < ratings[i] {
            start--
        }

        // 找严格递增段
        for i+1 < n && ratings[i] < ratings[i+1] {
            i++
        }
        top := i // 峰顶

        // 找严格递减段
        for i+1 < n && ratings[i] > ratings[i+1] {
            i++
        }

        inc := top - start // start 到 top 严格递增
        dec := i - top     // top 到 i 严格递减
        ans += (inc*(inc-1)+dec*(dec-1))/2 + max(inc, dec)
    }
    return ans
}
```

```js [sol-JavaScript]
var candy = function(ratings) {
    const n = ratings.length;
    let ans = n; // 先给每人分一个
    for (let i = 0; i < n; i++) {
        let start = i > 0 && ratings[i - 1] < ratings[i] ? i - 1 : i;

        // 找严格递增段
        while (i + 1 < n && ratings[i] < ratings[i + 1]) {
            i++;
        }
        const top = i; // 峰顶

        // 找严格递减段
        while (i + 1 < n && ratings[i] > ratings[i + 1]) {
            i++;
        }

        const inc = top - start; // start 到 top 严格递增
        const dec = i - top;     // top 到 i 严格递减
        ans += (inc * (inc - 1) + dec * (dec - 1)) / 2 + Math.max(inc, dec);
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn candy(ratings: Vec<i32>) -> i32 {
        let n = ratings.len();
        let mut ans = n; // 先给每人分一个
        let mut i = 0;
        while i < n {
            let start = if i > 0 && ratings[i - 1] < ratings[i] { i - 1 } else { i };

            // 找严格递增段
            while i + 1 < n && ratings[i] < ratings[i + 1] {
                i += 1;
            }
            let top = i; // 峰顶

            // 找严格递减段
            while i + 1 < n && ratings[i] > ratings[i + 1] {
                i += 1;
            }

            let inc = top - start; // start 到 top 严格递增
            let dec = i - top;     // top 到 i 严格递减
            ans += (inc * (inc - 1) + dec * (dec - 1)) / 2 + inc.max(dec);
            i += 1;
        }
        ans as _
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{ratings}$ 的长度。时间复杂度乍一看是 $\mathcal{O}(n^2)$，但注意变量 $i$ 只会增加，不会重置也不会减少。所以二重循环总共循环 $\mathcal{O}(n)$ 次，所以时间复杂度是 $\mathcal{O}(n)$。
- 空间复杂度：$\mathcal{O}(1)$。

## 相似题目

- [845. 数组中的最长山脉](https://leetcode.cn/problems/longest-mountain-in-array/)
- [3105. 最长的严格递增或递减子数组](https://leetcode.cn/problems/longest-strictly-increasing-or-strictly-decreasing-subarray/)

更多相似题目，见下面滑动窗口与双指针题单的「**六、分组循环**」。

## 分类题单

[如何科学刷题？](https://leetcode.cn/discuss/post/3141566/ru-he-ke-xue-shua-ti-by-endlesscheng-q3yd/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/discuss/post/3578981/ti-dan-hua-dong-chuang-kou-ding-chang-bu-rzz7/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/discuss/post/3579164/ti-dan-er-fen-suan-fa-er-fen-da-an-zui-x-3rqn/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/discuss/post/3579480/ti-dan-dan-diao-zhan-ju-xing-xi-lie-zi-d-u4hk/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/discuss/post/3580195/fen-xiang-gun-ti-dan-wang-ge-tu-dfsbfszo-l3pa/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/discuss/post/3580371/fen-xiang-gun-ti-dan-wei-yun-suan-ji-chu-nth4/)
6. [图论算法（DFS/BFS/拓扑排序/基环树/最短路/最小生成树/网络流）](https://leetcode.cn/discuss/post/3581143/fen-xiang-gun-ti-dan-tu-lun-suan-fa-dfsb-qyux/)
7. [动态规划（入门/背包/划分/状态机/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/discuss/post/3581838/fen-xiang-gun-ti-dan-dong-tai-gui-hua-ru-007o/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/discuss/post/3583665/fen-xiang-gun-ti-dan-chang-yong-shu-ju-j-bvmv/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/discuss/post/3584388/fen-xiang-gun-ti-dan-shu-xue-suan-fa-shu-gcai/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/discuss/post/3091107/fen-xiang-gun-ti-dan-tan-xin-ji-ben-tan-k58yb/)
11. [链表、树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/discuss/post/3142882/fen-xiang-gun-ti-dan-lian-biao-er-cha-sh-6srp/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/discuss/post/3144832/fen-xiang-gun-ti-dan-zi-fu-chuan-kmpzhan-ugt4/)

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
