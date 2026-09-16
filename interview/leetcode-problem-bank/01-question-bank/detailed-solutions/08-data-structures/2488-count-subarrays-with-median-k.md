# 2488. 统计中位数为 K 的子数组

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/count-subarrays-with-median-k/
- 题目 slug：`count-subarrays-with-median-k`
- 来源专题：常用数据结构
- 来源分类路径：一、前缀和 / §1.2 前缀和与哈希表
- 难度分：1999
- 外部题解来源：https://leetcode.cn/problems/count-subarrays-with-median-k/solutions/1993439/deng-jie-zhuan-huan-pythonjavacgo-by-end-5w11/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[转化成 560 题，一次遍历（Python/Java/C++/Go）](https://leetcode.cn/problems/count-subarrays-with-median-k/solutions/1993439/deng-jie-zhuan-huan-pythonjavacgo-by-end-5w11/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`deng-jie-zhuan-huan-pythonjavacgo-by-end-5w11`
- topic id：`1993439`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

$\textit{nums}$ 是一个排列。根据中位数的定义，如果子数组中「大于 $k$ 的元素个数」等于「小于 $k$ 的元素个数」，或者多一个，那么 $k$ 就是中位数。

也就是「大于 $k$ 的元素个数」减去「小于 $k$ 的元素个数」等于 $0$ 或者 $1$。前者对应奇数长度子数组，后者对应偶数长度子数组。

把大于 $k$ 的数看成 $1$，小于 $k$ 的数看成 $-1$，等于 $k$ 的数看成 $0$，得到一个新的数组 $a$，问题变成：

- 计算 $a$ 中包含 $0$ 且和为 $0$ 或者 $1$ 的子数组个数。

