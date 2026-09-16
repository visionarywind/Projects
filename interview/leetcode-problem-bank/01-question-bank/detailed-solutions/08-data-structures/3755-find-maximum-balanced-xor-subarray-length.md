# 3755. 最大平衡异或子数组的长度

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/find-maximum-balanced-xor-subarray-length/
- 题目 slug：`find-maximum-balanced-xor-subarray-length`
- 来源专题：常用数据结构
- 来源分类路径：一、前缀和 / §1.2 前缀和与哈希表
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/find-maximum-balanced-xor-subarray-length/solutions/3839716/qian-zhui-he-yu-ha-xi-biao-pythonjavacgo-2504/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[前缀和与哈希表（Python/Java/C++/Go）](https://leetcode.cn/problems/find-maximum-balanced-xor-subarray-length/solutions/3839716/qian-zhui-he-yu-ha-xi-biao-pythonjavacgo-2504/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`qian-zhui-he-yu-ha-xi-biao-pythonjavacgo-2504`
- topic id：`3839716`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 处理异或和

计算 $\textit{nums}$ 的**前缀异或和** $s$。关于 $s$ 数组的定义，请看 [前缀和](https://leetcode.cn/problems/range-sum-query-immutable/solution/qian-zhui-he-ji-qi-kuo-zhan-fu-ti-dan-py-vaar/)。

子数组 $[l,r)$ 的异或和即 $s[r]\oplus s[l]$。

$s[r]\oplus s[l] = 0$ 意味着 $s[r] = s[l]$。

## 处理奇偶

请先完成这题：[525. 连续数组](https://leetcode.cn/problems/contiguous-array/)。

525 题告诉我们，可以把奇数视作 $1$，偶数视作 $-1$，得到数组 $b$。

奇数偶数数量相等，等价于奇数个数减去偶数个数为 $0$，等价于 $b$ 的子数组和等于 $0$。这同样可以用前缀和处理。

## 合二为一

如果只要求前缀和异或和相同，做法类似 [1512. 好数对的数目](https://leetcode.cn/problems/number-of-good-pairs/)。

本题有两个要求，把哈希表的 key **从 int 改成 pair** 即可。

为了计算子数组长度，用一个哈希表记录二元组 $(\textit{xor}, \textit{diff})$ 首次出现的位置，其中：

- $\textit{xor}$ 表示 $\textit{nums}$ 的前缀异或和。
- $\textit{diff}$ 表示 $b$ 的前缀和。

遍历 $\textit{nums}$：

- 如果在哈希表中查询到二元组，那么用当前下标减去哈希表中保存的下标，即为子数组长度，更新答案的最大值。
- 否则，把二元组及当前下标保存到哈希表中。

```py [sol-Python3]
class Solution:
    def maxBalancedSubarray(self, nums: List[int]) -> int:
        ans = 0
        pos = {(0, 0): -1}  # 空前缀的位置视作 -1
        xor = diff = 0
        for i, x in enumerate(nums):
            xor ^= x
            diff += 1 if x % 2 else -1
            t = (xor, diff)
            if t in pos:
                ans = max(ans, i - pos[t])
            else:
                pos[t] = i
        return ans
```

```java [sol-Java]
class Solution {
    public int maxBalancedSubarray(int[] nums) {
        int n = nums.length;
        int ans = 0;
        int xor = 0;
        int diff = n; // 保证 diff 非负
        Map<Long, Integer> pos = new HashMap<>(n + 1, 1); // 预分配空间
        // 把 xor 和 diff 合并为一个 long
        pos.put((long) xor << 20 | diff, -1); // 空前缀的位置视作 -1
        for (int i = 0; i < n; i++) {
            xor ^= nums[i];
            diff += nums[i] % 2 != 0 ? 1 : -1;
            // 把 xor 和 diff 合并为一个 long
            long key = (long) xor << 20 | diff;
            Integer j = pos.get(key);
            if (j != null) {
                ans = Math.max(ans, i - j);
            } else {
                pos.put(key, i);
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxBalancedSubarray(vector<int>& nums) {
        int n = nums.size();
        int ans = 0, xor_ = 0, diff = n; // 保证 diff 非负
        unordered_map<long long, int> pos;
        // 把 xor_ 和 diff 合并为一个 long long
        pos[1LL * xor_ << 32 | diff] = -1; // 空前缀的位置视作 -1
        for (int i = 0; i < n; i++) {
            xor_ ^= nums[i];
            diff += nums[i] % 2 ? 1 : -1;
            long long key = 1LL * xor_ << 32 | diff;
            if (auto it = pos.find(key); it != pos.end()) {
                ans = max(ans, i - it->second);
            } else {
                pos[key] = i;
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func maxBalancedSubarray(nums []int) (ans int) {
	type pair struct{ xor, diff int }
	pos := map[pair]int{{}: -1} // 空前缀的位置视作 -1
	p := pair{}
	for i, x := range nums {
		p.xor ^= x
		p.diff += x%2*2 - 1
		if j, ok := pos[p]; ok {
			ans = max(ans, i-j)
		} else {
			pos[p] = i
		}
	}
	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 相似题目

- [3728. 边界与内部和相等的稳定子数组](https://leetcode.cn/problems/stable-subarrays-with-equal-boundary-and-interior-sum/) 1909
- [3714. 最长的平衡子串 II](https://leetcode.cn/problems/longest-balanced-substring-ii/) 2202
- [2949. 统计美丽子字符串 II](https://leetcode.cn/problems/count-beautiful-substrings-ii/) 2445

## 专题训练

见下面数据结构题单的「**§1.2 前缀和与哈希表**」。

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

## 本地原创解析

### 1. 题意重述

本题来自 `一、前缀和 / §1.2 前缀和与哈希表`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、前缀和 / §1.2 前缀和与哈希表`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
