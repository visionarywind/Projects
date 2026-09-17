# 2811. 判断是否能拆分数组

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/check-if-it-is-possible-to-split-array/
- 题目 slug：`check-if-it-is-possible-to-split-array`
- 来源专题：贪心与思维
- 来源分类路径：五、思维题 / §5.2 脑筋急转弯
- 难度分：1543
- 外部题解来源：https://leetcode.cn/problems/check-if-it-is-possible-to-split-array/solutions/2375178/nao-jin-ji-zhuan-wan-by-endlesscheng-0l19/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[脑筋急转弯（Python/Java/C++/Go）](https://leetcode.cn/problems/check-if-it-is-possible-to-split-array/solutions/2375178/nao-jin-ji-zhuan-wan-by-endlesscheng-0l19/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`nao-jin-ji-zhuan-wan-by-endlesscheng-0l19`
- topic id：`2375178`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:07:12 +0800

请看 [视频讲解](https://www.bilibili.com/video/BV1Yr4y1o7aP/) 第二题。

先特判 $n\le 2$ 的情况，这是满足要求的。

对于 $n\ge 3$ 的情况，无论按照何种方式分割，一定会在某个时刻，分割出一个长为 $2$ 的子数组。

如果 $\textit{nums}$ 中任何长为 $2$ 的子数组的元素和都小于 $m$，那么无法满足要求。

否则，可以用这个子数组作为「核心」，像剥洋葱一样，**一个一个地**去掉 $\textit{nums}$ 的首尾元素，最后得到这个子数组。由于子数组的元素和 $\ge m$，所以每次分割出一个元素时，剩余的子数组的元素和也必然是 $\ge m$ 的，满足要求。

所以问题变成：判断数组中是否有两个相邻数字 $\ge m$。

```py [sol-Python3]
class Solution:
    def canSplitArray(self, nums: List[int], m: int) -> bool:
        return len(nums) <= 2 or any(x + y >= m for x, y in pairwise(nums))
```

```java [sol-Java]
class Solution {
    public boolean canSplitArray(List<Integer> nums, int m) {
        int n = nums.size();
        if (n <= 2) return true;
        for (int i = 1; i < n; i++)
            if (nums.get(i - 1) + nums.get(i) >= m)
                return true;
        return false;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    bool canSplitArray(vector<int> &nums, int m) {
        int n = nums.size();
        if (n <= 2) return true;
        for (int i = 1; i < n; i++)
            if (nums[i - 1] + nums[i] >= m)
                return true;
        return false;
    }
};
```

```go [sol-Go]
func canSplitArray(nums []int, m int) bool {
	n := len(nums)
	if n <= 2 {
		return true
	}
	for i := 1; i < n; i++ {
		if nums[i-1]+nums[i] >= m {
			return true
		}
	}
	return false
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。仅用到若干额外变量。

## 本地原创解析

### 1. 题意重述

本题来自 `五、思维题 / §5.2 脑筋急转弯`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `五、思维题 / §5.2 脑筋急转弯`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
