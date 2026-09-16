# 1342. 将数字变成 0 的操作次数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/number-of-steps-to-reduce-a-number-to-zero/
- 题目 slug：`number-of-steps-to-reduce-a-number-to-zero`
- 来源专题：位运算
- 来源分类路径：一、基础题
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/number-of-steps-to-reduce-a-number-to-zero/solutions/3643305/liang-chong-fang-fa-mo-ni-o1-wei-yun-sua-l6uc/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：模拟 / O(1) 位运算（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/number-of-steps-to-reduce-a-number-to-zero/solutions/3643305/liang-chong-fang-fa-mo-ni-o1-wei-yun-sua-l6uc/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`liang-chong-fang-fa-mo-ni-o1-wei-yun-sua-l6uc`
- topic id：`3643305`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 方法一：模拟

题目说什么，我们就怎么做。

初始化操作次数为 $0$。写一个循环：

- 如果 $\textit{num}=0$，结束循环。
- 如果 $\textit{num}$ 是偶数，把 $\textit{num}$ 除以 $2$。
- 如果 $\textit{num}$ 是奇数，把 $\textit{num}$ 减少 $1$。
- 把操作次数加一。

最后返回操作次数。

```py [sol-Python3]
class Solution:
    def numberOfSteps(self, num: int) -> int:
        ans = 0
        while num:
            if num % 2:
                num -= 1
            else:
                num //= 2
            ans += 1
        return ans
```

```java [sol-Java]
class Solution {
    public int numberOfSteps(int num) {
        int ans = 0;
        while (num != 0) {
            if (num % 2 == 1) {
                num--;
            } else {
                num /= 2;
            }
            ans++;
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int numberOfSteps(int num) {
        int ans = 0;
        while (num) {
            if (num % 2) {
                num--;
            } else {
                num /= 2;
            }
            ans++;
        }
        return ans;
    }
};
```

```c [sol-C]
int numberOfSteps(int num) {
    int ans = 0;
    while (num) {
        if (num % 2) {
            num--;
        } else {
            num /= 2;
        }
        ans++;
    }
    return ans;
}
```

```go [sol-Go]
func numberOfSteps(num int) (ans int) {
    for num != 0 {
        if num%2 == 1 {
            num--
        } else {
            num /= 2
        }
        ans++
    }
    return
}
```

```js [sol-JavaScript]
var numberOfSteps = function(num) {
    let ans = 0;
    while (num) {
        if (num % 2) {
            num--;
        } else {
            num /= 2;
        }
        ans++;
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn number_of_steps(mut num: i32) -> i32 {
        let mut ans = 0;
        while num != 0 {
            if num % 2 == 1 {
                num -= 1;
            } else {
                num /= 2;
            }
            ans += 1;
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(\log \textit{num})$。理由见方法二。
- 空间复杂度：$\mathcal{O}(1)$。

## 方法二：位运算

把问题拆分成两个问题：

1. 我们会执行多少次「除以 $2$」？
2. 我们会执行多少次「减少 $1$」？

比如 $\textit{num}=13=1101_{(2)}$。

当 $\textit{num}$ 是奇数的时候，$\textit{num}$ 的二进制的最低位一定是 $1$，所以把 $\textit{num}$ 减少 $1$，仅仅把 $\textit{num}$ 的二进制的最低位从 $1$ 变成了 $0$，并不会改变 $\textit{num}$ 的二进制长度。并且减少 $1$ 后，$\textit{num}$ 变成偶数，下次操作一定要除以 $2$。除以 $2$ 后，$\textit{num}$ 的长度恰好会减少 $1$。

所以 $\textit{num}=1101_{(2)}$ 一定会恰好执行 $3$ 次「除以 $2$」。注意执行 $3$ 次「除以 $2$」后，$\textit{num}=1$，只需执行一次「减少 $1$」就变成 $0$ 了，无需继续操作。

一般地，我们会执行 $L-1$ 次「除以 $2$」，其中 $L$ 为 $\textit{num}$ 的二进制长度。

继续讨论：

- 如果除以 $2$ 后，$\textit{num}$ 的最低位是 $0$，那么无需减少 $1$。
- 如果除以 $2$ 后，$\textit{num}$ 的最低位是 $1$，那么一定要减少 $1$。

所以对于 $\textit{num}=1101_{(2)}$ 来说，会恰好执行 $3$ 次「减少 $1$」。

一般地，我们会执行 $K$ 次「减少 $1$」，其中 $K$ 为 $\textit{num}$ 的二进制中的 $1$ 的个数。

所以答案为

$$
L+K-1
$$

特殊情况：当 $\textit{num}=0$ 的时候，$L=K=0$，上式会算出 $-1$。我们可以特判这种情况，或者把结果与 $0$ 取最大值。

> 注意上式中的 $K\le L = \left\lfloor \log_2 \textit{num} \right\rfloor + 1$，所以方法一的复杂度为 $\mathcal{O}(\log \textit{num})$。

```py [sol-Python3]
class Solution:
    def numberOfSteps(self, num: int) -> int:
        return num.bit_length() + num.bit_count() - 1 if num else 0
```

```java [sol-Java]
class Solution {
    public int numberOfSteps(int num) {
        return num == 0 ? 0 : 31 - Integer.numberOfLeadingZeros(num) + Integer.bitCount(num);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int numberOfSteps(int num) {
        unsigned x = num;
        return x == 0 ? 0 : bit_width(x) + popcount(x) - 1;
    }
};
```

```go [sol-Go]
func numberOfSteps(num int) int {
    return max(bits.Len(uint(num))+bits.OnesCount(uint(num))-1, 0)
}
```

```js [sol-JavaScript]
var numberOfSteps = function(num) {
    return num === 0 ? 0 : 31 - Math.clz32(num) + bitCount32(num);
};

// 参考 Java 的 Integer.bitCount
function bitCount32(i) {
    i = i - ((i >>> 1) & 0x55555555);
    i = (i & 0x33333333) + ((i >>> 2) & 0x33333333);
    i = (i + (i >>> 4)) & 0x0f0f0f0f;
    i = i + (i >>> 8);
    i = i + (i >>> 16);
    return i & 0x3f;
}
```

```rust [sol-Rust]
impl Solution {
    pub fn number_of_steps(num: i32) -> i32 {
        (num.max(1).ilog2() + num.count_ones()) as _
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(1)$。这些库函数的时间复杂度都是 $\mathcal{O}(1)$，具体可以看源码。
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
