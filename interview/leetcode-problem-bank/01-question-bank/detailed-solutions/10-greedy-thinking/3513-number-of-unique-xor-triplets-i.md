# 3513. 不同 XOR 三元组的数目 I

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/number-of-unique-xor-triplets-i/
- 题目 slug：`number-of-unique-xor-triplets-i`
- 来源专题：贪心与思维
- 来源分类路径：六、构造题
- 难度分：1663
- 外部题解来源：https://leetcode.cn/problems/number-of-unique-xor-triplets-i/solutions/3649390/gou-zao-ti-o1-shi-jian-zuo-fa-pythonjava-ie0a/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[构造题，O(1) 时间做法（Python/Java/C++/Go）](https://leetcode.cn/problems/number-of-unique-xor-triplets-i/solutions/3649390/gou-zao-ti-o1-shi-jian-zuo-fa-pythonjava-ie0a/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`gou-zao-ti-o1-shi-jian-zuo-fa-pythonjava-ie0a`
- topic id：`3649390`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:22:48 +0800

虽然题目要求 $i\le j\le k$，但因为异或运算满足交换律 $a\oplus b = b\oplus a$，实际上我们可以随意选。所以本质上，这题就是从 $\textit{nums}$ 中（可重复地）选三个数。

设 $n$ 为 $\textit{nums}$ 的长度。由于 $\textit{nums}$ 是一个排列，所以 $\textit{nums}$ 包含 $[1,n]$ 中的所有整数。

如果 $n=1$，只能选三个 $1$，异或值为 $1$，答案为 $1$。

如果 $n=2$，三个数中必然有两个数相等（鸽巢原理），这两个数的异或为 $0$，另一个数可以是 $1$ 也可以是 $2$，所以三数异或的结果是 $1$ 或者 $2$，答案为 $2$。

如果 $n\ge 3$，我们可以得到哪些异或值？

- 可以得到 $0$，选择 $1,2,3$ 这三个数。
- 可以得到 $[1,n]$ 中的任意整数 $a$，选择三个 $a$ 即可。
- 可以得到 $[n+1, 2^L-1]$ 中的任意整数 $a$，其中 $L=\left\lfloor \log_2 n\right\rfloor + 1$，也就是 $n$ 的二进制长度。
   - 比如二进制数 $a=1100$，我们可以先选一个最高位 $2^{L-1}=1000$，剩余的二进制数为 $a'=100$，可以通过 $a'\oplus 1 = 101$ 和 $1$ 异或得到。
   - 一般地，选择 $2^{L-1},a\oplus 2^{L-1}\oplus 1, 1$ 这三个数，可以异或得到 $[n+1, 2^L-1]$ 中的任意整数 $a$。
   - 特殊情况，如果 $a=2^{L-1}+1$，那么 $a\oplus 2^{L-1}\oplus 1=0$，可以改为选择 $2^{L-1},3,2$，异或得到 $a$。
- 大于 $2^L-1$ 的数是无法得到的，因为这些数的二进制长度比 $L$ 还长，而 $\textit{nums}$ 中没有数字在第 $> L$ 个比特位上是 $1$。

所以我们能得到 $[0,2^L-1]$ 中的任意整数，这有 $2^L$ 个。

综上所述，如果 $n\le 2$，返回 $n$，否则返回 $2^L$。

具体请看 [视频讲解](https://www.bilibili.com/video/BV1dSdBYEEW6/?t=57s)，欢迎点赞关注~

```py [sol-Python3]
class Solution:
    def uniqueXorTriplets(self, nums: List[int]) -> int:
        n = len(nums)
        return n if n <= 2 else 1 << n.bit_length()
```

```java [sol-Java]
class Solution {
    public int uniqueXorTriplets(int[] nums) {
        int n = nums.length;
        return n <= 2 ? n : 1 << (32 - Integer.numberOfLeadingZeros(n));
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int uniqueXorTriplets(vector<int>& nums) {
        size_t n = nums.size();
        return n <= 2 ? n : 1 << bit_width(n);
    }
};
```

```go [sol-Go]
func uniqueXorTriplets(nums []int) int {
	n := len(nums)
	if n <= 2 {
		return n
	}
	return 1 << bits.Len(uint(n))
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(1)$。
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

本题来自 `六、构造题`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `六、构造题`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
