# 2607. 使子数组元素和相等

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/make-k-subarray-sums-equal/
- 题目 slug：`make-k-subarray-sums-equal`
- 来源专题：贪心与思维
- 来源分类路径：四、数学贪心 / §4.5 中位数贪心
- 难度分：2071
- 外部题解来源：https://leetcode.cn/problems/make-k-subarray-sums-equal/solutions/2203591/zhuan-huan-zhong-wei-shu-tan-xin-pei-shu-4dut/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【转换】中位数贪心+裴蜀定理（Python/Java/C++/Go）](https://leetcode.cn/problems/make-k-subarray-sums-equal/solutions/2203591/zhuan-huan-zhong-wei-shu-tan-xin-pei-shu-4dut/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`zhuan-huan-zhong-wei-shu-tan-xin-pei-shu-4dut`
- topic id：`2203591`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:07:12 +0800

### 视频讲解

见[【双周赛 101】](https://www.bilibili.com/video/BV1Ga4y1M72A/)。

**视频还讲解了如何手算二元一次不定方程！欢迎点赞！**

---

为方便描述，将 $\textit{arr}$ 简记为 $a$。

### 提示 1

先来解决 $a$ 不是循环数组的情况。

根据题意，考虑从 $i$ 和 $i+1$ 开始的两个长为 $k$ 的子数组的和，如果要求这两个和相等，则有

$$
a[i]+a[i+1]+\cdots + a[i+k-1] = a[i+1]+a[i+2]+\cdots + a[i+k]
$$

化简得

$$
a[i] = a[i+k]
$$

换句话说：

- $a[0] = a[k] = a[2k] = \cdots$
- $a[1] = a[k+1] = a[2k+1] = \cdots$
- $a[2] = a[k+2] = a[2k+2] = \cdots$
- ……

### 提示 2

按照 $i\bmod k$ 的结果将 $a$ 分组，对每一组（记作 $b$），我们需要解决：

让数组 $b$ 的所有元素相等的最少运算次数。

根据**中位数贪心**，将 $b$ 的所有元素变为 $b$ 的中位数是最优的。

证明：设 $b$ 的长度为 $m$，设要将所有 $b[i]$ 变为 $x$。假设 $b$ 已经从小到大排序。首先，如果 $x$ 取在区间 $[b[0],b[m-1]]$ 之外，那么 $x$ 向区间方向移动可以使距离和变小；同时，如果 $x$ 取在区间 $[b[0],b[m-1]]$ 之内，无论如何移动 $x$，它到 $b[0]$ 和 $b[m-1]$ 的距离和都是一个定值 $b[m-1]-b[0]$，那么去掉 $b[0]$ 和 $b[m-1]$ 这两个最左最右的数，问题规模缩小。不断缩小问题规模，如果最后剩下 $1$ 个数，那么 $x$ 就取它；如果最后剩下 $2$ 个数，那么 $x$ 取这两个数之间的任意值都可以（包括这两个数）。因此 $x$ 可以取 $b[m/2]$。

### 提示 3

回到原问题。

比如 $n=6,k=4$，那么 $a[2]$ 循环后是 $a[8]$，和 $a[0]$ 在同一组，而 $a[1]$ 无论怎么循环都无法和 $a[0]$ 在同一组。（$(1+6n)\bmod 4 \ne 0$）

根据这个例子，可以猜想一个结论：

**一个循环数组如果既有周期 $n$，又有周期 $k$，则必然有周期 $\gcd(n,k)$。**

证明：根据 [裴蜀定理](https://oi-wiki.org/math/number-theory/bezouts/)，有

$$
a[i] = a[i+nx+ky] = a[i+\gcd(n,k)]
$$

这样就转换成了不是循环数组的情况。

> 注：代码中的排序可以换成快速选择，从而做到 $O(n)$ 的时间复杂度。具体见 C++ 代码。

```py [sol1-Python3]
class Solution:
    def makeSubKSumEqual(self, arr: List[int], k: int) -> int:
        k = gcd(k, len(arr))
        ans = 0
        for i in range(k):
            b = sorted(arr[i::k])
            mid = b[len(b) // 2]
            ans += sum(abs(x - mid) for x in b)
        return ans
```

```java [sol1-Java]
class Solution {
    public long makeSubKSumEqual(int[] arr, int k) {
        int n = arr.length;
        k = gcd(k, n);
        long ans = 0;
        for (int i = 0; i < k; ++i) {
            var b = new ArrayList<Integer>();
            for (int j = i; j < n; j += k)
                b.add(arr[j]);
            Collections.sort(b);
            int mid = b.get(b.size() / 2);
            for (int x : b)
                ans += Math.abs(x - mid);
        }
        return ans;
    }

    private int gcd(int a, int b) {
        while (a != 0) {
            int tmp = a;
            a = b % a;
            b = tmp;
        }
        return b;
    }
}
```

```cpp [sol1-C++]
class Solution {
public:
    long long makeSubKSumEqual(vector<int> &arr, int k) {
        int n = arr.size();
        k = gcd(k, n);
        vector<vector<int>> g(k);
        for (int i = 0; i < n; ++i)
            g[i % k].push_back(arr[i]);

        long long ans = 0;
        for (auto &b: g) {
            nth_element(b.begin(), b.begin() + b.size() / 2, b.end());
            for (int x: b)
                ans += abs(x - b[b.size() / 2]);
        }
        return ans;
    }
};
```

```go [sol1-Go]
func makeSubKSumEqual(arr []int, k int) (ans int64) {
	k = gcd(k, len(arr))
	g := make([][]int, k)
	for i, x := range arr {
		g[i%k] = append(g[i%k], x)
	}
	for _, b := range g {
		sort.Ints(b)
		for _, x := range b {
			ans += int64(abs(x - b[len(b)/2]))
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

func abs(x int) int {
	if x < 0 {
		return -x
	}
	return x
}
```

### 复杂度分析

- 时间复杂度：$O(n\log n)$ 或 $O(n)$，其中 $n$ 为 $\textit{arr}$ 的长度。采用快速选择找中位数可以做到 $O(n)$，见 C++ 代码。
- 空间复杂度：$O(n)$。

### 相似题目

- [462. 最小操作次数使数组元素相等 II](https://leetcode.cn/problems/minimum-moves-to-equal-array-elements-ii/)

## 本地原创解析

### 1. 题意重述

本题来自 `四、数学贪心 / §4.5 中位数贪心`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `四、数学贪心 / §4.5 中位数贪心`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
