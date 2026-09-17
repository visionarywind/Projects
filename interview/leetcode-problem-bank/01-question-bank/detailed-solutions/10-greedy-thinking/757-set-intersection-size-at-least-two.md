# 757. 设置交集大小至少为2

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/set-intersection-size-at-least-two/
- 题目 slug：`set-intersection-size-at-least-two`
- 来源专题：贪心与思维
- 来源分类路径：二、区间贪心 / §2.3 区间选点
- 难度分：2379
- 外部题解来源：https://leetcode.cn/problems/set-intersection-size-at-least-two/solutions/3836878/tong-yong-jie-fa-zhan-er-fen-cha-zhao-py-i542/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[通用解法：栈+二分查找（Python/Java/C++/Go）](https://leetcode.cn/problems/set-intersection-size-at-least-two/solutions/3836878/tong-yong-jie-fa-zhan-er-fen-cha-zhao-py-i542/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tong-yong-jie-fa-zhan-er-fen-cha-zhao-py-i542`
- topic id：`3836878`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:54:06 +0800

把 $2$ 改成 $k$ 怎么做？

更一般地，每个区间要包含的数字个数各有不同，怎么做？

这题是 [2589. 完成所有任务的最少时间](https://leetcode.cn/problems/minimum-time-to-complete-all-tasks/)，[我的题解](https://leetcode.cn/problems/minimum-time-to-complete-all-tasks/solutions/2163130/tan-xin-pythonjavacgo-by-endlesscheng-w3k3/)。

```py [sol-Python3]
class Solution:
    def intersectionSizeTwo(self, intervals: List[List[int]]) -> int:
        intervals.sort(key=lambda interval: interval[1])
        # 栈中保存闭区间左右端点，栈底到栈顶的区间长度的和
        st = [(-2, -2, 0)]  # 哨兵，保证不和任何区间相交
        for start, end in intervals:
            _, r, s = st[bisect_left(st, (start,)) - 1]
            d = 2 - (st[-1][2] - s)  # 去掉运行中的时间点
            if start <= r:  # start 在区间 st[i] 内
                d -= r - start + 1  # 去掉运行中的时间点
            if d <= 0:
                continue
            while end - st[-1][1] <= d:  # 剩余的 d 填充区间后缀
                l, r, _ = st.pop()
                d += r - l + 1  # 合并区间
            st.append((end - d + 1, end, st[-1][2] + d))
        return st[-1][2]
```

```java [sol-Java]
class Solution {
    public int intersectionSizeTwo(int[][] intervals) {
        Arrays.sort(intervals, (a, b) -> a[1] - b[1]);
        // 栈中保存闭区间左右端点，栈底到栈顶的区间长度的和
        List<int[]> st = new ArrayList<>();
        st.add(new int[]{-2, -2, 0}); // 哨兵，保证不和任何区间相交
        for (int[] t : intervals) {
            int start = t[0], end = t[1];
            int[] e = st.get(lowerBound(st, start) - 1);
            int d = 2 - (st.get(st.size() - 1)[2] - e[2]); // 去掉运行中的时间点
            if (start <= e[1]) { // start 在区间 st[i] 内
                d -= e[1] - start + 1; // 去掉运行中的时间点
            }
            if (d <= 0) {
                continue;
            }
            while (end - st.get(st.size() - 1)[1] <= d) { // 剩余的 d 填充区间后缀
                e = st.remove(st.size() - 1);
                d += e[1] - e[0] + 1; // 合并区间
            }
            st.add(new int[]{end - d + 1, end, st.get(st.size() - 1)[2] + d});
        }
        return st.get(st.size() - 1)[2];
    }

    // 开区间二分
    // 见 https://www.bilibili.com/video/BV1AP41137w7/
    private int lowerBound(List<int[]> st, int target) {
        int left = -1, right = st.size(); // 开区间 (left, right)
        while (left + 1 < right) { // 区间不为空
            // 循环不变量：
            // st[left] < target
            // st[right] >= target
            int mid = (left + right) >>> 1;
            if (st.get(mid)[0] < target) {
                left = mid; // 范围缩小到 (mid, right)
            } else {
                right = mid; // 范围缩小到 (left, mid)
            }
        }
        return right;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int intersectionSizeTwo(vector<vector<int>>& intervals) {
        ranges::sort(intervals, {}, [](auto& a) { return a[1]; });
        // 栈中保存闭区间左右端点，栈底到栈顶的区间长度的和
        vector<array<int, 3>> st = {{-2, -2, 0}}; // 哨兵，保证不和任何区间相交
        for (auto& t : intervals) {
            int start = t[0], end = t[1];
            auto [_, r, s] = *--ranges::lower_bound(st, start, {}, [](auto& x) { return x[0]; });
            int d = 2 - (st.back()[2] - s); // 去掉运行中的时间点
            if (start <= r) { // start 在区间 st[i] 内
                d -= r - start + 1; // 去掉运行中的时间点
            }
            if (d <= 0) {
                continue;
            }
            while (end - st.back()[1] <= d) { // 剩余的 d 填充区间后缀
                auto [l, r, _] = st.back();
                st.pop_back();
                d += r - l + 1; // 合并区间
            }
            st.push_back({end - d + 1, end, st.back()[2] + d});
        }
        return st.back()[2];
    }
};
```

```go [sol-Go]
func intersectionSizeTwo(intervals [][]int) int {
    slices.SortFunc(intervals, func(a, b []int) int { return a[1] - b[1] })
    // 栈中保存闭区间左右端点，栈底到栈顶的区间长度的和
    type tuple struct{ l, r, s int }
    st := []tuple{{-2, -2, 0}} // 哨兵，保证不和任何区间相交
    for _, p := range intervals {
        start, end := p[0], p[1]
        i := sort.Search(len(st), func(i int) bool { return st[i].l >= start }) - 1
        d := 2 - (st[len(st)-1].s - st[i].s) // 去掉运行中的时间点
        if start <= st[i].r { // start 在区间 st[i] 内
            d -= st[i].r - start + 1 // 去掉运行中的时间点
        }
        if d <= 0 {
            continue
        }
        for end-st[len(st)-1].r <= d { // 剩余的 d 填充区间后缀
            top := st[len(st)-1]
            st = st[:len(st)-1]
            d += top.r - top.l + 1 // 合并区间
        }
        st = append(st, tuple{end - d + 1, end, st[len(st)-1].s + d})
    }
    return st[len(st)-1].s
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n)$，其中 $n$ 是 $\textit{intervals}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 相似题目

见下面贪心题单的「**§2.3 区间选点**」。

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
11. [链表、树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `二、区间贪心 / §2.3 区间选点`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、区间贪心 / §2.3 区间选点`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
