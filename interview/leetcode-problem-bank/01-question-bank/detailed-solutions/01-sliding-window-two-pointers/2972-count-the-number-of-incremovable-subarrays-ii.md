# 2972. 统计移除递增子数组的数目 II

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/count-the-number-of-incremovable-subarrays-ii/
- 题目 slug：`count-the-number-of-incremovable-subarrays-ii`
- 来源专题：滑动窗口与双指针
- 来源分类路径：三、单序列双指针 / §3.3 同向双指针
- 难度分：2153
- 外部题解来源：https://leetcode.cn/problems/count-the-number-of-incremovable-subarrays-ii/solutions/2577663/shuang-zhi-zhen-on-shi-jian-o1-kong-jian-2hsz/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[双指针，O(n) 时间 O(1) 空间，附变形题（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/count-the-number-of-incremovable-subarrays-ii/solutions/2577663/shuang-zhi-zhen-on-shi-jian-o1-kong-jian-2hsz/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`shuang-zhi-zhen-on-shi-jian-o1-kong-jian-2hsz`
- topic id：`2577663`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

首先讨论有多少个**后缀**可以移除，然后讨论一般情况。

### 可以移除多少个后缀？

**核心思路**：如果移除的是后缀，那么移除后，剩下的是前缀，且这个前缀必须是严格递增的。

为方便描述，下文将 $\textit{nums}$ 简记为 $a$，其长度为 $n$。

设 $a$ 的**最长严格递增前缀**的最后一个数是 $a[i]$。例如 $a=[1,3,4,1,2]$，最长严格递增前缀的最后一个数是 $a[2]=4$。

首先特判，如果 $i=n-1$，说明 $a$ 是严格递增数组，所有非空子数组都可以移除，那么直接返回非空子数组的个数 $\dfrac{n(n+1)}{2}$。

接下来讨论 $a$ **不是**严格递增数组的情况。

可以移除如下后缀（下标范围）：

- $[i+1,n-1]$，移除该后缀，完整保留最长严格递增前缀。
- $[i,n-1]$
- $[i-1,n-1]$
- $\cdots$
- $[0,n-1]$，移除整个数组。

这一共有 $i+2$ 个。

例如 $a=[1,3,4,1,2]$，计算出的 $i=2$，可以移除：

- 下标范围 $[3,4]$，即后缀 $[1,2]$，剩余元素为 $[1,3,4]$。
- 下标范围 $[2,4]$，即后缀 $[4,1,2]$，剩余元素为 $[1,3]$。
- 下标范围 $[1,4]$，即后缀 $[3,4,1,2]$，剩余元素为 $[1]$。
- 下标范围 $[0,4]$，即整个数组，剩余元素为 $[]$。

一共 $i+2=4$ 个后缀。

### 一般情况

**核心思路**：移除子数组后，剩下的部分是一个前缀加一个后缀，需要满足：

- 前缀是严格递增的；
- 后缀是严格递增的；
- 前缀的最后一个数严格小于后缀的第一个数。

设后缀的第一个数为 $a[j]$，也就是说，移除的子数组的最后一个数是 $a[j-1]$。

枚举 $j=n-1,n-2,n-3,\cdots,1$，如果 $a[j]\ge a[j+1]$ 则停止枚举。对于**固定的** $j$，有多少个不同的子数组可以移除？

> 注意 $j$ 不能为 $0$，因为不能移除空数组。

枚举 $j$ 的同时，维护最长前缀的最后一个数的下标 $i$（满足 $a[i] < a[j]$），也就是说，移除的子数组的第一个数的下标至多为 $i+1$。

