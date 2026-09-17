# 3961. 设备评分的最大和

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximize-sum-of-device-ratings/
- 题目 slug：`maximize-sum-of-device-ratings`
- 来源专题：贪心与思维
- 来源分类路径：五、思维题 / §5.2 脑筋急转弯
- 难度分：1880
- 外部题解来源：https://leetcode.cn/problems/maximize-sum-of-device-ratings/solutions/3983487/ba-dan-yuan-ji-zhong-dao-yi-ge-she-bei-z-afnf/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[把最小值汇集到一起，一次遍历（Python/Java/C++/Go）](https://leetcode.cn/problems/maximize-sum-of-device-ratings/solutions/3983487/ba-dan-yuan-ji-zhong-dao-yi-ge-she-bei-z-afnf/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`ba-dan-yuan-ji-zhong-dao-yi-ge-she-bei-z-afnf`
- topic id：`3983487`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:07:12 +0800

先特判 $n=1$ 的情况。由于我们无法让任何设备的评分变大，所以不移动单元更好，答案为所有 $\textit{units}[i][0]$ 之和。

下面讨论 $n\ge 2$ 的情况。

每个设备只能移除一个单元。根据题意，只有移除最小单元才能让设备的评分变大。我们先移除每个设备的最小单元，放在一边，那么每个设备的评分就从其最小值变成其**次小值**。

这 $m$ 个移出来的最小值，怎么处理？

如果分开存放，会有多个设备的评分都降低。所以最优做法是**把最小值都集中到同一个设备**中。

集中到哪个设备最优？

设 $\textit{units}$ 中的最小值为 $\textit{mn}$，如果集中到设备 $i$，那么其评分会从 $\textit{units}[i]$ 的次小值减少至 $\textit{mn}$。为了让这个减小量尽量小，次小值越小越好。

设所有次小值中的最小值为 $\textit{mn}_2$。贪心地，把最小值集中到包含 $\textit{mn}_2$ 的那个设备中，可以让评分的减少量尽量小。

[本题视频讲解](https://www.bilibili.com/video/BV1ptJw6hENZ/?t=9m23s)，欢迎点赞关注~

```py [sol-Python3]
class Solution:
    def maxRatings(self, units: list[list[int]]) -> int:
        if len(units[0]) == 1:
            # 每个设备都只有一个单元
            return sum(unit[0] for unit in units)

        ans = 0
        mn = mn2 = inf
        for unit in units:
            unit.sort()  # 线性做法见【Python3 手动计算最小次小】
            ans += unit[1]  # 先加上次小
            mn2 = min(mn2, unit[1])
            mn = min(mn, unit[0])

        # 把包含 mn2 的那个设备作为集中站，存放每个设备的最小值
        ans += mn - mn2  # 把 ans 中的 mn2 替换成 mn
        return ans
```

```py [sol-Python3 手动计算最小次小]
class Solution:
    def maxRatings(self, units: list[list[int]]) -> int:
        if len(units[0]) == 1:
            # 每个设备都只有一个单元
            return sum(unit[0] for unit in units)

        ans = 0
        mn = mn2 = inf
        for unit in units:
            # 计算最小次小
            unit_min = unit_min2 = inf
            for x in unit:
                if x < unit_min:
                    unit_min2 = unit_min
                    unit_min = x
                elif x < unit_min2:
                    unit_min2 = x

            ans += unit_min2  # 先加上次小
            mn2 = min(mn2, unit_min2)
            mn = min(mn, unit_min)

        # 把包含 mn2 的那个设备作为集中站，存放每个设备的最小值
        ans += mn - mn2  # 把 ans 中的 mn2 替换成 mn
        return ans
```

```java [sol-Java]
class Solution {
    public long maxRatings(int[][] units) {
        long ans = 0;
        if (units[0].length == 1) {
            // 每个设备都只有一个单元
            for (int[] unit : units) {
                ans += unit[0];
            }
            return ans;
        }

        int mn = Integer.MAX_VALUE;
        int mn2 = Integer.MAX_VALUE;
        for (int[] unit : units) {
            // 计算最小次小
            int unitMin = Integer.MAX_VALUE;
            int unitMin2 = Integer.MAX_VALUE;
            for (int x : unit) {
                if (x < unitMin) {
                    unitMin2 = unitMin;
                    unitMin = x;
                } else if (x < unitMin2) {
                    unitMin2 = x;
                }
            }

            ans += unitMin2; // 先加上次小
            mn2 = Math.min(mn2, unitMin2);
            mn = Math.min(mn, unitMin);
        }

        // 把包含 mn2 的那个设备作为集中站，存放每个设备的最小值
        ans += mn - mn2; // 把 ans 中的 mn2 替换成 mn
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long maxRatings(vector<vector<int>>& units) {
        long long ans = 0;
        if (units[0].size() == 1) {
            // 每个设备都只有一个单元
            for (auto& unit : units) {
                ans += unit[0];
            }
            return ans;
        }

        int mn = INT_MAX, mn2 = INT_MAX;
        for (auto& unit : units) {
            // 计算最小次小
            int unit_min = INT_MAX, unit_min2 = INT_MAX;
            for (auto x : unit) {
                if (x < unit_min) {
                    unit_min2 = unit_min;
                    unit_min = x;
                } else if (x < unit_min2) {
                    unit_min2 = x;
                }
            }

            ans += unit_min2; // 先加上次小
            mn2 = min(mn2, unit_min2);
            mn = min(mn, unit_min);
        }

        // 把包含 mn2 的那个设备作为集中站，存放每个设备的最小值
        ans += mn - mn2; // 把 ans 中的 mn2 替换成 mn
        return ans;
    }
};
```

```go [sol-Go]
func maxRatings(units [][]int) int64 {
	ans := 0
	if len(units[0]) == 1 {
		// 每个设备都只有一个单元
		for _, unit := range units {
			ans += unit[0]
		}
		return int64(ans)
	}

	mn, mn2 := math.MaxInt, math.MaxInt
	for _, unit := range units {
		// 计算最小次小
		unitMin, unitMin2 := math.MaxInt, math.MaxInt
		for _, x := range unit {
			if x < unitMin {
				unitMin2 = unitMin
				unitMin = x
			} else if x < unitMin2 {
				unitMin2 = x
			}
		}

		ans += unitMin2 // 先加上次小
		mn2 = min(mn2, unitMin2)
		mn = min(mn, unitMin)
	}

	// 把包含 mn2 的那个设备作为集中站，存放每个设备的最小值
	ans += mn - mn2 // 把 ans 中的 mn2 替换成 mn
	return int64(ans)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(mn)$，其中 $m$ 和 $n$ 分别是 $\textit{units}$ 的行数和列数。
- 空间复杂度：$\mathcal{O}(1)$。

## 进阶问题

如果不保证每个 $\textit{units}[i]$ 的长度都是 $n$，怎么做？

欢迎在评论区分享你的思路/代码。

> 我在视频讲解的末尾，讲了这个进阶问题的思路。

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

本题来自 `五、思维题 / §5.2 脑筋急转弯`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `五、思维题 / §5.2 脑筋急转弯`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
