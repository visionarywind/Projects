# 2134. 最少交换次数来组合所有的 1 II

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-swaps-to-group-all-1s-together-ii/
- 题目 slug：`minimum-swaps-to-group-all-1s-together-ii`
- 来源专题：滑动窗口与双指针
- 来源分类路径：一、定长滑动窗口 / §1.2 进阶（选做）
- 难度分：1748
- 外部题解来源：https://leetcode.cn/problems/minimum-swaps-to-group-all-1s-together-ii/solutions/1200295/duan-huan-cheng-lian-hua-dong-chuang-kou-ws80/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[定长滑动窗口（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/minimum-swaps-to-group-all-1s-together-ii/solutions/1200295/duan-huan-cheng-lian-hua-dong-chuang-kou-ws80/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`duan-huan-cheng-lian-hua-dong-chuang-kou-ws80`
- topic id：`1200295`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

设 $\textit{nums}$ 中有 $k$ 个 $1$。

题目让我们在环形数组 $\textit{nums}$ 中找到一个长为 $k$ 的子数组，把这个子数组变成全 $1$ 子数组。

- 对于子数组中的 $1$，无需操作。
- 对于子数组中的 $0$，我们可以把 $0$ 与在子数组外面的 $1$ 交换。不断交换，就能让所有 $1$ 都在子数组中。

因此，子数组中有多少个 $0$，就需要交换多少次。$0$ 的个数越少，交换次数就越少。所以问题变成：

- 环形数组 $\textit{nums}$ 中的长为 $k$ 的子数组中的 $0$ 的个数的**最小值**。

