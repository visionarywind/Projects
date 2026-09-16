# 2344. 使数组可以被整除的最少删除次数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-deletions-to-make-array-divisible/
- 题目 slug：`minimum-deletions-to-make-array-divisible`
- 来源专题：数学算法
- 来源分类路径：一、数论 / §1.6 最大公约数（GCD）
- 难度分：1641
- 外部题解来源：https://leetcode.cn/problems/minimum-deletions-to-make-array-divisible/solutions/1676167/zhuan-huan-tan-xin-pythonjavacgo-by-endl-vdgw/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[转换 + 贪心（Python/Java/C++/Go）](https://leetcode.cn/problems/minimum-deletions-to-make-array-divisible/solutions/1676167/zhuan-huan-tan-xin-pythonjavacgo-by-endl-vdgw/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`zhuan-huan-tan-xin-pythonjavacgo-by-endl-vdgw`
- topic id：`1676167`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

本题 [视频讲解](https://www.bilibili.com/video/BV1GV4y1J7kc) 已出炉，欢迎点赞三连~

---

#### 提示 1

元素 $x$ 若能整除 $\textit{numsDivide}$ 的所有元素，等价于 $x$ 是所有 $\textit{numsDivide}[i]$ 的因子，这也等价于 $x$ 是 $\textit{numsDivide}$ 所有元素的最大公因数 $g$ 的因子。

#### 提示 2

由于要求用 $\textit{nums}$ 的最小元素去整除 $g$，不妨将 $\textit{nums}$ 排序后，从小到大找到第一个能整除 $g$ 的元素 $x$，所有小于 $x$ 的元素都需要删除。

```py [sol1-Python3]
class Solution:
    def minOperations(self, nums: List[int], numsDivide: List[int]) -> int:
        g = gcd(*numsDivide)
        nums.sort()
        return next((i for i, x in enumerate(nums) if g % x == 0), -1)
```

```java [sol1-Java]
class Solution {
    public int minOperations(int[] nums, int[] numsDivide) {
        var g = 0;
        for (var x : numsDivide) g = gcd(g, x);
        Arrays.sort(nums);
        for (var i = 0; i < nums.length; i++) if (g % nums[i] == 0) return i;
        return -1;
    }

    int gcd(int a, int b) {
        return a == 0 ? b : gcd(b % a, a);
    }
}
```

```cpp [sol1-C++]
class Solution {
public:
    int minOperations(vector<int> &nums, vector<int> &numsDivide) {
        int g = 0;
        for (int x : numsDivide) g = gcd(g, x);
        sort(nums.begin(), nums.end());
        for (int i = 0; i < nums.size(); i++) if (g % nums[i] == 0) return i;
        return -1;
    }
};
```

```go [sol1-Go]
func minOperations(nums, numsDivide []int) int {
	g := 0
	for _, x := range numsDivide {
		g = gcd(g, x)
	}
	sort.Ints(nums)
	for i, x := range nums {
		if g%x == 0 {
			return i
		}
	}
	return -1
}

func gcd(a, b int) int {
	for a != 0 {
		a, b = b%a, a
	}
	return b
}
```

也可以不用排序，通过两次遍历得到答案。

```py [sol2-Python3]
class Solution:
    def minOperations(self, nums: List[int], numsDivide: List[int]) -> int:
        g = gcd(*numsDivide)
        mn = min((x for x in nums if g % x == 0), default=0)
        return sum(x < mn for x in nums) if mn else -1
```

```java [sol2-Java]
class Solution {
    public int minOperations(int[] nums, int[] numsDivide) {
        var g = 0;
        for (var x : numsDivide) g = gcd(g, x);
        var min = Integer.MAX_VALUE;
        for (var num : nums) if (g % num == 0) min = Math.min(min, num);
        if (min == Integer.MAX_VALUE) return -1;
        var ans = 0;
        for (var x : nums) if (x < min) ++ans;
        return ans;
    }

    int gcd(int a, int b) {
        return a == 0 ? b : gcd(b % a, a);
    }
}
```

```cpp [sol2-C++]
class Solution {
public:
    int minOperations(vector<int> &nums, vector<int> &numsDivide) {
        int g = 0;
        for (int x : numsDivide) g = gcd(g, x);
        int mn = INT_MAX;
        for (int x : nums) if (g % x == 0) mn = min(mn, x);
        if (mn == INT_MAX) return -1;
        int ans = 0;
        for (int x : nums) if (x < mn) ++ans;
        return ans;
    }
};
```

```go [sol2-Go]
func minOperations(nums, numsDivide []int) (ans int) {
	g := 0
	for _, x := range numsDivide {
		g = gcd(g, x)
	}
	min := math.MaxInt32
	for _, x := range nums {
		if g%x == 0 && x < min {
			min = x
		}
	}
	if min == math.MaxInt32 {
		return -1
	}
	for _, x := range nums {
		if x < min {
			ans++
		}
	}
	return
}

func gcd(a, b int) int {
	for a != 0 {
		a, b = b%a, a
	}
	return b
}
```

#### 复杂度分析

- 时间复杂度：$O(m+\log U + n)$，其中 $m$ 为数组 $\textit{numsDivide}$ 的长度，$U=\max(\textit{numsDivide})$，$n$ 为数组 $\textit{nums}$ 的长度。注意到求最大公因数 $g$ 的过程（设初始 $g=U$），要么使 $g$ 不变，要么使 $g$ 至少减半，而 $g$ 至多减半 $O(\log U)$ 次，因此求最大公因数的迭代次数为 $O(m+\log U)$ 次。总的时间复杂度为 $O(m+\log U + n)$。
- 空间复杂度：$O(1)$。仅需要几个额外的变量。

## 本地原创解析

### 1. 题意重述

本题来自 `一、数论 / §1.6 最大公约数（GCD）`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、数论 / §1.6 最大公约数（GCD）`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
