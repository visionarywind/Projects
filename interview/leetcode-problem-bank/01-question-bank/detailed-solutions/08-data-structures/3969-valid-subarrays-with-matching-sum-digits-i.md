# 3969. 求和后首尾数字相同的有效子数组 I

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/valid-subarrays-with-matching-sum-digits-i/
- 题目 slug：`valid-subarrays-with-matching-sum-digits-i`
- 来源专题：常用数据结构
- 来源分类路径：一、前缀和 / §1.2 前缀和与哈希表
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/valid-subarrays-with-matching-sum-digits-i/solutions/3986177/onlogs-zuo-fa-qian-zhui-he-hua-dong-chua-y1o3/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[O(nlogS) 做法：前缀和 + 滑动窗口（Python/Java/C++/Go）](https://leetcode.cn/problems/valid-subarrays-with-matching-sum-digits-i/solutions/3986177/onlogs-zuo-fa-qian-zhui-he-hua-dong-chua-y1o3/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`onlogs-zuo-fa-qian-zhui-he-hua-dong-chua-y1o3`
- topic id：`3986177`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 16:46:02 +0800

## 方法一：暴力枚举

枚举子数组的左右端点。

在枚举右端点的同时，维护子数组的元素和 $s$。

$s\bmod 10$ 即 $s$ 的个位数。

不断把 $s$ 除以 $10$（下取整），直到 $s\le 9$，此时的 $s$ 即为 $s$ 的最高位数字。

```py [sol-Python3]
class Solution:
    def countValidSubarrays(self, nums: list[int], x: int) -> int:
        ans = 0

        # 枚举子数组的左右端点
        for i in range(len(nums)):
            s = 0
            for v in nums[i:]:
                s += v
                # 计算 s 的最低位
                if s % 10 != x:
                    continue
                # 计算 s 的最高位
                t = s
                while t > 9:
                    t //= 10
                if t == x:
                    ans += 1

        return ans
```

```java [sol-Java]
class Solution {
    public int countValidSubarrays(int[] nums, int x) {
        int ans = 0;

        // 枚举子数组的左右端点
        for (int i = 0; i < nums.length; i++) {
            long sum = 0;
            for (int j = i; j < nums.length; j++) {
                sum += nums[j];
                // 计算 sum 的最低位
                if (sum % 10 != x) {
                    continue;
                }
                // 计算 sum 的最高位
                long s = sum;
                while (s > 9) {
                    s /= 10;
                }
                if (s == x) {
                    ans++;
                }
            }
        }

        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int countValidSubarrays(vector<int>& nums, int x) {
        int n = nums.size();
        int ans = 0;

        // 枚举子数组的左右端点
        for (int i = 0; i < n; i++) {
            long long sum = 0;
            for (int j = i; j < n; j++) {
                sum += nums[j];
                // 计算 sum 的最低位
                if (sum % 10 != x) {
                    continue;
                }
                // 计算 sum 的最高位
                auto s = sum;
                while (s > 9) {
                    s /= 10;
                }
                if (s == x) {
                    ans++;
                }
            }
        }

        return ans;
    }
};
```

```go [sol-Go]
func countValidSubarrays(nums []int, x int) (ans int) {
	// 枚举子数组的左右端点
	for i := range nums {
		sum := 0
		for _, v := range nums[i:] {
			sum += v
			// 计算 sum 的最低位
			if sum%10 != x {
				continue
			}
			// 计算 sum 的最高位
			s := sum
			for s > 9 {
				s /= 10
			}
			if s == x {
				ans++
			}
		}
	}
	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2\log S)$，其中 $n$ 是 $\textit{nums}$ 的长度，$S$ 是 $\textit{nums}$ 中的所有元素之和。
- 空间复杂度：$\mathcal{O}(1)$。

## 方法二：前缀和 + 三指针滑动窗口

**前置题目**：[974. 和可被 K 整除的子数组](https://leetcode.cn/problems/subarray-sums-divisible-by-k/)。

枚举子数组和的十进制长度。

例如，子数组和的十进制长度为 $3$，$x=5$，那么问题相当于

- 计算子数组和在 $[500, 599]$ 中，且子数组和的个位数是 $x=5$ 的子数组个数。

设 $\textit{nums}$ 的**前缀和**数组为 $s$。关于 $s$ 数组的定义，请看 [前缀和](https://leetcode.cn/problems/range-sum-query-immutable/solution/qian-zhui-he-ji-qi-kuo-zhan-fu-ti-dan-py-vaar/)。

子数组 $[\ell,r)$ 的元素和为 

$$
s[r] - s[\ell]
$$

子数组和的个位数是 $x$，等价于

$$
(s[r] - s[\ell])\bmod 10 = x
$$

根据 [模运算的世界：当加减乘除遇上取模](https://leetcode.cn/circle/discuss/mDfnkW/) 中的**同余**理论，枚举 $r$，我们需要计算满足

$$
s[\ell] \bmod 10 = (s[r]-x)\bmod 10
$$

的 $s[\ell]$ 的个数。

> 注意上式等号右边有减法，代码实现时，要保证取模结果非负。

本题元素值非负，可以用**滑动窗口**。在枚举 $r$ 的过程中，用一个滑动窗口维护满足 $x\cdot 10^k\le s[r] - s[\ell] < (x+1)\cdot 10^k$ 的 $s[\ell]\bmod 10$ 的个数。 

> 如果元素值有负数，则需要用有序集合。

[本题视频讲解](https://www.bilibili.com/video/BV1uqjt6zEMT/)，欢迎点赞关注~

```py [sol-Python3]
class Solution:
    def countValidSubarrays(self, nums: list[int], x: int) -> int:
        pre = list(accumulate(nums, initial=0))
        ans = 0

        # 枚举子数组和的十进制长度
        low, high = x, x + 1
        while low <= pre[-1]:
            # 计算子数组和在 [low, high-1] 中，且子数组和模 10 为 x 的子数组个数
            cnt = [0] * 10
            left1 = left2 = 0
            for s in pre:
                # 随着 s 的增大，<= s-high 的前缀和离开窗口，<= s-low 的前缀和进入窗口
                while pre[left1] <= s - high:
                    cnt[pre[left1] % 10] -= 1
                    left1 += 1
                while pre[left2] <= s - low:
                    cnt[pre[left2] % 10] += 1
                    left2 += 1
                ans += cnt[(s - x) % 10]
            low *= 10
            high *= 10

        return ans
```

```java [sol-Java]
class Solution {
    public int countValidSubarrays(int[] nums, int x) {
        int n = nums.length;
        long[] sum = new long[n + 1];
        for (int i = 0; i < n; i++) {
            sum[i + 1] = sum[i] + nums[i];
        }

        int ans = 0;

        // 枚举子数组和的十进制长度
        for (long low = x, high = x + 1; low <= sum[n]; low *= 10, high *= 10) {
            // 计算子数组和在 [low, high-1] 中，且子数组和模 10 为 x 的子数组个数
            int[] cnt = new int[10];
            int left1 = 0;
            int left2 = 0;
            for (long s : sum) {
                // 随着 s 的增大，<= s-high 的前缀和离开窗口，<= s-low 的前缀和进入窗口
                while (sum[left1] <= s - high) {
                    cnt[(int) (sum[left1] % 10)]--;
                    left1++;
                }
                while (sum[left2] <= s - low) {
                    cnt[(int) (sum[left2] % 10)]++;
                    left2++;
                }
                ans += cnt[(int) ((s - x + 10) % 10)];
            }
        }

        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int countValidSubarrays(vector<int>& nums, int x) {
        int n = nums.size();
        vector<long long> sum(n + 1);
        for (int i = 0; i < n; i++) {
            sum[i + 1] = sum[i] + nums[i];
        }

        int ans = 0;

        // 枚举子数组和的十进制长度
        for (long long low = x, high = x + 1; low <= sum[n]; low *= 10, high *= 10) {
            // 计算子数组和在 [low, high-1] 中，且子数组和模 10 为 x 的子数组个数
            int cnt[10]{};
            int left1 = 0, left2 = 0;
            for (auto s : sum) {
                // 随着 s 的增大，<= s-high 的前缀和离开窗口，<= s-low 的前缀和进入窗口
                while (sum[left1] <= s - high) {
                    cnt[sum[left1] % 10]--;
                    left1++;
                }
                while (sum[left2] <= s - low) {
                    cnt[sum[left2] % 10]++;
                    left2++;
                }
                ans += cnt[(s - x + 10) % 10];
            }
        }

        return ans;
    }
};
```

```go [sol-Go]
func countValidSubarrays(nums []int, x int) (ans int) {
	n := len(nums)
	sum := make([]int, n+1)
	for i, v := range nums {
		sum[i+1] = sum[i] + v
	}

	// 枚举子数组和的十进制长度
	for low, high := x, x+1; low <= sum[n]; low, high = low*10, high*10 {
		// 计算子数组和在 [low, high-1] 中，且子数组和模 10 为 x 的子数组个数
		cnt := [10]int{}
		left1, left2 := 0, 0
		for _, s := range sum {
			// 随着 s 的增大，<= s-high 的前缀和离开窗口，<= s-low 的前缀和进入窗口
			for sum[left1] <= s-high {
				cnt[sum[left1]%10]--
				left1++
			}
			for sum[left2] <= s-low {
				cnt[sum[left2]%10]++
				left2++
			}
			ans += cnt[(s-x+10)%10]
		}
	}
	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log S)$，其中 $n$ 是 $\textit{nums}$ 的长度，$S$ 是 $\textit{nums}$ 中的所有元素之和。我们跑了 $\mathcal{O}(\log S)$ 次 $\mathcal{O}(n)$ 的滑动窗口。
- 空间复杂度：$\mathcal{O}(n + D)$。其中 $D=10$。**注**：也可以在滑窗的同时计算前缀和，从而做到 $\mathcal{O}(D)$ 的空间复杂度。

## 专题训练

1. 滑动窗口题单的「**二、不定长滑动窗口**」。
2. 数据结构题单的「**§1.2 前缀和与哈希表**」

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
