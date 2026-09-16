# 3578. 统计极差最大为 K 的分割方式数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/count-partitions-with-max-min-difference-at-most-k/
- 题目 slug：`count-partitions-with-max-min-difference-at-most-k`
- 来源专题：动态规划
- 来源分类路径：五、划分型 DP / §5.2 最优划分
- 难度分：2033
- 外部题解来源：https://leetcode.cn/problems/count-partitions-with-max-min-difference-at-most-k/solutions/3695716/hua-fen-xing-dp-dan-diao-dui-lie-you-hua-9rtj/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[划分型 DP + 单调队列优化（Python/Java/C++/Go）](https://leetcode.cn/problems/count-partitions-with-max-min-difference-at-most-k/solutions/3695716/hua-fen-xing-dp-dan-diao-dui-lie-you-hua-9rtj/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`hua-fen-xing-dp-dan-diao-dui-lie-you-hua-9rtj`
- topic id：`3695716`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## O(n^2) 做法

本题是标准的划分型 DP，见 [DP 题单](https://leetcode.cn/circle/discuss/tXLS3i/) 的「§5.2 最优划分」。

一般定义 $f[i+1]$ 表示前缀 $\textit{nums}[0]$ 到 $\textit{nums}[i]$ 在题目约束下，分割出的最少（最多）子数组个数，本题是定义成分割方案数。这里 $i+1$ 是为了把 $f[0]$ 当作初始值。

枚举最后一个子数组的左端点 $j$，那么问题变成前缀 $\textit{nums}[0]$ 到 $\textit{nums}[j-1]$ 在题目约束下的分割方案数，即 $f[j]$。

当子数组右端点 $i$ 固定时，由于子数组越长，最大值越大，最小值越小，最大最小的差值越可能大于 $k$。所以符合要求的左端点 $j$ 一定在一个**连续区间** $[L,i]$ 中。累加 $f[j]$ 得

$$
f[i+1] = \sum_{j=L}^{i} f[j]
$$

初始值 $f[0] = 1$，空子数组算一个方案。也可以从递归的角度理解，递归到空子数组，就表示我们找到了一个合法分割方案。

答案为 $f[n]$。

## O(n) 做法

由于 $i$ 越大，$L$ 也越大，可以用 [滑动窗口【基础算法精讲 03】](https://www.bilibili.com/video/BV1hd4y1r7Gq/)。

同时，我们需要计算 [239. 滑动窗口最大值](https://leetcode.cn/problems/sliding-window-maximum/) 和滑动窗口最小值，这可以用 [单调队列【基础算法精讲 27】](https://www.bilibili.com/video/BV1bM411X72E/)解决。

维护窗口中的 $\displaystyle\sum_{j=L}^{i} f[j]$，记作 $\textit{sumF}$，转移方程优化成

$$
f[i+1] = \textit{sumF}
$$

注意取模。关于模运算的知识点，见 [模运算的世界：当加减乘除遇上取模](https://leetcode.cn/circle/discuss/mDfnkW/)。

[本题视频讲解](https://www.bilibili.com/video/BV113T9zFEjQ/?t=13m17s)，欢迎点赞关注~

```py [sol-Python3]
class Solution:
    def countPartitions(self, nums: List[int], k: int) -> int:
        MOD = 1_000_000_007
        n = len(nums)
        min_q = deque()
        max_q = deque()
        f = [0] * (n + 1)
        f[0] = 1
        sum_f = 0  # 窗口中的 f[i] 之和
        left = 0

        for i, x in enumerate(nums):
            # 1. 入
            sum_f += f[i]

            while min_q and x <= nums[min_q[-1]]:
                min_q.pop()
            min_q.append(i)

            while max_q and x >= nums[max_q[-1]]:
                max_q.pop()
            max_q.append(i)

            # 2. 出
            while nums[max_q[0]] - nums[min_q[0]] > k:
                sum_f -= f[left]
                left += 1
                if min_q[0] < left:
                    min_q.popleft()
                if max_q[0] < left:
                    max_q.popleft()

            # 3. 更新答案
            f[i + 1] = sum_f % MOD

        return f[n]
```

```java [sol-Java]
class Solution {
    public int countPartitions(int[] nums, int k) {
        final int MOD = 1_000_000_007;
        int n = nums.length;
        Deque<Integer> minQ = new ArrayDeque<>(); // 更快的写法见【Java 数组】
        Deque<Integer> maxQ = new ArrayDeque<>();
        int[] f = new int[n + 1];
        f[0] = 1;
        long sumF = 0; // 窗口中的 f[i] 之和
        int left = 0;

        for (int i = 0; i < n; i++) {
            // 1. 入
            sumF += f[i];

            int x = nums[i];
            while (!minQ.isEmpty() && x <= nums[minQ.peekLast()]) {
                minQ.pollLast();
            }
            minQ.addLast(i);

            while (!maxQ.isEmpty() && x >= nums[maxQ.peekLast()]) {
                maxQ.pollLast();
            }
            maxQ.addLast(i);

            // 2. 出
            while (nums[maxQ.peekFirst()] - nums[minQ.peekFirst()] > k) {
                sumF -= f[left];
                left++;
                if (minQ.peekFirst() < left) {
                    minQ.pollFirst();
                }
                if (maxQ.peekFirst() < left) {
                    maxQ.pollFirst();
                }
            }

            // 3. 更新答案
            f[i + 1] = (int) (sumF % MOD);
        }

        return f[n];
    }
}
```

```java [sol-Java 数组]
class Solution {
    public int countPartitions(int[] nums, int k) {
        final int MOD = 1_000_000_007;
        int n = nums.length;
        int[] minQ = new int[n];
        int[] maxQ = new int[n];
        int minHead = 0, minTail = -1;
        int maxHead = 0, maxTail = -1;
        int[] f = new int[n + 1];
        f[0] = 1;
        long sumF = 0; // 窗口中的 f[i] 之和
        int left = 0;

        for (int i = 0; i < n; i++) {
            // 1. 入
            sumF += f[i];

            int x = nums[i];
            while (minHead <= minTail && x <= nums[minQ[minTail]]) {
                minTail--;
            }
            minQ[++minTail] = i;

            while (maxHead <= maxTail && x >= nums[maxQ[maxTail]]) {
                maxTail--;
            }
            maxQ[++maxTail] = i;

            // 2. 出
            while (nums[maxQ[maxHead]] - nums[minQ[minHead]] > k) {
                sumF -= f[left];
                left++;
                if (minQ[minHead] < left) {
                    minHead++;
                }
                if (maxQ[maxHead] < left) {
                    maxHead++;
                }
            }

            // 3. 更新答案
            f[i + 1] = (int) (sumF % MOD);
        }

        return f[n];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int countPartitions(vector<int>& nums, int k) {
        const int MOD = 1'000'000'007;
        int n = nums.size();
        deque<int> min_q, max_q;
        vector<int> f(n + 1);
        f[0] = 1;
        long long sum_f = 0; // 窗口中的 f[i] 之和
        int left = 0;

        for (int i = 0; i < n; i++) {
            int x = nums[i];
            // 1. 入
            sum_f += f[i];

            while (!min_q.empty() && x <= nums[min_q.back()]) {
                min_q.pop_back();
            }
            min_q.push_back(i);

            while (!max_q.empty() && x >= nums[max_q.back()]) {
                max_q.pop_back();
            }
            max_q.push_back(i);

            // 2. 出
            while (nums[max_q.front()] - nums[min_q.front()] > k) {
                sum_f -= f[left];
                left++;
                if (min_q.front() < left) {
                    min_q.pop_front();
                }
                if (max_q.front() < left) {
                    max_q.pop_front();
                }
            }

            // 3. 更新答案
            f[i + 1] = sum_f % MOD;
        }

        return f[n];
    }
};
```

```go [sol-Go]
func countPartitions(nums []int, k int) int {
	const mod = 1_000_000_007
	n := len(nums)
	var minQ, maxQ []int
	f := make([]int, n+1)
	f[0] = 1
	sumF := 0 // 窗口中的 f[i] 之和
	left := 0

	for i, x := range nums {
		// 1. 入
		sumF += f[i]

		for len(minQ) > 0 && x <= nums[minQ[len(minQ)-1]] {
			minQ = minQ[:len(minQ)-1]
		}
		minQ = append(minQ, i)

		for len(maxQ) > 0 && x >= nums[maxQ[len(maxQ)-1]] {
			maxQ = maxQ[:len(maxQ)-1]
		}
		maxQ = append(maxQ, i)

		// 2. 出
		for nums[maxQ[0]]-nums[minQ[0]] > k {
			sumF -= f[left]
			left++
			if minQ[0] < left {
				minQ = minQ[1:]
			}
			if maxQ[0] < left {
				maxQ = maxQ[1:]
			}
		}

		// 3. 更新答案
		f[i+1] = sumF % mod
	}
	return f[n]
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{nums}$ 的长度。每个下标至多入队出队各两次。
- 空间复杂度：$\mathcal{O}(n)$。

## 相似题目

- [2762. 不间断子数组](https://leetcode.cn/problems/continuous-subarrays/)

更多相似题目，见下面动态规划题单的「**§5.2 最优划分**」和「**§11.3 单调队列优化 DP**」。

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

## O(n^2) 做法

本题是标准的划分型 DP，见 [DP 题单](https://leetcode.cn/circle/discuss/tXLS3i/) 的「§5.2 最优划分」。

一般定义 $f[i+1]$ 表示前缀 $\textit{nums}[0]$ 到 $\textit{nums}[i]$ 在题目约束下，分割出的最少（最多）子数组个数，本题是定义成分割方案数。这里 $i+1$ 是为了把 $f[0]$ 当作初始值。

枚举最后一个子数组的左端点 $j$，那么问题变成前缀 $\textit{nums}[0]$ 到 $\textit{nums}[j-1]$ 在题目约束下的分割方案数，即 $f[j]$。

当子数组右端点 $i$ 固定时，由于子数组越长，最大值越大，最小值越小，最大最小的差值越可能大于 $k$。所以符合要求的左端点 $j$ 一定在一个**连续区间** $[L,i]$ 中。累加 $f[j]$ 得

$$
f[i+1] = \sum_{j=L}^{i} f[j]
$$

初始值 $f[0] = 1$，空子数组算一个方案。也可以从递归的角度理解，递归到空子数组，就表示我们找到了一个合法分割方案。

答案为 $f[n]$。

## O(n) 做法

由于 $i$ 越大，$L$ 也越大，可以用 [滑动窗口【基础算法精讲 03】](https://www.bilibili.com/video/BV1hd4y1r7Gq/)。

同时，我们需要计算 [239. 滑动窗口最大值](https://leetcode.cn/problems/sliding-window-maximum/) 和滑动窗口最小值，这可以用 [单调队列【基础算法精讲 27】](https://www.bilibili.com/video/BV1bM411X72E/)解决。

维护窗口中的 $\displaystyle\sum_{j=L}^{i} f[j]$，记作 $\textit{sumF}$，转移方程优化成

$$
f[i+1] = \textit{sumF}
$$

注意取模。关于模运算的知识点，见 [模运算的世界：当加减乘除遇上取模](https://leetcode.cn/circle/discuss/mDfnkW/)。

[本题视频讲解](https://www.bilibili.com/video/BV113T9zFEjQ/?t=13m17s)，欢迎点赞关注~

```py [sol-Python3]
class Solution:
    def countPartitions(self, nums: List[int], k: int) -> int:
        MOD = 1_000_000_007
        n = len(nums)
        min_q = deque()
        max_q = deque()
        f = [0] * (n + 1)
        f[0] = 1
        sum_f = 0  # 窗口中的 f[i] 之和
        left = 0

        for i, x in enumerate(nums):
            # 1. 入
            sum_f += f[i]

            while min_q and x <= nums[min_q[-1]]:
                min_q.pop()
            min_q.append(i)

            while max_q and x >= nums[max_q[-1]]:
                max_q.pop()
            max_q.append(i)

            # 2. 出
            while nums[max_q[0]] - nums[min_q[0]] > k:
                sum_f -= f[left]
                left += 1
                if min_q[0] < left:
                    min_q.popleft()
                if max_q[0] < left:
                    max_q.popleft()

            # 3. 更新答案
            f[i + 1] = sum_f % MOD

        return f[n]
```

```java [sol-Java]
class Solution {
    public int countPartitions(int[] nums, int k) {
        final int MOD = 1_000_000_007;
        int n = nums.length;
        Deque<Integer> minQ = new ArrayDeque<>(); // 更快的写法见【Java 数组】
        Deque<Integer> maxQ = new ArrayDeque<>();
        int[] f = new int[n + 1];
        f[0] = 1;
        long sumF = 0; // 窗口中的 f[i] 之和
        int left = 0;

        for (int i = 0; i < n; i++) {
            // 1. 入
            sumF += f[i];

            int x = nums[i];
            while (!minQ.isEmpty() && x <= nums[minQ.peekLast()]) {
                minQ.pollLast();
            }
            minQ.addLast(i);

            while (!maxQ.isEmpty() && x >= nums[maxQ.peekLast()]) {
                maxQ.pollLast();
            }
            maxQ.addLast(i);

            // 2. 出
            while (nums[maxQ.peekFirst()] - nums[minQ.peekFirst()] > k) {
                sumF -= f[left];
                left++;
                if (minQ.peekFirst() < left) {
                    minQ.pollFirst();
                }
                if (maxQ.peekFirst() < left) {
                    maxQ.pollFirst();
                }
            }

            // 3. 更新答案
            f[i + 1] = (int) (sumF % MOD);
        }

        return f[n];
    }
}
```

```java [sol-Java 数组]
class Solution {
    public int countPartitions(int[] nums, int k) {
        final int MOD = 1_000_000_007;
        int n = nums.length;
        int[] minQ = new int[n];
        int[] maxQ = new int[n];
        int minHead = 0, minTail = -1;
        int maxHead = 0, maxTail = -1;
        int[] f = new int[n + 1];
        f[0] = 1;
        long sumF = 0; // 窗口中的 f[i] 之和
        int left = 0;

        for (int i = 0; i < n; i++) {
            // 1. 入
            sumF += f[i];

            int x = nums[i];
            while (minHead <= minTail && x <= nums[minQ[minTail]]) {
                minTail--;
            }
            minQ[++minTail] = i;

            while (maxHead <= maxTail && x >= nums[maxQ[maxTail]]) {
                maxTail--;
            }
            maxQ[++maxTail] = i;

            // 2. 出
            while (nums[maxQ[maxHead]] - nums[minQ[minHead]] > k) {
                sumF -= f[left];
                left++;
                if (minQ[minHead] < left) {
                    minHead++;
                }
                if (maxQ[maxHead] < left) {
                    maxHead++;
                }
            }

            // 3. 更新答案
            f[i + 1] = (int) (sumF % MOD);
        }

        return f[n];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int countPartitions(vector<int>& nums, int k) {
        const int MOD = 1'000'000'007;
        int n = nums.size();
        deque<int> min_q, max_q;
        vector<int> f(n + 1);
        f[0] = 1;
        long long sum_f = 0; // 窗口中的 f[i] 之和
        int left = 0;

        for (int i = 0; i < n; i++) {
            int x = nums[i];
            // 1. 入
            sum_f += f[i];

            while (!min_q.empty() && x <= nums[min_q.back()]) {
                min_q.pop_back();
            }
            min_q.push_back(i);

            while (!max_q.empty() && x >= nums[max_q.back()]) {
                max_q.pop_back();
            }
            max_q.push_back(i);

            // 2. 出
            while (nums[max_q.front()] - nums[min_q.front()] > k) {
                sum_f -= f[left];
                left++;
                if (min_q.front() < left) {
                    min_q.pop_front();
                }
                if (max_q.front() < left) {
                    max_q.pop_front();
                }
            }

            // 3. 更新答案
            f[i + 1] = sum_f % MOD;
        }

        return f[n];
    }
};
```

```go [sol-Go]
func countPartitions(nums []int, k int) int {
	const mod = 1_000_000_007
	n := len(nums)
	var minQ, maxQ []int
	f := make([]int, n+1)
	f[0] = 1
	sumF := 0 // 窗口中的 f[i] 之和
	left := 0

	for i, x := range nums {
		// 1. 入
		sumF += f[i]

		for len(minQ) > 0 && x <= nums[minQ[len(minQ)-1]] {
			minQ = minQ[:len(minQ)-1]
		}
		minQ = append(minQ, i)

		for len(maxQ) > 0 && x >= nums[maxQ[len(maxQ)-1]] {
			maxQ = maxQ[:len(maxQ)-1]
		}
		maxQ = append(maxQ, i)

		// 2. 出
		for nums[maxQ[0]]-nums[minQ[0]] > k {
			sumF -= f[left]
			left++
			if minQ[0] < left {
				minQ = minQ[1:]
			}
			if maxQ[0] < left {
				maxQ = maxQ[1:]
			}
		}

		// 3. 更新答案
		f[i+1] = sumF % mod
	}
	return f[n]
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{nums}$ 的长度。每个下标至多入队出队各两次。
- 空间复杂度：$\mathcal{O}(n)$。

## 相似题目

- [2762. 不间断子数组](https://leetcode.cn/problems/continuous-subarrays/)

更多相似题目，见下面动态规划题单的「**§5.2 最优划分**」和「**§11.3 单调队列优化 DP**」。

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

## 本地原创解析

### 1. 题意重述

本题来自 `五、划分型 DP / §5.2 最优划分`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `五、划分型 DP / §5.2 最优划分`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
