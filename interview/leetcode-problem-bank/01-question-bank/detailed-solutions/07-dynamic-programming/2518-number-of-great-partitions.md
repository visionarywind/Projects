# 2518. 好分区的数目

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/number-of-great-partitions/
- 题目 slug：`number-of-great-partitions`
- 来源专题：动态规划
- 来源分类路径：三、背包 / §3.1 0-1 背包
- 难度分：2415
- 外部题解来源：https://leetcode.cn/problems/number-of-great-partitions/solutions/2032009/ni-xiang-si-wei-01-bei-bao-fang-an-shu-p-v47x/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[逆向思维 + 01 背包方案数（Python/Java/C++/Go）](https://leetcode.cn/problems/number-of-great-partitions/solutions/2032009/ni-xiang-si-wei-01-bei-bao-fang-an-shu-p-v47x/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`ni-xiang-si-wei-01-bei-bao-fang-an-shu-p-v47x`
- topic id：`2032009`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

[视频讲解](https://www.bilibili.com/video/BV1FV4y1F7v7/) 已出炉，欢迎点赞三连，在评论区分享你对这场周赛的看法~

---

如果直接计算好分区的数目，我们可以用 **01 背包**来做，但是背包容量太大，会超时。

**正难则反**，我们可以反过来，计算**坏分区**的数目，即第一个组或第二个组的元素和小于 $k$ 的方案数。根据对称性，我们只需要计算第一个组的元素和小于 $k$ 的方案数，然后乘 $2$ 即可。

> 注意，如果 $\textit{nums}$ 的所有元素之和小于 $2k$，则不存在好分区，我们可以特判这种情况，直接返回 $0$。如果不特判，计算坏分区会重复统计，导致错误的结果。

那么原问题就转换为「从 $\textit{nums}$ 中选择若干元素，使得元素和小于 $k$ 的方案数」，这样用 01 背包就不会超时了。

具体来说，定义 $f[i][j]$ 表示从前 $i$ 个数中选择若干元素，和为 $j$ 的方案数。

分类讨论：

- 不选第 $i$ 个数：$f[i][j] = f[i-1][j]$；
- 选第 $i$ 个数：$f[i][j] = f[i-1][j-\textit{nums}[i]]$。

因此 $f[i][j] = f[i-1][j] + f[i-1][j-\textit{nums}[i]]$。

初始值 $f[0][0] = 1$。

坏分区的数目 $\textit{bad} =(f[n][0]+f[n][1]+\cdots+f[n][k-1])\cdot 2$。

答案为所有分区的数目减去坏分区的数目，即 $2^n-\textit{bad}$，这里 $n$ 为 $\textit{nums}$ 的长度。

代码实现时，可以用倒序循环的技巧来压缩空间。

```py [sol1-Python3]
class Solution:
    def countPartitions(self, nums: List[int], k: int) -> int:
        if sum(nums) < k * 2: return 0
        MOD = 10 ** 9 + 7
        f = [0] * k
        f[0] = 1
        for x in nums:
            for j in range(k - 1, x - 1, -1):
                f[j] = (f[j] + f[j - x]) % MOD
        return (pow(2, len(nums), MOD) - sum(f) * 2) % MOD
```

```java [sol1-Java]
class Solution {
    private static final int MOD = (int) 1e9 + 7;

    public int countPartitions(int[] nums, int k) {
        var sum = 0L;
        for (var x : nums) sum += x;
        if (sum < k * 2) return 0;
        var ans = 1;
        var f = new int[k];
        f[0] = 1;
        for (var x : nums) {
            ans = ans * 2 % MOD;
            for (var j = k - 1; j >= x; --j)
                f[j] = (f[j] + f[j - x]) % MOD;
        }
        for (var x : f)
            ans = (ans - x * 2 % MOD + MOD) % MOD; // 保证答案非负
        return ans;
    }
}
```

```cpp [sol1-C++]
class Solution {
    const int MOD = 1e9 + 7;
public:
    int countPartitions(vector<int> &nums, int k) {
        if (accumulate(nums.begin(), nums.end(), 0L) < k * 2) return 0;
        int ans = 1, f[k]; memset(f, 0, sizeof(f));
        f[0] = 1;
        for (int x : nums) {
            ans = ans * 2 % MOD;
            for (int j = k - 1; j >= x; --j)
                f[j] = (f[j] + f[j - x]) % MOD;
        }
        for (int x : f)
            ans = (ans - x * 2 % MOD + MOD) % MOD; // 保证答案非负
        return ans;
    }
};
```

```go [sol1-Go]
func countPartitions(nums []int, k int) int {
	const mod int = 1e9 + 7
	sum := 0
	for _, x := range nums {
		sum += x
	}
	if sum < k*2 {
		return 0
	}
	ans := 1
	f := make([]int, k)
	f[0] = 1
	for _, x := range nums {
		ans = ans * 2 % mod
		for j := k - 1; j >= x; j-- {
			f[j] = (f[j] + f[j-x]) % mod
		}
	}
	for _, x := range f {
		ans -= x * 2
	}
	return (ans%mod + mod) % mod // 保证答案非负
}
```

#### 复杂度分析

- 时间复杂度：$O(nk)$，其中 $n$ 为 $\textit{nums}$ 的长度。
- 空间复杂度：$O(k)$。

#### 相似题目

- [494. 目标和](https://leetcode.cn/problems/target-sum/)

## 本地原创解析

### 1. 题意重述

本题来自 `三、背包 / §3.1 0-1 背包`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `三、背包 / §3.1 0-1 背包`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