由于 $j$ 越小，$a[j]$ 越小，$a[i]$ 也越小，有**单调性**，所以可以像 [滑动窗口](https://www.bilibili.com/video/BV1hd4y1r7Gq/) 那样不断左移 $i$ 直到 $i<0$ 或者 $a[i]<a[j]$ 为止。

类似移除后缀的情况，对于固定的 $j$，可以移除如下子数组（下标区间）：

- $[i+1,j-1]$
- $[i,j-1]$
- $[i-1,j-1]$
- $\cdots$
- $[0,j-1]$

这一共有 $i+2$ 个。注意 $i=-1$ 时只能移除 $1$ 个子数组，即 $[0,j-1]$，同样符合 $i+2$ 这个结论，因为 $(-1)+2=1$。

累加这些 $i+2$，即为答案。

> 由于不能移除空数组，$i$ 与 $j$ 的中间至少要有一个数，所以必须要有 $i\le j-2$。但是 $i=j-1$ 的情况说明 $a$ 是严格递增数组，已经在前面特判了，所以无需判断 $i$ 和 $j-2$ 的大小关系。

[本题视频讲解](https://www.bilibili.com/video/BV1jg4y1y7PA/?t=5m04s)

```py [sol-Python3]
class Solution:
    def incremovableSubarrayCount(self, a: List[int]) -> int:
        n = len(a)
        i = 0
        while i < n - 1 and a[i] < a[i + 1]:
            i += 1
        if i == n - 1:  # 每个非空子数组都可以移除
            return n * (n + 1) // 2

        ans = i + 2  # 不保留后缀的情况，一共 i+2 个
        # 枚举保留的后缀为 a[j:]
        j = n - 1
        while j == n - 1 or a[j] < a[j + 1]:
            while i >= 0 and a[i] >= a[j]:
                i -= 1
            # 可以保留前缀 a[:i+1], a[:i], ..., a[:0] 一共 i+2 个
            ans += i + 2
            j -= 1
        return ans
```

```java [sol-Java]
class Solution {
    public long incremovableSubarrayCount(int[] a) {
        int n = a.length;
        int i = 0;
        while (i < n - 1 && a[i] < a[i + 1]) {
            i++;
        }
        if (i == n - 1) { // 每个非空子数组都可以移除
            return (long) n * (n + 1) / 2;
        }

        long ans = i + 2; // 不保留后缀的情况，一共 i+2 个
        // 枚举保留的后缀为 a[j:]
        for (int j = n - 1; j == n - 1 || a[j] < a[j + 1]; j--) {
            while (i >= 0 && a[i] >= a[j]) {
                i--;
            }
            // 可以保留前缀 a[:i+1], a[:i], ..., a[:0] 一共 i+2 个
            ans += i + 2;
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long incremovableSubarrayCount(vector<int> &a) {
        int n = a.size();
        int i = 0;
        while (i < n - 1 && a[i] < a[i + 1]) {
            i++;
        }
        if (i == n - 1) { // 每个非空子数组都可以移除
            return (long long) n * (n + 1) / 2;
        }

        long long ans = i + 2; // 不保留后缀的情况，一共 i+2 个
        // 枚举保留的后缀为 a[j:]
        for (int j = n - 1; j == n - 1 || a[j] < a[j + 1]; j--) {
            while (i >= 0 && a[i] >= a[j]) {
                i--;
            }
            // 可以保留前缀 a[:i+1], a[:i], ..., a[:0] 一共 i+2 个
            ans += i + 2;
        }
        return ans;
    }
};
```

```c [sol-C]
long long incremovableSubarrayCount(int* a, int n) {
    int i = 0;
    while (i < n - 1 && a[i] < a[i + 1]) {
        i++;
    }
    if (i == n - 1) { // 每个非空子数组都可以移除
        return (long long) n * (n + 1) / 2;
    }

    long long ans = i + 2; // 不保留后缀的情况，一共 i+2 个
    // 枚举保留的后缀为 a[j:]
    for (int j = n - 1; j == n - 1 || a[j] < a[j + 1]; j--) {
        while (i >= 0 && a[i] >= a[j]) {
            i--;
        }
        // 可以保留前缀 a[:i+1], a[:i], ..., a[:0] 一共 i+2 个
        ans += i + 2;
    }
    return ans;
}
```

```go [sol-Go]
func incremovableSubarrayCount(a []int) int64 {
	n := len(a)
	i := 0
	for i < n-1 && a[i] < a[i+1] {
		i++
	}
	if i == n-1 { // 每个非空子数组都可以移除
		return int64(n) * int64(n+1) / 2
	}

	ans := int64(i + 2) // 不保留后缀的情况，一共 i+2 个
	// 枚举保留的后缀为 a[j:]
	for j := n - 1; j == n-1 || a[j] < a[j+1]; j-- {
		for i >= 0 && a[i] >= a[j] {
			i--
		}
		// 可以保留前缀 a[:i+1], a[:i], ..., a[:0] 一共 i+2 个
		ans += int64(i + 2)
	}
	return ans
}
```

```js [sol-JavaScript]
var incremovableSubarrayCount = function(a) {
    const n = a.length;
    let i = 0;
    while (i < n - 1 && a[i] < a[i + 1]) {
        i++;
    }
    if (i === n - 1) { // 每个非空子数组都可以移除
        return n * (n + 1) / 2;
    }

    let ans = i + 2; // 不保留后缀的情况，一共 i+2 个
    // 枚举保留的后缀为 a[j:]
    for (let j = n - 1; j === n - 1 || a[j] < a[j + 1]; j--) {
        while (i >= 0 && a[i] >= a[j]) {
            i--;
        }
        // 可以保留前缀 a[:i+1], a[:i], ..., a[:0] 一共 i+2 个
        ans += i + 2;
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn incremovable_subarray_count(a: Vec<i32>) -> i64 {
        let n = a.len();
        let mut i = 0;
        while i < n - 1 && a[i] < a[i + 1] {
            i += 1;
        }
        if i == n - 1 { // 每个非空子数组都可以移除
            return n as i64 * (n + 1) as i64 / 2;
        }

        let mut i = i as i64;
        let mut ans = i + 2; // 不保留后缀的情况，一共 i+2 个
        // 枚举保留的后缀为 a[j:]
        let mut j = n - 1;
        while j == n - 1 || a[j] < a[j + 1] {
            while i >= 0 && a[i as usize] >= a[j] {
                i -= 1;
            }
            // 可以保留前缀 a[:i+1], a[:i], ..., a[:0] 一共 i+2 个
            ans += i + 2;
            j -= 1;
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{nums}$ 的长度。注意二重循环中的下标 $i$ 和 $j$ 都只会减小，不会变大。由于下标只会减小 $\mathcal{O}(n)$ 次，所以二重循环的总循环次数是 $\mathcal{O}(n)$ 的。
- 空间复杂度：$\mathcal{O}(1)$。

## 思考题（变形题）

1. 移除的子数组，最短长度是多少？见 [1574. 删除最短的子数组使剩余数组有序](https://leetcode.cn/problems/shortest-subarray-to-be-removed-to-make-array-sorted/)。
2. 改成移除所有元素值在 $[L,R]$ 内的元素，使得移除后，剩余元素是非降的。有多少个这样的 $(L,R)$ 数对？见 [CF1167E. Range Deleting](https://codeforces.com/problemset/problem/1167/E)。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口（定长/不定长/多指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心算法（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

## 本地原创解析

### 1. 题意重述

本题来自 `三、单序列双指针 / §3.3 同向双指针`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `三、单序列双指针 / §3.3 同向双指针`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
