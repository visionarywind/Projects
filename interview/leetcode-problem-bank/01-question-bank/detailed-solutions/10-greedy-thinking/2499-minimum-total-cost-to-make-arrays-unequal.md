# 2499. 让数组不相等的最小总代价

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-total-cost-to-make-arrays-unequal/
- 题目 slug：`minimum-total-cost-to-make-arrays-unequal`
- 来源专题：贪心与思维
- 来源分类路径：五、思维题 / §5.1 从特殊到一般
- 难度分：2633
- 外部题解来源：https://leetcode.cn/problems/minimum-total-cost-to-make-arrays-unequal/solutions/2015672/li-yong-nums10-tan-xin-zhao-bu-deng-yu-z-amvw/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[思维题 + 分类讨论（Python/Java/C++/Go）](https://leetcode.cn/problems/minimum-total-cost-to-make-arrays-unequal/solutions/2015672/li-yong-nums10-tan-xin-zhao-bu-deng-yu-z-amvw/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`li-yong-nums10-tan-xin-zhao-bu-deng-yu-z-amvw`
- topic id：`2015672`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:07:12 +0800

[视频讲解](https://www.bilibili.com/video/BV1kR4y1r7Df/) 已出炉，欢迎点赞三连，在评论区分享你对这场双周赛的看法~

---

统计满足 $x=\textit{nums}_1[i]=\textit{nums}_2[i]$ 的数对的个数 $\textit{swapCnt}$，以及 $x$ 的众数 $\textit{mode}$ 及其出现次数 $\textit{modeCnt}$。

分类讨论：
- 如果 $\textit{modeCnt}$ 没有超过 $\textit{swapCnt}$ 的一半：
  - 如果 $\textit{swapCnt}$ 是偶数，那么两两交换即可；
  - 如果 $\textit{swapCnt}$ 是奇数，那么至少有三种不同的 $x$，其中一个数必然可以和 $\textit{nums}_1[0]$ 交换；
  - 因此这种情况下，代价就是这些 $x$ 的下标之和。
- 如果 $\textit{modeCnt}$ 超过 $\textit{swapCnt}$ 的一半，或者说 $\textit{modeCnt}\cdot 2 > \textit{swapCnt}$，根据鸽巢原理，无法通过重排这些数字，让数组不相等（因为还存在一些 $\textit{mode}$ 仍然相同）。这种情况必须不断寻找其他的满足 $\textit{nums}_1[j]\ne\textit{nums}_2[j]$ 的数对，且数对中的数都不等于 $\textit{mode}$，直到 $\textit{modeCnt}\cdot 2 \le \textit{swapCnt}$ 为止。为了让答案尽量小，应从左到右遍历数组。如果仍然无法满足要求，则返回 $-1$。

```py [sol1-Python3]
class Solution:
    def minimumTotalCost(self, nums1: List[int], nums2: List[int]) -> int:
        ans = swap_cnt = mode_cnt = mode = 0
        cnt = [0] * (len(nums1) + 1)
        for i, (x, y) in enumerate(zip(nums1, nums2)):
            if x == y:
                ans += i
                swap_cnt += 1
                cnt[x] += 1
                if cnt[x] > mode_cnt:
                    mode_cnt, mode = cnt[x], x

        for i, (x, y) in enumerate(zip(nums1, nums2)):
            if mode_cnt * 2 <= swap_cnt: break
            if x != y and x != mode and y != mode:
                ans += i
                swap_cnt += 1
        return ans if mode_cnt * 2 <= swap_cnt else -1
```

```java [sol1-Java]
class Solution {
    public long minimumTotalCost(int[] nums1, int[] nums2) {
        long ans = 0L;
        int swapCnt = 0, modeCnt = 0, mode = 0, n = nums1.length;
        int[] cnt = new int[n + 1];
        for (int i = 0; i < n; ++i) {
            int x = nums1[i];
            if (x == nums2[i]) {
                ans += i;
                ++swapCnt;
                ++cnt[x];
                if (cnt[x] > modeCnt) {
                    modeCnt = cnt[x];
                    mode = x;
                }
            }
        }

        for (int i = 0; i < n && modeCnt * 2 > swapCnt; ++i) {
            int x = nums1[i], y = nums2[i];
            if (x != y && x != mode && y != mode) {
                ans += i;
                ++swapCnt;
            }
        }
        return modeCnt * 2 > swapCnt ? -1 : ans;
    }
}
```

```cpp [sol1-C++]
class Solution {
public:
    long long minimumTotalCost(vector<int> &nums1, vector<int> &nums2) {
        long ans = 0L;
        int swap_cnt = 0, mode_cnt = 0, mode, n = nums1.size(), cnt[n + 1];
        memset(cnt, 0, sizeof(cnt));
        for (int i = 0; i < n; ++i)
            if (int x = nums1[i]; x == nums2[i]) {
                ans += i;
                ++swap_cnt;
                ++cnt[x];
                if (cnt[x] > mode_cnt) {
                    mode_cnt = cnt[x];
                    mode = x;
                }
            }

        for (int i = 0; i < n && mode_cnt * 2 > swap_cnt; ++i) {
            int x = nums1[i], y = nums2[i];
            if (x != y && x != mode && y != mode) {
                ans += i;
                ++swap_cnt;
            }
        }
        return mode_cnt * 2 > swap_cnt ? -1 : ans;
    }
};
```

```go [sol1-Go]
func minimumTotalCost(nums1, nums2 []int) (ans int64) {
	var swapCnt, modeCnt, mode int
	cnt := make([]int, len(nums1)+1)
	for i, x := range nums1 {
		if x == nums2[i] {
			ans += int64(i)
			swapCnt++
			cnt[x]++
			if cnt[x] > modeCnt {
				modeCnt, mode = cnt[x], x
			}
		}
	}

	for i, x := range nums1 {
		if modeCnt*2 <= swapCnt {
			break
		}
		if x != nums2[i] && x != mode && nums2[i] != mode {
			ans += int64(i)
			swapCnt++
		}
	}
	if modeCnt*2 > swapCnt {
		return -1
	}
	return
}
```

#### 复杂度分析

- 时间复杂度：$O(n)$，其中 $n$ 为 $\textit{nums}_1$ 的长度。
- 空间复杂度：$O(n)$。还可以用**摩尔投票算法**做到 $O(1)$，见 [169. 多数元素](https://leetcode.cn/problems/majority-element/)。

## 本地原创解析

### 1. 题意重述

本题来自 `五、思维题 / §5.1 从特殊到一般`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `五、思维题 / §5.1 从特殊到一般`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
