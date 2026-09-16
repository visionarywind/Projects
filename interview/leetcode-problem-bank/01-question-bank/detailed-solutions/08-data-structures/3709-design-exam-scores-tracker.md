# 3709. 设计考试分数记录器

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/design-exam-scores-tracker/
- 题目 slug：`design-exam-scores-tracker`
- 来源专题：常用数据结构
- 来源分类路径：一、前缀和 / §1.5 进阶
- 难度分：1648
- 外部题解来源：https://leetcode.cn/problems/design-exam-scores-tracker/solutions/3803666/qian-zhui-he-er-fen-cha-zhao-pythonjavac-83un/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[前缀和 + 二分查找（Python/Java/C++/Go）](https://leetcode.cn/problems/design-exam-scores-tracker/solutions/3803666/qian-zhui-he-er-fen-cha-zhao-pythonjavac-83un/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`qian-zhui-he-er-fen-cha-zhao-pythonjavac-83un`
- topic id：`3803666`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

根据题意：

- $\textit{time}$ 参数是严格递增的，我们可以用一个 $\textit{times}$ 数组记录 $\textit{time}$。这样 $\textit{times}$ 是严格递增数组，可以在上面**二分查找**，原理请看 [二分查找 红蓝染色法【基础算法精讲 04】](https://www.bilibili.com/video/BV1AP41137w7/)。
- $\texttt{totalScore}$ 本质上计算的是关于 $\textit{score}$ 的子数组和，我们可以维护 $\textit{score}$ 的**前缀和**数组 $\textit{preSum}$，从而 $\mathcal{O}(1)$ 计算子数组和。原理请看 [前缀和](https://leetcode.cn/problems/range-sum-query-immutable/solution/qian-zhui-he-ji-qi-kuo-zhan-fu-ti-dan-py-vaar/)。

二分查找 $\textit{times}$ 在 $[\textit{startTime},\textit{endTime}]$ 中的元素的下标范围 $[\textit{left},\textit{right}-1]$。

其中：

- $\textit{left}$ 是 $\textit{times}$ 第一个 $\ge \textit{startTime}$ 的元素下标。
- $\textit{right}-1$ 是 $\textit{times}$ 最后一个 $\le \textit{endTime}$ 的元素下标。
- $\textit{right}$ 是 $\textit{times}$ 第一个 $> \textit{endTime}$ 的元素下标，也是第一个 $\ge \textit{endTime}+1$ 的元素下标。

知道了下标范围，根据 [前缀和](https://leetcode.cn/problems/range-sum-query-immutable/solution/qian-zhui-he-ji-qi-kuo-zhan-fu-ti-dan-py-vaar/) 中的公式，对应的子数组和为

$$
\textit{preSum}[\textit{right}] - \textit{preSum}[\textit{left}]
$$

[本题视频讲解](https://www.bilibili.com/video/BV16E4uzLEdK/?t=29m19s)，欢迎点赞关注~

```py [sol-Python3]
class ExamTracker:
    def __init__(self):
        self.times = []
        self.pre_sum = [0]  # 原理见题目 303. 区域和检索 - 数组不可变

    def record(self, time: int, score: int) -> None:
        self.times.append(time)
        self.pre_sum.append(self.pre_sum[-1] + score)

    def totalScore(self, startTime: int, endTime: int) -> int:
        left = bisect_left(self.times, startTime)
        right = bisect_right(self.times, endTime)
        return self.pre_sum[right] - self.pre_sum[left]
```

```java [sol-Java]
class ExamTracker {
    private final List<Integer> times = new ArrayList<>();
    private final List<Long> preSum = new ArrayList<>();

    public ExamTracker() {
        preSum.add(0L); // 原理见题目 303. 区域和检索 - 数组不可变
    }

    public void record(int time, int score) {
        times.add(time);
        preSum.add(preSum.getLast() + score);
    }

    public long totalScore(int startTime, int endTime) {
        // times 没有重复元素，可以用库函数二分（有重复元素则不行）
        int left = Collections.binarySearch(times, startTime);
        if (left < 0) left = ~left;
        int right = Collections.binarySearch(times, endTime + 1);
        if (right < 0) right = ~right;
        return preSum.get(right) - preSum.get(left);
    }
}
```

```cpp [sol-C++]
class ExamTracker {
    vector<int> times;
    vector<long long> pre_sum = {0}; // 原理见题目 303. 区域和检索 - 数组不可变

public:
    void record(int time, int score) {
        times.push_back(time);
        pre_sum.push_back(pre_sum.back() + score);
    }

    long long totalScore(int startTime, int endTime) {
        int left = ranges::lower_bound(times, startTime) - times.begin();
        int right = ranges::upper_bound(times, endTime) - times.begin();
        return pre_sum[right] - pre_sum[left];
    }
};
```

```go [sol-Go]
type ExamTracker struct {
	times  []int
	preSum []int64
}

func Constructor() ExamTracker {
	// preSum 为什么加个 0，见题目 303. 区域和检索 - 数组不可变
	return ExamTracker{[]int{}, []int64{0}}
}

func (e *ExamTracker) Record(time, score int) {
	e.times = append(e.times, time)
	e.preSum = append(e.preSum, e.preSum[len(e.preSum)-1]+int64(score))
}

func (e *ExamTracker) TotalScore(startTime, endTime int) int64 {
	left := sort.SearchInts(e.times, startTime)
	right := sort.SearchInts(e.times, endTime+1) // 也可以在 e.times[left:] 中二分
	return e.preSum[right] - e.preSum[left]
}
```

#### 复杂度分析

- 时间复杂度：
  - 初始化：$\mathcal{O}(1)$。
  - $\texttt{record}$：均摊 $\mathcal{O}(1)$。
  - $\texttt{totalScore}$：$\mathcal{O}(\log q)$，其中 $q$ 是 $\texttt{record}$ 的调用次数。
- 空间复杂度：$\mathcal{O}(q)$。

## 专题训练

1. 下面二分题单的「**一、二分查找**」。
2. 下面数据结构题单的「**一、前缀和**」。

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

## 本地原创解析

### 1. 题意重述

本题来自 `一、前缀和 / §1.5 进阶`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、前缀和 / §1.5 进阶`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
