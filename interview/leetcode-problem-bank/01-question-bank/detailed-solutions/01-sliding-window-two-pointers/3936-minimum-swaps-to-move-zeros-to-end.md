# 3936. 将 0 移到末尾的最少交换次数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-swaps-to-move-zeros-to-end/
- 题目 slug：`minimum-swaps-to-move-zeros-to-end`
- 来源专题：滑动窗口与双指针
- 来源分类路径：三、单序列双指针 / §3.2 相向双指针
- 难度分：1346
- 外部题解来源：https://leetcode.cn/problems/minimum-swaps-to-move-zeros-to-end/solutions/3973289/liang-chong-fang-fa-liang-ci-bian-li-yi-8tpos/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：两次遍历 / 一次遍历（Python/Java/C++/Go）](https://leetcode.cn/problems/minimum-swaps-to-move-zeros-to-end/solutions/3973289/liang-chong-fang-fa-liang-ci-bian-li-yi-8tpos/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`liang-chong-fang-fa-liang-ci-bian-li-yi-8tpos`
- topic id：`3973289`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 方法一：两次遍历

遍历 $\textit{nums}$，统计 $\textit{nums}$ 中的非零元素个数，记作 $\textit{cnt}$。

那么 $\textit{nums}$ 的前 $\textit{cnt}$ 个数中的 $0$，要和 $\textit{nums}$ 的后 $n-\textit{cnt}$ 个数中的非零元素交换。答案为 $\textit{nums}$ 的前 $\textit{cnt}$ 个数中的 $0$ 的个数。

[本题视频讲解](https://www.bilibili.com/video/BV1iuG76VEXy/)，欢迎点赞关注~

```py [sol-Python3]
class Solution:
    def minimumSwaps(self, nums: list[int]) -> int:
        cnt = sum(x != 0 for x in nums) 
        return sum(x == 0 for x in nums[:cnt]) 
```

```java [sol-Java]
class Solution {
    public int minimumSwaps(int[] nums) {
        int cnt = 0;
        for (int x : nums) {
            if (x != 0) {
                cnt++;
            }
        }

        int ans = 0;
        for (int i = 0; i < cnt; i++) {
            if (nums[i] == 0) {
                ans++;
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minimumSwaps(vector<int>& nums) {
        int cnt = 0;
        for (int x : nums) {
            cnt += x != 0;
        }

        int ans = 0;
        for (int i = 0; i < cnt; i++) {
            ans += nums[i] == 0;
        }
        return ans;
    }
};
```

```go [sol-Go]
func minimumSwaps(nums []int) (ans int) {
	cnt := 0
	for _, x := range nums {
		if x != 0 {
			cnt++
		}
	}

	for _, x := range nums[:cnt] {
		if x == 0 {
			ans++
		}
	}

	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

## 方法二：相向双指针

如果 $\textit{nums}[0] \ne 0$，无需交换，问题变成剩余 $n-1$ 个数的子问题。

如果 $\textit{nums}[n-1] = 0$，无需交换，问题变成剩余 $n-1$ 个数的子问题。

如果 $\textit{nums}[0] = 0$ 且 $\textit{nums}[n-1] \ne 0$，必须交换，问题变成剩余 $n-2$ 个数的子问题。

用两个指针 $\ell$ 和 $r$ 表示 $\textit{nums}$ 的剩余元素为子数组 $[\ell, r]$。

```py [sol-Python3]
class Solution:
    def minimumSwaps(self, nums: list[int]) -> int:
        ans = 0
        l, r = 0, len(nums) - 1
        while l < r:
            if nums[l] != 0:
                l += 1
            elif nums[r] == 0:
                r -= 1
            else:
                # 交换 nums[l] 和 nums[r]
                ans += 1
                l += 1
                r -= 1
        return ans
```

```java [sol-Java]
class Solution {
    public int minimumSwaps(int[] nums) {
        int ans = 0;
        int l = 0;
        int r = nums.length - 1;
        while (l < r) {
            if (nums[l] != 0) {
                l++;
            } else if (nums[r] == 0) {
                r--;
            } else {
                // 交换 nums[l] 和 nums[r]
                ans++;
                l++;
                r--;
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minimumSwaps(vector<int>& nums) {
        int ans = 0;
        int l = 0, r = nums.size() - 1;
        while (l < r) {
            if (nums[l] != 0) {
                l++;
            } else if (nums[r] == 0) {
                r--;
            } else {
                // 交换 nums[l] 和 nums[r]
                ans++;
                l++;
                r--;
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func minimumSwaps(nums []int) (ans int) {
	l, r := 0, len(nums)-1
	for l < r {
		if nums[l] != 0 {
			l++
		} else if nums[r] == 0 {
			r--
		} else {
			// 交换 nums[l] 和 nums[r]
			ans++
			l++
			r--
		}
	}
	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

## 专题训练

见下面双指针题单的「**§3.2 相向双指针**」。

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

本题来自 `三、单序列双指针 / §3.2 相向双指针`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `三、单序列双指针 / §3.2 相向双指针`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
