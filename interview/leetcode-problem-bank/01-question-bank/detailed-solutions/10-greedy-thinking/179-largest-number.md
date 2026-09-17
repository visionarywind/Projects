# 179. 最大数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/largest-number/
- 题目 slug：`largest-number`
- 来源专题：贪心与思维
- 来源分类路径：一、贪心策略 / §1.7 交换论证法
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/largest-number/solutions/3949819/zheng-ming-ti-chuan-di-xing-he-zui-you-x-xbxz/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[证明题：传递性和最优性（Python/Java/C++/Go）](https://leetcode.cn/problems/largest-number/solutions/3949819/zheng-ming-ti-chuan-di-xing-he-zui-you-x-xbxz/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`zheng-ming-ti-chuan-di-xing-he-zui-you-x-xbxz`
- topic id：`3949819`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:54:06 +0800

如果 $\textit{nums}=[a,b]$，只有两个元素，怎么做？

比较 $a+b$ 和 $b+a$ 谁大谁小即可。这里的 $+$ 是字符串拼接。

能否推广这个思路？

**猜想**：把上述规则作为排序时比大小的策略，把 $\textit{nums}$ 排序后，拼接 $\textit{nums}$ 即为最优解。

## 我们需要证明什么？

1. 证明这个规则是没有矛盾的（有传递性），不会出现 $a$ 排在 $b$ 的左边，$b$ 排在 $c$ 的左边，但 $c$ 却排在 $a$ 的左边。
2. 证明按照这个规则排序就够了，足以得到最优解。

## 传递性

对于数字 $x$ 和 $y$，设其十进制长度分别为 $n$ 和 $m$。

- 如果 $x$ 在 $y$ 的左边，拼接得到数字 $x\cdot 10^m + y$。
- 如果 $y$ 在 $x$ 的左边，拼接得到数字 $y\cdot 10^n + x$。

如果 $x\cdot 10^m + y > y\cdot 10^n + x$，移项得

$$
x\cdot 10^m - x > y\cdot 10^n - y
$$

即

$$
x\cdot(10^m - 1) > y\cdot(10^n - 1)
$$

变形得

$$
\dfrac{x}{10^n - 1} > \dfrac{y}{10^m - 1}
$$

设长为 $n$ 的字符串 $s$ 对应的数字为 $x$，定义

$$
f(s) = \dfrac{x}{10^n - 1}
$$

> **注**：根据等比数列求和公式，$f(s) = \dfrac{x}{10^n - 1} = x\cdot(10^{-n} + 10^{-2n} + 10^{-3n}+\cdots)$，即无限循环小数 $0.\overline{x}$，例如 $f(\texttt{12}) = 0.121212\cdots$

对于字符串 $a$ 和 $b$，如果 $f(a)> f(b)$，那么 $a+b> b+a$。

$f(s)$ 是个实数，有传递性。对于字符串 $a,b,c$，如果 $f(a)> f(b)$ 且 $f(b)> f(c)$，那么 $f(a) > f(c)$。这意味着，如果 $a+b> b+a$ 且 $b+c> c+b$，则 $a+c> c+a$。传递性得证。

## 最优性

设按照规则排序后，拼接得到的字符串为 $S = s_1 + s_2 + \cdots + s_n$。由传递性可知

$$
f(s_1) \ge f(s_2) \ge \cdots \ge f(s_n)
$$

下面证明，$S$ 是最优的。

**反证法**。假设最优解为 $T$ 且 $T > S$。换句话说，最优解不是按照 $s_1 + s_2 + \cdots + s_n$ 的顺序拼接的。那么在 $T$ 中存在 $f(s_i) < f(s_{i+1})$，即 $s_i + s_{i+1} < s_{i+1} + s_i$，那么交换 $s_i$ 和 $s_{i+1}$，可以得到一个更大的答案，这与 $T$ 是最优解矛盾。所以 $S$ 是最优的。

## 细节

如果 $\textit{nums}$ 全为 $0$，那么答案是 $\texttt{0}$，而不是 $\texttt{00}\cdots \texttt{0}$。

```py [sol-Python3]
class Solution:
    def largestNumber(self, nums: List[int]) -> str:
        nums = list(map(str, nums))
        nums.sort(key=cmp_to_key(lambda a, b: int(b + a) - int(a + b)))
        return "0" if nums[0] == "0" else ''.join(nums)
```

```java [sol-Java]
class Solution {
    public String largestNumber(int[] nums) {
        // 把 int[] 转成 Integer[] 才能自定义排序
        Integer[] arr = Arrays.stream(nums).boxed().toArray(Integer[]::new);

        Arrays.sort(arr, (a, b) -> {
            String s = String.valueOf(a);
            String t = String.valueOf(b);
            return (t + s).compareTo(s + t);
        });

        if (arr[0] == 0) {
            return "0";
        }

        StringBuilder ans = new StringBuilder();
        for (int x : arr) {
            ans.append(x);
        }
        return ans.toString();
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    string largestNumber(vector<int>& nums) {
        ranges::sort(nums, [&](int a, int b) {
            string s = to_string(a);
            string t = to_string(b);
            return s + t > t + s;
        });

        if (nums[0] == 0) {
            return "0";
        }

        string ans = "";
        for (int x : nums) {
            ans += to_string(x);
        }
        return ans;
    }
};
```

```go [sol-Go]
func largestNumber(nums []int) string {
	slices.SortFunc(nums, func(a, b int) int {
		s := strconv.Itoa(a)
		t := strconv.Itoa(b)
		return cmp.Compare(t+s, s+t)
	})

	if nums[0] == 0 {
		return "0"
	}

	ans := []byte{}
	for _, x := range nums {
		ans = append(ans, strconv.Itoa(x)...)
	}
	return string(ans)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n \log U)$，其中 $n$ 是 $\textit{nums}$ 的长度，$U=\max(\textit{nums})$。排序会执行 $\mathcal{O}(n\log n)$ 次比较，每次比较需要 $\mathcal{O}(\log U)$ 的时间。
- 空间复杂度：$\mathcal{O}(n\log U)$（Python3）或 $\mathcal{O}(\log U)$（其他语言）。不计入排序的栈开销，不计入返回值。

**注**：通过预处理 $2$ 的幂的十进制长度，结合 $\mathcal{O}(1)$ 计算二进制长度的方法，可以做到 $\mathcal{O}(n\log n + n\log U)$ 的时间复杂度。

## 相似题目

- [3309. 连接二进制表示可形成的最大数值](https://leetcode.cn/problems/maximum-possible-number-by-binary-concatenation/)
- [3897. 连接二进制片段得到的最大值](https://leetcode.cn/problems/maximum-value-of-concatenated-binary-segments/)

## 专题训练

见下面贪心题单的「**§1.7 交换论证法**」。

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

本题来自 `一、贪心策略 / §1.7 交换论证法`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、贪心策略 / §1.7 交换论证法`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
