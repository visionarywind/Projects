# 1477. 找两个和为目标值且不重叠的子数组

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/find-two-non-overlapping-sub-arrays-each-with-target-sum/
- 题目 slug：`find-two-non-overlapping-sub-arrays-each-with-target-sum`
- 来源专题：常用数据结构
- 来源分类路径：一、前缀和 / §1.2 前缀和与哈希表
- 难度分：1851
- 外部题解来源：https://leetcode.cn/problems/find-two-non-overlapping-sub-arrays-each-with-target-sum/solutions/4022830/liang-chong-fang-fa-qian-hou-zhui-fen-ji-pm1t/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：前后缀分解 / 枚举右维护左（Python/Java/C++/Go）](https://leetcode.cn/problems/find-two-non-overlapping-sub-arrays-each-with-target-sum/solutions/4022830/liang-chong-fang-fa-qian-hou-zhui-fen-ji-pm1t/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`liang-chong-fang-fa-qian-hou-zhui-fen-ji-pm1t`
- topic id：`4022830`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 方法一：前后缀分解

枚举第一个子数组的右端点 $r$，用**滑动窗口**维护第一个子数组，使得子数组的和不超过 $\textit{target}$。如果子数组的和恰好等于 $\textit{target}$，我们需要知道：

- 当第二个子数组的左端点 $\ge r+1$ 时，第二个子数组的最短长度 $\textit{sufMin}[r+1]$。

这个问题同样可以用滑动窗口解决（预处理）。枚举第二个子数组的左端点 $\ell$，从右到左滑窗，把发现的和为 $\textit{target}$ 的子数组的最短长度记录到 $\textit{minLen}$ 中。对于每个 $\ell$，记录此时的 $\textit{sufMin}[\ell] = \textit{minLen}$。

