# 201. 数字范围按位与

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/bitwise-and-of-numbers-range/
- 题目 slug：`bitwise-and-of-numbers-range`
- 来源专题：位运算
- 来源分类路径：九、其他
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/bitwise-and-of-numbers-range/solutions/538550/golang-yi-xing-suan-fa-by-endlesscheng-iw6y/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[O(1) 写法（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/bitwise-and-of-numbers-range/solutions/538550/golang-yi-xing-suan-fa-by-endlesscheng-iw6y/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`golang-yi-xing-suan-fa-by-endlesscheng-iw6y`
- topic id：`538550`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 举例

$$
\begin{aligned}
\textit{left}  &= 110010      \\
\textit{right} &= 110110     \\
\end{aligned}
$$

在 $[\textit{left},\textit{right}]$ 中的二进制数为

$$
\begin{aligned}
110010   \\
110011   \\
110100   \\
110101   \\
110110   \\
\end{aligned}
$$

这些二进制数的公共前缀都是 $110$，对于后三位，由于包含了

$$
\begin{aligned}
110011   \\
110100   \\
\end{aligned}
$$

这两个数，所以后三位的 AND 一定是 $0$。

所以答案是 $110000$。

一般地，把 $\textit{left}$（或者 $\textit{right}$）的后三位置为 $0$，就可以得到最终答案了。

## 怎么求出需要置 0 的长度？

注意 $110\underline{0}10$ 和 $110\underline{1}10$ 画下划线的地方，这是它们俩从左往右看，首次不同的位置。

这可以通过计算 $\textit{left}$ 和 $\textit{right}$ 的 XOR 得到，即 $100$。

计算 $100$ 的长度（$3$），就是我们要置为 $0$ 的长度了。

## 怎么用位运算置 0？

设 $\textit{left}$ 和 $\textit{right}$ 的 XOR 的长度为 $m$。

根据 [从集合论到位运算](https://leetcode.cn/circle/discuss/CaOJ45/)，我们相当于去掉集合 $\textit{left}$ 中的元素 $0,1,2,\ldots,m-1$。

元素 $0,1,2,\ldots,m-1$ 组成的集合为 $2^m-1$。

所以答案为 

$$
\textit{left}\ \&\sim (2^m-1)
$$

写成代码就是：

```py [sol-Python3]
class Solution:
    def rangeBitwiseAnd(self, left: int, right: int) -> int:
        m = (left ^ right).bit_length()
        return left & ~((1 << m) - 1)
```

```java [sol-Java]
class Solution {
    public int rangeBitwiseAnd(int left, int right) {
        int m = 32 - Integer.numberOfLeadingZeros(left ^ right);
        return left & ~((1 << m) - 1);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int rangeBitwiseAnd(int left, int right) {
        int m = bit_width((uint32_t) left ^ right);
        return left & ~((1u << m) - 1);
    }
};
```

```c [sol-C]
int rangeBitwiseAnd(int left, int right) {
    // 0 的二进制长度为 0
    int m = left == right ? 0 : 32 - __builtin_clz(left ^ right);
    return left & ~((1u << m) - 1);
}
```

```go [sol-Go]
func rangeBitwiseAnd(left, right int) int {
    m := bits.Len(uint(left ^ right))
    return left &^ (1<<m - 1)
}
```

```js [sol-JavaScript]
var rangeBitwiseAnd = function(left, right) {
    const m = 32 - Math.clz32(left ^ right);
    return left & ~((1 << m) - 1);
};
```

```rust [sol-Rust]
impl Solution {
    pub fn range_bitwise_and(left: i32, right: i32) -> i32 {
        let m = 32 - (left ^ right).leading_zeros();
        left & !((1 << m) - 1)
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(1)$。
- 空间复杂度：$\mathcal{O}(1)$。

## 思考题

1. 计算 $[\textit{left},\textit{right}]$ 所有元素的 OR（按位或）。见 [CF1981B](https://codeforces.com/contest/1981/problem/B)。
2. 计算 $[\textit{left},\textit{right}]$ 所有元素的 XOR（按位异或）。见 [1486. 数组异或操作](https://leetcode.cn/problems/xor-operation-in-an-array/)。

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

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)
