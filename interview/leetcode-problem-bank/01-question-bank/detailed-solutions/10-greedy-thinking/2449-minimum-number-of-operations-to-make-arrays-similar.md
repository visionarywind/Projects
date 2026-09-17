# 2449. 使数组相似的最少操作次数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-number-of-operations-to-make-arrays-similar/
- 题目 slug：`minimum-number-of-operations-to-make-arrays-similar`
- 来源专题：贪心与思维
- 来源分类路径：一、贪心策略 / §1.3 双序列配对
- 难度分：2076
- 外部题解来源：https://leetcode.cn/problems/minimum-number-of-operations-to-make-arrays-similar/solutions/1917413/by-endlesscheng-lusx/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[按奇偶性分类贪心（Python/Java/C++/Go）](https://leetcode.cn/problems/minimum-number-of-operations-to-make-arrays-similar/solutions/1917413/by-endlesscheng-lusx/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`by-endlesscheng-lusx`
- topic id：`1917413`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:46:10 +0800

[视频讲解](https://www.bilibili.com/video/BV1ne4y1e7nu) 已出炉，欢迎点赞三连，在评论区分享你对这场周赛的看法~

---

#### 提示 1

如果把问题中的 $+2$ 和 $-2$ 改成 $+1$ 和 $-1$，要怎么做？

例如 $\textit{nums}=[2,8]$，$\textit{target}=[4,6]$，那么应该让 $2$ 和 $4$ 一对，$8$ 和 $6$ 一对。如果让 $2$ 和 $6$ 一对，$8$ 和 $4$ 一对，是会让变化量的和变得更大的。

通过这种**邻项交换法**，我们可以证明，让最小的一对，次小的一对，第三小的一对，……，累加每对元素的差的绝对值，就得到了每个数的变化量的和的最小值。

#### 提示 2

回到原问题，$+2$ 和 $-2$ 会导致无法直接排序然后一一匹配，但注意到 $+2$ 和 $-2$ 并不会改变元素的**奇偶性**，因此我们可以把偶数分为一组，奇数分为一组，每组分别计算，这样就像提示 1 那样一一匹配了。

最后把变化量的和除以 $4$，即为答案。

#### 提示 3

代码实现时可以先奇数再偶数，然后奇数偶数内部再排序。

由于数组元素都是正数，可以先**把所有奇数变成相反数**，然后排序，奇偶就自动分开了。

```py [sol1-Python3]
def f(a: List[int]) -> None:
    for i, x in enumerate(a):
        if x % 2: a[i] = -x  # 由于元素都是正数，把奇数变成相反数，这样排序后奇偶就自动分开了
    a.sort()

class Solution:
    def makeSimilar(self, nums: List[int], target: List[int]) -> int:
        f(nums)
        f(target)
        return sum(abs(x - y) for x, y in zip(nums, target)) // 4
```

```java [sol1-Java]
class Solution {
    public long makeSimilar(int[] nums, int[] target) {
        f(nums);
        f(target);
        var ans = 0L;
        for (var i = 0; i < nums.length; ++i)
            ans += Math.abs(nums[i] - target[i]);
        return ans / 4;
    }

    private void f(int[] a) {
        // 由于元素都是正数，把奇数变成相反数，这样排序后奇偶就自动分开了
        for (var i = 0; i < a.length; ++i)
            if (a[i] % 2 != 0) a[i] = -a[i];
        Arrays.sort(a);
    }
}
```

```cpp [sol1-C++]
class Solution {
    void f(vector<int> &a) {
        for (int &x : a)
            if (x % 2) x = -x; // 由于元素都是正数，把奇数变成相反数，这样排序后奇偶就自动分开了
        sort(a.begin(), a.end());
    }

public:
    long long makeSimilar(vector<int> &nums, vector<int> &target) {
        f(nums);
        f(target);
        long long ans = 0L;
        for (int i = 0; i < nums.size(); ++i)
            ans += abs(nums[i] - target[i]);
        return ans / 4;
    }
};
```

```go [sol1-Go]
func f(a []int) {
	for i, x := range a {
		if x%2 > 0 {
			a[i] = -x // 由于元素都是正数，把奇数变成相反数，这样排序后奇偶就自动分开了
		}
	}
	sort.Ints(a)
}

func makeSimilar(nums, target []int) (ans int64) {
	f(nums)
	f(target)
	for i, x := range nums {
		ans += int64(abs(x - target[i]))
	}
	return ans / 4
}

func abs(x int) int { if x < 0 { return -x }; return x }
```

#### 复杂度分析

- 时间复杂度：$O(n\log n)$，其中 $n$ 为 $\textit{nums}$ 的长度。
- 空间复杂度：$O(1)$。忽略快排的栈开销。

## 本地原创解析

### 1. 题意重述

本题来自 `一、贪心策略 / §1.3 双序列配对`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、贪心策略 / §1.3 双序列配对`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
