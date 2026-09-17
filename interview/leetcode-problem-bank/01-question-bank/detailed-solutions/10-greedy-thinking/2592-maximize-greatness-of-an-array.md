# 2592. 最大化数组的伟大值

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximize-greatness-of-an-array/
- 题目 slug：`maximize-greatness-of-an-array`
- 来源专题：贪心与思维
- 来源分类路径：一、贪心策略 / §1.2 单序列配对
- 难度分：1569
- 外部题解来源：https://leetcode.cn/problems/maximize-greatness-of-an-array/solutions/2177186/tian-ji-sai-ma-by-endlesscheng-buk1/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[田忌赛马，从 O(nlogn) 到 O(n)](https://leetcode.cn/problems/maximize-greatness-of-an-array/solutions/2177186/tian-ji-sai-ma-by-endlesscheng-buk1/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tian-ji-sai-ma-by-endlesscheng-buk1`
- topic id：`2177186`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:46:10 +0800

### 本题视频讲解

见[【双周赛 100】](https://www.bilibili.com/video/BV1WM411H7UE/)。

# 方法一：贪心 + 排序 + 双指针

### 提示 1

田忌赛马。

### 提示 2-1

想一想，$\textit{nums}$ 的最小值是否要参与贡献伟大值？要和谁匹配？

### 提示 2-2

$\textit{nums}$ 的最小值要参与匹配，否则更大的数字更难匹配上。

$\textit{nums}$ 的最小值要与次小值匹配，这样后面的数字才能取匹配更大的数。

### 提示 3

为了方便实现，对 $\textit{nums}$ 从小到大排序。（为什么可以排序？因为只在乎匹配关系，与下标无关。）

例如示例 1 排序后为 $[1,1,1,2,3,3,5]$。那么前三个 $1$ 分别与 $2,3,3$ 匹配，$2$ 与 $5$ 匹配，后面就没有数字能匹配了。

```py [sol1-Python3]
class Solution:
    def maximizeGreatness(self, nums: List[int]) -> int:
        nums.sort()
        i = 0
        for x in nums:
            if x > nums[i]:
                i += 1
        return i
```

```java [sol1-Java]
class Solution {
    public int maximizeGreatness(int[] nums) {
        Arrays.sort(nums);
        int i = 0;
        for (int x : nums)
            if (x > nums[i])
                ++i;
        return i;
    }
}
```

```cpp [sol1-C++]
class Solution {
public:
    int maximizeGreatness(vector<int> &nums) {
        sort(nums.begin(), nums.end());
        int i = 0;
        for (int x : nums)
            if (x > nums[i])
                ++i;
        return i;
    }
};
```

```go [sol1-Go]
func maximizeGreatness(nums []int) (i int) {
	sort.Ints(nums)
	for _, x := range nums {
		if x > nums[i] {
			i++
		}
	}
	return
}
```

### 复杂度分析

- 时间复杂度：$O(n\log n)$，其中 $n$ 为 $\textit{nums}$ 的长度。
- 空间复杂度：$O(1)$。忽略排序时的栈开销，仅用到若干额外变量。

# 方法二：利用两个指针的距离

考虑无法匹配的个数 $m$，答案为 $\textit{nums}$ 的长度减去 $m$。

再来看方法一的双指针，设另一个指针为 $j$，即 $x=\textit{nums}[j]$。

每次 `x > nums[i]` **不成立**时，两个指针错开的距离 $j-i$ 就增加 $1$。

那么循环结束后，两个指针最终错开的距离 $j-i$，就是 $m$。

以 $[1,1,2,2,2,2,3,3]$ 为例，初始 $i=0,j=0$：

- $j=0$，无法匹配，$j$ 加一。
- $j=1$，无法匹配，$j$ 加一。
- $j=2$，可以匹配，$i,j$ 都加一。
- $j=3$，可以匹配，$i,j$ 都加一。
- $j=4$，无法匹配，$j$ 加一。注意此时 $i$ 指向第一个 $2$。
- $j=5$，无法匹配，$j$ 加一。
- $j=6$，可以匹配，$i,j$ 都加一。注意此时 $j$ 指向最后一个 $2$ 的右侧相邻元素。
- $j=7$，可以匹配，$i,j$ 都加一。循环结束。

可以发现，当 $i$ 指向出现次数最多的数 $p$ 的时候，$j$ 要一直移动到下一个不等于 $p$ 的数，此时错开的距离是最大的。

由于后面的数出现次数不会超过 $p$，所以不会出现无法匹配的情况。（可以用反证法证明，如果出现了，说明这个数的出现次数大于 $p$，矛盾。）

所以 $m$ 就是 $\textit{nums}$ 中元素出现次数的最大值。用哈希表统计即可，无需排序。

```py [sol2-Python3]
class Solution:
    def maximizeGreatness(self, nums: List[int]) -> int:
        return len(nums) - max(Counter(nums).values())
```

```java [sol2-Java]
class Solution {
    public int maximizeGreatness(int[] nums) {
        int mx = 0;
        var cnt = new HashMap<Integer, Integer>();
        for (int x : nums)
            mx = Math.max(mx, cnt.merge(x, 1, Integer::sum));
        return nums.length - mx;
    }
}
```

```cpp [sol2-C++]
class Solution {
public:
    int maximizeGreatness(vector<int> &nums) {
        int mx = 0;
        unordered_map<int, int> cnt;
        for (int x : nums)
            mx = max(mx, ++cnt[x]);
        return nums.size() - mx;
    }
};
```

```go [sol2-Go]
func maximizeGreatness(nums []int) int {
	maxCnt := 0
	cnt := map[int]int{}
	for _, v := range nums {
		cnt[v]++
		maxCnt = max(maxCnt, cnt[v])
	}
	return len(nums) - maxCnt
}

func max(a, b int) int { if a < b { return b }; return a }
```

### 复杂度分析

- 时间复杂度：$O(n)$，其中 $n$ 为 $\textit{nums}$ 的长度。
- 空间复杂度：$O(n)$。

## 本地原创解析

### 1. 题意重述

本题来自 `一、贪心策略 / §1.2 单序列配对`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、贪心策略 / §1.2 单序列配对`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
