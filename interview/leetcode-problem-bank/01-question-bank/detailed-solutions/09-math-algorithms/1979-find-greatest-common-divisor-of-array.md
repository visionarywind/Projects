# 1979. 找出数组的最大公约数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/find-greatest-common-divisor-of-array/
- 题目 slug：`find-greatest-common-divisor-of-array`
- 来源专题：数学算法
- 来源分类路径：一、数论 / §1.6 最大公约数（GCD）
- 难度分：1184
- 外部题解来源：https://leetcode.cn/problems/find-greatest-common-divisor-of-array/solutions/951154/go-mo-ni-by-endlesscheng-cg8e/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[只需约 1.5n 次比较，就能找到最小值和最大值（Python/Java/C++/Go）](https://leetcode.cn/problems/find-greatest-common-divisor-of-array/solutions/951154/go-mo-ni-by-endlesscheng-cg8e/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`go-mo-ni-by-endlesscheng-cg8e`
- topic id：`951154`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

```py [sol-Python3]
class Solution:
    def findGCD(self, nums: List[int]) -> int:
        return gcd(min(nums), max(nums))
```

```java [sol-Java]
class Solution {
    public int findGCD(int[] nums) {
        int mn = Arrays.stream(nums).min().getAsInt();
        int mx = Arrays.stream(nums).max().getAsInt();
        return gcd(mn, mx);
    }

    private int gcd(int a, int b) {
        while (a != 0) {
            int tmp = a;
            a = b % a;
            b = tmp;
        }
        return b;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int findGCD(vector<int>& nums) {
        return gcd(ranges::min(nums), ranges::max(nums));
    }
};
```

```cpp [sol-C++ 写法二]
class Solution {
public:
    int findGCD(vector<int>& nums) {
        auto [mn, mx] = ranges::minmax(nums);
        return gcd(mn, mx);
    }
};
```

```go [sol-Go]
func findGCD(nums []int) int {
	return gcd(slices.Min(nums), slices.Max(nums))
}

func gcd(a, b int) int {
	for a != 0 {
		a, b = b%a, a
	}
	return b
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n + \log U)$，其中 $n$ 是 $\textit{nums}$ 的长度，$U=\max(\textit{nums})$。计算 GCD 需要 $\mathcal{O}(\log U)$ 的时间。
- 空间复杂度：$\mathcal{O}(1)$。

---

这道题的思路是简单的。借此机会，聊一聊如何更快地找到数组的最小值和最大值。

朴素做法是，先遍历一遍 $\textit{nums}$ 求出最小值，然后再遍历一遍 $\textit{nums}$ 求出最大值。每次遍历，先用 $\textit{nums}[0]$ 作为当前最值，其余 $n-1$ 个数和当前最值比大小（打擂台），如果更小（更大），就更新最值。求出最小值和最大值，一共需要 $2(n-1)$ 次比较。

对于整数来说，比大小的开销是很低的。但如果 $\textit{nums}$ 是一个字符串数组，每次比较两个长长的字符串，开销就很大了。能否减小比较的次数？

设 $x$ 和 $y$ 是遍历过程中遇到的两个元素。在朴素做法中，$x$ 和 $y$ 会与最小值和最大值都比较一次，一共比较 $4$ 次。能不能减少比较次数？

我们可以先比较 $x$ 和 $y$：

- 如果 $x<y$，那么 $x$ 不可能是最大值，$y$ 不可能是最小值。此时只需比较 $x$ 和最小值的大小，以及 $y$ 和最大值的大小。一共比较 $3$ 次。
- 如果 $x>y$，那么 $x$ 不可能是最小值，$y$ 不可能是最大值。此时只需比较 $x$ 和最大值的大小，以及 $y$ 和最小值的大小。一共比较 $3$ 次。
- 对于 $x=y$ 的情况，可以并入 $x<y$ 或者 $x>y$ 的情况中。

一般地，把数组中的元素两两一组，每一组内的两个元素先比一次大小，再把二者的最小值和当前最小值比大小，把二者的最大值和当前最大值比大小。

初始值：

- 如果数组长度 $n$ 是奇数，那么把 $\textit{nums}[0]$ 作为数组最小值和最大值的初始值。其余元素两两一组。
- 如果数组长度 $n$ 是偶数，那么先比较 $\textit{nums}[0]$ 和 $\textit{nums}[1]$ 的大小，作为数组最小值和最大值的初始值。其余元素两两一组。

```py [sol-Python3]
class Solution:
    def findGCD(self, nums: List[int]) -> int:
        n = len(nums)
        if n % 2:
            mn = mx = nums[0]
        elif nums[0] < nums[1]:
            mn, mx = nums[0], nums[1]
        else:
            mn, mx = nums[1], nums[0]

        for i in range(2 - n % 2, n, 2):
            x, y = nums[i], nums[i + 1]
            if x > y:
                x, y = y, x
            if x < mn:
                mn = x
            if y > mx:
                mx = y

        return gcd(mn, mx)
