# 3012. 通过操作使数组长度最小

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimize-length-of-array-using-operations/
- 题目 slug：`minimize-length-of-array-using-operations`
- 来源专题：贪心与思维
- 来源分类路径：五、思维题 / §5.1 从特殊到一般
- 难度分：1833
- 外部题解来源：https://leetcode.cn/problems/minimize-length-of-array-using-operations/solutions/2613059/on-nao-jin-ji-zhuan-wan-pythonjavacgo-by-2lea/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[O(n) 脑筋急转弯（Python/Java/C++/Go）](https://leetcode.cn/problems/minimize-length-of-array-using-operations/solutions/2613059/on-nao-jin-ji-zhuan-wan-pythonjavacgo-by-2lea/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`on-nao-jin-ji-zhuan-wan-pythonjavacgo-by-2lea`
- topic id：`2613059`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:07:12 +0800

[视频讲解](https://www.bilibili.com/video/BV1oV411D7gB/) 第三题。

考虑这个例子：$\textit{nums}=[2,3,4,5,6]$，每次操作都可以选择 $2$ 和另一个数字 $x$，由于 $x>2$，所以 $2\bmod x = 2$，于是操作等价于：

- 移除 $x$。

所以最后必定只会剩下 $2$。

所以，如果数组中的最小值只有一个，我们可以操作成只剩下一个数，返回 $1$。

但如果最小值不止一个呢？如果能构造出一个小于 $m = \min(\textit{nums})$ 的正整数，那么也可以返回 $1$。

**结论**：当且仅当 $\textit{nums}$ 中有不是 $m$ 的倍数的数，我们才能构造出一个小于 $m$ 的正整数。

**证明**：如果有不是 $m$ 的倍数的数 $x$，那么 $0 < x\bmod m < m$，构造成功。如果所有数都是 $m$ 的倍数，那么任意两个数的模都是 $m$ 的倍数，我们无法得到一个在 $[1,m-1]$ 内的数。

如果所有数都是 $m$ 的倍数，我们可以先用 $m$ 把大于 $m$ 的数都移除，然后剩下的 $\textit{cnt}$ 个 $m$ 两两一对消除，最后剩下 $\left\lceil\dfrac{\textit{cnt}}{2}\right\rceil$ 个数。

```py [sol-Python3]
class Solution:
    def minimumArrayLength(self, nums: List[int]) -> int:
        m = min(nums)
        for x in nums:
            if x % m:
                return 1
        return (nums.count(m) + 1) // 2
```

```java [sol-Java]
class Solution {
    public int minimumArrayLength(int[] nums) {
        int m = Integer.MAX_VALUE;
        for (int x : nums) {
            m = Math.min(m, x);
        }

        for (int x : nums) {
            if (x % m > 0) {
                return 1;
            }
        }

        int cnt = 0;
        for (int x : nums) {
            if (x == m) {
                cnt++;
            }
        }
        return (cnt + 1) / 2;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minimumArrayLength(vector<int> &nums) {
        int m = ranges::min(nums);
        for (int x : nums) {
            if (x % m) {
                return 1;
            }
        }
        return (ranges::count(nums, m) + 1) / 2;
    }
};
```

```go [sol-Go]
func minimumArrayLength(nums []int) int {
	m := slices.Min(nums)
	for _, x := range nums {
		if x%m > 0 {
			return 1
		}
	}
	cnt := 0
	for _, x := range nums {
		if x == m {
			cnt++
		}
	}
	return (cnt + 1) / 2
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

[2023 下半年周赛题目总结](https://leetcode.cn/circle/discuss/lUu0KB/)

## 本地原创解析

### 1. 题意重述

本题来自 `五、思维题 / §5.1 从特殊到一般`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `五、思维题 / §5.1 从特殊到一般`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
