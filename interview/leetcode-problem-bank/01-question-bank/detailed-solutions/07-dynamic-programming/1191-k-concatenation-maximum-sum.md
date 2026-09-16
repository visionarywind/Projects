# 1191. K 次串联后最大子数组之和

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/k-concatenation-maximum-sum/
- 题目 slug：`k-concatenation-maximum-sum`
- 来源专题：动态规划
- 来源分类路径：一、入门 DP / §1.3 最大子数组和（最大子段和）
- 难度分：1748
- 外部题解来源：https://leetcode.cn/problems/k-concatenation-maximum-sum/solutions/3675237/fu-yong-53-ti-dai-ma-jian-ji-xie-fa-pyth-qmtp/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[复用 53 题代码，简洁写法（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/k-concatenation-maximum-sum/solutions/3675237/fu-yong-53-ti-dai-ma-jian-ji-xie-fa-pyth-qmtp/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`fu-yong-53-ti-dai-ma-jian-ji-xie-fa-pyth-qmtp`
- topic id：`3675237`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

**前置题目**：[53. 最大子数组和](https://leetcode.cn/problems/maximum-subarray/)。

如果 $k=1$，那就是 53 题。注意本题允许子数组为空，答案最小是 $0$，不可能是负数。

如果 $k=2$，我们计算的是 $\textit{arr}+\textit{arr}$ 的最大子数组和。直接调用 53 题的代码。

如果 $k\ge 3$ 呢？

**定理**：设 $s$ 是 $\textit{arr}$ 的元素和。如果 $s>0$，那么 $\textit{arr}+\textit{arr}$ 的最大子数组和必然横跨两个 $\textit{arr}$，不会在其中一个 $\textit{arr}$ 中间。

**证明方法一**：用 53 题的前缀和做法（请看 [我的题解](https://leetcode.cn/problems/maximum-subarray/solutions/2533977/qian-zhui-he-zuo-fa-ben-zhi-shi-mai-mai-abu71/)）解释。如果 $s>0$，那么前缀和的最大值一定在第二段，前缀和的最小值一定在第一段，所以最大前缀和减去最小前缀和，对应的子数组一定横跨两个 $\textit{arr}$。如下图。

![lc1191-c.png](https://pic.leetcode.cn/1747051779-OFlqet-lc1191-c.png){:width=500}

**证明方法二**：反证法。假设最大子数组只在第一个 $\textit{arr}$ 中间，下标范围为 $[l,r]$。我们可以在最大子数组后面拼接一个完整的 $\textit{arr}$，即第一个数组的后缀 $[r+1,n-1]$ 加上第二个数组的前缀 $[0,r]$，就可以得到一个更大的子数组和。这个更大的子数组一定横跨两个 $\textit{arr}$，矛盾。

对于 $k\ge 3$ 的情况，分类讨论：

- 如果 $s < 0$，那么最大子数组不可能包含一个完整的 $\textit{arr}$。反证法：如果包含，那么去掉这个完整的 $\textit{arr}$，子数组和可以增大 $-s$，矛盾。
- 如果 $s > 0$，此时我们可以把 $k$ 个 $\textit{arr}$ 看成是 $\textit{arr} + (k-2)\times \textit{arr} + \textit{arr}$。这中间的 $(k-2)$ 的 $\textit{arr}$ **必须全部包含**。反证法：如果没有包含中间的某个 $\textit{arr}$，那么包含这个 $\textit{arr}$ 可以让子数组增大 $s$，矛盾。
- $s=0$ 的情况包不包含中间的 $\textit{arr}$ 都可以。

所以我们只需要计算 $\textit{arr}+\textit{arr}$ 的最大子数组和。如果 $s>0$，那么答案额外增加 $s\cdot (k-2)$。相当于在 $\textit{arr}+\textit{arr}$ 的最大子数组和中间插入 $k-2$ 个完整的 $\textit{arr}$。

最后，小心乘法溢出，以及别忘了取模。

```py [sol-Python3]
class Solution:
    # 53. 最大子数组和（遍历 nums repeat 次）
    def maxSubArray(self, nums: List[int], repeat: int) -> int:
        ans = f = 0  # 本题允许子数组为空，ans 可以初始化成 0
        for _ in range(repeat):
            for x in nums:
                f = max(f, 0) + x
                ans = max(ans, f)
        return ans

    def kConcatenationMaxSum(self, arr: List[int], k: int) -> int:
        if k == 1:
            return self.maxSubArray(arr, 1)  # arr 的最大子数组和
        ans = self.maxSubArray(arr, 2)  # arr+arr 的最大子数组和
        ans += max(sum(arr), 0) * (k - 2)
        return ans % 1_000_000_007
```

```java [sol-Java]
class Solution {
    // 53. 最大子数组和（遍历 nums repeat 次）
    private int maxSubArray(int[] nums, int repeat) {
        int ans = 0; // 本题允许子数组为空，ans 可以初始化成 0
        int f = 0;
        while (repeat-- > 0) {
            for (int x : nums) {
                f = Math.max(f, 0) + x;
                ans = Math.max(ans, f);
            }
        }
        return ans;
    }

    public int kConcatenationMaxSum(int[] arr, int k) {
        if (k == 1) {
            return maxSubArray(arr, 1); // arr 的最大子数组和
        }
        long ans = maxSubArray(arr, 2); // arr+arr 的最大子数组和
        int s = 0;
        for (int x : arr) {
            s += x;
        }
        ans += (long) Math.max(s, 0) * (k - 2);
        return (int) (ans % 1_000_000_007);
    }
}
```

```cpp [sol-C++]
class Solution {
    // 53. 最大子数组和（遍历 nums repeat 次）
    int maxSubArray(vector<int>& nums, int repeat) {
        int ans = 0, f = 0; // 本题允许子数组为空，ans 可以初始化成 0
        while (repeat--) {
            for (int x : nums) {
                f = max(f, 0) + x;
                ans = max(ans, f);
            }
        }
        return ans;
    }

public:
    int kConcatenationMaxSum(vector<int>& arr, int k) {
        if (k == 1) {
            return maxSubArray(arr, 1); // arr 的最大子数组和
        }
        long long ans = maxSubArray(arr, 2); // arr+arr 的最大子数组和
        int s = reduce(arr.begin(), arr.end());
        ans += 1LL * max(s, 0) * (k - 2);
        return ans % 1'000'000'007;
    }
};
```

```c [sol-C]
// 53. 最大子数组和（遍历 nums repeat 次）
int maxSubArray(int* nums, int numsSize, int repeat) {
    int ans = 0, f = 0; // 本题允许子数组为空，ans 可以初始化成 0
    while (repeat--) {
        for (int i = 0; i < numsSize; i++) {
            f = MAX(f, 0) + nums[i];
            ans = MAX(ans, f);
        }
    }
    return ans;
}

int kConcatenationMaxSum(int* arr, int arrSize, int k) {
    if (k == 1) {
        return maxSubArray(arr, arrSize, 1); // arr 的最大子数组和
    }
    long long ans = maxSubArray(arr, arrSize, 2); // arr+arr 的最大子数组和
    int s = 0;
    for (int i = 0; i < arrSize; i++) {
        s += arr[i];
    }
    ans += 1LL * MAX(s, 0) * (k - 2);
    return ans % 1000000007;
}
```

```go [sol-Go]
// 53. 最大子数组和（遍历 nums repeat 次）
// 本题允许子数组为空，ans 可以初始化成 0
func maxSubArray(nums []int, repeat int) (ans int) {
	f := 0
	for range repeat {
		for _, x := range nums {
			f = max(f, 0) + x
			ans = max(ans, f)
		}
	}
	return
}

func kConcatenationMaxSum(arr []int, k int) int {
	if k == 1 {
		return maxSubArray(arr, 1) // arr 的最大子数组和
	}
	ans := maxSubArray(arr, 2) // arr+arr 的最大子数组和
	s := 0
	for _, x := range arr {
		s += x
	}
	ans += max(s, 0) * (k - 2)
	return ans % 1_000_000_007
}
```

```js [sol-JavaScript]
// 53. 最大子数组和（遍历 nums repeat 次）
var maxSubArray = function(nums, repeat) {
    let ans = 0, f = 0; // 本题允许子数组为空，ans 可以初始化成 0
    while (repeat--) {
        for (const x of nums) {
            f = Math.max(f, 0) + x;
            ans = Math.max(ans, f);
        }
    }
    return ans;
};

var kConcatenationMaxSum = function(arr, k) {
    if (k === 1) {
        return maxSubArray(arr, 1); // arr 的最大子数组和
    }
    let ans = maxSubArray(arr, 2); // arr+arr 的最大子数组和
    ans += Math.max(_.sum(arr), 0) * (k - 2);
    return ans % 1_000_000_007;
};
```

```rust [sol-Rust]
impl Solution {
    // 53. 最大子数组和（遍历 nums repeat 次）
    fn max_sub_array(nums: &[i32], repeat: usize) -> i32 {
        let mut ans = 0; // 本题允许子数组为空，ans 可以初始化成 0
        let mut f = 0;
        for _ in 0..repeat {
            for x in nums {
                f = f.max(0) + x;
                ans = ans.max(f);
            }
        }
        ans
    }

    pub fn k_concatenation_max_sum(arr: Vec<i32>, k: i32) -> i32 {
        if k == 1 {
            return Self::max_sub_array(&arr, 1); // arr 的最大子数组和
        }
        let mut ans = Self::max_sub_array(&arr, 2) as i64; // arr+arr 的最大子数组和
        let s = arr.into_iter().sum::<i32>();
        ans += s.max(0) as i64 * (k - 2) as i64;
        (ans % 1_000_000_007) as _
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{arr}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

## 专题训练

见下面动态规划题单的「**§1.3 最大子数组和**」。

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

本题来自 `一、入门 DP / §1.3 最大子数组和（最大子段和）`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、入门 DP / §1.3 最大子数组和（最大子段和）`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
