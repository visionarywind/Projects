# 1611. 使整数变为 0 的最少操作次数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-one-bit-operations-to-make-integers-zero/
- 题目 slug：`minimum-one-bit-operations-to-make-integers-zero`
- 来源专题：位运算
- 来源分类路径：九、其他
- 难度分：2345
- 外部题解来源：https://leetcode.cn/problems/minimum-one-bit-operations-to-make-integers-zero/solutions/3823952/tui-gong-shi-pythonjavaccgojsrust-by-end-y2yr/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[逆向思维+公式推导（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/minimum-one-bit-operations-to-make-integers-zero/solutions/3823952/tui-gong-shi-pythonjavaccgojsrust-by-end-y2yr/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tui-gong-shi-pythonjavaccgojsrust-by-end-y2yr`
- topic id：`3823952`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

> 九连环，玩具也，以铜制之。欲使九环同贯于柱上，则先上第一环，再上第二环，而下其第一环，更上第三环，而下其第一二环，再上第四环，如是更迭上下，凡八十一次，而九环毕上矣。解之之法，先下其第一环，次下其第三环，更上第一环，而并下其第一二环，又下其第三环，如是更迭上下，凡八十一次，而九环毕下矣。
> 
> ——《清稗类钞》

下文的 $n$ 均为二进制。

**题意解读**：第二种操作，翻转的是 $n$ 最低的 $1$ 左侧相邻的比特位。例如 $10010$ 操作后是 $10110$。注意操作是可逆的，$10110$ 执行第二种操作得到 $10010$。

逆向思维，从 $0$ 开始操作，我们依次得到的数字是什么？

由于连续两次相同操作后数字不变，如果要最小化操作次数，不能连续执行相同的操作，所以只能第一种操作和第二种操作**交替执行**。这意味着最优操作方案是**唯一**的，如下：

$$
0\to 1\to 11\to 10\to 110\to 111\to 101 \to 100 \to 1100\to \cdots
$$

从特殊到一般，考察从 $0$ 到 $2^k$ 的操作次数：

- 从 $0$ 到 $10$ 需要操作 $3$ 次。
- 从 $0$ 到 $100$ 需要操作 $7$ 次。
- 从 $0$ 到 $1000$ 需要操作多少次？

仔细考察从 $0$ 到 $100$ 的过程，其中后半段从 $110$ 到 $100$ 的过程是 $110\to 111\to 101 \to 100$，只看低 $2$ 位是 $10\to 11\to 01 \to 00$，倒过来看是 $00\to 01\to 11\to 10$，这和 $0$ 到 $10$ 的过程是完全一样的！

定义 $f(n)$ 表示把 $0$ 变成 $n$ 的最小操作次数，这也等于把 $n$ 变成 $0$ 最小操作次数。那么有

$$
0 \xrightarrow{操作\ f(10)\ 次} 10 \xrightarrow{操作\ 1\ 次} 110 \xrightarrow{操作\ f(10)\ 次} 100
$$

所以从 $0$ 到 $100$ 需要操作 $f(10) + 1 + f(10) = 3+1+3=7$ 次。

同理有

$$
0 \xrightarrow{操作\ f(100)\ 次} 100 \xrightarrow{操作\ 1\ 次} 1100 \xrightarrow{操作\ f(100)\ 次} 1000
$$

所以从 $0$ 到 $1000$ 需要操作 $f(100) + 1 + f(100) = 7+1+7=15$ 次。

一般地，我们有

$$
f(2^k) = 2f(2^{k-1}) + 1
$$

两边同时加一，得

$$
f(2^k) + 1 = 2(f(2^{k-1}) + 1)
$$

所以 $\{f(2^k) + 1\}$ 是个首项为 $f(1)+1 = 2$，公比为 $2$ 的等比数列，得

$$
f(2^k) = 2^{k+1} - 1
$$

> 注：另一种理解角度是，从 $0$ 到 $2^k$ 的过程中，恰好访问了 $[0,2^{k+1}-1]$ 中的每个整数各一次，所以需要操作 $2^{k+1}-1$ 次。

我们解决了 $n$ 是 $2$ 的幂的情况。下面考虑一般情况。

再来看这个过程

$$
0\to 1\to 11\to 10\to 110\to 111\to 101 \to 100
$$

其中从 $0$ 到 $111$ 需要操作多少次？

- 先计算从 $0$ 到 $100$ 的操作次数 $f(100)$。
- 然后**减去**从 $111$ 到 $100$ 的操作次数。这等于从 $11$ 到 $00$ 的操作次数，即 $f(11)$。

所以 $f(111) = f(100) - f(11)$。

一般地，设 $n$ 的二进制长度为 $k$，我们有

$$
\begin{aligned}
f(n) &= f(2^{k-1}) - f(n - 2^{k-1})        \\
     &= 2^k - 1 - f(n - 2^{k-1})         \\
\end{aligned}
$$

其中 $n - 2^{k-1}$ 表示 $n$ 去掉最高的 $1$ 后的值。

递归边界：$f(0) = 0$。

> 注：九连环需要 $f(2^9-1) = 341$ 次操作。开头那段文言由于把多次操作算作一次，给出的操作次数比实际的少。

## 写法一：递归

