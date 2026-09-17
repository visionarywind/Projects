# 3750. 最少反转次数得到翻转二进制字符串

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-number-of-flips-to-reverse-binary-string/
- 题目 slug：`minimum-number-of-flips-to-reverse-binary-string`
- 来源专题：位运算
- 来源分类路径：一、基础题
- 难度分：1289
- 外部题解来源：https://leetcode.cn/problems/minimum-number-of-flips-to-reverse-binary-string/solutions/3839577/liang-chong-fang-fa-cong-olog-n-dao-o1py-nhiq/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：从 O(log n) 到 O(1)（Python/Java/C++/Go）](https://leetcode.cn/problems/minimum-number-of-flips-to-reverse-binary-string/solutions/3839577/liang-chong-fang-fa-cong-olog-n-dao-o1py-nhiq/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`liang-chong-fang-fa-cong-olog-n-dao-o1py-nhiq`
- topic id：`3839577`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 16:46:02 +0800

## 方法一：转成字符串处理

设 $n$ 的二进制字符串为 $s$，长度为 $m$。

根据题意，遍历 $i=0,1,2,\ldots,n-1$，如果 $s[i]\ne s[n-1-i]$，那么需要反转 $s[i]$，答案加一。

也可以遍历 $i=0,1,2,\ldots,\left\lfloor\dfrac{n}{2}\right\rfloor$，如果 $s[i]\ne s[n-1-i]$，那么需要反转 $s[i]$ 和 $s[n-1-i]$，答案加二。

示例 2 在 $i=0$ 和 $i=1$ 处都满足 $s[i]\ne s[n-1-i]$，答案增加两次 $2$，得到答案 $4$。

[本题视频讲解](https://www.bilibili.com/video/BV1fbUKBqEa7/)，欢迎点赞关注~

```py [sol-Python3]
class Solution:
    def minimumFlips(self, n: int) -> int:
        s = bin(n)[2:]
        ans = 0
        for i in range(len(s) // 2):
            if s[i] != s[-1 - i]:
                ans += 2
        return ans
```

```java [sol-Java]
class Solution {
    public int minimumFlips(int n) {
        char[] s = Long.toBinaryString(n).toCharArray();
        int m = s.length;
        int ans = 0;
        for (int i = 0; i < m / 2; i++) {
            if (s[i] != s[m - 1 - i]) {
                ans += 2;
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minimumFlips(int n) {
        string s = format("{:b}", n); // #include<format>
        int m = s.size();
        int ans = 0;
        for (int i = 0; i < m / 2; i++) {
            if (s[i] != s[m - 1 - i]) {
                ans += 2;
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func minimumFlips(n int) (ans int) {
	s := strconv.FormatUint(uint64(n), 2)
	m := len(s)
	for i := range m / 2 {
		if s[i] != s[m-1-i] {
			ans += 2
		}
	}
	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(\log n)$。其中 $n$ 的二进制长度为 $\mathcal{O}(\log n)$。
- 空间复杂度：$\mathcal{O}(\log n)$。

## 方法二：位运算

根据 [190. 颠倒二进制位](https://leetcode.cn/problems/reverse-bits/)，我们可以 $\mathcal{O}(1)$ 得到 $n$ 反转后的值 $\textit{rev}$。

计算 $n$ 和 $\textit{rev}$ 有多少个位置不同，等价于计算 $n\oplus \textit{rev}$ 的二进制中有多少个 $1$。其中 $\oplus$ 是异或运算，对于每个比特位，两个数不同时结果才是 $1$。

```py [sol-Python3]
class Solution:
    def minimumFlips(self, n: int) -> int:
        rev = self.reverseBits(n) >> (32 - n.bit_length())
        return (n ^ rev).bit_count()

    # 190. 颠倒二进制位
    def reverseBits(self, n: int) -> int:
        # 交换 16 位
        n = ((n >> 16) | (n << 16)) & 0xFFFFFFFF
        # 交换每个 8 位块
        n = (((n & 0xFF00FF00) >> 8) | ((n & 0x00FF00FF) << 8)) & 0xFFFFFFFF
        # 交换每个 4 位块
        n = (((n & 0xF0F0F0F0) >> 4) | ((n & 0x0F0F0F0F) << 4)) & 0xFFFFFFFF
        # 交换每个 2 位块
        n = (((n & 0xCCCCCCCC) >> 2) | ((n & 0x33333333) << 2)) & 0xFFFFFFFF
        # 交换相邻位
        n = (((n & 0xAAAAAAAA) >> 1) | ((n & 0x55555555) << 1)) & 0xFFFFFFFF
        return n
```

```java [sol-Java]
class Solution {
    public int minimumFlips(int n) {
        int rev = Integer.reverse(n) >>> Integer.numberOfLeadingZeros(n);
        return Integer.bitCount(n ^ rev);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minimumFlips(int num) {
        uint32_t n = num;
        uint32_t rev = __builtin_bitreverse32(n) >> countl_zero(n);
        return popcount(n ^ rev);
    }
};
```

```go [sol-Go]
func minimumFlips(num int) int {
	n := uint(num)
	rev := bits.Reverse(n) >> bits.LeadingZeros(n)
	return bits.OnesCount(n ^ rev)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(1)$。
- 空间复杂度：$\mathcal{O}(1)$。

## 专题训练

见下面位运算题单的「**一、基础题**」。

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

## 本地原创解析

### 1. 题意重述

本题来自 `一、基础题`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、基础题`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
