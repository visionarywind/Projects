# 303. 区域和检索 - 数组不可变

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/range-sum-query-immutable/
- 题目 slug：`range-sum-query-immutable`
- 来源专题：常用数据结构
- 来源分类路径：一、前缀和 / §1.1 基础
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/range-sum-query-immutable/solutions/2693498/qian-zhui-he-ji-qi-kuo-zhan-fu-ti-dan-py-vaar/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[前缀和，附扩展问题（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/range-sum-query-immutable/solutions/2693498/qian-zhui-he-ji-qi-kuo-zhan-fu-ti-dan-py-vaar/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`qian-zhui-he-ji-qi-kuo-zhan-fu-ti-dan-py-vaar`
- topic id：`2693498`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 16:46:02 +0800

## 关键思路

> 注：本文中的「子数组」均表示「连续子数组」。

比如 $\textit{nums}=[1,2,3,4,5,6]$，要想计算子数组 $[3,4,5]$ 的元素和，可以用前缀 $[1,2,3,4,5]$ 的元素和，减去另一个前缀 $[1,2]$ 的元素和，就得到了子数组 $[3,4,5]$ 的元素和，即

$$
3+4+5 = (1+2+3+4+5) - (1+2)
$$

换句话说，把前缀 $[1,2,3,4,5]$ 的前缀 $[1,2]$ 去掉，就得到了子数组 $[3,4,5]$。

一般地，任意子数组都是一个前缀去掉前缀后的结果。所以**任意子数组的和，都可以表示为两个前缀和的差**。

于是，预处理 $\textit{nums}$ **所有**的前缀和，就可以 $\mathcal{O}(1)$ 计算任意子数组的元素和。

> 注：$\textit{nums}$ 有 $\mathcal{O}(n^2)$ 个子数组，如果把每个子数组都算一遍，就太慢了。但注意到 $\textit{nums}$ 只有 $\mathcal{O}(n)$ 个前缀，且子数组可以用两个前缀和之差表示，所以本题解的做法相当于把 $\mathcal{O}(n^2)$ 的信息压缩到 $\mathcal{O}(n)$ 个前缀中。

## 具体思路

为方便描述，把 $\textit{nums}$ 记作 $a$，设其长度为 $n$。

对于数组 $\textit{a}$，计算它的长为 $n+1$ 的前缀和数组 $s$，即 $a$ 的前 $0$ 个数的和，前 $1$ 个数的和，前 $2$ 个数的和……前 $n$ 个数的和。

$$
\begin{aligned}
s[0] &= 0\\
s[1] &= a[0]\\
s[2] &= a[0]+a[1]\\
&\ \ \vdots\\
s[i] &= a[0] + a[1] + \cdots + a[i-1] \\
s[i+1] &= a[0] + a[1] + \cdots + a[i-1] + a[i] \\
&\ \ \vdots\\
s[n] &= a[0] + a[1] + \cdots + a[n-1]
\end{aligned}
$$

为什么要定义 $s[0]=0$ 见下文答疑。

根据这个定义，前 $i$ 个数的和，加上 $a[i]$，就是前 $i+1$ 个数的和，即

$$
s[i+1]=s[i]+\textit{a}[i]
$$

示例中的数组 $[-2, 0, 3, -5, 2, -1]$，对应的前缀和数组 $s=[0,-2, -2, 1, -4, -2, -3]$。

通过前缀和，我们可以把**子数组的元素和转化成两个前缀和的差**，下标区间 $[\textit{left},\textit{right}]$ 的元素和等于前缀 $[0,\textit{right}]$ 的元素和减去另一个前缀 $[0,\textit{left}-1]$ 的元素和，即

$$
\textit{s}[\textit{right}+1] - \textit{s}[\textit{left}]
$$

有了这个式子，示例中子数组 $[3, -5, 2, -1]$ 的和，就可以 $\mathcal{O}(1)$ 地用 $s[6]-s[2]=-3-(-2)=-1$ 算出来。

## 答疑

**问**：为什么要定义 $s[0]=0$，这样做有什么好处？

**答**：如果 $\textit{left}=0$，要计算的子数组是一个前缀（从 $\textit{a}[0]$ 到 $\textit{a}[\textit{right}]$）。按照公式，我们要用 $s[\textit{right}+1]$ 减去 $s[0]$。如果不定义 $s[0]=0$，就必须特判 $\textit{left}=0$ 的情况了（读者可以试试）。通过定义 $s[0]=0$，任意子数组（包括前缀）都可以表示为两个前缀和的差。此外，如果 $\textit{a}$ 是空数组，定义 $s[0]=0$ 的写法是可以兼容这种情况的。

```py [sol-Python3]
class NumArray:
    def __init__(self, nums: List[int]):
        s = [0] * (len(nums) + 1)
        for i, x in enumerate(nums):
            s[i + 1] = s[i] + x
        self.s = s

    def sumRange(self, left: int, right: int) -> int:
        return self.s[right + 1] - self.s[left]
```

```py [sol-Python3 库函数]
class NumArray:
    def __init__(self, nums: List[int]):
        self.s = list(accumulate(nums, initial=0))

    def sumRange(self, left: int, right: int) -> int:
        return self.s[right + 1] - self.s[left]
```

```java [sol-Java]
class NumArray {
    private final int[] s;

    public NumArray(int[] nums) {
        s = new int[nums.length + 1];
        for (int i = 0; i < nums.length; i++) {
            s[i + 1] = s[i] + nums[i];
        }
    }

    public int sumRange(int left, int right) {
        return s[right + 1] - s[left];
    }
}
```

```cpp [sol-C++]
class NumArray {
    vector<int> s;

public:
    NumArray(vector<int>& nums) {
        s.resize(nums.size() + 1);
        for (int i = 0; i < nums.size(); i++) {
            s[i + 1] = s[i] + nums[i];
        }
    }

    int sumRange(int left, int right) {
        return s[right + 1] - s[left];
    }
};
```

```cpp [sol-C++ 库函数]
class NumArray {
    vector<int> s;

public:
    NumArray(vector<int>& nums) {
        s.resize(nums.size() + 1);
        partial_sum(nums.begin(), nums.end(), s.begin() + 1);
    }

    int sumRange(int left, int right) {
        return s[right + 1] - s[left];
    }
};
```

```c [sol-C]
typedef struct {
    int* s;
} NumArray;

NumArray* numArrayCreate(int* nums, int numsSize) {
    NumArray* obj = malloc(sizeof(NumArray));
    obj->s = malloc((numsSize + 1) * sizeof(int));
    obj->s[0] = 0;
    for (int i = 0; i < numsSize; i++) {
        obj->s[i + 1] = obj->s[i] + nums[i];
    }
    return obj;
}

int numArraySumRange(NumArray* obj, int left, int right) {
    return obj->s[right + 1] - obj->s[left];
}

void numArrayFree(NumArray* obj) {
    free(obj->s);
    free(obj);
}
```

```go [sol-Go]
type NumArray []int

func Constructor(nums []int) NumArray {
    s := make(NumArray, len(nums)+1)
    for i, x := range nums {
        s[i+1] = s[i] + x
    }
    return s
}

func (s NumArray) SumRange(left, right int) int {
    return s[right+1] - s[left]
}
```

```js [sol-JS]
class NumArray {
    constructor(nums) {
        this.s = Array(nums.length + 1);
        this.s[0] = 0;
        for (let i = 0; i < nums.length; i++) {
            this.s[i + 1] = this.s[i] + nums[i];
        }
    }

    sumRange(left, right) {
        return this.s[right + 1] - this.s[left];
    }
}
```

```rust [sol-Rust]
struct NumArray {
    s: Vec<i32>,
}

impl NumArray {
    fn new(nums: Vec<i32>) -> Self {
        let mut s = vec![0; nums.len() + 1];
        for (i, x) in nums.into_iter().enumerate() {
            s[i + 1] = s[i] + x;
        }
        Self { s }
    }

    fn sum_range(&self, left: i32, right: i32) -> i32 {
        self.s[right as usize + 1] - self.s[left as usize]
    }
}
```

#### 复杂度分析

- 时间复杂度：初始化是 $\mathcal{O}(n)$，其中 $n$ 是 $\textit{nums}$ 的长度。$\texttt{sumRange}$ 是 $\mathcal{O}(1)$。
- 空间复杂度：$\mathcal{O}(n)$。

## 扩展问题

1. 如何计算数组元素到某个数的**距离之和**？见 [2602. 使数组元素全部相等的最少操作次数](https://leetcode.cn/problems/minimum-operations-to-make-all-array-elements-equal/)，[我的题解](https://leetcode.cn/problems/minimum-operations-to-make-all-array-elements-equal/solution/yi-tu-miao-dong-pai-xu-qian-zhui-he-er-f-nf55/)
2. 如何计算元素和等于 $k$ 的**子数组个数**？见 [560. 和为 K 的子数组](https://leetcode.cn/problems/subarray-sum-equals-k/)
3. 把 $\textit{nums}$ 改成**二维矩阵**，如何计算**子矩阵的元素和**？见 [304. 二维区域和检索 - 矩阵不可变](https://leetcode.cn/problems/range-sum-query-2d-immutable/)，[图解](https://leetcode.cn/problems/range-sum-query-2d-immutable/solution/tu-jie-yi-zhang-tu-miao-dong-er-wei-qian-84qp/)
4. 如果可以**修改** $\textit{nums}$ 的元素值呢？见 [307. 区域和检索 - 数组可修改](https://leetcode.cn/problems/range-sum-query-mutable/)，[我的题解](https://leetcode.cn/problems/range-sum-query-mutable/solution/dai-ni-fa-ming-shu-zhuang-shu-zu-fu-shu-lyfll/)
5. 对于 [53. 最大子数组和](https://leetcode.cn/problems/maximum-subarray/)，除了 DP 做法外，还可以用 [前缀和](https://leetcode.cn/problems/maximum-subarray/solution/qian-zhui-he-zuo-fa-ben-zhi-shi-mai-mai-abu71/) 解决。这一做法可以扩展到子数组长度有下限/上限，子数组元素和有上限等。

更多相似题目，见下面数据结构题单中的「**一、前缀和**」。

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

本题来自 `一、前缀和 / §1.1 基础`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、前缀和 / §1.1 基础`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