```py [sol-Python3]
class Solution:
    def minimumOneBitOperations(self, n: int) -> int:
        if n == 0:
            return 0
        k = n.bit_length()
        return (1 << k) - 1 - self.minimumOneBitOperations(n - (1 << (k - 1)))
```

```java [sol-Java]
class Solution {
    public int minimumOneBitOperations(int n) {
        if (n == 0) {
            return 0;
        }
        int k = 32 - Integer.numberOfLeadingZeros(n);
        return (1 << k) - 1 - minimumOneBitOperations(n - (1 << (k - 1)));
    }
}
```

```java [sol-Java 写法二]
class Solution {
    public int minimumOneBitOperations(int n) {
        if (n == 0) {
            return 0;
        }
        int hb = Integer.highestOneBit(n);
        return (hb << 1) - 1 - minimumOneBitOperations(n - hb);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minimumOneBitOperations(int n) {
        if (n == 0) {
            return 0;
        }
        int k = bit_width((uint32_t) n);
        return (1 << k) - 1 - minimumOneBitOperations(n - (1 << (k - 1)));
    }
};
```

```c [sol-C]
int minimumOneBitOperations(int n) {
    if (n == 0) {
        return 0;
    }
    int k = 32 - __builtin_clz(n);
    return (1 << k) - 1 - minimumOneBitOperations(n - (1 << (k - 1)));
}
```

```go [sol-Go]
func minimumOneBitOperations(n int) int {
	if n == 0 {
		return 0
	}
	k := bits.Len(uint(n))
	return 1<<k - 1 - minimumOneBitOperations(n-1<<(k-1))
}
```

```js [sol-JavaScript]
var minimumOneBitOperations = function(n) {
    if (n === 0) {
        return 0;
    }
    let k = 32 - Math.clz32(n);
    return (1 << k) - 1 - minimumOneBitOperations(n - (1 << (k - 1)));
};
```

```rust [sol-Rust]
impl Solution {
    pub fn minimum_one_bit_operations(n: i32) -> i32 {
        if n == 0 {
            return 0;
        }
        let k = 32 - n.leading_zeros();
        (1 << k) - 1 - Self::minimum_one_bit_operations(n - (1 << (k - 1)))
    }
}
```

## 写法二：迭代

递归是从高到低遍历 $n$ 的值为 $1$ 的比特位。

也可以从低到高遍历这些 $1$。

初始化答案 $\textit{ans}= 0$，即递归边界。

计算 $n$ 的最低位的 $1$，即 $\text{lowbit}$，原理见 [从集合论到位运算，常见位运算技巧分类总结！](https://leetcode.cn/circle/discuss/CaOJ45/)

这里 $\text{lowbit}$ 相当于上面的 $2^{k-1}$。

然后更新 $\textit{ans}$ 为 $\text{lowbit}\cdot 2 - 1 - \textit{ans}$，相当于去掉递归的「递」，只在「归」的过程中计算答案。

```py [sol-Python3]
class Solution:
    def minimumOneBitOperations(self, n: int) -> int:
        ans = 0
        while n > 0:
            lb = n & -n  # n 的最低 1
            ans = (lb << 1) - 1 - ans
            n ^= lb  # 去掉 n 的最低 1
        return ans
```

```java [sol-Java]
class Solution {
    public int minimumOneBitOperations(int n) {
        int ans = 0;
        while (n > 0) {
            int lb = n & -n; // n 的最低 1
            ans = (lb << 1) - 1 - ans;
            n ^= lb; // 去掉 n 的最低 1
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minimumOneBitOperations(int n) {
        int ans = 0;
        while (n > 0) {
            int lb = n & -n; // n 的最低 1
            ans = (lb << 1) - 1 - ans;
            n ^= lb; // 去掉 n 的最低 1
        }
        return ans;
    }
};
```

```c [sol-C]
int minimumOneBitOperations(int n) {
    int ans = 0;
    while (n > 0) {
        int lb = n & -n; // n 的最低 1
        ans = (lb << 1) - 1 - ans;
        n ^= lb; // 去掉 n 的最低 1
    }
    return ans;
}
```

```go [sol-Go]
func minimumOneBitOperations(n int) (ans int) {
	for n > 0 {
		lb := n & -n // n 的最低 1
		ans = lb<<1 - 1 - ans
		n ^= lb // 去掉 n 的最低 1
	}
	return
}
```

```js [sol-JavaScript]
var minimumOneBitOperations = function(n) {
    let ans = 0;
    while (n > 0) {
        const lb = n & -n; // n 的最低 1
        ans = (lb << 1) - 1 - ans;
        n ^= lb; // 去掉 n 的最低 1
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn minimum_one_bit_operations(mut n: i32) -> i32 {
        let mut ans = 0;
        while n > 0 {
            let lb = n & -n; // n 的最低 1
            ans = (lb << 1) - 1 - ans;
            n ^= lb; // 去掉 n 的最低 1
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(\log n)$。循环次数为 $n$ 的二进制中的 $1$ 的个数。特别地，如果 $n$ 是 $2$ 的幂，这个做法只需循环一次。
- 空间复杂度：$\mathcal{O}(1)$。

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

本题来自 `九、其他`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `九、其他`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
