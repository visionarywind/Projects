# 3649. 完美对的数目

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/number-of-perfect-pairs/
- 题目 slug：`number-of-perfect-pairs`
- 来源专题：滑动窗口与双指针
- 来源分类路径：三、单序列双指针 / §3.3 同向双指针
- 难度分：1716
- 外部题解来源：https://leetcode.cn/problems/number-of-perfect-pairs/solutions/3755121/yong-jue-dui-zhi-de-ji-he-yi-yi-hua-jian-xois/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[用绝对值的几何意义化简式子（Python/Java/C++/Go）](https://leetcode.cn/problems/number-of-perfect-pairs/solutions/3755121/yong-jue-dui-zhi-de-ji-he-yi-yi-hua-jian-xois/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`yong-jue-dui-zhi-de-ji-he-yi-yi-hua-jian-xois`
- topic id：`3755121`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

设 $a=\textit{nums}[i]$，$b=\textit{nums}[j]$。

把 $a$、$b$ 和 $-b$ 画在一维数轴上，分别用点 $A$、点 $B$ 和点 $B'$ 表示，其中 $B'$ 是 $B$ 关于原点 $O$ 的对称点。

- $|a-b|$ 表示 $A$ 到 $B$ 的距离，记作 $AB$。**注**：通常来说要记作 $|AB|$，这里为方便阅读简记为 $AB$。
- $|a+b| = |a - (-b)|$ 表示 $A$ 到 $B'$ 的距离 $AB'$。

## 题目的第二个式子

$\max(|a - b|, |a + b|) = \max(AB, AB')$。如果 $A$ 和 $B$ 在原点同一侧（同号），那么 $AB' = OA+OB'=OA+OB$ 更大；如果 $A$ 和 $B$ 在原点两侧（异号），那么 $AB = OA+OB$ 更大。所以 $\max(AB,AB') = OA + OB$。

对于两个非负数来说，两数之和大于两数最大值，所以 $OA +OB\ge \max(OA,OB) = \max(|a|, |b|)$。

总结：

$$
\max(|a - b|, |a + b|) = \max(AB,AB') = OA + OB \ge \max(OA,OB) = \max(|a|, |b|)
$$

所以题目的第二个式子**恒成立**。

## 题目的第一个式子

$\min(|a - b|, |a + b|) = \min(AB, AB')$。如果 $A$ 和 $B$ 在原点同一侧（同号），那么 $AB = |OA-OB|$ 更小；如果 $A$ 和 $B$ 在原点两侧（异号），那么 $AB' = |OA-OB'| = |OA-OB|$ 更小。所以 $\min(AB,AB') = |OA-OB|$。

$\min(|a|, |b|) = \min(OA,OB)$。

题目的第一个式子等价于

$$
|OA-OB|\le \min(OA,OB)
$$

- 如果 $OA\le OB$，那么上式化简为 $OB - OA \le OA$，即 $OB\le 2\cdot OA$。
- 如果 $OB\le OA$，那么上式化简为 $OA - OB \le OB$，即 $OA\le 2\cdot OB$。

相当于绝对值大的那个数，不能超过绝对值小的那个数乘以 $2$。如果 $|a|\le |b|$，那么需要满足 $|b|\le 2|a|$。

## 算法

本质上来说，我们需要从数组中选两个数，满足大的那个数的绝对值，不能超过小的那个数的绝对值乘以 $2$。**这与元素位置无关，所以可以排序**。比如 $\textit{nums}=[1,2]$ 还是 $[2,1]$，算出来的答案是一样的。

设绝对值小的数为 $a$，绝对值大的数为 $b$。

枚举 $b$，问题变成：

- 统计有多少个 $a$ 满足 $|a|\le |b|\le 2|a|$。

为方便计算，**按照绝对值从小到大排序**，那么数组中满足 $|a|\le |b|$ 的 $a$ 都在 $b$ 的左边，我们只需关心 $|b|\le 2|a|$。

随着 $|b|$ 的变大，满足 $|b|\le 2|a|$ 的最小的 $|a|$ 也会变大，所以可以用**同向双指针**解决。你也可以用滑动窗口理解，原理讲解见 [滑动窗口【基础算法精讲 03】](https://www.bilibili.com/video/BV1hd4y1r7Gq/)。

具体请看 [视频讲解](https://www.bilibili.com/video/BV191YCzjEvc/?t=2m47s)，欢迎点赞关注~

```py [sol-Python3]
class Solution:
    def perfectPairs(self, nums: List[int]) -> int:
        nums.sort(key=abs)
        ans = left = 0
        for j, b in enumerate(nums):
            while abs(nums[left]) * 2 < abs(b):
                left += 1
            # a=nums[i]，其中 i 最小是 left，最大是 j-1，一共有 j-left 个
            ans += j - left
        return ans
```

```py [sol-Python3 写法二]
class Solution:
    def perfectPairs(self, nums: List[int]) -> int:
        for i, x in enumerate(nums):
            nums[i] = abs(x)

        nums.sort()
        ans = left = 0
        for j, b in enumerate(nums):
            while nums[left] * 2 < b:
                left += 1
            # a=nums[i]，其中 i 最小是 left，最大是 j-1，一共有 j-left 个
            ans += j - left
        return ans
```

```java [sol-Java]
class Solution {
    public long perfectPairs(int[] nums) {
        for (int i = 0; i < nums.length; i++) {
            // nums[i] = Math.abs(nums[i]);
            if (nums[i] < 0) {
                nums[i] *= -1;
            }
        }

        Arrays.sort(nums);
        long ans = 0;
        int left = 0;
        for (int j = 0; j < nums.length; j++) {
            int b = nums[j];
            while (nums[left] * 2 < b) {
                left++;
            }
            // a=nums[i]，其中 i 最小是 left，最大是 j-1，一共有 j-left 个
            ans += j - left;
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long perfectPairs(vector<int>& nums) {
        for (int& x : nums) {
            x = abs(x);
        }

        ranges::sort(nums);
        long long ans = 0;
        int left = 0;
        for (int j = 0; j < nums.size(); j++) {
            int b = nums[j];
            while (nums[left] * 2 < b) {
                left++;
            }
            // a=nums[i]，其中 i 最小是 left，最大是 j-1，一共有 j-left 个
            ans += j - left;
        }
        return ans;
    }
};
```

```go [sol-Go]
func perfectPairs(nums []int) (ans int64) {
	for i, x := range nums {
		if x < 0 {
			nums[i] *= -1
		}
	}

	slices.Sort(nums)
	left := 0
	for j, b := range nums {
		for nums[left]*2 < b {
			left++
		}
		// a=nums[i]，其中 i 最小是 left，最大是 j-1，一共有 j-left 个
		ans += int64(j - left)
	}
	return
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
