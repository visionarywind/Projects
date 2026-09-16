# 2593. 标记所有元素后数组的分数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/find-score-of-an-array-after-marking-all-elements/
- 题目 slug：`find-score-of-an-array-after-marking-all-elements`
- 来源专题：滑动窗口与双指针
- 来源分类路径：六、分组循环
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/find-score-of-an-array-after-marking-all-elements/solutions/2177231/dai-zhao-xia-biao-qu-pai-xu-pythonjavacg-umuv/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种做法：从 O(nlogn) 到 O(n)（Python/Java/C++/Go）](https://leetcode.cn/problems/find-score-of-an-array-after-marking-all-elements/solutions/2177231/dai-zhao-xia-biao-qu-pai-xu-pythonjavacg-umuv/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`dai-zhao-xia-biao-qu-pai-xu-pythonjavacg-umuv`
- topic id：`2177231`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

### 本题视频讲解

见[【双周赛 100】](https://www.bilibili.com/video/BV1WM411H7UE/)。

# 方法一：带着下标去排序

技巧题，把 $\textit{nums}[i]$ 及其下标绑定后，按照元素值从小到大排序，元素值相同的按照下标排序。

然后按照题目模拟，用一个 $\textit{vis}$ 数组来实现标记。

也可以生成一个下标数组，对下标排序。具体见 Java 和 C++ 的实现。

```py [sol1-Python3]
class Solution:
    def findScore(self, nums: List[int]) -> int:
        ans = 0
        vis = [False] * (len(nums) + 2)  # 保证下标不越界
        for i, x in sorted(enumerate(nums, 1), key=lambda p: p[1]):
            if not vis[i]:
                vis[i - 1] = vis[i + 1] = True  # 标记相邻的两个元素
                ans += x
        return ans
```

```java [sol1-Java]
class Solution {
    public long findScore(int[] nums) {
        int n = nums.length;
        var ids = new Integer[n];
        for (int i = 0; i < n; ++i) ids[i] = i;
        Arrays.sort(ids, (i, j) -> nums[i] - nums[j]);

        long ans = 0;
        var vis = new boolean[n + 2]; // 保证下标不越界
        for (int i : ids)
            if (!vis[i + 1]) { // 避免 -1，偏移一位
                vis[i] = vis[i + 2] = true;
                ans += nums[i];
            }
        return ans;
    }
}
```

```cpp [sol1-C++]
class Solution {
public:
    long long findScore(vector<int> &nums) {
        int n = nums.size(), ids[n];
        iota(ids, ids + n, 0);
        stable_sort(ids, ids + n, [&](int i, int j) {
            return nums[i] < nums[j];
        });

        long long ans = 0;
        bool vis[n + 2]; // 保证下标不越界
        memset(vis, 0, sizeof(vis));
        for (int i : ids)
            if (!vis[i + 1]) { // 避免 -1，偏移一位
                vis[i] = vis[i + 2] = true;
                ans += nums[i];
            }
        return ans;
    }
};
```

```go [sol1-Go]
func findScore(nums []int) (ans int64) {
	type pair struct{ v, i int }
	a := make([]pair, len(nums))
	for i, x := range nums {
		a[i] = pair{x, i + 1} // +1 保证下面 for 循环下标不越界
	}
	sort.Slice(a, func(i, j int) bool {
		a, b := a[i], a[j]
		return a.v < b.v || a.v == b.v && a.i < b.i
	})
	vis := make([]bool, len(nums)+2) // 保证下标不越界
	for _, p := range a {
		if !vis[p.i] {
			vis[p.i-1] = true
			vis[p.i+1] = true // 标记相邻的两个元素
			ans += int64(p.v)
		}
	}
	return
}
```

### 复杂度分析

- 时间复杂度：$O(n\log n)$，其中 $n$ 为 $\textit{nums}$ 的长度。
- 空间复杂度：$O(n)$。

# 方法二：转换 + 分组循环

把 $\textit{nums}$ 视作由若干严格递减子段组成的数组。

例如示例 1 可以看成 $[2,1]+[3]+[4]+[5,2]$，示例 2 可以看成 $[2]+[3]+[5,1]+[3,2]$。

从左到右遍历 $\textit{nums}$，严格递减子段的最小值 $\textit{nums}[i]$ 一定可以选，因为它比 $\textit{nums}[i-1]$ 小，且不会超过 $\textit{nums}[i+1]$。如果等于 $\textit{nums}[i+1]$，由于我们是从左到右遍历的，下标也是最小的。

$\textit{nums}[i]$ 选了，那么这一段左侧的 $\textit{nums}[i-2],\textit{nums}[i-4],\cdots$ 就可以一起选了（因为不能选相邻的），且 $\textit{nums}[i+1]$ 不能选。

于是遍历 $\textit{nums}$ 就可以算出答案了。

```py [sol2-Python3]
class Solution:
    def findScore(self, nums: List[int]) -> int:
        ans = 0
        i, n = 0, len(nums)
        while i < n:
            i0 = i
            while i + 1 < n and nums[i] > nums[i + 1]:  # 找到下坡的坡底
                i += 1
            for j in range(i, i0 - 1, -2):  # 从坡底 i 到坡顶 i0，每隔一个累加
                ans += nums[j]
            i += 2  # i 选了 i+1 不能选
        return ans
```

```java [sol2-Java]
class Solution {
    public long findScore(int[] nums) {
        long ans = 0;
        for (int i = 0, n = nums.length; i < n; i += 2) { // i 选了 i+1 不能选
            int i0 = i;
            while (i + 1 < n && nums[i] > nums[i + 1]) // 找到下坡的坡底
                ++i;
            for (int j = i; j >= i0; j -= 2) // 从坡底 i 到坡顶 i0，每隔一个累加
                ans += nums[j];
        }
        return ans;
    }
}
```

```cpp [sol2-C++]
class Solution {
public:
    long long findScore(vector<int> &nums) {
        long long ans = 0;
        for (int i = 0, n = nums.size(); i < n; i += 2) { // i 选了 i+1 不能选
            int i0 = i;
            while (i + 1 < n && nums[i] > nums[i + 1]) // 找到下坡的坡底
                ++i;
            for (int j = i; j >= i0; j -= 2) // 从坡底 i 到坡顶 i0，每隔一个累加
                ans += nums[j];
        }
        return ans;
    }
};
```

```go [sol2-Go]
func findScore(nums []int) (ans int64) {
	for i, n := 0, len(nums); i < n; i += 2 { // i 选了 i+1 不能选
		i0 := i
		for i+1 < n && nums[i] > nums[i+1] { // 找到下坡的坡底
			i++
		}
		for j := i; j >= i0; j -= 2 { // 从坡底 i 到坡顶 i0，每隔一个累加
			ans += int64(nums[j])
		}
	}
	return
}
```

### 复杂度分析

- 时间复杂度：$O(n)$，其中 $n$ 为 $\textit{nums}$ 的长度。注意代码中的 $i$ 只增不减，所以整个二重循环是 $O(n)$ 的。
- 空间复杂度：$O(1)$。仅用到若干额外变量。

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