```

```java [sol-Java]
class Solution {
    public int findGCD(int[] nums) {
        int n = nums.length;
        int mn = nums[0];
        int mx = mn;
        if (n % 2 == 0) {
            if (nums[0] < nums[1]) {
                mx = nums[1];
            } else {
                mn = nums[1];
            }
        }

        for (int i = 2 - n % 2; i < n; i += 2) {
            int x = nums[i];
            int y = nums[i + 1];
            if (x > y) {
                x = nums[i + 1];
                y = nums[i];
            }
            if (x < mn) {
                mn = x;
            }
            if (y > mx) {
                mx = y;
            }
        }

        return gcd(mn, mx);
    }

    private int gcd(int a, int b) {
        while (a != 0) {
            int tmp = a;
            a = b % a;
            b = tmp;
        }
        return b;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int findGCD(vector<int>& nums) {
        int n = nums.size();
        int mn = nums[0], mx = nums[0];
        if (n % 2 == 0) {
            if (nums[0] < nums[1]) {
                mx = nums[1];
            } else {
                mn = nums[1];
            }
        }

        for (int i = 2 - n % 2; i < n; i += 2) {
            int x = nums[i], y = nums[i + 1];
            if (x > y) {
                swap(x, y);
            }
            if (x < mn) {
                mn = x;
            }
            if (y > mx) {
                mx = y;
            }
        }

        return gcd(mn, mx);
    }
};
```

```go [sol-Go]
func findGCD(nums []int) int {
	n := len(nums)
	mn, mx := nums[0], nums[0]
	if n%2 == 0 {
		if nums[0] < nums[1] {
			mx = nums[1]
		} else {
			mn = nums[1]
		}
	}

	for i := 2 - n%2; i < n; i += 2 {
		x, y := nums[i], nums[i+1]
		if x > y {
			x, y = y, x
		}
		if x < mn {
			mn = x
		}
		if y > mx {
			mx = y
		}
	}

	return gcd(mn, mx)
}

func gcd(a, b int) int {
	for a != 0 {
		a, b = b%a, a
	}
	return b
}
```

#### 分析比较次数

如果 $n$ 是奇数，那么 $n-1$ 个元素分成 $\dfrac{n-1}{2}$ 组，每组比较 $3$ 次，一共比较 $\dfrac{3(n-1)}{2}$ 次。

如果 $n$ 是偶数，前两个元素比较 $1$ 次，剩余 $n-2$ 个元素比较 $\dfrac{3(n-2)}{2}$ 次，一共比较 $1+\dfrac{3(n-2)}{2}$ 次。

$$
1+\dfrac{3(n-2)}{2} = \dfrac{3n-4}{2} = \dfrac{3(n-1)}{2} - \dfrac{1}{2}
$$

当 $n$ 是偶数时，$3(n-1)$ 是奇数，所以 $\dfrac{3(n-1)}{2}$ 的小数部分是 $0.5$。结合上式可得

$$
\dfrac{3(n-1)}{2} - \dfrac{1}{2} = \left\lfloor\dfrac{3(n-1)}{2}\right\rfloor
$$

所以比较次数可以统一成

$$
\left\lfloor\dfrac{3(n-1)}{2}\right\rfloor
$$

相比朴素算法的 $2(n-1)$，比较次数减少了约 $25\%$。

## 相关题目

[CF730B. Minimum and Maximum](https://codeforces.com/problemset/problem/730/B)

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

本题来自 `一、数论 / §1.6 最大公约数（GCD）`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、数论 / §1.6 最大公约数（GCD）`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
