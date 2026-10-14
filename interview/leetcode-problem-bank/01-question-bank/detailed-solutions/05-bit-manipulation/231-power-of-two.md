# 231. 2 的幂

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/power-of-two/
- 题目 slug：`power-of-two`
- 来源专题：位运算
- 来源分类路径：一、基础题
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/power-of-two/solutions/2973442/yan-ge-zheng-ming-yi-xing-xie-fa-pythonj-h04o/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[O(1) 位运算，一行写法（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/power-of-two/solutions/2973442/yan-ge-zheng-ming-yi-xing-xie-fa-pythonj-h04o/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`yan-ge-zheng-ming-yi-xing-xie-fa-pythonj-h04o`
- topic id：`2973442`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

如果 $n$ 是 $2$ 的幂，那么 $n$ 的二进制是非常特殊的，形如 $100\cdots 0$。例如 $4=100_{(2)},\ 8=1000_{(2)},\ 16=10000_{(2)}$。（下文二进制省略角标）

如果 $n$ 是 $2$ 的幂，把 $n$ 减一会使 $n$ 的最高位变成 $0$，其余位变成 $1$，所以 $n\&(n-1)$ 一定是 $0$。

例如

$$
\begin{aligned}
8 &= 1000     \\
8-1 &= 0111     \\
\end{aligned}
$$

所以 $8\&(8-1) = 0$。

反之，如果 $n$ 不是 $2$ 的幂，那么 $n$ 的二进制至少有两个 $1$。把 $n$ 减一，并不会影响 $n$ 最高位的 $1$，所以 $n\&(n-1)$ 一定不为 $0$。

例如

$$
\begin{aligned}
9 &= 1001     \\
9-1 &= 1000     \\
\end{aligned}
$$

所以 $9\&(9-1) \ne 0$。

综上所述：

- 如果 $n>0$，可以用 $n\&(n-1) = 0$ 判断。若成立，则 $n$ 是 $2$ 的幂；若不成立，则 $n$ 不是 $2$ 的幂。
- 如果 $n\le 0$，$n$ 不是 $2$ 的幂。

> **注**：$n\&(n-1)$ 相当于去掉 $n$ 二进制的最低位，可以用这个技巧枚举 $n$ 中的比特 $1$。

更多位运算技巧，请看 [从集合论到位运算，常见位运算技巧分类总结！](https://leetcode.cn/circle/discuss/CaOJ45/)

```py [sol-Python3]
class Solution:
    def isPowerOfTwo(self, n: int) -> bool:
        return n > 0 and n & (n - 1) == 0
```

```java [sol-Java]
class Solution {
    public boolean isPowerOfTwo(int n) {
        return n > 0 && (n & (n - 1)) == 0;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    bool isPowerOfTwo(int n) {
        return n > 0 && (n & (n - 1)) == 0;
    }
};
```

```c [sol-C]
bool isPowerOfTwo(int n) {
    return n > 0 && (n & (n - 1)) == 0;
}
```

```go [sol-Go]
func isPowerOfTwo(n int) bool {
    return n > 0 && n&(n-1) == 0
}
```

```js [sol-JavaScript]
var isPowerOfTwo = function(n) {
    return n > 0 && (n & (n - 1)) === 0;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn is_power_of_two(n: i32) -> bool {
        n > 0 && n & (n - 1) == 0
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(1)$。
- 空间复杂度：$\mathcal{O}(1)$。

更多相似题目，见 [位运算题单](https://leetcode.cn/circle/discuss/dHn9Vk/)。

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
