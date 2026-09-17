# 961. 在长度 2N 的数组中找出重复 N 次的元素

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/n-repeated-element-in-size-2n-array/
- 题目 slug：`n-repeated-element-in-size-2n-array`
- 来源专题：数学算法
- 来源分类路径：六、随机算法 / §6.2 随机化技巧
- 难度分：1162
- 外部题解来源：https://leetcode.cn/problems/n-repeated-element-in-size-2n-array/solutions/3870905/si-chong-fang-fa-ha-xi-ji-he-mo-er-tou-p-f95m/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[四种方法：哈希集合/摩尔投票/邻近元素/随机（Python/Java/C++/Go）](https://leetcode.cn/problems/n-repeated-element-in-size-2n-array/solutions/3870905/si-chong-fang-fa-ha-xi-ji-he-mo-er-tou-p-f95m/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`si-chong-fang-fa-ha-xi-ji-he-mo-er-tou-p-f95m`
- topic id：`3870905`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:37:43 +0800

## 方法一：哈希集合（暴力）

遍历 $\textit{nums}$，同时用一个哈希集合记录遍历过的数。

如果遍历到相同数字（哈希集合中有），由于题目保证只有一个数字是重复的，返回这个数。

```py [sol-Python3]
class Solution:
    def repeatedNTimes(self, nums: List[int]) -> int:
        seen = set()
        for x in nums:
            if x in seen:
                return x
            seen.add(x)
```

```java [sol-Java]
class Solution {
    public int repeatedNTimes(int[] nums) {
        HashSet<Integer> seen = new HashSet<>();
        for (int x : nums) {
            if (!seen.add(x)) { // x 在 seen 中
                return x;
            }
        }
        return -1; // 代码不会执行到这里
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int repeatedNTimes(vector<int>& nums) {
        unordered_set<int> seen;
        for (int x : nums) {
            if (!seen.insert(x).second) { // x 在 seen 中
                return x;
            }
        }
        return -1; // 代码不会执行到这里
    }
};
```

```go [sol-Go]
func repeatedNTimes(nums []int) int {
	seen := map[int]struct{}{}
	for _, x := range nums {
		if _, ok := seen[x]; ok {
			return x
		}
		seen[x] = struct{}{}
	}
	panic(-1)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

如何做到 $\mathcal{O}(1)$ 空间？下面再介绍三种方法。

## 方法二：摩尔投票

请先完成 [169. 多数元素](https://leetcode.cn/problems/majority-element/)，[我的题解](https://leetcode.cn/problems/majority-element/solutions/3744717/on-mo-er-tou-piao-fa-yan-jin-zheng-ming-ww1zv/)。

为了让出现 $n$ 次的那个数变成绝对众数，我们可以分类讨论：

- 如果 $\textit{nums}[0]$ 在下标 $[1,n-1]$ 中出现过，那么返回 $\textit{nums}[0]$。
- 否则，去掉 $\textit{nums}[0]$，剩下 $2n-1$ 个数，出现次数为 $n$ 的那个数变成绝对众数，可以用 169 题的算法解决。

这两件事情可以在同一个循环中完成。

```py [sol-Python3]
class Solution:
    def repeatedNTimes(self, nums: List[int]) -> int:
        ans = hp = 0
        for x in nums[1:]:  # 也可以写 for i in range(1, len(nums)) 避免切片
            if x == nums[0]:
                return x
            if hp == 0:  # x 是初始擂主，生命值为 1
                ans, hp = x, 1
            else:  # 比武，同门加血，否则扣血
                hp += 1 if x == ans else -1
        return ans
