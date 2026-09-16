# 2470. 最小公倍数为 K 的子数组数目

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/number-of-subarrays-with-lcm-equal-to-k/
- 题目 slug：`number-of-subarrays-with-lcm-equal-to-k`
- 来源专题：数学算法
- 来源分类路径：一、数论 / §1.7 最小公倍数（LCM）
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/number-of-subarrays-with-lcm-equal-to-k/solutions/1965427/by-endlesscheng-3qnt/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种做法：暴力 / 利用 LCM 的性质](https://leetcode.cn/problems/number-of-subarrays-with-lcm-equal-to-k/solutions/1965427/by-endlesscheng-3qnt/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`by-endlesscheng-3qnt`
- topic id：`1965427`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

[视频讲解](https://www.bilibili.com/video/BV13841187gz/) 已出炉，欢迎点赞三连，在评论区分享你对这场周赛的看法~

---

思路和 [2447. 最大公因数等于 K 的子数组数目](https://leetcode.cn/problems/number-of-subarrays-with-gcd-equal-to-k/) 完全一致。

# 方法一：暴力枚举

```py [sol1-Python3]
class Solution:
    def subarrayLCM(self, nums: List[int], k: int) -> int:
        ans, n = 0, len(nums)
        for i in range(n):
            res = 1
            for j in range(i, n):
                res = lcm(res, nums[j])
                if k % res: break  # 剪枝：LCM 必须是 k 的因子
                if res == k: ans += 1
        return ans
```

```go [sol1-Go]
func subarrayLCM(nums []int, k int) (ans int) {
	for i := range nums {
		lcm := 1
		for _, x := range nums[i:] {
			lcm = lcm / gcd(lcm, x) * x
			if k%lcm > 0 { // 剪枝：lcm 必须是 k 的因子
				break
			}
			if lcm == k {
				ans++
			}
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

- 时间复杂度：$O(n(n+\log k))$，其中 $n$ 为 $\textit{nums}$ 的长度。LCM 倍增的次数是 $O(\log k)$ 次，因此内层循环的时间复杂度为 $O(n+\log k)$，所以总的时间复杂度为 $O(n(n+\log k))$。
- 空间复杂度：$O(1)$，仅用到若干变量。

# 方法二：利用 LCM 的性质

参考我之前的那篇 [题解](https://leetcode.cn/problems/number-of-subarrays-with-gcd-equal-to-k/solutions/1917454/by-endlesscheng-1f1r/)。

最小公倍数要么不变，要么至少 $\times 2$，因此在遍历 $\textit{nums}$ 的同时，维护最小公倍数集合（数组），这至多有 $O(\log k)$ 个。

注意最小公倍数必须是 $k$ 的因子。

```py [sol2-Python3]
class Solution:
    def subarrayLCM(self, nums: List[int], k: int) -> int:
        ans = 0
        a = []  # [LCM，相同 LCM 区间的右端点]
        i0 = -1
        for i, x in enumerate(nums):
            if k % x:  # 保证后续求的 LCM 都是 k 的因子
                a = []
                i0 = i
                continue
            a.append([x, i])
            # 原地去重，因为相同的 LCM 都相邻在一起
            j = 0
            for p in a:
                p[0] = lcm(p[0], x)
                if a[j][0] != p[0]:
                    j += 1
                    a[j] = p
                else:
                    a[j][1] = p[1]
            del a[j + 1:]
            if a[0][0] == k:
                ans += a[0][1] - i0
        return ans
```

```go [sol2-Go]
func subarrayLCM(nums []int, k int) (ans int) {
	type result struct{ lcm, i int }
	var a []result
	i0 := -1
	for i, x := range nums {
		if k%x > 0 {
			a = nil
			i0 = i
			continue
		}
		for j, p := range a {
			a[j].lcm = p.lcm / gcd(p.lcm, x) * x
		}
		a = append(a, result{x, i})
		j := 0
		for _, q := range a[1:] {
			if a[j].lcm != q.lcm {
				j++
				a[j] = q
			} else {
				a[j].i = q.i
			}
		}
		a = a[:j+1]
		if a[0].lcm == k {
			ans += a[0].i - i0
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

- 时间复杂度：$O(n\log k)$，其中 $n$ 为 $\textit{nums}$ 的长度。LCM 倍增的次数是 $O(\log k)$ 次，并且每次去重的时间复杂度也为 $O(\log k)$，因此时间复杂度为 $O(n\log k)$。
- 空间复杂度：$O(\log k)$。

## 本地原创解析

### 1. 题意重述

本题来自 `一、数论 / §1.7 最小公倍数（LCM）`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、数论 / §1.7 最小公倍数（LCM）`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
