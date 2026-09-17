# 3281. 范围内整数的最大得分

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximize-score-of-numbers-in-ranges/
- 题目 slug：`maximize-score-of-numbers-in-ranges`
- 来源专题：二分算法
- 来源分类路径：二、二分答案 / §2.5 最大化最小值
- 难度分：1768
- 外部题解来源：https://leetcode.cn/problems/maximize-score-of-numbers-in-ranges/solutions/2908931/er-fen-da-an-zui-da-hua-zui-xiao-zhi-pyt-twe2/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[二分答案：最大化最小值（Python/Java/C++/Go）](https://leetcode.cn/problems/maximize-score-of-numbers-in-ranges/solutions/2908931/er-fen-da-an-zui-da-hua-zui-xiao-zhi-pyt-twe2/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`er-fen-da-an-zui-da-hua-zui-xiao-zhi-pyt-twe2`
- topic id：`2908931`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 16:46:02 +0800

## 题意

给你 $n$ 个一样长的区间，每个区间选一个数，最大化**得分**。得分即所选数字中的**任意两数之差的最小值**。

## 二分答案

假设得分为 $\textit{score}$。

把区间按照左端点排序。这样我们只需考虑相邻区间所选数字之差。

设从第一个区间选了数字 $x$，那么第二个区间所选的数字至少为 $x+\textit{score}$，否则不满足得分的定义。

由于得分越大，所选数字越可能不在区间内，有单调性，可以二分答案。

> 或者说，看到「最大化最小值」就要先思考二分。

## 贪心

现在问题变成：

- 给定 $\textit{score}$，能否从每个区间各选一个数，使得任意两数之差的最小值**至少**为 $\textit{score}$。

⚠**注意**：这里是至少，不是恰好，两数之差的最小值可以不等于 $\textit{score}$。由于二分会不断缩小范围，最终一定会缩小到任意两数之差的最小值恰好等于 $\textit{score}$ 的位置上。

把区间按照左端点排序。第一个数选谁？

贪心地想，第一个数越小，第二个数就越能在区间内，所以第一个数要选 $x_0 = \textit{start}[0]$。

如果第二个数 $x_1 = x_0+\textit{score}$ 超过了区间右端点 $\textit{start}[1] + d$，那么 $\textit{score}$ 太大了，应当减小二分的右边界 $\textit{right}$。

如果 $x_1\le \textit{start}[1] + d$，我们还需要保证 $x_1$ 大于等于区间左端点 $\textit{start}[1]$，所以最终

$$
x_1 = \max(x_0+\textit{score}, \textit{start}[1])
$$

依此类推，第 $i$ 个区间所选的数为

$$
x_i = \max(x_{i-1}+\textit{score}, \textit{start}[i])
$$

必须满足

$$
x_i\le \textit{start}[i] + d
$$

如果所有选的数都满足上式，那么增大二分的左边界 $\textit{left}$。

## 细节

下面代码采用开区间二分，这仅仅是二分的一种写法，使用闭区间或者半闭半开区间都是可以的。

- 开区间左端点初始值：$0$。一定可以选出 $n$ 个数，两两之差都大于等于 $0$。
- 开区间右端点初始值：$\left\lfloor\dfrac{\textit{start}[n-1]+d-\textit{start}[0]}{n-1}\right\rfloor+1$。最小值不会大于平均值，所以比平均值更大的数必然无法满足要求。

对于开区间写法，简单来说 `check(mid) == true` 时更新的是谁，最后就返回谁。相比其他二分写法，开区间写法不需要思考加一减一等细节，更简单。推荐使用开区间写二分。

代码实现时，可以假设第一个区间左边还选了一个数 $-\infty$，这样不影响答案且代码更简洁。

具体请看 [视频讲解](https://www.bilibili.com/video/BV1z5pieUEkQ/) 第二题，欢迎点赞关注~

```py [sol-Python3]
class Solution:
    def maxPossibleScore(self, start: List[int], d: int) -> int:
        start.sort()

        def check(score: int) -> bool:
            x = -inf
            for s in start:
                x = max(x + score, s)  # x 必须 >= 区间左端点 s
                if x > s + d:
                    return False
            return True

        left, right = 0, (start[-1] + d - start[0]) // (len(start) - 1) + 1
        while left + 1 < right:
            mid = (left + right) // 2
            if check(mid):
                left = mid
            else:
                right = mid
        return left
```

```py [sol-Python3 库函数写法]
class Solution:
    def maxPossibleScore(self, start: List[int], d: int) -> int:
        start.sort()
        # 二分最小的不满足要求的 score+1，最终得到的答案就是最大的满足要求的 score
        def check(score: int) -> bool:
            score += 1
            x = -inf
            for s in start:
                x = max(x + score, s)  # x 必须 >= 区间左端点 s
                if x > s + d:
                    return True
            return False
        return bisect_left(range((start[-1] + d - start[0]) // (len(start) - 1)), True, key=check)
```

```java [sol-Java]
class Solution {
    public int maxPossibleScore(int[] start, int d) {
        Arrays.sort(start);
        int n = start.length;
        int left = 0;
        int right = (start[n - 1] + d - start[0]) / (n - 1) + 1;
        while (left + 1 < right) {
            int mid = (left + right) >>> 1;
            if (check(start, d, mid)) {
                left = mid;
            } else {
                right = mid;
            }
        }
        return left;
    }

    private boolean check(int[] start, int d, int score) {
        long x = Long.MIN_VALUE;
        for (int s : start) {
            x = Math.max(x + score, s); // x 必须 >= 区间左端点 s
            if (x > s + d) {
                return false;
            }
        }
        return true;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxPossibleScore(vector<int>& start, int d) {
        ranges::sort(start);

        auto check = [&](int score) -> bool {
            long long x = LLONG_MIN;
            for (int s : start) {
                x = max(x + score, (long long) s); // x 必须 >= 区间左端点 s
                if (x > s + d) {
                    return false;
                }
            }
            return true;
        };

        int left = 0, right = (start.back() + d - start[0]) / (start.size() - 1) + 1;
        while (left + 1 < right) {
            int mid = left + (right - left) / 2;
            (check(mid) ? left : right) = mid;
        }
        return left;
    }
};
```

```go [sol-Go]
func maxPossibleScore(start []int, d int) int {
	slices.Sort(start)
	n := len(start)
	// 二分最小的不满足要求的 score+1，最终得到的答案就是最大的满足要求的 score
	return sort.Search((start[n-1]+d-start[0])/(n-1), func(score int) bool {
		score++
		x := math.MinInt
		for _, s := range start {
			x = max(x+score, s) // x 必须 >= 区间左端点 s
			if x > s+d {
				return true
			}
		}
		return false
	})
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n + n\log A)$，其中 $n$ 是 $\textit{nums}$ 的长度，$A = \dfrac{\max(\textit{start}) + d - \min(\textit{start})}{n-1}$。排序 $\mathcal{O}(n\log n)$。二分 $\mathcal{O}(\log A)$ 次，每次 $\mathcal{O}(n)$。
- 空间复杂度：$\mathcal{O}(1)$。忽略排序的栈开销。

更多相似题目，见下面二分题单中的「**最大化最小值**」。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与一般树（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/circle/discuss/K0n2gO/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

## 本地原创解析

### 1. 题意重述

本题来自 `二、二分答案 / §2.5 最大化最小值`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、二分答案 / §2.5 最大化最小值`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