```

```java [sol-Java]
class Solution {
    public int repeatedNTimes(int[] nums) {
        int ans = 0;
        int hp = 0;
        for (int i = 1; i < nums.length; i++) {
            int x = nums[i];
            if (x == nums[0]) {
                return x;
            }
            if (hp == 0) { // x 是初始擂主，生命值为 1
                ans = x;
                hp = 1;
            } else { // 比武，同门加血，否则扣血
                hp += x == ans ? 1 : -1;
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int repeatedNTimes(vector<int>& nums) {
        int ans = 0, hp = 0;
        for (int i = 1; i < nums.size(); i++) {
            int x = nums[i];
            if (x == nums[0]) {
                return x;
            }
            if (hp == 0) { // x 是初始擂主，生命值为 1
                ans = x;
                hp = 1;
            } else { // 比武，同门加血，否则扣血
                hp += x == ans ? 1 : -1;
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func repeatedNTimes(nums []int) (ans int) {
	hp := 0
	for _, x := range nums[1:] {
		if x == nums[0] {
			return x
		}
		if hp == 0 { // x 是初始擂主，生命值为 1
			ans, hp = x, 1
		} else if x == ans { // 比武，同门加血，否则扣血
			hp++
		} else {
			hp--
		}
	}
	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

## 方法三：检查邻近元素

设出现次数为 $n$ 的那个数为 $x$。

如果相邻两个 $x$ 之间都至少有一个数，那么 $\textit{nums}$ 至少要有 $2n-1$ 个数，这是合法的。

如果相邻两个 $x$ 之间都至少有两个数，那么 $\textit{nums}$ 至少要有 $3n-2$ 个数。

- 如果 $n=2$，这是合法的，例如 $[3,1,2,3]$。
- 如果 $n\ge 3$，那么 $3n-2 > 2n$，不合法。这意味着，当 $n\ge 3$ 时，一定存在 $\textit{nums}[i] = \textit{nums}[i-1]$ 或者 $\textit{nums}[i] = \textit{nums}[i-2]$。

为了兼容 $n=2$ 的情况，我们可以判断 $\textit{nums}[i]$ 是否与下标 $[i-3, i-1]$ 中的元素相等。

```py [sol-Python3]
class Solution:
    def repeatedNTimes(self, nums: List[int]) -> int:
        for i in range(1, len(nums)):
            x = nums[i]
            if x == nums[i - 1] or \
               i > 1 and x == nums[i - 2] or \
               i > 2 and x == nums[i - 3]:
                return x
```

```java [sol-Java]
class Solution {
    public int repeatedNTimes(int[] nums) {
        for (int i = 1; ; i++) {
            int x = nums[i];
            if (x == nums[i - 1] || 
                i > 1 && x == nums[i - 2] || 
                i > 2 && x == nums[i - 3]) {
                return x;
            }
        }
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int repeatedNTimes(vector<int>& nums) {
        for (int i = 1; ; i++) {
            int x = nums[i];
            if (x == nums[i - 1] || 
                i > 1 && x == nums[i - 2] || 
                i > 2 && x == nums[i - 3]) {
                return x;
            }
        }
    }
};
```

```go [sol-Go]
func repeatedNTimes(nums []int) int {
	for i := 1; ; i++ {
		x := nums[i]
		if x == nums[i-1] ||
			i > 1 && x == nums[i-2] ||
			i > 2 && x == nums[i-3] {
			return x
		}
	}
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

## 方法四：随机

在 $\textit{nums}$ 中随机选择两个下标不同的元素，如果两数相等，即找到了重复元素。

首先，在 $[0,n-1]$ 中随机一个数，作为下标 $i$。

然后，我们要在 $[0,i-1]\cup [i+1,n-1]$ 中随机另一个下标 $j$。

但是，标准库只支持在一个连续范围中随机元素，如何在一个间断的区间中随机元素呢？

考虑**映射**，把 $[0,n-2]$ 映射到 $[0,i-1]\cup [i+1,n-1]$ 中：

$$
f(x) =
\begin{cases} 
x, & 0\le x \le i-1     \\
x+1, & i\le x \le n-2     \\
\end{cases}
$$

具体地，在 $[0,n-2]$ 中随机一个数 $x$：

- 如果 $x < i$，那么把 $x$ 作为下标 $j$。
- 如果 $x \ge i$，那么把 $x+1$ 作为下标 $j$。

```py [sol-Python3]
class Solution:
    def repeatedNTimes(self, nums: List[int]) -> int:
        n = len(nums)
        while True:
            # 在 [0, n-1] 中随机生成两个不同下标
            i = randrange(n)
            j = randrange(n - 1)
            if j >= i:
                j += 1
            if nums[i] == nums[j]:
                return nums[i]
```

```java [sol-Java]
class Solution {
    private static final Random rand = new Random();

    public int repeatedNTimes(int[] nums) {
        int n = nums.length;
        while (true) {
            // 在 [0, n-1] 中随机生成两个不同下标
            int i = rand.nextInt(n);
            int j = rand.nextInt(n - 1);
            if (j >= i) {
                j++;
            }
            if (nums[i] == nums[j]) {
                return nums[i];
            }
        }
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int repeatedNTimes(vector<int>& nums) {
        int n = nums.size();
        mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
        while (true) {
            // 在 [0, n-1] 中随机生成两个不同下标
            int i = uniform_int_distribution<>(0, n - 1)(rng);
            int j = uniform_int_distribution<>(0, n - 2)(rng);
            if (j >= i) {
                j++;
            }
            if (nums[i] == nums[j]) {
                return nums[i];
            }
        }
    }
};
```

```go [sol-Go]
func repeatedNTimes(nums []int) int {
	n := len(nums)
	for {
		// 在 [0, n-1] 中随机生成两个不同下标
		i := rand.Intn(n)
		j := rand.Intn(n - 1)
		if j >= i {
			j++
		}
		if nums[i] == nums[j] {
			return nums[i]
		}
	}
}
```

#### 复杂度分析

- 时间复杂度：期望 $\mathcal{O}(1)$。在 $\textit{nums}$ 中随机选择两个下标不同的元素，两数相等的概率为 $\dfrac{n}{2n}\times \dfrac{n-1}{2n-1}$，当 $n=2$ 时概率为 $p=\dfrac{1}{6}$，当 $n$ 增大时概率 $p\to\dfrac{1}{4}$，期望循环次数 $\dfrac{1}{p} \le 6 = \mathcal{O}(1)$。
- 空间复杂度：$\mathcal{O}(1)$。

## 专题训练

见下面数学题单的「**§6.2 随机化技巧**」。

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

本题来自 `六、随机算法 / §6.2 随机化技巧`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `六、随机算法 / §6.2 随机化技巧`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
