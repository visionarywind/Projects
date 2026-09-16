# 3911. 移除子数组元素后第 K 小偶数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/k-th-smallest-remaining-even-integer-in-subarray-queries/
- 题目 slug：`k-th-smallest-remaining-even-integer-in-subarray-queries`
- 来源专题：二分算法
- 来源分类路径：四、其他
- 难度分：2156
- 外部题解来源：https://leetcode.cn/problems/k-th-smallest-remaining-even-integer-in-subarray-queries/solutions/3959171/liang-chong-fang-fa-er-fen-tao-er-fen-er-ss4r/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：二分套二分 / 二分（Python/Java/C++/Go）](https://leetcode.cn/problems/k-th-smallest-remaining-even-integer-in-subarray-queries/solutions/3959171/liang-chong-fang-fa-er-fen-tao-er-fen-er-ss4r/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`liang-chong-fang-fa-er-fen-tao-er-fen-er-ss4r`
- topic id：`3959171`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 方法一：二分套二分

**前置题目**：推荐读者先做几道「**第 K 小/大**」型的二分问题，见 [二分题单](https://leetcode.cn/circle/discuss/SqopEo/) 的 §2.6 节。

**套路**：

- 第 $k$ 小等价于：求**最小**的 $x$，满足 $\le x$ 的数**至少**有 $k$ 个。
- 第 $k$ 大等价于：求**最大**的 $x$，满足 $\ge x$ 的数**至少**有 $k$ 个。

对于本题，我们二分答案是第 $x$ 个正偶数，那么需要求出 $\le 2x$ 的正偶数个数：

- 在 $[1,2x]$ 中，有 $x$ 个正偶数。
- 在去掉奇数后的子数组中，二分查找 $> 2x$ 的第一个数的下标 $j$，由于子数组是递增的，所以子数组的下标 $[0,j-1]$ 都是 $\le 2x$ 的，这有 $j$ 个。
- 二者相减，得到 $\le 2x$ 的正偶数个数为 $x-j$。

二分范围：

- **开区间二分下界**：$k-1$。答案至少是第 $k$ 个正偶数。
- **开区间二分上界**：$k+m$，其中 $m$ 是子数组中的偶数个数。答案至多是第 $k+m$ 个正偶数。

```py [sol-Python3]
# 这个做法很慢，更快的做法见方法二
class Solution:
    def kthRemainingInteger(self, nums: list[int], queries: list[list[int]]) -> list[int]:
        # 记录所有偶数的下标
        even_pos = [i for i, x in enumerate(nums) if x % 2 == 0]
        ans = [0] * len(queries)

        for i, (l, r, k) in enumerate(queries):
            # 计算询问对应的 even_pos 的子数组 even_pos[li:ri]
            li = bisect_left(even_pos, l)
            ri = bisect_right(even_pos, r)

            def check(x: int) -> bool:
                # 计算子数组中的 <= x*2 的偶数个数
                j = bisect_right(range(ri), x * 2, lo=li, key=lambda i: nums[even_pos[i]]) - li
                return x - j >= k

            # 二分答案是第几个正偶数
            # 二分下界：至少是第 k 个正偶数
            # 二分上界：至多是第 k+ri-li 个正偶数
            left, right = k, k + ri - li
            res = bisect_left(range(right), True, lo=left, key=check)
            ans[i] = res * 2  # 答案是第 res 个正偶数

        return ans
```

```java [sol-Java]
class Solution {
    public int[] kthRemainingInteger(int[] nums, int[][] queries) {
        // 记录所有偶数的下标
        List<Integer> evenPos = new ArrayList<>();
        for (int i = 0; i < nums.length; i++) {
            if (nums[i] % 2 == 0) {
                evenPos.add(i);
            }
        }

        int[] ans = new int[queries.length];

        for (int i = 0; i < queries.length; i++) {
            int[] q = queries[i];
            // 计算询问对应的 evenPos 子数组
            int li = lowerBound(evenPos, q[0]);
            int ri = lowerBound(evenPos, q[1] + 1);
            int k = q[2];

            // 二分答案是第几个正偶数
            // 二分下界：至少是第 k 个正偶数
            // 二分上界：至多是第 k+ri-li 个正偶数
            int left = k - 1, right = k + ri - li;
            while (left + 1 < right) {
                int mid = left + (right - left) / 2;
                if (check(mid, li, ri, k, evenPos, nums)) {
                    right = mid;
                } else {
                    left = mid;
                }
            }
            ans[i] = right * 2; // 答案是第 right 个正偶数
        }

        return ans;
    }

    private boolean check(int x, int li, int ri, int k, List<Integer> evenPos, int[] nums) {
        // 计算子数组中的 <= x*2 的偶数个数
        int left = li - 1, right = ri;
        while (left + 1 < right) {
            int mid = left + (right - left) / 2;
            if (nums[evenPos.get(mid)] > x * 2) {
                right = mid;
            } else {
                left = mid;
            }
        }
        int j = right - li;
        return x - j >= k;
    }

    private int lowerBound(List<Integer> a, int target) {
        int left = -1, right = a.size();
        while (left + 1 < right) {
            int mid = left + (right - left) / 2;
            if (a.get(mid) >= target) {
                right = mid;
            } else {
                left = mid;
            }
        }
        return right;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> kthRemainingInteger(vector<int>& nums, vector<vector<int>>& queries) {
        // 记录所有偶数的下标
        vector<int> even_pos;
        for (int i = 0; i < nums.size(); i++) {
            if (nums[i] % 2 == 0) {
                even_pos.push_back(i);
            }
        }

        vector<int> ans;
        ans.reserve(queries.size()); // 预分配空间

        for (auto& q : queries) {
            // 计算询问对应的 even_pos 子数组
            int li = ranges::lower_bound(even_pos, q[0]) - even_pos.begin();
            int ri = ranges::upper_bound(even_pos, q[1]) - even_pos.begin();
            int k = q[2];

            auto check = [&](int x) -> bool {
                // 计算子数组中的 <= x*2 的偶数个数
                int left = li - 1, right = ri;
                while (left + 1 < right) {
                    int mid = left + (right - left) / 2;
                    (nums[even_pos[mid]] > x * 2 ? right : left) = mid;
                }
                int j = right - li;
                return x - j >= k;
            };

            // 二分答案是第几个正偶数
            // 二分下界：至少是第 k 个正偶数
            // 二分上界：至多是第 k+ri-li 个正偶数
            int left = k - 1, right = k + ri - li;
            while (left + 1 < right) {
                int mid = left + (right - left) / 2;
                (check(mid) ? right : left) = mid;
            }
            ans.push_back(right * 2); // 答案是第 right 个正偶数
        }

        return ans;
    }
};
```

```go [sol-Go]
func kthRemainingInteger(nums []int, queries [][]int) []int {
	// 记录所有偶数的下标
	evenPos := []int{}
	for i, x := range nums {
		if x%2 == 0 {
			evenPos = append(evenPos, i)
		}
	}

	ans := make([]int, len(queries))
	for i, q := range queries {
		// 计算询问对应的 evenPos 子数组
		l := sort.SearchInts(evenPos, q[0])
		r := sort.SearchInts(evenPos, q[1]+1)
		pos := evenPos[l:r]
		k := q[2]

		// 二分答案是第几个正偶数
		// 二分下界：至少是第 k 个正偶数
		// 二分上界：至多是第 k+len(pos) 个正偶数
		left, right := k, k+len(pos)
		res := left + sort.Search(right-left, func(x int) bool {
			x += left
			// 计算子数组中的 <= x*2 的偶数个数
			j := sort.Search(len(pos), func(j int) bool { return nums[pos[j]] > x*2 })
			return x-j >= k
		})
		ans[i] = res * 2 // 答案是第 res 个正偶数
	}
	return ans
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n + q\log^2 n)$，其中 $n$ 是 $\textit{nums}$ 的长度，$q$ 是 $\textit{queries}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。返回值不计入。

## 方法二：二分

**前置题目**：[1539. 第 k 个缺失的正整数](https://leetcode.cn/problems/kth-missing-positive-number/)，[我的题解](https://leetcode.cn/problems/kth-missing-positive-number/solutions/3959163/olog-n-xiang-xi-tui-dao-jian-ji-xie-fa-p-67ag/)。

换个方式描述问题：找不在子数组中的第 $k$ 个缺失的正偶数。

如果把偶数都除以 $2$，那不就是 1539 题吗？

[本题视频讲解](https://www.bilibili.com/video/BV15pZcBzEmR/?t=11m2s)，欢迎点赞关注~

```py [sol-Python3]
# 更快的写法见【Python3 写法二】
class Solution:
    def kthRemainingInteger(self, nums: list[int], queries: list[list[int]]) -> list[int]:
        # 记录所有偶数的下标
        even_pos = [i for i, x in enumerate(nums) if x % 2 == 0]
        ans = [0] * len(queries)

        for i, (l, r, k) in enumerate(queries):
            # 计算询问对应的 even_pos 的子数组 even_pos[li:ri]
            li = bisect_left(even_pos, l)
            ri = bisect_right(even_pos, r)

            # 推导过程见 1539 题解
            j = bisect_left(range(ri - li), True, key=lambda j: nums[even_pos[li + j]] // 2 - 1 - j >= k)
            ans[i] = (j + k) * 2

        return ans
```

```py [sol-Python3 写法二]
class Solution:
    def kthRemainingInteger(self, nums: list[int], queries: list[list[int]]) -> list[int]:
        # 记录所有偶数的下标
        even_pos = [i for i, x in enumerate(nums) if x % 2 == 0]
        #       nums[even_pos[li + j]] // 2 - 1 - j >= k
        # 等价于 nums[even_pos[j']] // 2 - 1 - (j' - li) >= k
        # 等价于 nums[even_pos[j']] // 2 - 1 - j' >= k - li
        # 所以在 a 中二分 k - li 即可
        a = [nums[idx] // 2 - 1 - i for i, idx in enumerate(even_pos)]
        ans = [0] * len(queries)

        for i, (l, r, k) in enumerate(queries):
            li = bisect_left(even_pos, l)
            ri = bisect_right(even_pos, r)
            j = bisect_left(a, k - li, lo=li, hi=ri) - li
            ans[i] = (j + k) * 2

        return ans
```

```java [sol-Java]
class Solution {
    public int[] kthRemainingInteger(int[] nums, int[][] queries) {
        // 记录所有偶数的下标
        List<Integer> evenPos = new ArrayList<>();
        for (int i = 0; i < nums.length; i++) {
            if (nums[i] % 2 == 0) {
                evenPos.add(i);
            }
        }

        int[] ans = new int[queries.length];

        for (int i = 0; i < queries.length; i++) {
            int[] q = queries[i];
            // 计算询问对应的 evenPos 子数组
            int li = lowerBound(evenPos, q[0]);
            int ri = lowerBound(evenPos, q[1] + 1);
            int k = q[2];

            int left = -1, right = ri - li;
            while (left + 1 < right) {
                int mid = left + (right - left) / 2;
                if (nums[evenPos.get(li + mid)] / 2 - 1 - mid >= k) {
                    right = mid;
                } else {
                    left = mid;
                }
            }
            ans[i] = (right + k) * 2; // 推导过程见 1539 题解
        }

        return ans;
    }

    private int lowerBound(List<Integer> a, int target) {
        int left = -1, right = a.size();
        while (left + 1 < right) {
            int mid = left + (right - left) / 2;
            if (a.get(mid) >= target) {
                right = mid;
            } else {
                left = mid;
            }
        }
        return right;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> kthRemainingInteger(vector<int>& nums, vector<vector<int>>& queries) {
        // 记录所有偶数的下标
        vector<int> even_pos;
        for (int i = 0; i < nums.size(); i++) {
            if (nums[i] % 2 == 0) {
                even_pos.push_back(i);
            }
        }

        vector<int> ans;
        ans.reserve(queries.size()); // 预分配空间

        for (auto& q : queries) {
            // 计算询问对应的 even_pos 子数组
            int li = ranges::lower_bound(even_pos, q[0]) - even_pos.begin();
            int ri = ranges::upper_bound(even_pos, q[1]) - even_pos.begin();
            int k = q[2];

            int left = -1, right = ri - li;
            while (left + 1 < right) {
                int mid = left + (right - left) / 2;
                (nums[even_pos[li + mid]] / 2 - 1 - mid >= k ? right : left) = mid;
            }
            ans.push_back((right + k) * 2); // 推导过程见 1539 题解
        }

        return ans;
    }
};
```

```go [sol-Go]
func kthRemainingInteger(nums []int, queries [][]int) []int {
	// 记录所有偶数的下标
	evenPos := []int{}
	for i, x := range nums {
		if x%2 == 0 {
			evenPos = append(evenPos, i)
		}
	}

	ans := make([]int, len(queries))
	for i, q := range queries {
		// 找到询问对应的 evenPos 的子数组
		l := sort.SearchInts(evenPos, q[0])
		r := sort.SearchInts(evenPos, q[1]+1)
		pos := evenPos[l:r]
		k := q[2]

		// 推导过程见 1539 题解
		j := sort.Search(len(pos), func(j int) bool {
			return nums[pos[j]]/2-1-j >= k
		})
		ans[i] = (j + k) * 2
	}
	return ans
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n + q\log n)$，其中 $n$ 是 $\textit{nums}$ 的长度，$q$ 是 $\textit{queries}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。返回值不计入。

## 专题训练

见下面二分题单的「**§2.6 第 K 小/大**」。

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

本题来自 `四、其他`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `四、其他`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
