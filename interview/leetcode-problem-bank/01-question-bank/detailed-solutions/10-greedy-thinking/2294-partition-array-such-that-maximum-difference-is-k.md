# 2294. 划分数组使最大差为 K

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/partition-array-such-that-maximum-difference-is-k/
- 题目 slug：`partition-array-such-that-maximum-difference-is-k`
- 来源专题：贪心与思维
- 来源分类路径：一、贪心策略 / §1.5 划分型贪心
- 难度分：1416
- 外部题解来源：https://leetcode.cn/problems/partition-array-such-that-maximum-difference-is-k/solutions/1536271/pai-xu-tan-xin-by-endlesscheng-k5i4/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[排序 + 贪心（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/partition-array-such-that-maximum-difference-is-k/solutions/1536271/pai-xu-tan-xin-by-endlesscheng-k5i4/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`pai-xu-tan-xin-by-endlesscheng-k5i4`
- topic id：`1536271`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:54:06 +0800

我们只关心子序列的最大值和最小值，这与子序列的元素顺序无关。为方便计算，先把 $\textit{nums}$ 从小到大排序。

示例 1 的 $\textit{nums}=[3,6,1,2,5]$，排序后为 $[1,2,3,5,6]$，分成 $[1,2,3]$ 和 $[5,6]$ 两组，分别对应原数组的子序列 $[3,1,2]$ 和 $[6,5]$。

排序后，对于数组最小值 $1$ 来说，所有与 $1$ 相差 $\le k=2$ 的数都可以和 $1$ 在同一组。由于我们已经把数组排序了，所以与 $1$ 在同一组的数是连续的，即连续子数组 $[1,2,3]$。

现在问题变成：

- 把 $\textit{nums}$ 分割成若干段，每一段的首尾元素（最小值与最大值）之差 $\le k$，最少分多少段？

从左到右遍历数组，记录当前这段的最小值 $\textit{mn}$。如果 $\textit{nums}[i] - \textit{mn} > k$，说明 $\textit{nums}[i]$ 是下一段的最小值，那么把答案加一，同时更新 $\textit{mn} = \textit{nums}[i]$。

代码实现时，可以初始化 $\textit{mn}=-\infty$，而不是 $\textit{nums}[0]$。如果把数据范围改成 $\textit{nums}$ 长度可以是 $0$，下面的代码仍然正确。

```py [sol-Python3]
class Solution:
    def partitionArray(self, nums: List[int], k: int) -> int:
        nums.sort()
        ans = 0
        mn = -inf
        for x in nums:
            if x - mn > k:  # 必须分割
                ans += 1
                mn = x  # mn 是下一段的最小值
        return ans
```

```java [sol-Java]
class Solution {
    public int partitionArray(int[] nums, int k) {
        Arrays.sort(nums);
        int ans = 0;
        int mn = Integer.MIN_VALUE / 2; // 防止减法溢出
        for (int x : nums) {
            if (x - mn > k) { // 必须分割
                ans++;
                mn = x; // mn 是下一段的最小值
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int partitionArray(vector<int>& nums, int k) {
        ranges::sort(nums);
        int ans = 0;
        int mn = INT_MIN / 2; // 防止减法溢出
        for (int x : nums) {
            if (x - mn > k) { // 必须分割
                ans++;
                mn = x; // mn 是下一段的最小值
            }
        }
        return ans;
    }
};
```

```c [sol-C]
int cmp(const void* a, const void* b) {
    return *(int*)a - *(int*)b;
}

int partitionArray(int* nums, int numsSize, int k) {
    qsort(nums, numsSize, sizeof(int), cmp);
    int ans = 0;
    int mn = INT_MIN / 2; // 防止减法溢出
    for (int i = 0; i < numsSize; i++) {
        if (nums[i] - mn > k) { // 必须分割
            ans++;
            mn = nums[i]; // mn 是下一段的最小值
        }
    }
    return ans;
}
```

```go [sol-Go]
func partitionArray(nums []int, k int) (ans int) {
	slices.Sort(nums)
	mn := math.MinInt / 2 // 防止减法溢出
	for _, x := range nums {
		if x-mn > k { // 必须分割
			ans++
			mn = x // mn 是下一段的最小值
		}
	}
	return
}
```

```js [sol-JavaScript]
var partitionArray = function(nums, k) {
    nums.sort((a, b) => a - b);
    let ans = 0;
    let mn = -Infinity;
    for (const x of nums) {
        if (x - mn > k) { // 必须分割
            ans++;
            mn = x; // mn 是下一段的最小值
        }
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn partition_array(mut nums: Vec<i32>, k: i32) -> i32 {
        nums.sort_unstable();
        let mut ans = 0;
        let mut mn = i32::MIN / 2; // 防止减法溢出
        for x in nums {
            if x - mn > k { // 必须分割
                ans += 1;
                mn = x; // mn 是下一段的最小值
            }
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n)$，其中 $n$ 是 $\textit{nums}$ 的长度。瓶颈在排序上。
- 空间复杂度：$\mathcal{O}(1)$。忽略排序的栈开销。

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

本题来自 `一、贪心策略 / §1.5 划分型贪心`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、贪心策略 / §1.5 划分型贪心`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
