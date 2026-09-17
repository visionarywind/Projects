# 2653. 滑动子数组的美丽值

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/sliding-subarray-beauty/
- 题目 slug：`sliding-subarray-beauty`
- 来源专题：滑动窗口与双指针
- 来源分类路径：一、定长滑动窗口 / §1.2 进阶（选做）
- 难度分：1786
- 外部题解来源：https://leetcode.cn/problems/sliding-subarray-beauty/solutions/2241294/hua-dong-chuang-kou-bao-li-mei-ju-by-end-9mvl/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[滑动窗口+暴力枚举（Python/Java/C++/Go）](https://leetcode.cn/problems/sliding-subarray-beauty/solutions/2241294/hua-dong-chuang-kou-bao-li-mei-ju-by-end-9mvl/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`hua-dong-chuang-kou-bao-li-mei-ju-by-end-9mvl`
- topic id：`2241294`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 16:46:02 +0800

## 什么是第 x 小？

例如数组 $[1,1,1,2,2]$，其中第 $1$ 小、第 $2$ 小和第 $3$ 小的数都是 $1$，第 $4$ 小和第 $5$ 小的数都是 $2$。

- 第 $x$ 小等价于：求**最小**的 $v$，满足 $\le v$ 的数**至少**有 $x$ 个。
- 第 $x$ 大等价于：求**最大**的 $v$，满足 $\ge v$ 的数**至少**有 $x$ 个。

## 思路

本题数据范围很小（$-50\le \textit{nums}[i]\le 50$），我们可以借鉴**计数排序**的思想，用一个 $\textit{cnt}$ 数组维护窗口内每个数的出现次数。

枚举 $v=-50,-49,\ldots,-1$，统计 $\le v$ 的元素个数。如果个数 $\ge x$，那么 $v$ 就是答案。

```py [sol-Python3]
class Solution:
    def getSubarrayBeauty(self, nums: List[int], k: int, x: int) -> List[int]:
        cnt = [0] * 101
        for num in nums[:k - 1]:  # 先往窗口内添加 k-1 个数
            cnt[num] += 1

        ans = [0] * (len(nums) - k + 1)
        for i, (in_, out) in enumerate(zip(nums[k - 1:], nums)):
            cnt[in_] += 1  # 进入窗口（保证窗口有恰好 k 个数）
            left = x  # 从 x 开始倒着减，减到 <= 0 就找到了答案
            for v in range(-50, 0):  # 暴力枚举负数范围 [-50, -1]
                left -= cnt[v]
                if left <= 0:  # 找到答案
                    ans[i] = v
                    break
            cnt[out] -= 1  # 离开窗口
        return ans
```

```java [sol-Java]
class Solution {
    public int[] getSubarrayBeauty(int[] nums, int k, int x) {
        final int BIAS = 50;
        int[] cnt = new int[BIAS * 2 + 1];
        for (int i = 0; i < k - 1; i++) { // 先往窗口内添加 k-1 个数
            cnt[nums[i] + BIAS]++;
        }

        int n = nums.length;
        int[] ans = new int[n - k + 1];
        for (int i = k - 1; i < n; i++) {
            cnt[nums[i] + BIAS]++; // 进入窗口（保证窗口有恰好 k 个数）
            int left = x; // 从 x 开始倒着减，减到 <= 0 就找到了答案
            for (int v = -50; v < 0; v++) { // 暴力枚举负数范围 [-50, -1]
                left -= cnt[v + BIAS];
                if (left <= 0) { // 找到答案
                    ans[i - k + 1] = v;
                    break;
                }
            }
            cnt[nums[i - k + 1] + BIAS]--; // 离开窗口
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> getSubarrayBeauty(vector<int>& nums, int k, int x) {
        constexpr int BIAS = 50;
        int cnt[BIAS * 2 + 1]{};
        for (int i = 0; i < k - 1; i++) { // 先往窗口内添加 k-1 个数
            cnt[nums[i] + BIAS]++;
        }

        int n = nums.size();
        vector<int> ans(n - k + 1);
        for (int i = k - 1; i < n; i++) {
            cnt[nums[i] + BIAS]++; // 进入窗口（保证窗口有恰好 k 个数）
            int left = x; // 从 x 开始倒着减，减到 <= 0 就找到了答案
            for (int v = -50; v < 0; v++) { // 暴力枚举负数范围 [-50, -1]
                left -= cnt[v + BIAS];
                if (left <= 0) { // 找到答案
                    ans[i - k + 1] = v;
                    break;
                }
            }
            cnt[nums[i - k + 1] + BIAS]--; // 离开窗口
        }
        return ans;
    }
};
```

```go [sol-Go]
func getSubarrayBeauty(nums []int, k, x int) []int {
    const bias = 50
    cnt := [bias*2 + 1]int{}
    for _, num := range nums[:k-1] { // 先往窗口内添加 k-1 个数
        cnt[num+bias]++
    }

    ans := make([]int, len(nums)-k+1)
    for i, num := range nums[k-1:] {
        cnt[num+bias]++ // 进入窗口（保证窗口有恰好 k 个数）
        left := x // 从 x 开始倒着减，减到 <= 0 就找到了答案
        for v, c := range cnt[:bias] { // 暴力枚举负数范围 [-50, -1]
            left -= c
            if left <= 0 { // 找到答案
                ans[i] = v - bias
                break
            }
        }
        cnt[nums[i]+bias]-- // 离开窗口
    }
    return ans
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(nU)$，其中 $n$ 为 $\textit{nums}$ 的长度，$U=50$。
- 空间复杂度：$\mathcal{O}(U)$。

## 附：有序集合写法

```py [sol-Python3]
class Solution:
    def getSubarrayBeauty(self, nums: List[int], k: int, x: int) -> List[int]:
        sl = SortedList(nums[:k - 1])  # SortedList 来自 sortedcontainers
        ans = []
        for in_, out in zip(nums[k - 1:], nums):
            sl.add(in_)  # 进入窗口（保证窗口有恰好 k 个数）
            ans.append(min(sl[x - 1], 0))
            sl.discard(out)  # 离开窗口（也可以写 remove）
        return ans
```

```cpp [sol-C++]
#include <ext/pb_ds/assoc_container.hpp>

using namespace __gnu_pbds;

// 使用 pair<value, index> 支持重复元素
using ordered_set = tree<pair<int, int>, null_type, less<>, rb_tree_tag, tree_order_statistics_node_update>;

class Solution {
public:
    vector<int> getSubarrayBeauty(vector<int>& nums, int k, int x) {
        ordered_set st;
        for (int i = 0; i < k - 1; i++) {
            st.insert({nums[i], i});
        }

        int n = nums.size();
        vector<int> ans(n - k + 1);
        for (int i = k - 1; i < n; i++) {
            st.insert({nums[i], i});
            auto [v, _] = *st.find_by_order(x - 1); // 第 x 小
            ans[i - k + 1] = min(v, 0);
            st.erase({nums[i - k + 1], i - k + 1});
        }
        return ans;
    }
};
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log k)$，其中 $n$ 为 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(k)$。

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

本题来自 `一、定长滑动窗口 / §1.2 进阶（选做）`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、定长滑动窗口 / §1.2 进阶（选做）`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
