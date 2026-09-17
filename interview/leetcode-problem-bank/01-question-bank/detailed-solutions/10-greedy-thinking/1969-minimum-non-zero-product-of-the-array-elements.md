# 1969. 数组元素的最小非零乘积

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-non-zero-product-of-the-array-elements/
- 题目 slug：`minimum-non-zero-product-of-the-array-elements`
- 来源专题：贪心与思维
- 来源分类路径：四、数学贪心 / §4.4 均值不等式
- 难度分：1967
- 外部题解来源：https://leetcode.cn/problems/minimum-non-zero-product-of-the-array-elements/solutions/936621/tan-xin-ji-qi-shu-xue-zheng-ming-by-endl-uumv/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[贪心+构造（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/minimum-non-zero-product-of-the-array-elements/solutions/936621/tan-xin-ji-qi-shu-xue-zheng-ming-by-endl-uumv/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tan-xin-ji-qi-shu-xue-zheng-ming-by-endl-uumv`
- topic id：`936621`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:01:04 +0800

## 贪心

例如 $x=1000,\ y=0111$，如果把 $y$ 中最低位以外的 $1$ 全部给 $x$，那么 $x'=1110,\ y'=0001$，这样两数乘积最小且**非零**。

为什么这样做乘积是最小的？

不失一般性，假设 $x$ 参与交换的比特为 $0$，$y$ 参与交换的比特为 $1$，交换的位置为第 $k$ 位。

记 $y=y'+2^k$。交换前，两数的乘积为

$$
x\cdot y = x\cdot (y'+2^k) = x\cdot y'+x\cdot 2^k
$$

交换后，$x$ 变成 $x+2^k$，$y$ 变成 $y'$。两数的乘积为

$$
(x+2^k)\cdot y' = x\cdot y'+y'\cdot 2^k
$$

对比两个等式可知，满足

$$
x>y'
$$

就可以使交换后的乘积变小。

所以我们不断地将 $y$ 中的 $1$ 与 $x$ 中的 $0$ 交换，就可以将乘积不断减小。由于题目要求乘积不能为 $0$，我们可以先将 $y$ 减小至 $0$，然后再寻找一个最低位为 $1$ 的数交换，从而让 $y$ 变成 $1$。

## 构造

$\textit{nums}$ 包含了 $[1, 2^p - 1]$ 内的所有整数，我们将这些数分为两组，小于 $2^{p-1}$ 的为一组，记作 $A$，其余的为另一组，记作 $B$。

例如 $p=3$ 时 $A=[1,2,3],\ B=[4,5,6,7]$。

$B$ 中除了 $2^p-1=7$ 以外，其余的数均可以和 $A$ 组中的数一一配对，其中配对的两个数之和为 $2^p-1=7$。

例如 $p=3$ 时我们有三个数对 $(6,1),(5,2),(4,3)$，写成二进制为 $(110,001),(101,010),(100,011)$。

如此构造，数对内没有相同的比特位（一个是 $0$ 另一个必然是 $1$），我们可以**完美地**按照上述交换流程交换，交换后的结果为 $2^p-2=6$ 和 $1$。

交换后，每一对的乘积为 $2^p-2=6$，这一共有 $2^{p-1}-1=3$ 对，再乘上不参与配对的 $2^p-1=7$，得到最小乘积为 $7\times 6\times 6\times 6=7\times 6^3=1512$（对比一下，交换前的乘积为 $7!=5040$）。

一般地，最小乘积为

$$
(2^p-1)\cdot (2^p-2)^{2^{p-1}-1}
$$

由于幂次很大，计算时需要用到快速幂，请看[【图解】一张图秒懂快速幂](https://leetcode.cn/problems/powx-n/solution/tu-jie-yi-zhang-tu-miao-dong-kuai-su-mi-ykp3i/)。

注意，由于 $2^{p-1}-1$ 的二进制全是 $1$，下面写法去掉了快速幂中的 `if` 判断。

```py [sol-Python3]
class Solution:
    def minNonZeroProduct(self, p: int) -> int:
        MOD = 1_000_000_007
        k = (1 << p) - 1
        return k * pow(k - 1, k >> 1, MOD) % MOD
```

```java [sol-Java]
class Solution {
    private static final int MOD = 1_000_000_007;

    private long pow(long x, int p) {
        x %= MOD;
        long res = 1;
        while (p-- > 0) {
            res = res * x % MOD;
            x = x * x % MOD;
        }
        return res;
    }

    public int minNonZeroProduct(int p) {
        long k = (1L << p) - 1;
        return (int) (k % MOD * pow(k - 1, p - 1) % MOD);
    }
}
```

```cpp [sol-C++]
class Solution {
    const int mod = 1'000'000'007;

    long long pow(long long x, int p) {
        x %= mod;
        long long res = 1;
        while (p--) {
            res = res * x % mod;
            x = x * x % mod;
        }
        return res;
    }

public:
    int minNonZeroProduct(int p) {
        long long k = (1LL << p) - 1;
        return k % mod * pow(k - 1, p - 1) % mod;
    }
};
```

```go [sol-Go]
const mod = 1_000_000_007

func minNonZeroProduct(p int) int {
    k := 1<<p - 1
    return k % mod * pow(k-1, p-1) % mod
}

func pow(x, p int) int {
    res := 1
    for x %= mod; p > 0; p-- {
        res = res * x % mod
        x = x * x % mod
    }
    return res
}
```

```js [sol-JavaScript]
const MOD = 1_000_000_007n;

function pow(x, p) {
    let res = 1n;
    while (p--) {
        res = res * x % MOD;
        x = x * x % MOD;
    }
    return res;
}

var minNonZeroProduct = function(p) {
    const k = (1n << BigInt(p)) - 1n;
    return Number(k * pow(k - 1n, p - 1) % MOD);
};
```

```rust [sol-Rust]
impl Solution {
    pub fn min_non_zero_product(p: i32) -> i32 {
        const MOD: i64 = 1_000_000_007;

        fn pow(mut x: i64, p: i32) -> i64 {
            x %= MOD;
            let mut res = 1;
            for _ in 0..p {
                res = res * x % MOD;
                x = x * x % MOD;
            }
            res
        }

        let k = (1 << p) - 1;
        (k % MOD * pow(k - 1, p - 1) % MOD) as _
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(p)$。
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

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `四、数学贪心 / §4.4 均值不等式`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `四、数学贪心 / §4.4 均值不等式`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
