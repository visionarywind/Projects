# 3113. 边界元素是最大值的子数组数目

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/find-the-number-of-subarrays-where-boundary-elements-are-maximum/
- 题目 slug：`find-the-number-of-subarrays-where-boundary-elements-are-maximum`
- 来源专题：单调栈
- 来源分类路径：一、单调栈 / §1.2 进阶
- 难度分：2046
- 外部题解来源：https://leetcode.cn/problems/find-the-number-of-subarrays-where-boundary-elements-are-maximum/solutions/2738894/on-dan-diao-zhan-pythonjavacgo-by-endles-y00d/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[O(n) 单调栈（Python/Java/C++/Go）](https://leetcode.cn/problems/find-the-number-of-subarrays-where-boundary-elements-are-maximum/solutions/2738894/on-dan-diao-zhan-pythonjavacgo-by-endles-y00d/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`on-dan-diao-zhan-pythonjavacgo-by-endles-y00d`
- topic id：`2738894`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

「第一个和最后一个元素都是子数组中的最大值」意味着子数组的**首尾元素必须相等**。

例如 $\textit{nums}=[4,3,1,2,1]$，在从左到右遍历的过程中，由于 $2$ 的出现，左边的 $1$ 永远不可能与右边的 $1$ 组成一个题目要求的子数组。所以当遍历到 $2$ 时，左边的 $1$ 就是**无用数据**了，可以清除。清除后我们会得到一个**从左到右递减**的数据结构。

这个性质和 [单调栈【基础算法精讲 26】](https://www.bilibili.com/video/BV1VN411J7S7/)很像，启发我们用单调栈思考。具体来说：

1. 初始化答案等于 $n$，因为每个元素可以单独组成一个长为 $1$ 的子数组，满足题目要求。
2. 维护一个底大顶小的单调栈，记录元素及其出现次数。
3. 从左到右遍历 $\textit{nums}$。
4. 只要 $x=\textit{nums}[i]$ 大于栈顶，就把栈顶出栈。
5. 如果 $x$ 小于栈顶，把 $x$ 及其出现次数 $1$ 入栈。
6. 如果 $x$ 等于栈顶，设栈顶记录的出现次数为 $\textit{cnt}$，那么 $x$ 可以和左边 $\textit{cnt}$ 个 $x$ 组成 $\textit{cnt}$ 个满足要求的子数组，把答案增加 $\textit{cnt}$，然后把 $\textit{cnt}$ 加一。

注意可能出现某个元素 $v$ 出栈后，又重新入栈的情况，此时 $v$ 的出现次数会重置成 $1$。

代码实现时，可以往栈底加入一个 $\infty$ 哨兵，从而简化判断逻辑。

请看 [视频讲解](https://www.bilibili.com/video/BV1et42177VM/) 第四题，欢迎点赞关注！

```py [sol-Python3]
class Solution:
    def numberOfSubarrays(self, nums: List[int]) -> int:
        ans = len(nums)
        st = [[inf, 0]]  # 无穷大哨兵
        for x in nums:
            while x > st[-1][0]:
                st.pop()
            if x == st[-1][0]:
                ans += st[-1][1]
                st[-1][1] += 1
            else:
                st.append([x, 1])
        return ans
```

```java [sol-Java]
class Solution {
    public long numberOfSubarrays(int[] nums) {
        long ans = nums.length;
        Deque<int[]> st = new ArrayDeque<>();
        st.push(new int[]{Integer.MAX_VALUE, 0}); // 无穷大哨兵
        for (int x : nums) {
            while (x > st.peek()[0]) {
                st.pop();
            }
            if (x == st.peek()[0]) {
                ans += st.peek()[1]++;
            } else {
                st.push(new int[]{x, 1});
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long numberOfSubarrays(vector<int>& nums) {
        long long ans = nums.size();
        stack<pair<int, int>> st;
        st.emplace(INT_MAX, 0); // 无穷大哨兵
        for (int x : nums) {
            while (x > st.top().first) {
                st.pop();
            }
            if (x == st.top().first) {
                ans += st.top().second++;
            } else {
                st.emplace(x, 1);
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func numberOfSubarrays(nums []int) int64 {
	ans := len(nums)
	type pair struct{ x, cnt int }
	st := []pair{{math.MaxInt, 0}} // 无穷大哨兵
	for _, x := range nums {
		for x > st[len(st)-1].x {
			st = st[:len(st)-1]
		}
		if x == st[len(st)-1].x {
			ans += st[len(st)-1].cnt
			st[len(st)-1].cnt++
		} else {
			st = append(st, pair{x, 1})
		}
	}
	return int64(ans)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

更多相似题目，见下面的单调栈题单。

## 思考题

1. 改成子数组第一个 **或** 最后一个元素是最大值，要怎么做？
2. 改成子数组第一个元素是最大值，最后一个元素是 **最小值**，要怎么做？
3. 改成树上路径问题，见 [2421. 好路径的数目](https://leetcode.cn/problems/number-of-good-paths/)。本题相当于把 2421 的树特化成一条链。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/基环树/最短路/最小生成树/网络流）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/划分/状态机/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

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
