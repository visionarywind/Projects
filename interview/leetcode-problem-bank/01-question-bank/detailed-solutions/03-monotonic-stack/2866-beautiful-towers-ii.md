# 2866. 美丽塔 II

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/beautiful-towers-ii/
- 题目 slug：`beautiful-towers-ii`
- 来源专题：单调栈
- 来源分类路径：一、单调栈 / §1.2 进阶
- 难度分：2072
- 外部题解来源：https://leetcode.cn/problems/beautiful-towers-ii/solutions/2456562/qian-hou-zhui-fen-jie-dan-diao-zhan-pyth-1exe/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[前后缀分解+单调栈（附题单！）Python/Java/C++/Go](https://leetcode.cn/problems/beautiful-towers-ii/solutions/2456562/qian-hou-zhui-fen-jie-dan-diao-zhan-pyth-1exe/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`qian-hou-zhui-fen-jie-dan-diao-zhan-pyth-1exe`
- topic id：`2456562`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

下面把 $\textit{maxHeights}$ 简记为 $a$。

- 计算从 $a[0]$ 到 $a[i]$ 形成山状数组的左侧递增段，元素和最大是多少，记到数组 $\textit{pre}[i]$ 中。
- 计算从 $a[i]$ 到 $a[n-1]$ 形成山状数组的右侧递减段，元素和最大是多少，记到数组 $\textit{suf}[i]$ 中。

那么答案就是 $\textit{pre}[i]+\textit{suf}[i+1]$ 的最大值。

如何计算 $\textit{pre}$ 和 $\textit{suf}$ 呢？

用**单调栈**，元素值从栈底到栈顶严格递增。

以 $\textit{suf}$ 为例，我们从右往左遍历 $a$，设当前得到的元素和为 $\textit{sum}$。

