# 3041. 修改数组后最大化数组中的连续元素数目

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximize-consecutive-elements-in-an-array-after-modification/
- 题目 slug：`maximize-consecutive-elements-in-an-array-after-modification`
- 来源专题：动态规划
- 来源分类路径：七、其他线性 DP / §7.4 合法子序列 DP
- 难度分：2231
- 外部题解来源：https://leetcode.cn/problems/maximize-consecutive-elements-in-an-array-after-modification/solutions/2643723/ben-ti-zui-jian-dan-xie-fa-pythonjavacgo-kcc6/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[本题最简单写法（Python/Java/C++/Go）](https://leetcode.cn/problems/maximize-consecutive-elements-in-an-array-after-modification/solutions/2643723/ben-ti-zui-jian-dan-xie-fa-pythonjavacgo-kcc6/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`ben-ti-zui-jian-dan-xie-fa-pythonjavacgo-kcc6`
- topic id：`2643723`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

[视频讲解](https://www.bilibili.com/video/BV1Sm411U7cR/) 第四题，额外讲了另一种做法，以及如何分类思考子序列 DP。

## 提示 1

把数组排序。

为什么？设我们选的元素在排序后为 $b$，那么有 $b[i] + 1 = b[i+1]$，这意味着 $b$ 中元素在操作前，必然有 $b[i] \le b[i+1]$。反证：如果操作前 $b[i] > b[i+1]$，那么操作后 $b[i]$ 至多和 $b[i+1]$ 相等，不会出现 $b[i+1]$ 比 $b[i]$ 多 $1$ 的情况。

所以可以排序。

## 提示 2

排序后，我们选的是 $\textit{nums}$ 中的一个**子序列**。

定义 $f[x]$ 表示子序列的最后一个数是 $x$ 时，子序列的最大长度。

从左到右遍历数组 $x = \textit{nums}[i]$：

- 如果操作，那么 $x+1$ 可以接在末尾为 $x$ 的子序列后面，即 $f[x+1] = f[x] + 1$。
- 如果不操作，那么 $x$ 可以接在末尾为 $x-1$ 的子序列后面，即 $f[x] = f[x-1] + 1$。

比如 $\textit{nums} = [1,2,2]$：

- 遍历到 $\textit{nums}[0]=1$ 时，$f[2]=1,\ f[1]=1$。
- 遍历到 $\textit{nums}[1]=2$ 时，$f[3]=f[2]+1=2,\ f[2]=f[1]+1=2$。注意要先计算 $f[x+1]$ 再计算 $f[x]$（不然这里会算出 $f[3]=3$）。此时 $f[1]$ 还是 $1$。
- 遍历到 $\textit{nums}[2]=2$ 时，$f[3]=f[2]+1=3,\ f[2]=f[1]+1=2$。此时 $f[1]$ 还是 $1$。

最后返回 $f[x]$ 的最大值。

```py [sol-Python3]
class Solution:
    def maxSelectedElements(self, nums: List[int]) -> int:
        nums.sort()
        f = defaultdict(int)
        for x in nums:
            f[x + 1] = f[x] + 1
            f[x] = f[x - 1] + 1
        return max(f.values())
```

```java [sol-Java]
class Solution {
    public int maxSelectedElements(int[] nums) {
        Arrays.sort(nums);
        Map<Integer, Integer> f = new HashMap<>();
        for (int x : nums) {
            f.put(x + 1, f.getOrDefault(x, 0) + 1);
            f.put(x, f.getOrDefault(x - 1, 0) + 1);
        }
        int ans = 0;
        for (int res : f.values()) {
            ans = Math.max(ans, res);
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxSelectedElements(vector<int> &nums) {
        ranges::sort(nums);
        unordered_map<int, int> f;
        for (int x : nums) {
            f[x + 1] = f[x] + 1;
            f[x] = f[x - 1] + 1;
        }
        int ans = 0;
        for (auto &[_, res] : f) {
            ans = max(ans, res);
        }
        return ans;
    }
};
```

```go [sol-Go]
func maxSelectedElements(nums []int) (ans int) {
	slices.Sort(nums)
	f := map[int]int{}
	for _, x := range nums {
		f[x+1] = f[x] + 1
		f[x] = f[x-1] + 1
	}
	for _, res := range f {
		ans = max(ans, res)
	}
	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n)$，其中 $n$ 为 $\textit{nums}$ 的长度。瓶颈在排序上。
- 空间复杂度：$\mathcal{O}(n)$。

## 分类题单

以下题单没有特定的顺序，可以按照个人喜好刷题。

1. [滑动窗口（定长/不定长/多指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/贪心/脑筋急转弯）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `七、其他线性 DP / §7.4 合法子序列 DP`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `七、其他线性 DP / §7.4 合法子序列 DP`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
