# 3411. 最长乘积等价子数组

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-subarray-with-equal-products/
- 题目 slug：`maximum-subarray-with-equal-products`
- 来源专题：数学算法
- 来源分类路径：一、数论 / §1.8 互质
- 难度分：2300
- 外部题解来源：https://leetcode.cn/problems/maximum-subarray-with-equal-products/solutions/3039079/mei-ju-ti-qian-tui-chu-xun-huan-pythonja-a21k/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：枚举/滑动窗口（Python/Java/C++/Go）](https://leetcode.cn/problems/maximum-subarray-with-equal-products/solutions/3039079/mei-ju-ti-qian-tui-chu-xun-huan-pythonja-a21k/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`mei-ju-ti-qian-tui-chu-xun-huan-pythonja-a21k`
- topic id：`3039079`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 方法一：枚举子数组左右端点

暴力枚举所有子数组（左右端点），计算子数组的元素乘积、$\text{LCM}$ 和 $\text{GCD}$。

注意乘积太大会溢出。设 $\textit{allLcm}$ 为所有元素的 $\text{LCM}$，我们可以枚举到子数组乘积超过 $\textit{allLcm}\cdot \max(\textit{nums})$ 为止，这二者分别是 $\text{LCM}$ 的最大值和 $\text{GCD}$ 的最大值。

注意题目保证 $n\ge 2$，又由于两个数一定满足题目要求（见方法二），所以合法子数组是一定存在的，且长度至少是 $2$。

> 注：$\text{LCM}(1,2,\ldots,10)=2520$。

