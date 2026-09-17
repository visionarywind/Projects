# 3107. 使数组中位数等于 K 的最少操作数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-operations-to-make-median-of-array-equal-to-k/
- 题目 slug：`minimum-operations-to-make-median-of-array-equal-to-k`
- 来源专题：贪心与思维
- 来源分类路径：四、数学贪心 / §4.7 其他数学贪心
- 难度分：1605
- 外部题解来源：https://leetcode.cn/problems/minimum-operations-to-make-median-of-array-equal-to-k/solutions/2727218/pai-xu-kuai-su-xuan-ze-pythonjavacgo-by-23yt5/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[排序（附思考题）Python/Java/C++/Go](https://leetcode.cn/problems/minimum-operations-to-make-median-of-array-equal-to-k/solutions/2727218/pai-xu-kuai-su-xuan-ze-pythonjavacgo-by-23yt5/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`pai-xu-kuai-su-xuan-ze-pythonjavacgo-by-23yt5`
- topic id：`2727218`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:07:12 +0800

把 $\textit{nums}$ 从小到大排序后，中位数为 $\textit{nums}[m]$，其中 $m=\left\lfloor\dfrac{n}{2}\right\rfloor$，$n$ 为 $\textit{nums}$ 的长度。

我们需要把中位数左边的数都变成 $\le k$ 的，右边的数都变成 $\ge k$ 的。

分类讨论：

- 如果 $\textit{nums}[m] > k$，要把下标在 $[0,m]$ 中的大于 $k$ 的数都变成 $k$，由于下标在 $[m+1,n-1]$ 中的数已经大于 $k$（因为数组是有序的），所以下标在 $[m+1,n-1]$ 中的数无需操作。
- 如果 $\textit{nums}[m] < k$，要把下标在 $[m,n-1]$ 中的小于 $k$ 的数都变成 $k$，由于下标在 $[0,m-1]$ 中的数已经小于 $k$（因为数组是有序的），所以下标在 $[0,m-1]$ 中的数无需操作。

累加元素的变化量，即为答案。具体请看 [视频讲解](https://www.bilibili.com/video/BV1ut421H7Wv/) 第三题，欢迎点赞关注！

```py [sol-Python3]
class Solution:
    def minOperationsToMakeMedianK(self, nums: List[int], k: int) -> int:
        nums.sort()
        m = len(nums) // 2
        ans = 0
        if nums[m] > k:
            for i in range(m, -1, -1):
                if nums[i] <= k:
                    break
                ans += nums[i] - k
        else:
            for i in range(m, len(nums)):
                if nums[i] >= k:
                    break
                ans += k - nums[i]
        return ans
```

```java [sol-Java]
class Solution {
    public long minOperationsToMakeMedianK(int[] nums, int k) {
        Arrays.sort(nums);
        long ans = 0;
        int m = nums.length / 2;
        if (nums[m] > k) {
            for (int i = m; i >= 0 && nums[i] > k; i--) {
                ans += nums[i] - k;
            }
        } else {
            for (int i = m; i < nums.length && nums[i] < k; i++) {
                ans += k - nums[i];
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long minOperationsToMakeMedianK(vector<int> &nums, int k) {
        ranges::sort(nums);
        long long ans = 0;
        int m = nums.size() / 2;
        if (nums[m] > k) {
            for (int i = m; i >= 0 && nums[i] > k; i--) {
                ans += nums[i] - k;
            }
        } else {
            for (int i = m; i < nums.size() && nums[i] < k; i++) {
                ans += k - nums[i];
            }
        }
        return ans;
    }
};
```

```cpp [sol-C++ 快速选择]
class Solution {
public:
    long long minOperationsToMakeMedianK(vector<int> &nums, int k) {
        int m = nums.size() / 2;
        ranges::nth_element(nums, nums.begin() + m);
        long long ans = 0;
        if (nums[m] > k) {
            for (int i = 0; i <= m; i++) {
                ans += max(nums[i] - k, 0);
            }
        } else {
            for (int i = m; i < nums.size(); i++) {
                ans += max(k - nums[i], 0);
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func minOperationsToMakeMedianK(nums []int, k int) (ans int64) {
	slices.Sort(nums)
	m := len(nums) / 2
	if nums[m] > k {
		for i := m; i >= 0 && nums[i] > k; i-- {
			ans += int64(nums[i] - k)
		}
	} else {
		for i := m; i < len(nums) && nums[i] < k; i++ {
			ans += int64(k - nums[i])
		}
	}
	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n)$ 或 $\mathcal{O}(n)$，其中 $n$ 为 $\textit{nums}$ 的长度。用快速选择算法可以做到期望 $\mathcal{O}(n)$ 的时间复杂度。
- 空间复杂度：$\mathcal{O}(1)$。忽略排序的栈开销。

## 思考题

改成输入 $10^5$ 个询问，每个询问包含一个 $k$，如何高效地回答每个询问？

欢迎在评论区发表你的思路。

## 分类题单

1. [滑动窗口（定长/不定长/多指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（矩形系列/字典序最小/贡献法）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/贪心/脑筋急转弯）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)

更多题单，点我个人主页 - 讨论发布。

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

## 本地原创解析

### 1. 题意重述

本题来自 `四、数学贪心 / §4.7 其他数学贪心`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `四、数学贪心 / §4.7 其他数学贪心`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
