# 3153. 所有数对中数位不同之和

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/sum-of-digit-differences-of-all-pairs/
- 题目 slug：`sum-of-digit-differences-of-all-pairs`
- 来源专题：位运算
- 来源分类路径：四、拆位 / 贡献法
- 难度分：1645
- 外部题解来源：https://leetcode.cn/problems/sum-of-digit-differences-of-all-pairs/solutions/2782811/chai-wei-suan-gong-xian-yi-ci-bian-li-by-46rf/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[拆位法+枚举右维护左（Python/Java/C++/Go）](https://leetcode.cn/problems/sum-of-digit-differences-of-all-pairs/solutions/2782811/chai-wei-suan-gong-xian-yi-ci-bian-li-by-46rf/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`chai-wei-suan-gong-xian-yi-ci-bian-li-by-46rf`
- topic id：`2782811`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

横看成岭侧成峰，换一个角度，把每一位拆开：

- 计算个位数中的不同数对个数；
- 计算十位数中的不同数对个数；
- 计算百位数中的不同数对个数；
- ……

单独考虑每个数位，此时问题变成：

- 给你一个长为 $n$ 的数组 $a$，只包含数字 $0$ 到 $9$，其中有多少个**不同的数对**？

做法有多种，一次遍历的做法如下。

遍历 $a$，同时用一个长为 $10$ 的数组 $\textit{cnt}$ 统计 $0$ 到 $9$ 每个数字的出现次数。假设现在遍历到 $d=a[k]$，那么前面有 $k$ 个数字，其中有 $\textit{cnt}[d]$ 个数和 $d$ 是一样的，所以有

$$
k - \textit{cnt}[d]
$$

个数和 $d$ 是不一样的，这正是我们要统计的，加入答案。

代码实现时，可以外层循环枚举个位数、十位数、百位数等，内层循环枚举 $\textit{nums}$；也可以外层循环枚举 $\textit{nums}$，内层循环枚举个位数、十位数、百位数等。下面代码用的后者。

附：[视频讲解](https://www.bilibili.com/video/BV19D421G7mw/) 第三题，欢迎点赞关注！

### 答疑

**问**：为什么代码要先更新 $\textit{ans}$，再更新 $\textit{cnt}$？

**答**：如果先更新 $\textit{cnt}$，再更新 $\textit{ans}$ 的话，假设 $\textit{nums}=[1]$，这样写会算出 $-1$。或者说，题目要求两个数的下标是不同的，如果先更新 $\textit{cnt}$，就把下标相同的数对也考虑进来了。

```py [sol-Python3]
class Solution:
    def sumDigitDifferences(self, nums: List[int]) -> int:
        ans = 0
        cnt = [[0] * 10 for _ in str(nums[0])]
        for k, x in enumerate(nums):
            i = 0
            while x:
                x, d = divmod(x, 10)
                ans += k - cnt[i][d]
                cnt[i][d] += 1
                i += 1
        return ans
```

```java [sol-Java]
class Solution {
    public long sumDigitDifferences(int[] nums) {
        long ans = 0;
        int[][] cnt = new int[Integer.toString(nums[0]).length()][10];
        for (int k = 0; k < nums.length; k++) {
            int x = nums[k];
            for (int i = 0; x > 0; x /= 10, i++) {
                ans += k - cnt[i][x % 10]++;
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long sumDigitDifferences(vector<int>& nums) {
        long long ans = 0;
        vector<array<int, 10>> cnt(to_string(nums[0]).length());
        for (int k = 0; k < nums.size(); k++) {
            int x = nums[k];
            for (int i = 0; x; x /= 10, i++) {
                ans += k - cnt[i][x % 10]++;
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func sumDigitDifferences(nums []int) (ans int64) {
	cnt := make([][10]int, len(strconv.Itoa(nums[0])))
	for k, x := range nums {
		for i := 0; x > 0; x /= 10 {
			d := x % 10
			ans += int64(k - cnt[i][d])
			cnt[i][d]++
			i++
		}
	}
	return
}
```

也可以逆向思考，设 $\textit{nums}[0]$ 的十进制长度为 $m$，那么总共有

$$
\dfrac{mn(n-1)}{2}
$$

个数对。

在此基础上，减去**相同的数对**。这就和 [1512. 好数对的数目](https://leetcode.cn/problems/number-of-good-pairs/) 完全一样了。

```py [sol-Python3]
class Solution:
    def sumDigitDifferences(self, nums: List[int]) -> int:
        n, m = len(nums), len(str(nums[0]))
        ans = m * n * (n - 1) // 2
        cnt = [[0] * 10 for _ in range(m)]
        for x in nums:
            i = 0
            while x:
                x, d = divmod(x, 10)
                ans -= cnt[i][d]
                cnt[i][d] += 1
                i += 1
        return ans
```

```java [sol-Java]
class Solution {
    public long sumDigitDifferences(int[] nums) {
        int n = nums.length;
        int m = Integer.toString(nums[0]).length();
        long ans = (long) m * n * (n - 1) / 2;
        int[][] cnt = new int[m][10];
        for (int x : nums) {
            for (int i = 0; x > 0; x /= 10) {
                ans -= cnt[i++][x % 10]++;
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long sumDigitDifferences(vector<int>& nums) {
        long long n = nums.size(), m = to_string(nums[0]).length();
        long long ans = m * n * (n - 1) / 2;
        vector<array<int, 10>> cnt(m);
        for (int x : nums) {
            for (int i = 0; x; x /= 10) {
                ans -= cnt[i++][x % 10]++;
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func sumDigitDifferences(nums []int) int64 {
	n, m := len(nums), len(strconv.Itoa(nums[0]))
	ans := m * n * (n - 1) / 2
	cnt := make([][10]int, m)
	for _, x := range nums {
		for i := 0; x > 0; x /= 10 {
			d := x % 10
			ans -= cnt[i][d]
			cnt[i][d]++
			i++
		}
	}
	return int64(ans)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log U)$，其中 $n$ 是 $\textit{nums}$ 的长度，$U=\textit{nums}[0]$。
- 空间复杂度：$\mathcal{O}(D\log U)$，其中 $D=10$。

## 相关题目

- [数据结构题单](https://leetcode.cn/circle/discuss/mOr1u6/) 第零章，枚举右维护左。
- [位运算题单](https://leetcode.cn/circle/discuss/dHn9Vk/) 第四章，拆位/贡献法。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与一般树（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/circle/discuss/K0n2gO/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `四、拆位 / 贡献法`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `四、拆位 / 贡献法`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