滑动窗口的原理请看视频[【基础算法精讲 03】](https://www.bilibili.com/video/BV1hd4y1r7Gq/)。

```py [sol-Python3]
class Solution:
    def minSumOfLengths(self, arr: List[int], target: int) -> int:
        n = len(arr)
        # suf_min[i] 表示左端点 >= i 的和为 target 的最短子数组长度（不存在则为 inf）
        suf_min = [0] * n
        min_len = inf
        s = 0
        r = n - 1
        for l in range(n - 1, 0, -1):
            s += arr[l]
            while s > target:
                s -= arr[r]
                r -= 1
            if s == target:
                min_len = min(min_len, r - l + 1)  # 维护遍历过的和为 target 的子数组的最短长度
            suf_min[l] = min_len  # 保存此时此刻的 min_len

        ans = inf
        s = l = 0
        for r in range(n - 1):
            s += arr[r]
            while s > target:
                s -= arr[l]
                l += 1
            if s == target:
                ans = min(ans, r - l + 1 + suf_min[r + 1])

        return -1 if ans == inf else ans
```

```java [sol-Java]
class Solution {
    public int minSumOfLengths(int[] arr, int target) {
        int n = arr.length;
        // sufMin[i] 表示左端点 >= i 的和为 target 的最短子数组长度
        // 不存在子数组时，长度设为 n+1
        int[] sufMin = new int[n];
        int minLen = n + 1;
        int sum = 0;
        int r = n - 1;
        for (int l = n - 1; l > 0; l--) {
            sum += arr[l];
            while (sum > target) {
                sum -= arr[r];
                r--;
            }
            if (sum == target) {
                minLen = Math.min(minLen, r - l + 1); // 维护遍历过的和为 target 的子数组的最短长度
            }
            sufMin[l] = minLen; // 保存此时此刻的 minLen
        }

        int ans = n + 1;
        sum = 0;
        int l = 0;
        for (r = 0; r < n - 1; r++) {
            sum += arr[r];
            while (sum > target) {
                sum -= arr[l];
                l++;
            }
            if (sum == target) {
                ans = Math.min(ans, r - l + 1 + sufMin[r + 1]);
            }
        }

        return ans > n ? -1 : ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minSumOfLengths(vector<int>& arr, int target) {
        int n = arr.size();
        // suf_min[i] 表示左端点 >= i 的和为 target 的最短子数组长度
        // 不存在子数组时，长度设为 n+1
        vector<int> suf_min(n);
        int min_len = n + 1;
        int sum = 0;
        int r = n - 1;
        for (int l = n - 1; l > 0; l--) {
            sum += arr[l];
            while (sum > target) {
                sum -= arr[r];
                r--;
            }
            if (sum == target) {
                min_len = min(min_len, r - l + 1); // 维护遍历过的和为 target 的子数组的最短长度
            }
            suf_min[l] = min_len; // 保存此时此刻的 min_len
        }

        int ans = n + 1;
        sum = 0;
        int l = 0;
        for (r = 0; r < n - 1; r++) {
            sum += arr[r];
            while (sum > target) {
                sum -= arr[l];
                l++;
            }
            if (sum == target) {
                ans = min(ans, r - l + 1 + suf_min[r + 1]);
            }
        }

        return ans > n ? -1 : ans;
    }
};
```

```go [sol-Go]
func minSumOfLengths(arr []int, target int) int {
	n := len(arr)
	// sufMin[i] 表示左端点 >= i 的和为 target 的最短子数组长度
	// 不存在子数组时，长度设为 n+1
	sufMin := make([]int, n)
	minLen := n + 1
	sum := 0
	r := n - 1
	for l := n - 1; l > 0; l-- {
		sum += arr[l]
		for sum > target {
			sum -= arr[r]
			r--
		}
		if sum == target {
			minLen = min(minLen, r-l+1) // 维护遍历过的和为 target 的子数组的最短长度
		}
		sufMin[l] = minLen // 保存此时此刻的 minLen
	}

	ans := n + 1
	sum = 0
	l := 0
	for r, x := range arr[:n-1] {
		sum += x
		for sum > target {
			sum -= arr[l]
			l++
		}
		if sum == target {
			ans = min(ans, r-l+1+sufMin[r+1])
		}
	}

	if ans > n {
		return -1
	}
	return ans
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{arr}$ 的长度。对于滑动窗口算法，虽然写的是二重循环，但是内层循环中对下标减一（加一）的**总**执行次数不会超过 $n$ 次，所以二重循环的循环次数是 $\mathcal{O}(n)$ 的。
- 空间复杂度：$\mathcal{O}(n)$。

## 方法二：枚举右维护左

用滑动窗口枚举第二个子数组 $[\ell, r]$，我们需要知道左端点 $< \ell$ 的最短子数组的长度 $\textit{preMin}[\ell]$。

类似方法一，$\textit{preMin}[\ell]$ 可以在滑动窗口的同时维护。

```py [sol-Python3]
class Solution:
    def minSumOfLengths(self, arr: List[int], target: int) -> int:
        n = len(arr)
        ans = inf
        # pre_min[i] 表示右端点 < i 的和为 target 的最短子数组长度（不存在则为 inf）
        pre_min = [inf] * (n + 1)
        min_len = inf
        s = l = 0

        for r, x in enumerate(arr):
            s += x
            while s > target:
                s -= arr[l]
                l += 1
            if s == target:
                # 枚举第二个子数组的右端点为 r，用滑动窗口算出此时第二个子数组的左端点为 l
                # 那么第一个子数组的右端点必须 < l
                ans = min(ans, pre_min[l] + r - l + 1)
                min_len = min(min_len, r - l + 1)  # 维护遍历过的和为 target 的子数组的最短长度
            pre_min[r + 1] = min_len  # 保存此时此刻的 min_len

        return -1 if ans == inf else ans
```

```java [sol-Java]
class Solution {
    public int minSumOfLengths(int[] arr, int target) {
        int n = arr.length;
        int ans = n + 1;
        // preMin[i] 表示右端点 < i 的和为 target 的最短子数组长度
        // 不存在子数组时，长度设为 n+1
        int[] preMin = new int[n + 1];
        preMin[0] = n + 1;
        int minLen = n + 1;
        int sum = 0;
        int l = 0;

        for (int r = 0; r < n; r++) {
            sum += arr[r];
            while (sum > target) {
                sum -= arr[l];
                l++;
            }
            if (sum == target) {
                // 枚举第二个子数组的右端点为 r，用滑动窗口算出此时第二个子数组的左端点为 l
                // 那么第一个子数组的右端点必须 < l
                ans = Math.min(ans, preMin[l] + r - l + 1);
                minLen = Math.min(minLen, r - l + 1); // 维护遍历过的和为 target 的子数组的最短长度
            }
            preMin[r + 1] = minLen; // 保存此时此刻的 minLen
        }

        return ans > n ? -1 : ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minSumOfLengths(vector<int>& arr, int target) {
        int n = arr.size();
        int ans = n + 1;
        // pre_min[i] 表示右端点 < i 的和为 target 的最短子数组长度
        // 不存在子数组时，长度设为 n+1
        vector<int> pre_min(n + 1);
        pre_min[0] = n + 1;
        int min_len = n + 1;
        int sum = 0;
        int l = 0;

        for (int r = 0; r < n; r++) {
            sum += arr[r];
            while (sum > target) {
                sum -= arr[l];
                l++;
            }
            if (sum == target) {
                // 枚举第二个子数组的右端点为 r，用滑动窗口算出此时第二个子数组的左端点为 l
                // 那么第一个子数组的右端点必须 < l
                ans = min(ans, pre_min[l] + r - l + 1);
                min_len = min(min_len, r - l + 1); // 维护遍历过的和为 target 的子数组的最短长度
            }
            pre_min[r + 1] = min_len; // 保存此时此刻的 min_len
        }

        return ans > n ? -1 : ans;
    }
};
```

```go [sol-Go]
func minSumOfLengths(arr []int, target int) int {
	n := len(arr)
	ans := n + 1
	// preMin[i] 表示右端点 < i 的和为 target 的最短子数组长度
	// 不存在子数组时，长度设为 n+1
	preMin := make([]int, n+1)
	preMin[0] = n + 1
	minLen := n + 1
	sum := 0
	l := 0

	for r, x := range arr {
		sum += x
		for sum > target {
			sum -= arr[l]
			l++
		}
		if sum == target {
			// 枚举第二个子数组的右端点为 r，用滑动窗口算出此时第二个子数组的左端点为 l
			// 那么第一个子数组的右端点必须 < l
			ans = min(ans, preMin[l]+r-l+1)
			minLen = min(minLen, r-l+1) // 维护遍历过的和为 target 的子数组的最短长度
		}
		preMin[r+1] = minLen // 保存此时此刻的 minLen
	}

	if ans > n {
		return -1
	}
	return ans
}
```

## 思考题

如果本题 $\textit{arr}$ 中有负数，怎么做？

欢迎在评论区分享你的思路/代码。

## 专题训练

1. 动态规划题单的「**专题：前后缀分解**」。
2. 数据结构题单的「**§0.1 枚举右，维护左**」和「**§1.2 前缀和与哈希表**」。

## 分类题单

[如何科学刷题？](https://leetcode.cn/discuss/post/3141566/ru-he-ke-xue-shua-ti-by-endlesscheng-q3yd/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/discuss/post/3578981/ti-dan-hua-dong-chuang-kou-ding-chang-bu-rzz7/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/discuss/post/3579164/ti-dan-er-fen-suan-fa-er-fen-da-an-zui-x-3rqn/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/discuss/post/3579480/ti-dan-dan-diao-zhan-ju-xing-xi-lie-zi-d-u4hk/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/discuss/post/3580195/fen-xiang-gun-ti-dan-wang-ge-tu-dfsbfszo-l3pa/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/discuss/post/3580371/fen-xiang-gun-ti-dan-wei-yun-suan-ji-chu-nth4/)
6. [图论算法（DFS/BFS/拓扑排序/基环树/最短路/最小生成树/网络流）](https://leetcode.cn/discuss/post/3581143/fen-xiang-gun-ti-dan-tu-lun-suan-fa-dfsb-qyux/)
7. [动态规划（入门/背包/划分/状态机/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/discuss/post/3581838/fen-xiang-gun-ti-dan-dong-tai-gui-hua-ru-007o/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/discuss/post/3583665/fen-xiang-gun-ti-dan-chang-yong-shu-ju-j-bvmv/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/discuss/post/3584388/fen-xiang-gun-ti-dan-shu-xue-suan-fa-shu-gcai/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/discuss/post/3091107/fen-xiang-gun-ti-dan-tan-xin-ji-ben-tan-k58yb/)
11. [链表、树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/discuss/post/3142882/fen-xiang-gun-ti-dan-lian-biao-er-cha-sh-6srp/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/discuss/post/3144832/fen-xiang-gun-ti-dan-zi-fu-chuan-kmpzhan-ugt4/)

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
