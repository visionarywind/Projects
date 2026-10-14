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
