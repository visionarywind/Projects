# 3825. 按位与结果非零的最长上升子序列

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/longest-strictly-increasing-subsequence-with-non-zero-bitwise-and/
- 题目 slug：`longest-strictly-increasing-subsequence-with-non-zero-bitwise-and`
- 来源专题：动态规划
- 来源分类路径：四、经典线性 DP / §4.2 最长递增子序列（LIS） / §4.2.2 进阶
- 难度分：1846
- 外部题解来源：https://leetcode.cn/problems/longest-strictly-increasing-subsequence-with-non-zero-bitwise-and/solutions/3893548/mei-ju-bi-te-wei-lispythonjavacgo-by-end-9y3b/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[枚举比特位 + LIS（Python/Java/C++/Go）](https://leetcode.cn/problems/longest-strictly-increasing-subsequence-with-non-zero-bitwise-and/solutions/3893548/mei-ju-bi-te-wei-lispythonjavacgo-by-end-9y3b/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`mei-ju-bi-te-wei-lispythonjavacgo-by-end-9y3b`
- topic id：`3893548`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

题目要求子序列的 AND 结果非零，也就是说，AND 结果的某个比特位一定是 $1$。

**枚举**这个比特位。

示例 1 的 $\textit{nums}=[5,4,7]$，二进制长度最多为 $3$。

- 如果 AND 结果的最低位是 $1$，那么只有 $\textit{nums}$ 中的 $[5,7]$ 能在子序列中，问题变成 $[5,7]$ 的 [300. 最长递增子序列](https://leetcode.cn/problems/longest-increasing-subsequence/)。注意本题 $n\le 10^5$，必须用二分优化，见 [我的题解](https://leetcode.cn/problems/longest-increasing-subsequence/solutions/2147040/jiao-ni-yi-bu-bu-si-kao-dpfu-o1-kong-jia-4zma/)。
- 如果 AND 结果的次低位是 $1$，那么只有 $\textit{nums}$ 中的 $[7]$ 能在子序列中。
- 如果 AND 结果的最高位是 $1$，那么 $\textit{nums}$ 中的 $[5,4,7]$ 都能在子序列中。

设 $\max(\textit{nums})$ 的二进制长度为 $w$，枚举 $i=0,1,2,\ldots,w-1$，对 $\textit{nums}$ 中的二进制第 $i$ 位是 $1$ 的数，计算最长递增子序列。

> **注**：也可以只枚举 $\textit{nums}$ 所有元素的 OR 中的 $1$。

下午两点 [B站@灵茶山艾府](https://space.bilibili.com/206214) 直播讲题，欢迎关注~

```py [sol-Python3]
class Solution:
    def longestSubsequence(self, nums: List[int]) -> int:
        ans = 0
        w = max(nums).bit_length()
        for i in range(w):
            bit = 1 << i
            # 300. 最长递增子序列
            f = []
            for x in nums:
                if x & bit == 0:  # x 二进制的第 i 位是 0
                    continue
                j = bisect_left(f, x)
                if j < len(f):
                    f[j] = x
                else:
                    f.append(x)
            ans = max(ans, len(f))
        return ans
```

```java [sol-Java]
class Solution {
    public int longestSubsequence(int[] nums) {
        int mx = 0;
        for (int x : nums) {
            mx = Math.max(mx, x);
        }

        int w = 32 - Integer.numberOfLeadingZeros(mx);
        int ans = 0;

        // 数组比 ArrayList 快，读者可以对比另一份代码【Java ArrayList】
        int[] f = new int[nums.length];

        for (int i = 0; i < w; i++) {
            // 300. 最长递增子序列
            int size = 0;
            for (int x : nums) {
                if ((x >> i & 1) == 0) { // x 二进制的第 i 位是 0
                    continue;
                }

                int j = Arrays.binarySearch(f, 0, size, x);
                if (j < 0) {
                    j = ~j;
                }

                f[j] = x;
                if (j == size) {
                    size++;
                }
            }
            ans = Math.max(ans, size);
        }

        return ans;
    }
}
```

```java [sol-Java ArrayList]
class Solution {
    public int longestSubsequence(int[] nums) {
        int mx = 0;
        for (int x : nums) {
            mx = Math.max(mx, x);
        }

        int w = 32 - Integer.numberOfLeadingZeros(mx);
        int ans = 0;

        for (int i = 0; i < w; i++) {
            // 300. 最长递增子序列
            List<Integer> f = new ArrayList<>();
            for (int x : nums) {
                if ((x >> i & 1) == 0) { // x 二进制的第 i 位是 0
                    continue;
                }

                int j = Collections.binarySearch(f, x);
                if (j < 0) {
                    j = ~j;
                }

                if (j < f.size()) {
                    f.set(j, x);
                } else {
                    f.add(x);
                }
            }
            ans = Math.max(ans, f.size());
        }

        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int longestSubsequence(vector<int>& nums) {
        int ans = 0;
        int w = bit_width((uint32_t) ranges::max(nums));
        for (int i = 0; i < w; i++) {
            // 300. 最长递增子序列
            vector<int> f;
            for (int x : nums) {
                if ((x >> i & 1) == 0) { // x 二进制的第 i 位是 0
                    continue;
                }
                auto it = ranges::lower_bound(f, x);
                if (it != f.end()) {
                    *it = x;
                } else {
                    f.push_back(x);
                }
            }
            ans = max(ans, (int) f.size());
        }
        return ans;
    }
};
```

```go [sol-Go]
func longestSubsequence(nums []int) (ans int) {
	w := bits.Len(uint(slices.Max(nums)))
	for i := range w {
		// 300. 最长递增子序列
		f := []int{}
		for _, x := range nums {
			if x>>i&1 == 0 { // x 二进制的第 i 位是 0
				continue
			}
			j := sort.SearchInts(f, x)
			if j < len(f) {
				f[j] = x
			} else {
				f = append(f, x)
			}
		}
		ans = max(ans, len(f))
	}
	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n \log U)$，其中 $n$ 是 $\textit{nums}$ 的长度，$U=\max(\textit{nums})$。
- 空间复杂度：$\mathcal{O}(n)$。

## 专题训练

见下面动态规划题单的「**§4.2 最长递增子序列**」。

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

本题来自 `四、经典线性 DP / §4.2 最长递增子序列（LIS） / §4.2.2 进阶`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `四、经典线性 DP / §4.2 最长递增子序列（LIS） / §4.2.2 进阶`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