根据 [560. 和为 K 的子数组](https://leetcode.cn/problems/subarray-sum-equals-k/) 的做法：

- 统计 $k$ 左边的前缀和及其出现次数（包含空前缀 $0$），记在哈希表中。
- 对于 $k$ 及其右边的前缀和 $\textit{sum}$，去哈希表中找 $\textit{sum}$ 或者 $\textit{sum}-1$ 的出现次数。前者对应和为 $0$ 的子数组，后者对应和为 $1$ 的子数组（因为 $\textit{sum} - (\textit{sum}-1) = 1$）。

### 答疑

**问**：答案的大小看上去是 $\mathcal{O}(n^2)$，这会不会超过 $32$ 位整数最大值？

**答**：这是个有趣的问题，怎么构造能让答案尽量大呢？

- 既然 $k$ 是中位数，不妨取 $k=\left\lfloor\dfrac{n}{2}\right\rfloor$ 且位于 $\textit{nums}$ 中间，从而让尽量多的子数组包含 $k$；
- 小于 $k$ 和大于 $k$ 的数**交替排布**，让 $a$ 的前缀和尽量接近 $0$。

按照算法，$k$ 左边有 $\dfrac{n}{4}$ 个前缀和等于 $0$，$\dfrac{n}{4}$ 个等于 $-1$；右边也有 $\dfrac{n}{4}$ 个前缀和等于 $0$，$\dfrac{n}{4}$ 个等于 $-1$。答案为

$$
\dfrac{n}{4}\cdot\left(\dfrac{n}{4} + \dfrac{n}{4}\right) + \dfrac{n}{4}\cdot \dfrac{n}{4} =  \dfrac{3n^2}{16}
$$

代入 $n=10^5$ 得 $1.875\times 10^9 < 2^{31}  \approx  2.1\times 10^9$，所以不会超过 $32$ 位整数最大值。

## 写法一：哈希表

```py [sol-Python3]
class Solution:
    def countSubarrays(self, nums: List[int], k: int) -> int:
        cnt = defaultdict(int)  # k 左边的前缀和的出现次数
        cnt[0] = 1
        ans = s = 0
        found_k = False
        for x in nums:
            if x == k:
                found_k = True
            elif x < k:
                s -= 1
            else:
                s += 1
            if not found_k:
                cnt[s] += 1  # 统计 k 左边的前缀和的出现次数
            else:
                ans += cnt[s] + cnt[s - 1]
        return ans
```

```java [sol-Java]
class Solution {
    public int countSubarrays(int[] nums, int k) {
        Map<Integer, Integer> cnt = new HashMap<>(); // k 左边的前缀和的出现次数
        cnt.put(0, 1);
        int sum = 0;
        int ans = 0;
        boolean foundK = false;
        for (int x : nums) {
            if (x == k) {
                foundK = true;
            } else if (x < k) {
                sum--;
            } else {
                sum++;
            }
            if (!foundK) {
                cnt.merge(sum, 1, Integer::sum); // 统计 k 左边的前缀和的出现次数
            } else {
                ans += cnt.getOrDefault(sum, 0) + cnt.getOrDefault(sum - 1, 0);
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int countSubarrays(vector<int>& nums, int k) {
        unordered_map<int, int> cnt = {{0, 1}}; // k 左边的前缀和的出现次数
        int ans = 0, sum = 0;
        bool found_k = false;
        for (int x : nums) {
            if (x == k) {
                found_k = true;
            } else if (x < k) {
                sum--;
            } else {
                sum++;
            }
            if (!found_k) {
                cnt[sum]++; // 统计 k 左边的前缀和的出现次数
            } else {
                ans += cnt[sum] + cnt[sum - 1];
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func countSubarrays(nums []int, k int) (ans int) {
	cnt := map[int]int{0: 1} // k 左边的前缀和的出现次数
	sum := 0
	foundK := false
	for _, x := range nums {
		if x == k {
			foundK = true
		} else if x < k {
			sum--
		} else {
			sum++
		}
		if !foundK {
			cnt[sum]++ // 统计 k 左边的前缀和的出现次数
		} else {
			ans += cnt[sum] + cnt[sum-1]
		}
	}
	return
}
```

## 写法二：数组

为避免出现负数下标，$\textit{sum}$ 初始化为 $n$。

```py [sol-Python3]
class Solution:
    def countSubarrays(self, nums: List[int], k: int) -> int:
        s = n = len(nums)
        ans = 0
        cnt = [0] * (n * 2)
        cnt[n] = 1
        found_k = False
        for x in nums:
            if x == k:
                found_k = True
            elif x < k:
                s -= 1
            else:
                s += 1
            if not found_k:
                cnt[s] += 1  # 统计 k 左边的前缀和的出现次数
            else:
                ans += cnt[s] + cnt[s - 1]
        return ans
```

```java [sol-Java]
class Solution {
    public int countSubarrays(int[] nums, int k) {
        int n = nums.length;
        int[] cnt = new int[n * 2];
        cnt[n] = 1;
        int sum = n;
        int ans = 0;
        boolean foundK = false;
        for (int x : nums) {
            if (x == k) {
                foundK = true;
            } else if (x < k) {
                sum--;
            } else {
                sum++;
            }
            if (!foundK) {
                cnt[sum]++; // 统计 k 左边的前缀和的出现次数
            } else {
                ans += cnt[sum] + cnt[sum - 1];
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int countSubarrays(vector<int>& nums, int k) {
        int n = nums.size();
        vector<int> cnt(n * 2);
        cnt[n] = 1;
        int ans = 0, sum = n;
        bool found_k = false;
        for (int x : nums) {
            if (x == k) {
                found_k = true;
            } else if (x < k) {
                sum--;
            } else {
                sum++;
            }
            if (!found_k) {
                cnt[sum]++; // 统计 k 左边的前缀和的出现次数
            } else {
                ans += cnt[sum] + cnt[sum - 1];
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func countSubarrays(nums []int, k int) (ans int) {
	n := len(nums)
	cnt := make([]int, n*2)
	cnt[n] = 1
	sum := n
	foundK := false
	for _, x := range nums {
		if x == k {
			foundK = true
		} else if x < k {
			sum--
		} else {
			sum++
		}
		if !foundK {
			cnt[sum]++ // 统计 k 左边的前缀和的出现次数
		} else {
			ans += cnt[sum] + cnt[sum-1]
		}
	}
	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

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

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

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