- 如果 $a[i]$ 大于栈顶的元素值，那么直接把 $a[i]$ 加到 $\textit{sum}$ 中，同时把 $i$ 入栈（栈中只需要保存下标）。
- 否则，只要 $a[i]$ 小于等于栈顶元素值，就不断循环，把之前加到 $\textit{sum}$ 的**撤销**掉。循环结束后，从 $a[i]$ 到 $a[j-1]$（假设现在栈顶下标是 $j$）都必须是 $a[i]$，把 $a[i]\cdot (j-i)$ 加到 $\textit{sum}$ 中。
- 具体例子请看 [视频讲解](https://www.bilibili.com/video/BV1yu4y1z7sE/) 第三题。

```py [sol-Python3]
class Solution:
    def maximumSumOfHeights(self, a: List[int]) -> int:
        n = len(a)
        suf = [0] * (n + 1)
        st = [n]  # 哨兵
        s = 0
        for i in range(n - 1, -1, -1):
            x = a[i]
            while len(st) > 1 and x <= a[st[-1]]:
                j = st.pop()
                s -= a[j] * (st[-1] - j)  # 撤销之前加到 s 中的
            s += x * (st[-1] - i)  # 从 i 到 st[-1]-1 都是 x
            suf[i] = s
            st.append(i)

        ans = s
        st = [-1]  # 哨兵
        pre = 0
        for i, x in enumerate(a):
            while len(st) > 1 and x <= a[st[-1]]:
                j = st.pop()
                pre -= a[j] * (j - st[-1])  # 撤销之前加到 pre 中的
            pre += x * (i - st[-1])  # 从 st[-1]+1 到 i 都是 x
            ans = max(ans, pre + suf[i + 1])
            st.append(i)
        return ans
```

```java [sol-Java]
class Solution {
    public long maximumSumOfHeights(List<Integer> maxHeights) {
        int[] a = maxHeights.stream().mapToInt(i -> i).toArray();
        int n = a.length;
        long[] suf = new long[n + 1];
        var st = new ArrayDeque<Integer>();
        st.push(n); // 哨兵
        long sum = 0;
        for (int i = n - 1; i >= 0; i--) {
            int x = a[i];
            while (st.size() > 1 && x <= a[st.peek()]) {
                int j = st.pop();
                sum -= (long) a[j] * (st.peek() - j); // 撤销之前加到 sum 中的
            }
            sum += (long) x * (st.peek() - i); // 从 i 到 st.peek()-1 都是 x
            suf[i] = sum;
            st.push(i);
        }

        long ans = sum;
        st.clear();
        st.push(-1); // 哨兵
        long pre = 0;
        for (int i = 0; i < n; i++) {
            int x = a[i];
            while (st.size() > 1 && x <= a[st.peek()]) {
                int j = st.pop();
                pre -= (long) a[j] * (j - st.peek()); // 撤销之前加到 pre 中的
            }
            pre += (long) x * (i - st.peek()); // 从 st.peek()+1 到 i 都是 x
            ans = Math.max(ans, pre + suf[i + 1]);
            st.push(i);
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long maximumSumOfHeights(vector<int> &a) {
        int n = a.size();
        vector<long long> suf(n + 1);
        stack<int> st;
        st.push(n); // 哨兵
        long long sum = 0;
        for (int i = n - 1; i >= 0; i--) {
            int x = a[i];
            while (st.size() > 1 && x <= a[st.top()]) {
                int j = st.top();
                st.pop();
                sum -= (long long) a[j] * (st.top() - j); // 撤销之前加到 sum 中的
            }
            sum += (long long) x * (st.top() - i); // 从 i 到 st.top()-1 都是 x
            suf[i] = sum;
            st.push(i);
        }

        long long ans = sum;
        st = stack<int>();
        st.push(-1); // 哨兵
        long long pre = 0;
        for (int i = 0; i < n; i++) {
            int x = a[i];
            while (st.size() > 1 && x <= a[st.top()]) {
                int j = st.top();
                st.pop();
                pre -= (long long) a[j] * (j - st.top()); // 撤销之前加到 pre 中的
            }
            pre += (long long) x * (i - st.top()); // 从 st.top()+1 到 i 都是 x
            ans = max(ans, pre + suf[i + 1]);
            st.push(i);
        }
        return ans;
    }
};
```

```go [sol-Go]
func maximumSumOfHeights(a []int) int64 {
	ans := 0
	n := len(a)
	suf := make([]int, n+1)
	st := []int{n} // 哨兵
	sum := 0
	for i := n - 1; i >= 0; i-- {
		x := a[i]
		for len(st) > 1 && x <= a[st[len(st)-1]] {
			j := st[len(st)-1]
			st = st[:len(st)-1]
			sum -= a[j] * (st[len(st)-1] - j) // 撤销之前加到 sum 中的
		}
		sum += x * (st[len(st)-1] - i) // 从 i 到 st[len(st)-1]-1 都是 x
		suf[i] = sum
		st = append(st, i)
	}
	ans = sum

	st = []int{-1} // 哨兵
	pre := 0
	for i, x := range a {
		for len(st) > 1 && x <= a[st[len(st)-1]] {
			j := st[len(st)-1]
			st = st[:len(st)-1]
			pre -= a[j] * (j - st[len(st)-1]) // 撤销之前加到 pre 中的
		}
		pre += x * (i - st[len(st)-1]) // 从 st[len(st)-1]+1 到 i 都是 x
		ans = max(ans, pre+suf[i+1])
		st = append(st, i)
	}
	return int64(ans)
}

func max(a, b int) int { if b > a { return b }; return a }
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{maxHeights}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 思考题

1. 改成先**严格递增**再**严格递减**，要怎么做？
2. 改成去掉一些数变成先递增再递减，至少要去掉多少个数？
   - 这题是 [1671. 得到山形数组的最少删除次数](https://leetcode.cn/problems/minimum-number-of-removals-to-make-mountain-array/)

## 练习：前后缀分解

- [42. 接雨水](https://leetcode.cn/problems/trapping-rain-water/)（[视频讲解](https://www.bilibili.com/video/BV1Qg411q7ia/?t=3m05s)）
- [238. 除自身以外数组的乘积](https://leetcode.cn/problems/product-of-array-except-self/)
- [2256. 最小平均差](https://leetcode.cn/problems/minimum-average-difference/)
- [2483. 商店的最少代价](https://leetcode.cn/problems/minimum-penalty-for-a-shop/)
- [2420. 找到所有好下标](https://leetcode.cn/problems/find-all-good-indices/)
- [2167. 移除所有载有违禁货物车厢所需的最少时间](https://leetcode.cn/problems/minimum-time-to-remove-all-cars-containing-illegal-goods/)
- [2484. 统计回文子序列数目](https://leetcode.cn/problems/count-palindromic-subsequences/)
- [2552. 统计上升四元组](https://leetcode.cn/problems/count-increasing-quadruplets/)
- [2565. 最少得分子序列](https://leetcode.cn/problems/subsequence-with-the-minimum-score/)

## 练习：单调栈

- [496. 下一个更大元素 I](https://leetcode.cn/problems/next-greater-element-i/)（单调栈模板题）
- [503. 下一个更大元素 II](https://leetcode.cn/problems/next-greater-element-ii/)
- [2454. 下一个更大元素 IV](https://leetcode.cn/problems/next-greater-element-iv/)
- [456. 132 模式](https://leetcode.cn/problems/132-pattern/)
- [739. 每日温度](https://leetcode.cn/problems/daily-temperatures/)
- [901. 股票价格跨度](https://leetcode.cn/problems/online-stock-span/)
- [1019. 链表中的下一个更大节点](https://leetcode.cn/problems/next-greater-node-in-linked-list/)
- [1124. 表现良好的最长时间段](https://leetcode.cn/problems/longest-well-performing-interval/)
- [1475. 商品折扣后的最终价格](https://leetcode.cn/problems/final-prices-with-a-special-discount-in-a-shop/)
- [2289. 使数组按非递减顺序排列](https://leetcode.cn/problems/steps-to-make-array-non-decreasing/)

#### 矩形系列

- [84. 柱状图中最大的矩形](https://leetcode.cn/problems/largest-rectangle-in-histogram/)
- [85. 最大矩形](https://leetcode.cn/problems/maximal-rectangle/)
- [1504. 统计全 1 子矩形](https://leetcode.cn/problems/count-submatrices-with-all-ones/)

#### 字典序最小

- [316. 去除重复字母](https://leetcode.cn/problems/remove-duplicate-letters/)
- [316 扩展：重复个数不超过 limit](https://leetcode.cn/contest/tianchi2022/problems/ev2bru/)
- [402. 移掉 K 位数字](https://leetcode.cn/problems/remove-k-digits/)
- [321. 拼接最大数](https://leetcode.cn/problems/create-maximum-number/)

#### 贡献法

- [907. 子数组的最小值之和](https://leetcode.cn/problems/sum-of-subarray-minimums/)
- [1856. 子数组最小乘积的最大值](https://leetcode.cn/problems/maximum-subarray-min-product/)
- [2104. 子数组范围和](https://leetcode.cn/problems/sum-of-subarray-ranges/)
- [2281. 巫师的总力量和](https://leetcode.cn/problems/sum-of-total-strength-of-wizards/)
- [2818. 操作使得分最大](https://leetcode.cn/problems/apply-operations-to-maximize-score/)

## 本地原创解析

### 1. 题意重述

本题来自 `一、单调栈 / §1.2 进阶`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、单调栈 / §1.2 进阶`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