这可以用 [定长滑动窗口](https://leetcode.cn/problems/maximum-number-of-vowels-in-a-substring-of-given-length/solutions/2809359/tao-lu-jiao-ni-jie-jue-ding-chang-hua-ch-fzfo/) 解决。

**细节**：

1. 本题是环形数组，第一个窗口是 $[0,k-1]$，最后一个窗口是 $[n-1,n+k-2]$。注意 $[n-1,n+k-2]$ 的下一个窗口是 $[n,n+k-1]$，在环形数组中，这与 $[0,k-1]$ 是完全一样的，所以不需要再继续枚举窗口了。
2. 如果下标 $\ge n$，我们可以将其模 $n$，映射到闭区间 $[0,n-1]$ 中。
3. 统计 $1$ 的个数比统计 $0$ 的个数方便，所以我们可以先计算出窗口中的 $1$ 的个数的最大值，然后用 $k$ 减去最大值，就是 $0$ 的个数的最小值。

```py [sol-Python3]
class Solution:
    def minSwaps(self, nums: List[int]) -> int:
        k = sum(nums)  # 1 的个数
        if k == 0:  # 没有 1，无需交换
            return 0

        n = len(nums)
        max1 = cnt1 = 0
        for i in range(n + k - 1):
            # 1. 进入窗口
            cnt1 += nums[i % n]
            if i < k - 1:  # 窗口大小不足 k
                continue
            # 2. 更新答案
            max1 = max(max1, cnt1)
            # 3. 离开窗口，为下一个循环做准备
            cnt1 -= nums[i - k + 1]  # 由于我们保证 i < n+k-1，所以 i-k+1 < n，无需取模
        return k - max1
```

```java [sol-Java]
class Solution {
    public int minSwaps(int[] nums) {
        // 统计 1 的个数
        int k = 0;
        for (int x : nums) {
            k += x;
        }
        if (k == 0) { // 没有 1，无需交换
            return 0;
        }

        int n = nums.length;
        int max1 = 0;
        int cnt1 = 0;
        for (int i = 0; i < n + k - 1; i++) {
            // 1. 进入窗口
            cnt1 += nums[i % n];
            if (i < k - 1) { // 窗口大小不足 k
                continue;
            }
            // 2. 更新答案
            max1 = Math.max(max1, cnt1);
            // 3. 离开窗口，为下一个循环做准备
            cnt1 -= nums[i - k + 1]; // 由于我们保证 i < n+k-1，所以 i-k+1 < n，无需取模
        }
        return k - max1;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minSwaps(vector<int>& nums) {
        int k = reduce(nums.begin(), nums.end(), 0); // 1 的个数
        if (k == 0) { // 没有 1，无需交换
            return 0;
        }

        int n = nums.size();
        int max1 = 0, cnt1 = 0;
        for (int i = 0; i < n + k - 1; i++) {
            // 1. 进入窗口
            cnt1 += nums[i % n];
            if (i < k - 1) { // 窗口大小不足 k
                continue;
            }
            // 2. 更新答案
            max1 = max(max1, cnt1);
            // 3. 离开窗口，为下一个循环做准备
            cnt1 -= nums[i - k + 1]; // 由于我们保证 i < n+k-1，所以 i-k+1 < n，无需取模
        }
        return k - max1;
    }
};
```

```c [sol-C]
#define MAX(a, b) ((b) > (a) ? (b) : (a))

int minSwaps(int* nums, int n) {
    // 统计 1 的个数
    int k = 0;
    for (int i = 0; i < n; i++) {
        k += nums[i];
    }
    if (k == 0) { // 没有 1，无需交换
        return 0;
    }

    int max1 = 0, cnt1 = 0;
    for (int i = 0; i < n + k - 1; i++) {
        // 1. 进入窗口
        cnt1 += nums[i % n];
        if (i < k - 1) { // 窗口大小不足 k
            continue;
        }
        // 2. 更新答案
        max1 = MAX(max1, cnt1);
        // 3. 离开窗口，为下一个循环做准备
        cnt1 -= nums[i - k + 1]; // 由于我们保证 i < n+k-1，所以 i-k+1 < n，无需取模
    }
    return k - max1;
}
```

```go [sol-Go]
func minSwaps(nums []int) int {
	// 统计 1 的个数
	k := 0
	for _, x := range nums {
		k += x
	}
	if k == 0 { // 没有 1，无需交换
		return 0
	}

	n := len(nums)
	max1, cnt1 := 0, 0
	for i := range n + k - 1 {
		// 1. 进入窗口
		cnt1 += nums[i%n]
		if i < k-1 { // 窗口大小不足 k
			continue
		}
		// 2. 更新答案
		max1 = max(max1, cnt1)
		// 3. 离开窗口，为下一个循环做准备
		cnt1 -= nums[i-k+1] // 由于我们保证 i < n+k-1，所以 i-k+1 < n，无需取模
	}
	return k - max1
}
```

```js [sol-JavaScript]
var minSwaps = function(nums) {
    const k = _.sum(nums); // 1 的个数
    if (k === 0) { // 没有 1，无需交换
        return 0;
    }

    const n = nums.length;
    let max1 = 0, cnt1 = 0;
    for (let i = 0; i < n + k - 1; i++) {
        // 1. 进入窗口
        cnt1 += nums[i % n];
        if (i < k - 1) { // 窗口大小不足 k
            continue;
        }
        // 2. 更新答案
        max1 = Math.max(max1, cnt1);
        // 3. 离开窗口，为下一个循环做准备
        cnt1 -= nums[i - k + 1]; // 由于我们保证 i < n+k-1，所以 i-k+1 < n，无需取模
    }
    return k - max1;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn min_swaps(nums: Vec<i32>) -> i32 {
        let k = nums.iter().sum::<i32>() as usize; // 1 的个数
        if k == 0 { // 没有 1，无需交换
            return 0;
        }

        let n = nums.len();
        let mut max1 = 0;
        let mut cnt1 = 0;
        for i in 0..n + k - 1 {
            // 1. 进入窗口
            cnt1 += nums[i % n];
            if i < k - 1 { // 窗口大小不足 k
                continue;
            }
            // 2. 更新答案
            max1 = max1.max(cnt1);
            // 3. 离开窗口，为下一个循环做准备
            cnt1 -= nums[i - k + 1]; // 由于我们保证 i < n+k-1，所以 i-k+1 < n，无需取模
        }
        k as i32 - max1
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

## 专题训练

见下面滑动窗口题单的「**一、定长滑动窗口**」。

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

本题来自 `一、定长滑动窗口 / §1.2 进阶（选做）`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、定长滑动窗口 / §1.2 进阶（选做）`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
