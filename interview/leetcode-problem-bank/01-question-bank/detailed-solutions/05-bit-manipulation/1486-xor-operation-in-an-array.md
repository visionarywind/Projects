# 1486. 数组异或操作

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/xor-operation-in-an-array/
- 题目 slug：`xor-operation-in-an-array`
- 来源专题：位运算
- 来源分类路径：二、异或（XOR）的性质
- 难度分：1181
- 外部题解来源：https://leetcode.cn/problems/xor-operation-in-an-array/solutions/2793723/o1-gong-shi-tui-dao-pythonjavaccgojsrust-le23/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：暴力 / O(1) 数学公式（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/xor-operation-in-an-array/solutions/2793723/o1-gong-shi-tui-dao-pythonjavaccgojsrust-le23/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`o1-gong-shi-tui-dao-pythonjavaccgojsrust-le23`
- topic id：`2793723`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 16:46:02 +0800

## 方法一：暴力

枚举 $i = 0,1,2,\ldots,n-1$，计算 $\textit{start}+2i$ 的异或和。 

```py [sol-Python3]
class Solution:
    def xorOperation(self, n: int, start: int) -> int:
        ans = 0
        for i in range(n):
            ans ^= start + 2 * i
        return ans
```

```java [sol-Java]
class Solution {
    public int xorOperation(int n, int start) {
        int ans = 0;
        for (int i = 0; i < n; i++) {
            ans ^= start + 2 * i;
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int xorOperation(int n, int start) {
        int ans = 0;
        for (int i = 0; i < n; i++) {
            ans ^= start + 2 * i;
        }
        return ans;
    }
};
```

```c [sol-C]
int xorOperation(int n, int start) {
    int ans = 0;
    for (int i = 0; i < n; i++) {
        ans ^= start + 2 * i;
    }
    return ans;
}
```

```go [sol-Go]
func xorOperation(n, start int) (ans int) {
	for i := range n {
		ans ^= start + 2*i
	}
	return
}
```

```js [sol-JavaScript]
var xorOperation = function(n, start) {
    let ans = 0;
    for (let i = 0; i < n; i++) {
        ans ^= start + 2 * i;
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn xor_operation(n: i32, start: i32) -> i32 {
        let mut ans = 0;
        for i in 0..n {
            ans ^= start + 2 * i;
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$。
- 空间复杂度：$\mathcal{O}(1)$。

⚠**如果你刚开始刷题，掌握方法一即可**。

## 方法二：数学公式

### start 是偶数的情况

如果 $\textit{start}$ 是偶数，那么 $\textit{nums}[i] = \textit{start}+2i$ 也是偶数。例如 $n=5,\ \textit{start}=0$，即

$$
0,2,4,6,8
$$

由于这些数二进制最低位都是 $0$，我们可以先把这些数都右移一位（除以 $2$），计算异或和，然后把异或和左移一位（乘以 $2$），就得到了答案，也就是

$$
0\oplus 2\oplus 4\oplus 6\oplus 8 = (0\oplus 1\oplus 2\oplus 3\oplus 4)\cdot 2 = 4\cdot 2 = 8
$$

### start 是奇数的情况

如果 $\textit{start}$ 是奇数，那么 $\textit{nums}[i] = \textit{start}+2i$ 也是奇数。例如 $n=5,\ \textit{start}=3$，即

$$
3,5,7,9,11
$$

我们分两部分计算这些数的异或和：

- **最低位**：由于这些数二进制最低位都是 $1$，所以答案的最低位取决于有多少个 $1$，偶数个 $1$ 的异或和是 $0$，奇数个 $1$ 的异或和是 $1$。
- **除了最低位的其余比特位**：先把这些数都右移一位（除以 $2$），计算异或和，然后把异或和左移一位（乘以 $2$）。

两部分相加，得

$$
3\oplus 5\oplus 7\oplus 9\oplus 11 = (1\oplus 2\oplus 3\oplus 4\oplus 5)\cdot 2 + 1 = 1\cdot 2 +1 = 3
$$

### 合二为一

无论 $\textit{start}$ 是偶数还是奇数，都可以用同一个规则计算。

设 $a = \left\lfloor\dfrac{\textit{start}}{2}\right\rfloor$，$b$ 为异或和的最低位。只有 $n$ 和 $\textit{start}$ 都为奇数时 $b=1$，其余情况 $b=0$。那么异或和等于

$$
(a\oplus(a+1)\oplus(a+2)\oplus\cdots\oplus(a+n-1))\cdot 2 + b
$$

如何计算 $a\oplus(a+1)\oplus(a+2)\oplus\cdots\oplus(a+n-1)$？

异或与加法是类似的，我们有

$$
\begin{aligned}
    & a + (a+1)+(a+2)+\cdots+(a+n-1)     \\
={} & (0 + 1+\cdots + (a+n-1)) - (0 + 1 + \cdots + (a-1))        \\
\end{aligned}
$$

也就是先把 $0$ 到 $a+n-1$ 的和求出来，再减去 $0$ 到 $a-1$ 的和，那么剩下的就是 $a$ 到 $a+n-1$ 的和了。

同样地，对于异或运算来说，有

$$
\begin{aligned}
&a\oplus(a+1)\oplus(a+2)\oplus\cdots\oplus(a+n-1)\\
=&\ (0\oplus 1\oplus \cdots \oplus (a+n-1)) \oplus (0\oplus 1\oplus \cdots \oplus (a-1)) \\ 
\end{aligned}
$$

也就是先把 $0$ 到 $a+n-1$ 的异或和求出来，再异或 $0$ 到 $a-1$ 的异或和，就得到了 $a$ 到 $a+n-1$ 的异或和。为什么？因为 $x\oplus x = 0$，上式中 $0$ 到 $a-1$ 的每个数都出现了两次，异或抵消掉了。

### 0 到 n 的异或和

当 $x$ 是偶数时，$x$ 和 $x+1$ 只有最低位不同，所以 $x\oplus (x+1)=1$，即

$$
\begin{align}
&0\oplus 1 = 1\\
&2\oplus 3 = 1\\
&4\oplus 5=1\\
&6\oplus 7=1\\
&\cdots
\end{align}
$$

又因为 $1\oplus 1 = 0$，所以从 $0$ 开始，每 $4$ 个数的异或和都是 $0$：

$$
\begin{align}
&0\oplus 1 \oplus  2\oplus 3 = 0\\
&4\oplus 5 \oplus  6\oplus 7 = 0\\
&8\oplus 9 \oplus  10\oplus 11 = 0\\
&\cdots
\end{align}
$$

这启发我们按照 $n$ 模 $4$ 的结果分类，去计算 $0$ 到 $n$ 的异或和：

- 当 $n=4k+3$（例如 $3,7,11$）时，两个数一组，可以得到 $\dfrac{n+1}{2} = \dfrac{4k+4}{2} = 2(k+1)$ 组，也就是偶数个 $1$，其异或和等于 $0$。
- 当 $n=4k+1$（例如 $1,5,9$）时，两个数一组，可以得到 $\dfrac{n+1}{2} = \dfrac{4k+2}{2} = 2k+1$ 组，也就是奇数个 $1$，其异或和等于 $1$。
- 当 $n=4k+2$（例如 $2,6,10$）时，把 $0$ 到 $n$ 的异或和拆分成 $0$ 到 $n-1=4k+1$ 的异或和（这等于 $1$），再异或 $n=4k+2$，即 $1\oplus (4k+2) = 4k+3 = n+1$。
- 当 $n=4k+4$（例如 $4,8,12$）时，把 $0$ 到 $n$ 的异或和拆分成 $0$ 到 $n-1=4k+3$ 的异或和（这等于 $0$），再异或 $n=4k+4$，即 $0\oplus (4k+4) = 4k+4 = n$。

综上所述，$0$ 到 $n$ 的异或和为：

$$
\bigoplus_{i=0}^n i = 
\begin{cases}
n,\ &n=4k\\
1,\ &n=4k+1\\
n+1,\ &n=4k+2\\
0,\ &n=4k+3
\end{cases}
$$

```py [sol-Python3]
class Solution:
    def xorOperation(self, n: int, start: int) -> int:
        xor_n = lambda n: (n, 1, n + 1, 0)[n % 4]
        a = start // 2
        b = n & start & 1  # 都为奇数才是 1
        return (xor_n(a + n - 1) ^ xor_n(a - 1)) * 2 + b
