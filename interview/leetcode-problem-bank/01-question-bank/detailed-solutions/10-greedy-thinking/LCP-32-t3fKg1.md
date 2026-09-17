# LCP 32. 批量处理任务

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/t3fKg1/
- 题目 slug：`t3fKg1`
- 来源专题：贪心与思维
- 来源分类路径：二、区间贪心 / §2.3 区间选点
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/t3fKg1/solutions/2164272/tan-xin-zhan-you-hua-pythonjavacgo-by-en-sp1s/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[贪心+栈优化（Python/Java/C++/Go）](https://leetcode.cn/problems/t3fKg1/solutions/2164272/tan-xin-zhan-you-hua-pythonjavacgo-by-en-sp1s/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tan-xin-zhan-you-hua-pythonjavacgo-by-en-sp1s`
- topic id：`2164272`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:54:06 +0800

先来想想暴力怎么做。

### 提示 1

按照右端点排序。

### 提示 2

对于 $\textit{tasks}[i]$ 来说，它右侧的任务要么和它没有交集，要么包含它的区间**后缀**。

### 提示 3

遍历排序后的任务，先统计区间内的已有的电脑运行时间点，如果个数小于 $\textit{period}$，则需要新增时间点。根据提示 2，尽量把新增的时间点安排在区间 $[\textit{start},\textit{end}]$ 的后缀上，这样下一个区间就能统计到更多已有的时间点。

```py [sol1-Python3]
# 会超时/超内存的暴力代码
class Solution:
    def processTasks(self, tasks: List[List[int]]) -> int:
        tasks.sort(key=lambda t: t[1])
        run = [False] * (tasks[-1][1] + 1)
        for start, end, d in tasks:
            d -= sum(run[start:end + 1])  # 去掉运行中的时间点
            if d > 0:
                for i in range(end, start - 1, -1):  # 剩余的 d 填充区间后缀
                    if run[i]: continue
                    run[i] = True
                    d -= 1
                    if d == 0: break
        return sum(run)
```

```java [sol1-Java]
// 会超时/超内存的暴力代码
class Solution {
    public int processTasks(int[][] tasks) {
        Arrays.sort(tasks, (a, b) -> a[1] - b[1]);
        int ans = 0;
        var run = new boolean[tasks[tasks.length - 1][1] + 1];
        for (var t : tasks) {
            int start = t[0], end = t[1], d = t[2];
            for (int i = start; i <= end; ++i)
                if (run[i]) --d; // 去掉运行中的时间点
            for (int i = end; d > 0; --i) // 剩余的 d 填充区间后缀
                if (!run[i]) {
                    run[i] = true;
                    --d;
                    ++ans;
                }
        }
        return ans;
    }
}
```

```cpp [sol1-C++]
// 会超时/超内存的暴力代码
class Solution {
    bool run[(int) 1e9 + 1];
public:
    int processTasks(vector<vector<int>> &tasks) {
        sort(tasks.begin(), tasks.end(), [](auto &a, auto &b) {
            return a[1] < b[1];
        });
        int ans = 0;
        for (auto &t : tasks) {
            int start = t[0], end = t[1], d = t[2];
            for (int i = start; i <= end; ++i)
                d -= run[i]; // 去掉运行中的时间点
            for (int i = end; d > 0; --i) // 剩余的 d 填充区间后缀
                if (!run[i]) {
                    run[i] = true;
                    --d;
                    ++ans;
                }
        }
        return ans;
    }
};
```

```go [sol1-Go]
// 会超时/超内存的暴力代码
func processTasks(tasks [][]int) (ans int) {
	sort.Slice(tasks, func(i, j int) bool { return tasks[i][1] < tasks[j][1] })
	run := make([]bool, tasks[len(tasks)-1][1]+1)
	for _, t := range tasks {
		start, end, d := t[0], t[1], t[2]
		for _, b := range run[start : end+1] { // 去掉运行中的时间点
			if b {
				d--
			}
		}
		for i := end; d > 0; i-- { // 剩余的 d 填充区间后缀
			if !run[i] {
				run[i] = true
				d--
				ans++
			}
		}
	}
	return
}
```

### 复杂度分析

- 时间复杂度：$O(nU)$，其中 $n$ 为 $\textit{tasks}$ 的长度，$U=\max(\textit{end}_i)$。
- 空间复杂度：$O(U)$。

# 优化

### 前置知识：二分查找

见[【基础算法精讲 04】](https://www.bilibili.com/video/BV1AP41137w7/)。

### 思路

由于每次都是从右到左新增时间点，相当于把若干右侧的区间合并成一个大区间，因此可以用栈来优化。

栈中保存闭区间的左右端点，以及从栈底到栈顶的区间长度之和（类似前缀和）。

合并前先在栈中二分查找左端点所在区间，由于我们保存了长度之和，所以可以算出 $[\textit{start},\textit{end}]$ 范围内的运行中的时间点。

如果还需要新增时间点，那么就从右到左合并，具体细节见代码。

```py [sol3-Python3]
class Solution:
    def processTasks(self, tasks: List[List[int]]) -> int:
        tasks.sort(key=lambda t: t[1])
        st = [(-2, -2, 0)]  # 闭区间左右端点，栈底到栈顶的区间长度的和
        for start, end, d in tasks:
            _, r, s = st[bisect_left(st, (start,)) - 1]
            d -= st[-1][2] - s  # 去掉运行中的时间点
            if start <= r:  # start 在区间 st[i] 内
                d -= r - start + 1  # 去掉运行中的时间点
            if d <= 0: continue
            while end - st[-1][1] <= d:  # 剩余的 d 填充区间后缀
                l, r, _ = st.pop()
                d += r - l + 1  # 合并区间
            st.append((end - d + 1, end, st[-1][2] + d))
        return st[-1][2]
```

```java [sol3-Java]
class Solution {
    public int processTasks(int[][] tasks) {
        Arrays.sort(tasks, (a, b) -> a[1] - b[1]);
        var st = new ArrayList<int[]>();
        st.add(new int[]{-2, -2, 0}); // 闭区间左右端点，栈底到栈顶的区间长度的和
        for (var t : tasks) {
            int start = t[0], end = t[1], d = t[2];
            var e = st.get(lowerBound(st, start) - 1);
            d -= st.get(st.size() - 1)[2] - e[2]; // 去掉运行中的时间点
            if (start <= e[1]) // start 在区间 st[i] 内
                d -= e[1] - start + 1; // 去掉运行中的时间点
            if (d <= 0) continue;
            while (end - st.get(st.size() - 1)[1] <= d) { // 剩余的 d 填充区间后缀
                e = st.remove(st.size() - 1);
                d += e[1] - e[0] + 1; // 合并区间
            }
            st.add(new int[]{end - d + 1, end, st.get(st.size() - 1)[2] + d});
        }
        return st.get(st.size() - 1)[2];
    }

    // 开区间写法
    private int lowerBound(List<int[]> st, int target) {
        int left = -1, right = st.size(); // 开区间 (left, right)
        while (left + 1 < right) { // 区间不为空
            // 循环不变量：
            // st[left] < target
            // st[right] >= target
            int mid = (left + right) >>> 1;
            if (st.get(mid)[0] < target)
                left = mid; // 范围缩小到 (mid, right)
            else
                right = mid; // 范围缩小到 (left, mid)
        }
        return right; // 或者 left+1
    }
}
```

```cpp [sol3-C++]
class Solution {
public:
    int processTasks(vector<vector<int>> &tasks) {
        sort(tasks.begin(), tasks.end(), [](auto &a, auto &b) {
            return a[1] < b[1];
        });
        vector<tuple<int, int, int>> st{{-2, -2, 0}}; // 闭区间左右端点，栈底到栈顶的区间长度的和
        for (auto &t : tasks) {
            int start = t[0], end = t[1], d = t[2];
            auto[_, r, s] = *--lower_bound(st.begin(), st.end(), start, [](const auto &a, int b) {
                return get<0>(a) < b;
            });
            d -= get<2>(st.back()) - s; // 去掉运行中的时间点
            if (start <= r) // start 在区间 st[i] 内
                d -= r - start + 1; // 去掉运行中的时间点
            if (d <= 0) continue;
            while (end - get<1>(st.back()) <= d) { // 剩余的 d 填充区间后缀
                auto[l, r, _] = st.back();
                d += r - l + 1; // 合并区间
                st.pop_back();
            }
            st.emplace_back(end - d + 1, end, get<2>(st.back()) + d);
        }
        return get<2>(st.back());
    }
};
```

```go [sol3-Go]
func processTasks(tasks [][]int) int {
	sort.Slice(tasks, func(i, j int) bool { return tasks[i][1] < tasks[j][1] })
	type tuple struct{ l, r, s int }
	st := []tuple{{-2, -2, 0}} // 闭区间左右端点，栈底到栈顶的区间长度的和
	for _, p := range tasks {
		start, end, d := p[0], p[1], p[2]
		i := sort.Search(len(st), func(i int) bool { return st[i].l >= start }) - 1
		d -= st[len(st)-1].s - st[i].s // 去掉运行中的时间点
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

### 复杂度分析

- 时间复杂度：$O(n\log n)$，其中 $n$ 为 $\textit{tasks}$ 的长度。
- 空间复杂度：$O(n)$。

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
