# 3048. 标记所有下标的最早秒数 I

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/earliest-second-to-mark-indices-i/
- 题目 slug：`earliest-second-to-mark-indices-i`
- 来源专题：二分算法
- 来源分类路径：二、二分答案 / §2.1 求最小 / 答疑
- 难度分：2263
- 外部题解来源：https://leetcode.cn/problems/earliest-second-to-mark-indices-i/solutions/2653101/er-fen-da-an-pythonjavacgo-by-endlessche-or61/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：二分答案+正序/逆序（Python/Java/C++/Go）](https://leetcode.cn/problems/earliest-second-to-mark-indices-i/solutions/2653101/er-fen-da-an-pythonjavacgo-by-endlessche-or61/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`er-fen-da-an-pythonjavacgo-by-endlessche-or61`
- topic id：`2653101`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 更形象的题意

题意有点抽象，形象地解释一下：

你有 $n$ 门课程需要考试，第 $i$ 门课程需要用 $\textit{nums}[i]$ 天复习。同一天只能复习一门课程。

在第 $i$ 天，你可以选择参加第 $\textit{changeIndices}[i]$ 门课程的考试。考试这一天不能复习。

搞定所有课程的复习+考试，至少要多少天？

## 方法一：二分答案+正向遍历

#### 提示 1

答案越大，越能够搞定所有课程，反之越不能。据此，可以**二分答案**。

#### 提示 2

考试的时间越晚越好，这样我们能有更充足的时间复习。

设二分的答案为 $\textit{mx}$。在 $\textit{mx}$ 天内，设 $\textit{i}$ 在 $\textit{changeIndices}$ 中出现的最后下标为 $\textit{lastT}[\textit{i}]$，即第 $i$ 门课程的最晚考试时间。如果 $i$ 不在 $\textit{changeIndices}$ 的前 $\textit{mx}$ 个数中，二分返回 $\texttt{false}$。

- 初始化 $\textit{cnt}=0$，遍历 $\textit{changeIndices}$ 的前 $\textit{mx}$ 个数。
- 如果 $i\ne \textit{lastT}[i]$，这一天只能用来复习（或者什么也不做），但还不知道要复习哪一门课程，所以暂时记录一下，把 $\textit{cnt}$ 加一。
- 如果 $i= \textit{lastT}[i]$，先从 $\textit{cnt}$ 中消耗 $\textit{nums}[i]$ 天用来复习，然后考试。如果 $\textit{cnt}<\textit{nums}[i]$ 则无法完成考试，二分返回 $\texttt{false}$。

如果遍历中没有返回 $\texttt{false}$，返回 $\texttt{true}$。