```

```java [sol-Java]
class Solution {
    public int xorOperation(int n, int start) {
        int a = start / 2;
        int b = n & start & 1; // 都为奇数才是 1
        return (xorN(a + n - 1) ^ xorN(a - 1)) * 2 + b;
    }

    private int xorN(int n) {
        return switch (n % 4) {
            case 0 -> n;
            case 1 -> 1;
            case 2 -> n + 1;
            default -> 0;
        };
    }
}
```

```cpp [sol-C++]
class Solution {
    int xor_n(int n) {
        switch (n % 4) {
            case 0: return n;
            case 1: return 1;
            case 2: return n + 1;
            default: return 0;
        }
    }

public:
    int xorOperation(int n, int start) {
        int a = start / 2;
        int b = n & start & 1; // 都为奇数才是 1
        return (xor_n(a + n - 1) ^ xor_n(a - 1)) * 2 + b;
    }
};
```

```c [sol-C]
int xor_n(int n) {
    switch (n % 4) {
        case 0: return n;
        case 1: return 1;
        case 2: return n + 1;
        default: return 0;
    }
}

int xorOperation(int n, int start) {
    int a = start / 2;
    int b = n & start & 1; // 都为奇数才是 1
    return (xor_n(a + n - 1) ^ xor_n(a - 1)) * 2 + b;
}
```

```go [sol-Go]
func xorN(n int) int {
    switch n % 4 {
        case 0: return n
        case 1: return 1
        case 2: return n + 1
        default: return 0
    }
}

func xorOperation(n, start int) int {
    a := start / 2
    b := n & start & 1 // 都为奇数才是 1
    return (xorN(a+n-1)^xorN(a-1))*2 + b
}
```

```js [sol-JavaScript]
function xorN(n) {
    switch (n % 4) {
        case 0: return n;
        case 1: return 1;
        case 2: return n + 1;
        default: return 0;
    }
}

var xorOperation = function(n, start) {
    const a = Math.floor(start / 2);
    const b = n & start & 1; // 都为奇数才是 1
    return (xorN(a + n - 1) ^ xorN(a - 1)) * 2 + b;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn xor_operation(n: i32, start: i32) -> i32 {
        let xor_n = |n| match n % 4 {
            0 => n,
            1 => 1,
            2 => n + 1,
            _ => 0,
        };
        let a = start / 2;
        let b = n & start & 1; // 都为奇数才是 1
        (xor_n(a + n - 1) ^ xor_n(a - 1)) * 2 + b
    }
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
6. [图论算法（DFS/BFS/拓扑排序/基环树/最短路/最小生成树/网络流）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/划分/状态机/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `二、异或（XOR）的性质`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、异或（XOR）的性质`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
