# 190. 颠倒二进制位

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/reverse-bits/
- 题目 slug：`reverse-bits`
- 来源专题：位运算
- 来源分类路径：一、基础题
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/reverse-bits/solutions/3901287/o1-wei-yun-suan-fen-zhi-yuan-li-jiang-ji-g7c1/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[O(1) 位运算分治，原理讲解（Python/Java/C++/Go）](https://leetcode.cn/problems/reverse-bits/solutions/3901287/o1-wei-yun-suan-fen-zhi-yuan-li-jiang-ji-g7c1/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`o1-wei-yun-suan-fen-zhi-yuan-li-jiang-ji-g7c1`
- topic id：`3901287`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 16:46:02 +0800

以反转一个 $8$ 位整数为例。

为方便阅读，我把这个数字记作 $12345678$。目标是得到 $87654321$。

用分治思考，反转 $12345678$ 可以分成如下三步：

1. 递归反转左半 $1234$，得到 $4321$。
2. 递归反转右半 $5678$，得到 $8765$。
3. 交换 $4321$ 和 $8765$，得到 $87654321$。

反转 $1234$ 可以拆分为反转 $12$ 和 $34$，反转 $5678$ 可以拆分为反转 $56$ 和 $78$。

对于 $12$ 这种长为 $2$ 的情况，交换 $1$ 和 $2$ 即可完成反转。

![无法加载 SVG 图片，请在网页上查看](https://pic.leetcode.cn/1770859136-SSeDqX-lc190.svg)

你可能会问：这样做，算法能更快吗？

利用位运算「并行计算」的特点，我们可以高效地实现上述过程。

去掉递归的「递」，直接看「归」的过程（自底向上）。

递归的最底层是反转 $12$，反转 $34$，反转 $56$，反转 $78$。利用位运算，这些反转可以**同时完成**：

$$
\begin{array}{c}
\text{12345678}  \\
\left\downarrow \rule{0pt}{1.5em} \right. \rlap{\text{分离}} \\
\text{1\phantom{2}3\phantom{4}5\phantom{6}7\phantom{8}}  \\
\text{\phantom{1}2\phantom{3}4\phantom{5}6\phantom{7}8}  \\
\left\downarrow \rule{0pt}{1.5em} \right. \rlap{\text{移位}} \\
\text{\phantom{2}1\phantom{2}3\phantom{4}5\phantom{6}7}  \\
\text{2\phantom{3}4\phantom{5}6\phantom{7}8\phantom{7}}  \\
\left\downarrow \rule{0pt}{1.5em} \right. \rlap{\text{合并}} \\
\text{21436587}  \\
\end{array}
$$

然后两个两个交换：

$$
\begin{array}{c}
\text{21436587}  \\
\left\downarrow \rule{0pt}{1.5em} \right. \rlap{\text{分离}} \\
\text{21\phantom{11}65\phantom{11}}  \\
\text{\phantom{11}43\phantom{11}87}  \\
\left\downarrow \rule{0pt}{1.5em} \right. \rlap{\text{移位}} \\
\text{\phantom{11}21\phantom{11}65}  \\
\text{43\phantom{11}87\phantom{11}}  \\
\left\downarrow \rule{0pt}{1.5em} \right. \rlap{\text{合并}} \\
\text{43218765}  \\
\end{array}
$$

然后四个四个交换：

$$
\begin{array}{c}
\text{43218765}  \\
\left\downarrow \rule{0pt}{1.5em} \right. \rlap{\text{分离}} \\
\text{4321\phantom{1111}}  \\
\text{\phantom{1111}8765}  \\
\left\downarrow \rule{0pt}{1.5em} \right. \rlap{\text{移位}} \\
\text{\phantom{1111}4321}  \\
\text{8765\phantom{1111}}  \\
\left\downarrow \rule{0pt}{1.5em} \right. \rlap{\text{合并}} \\
\text{87654321}  \\
\end{array}
$$

依此类推。

对于 $32$ 位整数，还需要执行八个八个交换，最后把高低 $16$ 位交换。 

```py [sol-Python3]
m0 = 0x55555555  # 01010101 ...
m1 = 0x33333333  # 00110011 ...
m2 = 0x0f0f0f0f  # 00001111 ...
m3 = 0x00ff00ff  # 00000000111111110000000011111111
m4 = 0x0000ffff  # 00000000000000001111111111111111

class Solution:
    def reverseBits(self, n: int) -> int:
        n = n>>1&m0 | (n&m0)<<1  # 交换相邻位
        n = n>>2&m1 | (n&m1)<<2  # 两个两个交换
        n = n>>4&m2 | (n&m2)<<4  # 四个四个交换
        n = n>>8&m3 | (n&m3)<<8  # 八个八个交换
        return n>>16 | (n&m4)<<16  # 交换高低 16 位
```

```java [sol-Java]
class Solution {
    private static final int m0 = 0x55555555; // 01010101 ...
    private static final int m1 = 0x33333333; // 00110011 ...
    private static final int m2 = 0x0f0f0f0f; // 00001111 ...
    private static final int m3 = 0x00ff00ff; // 00000000111111110000000011111111

    public int reverseBits(int n) {
        n = n>>>1&m0 | (n&m0)<<1; // 交换相邻位
        n = n>>>2&m1 | (n&m1)<<2; // 两个两个交换
        n = n>>>4&m2 | (n&m2)<<4; // 四个四个交换
        n = n>>>8&m3 | (n&m3)<<8; // 八个八个交换
        return n>>>16 | n<<16;    // 交换高低 16 位
    }
}
```

```cpp [sol-C++]
class Solution {
    static constexpr uint32_t m0 = 0x55555555; // 01010101 ...
    static constexpr uint32_t m1 = 0x33333333; // 00110011 ...
    static constexpr uint32_t m2 = 0x0f0f0f0f; // 00001111 ...
    static constexpr uint32_t m3 = 0x00ff00ff; // 00000000111111110000000011111111

    uint32_t reverseBits32(uint32_t n) {
        n = n>>1&m0 | (n&m0)<<1; // 交换相邻位
        n = n>>2&m1 | (n&m1)<<2; // 两个两个交换
        n = n>>4&m2 | (n&m2)<<4; // 四个四个交换
        n = n>>8&m3 | (n&m3)<<8; // 八个八个交换
        return n>>16 | n<<16;    // 交换高低 16 位
    }

public:
    int reverseBits(int n) {
        return reverseBits32(n);
    }
};
```

```go [sol-Go]
const m0 = 0x55555555 // 01010101 ...
const m1 = 0x33333333 // 00110011 ...
const m2 = 0x0f0f0f0f // 00001111 ...
const m3 = 0x00ff00ff // 00000000111111110000000011111111
const m4 = 0x0000ffff // 00000000000000001111111111111111

func reverseBits(n int) int {
	n = n>>1&m0 | n&m0<<1   // 交换相邻位
	n = n>>2&m1 | n&m1<<2   // 两个两个交换
	n = n>>4&m2 | n&m2<<4   // 四个四个交换
	n = n>>8&m3 | n&m3<<8   // 八个八个交换
	return n>>16 | n&m4<<16 // 交换高低 16 位
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(1)$。无论输入的是 $0$ 还是 $2^{31}-2$，计算量没有任何区别。更精细地说，时间复杂度是 $\mathcal{O}(\log W)$，其中 $W=32$ 是位宽。
- 空间复杂度：$\mathcal{O}(1)$。

## 附：库函数写法

```py [sol-Python3]
class Solution:
    def reverseBits(self, n: int) -> int:
        # 没有 O(1) 的库函数，只能用字符串转换代替
        # 032b 中的 b 表示转成二进制串，032 表示补前导零到长度等于 32
        return int(f'{n:032b}'[::-1], 2)
```

```py [sol-Python3 写法二]
class Solution:
    def reverseBits(self, n: int) -> int:
        # 没有 O(1) 的库函数，只能用字符串转换代替
        return int(bin(n)[2:].zfill(32)[::-1], 2)
```

```java [sol-Java]
class Solution {
    public int reverseBits(int n) {
        return Integer.reverse(n);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int reverseBits(int n) {
        return __builtin_bitreverse32(n);
    }
};
```

```go [sol-Go]
func reverseBits(n int) int {
	return int(bits.Reverse32(uint32(n)))
}
```

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
