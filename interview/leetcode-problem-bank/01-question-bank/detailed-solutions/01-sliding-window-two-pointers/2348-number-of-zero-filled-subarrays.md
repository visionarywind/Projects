# 2348. 全 0 子数组的数目

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/number-of-zero-filled-subarrays/
- 题目 slug：`number-of-zero-filled-subarrays`
- 来源专题：滑动窗口与双指针
- 来源分类路径：六、分组循环
- 难度分：1316
- 外部题解来源：https://leetcode.cn/problems/number-of-zero-filled-subarrays/solutions/1693356/by-endlesscheng-men8/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种更具一般性的方法：滑动窗口/增量法（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/number-of-zero-filled-subarrays/solutions/1693356/by-endlesscheng-men8/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`by-endlesscheng-men8`
- topic id：`1693356`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 前言

本题做法很多，下面讲两个更具一般性的方法。

## 方法一：用滑动窗口思考

子数组越长，越可能包含非零元素，不满足题目要求；子数组越短，越可能全为 $0$，满足题目要求。我们枚举子数组的右端点，当右端点变大（子数组变长）的时候，子数组左端点要么不变，要么也变大。

不仅是本题，有这样性质的题目，都可以用**滑动窗口**解决，见 [滑动窗口【基础算法精讲 03】](https://www.bilibili.com/video/BV1hd4y1r7Gq/)。

本题属于「越短越合法」型滑动窗口。由于题目的特殊性，有更简单的解决方法：

1. 记录上一个非零数字的位置 $\textit{last}$。那么 $\textit{last}+1$ 就是窗口的左端点。
2. 当子数组右端点在 $i$ 时，子数组左端点可以是 $\textit{last}+1,\textit{last}+2,\dots,i$，一共有 $i-\textit{last}$ 个，加入答案。

示例 1 的 $\textit{nums} = [1,3,0,0,2,0,0,4]$，当右端点在 $i=3$ 时，$\textit{last}=1$，我们找到了 $i-\textit{last}=2$ 个右端点在 $3$ 的全 $0$ 子数组，加入答案。

为了兼容 $\textit{nums}=[0,0,0,2,0,0]$ 这种一开始就是 $0$ 的情况，初始化 $\textit{last}=-1$。

```py [sol-Python3]
class Solution:
    def zeroFilledSubarray(self, nums: List[int]) -> int:
        ans = 0
        last = -1
        for i, x in enumerate(nums):
            if x:
                last = i  # 记录上一个非 0 元素的位置
            else:
                ans += i - last
        return ans
```

```java [sol-Java]
class Solution {
    public long zeroFilledSubarray(int[] nums) {
        long ans = 0;
        int last = -1;
        for (int i = 0; i < nums.length; i++) {
            int x = nums[i];
            if (x != 0) {
                last = i; // 记录上一个非 0 元素的位置
            } else {
                ans += i - last;
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long zeroFilledSubarray(vector<int>& nums) {
        long long ans = 0;
        int last = -1;
        for (int i = 0; i < nums.size(); i++) {
            if (nums[i]) {
                last = i; // 记录上一个非 0 元素的位置
            } else {
                ans += i - last;
            }
        }
        return ans;
    }
};
```

```c [sol-C]
long long zeroFilledSubarray(int* nums, int numsSize) {
    long long ans = 0;
    int last = -1;
    for (int i = 0; i < numsSize; i++) {
        if (nums[i]) {
            last = i; // 记录上一个非 0 元素的位置
        } else {
            ans += i - last;
        }
    }
    return ans;
}
```

```go [sol-Go]
func zeroFilledSubarray(nums []int) (ans int64) {
	last := -1
	for i, x := range nums {
		if x != 0 {
			last = i // 记录上一个非 0 元素的位置
		} else {
			ans += int64(i - last)
		}
	}
	return
}
```

```js [sol-JavaScript]
var zeroFilledSubarray = function(nums) {
    let ans = 0;
    let last = -1;
    for (let i = 0; i < nums.length; i++) {
        if (nums[i] !== 0) {
            last = i; // 记录上一个非 0 元素的位置
        } else {
            ans += i - last;
        }
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn zero_filled_subarray(nums: Vec<i32>) -> i64 {
        let mut ans = 0;
        let mut last = -1;
        for (i, x) in nums.into_iter().enumerate() {
            let i = i as i64;
            if x != 0 {
                last = i; // 记录上一个非 0 元素的位置
            } else {
                ans += (i - last) as i64;
            }
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

## 方法二：增量法

遍历到 $\textit{nums}[i]=0$，现在来计算右端点为 $i$ 的全 $0$ 子数组的个数。

我们可以在右端点为 $i-1$ 的全 $0$ 子数组的末尾添加一个 $0$。比如右端点为 $i-1$ 的全 $0$ 子数组有 $5$ 个，那么在这 $5$ 个子数组的末尾添加 $\textit{nums}[i]=0$，再算上 $\textit{nums}[i]$ 单独组成一个长为 $1$ 的子数组，我们得到了 $5+1=6$ 个右端点为 $i$ 的全 $0$ 子数组，加入答案。

具体来说：

1. 用一个计数器 $\textit{cnt}_0$ 统计遍历到的连续 $0$ 的个数。
2. 如果 $\textit{nums}[i]\ne 0$，把计数器重置为 $0$。
3. 否则，把 $\textit{cnt}_0$ 加一，表示右端点为 $i$ 的全 $0$ 子数组比右端点为 $i-1$ 的全 $0$ 子数组多一个。然后把 $\textit{cnt}_0$ 加到答案中。

```py [sol-Python3]
class Solution:
    def zeroFilledSubarray(self, nums: List[int]) -> int:
        ans = cnt0 = 0
        for x in nums:
            if x:
                cnt0 = 0
            else:
                cnt0 += 1  # 右端点为 i 的全 0 子数组比右端点为 i-1 的全 0 子数组多一个
                ans += cnt0
        return ans
```

```java [sol-Java]
class Solution {
    public long zeroFilledSubarray(int[] nums) {
        long ans = 0;
        int cnt0 = 0;
        for (int x : nums) {
            if (x != 0) {
                cnt0 = 0;
            } else {
                cnt0++; // 右端点为 i 的全 0 子数组比右端点为 i-1 的全 0 子数组多一个
                ans += cnt0;
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long zeroFilledSubarray(vector<int>& nums) {
        long long ans = 0;
        int cnt0 = 0;
        for (int x : nums) {
            if (x) {
                cnt0 = 0;
            } else {
                cnt0++; // 右端点为 i 的全 0 子数组比右端点为 i-1 的全 0 子数组多一个
                ans += cnt0;
            }
        }
        return ans;
    }
};
```

```c [sol-C]
long long zeroFilledSubarray(int* nums, int numsSize) {
    long long ans = 0;
    int cnt0 = 0;
    for (int i = 0; i < numsSize; i++) {
        if (nums[i]) {
            cnt0 = 0;
        } else {
            cnt0++; // 右端点为 i 的全 0 子数组比右端点为 i-1 的全 0 子数组多一个
            ans += cnt0;
        }
    }
    return ans;
}
```

```go [sol-Go]
func zeroFilledSubarray(nums []int) (ans int64) {
	cnt0 := 0
	for _, x := range nums {
		if x != 0 {
			cnt0 = 0
		} else {
			cnt0++ // 右端点为 i 的全 0 子数组比右端点为 i-1 的全 0 子数组多一个
			ans += int64(cnt0)
		}
	}
	return
}
```

```js [sol-JavaScript]
var zeroFilledSubarray = function(nums) {
    let ans = 0;
    let cnt0 = 0;
    for (const x of nums) {
        if (x !== 0) {
            cnt0 = 0;
        } else {
            cnt0++; // 右端点为 i 的全 0 子数组比右端点为 i-1 的全 0 子数组多一个
            ans += cnt0;
        }
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn zero_filled_subarray(nums: Vec<i32>) -> i64 {
        let mut ans = 0;
        let mut cnt0 = 0;
        for x in nums {
            if x != 0 {
                cnt0 = 0;
            } else {
                cnt0 += 1; // 右端点为 i 的全 0 子数组比右端点为 i-1 的全 0 子数组多一个
                ans += cnt0 as i64;
            }
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

> **注**：本题还有其他方法，例如找极大连续 $0$ 的长度，然后用等差数列计算。

## 相似题目

[413. 等差数列划分](https://leetcode.cn/problems/arithmetic-slices/)

## 专题训练

方法一：滑动窗口题单的「**§2.3.1 越短越合法**」。

方法二：动态规划题单「**§7.3 子数组 DP**」的「**思维扩展**」。

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

本题来自 `六、分组循环`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `六、分组循环`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