具体请看 [视频讲解](https://www.bilibili.com/video/BV18srKYLEd8/)，欢迎点赞关注~

```py [sol-Python3]
class Solution:
    def maxLength(self, nums: List[int]) -> int:
        max_m = lcm(*nums) * max(nums)
        ans = 0
        for i in range(len(nums)):
            m, l, g = 1, 1, 0
            for j in range(i, len(nums)):
                x = nums[j]
                m *= x
                l = lcm(l, x)
                g = gcd(g, x)
                if m == l * g:
                    ans = max(ans, j - i + 1)
                if m > max_m:
                    break
        return ans
```

```java [sol-Java]
class Solution {
    public int maxLength(int[] nums) {
        int mx = Arrays.stream(nums).max().getAsInt();
        int allLcm = 1;
        for (int x : nums) {
            allLcm = lcm(allLcm, x);
        }

        int ans = 0;
        for (int i = 0; i < nums.length; i++) {
            int m = 1;
            int l = 1;
            int g = 0;
            for (int j = i; j < nums.length && m <= allLcm * mx; j++) {
                int x = nums[j];
                m *= x;
                l = lcm(l, x);
                g = gcd(g, x);
                if (m == l * g) {
                    ans = Math.max(ans, j - i + 1);
                }
            }
        }
        return ans;
    }

    private int gcd(int a, int b) {
        while (a != 0) {
            int tmp = a;
            a = b % a;
            b = tmp;
        }
        return b;
    }

    private int lcm(int a, int b) {
        return a / gcd(a, b) * b;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxLength(vector<int>& nums) {
        int mx = ranges::max(nums);
        int all_lcm = 1;
        for (int x : nums) {
            all_lcm = lcm(all_lcm, x);
        }

        int ans = 0;
        for (int i = 0; i < nums.size(); i++) {
            int m = 1, l = 1, g = 0;
            for (int j = i; j < nums.size() && m <= all_lcm * mx; j++) {
                int x = nums[j];
                m *= x;
                l = lcm(l, x);
                g = gcd(g, x);
                if (m == l * g) {
                    ans = max(ans, j - i + 1);
                }
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func maxLength(nums []int) (ans int) {
	mx := slices.Max(nums)
	allLcm := 1
	for _, x := range nums {
		allLcm = lcm(allLcm, x)
	}

	for i := range nums {
		m, l, g := 1, 1, 0
		for j := i; j < len(nums) && m <= allLcm*mx; j++ {
			x := nums[j]
			m *= x
			l = lcm(l, x)
			g = gcd(g, x)
			if m == l*g {
				ans = max(ans, j-i+1)
			}
		}
	}
	return
}

func gcd(a, b int) int { for a != 0 { a, b = b%a, a }; return b }
func lcm(a, b int) int { return a / gcd(a, b) * b }
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2\log U)$，其中 $n$ 是 $\textit{nums}$ 的长度，$U=\max(\textit{nums})$。
- 空间复杂度：$\mathcal{O}(1)$。

## 方法二：滑动窗口

考虑乘积、$\text{LCM}$ 和 $\text{GCD}$ 的质因数分解中的一个质数 $p$ 的指数 $E$：

- 在子数组乘积中，$E$ 为各个元素的 $p$ 的指数 $e_i$ 之和，即 $E=e_1+e_2+\cdots+e_k$，其中 $k$ 为子数组的长度。
- 在子数组 $\text{LCM}$ 中，$E$ 为各个元素的 $p$ 的指数 $e_i$ 的最大值，即 $E=\max(e_1,e_2,\ldots,e_k)$。
- 在子数组 $\text{GCD}$ 中，$E$ 为各个元素的 $p$ 的指数 $e_i$ 的最小值，即 $E=\min(e_1,e_2,\ldots,e_k)$。

如果 $k=2$，那么 $e_1+e_2 = \max(e_1,e_2) + \min(e_1,e_2)$ 恒成立。所以答案至少是 $2$。

如果 $k=3$，那么 $e_1+e_2+e_3 = \max(e_1,e_2,e_3) + \min(e_1,e_2,e_3)$ 当且仅当至少两个 $e_i=0$ 时成立。证明：

- 如果三个数都是 $0$，那么等式显然成立。
- 如果有两个数是 $0$，设非零的数是 $e_1$，那么等式左边是 $e_1$，右边是 $e_1+0=e_1$，所以等式成立。
- 如果有一个数是 $0$，设非零的数是 $e_1$ 和 $e_2$，那么等式左边是 $e_1+e_2$，右边是 $e_1+0=e_1$，所以等式不成立（左边大于右边）。
- 如果没有 $0$，那么等式左边是 $e_1+e_2+e_3$，右边是 $e_1+e_3$（假设 $e_2$ 是中间大小的数），所以等式不成立（左边大于右边）。

推广到一般情况，当 $k\ge 3$ 时，$e_1+e_2+\cdots+e_k = \max(e_1,e_2,\ldots,e_k) + \min(e_1,e_2,\ldots,e_k)$ 当且仅当至少 $k-1$ 个 $e_i=0$ 时成立。

这意味着，如果 $k\ge 3$，那么这 $k$ 个数必须**两两互质**。如果两个数有大于 $1$ 的公因子，那么不满足「至少 $k-1$ 个 $e_i=0$」的要求。

两两互质是一个很强的性质，我们可以把子数组中的所有数「压缩」到子数组的乘积 $\textit{mul}$ 中，计算 $\textit{mul}$ 与即将加入子数组的数 $x$ 的 $\text{GCD}$，如果 $\text{GCD}(\textit{mul},x)>1$，则说明 $x$ 与子数组的某个数不是互质的。

类似 [3. 无重复字符的最长子串](https://leetcode.cn/problems/longest-substring-without-repeating-characters/) 的 [滑动窗口做法](https://leetcode.cn/problems/longest-substring-without-repeating-characters/solutions/1959540/xia-biao-zong-suan-cuo-qing-kan-zhe-by-e-iaks/)，先去掉子数组中的与 $x$ 不互质的数，再把 $x$ 加到子数组中。

```py [sol-Python3]
class Solution:
    def maxLength(self, nums: List[int]) -> int:
        ans = 2
        mul = 1
        left = 0
        for right, x in enumerate(nums):
            while gcd(mul, x) > 1:
                mul //= nums[left]
                left += 1
            mul *= x
            ans = max(ans, right - left + 1)
        return ans
```

```java [sol-Java]
class Solution {
    public int maxLength(int[] nums) {
        int ans = 2;
        int mul = 1;
        int left = 0;
        for (int right = 0; right < nums.length; right++) {
            while (gcd(mul, nums[right]) > 1) {
                mul /= nums[left];
                left++;
            }
            mul *= nums[right];
            ans = Math.max(ans, right - left + 1);
        }
        return ans;
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
    int maxLength(vector<int>& nums) {
        int ans = 2, mul = 1, left = 0;
        for (int right = 0; right < nums.size(); right++) {
            while (gcd(mul, nums[right]) > 1) {
                mul /= nums[left];
                left++;
            }
            mul *= nums[right];
            ans = max(ans, right - left + 1);
        }
        return ans;
    }
};
```

```go [sol-Go]
func maxLength(nums []int) int {
	ans, mul, left := 2, 1, 0
	for right, x := range nums {
		for gcd(mul, x) > 1 {
			mul /= nums[left]
			left++
		}
		mul *= x
		ans = max(ans, right-left+1)
	}
	return ans
}

func gcd(a, b int) int { for a != 0 { a, b = b%a, a }; return b }
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log U)$，其中 $n$ 是 $\textit{nums}$ 的长度，$U=\max(\textit{nums})$。
- 空间复杂度：$\mathcal{O}(1)$。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

## 本地原创解析

### 1. 题意重述

本题来自 `一、数论 / §1.8 互质`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、数论 / §1.8 互质`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
