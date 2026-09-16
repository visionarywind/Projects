# 3878. 统计好子数组

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/count-good-subarrays/
- 题目 slug：`count-good-subarrays`
- 来源专题：位运算
- 来源分类路径：三、与或（AND/OR）的性质 / AND/OR LogTrick
- 难度分：2230
- 外部题解来源：https://leetcode.cn/problems/count-good-subarrays/solutions/3933380/mo-ban-logtrick-ji-lu-mei-ge-yuan-su-de-otgcv/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：LogTrick / 单调栈+贡献法（Python/Java/C++/Go）](https://leetcode.cn/problems/count-good-subarrays/solutions/3933380/mo-ban-logtrick-ji-lu-mei-ge-yuan-su-de-otgcv/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`mo-ban-logtrick-ji-lu-mei-ge-yuan-su-de-otgcv`
- topic id：`3933380`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 方法一：LogTrick

**前置知识**：[LogTrick 入门教程](https://zhuanlan.zhihu.com/p/1933215367158830792)，也可以看 [本题视频讲解](https://www.bilibili.com/video/BV1vfAuzyEp8/?t=33m56s)，解释了 LogTrick 的原理。

本题需要判断子数组是否包含 $\textit{or}$。我们可以在遍历 $\textit{nums}$ 的同时，维护 $\textit{nums}$ 的每个元素 $x$ 的**最近一次出现的位置** $\textit{last}[x]$，只要 $\textit{last}[\textit{or}]$ 大于等于子数组左端点，那么子数组就包含 $\textit{or}$。

设右端点在 $i$，左端点在 $[\ell,r]$ 中的子数组的 OR 都是 $\textit{or}$。设 $j = \textit{last}[\textit{or}]$。那么当 $j\ge \ell$ 时，左端点为 $\ell,\ell+1,\ldots,\min(r, j)$，右端点为 $i$ 的子数组都包含 $\textit{or}$，这一共有

$$
\min(r,j)-\ell+1
$$

个合法子数组，加入答案。

代码用到了原地去重算法，可以看 [26. 删除有序数组中的重复项](https://leetcode.cn/problems/remove-duplicates-from-sorted-array/)，[我的题解](https://leetcode.cn/problems/remove-duplicates-from-sorted-array/solutions/2807162/gen-zhao-wo-guo-yi-bian-shi-li-2ni-jiu-m-rvyk/)。

```py [sol-Python3]
class Solution:
    def countGoodSubarrays(self, nums: List[int]) -> int:
        or_left = []  # (子数组或值，最小左端点)
        last = {}
        ans = 0

        for i, x in enumerate(nums):
            last[x] = i

            # 计算以 i 为右端点的子数组或值
            for p in or_left:
                p[0] |= x
            # x 单独一个数作为子数组
            or_left.append([x, i])

            # 原地去重（相同或值只保留最左边的）
            # 原理见力扣 26. 删除有序数组中的重复项
            idx = 1
            for j in range(1, len(or_left)):
                if or_left[j][0] != or_left[j - 1][0]:
                    or_left[idx] = or_left[j]
                    idx += 1
            del or_left[idx:]

            for k, (or_val, left) in enumerate(or_left):
                right = or_left[k + 1][1] - 1 if k < len(or_left) - 1 else i
                # 对于左端点在 [left, right]，右端点为 i 的子数组，OR 值都是 or_val
                j = last.get(or_val, -1)
                if j >= left:
                    ans += min(right, j) - left + 1

        return ans
```

```java [sol-Java]
class Solution {
    public long countGoodSubarrays(int[] nums) {
        List<int[]> orLeft = new ArrayList<>(); // (子数组或值，最小左端点)
        Map<Integer, Integer> last = new HashMap<>();
        long ans = 0;

        for (int i = 0; i < nums.length; i++) {
            int x = nums[i];
            last.put(x, i);

            // 计算以 i 为右端点的子数组或值
            for (int[] p : orLeft) {
                p[0] |= x; // **根据题目修改**
            }
            // x 单独一个数作为子数组
            orLeft.add(new int[]{x, i});

            // 原地去重（相同或值只保留最左边的）
            // 原理见力扣 26. 删除有序数组中的重复项
            int m = 1;
            for (int j = 1; j < orLeft.size(); j++) {
                if (orLeft.get(j)[0] != orLeft.get(j - 1)[0]) {
                    orLeft.set(m++, orLeft.get(j));
                }
            }
            orLeft.subList(m, orLeft.size()).clear();

            for (int k = 0; k < m; k++) {
                int orVal = orLeft.get(k)[0];
                int left = orLeft.get(k)[1];
                int right = k < m - 1 ? orLeft.get(k + 1)[1] - 1 : i;
                // 对于左端点在 [left, right]，右端点为 i 的子数组，OR 值都是 orVal
                int j = last.getOrDefault(orVal, -1);
                if (j >= left) {
                    ans += Math.min(right, j) - left + 1;
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
    long long countGoodSubarrays(vector<int>& nums) {
        vector<pair<int, int>> or_left; // (子数组或值，最小左端点)
        unordered_map<int, int> last;
        long long ans = 0;

        for (int i = 0; i < nums.size(); i++) {
            int x = nums[i];
            last[x] = i;

            // 计算以 i 为右端点的子数组或值
            for (auto& [or_val, _] : or_left) {
                or_val |= x; // **根据题目修改**
            }
            // x 单独一个数作为子数组
            or_left.emplace_back(x, i);

            // 原地去重（相同或值只保留最左边的）
            // 原理见力扣 26. 删除有序数组中的重复项
            int m = 1;
            for (int j = 1; j < or_left.size(); j++) {
                if (or_left[j].first != or_left[j - 1].first) {
                    or_left[m++] = or_left[j];
                }
            }
            or_left.resize(m);

            for (int k = 0; k < m; k++) {
                auto [or_val, left] = or_left[k];
                int right = k + 1 < m ? or_left[k + 1].second - 1 : i;
                // 对于左端点在 [left, right]，右端点为 i 的子数组，OR 值都是 or_val
                auto it = last.find(or_val);
                if (it != last.end() && it->second >= left) {
                    ans += min(right, it->second) - left + 1;
                }
            }
        }

        return ans;
    }
};
```

```go [sol-Go]
func countGoodSubarrays(nums []int) (ans int64) {
	type pair struct{ or, left int } // 子数组或值，最小左端点
	orLeft := []pair{}
	last := map[int]int{}

	for i, x := range nums {
		last[x] = i

		// 计算以 i 为右端点的子数组或值
		for j := range orLeft {
			orLeft[j].or |= x
		}
		// x 单独一个数作为子数组
		orLeft = append(orLeft, pair{x, i})

		// 原地去重（相同或值只保留最左边的）
		// 原理见力扣 26. 删除有序数组中的重复项
		idx := 1
		for j := 1; j < len(orLeft); j++ {
			if orLeft[j].or != orLeft[j-1].or {
				orLeft[idx] = orLeft[j]
				idx++
			}
		}
		orLeft = orLeft[:idx]

		for k, p := range orLeft {
			orVal := p.or
			left := p.left
			right := i
			if k < len(orLeft)-1 {
				right = orLeft[k+1].left - 1
			}
			// 对于左端点在 [left, right]，右端点为 i 的子数组，OR 值都是 orVal
			j, ok := last[orVal]
			if ok && j >= left {
				ans += int64(min(right, j) - left + 1)
			}
		}
	}

	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log U)$，其中 $n$ 是 $\textit{nums}$ 的长度，$U=\max(\textit{nums})$。理由见 [LogTrick 入门教程](https://zhuanlan.zhihu.com/p/1933215367158830792)。
- 空间复杂度：$\mathcal{O}(n + \log U)$。

## 方法二：单调栈 + 贡献法

**前置题目**：[907. 子数组的最小值之和](https://leetcode.cn/problems/sum-of-subarray-minimums/)，[我的题解](https://leetcode.cn/problems/sum-of-subarray-minimums/solutions/1930857/gong-xian-fa-dan-diao-zhan-san-chong-shi-gxa5/)。**请注意 907 题避免重复统计的技巧，本文不再赘述**。

根据 [从集合论到位运算，常见位运算技巧分类总结](https://leetcode.cn/circle/discuss/CaOJ45/)，子数组的 OR 值，是子数组所有元素的超集。换句话说，子数组的每个数，都是子数组 OR 值的子集。

仿照 907 题的做法，枚举 $x = \textit{nums}[i]$ 作为子数组的 OR 值。计算子数组左端点的最小值，和子数组右端点的最大值，从而算出 $x$ 的贡献（子数组个数）。

对于 OR 值为 $x$ 的子数组，如果 $y$ 也在子数组中，那么 $y$ 必须是 $x$ 的子集，即 `(y | x) == x`。如果不满足要求，那么子数组一定不能包含 $y$，否则子数组 OR 值会大于 $x$。

对比一下：

- 在常规单调栈中，如果要计算左边比 $\textit{nums}[i]$ 大的最近的数的下标，假设我们先遍历到 $y$，再遍历到 $x$，且 $y\le x$，那么对于 $x$ 右边的数来说，$x$ 更大且更靠近右边的数，所以 $y$ 是个无用数据，直接出栈。**注**：如果 $y > \textit{nums}[i]$ 满足要求，那么 $x\ge y > \textit{nums}[i]$ 也满足要求（且更近）。
- 对于本题，我们要计算左边不是 $\textit{nums}[i]$ 的子集的最近的数的下标，假设我们先遍历到 $y$，再遍历到 $x$，且 $y$ 是 $x$ 的子集，那么对于 $x$ 右边的数来说，$x$ 更大（从集合角度来说）且更靠近右边的数，所以 $y$ 是个无用数据，直接出栈。**注**：如果集合 $y$ 有不在集合 $\textit{nums}[i]$ 中的元素，由于集合 $y$ 中的每个元素都在集合 $x$ 中，所以集合 $x$ 也必定有不在集合 $\textit{nums}[i]$ 中的元素。

因此，把常规单调栈中的 `y <= x` 替换成 `(y | x) == x` 即可。

### 答疑

**问**：是否会出现不同的 $x_1$ 和 $x_2$，对应的子数组都同时包含 $x_1$ 和 $x_2$ 的情况？这会导致重复统计。

**答**：不会，如果 $x_1\ne x_2$，那么二者不可能互为对方的子集（这只会在 $x_1=x_2$ 的时候出现）。所以我们只需考虑 $x_1=x_2$ 时，如何避免重复统计，这在 907 题的题解中解释了。

```py [sol-Python3]
class Solution:
    def countGoodSubarrays(self, nums: List[int]) -> int:
        n = len(nums)
        left = [0] * n
        st = [-1]  # 哨兵
        for i, x in enumerate(nums):
            while len(st) > 1 and (nums[st[-1]] | x) == x:
                st.pop()
            left[i] = st[-1]  # nums[left[i]] 不是 x 的子集
            st.append(i)

        st = [n]
        ans = 0
        for i in range(n - 1, -1, -1):
            x = nums[i]
            # 比如 nums = [...,1,...,1,...]，我们规定，包含左边的 1 的子数组，不能包含右边的 1，从而避免重复统计子数组
            # 注：包含右边的 1 的子数组，可以包含左边的 1
            while len(st) > 1 and nums[st[-1]] != x and (nums[st[-1]] | x) == x:
                st.pop()
            right = st[-1]  # nums[right] 不是 x 的子集
            st.append(i)

            # 子数组左端点可以从 left[i]+1 到 i，一共 i-left[i] 个
            # 子数组右端点可以从 i 到 right-1，一共 right-i 个
            ans += (i - left[i]) * (right - i)

        return ans
```

```java [sol-Java]
class Solution {
    public long countGoodSubarrays(int[] nums) {
        int n = nums.length;
        int[] left = new int[n];
        ArrayDeque<Integer> st = new ArrayDeque<>(); // 更快的写法见【Java 数组】
        st.push(-1); // 哨兵
        for (int i = 0; i < n; i++) {
            int x = nums[i];
            while (st.size() > 1 && (nums[st.peek()] | x) == x) {
                st.pop();
            }
            left[i] = st.peek(); // nums[left[i]] 不是 x 的子集
            st.push(i);
        }

        st.clear();
        st.push(n);
        long ans = 0;
        for (int i = n - 1; i >= 0; i--) {
            int x = nums[i];
            // 比如 nums = [...,1,...,1,...]，我们规定，包含左边的 1 的子数组，不能包含右边的 1，从而避免重复统计子数组
            // 注：包含右边的 1 的子数组，可以包含左边的 1
            while (st.size() > 1 && nums[st.peek()] != x && (nums[st.peek()] | x) == x) {
                st.pop();
            }
            int right = st.peek(); // nums[right] 不是 x 的子集
            st.push(i);

            // 子数组左端点可以从 left[i]+1 到 i，一共 i-left[i] 个
            // 子数组右端点可以从 i 到 right-1，一共 right-i 个
            ans += (long) (i - left[i]) * (right - i);
        }

        return ans;
    }
}
```

```java [sol-Java 数组]
class Solution {
    public long countGoodSubarrays(int[] nums) {
        int n = nums.length;
        int[] left = new int[n];
        int[] st = new int[n + 1];
        st[0] = -1;
        int top = 0;
        for (int i = 0; i < n; i++) {
            int x = nums[i];
            while (top > 0 && (nums[st[top]] | x) == x) {
                top--;
            }
            left[i] = st[top]; // nums[left[i]] 不是 x 的子集
            st[++top] = i;
        }

        st[0] = n;
        top = 0;
        long ans = 0;
        for (int i = n - 1; i >= 0; i--) {
            int x = nums[i];
            // 比如 nums = [...,1,...,1,...]，我们规定，包含左边的 1 的子数组，不能包含右边的 1，从而避免重复统计子数组
            // 注：包含右边的 1 的子数组，可以包含左边的 1
            while (top > 0 && nums[st[top]] != x && (nums[st[top]] | x) == x) {
                top--;
            }
            int right = st[top]; // nums[right] 不是 x 的子集
            st[++top] = i;

            // 子数组左端点可以从 left[i]+1 到 i，一共 i-left[i] 个
            // 子数组右端点可以从 i 到 right-1，一共 right-i 个
            ans += (long) (i - left[i]) * (right - i);
        }

        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long countGoodSubarrays(vector<int>& nums) {
        int n = nums.size();
        vector<int> left(n);
        stack<int> st;
        st.push(-1); // 哨兵
        for (int i = 0; i < n; i++) {
            int x = nums[i];
            while (st.size() > 1 && (nums[st.top()] | x) == x) {
                st.pop();
            }
            left[i] = st.top(); // nums[left[i]] 不是 x 的子集
            st.push(i);
        }

        st = stack<int>();
        st.push(n);
        long long ans = 0;
        for (int i = n - 1; i >= 0; i--) {
            int x = nums[i];
            // 比如 nums = [...,1,...,1,...]，我们规定，包含左边的 1 的子数组，不能包含右边的 1，从而避免重复统计子数组
            // 注：包含右边的 1 的子数组，可以包含左边的 1
            while (st.size() > 1 && nums[st.top()] != x && (nums[st.top()] | x) == x) {
                st.pop();
            }
            int right = st.top(); // nums[right] 不是 x 的子集
            st.push(i);

            // 子数组左端点可以从 left[i]+1 到 i，一共 i-left[i] 个
            // 子数组右端点可以从 i 到 right-1，一共 right-i 个
            ans += 1LL * (i - left[i]) * (right - i);
        }

        return ans;
    }
};
```

```go [sol-Go]
func countGoodSubarrays(nums []int) (ans int64) {
	n := len(nums)
	left := make([]int, n)
	st := []int{-1} // 哨兵
	for i, x := range nums {
		for len(st) > 1 && nums[st[len(st)-1]]|x == x {
			st = st[:len(st)-1]
		}
		left[i] = st[len(st)-1] // nums[left[i]] 不是 x 的子集
		st = append(st, i)
	}

	st = []int{n}
	for i := n - 1; i >= 0; i-- {
		x := nums[i]
		// 比如 nums = [...,1,...,1,...]，我们规定，包含左边的 1 的子数组，不能包含右边的 1，从而避免重复统计子数组
		// 注：包含右边的 1 的子数组，可以包含左边的 1
		for len(st) > 1 && nums[st[len(st)-1]] != x && nums[st[len(st)-1]]|x == x {
			st = st[:len(st)-1]
		}
		right := st[len(st)-1] // nums[right] 不是 x 的子集
		st = append(st, i)

		// 子数组左端点可以从 left[i]+1 到 i，一共 i-left[i] 个
		// 子数组右端点可以从 i 到 right-1，一共 right-i 个
		ans += int64(i-left[i]) * int64(right-i)
	}

	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{nums}$ 的长度。虽然我们写了二重循环，但站在 $\textit{nums}[i]$ 的视角看，$i$ 在二重循环中最多入栈出栈各一次（两个二重循环就是两次），因此二重循环的时间复杂度为 $\mathcal{O}(n)$。
- 空间复杂度：$\mathcal{O}(n)$。

## 专题训练

1. 位运算题单的「**AND/OR LogTrick**」。
2. 单调栈题单的「**三、贡献法**」。

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

## 本地原创解析

### 1. 题意重述

本题来自 `三、与或（AND/OR）的性质 / AND/OR LogTrick`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `三、与或（AND/OR）的性质 / AND/OR LogTrick`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
