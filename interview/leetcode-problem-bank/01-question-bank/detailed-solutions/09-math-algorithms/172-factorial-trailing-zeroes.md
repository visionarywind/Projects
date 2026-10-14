# 172. 阶乘后的零

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/factorial-trailing-zeroes/
- 题目 slug：`factorial-trailing-zeroes`
- 来源专题：数学算法
- 来源分类路径：一、数论 / §1.4 阶乘分解
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/factorial-trailing-zeroes/solutions/2972637/yan-jin-shu-xue-zheng-ming-pythonjavaccg-fe5t/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[严谨数学证明（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/factorial-trailing-zeroes/solutions/2972637/yan-jin-shu-xue-zheng-ming-pythonjavaccg-fe5t/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`yan-jin-shu-xue-zheng-ming-pythonjavaccg-fe5t`
- topic id：`2972637`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 16:46:02 +0800

例如 $10! = 3628800 = 36288\times 10^2$，尾零个数为 $2$。

一般地，$n!$ 的尾零个数，取决于 $n!$ 可以分解出多少个 $10$。由于 $10=2\times 5$，我们需要知道 $n!$ 中质因子 $2$ 的个数、质因子 $5$ 的个数，二者的较小值，即为 $n!$ 的尾零个数。

下面来计算 $n!$ 的质因数分解中，质数 $p$ 的个数。

- 在 $[1,n]$ 中，有 $\left\lfloor\dfrac{n}{p}\right\rfloor$ 个数可以被 $p$ 整除，其中又有 $\left\lfloor\dfrac{n}{p^2}\right\rfloor$ 个数可以被 $p^2$ 整除，所以**恰好**能被 $p$ 整除，但不能被 $p^2$ 整除的数有 $\left\lfloor\dfrac{n}{p}\right\rfloor - \left\lfloor\dfrac{n}{p^2}\right\rfloor$ 个。例如 $[1,10]$ 中，有 $3$ 个数 $2,6,10$ 恰好能被 $p=2$ 整除。
- 在 $[1,n]$ 中，有 $\left\lfloor\dfrac{n}{p^2}\right\rfloor$ 个数可以被 $p^2$ 整除，其中又有 $\left\lfloor\dfrac{n}{p^3}\right\rfloor$ 个数可以被 $p^3$ 整除，所以**恰好**能被 $p^2$ 整除，但不能被 $p^3$ 整除的数有 $\left\lfloor\dfrac{n}{p^2}\right\rfloor - \left\lfloor\dfrac{n}{p^3}\right\rfloor$ 个。
- 依此类推，$[1,n]$ 中**恰好**能被 $p^{k-1}$ 整除，但不能被 $p^{k}$ 整除的数有 $\left\lfloor\dfrac{n}{p^{k-1}}\right\rfloor - \left\lfloor\dfrac{n}{p^{k}}\right\rfloor$ 个。
- 设 $p^{k}\le n < p^{k+1}$，那么 $[1,n]$ 中**恰好**能被 $p^k$ 整除的数有 $\left\lfloor\dfrac{n}{p^{k}}\right\rfloor$ 个。

继续讨论：

- 恰好能被 $p$ 整除的数，有 $1$ 个质因子 $p$，所以这些数一共有 $1\cdot\left(\left\lfloor\dfrac{n}{p}\right\rfloor - \left\lfloor\dfrac{n}{p^2}\right\rfloor\right)$ 个 $p$。
- 恰好能被 $p^2$ 整除的数，有 $2$ 个质因子 $p$，所以这些数一共有 $2\cdot\left(\left\lfloor\dfrac{n}{p^2}\right\rfloor - \left\lfloor\dfrac{n}{p^3}\right\rfloor\right)$ 个 $p$。
- ……
- 恰好能被 $p^{k-1}$ 整除的数，有 $k-1$ 个质因子 $p$，所以这些数一共有 $(k-1)\cdot\left(\left\lfloor\dfrac{n}{p^{k-1}}\right\rfloor - \left\lfloor\dfrac{n}{p^k}\right\rfloor\right)$ 个 $p$。
- 恰好能被 $p^k$ 整除的数，有 $k$ 个质因子 $p$，所以这些数一共有 $k\cdot \left\lfloor\dfrac{n}{p^{k}}\right\rfloor$ 个 $p$。

累加，即为 $n!$ 的质因数分解中质数 $p$ 的个数：

$$
1\cdot\left(\left\lfloor\dfrac{n}{p}\right\rfloor - \left\lfloor\dfrac{n}{p^2}\right\rfloor\right) + 2\cdot\left(\left\lfloor\dfrac{n}{p^2}\right\rfloor - \left\lfloor\dfrac{n}{p^3}\right\rfloor\right) + \cdots + (k-1)\cdot\left(\left\lfloor\dfrac{n}{p^{k-1}}\right\rfloor - \left\lfloor\dfrac{n}{p^k}\right\rfloor\right) + k\cdot \left\lfloor\dfrac{n}{p^{k}}\right\rfloor
$$

化简得

$$
\left\lfloor\dfrac{n}{p}\right\rfloor + \left\lfloor\dfrac{n}{p^2}\right\rfloor + \cdots + \left\lfloor\dfrac{n}{p^k}\right\rfloor
$$

由于 $p$ 越大，上式越小，所以 $n!$ 中的质因子 $5$ 的个数比 $2$ 少，我们只需计算上式 $p=5$ 的结果。

此外，有如下恒等式

$$
\left\lfloor\dfrac{n}{p^k}\right\rfloor = \left\lfloor\dfrac{ n/{p^{k-1}} }{p}\right\rfloor = \left\lfloor\dfrac{\lfloor n/{p^{k-1}} \rfloor}{p}\right\rfloor
$$

证明见 [下取整恒等式及其应用](https://zhuanlan.zhihu.com/p/1893240318645732760)。

注意该恒等式是一个递推式，我们可以用 $\left\lfloor\dfrac{n}{p}\right\rfloor$ 算出 $\left\lfloor\dfrac{n}{p^2}\right\rfloor$，用 $\left\lfloor\dfrac{n}{p^2}\right\rfloor$ 算出 $\left\lfloor\dfrac{n}{p^3}\right\rfloor$，依此类推。

```py [sol-Python3]
class Solution:
    def trailingZeroes(self, n: int) -> int:
        ans = 0
        while n:
            # 循环 k 次后，n 变成了 floor(n/5^k)
            n //= 5
            ans += n
        return ans
```

```java [sol-Java]
class Solution {
    public int trailingZeroes(int n) {
        int ans = 0;
        while (n > 0) {
            // 循环 k 次后，n 变成了 floor(n/5^k)
            n /= 5;
            ans += n;
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int trailingZeroes(int n) {
        int ans = 0;
        while (n) {
            // 循环 k 次后，n 变成了 floor(n/5^k)
            n /= 5;
            ans += n;
        }
        return ans;
    }
};
```

```c [sol-C]
int trailingZeroes(int n) {
    int ans = 0;
    while (n) {
        // 循环 k 次后，n 变成了 floor(n/5^k)
        n /= 5;
        ans += n;
    }
    return ans;
}
```

```go [sol-Go]
func trailingZeroes(n int) (ans int) {
    for n > 0 {
        // 循环 k 次后，n 变成了 floor(n/5^k)
        n /= 5
        ans += n
    }
    return
}
```

```js [sol-JavaScript]
var trailingZeroes = function(n) {
    let ans = 0;
    while (n > 0) {
        // 循环 k 次后，n 变成了 floor(n/5^k)
        n = Math.floor(n / 5);
        ans += n;
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn trailing_zeroes(mut n: i32) -> i32 {
        let mut ans = 0;
        while n > 0 {
            // 循环 k 次后，n 变成了 floor(n/5^k)
            n /= 5;
            ans += n;
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(\log n)$。
- 空间复杂度：$\mathcal{O}(1)$。

## 相似题目

- [793. 阶乘函数后 K 个零](https://leetcode.cn/problems/preimage-size-of-factorial-zeroes-function/)

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

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)
