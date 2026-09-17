# 2366. 将数组排序的最少替换次数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-replacements-to-sort-the-array/
- 题目 slug：`minimum-replacements-to-sort-the-array`
- 来源专题：贪心与思维
- 来源分类路径：一、贪心策略 / §1.4 从最左/最右开始贪心
- 难度分：2060
- 外部题解来源：https://leetcode.cn/problems/minimum-replacements-to-sort-the-array/solutions/1728019/by-endlesscheng-d11y/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[贪心 + 详细证明（Python/Java/C++/Go）](https://leetcode.cn/problems/minimum-replacements-to-sort-the-array/solutions/1728019/by-endlesscheng-d11y/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`by-endlesscheng-d11y`
- topic id：`1728019`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:54:06 +0800

[视频讲解](https://www.bilibili.com/video/bv1gB4y1k7Jz) 已出炉，欢迎点赞三连，在评论区分享你对这场双周赛的看法~

--- 

#### 提示 1

每个数字要么不变，要么分成更小的数字。

#### 提示 2

最后一个数字需要操作吗？

不需要，如果操作，前面的数字就需要变得更小，这会让操作次数增多。

#### 提示 3

倒着遍历 $\textit{nums}$。设当前操作出的最小值为 $m$，如果 $\textit{nums}[i]>m$，那么需要拆分 $\textit{nums}[i]$，使得拆分出的数字的最大值不超过 $m$。

设拆分出了 $x$ 个数字，由于这 $x$ 个数字都不超过 $m$，即

$$
\textit{nums}[i] = v_1+v_2+\cdots+v_x \le m+m+\cdots+m = mx
$$ 
 
得 

$$
x\ge\left\lceil\dfrac{\textit{nums}[i]}{m}\right\rceil
$$

为了使操作次数尽量小，应取等号，即

$$
x=\left\lceil\dfrac{\textit{nums}[i]}{m}\right\rceil
$$

操作次数为 $k=x-1$。

为了使拆分出的数字的最小值尽可能地大，拆分出的最小数字应为 $\left\lfloor\dfrac{\textit{nums}[i]}{x}\right\rfloor$，证明如下：

> 若这 $x$ 个数均为 $\left\lfloor\dfrac{\textit{nums}[i]}{x}\right\rfloor$，那么有
> $$
> x\left\lfloor\dfrac{\textit{nums}[i]}{x}\right\rfloor\le \textit{nums}[i]
> $$
> 若这 $x$ 个数均为 $\left\lfloor\dfrac{\textit{nums}[i]}{x}\right\rfloor+1$，那么有
> $$
> x\left(\left\lfloor\dfrac{\textit{nums}[i]}{x}\right\rfloor+1\right)\ge x\left\lceil\dfrac{\textit{nums}[i]}{x}\right\rceil \ge \textit{nums}[i]
> $$
> 联合得到
> $$
> x\left\lfloor\dfrac{\textit{nums}[i]}{x}\right\rfloor\le \textit{nums}[i]\le x\left(\left\lfloor\dfrac{\textit{nums}[i]}{x}\right\rfloor+1\right)
> $$
> 据此，我们可以给出一个拆分方案：将这 $x$ 个数均初始化为 $\left\lfloor\dfrac{\textit{nums}[i]}{x}\right\rfloor$，然后给其中的 $\textit{nums}[i]-x\left\lfloor\dfrac{\textit{nums}[i]}{x}\right\rfloor$ 个数字加一，这样可以使这 $x$ 数的和恰好为 $\textit{nums}[i]$。上面的不等式说明这样的方案是存在的。

代码实现时，无需判断 $\textit{nums}[i]$ 与 $m$ 的大小关系。

#### 复杂度分析

- 时间复杂度：$O(n)$，其中 $n$ 为 $\textit{nums}$ 的长度。
- 空间复杂度：$O(1)$，仅用到若干额外变量。

```py [sol1-Python3]
class Solution:
    def minimumReplacement(self, nums: List[int]) -> int:
        ans, m = 0, nums[-1]
        for num in reversed(nums):
            k = (num - 1) // m
            ans += k
            m = num // (k + 1)
        return ans
```

```java [sol1-Java]
class Solution {
    public long minimumReplacement(int[] nums) {
        var ans = 0L;
        var m = nums[nums.length - 1];
        for (var i = nums.length - 2; i >= 0; --i) {
            var k = (nums[i] - 1) / m;
            ans += k;
            m = nums[i] / (k + 1);
        }
        return ans;
    }
}
```

```cpp [sol1-C++]
class Solution {
public:
    long long minimumReplacement(vector<int> &nums) {
        long ans = 0L;
        int m = nums.back();
        for (int i = int(nums.size()) - 2; i >= 0; --i) {
            int k = (nums[i] - 1) / m;
            ans += k;
            m = nums[i] / (k + 1);
        }
        return ans;
    }
};
```

```go [sol1-Go]
func minimumReplacement(nums []int) (ans int64) {
	m := nums[len(nums)-1]
	for i := len(nums) - 2; i >= 0; i-- {
		k := (nums[i] - 1) / m
		ans += int64(k)
		m = nums[i] / (k + 1)
	}
	return
}
```

#### 思考题

每个数字需要拆分成若干**整数**（目前题目没有说清楚这一点，读者可以通过测试 $[5,5,3]$ 这个输入来确认，预期结果为 $3$）。

如果可以拆分成实数，答案会是多少呢？比如 $[5,5,3]$ 可以拆分成 $[2.5,2.5,2.5,2.5,3]$，答案为 $2$。

如何在计算过程中避免使用浮点数？

## 本地原创解析

### 1. 题意重述

本题来自 `一、贪心策略 / §1.4 从最左/最右开始贪心`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、贪心策略 / §1.4 从最左/最右开始贪心`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
