# 3424. 将数组变相同的最小代价

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-cost-to-make-arrays-identical/
- 题目 slug：`minimum-cost-to-make-arrays-identical`
- 来源专题：贪心与思维
- 来源分类路径：五、思维题 / §5.2 脑筋急转弯
- 难度分：1503
- 外部题解来源：https://leetcode.cn/problems/minimum-cost-to-make-arrays-identical/solutions/3051409/nao-jin-ji-zhuan-wan-tan-xin-pythonjavac-c2gd/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[脑筋急转弯+贪心+优化（Python/Java/C++/Go）](https://leetcode.cn/problems/minimum-cost-to-make-arrays-identical/solutions/3051409/nao-jin-ji-zhuan-wan-tan-xin-pythonjavac-c2gd/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`nao-jin-ji-zhuan-wan-tan-xin-pythonjavac-c2gd`
- topic id：`3051409`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:07:12 +0800

把 $\textit{arr}$ 简记为 $a$，$\textit{brr}$ 简记为 $b$。

如果不使用操作一，那么答案为所有 $|a[i]-b[i]|$ 之和。

如果使用操作一，那么直接把 $a$ 分成 $n$ 个长为 $1$ 的子数组，这样 $a$ 就可以随意排了。

最优配对方式是最小的 $a[i]$ 与最小的 $b[i]$ 一对，次小的 $a[i]$ 与次小的 $b[i]$ 一对。用交换论证法可以证明这样做是最优的，详细解释请看 [视频讲解](https://www.bilibili.com/video/BV1xBwBeEEie/?t=1m49s)。

**优化**：如果 $k$ 很大，比只用操作二还大（或者相等），那么使用操作一一定不会得到更优的答案。所以可以据此提前返回，这样可以省去排序的时间。

```py [sol-Python3]
class Solution:
    def minCost(self, a: List[int], b: List[int], k: int) -> int:
        ans2 = sum(abs(x - y) for x, y in zip(a, b))
        if ans2 <= k:
            return ans2

        a.sort()
        b.sort()
        ans1 = sum(abs(x - y) for x, y in zip(a, b)) + k
        return min(ans1, ans2)
```

```java [sol-Java]
class Solution {
    public long minCost(int[] a, int[] b, long k) {
        long ans2 = 0;
        for (int i = 0; i < a.length; i++) {
            ans2 += Math.abs(a[i] - b[i]);
        }
        if (ans2 <= k) {
            return ans2;
        }

        Arrays.sort(a);
        Arrays.sort(b);
        long ans1 = k;
        for (int i = 0; i < a.length; i++) {
            ans1 += Math.abs(a[i] - b[i]);
        }

        return Math.min(ans1, ans2);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long minCost(vector<int>& a, vector<int>& b, long long k) {
        long long ans2 = 0;
        for (int i = 0; i < a.size(); i++) {
            ans2 += abs(a[i] - b[i]);
        }
        if (ans2 <= k) {
            return ans2;
        }

        ranges::sort(a);
        ranges::sort(b);
        long long ans1 = k;
        for (int i = 0; i < a.size(); i++) {
            ans1 += abs(a[i] - b[i]);
        }

        return min(ans1, ans2);
    }
};
```

```go [sol-Go]
func minCost(a, b []int, k int64) int64 {
	ans2 := int64(0)
	for i, x := range a {
		ans2 += int64(abs(x - b[i]))
	}
	if ans2 <= k {
		return ans2
	}

	slices.Sort(a)
	slices.Sort(b)
	ans1 := k
	for i, x := range a {
		ans1 += int64(abs(x - b[i]))
	}

	return min(ans1, ans2)
}

func abs(x int) int { if x < 0 { return -x }; return x }
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n)$，其中 $n$ 是 $a$ 的长度。瓶颈在排序上。
- 空间复杂度：$\mathcal{O}(1)$。忽略排序的栈开销。

更多相似题目，见下面贪心题单中的「**§1.1 从最小/最大开始贪心**」和「**§1.7 交换论证法**」。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. 【本题相关】[贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

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