下面代码用的开区间二分（其它写法也可以），原理请看 [二分查找 红蓝染色法【基础算法精讲 04】](https://www.bilibili.com/video/BV1AP41137w7/)。

[视频讲解](https://www.bilibili.com/video/BV1qx421179t/) 第三题。

```py [sol-Python3]
class Solution:
    def earliestSecondToMarkIndices(self, nums: List[int], changeIndices: List[int]) -> int:
        n, m = len(nums), len(changeIndices)
        if n > m:
            return -1

        def check(mx: int) -> bool:
            last_t = [-1] * n
            for t, idx in enumerate(changeIndices[:mx]):
                last_t[idx - 1] = t
            if -1 in last_t:  # 有课程没有考试时间
                return False

            cnt = 0
            for i, idx in enumerate(changeIndices[:mx]):
                idx -= 1
                if i == last_t[idx]:  # 考试
                    if nums[idx] > cnt:  # 没时间复习
                        return False
                    cnt -= nums[idx]  # 复习这门课程
                else:
                    cnt += 1  # 留着后面用
            return True

        left = n + sum(nums)
        ans = left + bisect_left(range(left, m + 1), True, key=check)
        return -1 if ans > m else ans
```

```java [sol-Java]
class Solution {
    public int earliestSecondToMarkIndices(int[] nums, int[] changeIndices) {
        int n = nums.length;
        int m = changeIndices.length;
        if (n > m) {
            return -1;
        }

        int[] lastT = new int[n];
        int left = n - 1, right = m + 1;
        while (left + 1 < right) {
            int mid = (left + right) / 2;
            if (check(nums, changeIndices, lastT, mid)) {
                right = mid;
            } else {
                left = mid;
            }
        }
        return right > m ? -1 : right;
    }

    private boolean check(int[] nums, int[] changeIndices, int[] lastT, int mx) {
        Arrays.fill(lastT, -1);
        for (int t = 0; t < mx; t++) {
            lastT[changeIndices[t] - 1] = t;
        }
        for (int t : lastT) {
            if (t < 0) { // 有课程没有考试时间
                return false;
            }
        }

        int cnt = 0;
        for (int i = 0; i < mx; i++) {
            int idx = changeIndices[i] - 1;
            if (i == lastT[idx]) { // 考试
                if (nums[idx] > cnt) { // 没时间复习
                    return false;
                }
                cnt -= nums[idx]; // 复习这门课程
            } else {
                cnt++; // 留着后面用
            }
        }
        return true;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int earliestSecondToMarkIndices(vector<int>& nums, vector<int>& changeIndices) {
        int n = nums.size(), m = changeIndices.size();
        if (n > m) {
            return -1;
        }

        vector<int> last_t(n);
        auto check = [&](int mx) -> bool {
            ranges::fill(last_t, -1);
            for (int t = 0; t < mx; t++) {
                last_t[changeIndices[t] - 1] = t;
            }
            if (ranges::find(last_t, -1) != last_t.end()) { // 有课程没有考试时间
                return false;
            }

            int cnt = 0;
            for (int i = 0; i < mx; i++) {
                int idx = changeIndices[i] - 1;
                if (i == last_t[idx]) { // 考试
                    if (nums[idx] > cnt) { // 没时间复习
                        return false;
                    }
                    cnt -= nums[idx]; // 复习这门课程
                } else {
                    cnt++; // 留着后面用
                }
            }
            return true;
        };

        int left = n - 1, right = m + 1;
        while (left + 1 < right) {
            int mid = (left + right) / 2;
            (check(mid) ? right : left) = mid;
        }
        return right > m ? -1 : right;
    }
};
```

```go [sol-Go]
func earliestSecondToMarkIndices(nums, changeIndices []int) int {
	n, m := len(nums), len(changeIndices)
	if n > m {
		return -1
	}

	lastT := make([]int, n)
	ans := n + sort.Search(m+1-n, func(mx int) bool {
		mx += n
		clear(lastT)
		for t, idx := range changeIndices[:mx] {
			lastT[idx-1] = t + 1
		}
		if slices.Contains(lastT, 0) { // 有课程没有考试时间
			return false
		}

		cnt := 0
		for i, idx := range changeIndices[:mx] {
			idx--
			if i == lastT[idx]-1 { // 考试
				if nums[idx] > cnt { // 没时间复习
					return false
				}
				cnt -= nums[idx] // 复习这门课程
			} else {
				cnt++ // 留着后面用
			}
		}
		return true
	})
	if ans > m {
		return -1
	}
	return ans
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(m\log m)$，其中 $m$ 为 $\textit{changeIndices}$ 的长度。二分的时候保证 $n\le m$，时间复杂度以 $m$ 为主。
- 空间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{nums}$ 的长度。

## 方法二：二分答案+逆向遍历

也可以倒着遍历 $\textit{changeIndices}$ 的前 $\textit{mx}$ 个数。

- 初始化需要考试的课程数 $\textit{exam}=n$ 和需要复习的天数 $\textit{study}=0$。
- 如果第一次遇到 $\textit{changeIndices}[i]$，那么就考试，把 $\textit{exam}$ 减一，把 $\textit{study}$ 增加这门课程需要的复习天数。
- 否则这天用来复习，把 $\textit{study}$ 减一（前提是 $\textit{study}>0$）。

最后如果 $\textit{exam}=0$ 且 $\textit{study}=0$，就表示所有课程都考完了，并且考试前有足够的天数用来复习。

```py [sol-Python3]
class Solution:
    def earliestSecondToMarkIndices(self, nums: List[int], changeIndices: List[int]) -> int:
        n, m = len(nums), len(changeIndices)
        if n > m:
            return -1

        done = [0] * n  # 避免反复创建和初始化数组
        def check(mx: int) -> bool:
            exam, study = n, 0
            for i in range(mx - 1, -1, -1):
                idx = changeIndices[i] - 1
                if done[idx] != mx:
                    done[idx] = mx
                    exam -= 1  # 考试
                    study += nums[idx]  # 需要复习的天数
                elif study:
                    study -= 1  # 复习
            return exam == 0 and study == 0  # 考完了并且复习完了

        left = n + sum(nums)
        ans = left + bisect_left(range(left, m + 1), True, key=check)
        return -1 if ans > m else ans
```

```java [sol-Java]
class Solution {
    public int earliestSecondToMarkIndices(int[] nums, int[] changeIndices) {
        int n = nums.length;
        int m = changeIndices.length;
        if (n > m) {
            return -1;
        }

        int[] done = new int[n]; // 避免反复创建和初始化数组
        int left = n - 1, right = m + 1;
        while (left + 1 < right) {
            int mid = (left + right) / 2;
            if (check(nums, changeIndices, done, mid)) {
                right = mid;
            } else {
                left = mid;
            }
        }
        return right > m ? -1 : right;
    }

    private boolean check(int[] nums, int[] changeIndices, int[] done, int mx) {
        int exam = nums.length;
        int study = 0;
        for (int i = mx - 1; i >= 0 && study <= i + 1; i--) { // 要复习的天数不能太多
            int idx = changeIndices[i] - 1;
            if (done[idx] != mx) {
                done[idx] = mx;
                exam--; // 考试
                study += nums[idx]; // 需要复习的天数
            } else if (study > 0) {
                study--; // 复习
            }
        }
        return exam == 0 && study == 0; // 考完了并且复习完了
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int earliestSecondToMarkIndices(vector<int>& nums, vector<int>& changeIndices) {
        int n = nums.size(), m = changeIndices.size();
        if (n > m) {
            return -1;
        }

        vector<int> done(n); // 避免反复创建和初始化数组
        auto check = [&](int mx) -> bool {
            int exam = n, study = 0;
            for (int i = mx - 1; i >= 0 && study <= i + 1; i--) { // 要复习的天数不能太多
                int idx = changeIndices[i] - 1;
                if (done[idx] != mx) {
                    done[idx] = mx;
                    exam--; // 考试
                    study += nums[idx]; // 需要复习的天数
                } else if (study) {
                    study--; // 复习
                }
            }
            return exam == 0 && study == 0; // 考完了并且复习完了
        };

        int left = n - 1, right = m + 1;
        while (left + 1 < right) {
            int mid = (left + right) / 2;
            (check(mid) ? right : left) = mid;
        }
        return right > m ? -1 : right;
    }
};
```

```go [sol-Go]
func earliestSecondToMarkIndices(nums, changeIndices []int) int {
	n, m := len(nums), len(changeIndices)
	if n > m {
		return -1
	}

	done := make([]int, n) // 避免反复创建和初始化数组
	ans := n + sort.Search(m+1-n, func(mx int) bool {
		mx += n
		exam, study := n, 0
		for i := mx - 1; i >= 0; i-- {
			idx := changeIndices[i] - 1
			if done[idx] != mx {
				done[idx] = mx
				exam-- // 考试
				study += nums[idx] // 需要复习的天数
			} else if study > 0 {
				study-- // 复习
			}
		}
		return exam == 0 && study == 0 // 考完了并且复习完了
	})
	if ans > m {
		return -1
	}
	return ans
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(m\log m)$，其中 $m$ 为 $\textit{changeIndices}$ 的长度。二分的时候保证 $n\le m$，时间复杂度以 $m$ 为主。
- 空间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{nums}$ 的长度。

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
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `二、二分答案 / §2.1 求最小 / 答疑`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、二分答案 / §2.1 求最小 / 答疑`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
