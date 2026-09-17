# 2844. 生成特殊数字的最少操作

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-operations-to-make-a-special-number/
- 题目 slug：`minimum-operations-to-make-a-special-number`
- 来源专题：贪心与思维
- 来源分类路径：五、思维题 / §5.8 分类讨论
- 难度分：1588
- 外部题解来源：https://leetcode.cn/problems/minimum-operations-to-make-a-special-number/solutions/2424068/mei-ju-shan-chu-hou-yi-00255075-jie-wei-zhjlu/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：枚举末尾 / 一次遍历（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/minimum-operations-to-make-a-special-number/solutions/2424068/mei-ju-shan-chu-hou-yi-00255075-jie-wei-zhjlu/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`mei-ju-shan-chu-hou-yi-00255075-jie-wei-zhjlu`
- topic id：`2424068`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:13:14 +0800

## 前言

一个数能被 $25$ 整除，有如下五种情况：

- 这个数是 $0$。
- 这个数的末尾是 $00$，例如 $100$。
- 这个数的末尾是 $25$，例如 $225$。
- 这个数的末尾是 $50$，例如 $350$。
- 这个数的末尾是 $75$，例如 $475$。

## 方法一：枚举末尾

设 $\textit{num}$ 的长度为 $n$。

首先，根据题目说的，我们可以把 $\textit{num}$ 中的所有数字都删除，得到 $0$，这需要删除 $n$ 次。但如果 $\textit{num}$ 中有 $0$，那么删除 $n-1$ 也可以得到 $0$。

接下来，以示例 1 为例，看能否删除成末尾是 $50$ 的数：

1. **从右往左**遍历 $\textit{num}$，找最右边的 $0$。如果没有找到，或者最右边的 $0$ 的下标是 $0$，则说明无法做到。
2. **继续向左**遍历，找最右边的 $5$，设其下标为 $i$。如果没有找到，则说明无法做到。
3. 删除这个 $5$ 右边的所有非 $0$ 数字，这样就得到了一个以 $50$ 结尾的字符串。
4. 删除次数为 $n-i-2$。例如示例 1 中 $5$ 的下标是 $i=3$，需要删除 $7-3-2=2$ 次。

其余 $00,25,75$ 的计算方式同理，取 $n-i-2$ 的最小值作为答案。

请看 [视频讲解](https://www.bilibili.com/video/BV1Nj411178Z/) 第二题。

### 答疑

**问**：如果删除后只得到 $00$，产生了前导零，这就不合法了呀？

**答**：不可能在删除后只得到 $00$，因为题目保证 $\textit{num}$ 不含前导零，如果有多个 $0$，任何 $0$ 的左侧必然有非 $0$ 数字。

```py [sol-Python3]
class Solution:
    def minimumOperations(self, num: str) -> int:
        n = len(num)
        def f(tail: str) -> int:
            i = num.rfind(tail[1])
            if i <= 0:
                return n
            # 也可以写 num[:i].rfind(tail[0])，但切片需要额外空间
            i = num.rfind(tail[0], 0, i)
            return n if i < 0 else n - i - 2
        return min(n - ('0' in num), f("00"), f("25"), f("50"), f("75"))
```

```java [sol-Java]
class Solution {
    public int minimumOperations(String num) {
        int zero = num.length() - (num.contains("0") ? 1 : 0);
        return min(zero, f(num, "00"), f(num, "25"), f(num, "50"), f(num, "75"));
    }

    private int f(String num, String tail) {
        int n = num.length();
        int i = num.lastIndexOf(tail.charAt(1));
        if (i <= 0) {
            return n;
        }
        i = num.lastIndexOf(tail.charAt(0), i - 1);
        return i < 0 ? n : n - i - 2;
    }

    private int min(int x, int... y) {
        for (int v : y) {
            x = Math.min(x, v);
        }
        return x;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minimumOperations(string num) {
        int n = num.length();
        auto f = [&](string tail) {
            int i = num.rfind(tail[1]);
            if (i == string::npos || i == 0) {
                return n;
            }
            i = num.rfind(tail[0], i - 1);
            return i == string::npos ? n : n - i - 2;
        };
        int zero = n - (num.find('0') != string::npos);
        return min({zero, f("00"), f("25"), f("50"), f("75")});
    }
};
```

```c [sol-C]
#define MIN(a, b) ((b) < (a) ? (b) : (a))

int f(char* s, int n, char x, char y) {
    int i = n - 1;
    while (i && s[i] != y) {
        i--;
    }
    if (i == 0) {
        return n;
    }
    i--;
    while (i >= 0 && s[i] != x) {
        i--;
    }
    return i < 0 ? n : n - i - 2;
}

int minimumOperations(char* num) {
    int n = strlen(num);
    int zero = n - (strchr(num, '0') != NULL);
    int res00 = f(num, n, '0', '0');
    int res25 = f(num, n, '2', '5');
    int res50 = f(num, n, '5', '0');
    int res75 = f(num, n, '7', '5');
    return MIN(zero, MIN(MIN(res00, res25), MIN(res50, res75)));
}
```

```go [sol-Go]
func minimumOperations(num string) int {
    ans := len(num)
    if strings.Contains(num, "0") {
        ans-- // 可以删除 len(num)-1 次得到 "0"
    }
    f := func(tail string) {
        i := strings.LastIndexByte(num, tail[1])
        if i <= 0 {
            return
        }
        i = strings.LastIndexByte(num[:i], tail[0])
        if i < 0 {
            return
        }
        ans = min(ans, len(num)-i-2)
    }
    f("00")
    f("25")
    f("50")
    f("75")
    return ans
}
```

```js [sol-JavaScript]
var minimumOperations = function(num) {
    const n = num.length;
    function f(tail) {
        let i = num.lastIndexOf(tail[1]);
        if (i <= 0) {
            return n;
        }
        i = num.lastIndexOf(tail[0], i - 1);
        return i < 0 ? n : n - i - 2;
    }
    const zero = n - (num.includes('0') ? 1 : 0)
    return Math.min(zero, f("00"), f("25"), f("50"), f("75"));
};
```

```rust [sol-Rust]
impl Solution {
    pub fn minimum_operations(num: String) -> i32 {
        let n = num.len() as i32;
        let f = |x: char, y: char| -> i32 {
            if let Some(i) = num.rfind(y) {
                if let Some(i) = num[..i].rfind(x) {
                    return n - i as i32 - 2;
                }
            }
            n
        };
        (n - (num.contains('0') as i32)).min(f('0', '0')).min(f('2', '5')).min(f('5', '0')).min(f('7', '5'))
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{num}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。仅用到若干额外变量。

## 方法二：一次遍历

方法一需要多次遍历 $\textit{num}$，能否只遍历一次呢？

方法一的寻找过程，可以合并成下图：

![lc2844.png](https://pic.leetcode.cn/1721712236-KMehiq-lc2844.png)

根据该图，在**从右往左**遍历的过程中：

- 在之前找到 $0$ 的情况下，如果当前数字 $\textit{num}[i]$ 是 $0$ 或者 $5$，则立刻返回 $n-i-2$。
- 在之前找到 $5$ 的情况下，如果当前数字 $\textit{num}[i]$ 是 $2$ 或者 $7$，则立刻返回 $n-i-2$。
- 否则，如果 $\textit{num}[i]$ 是 $0$，标记我们找到了 $0$。
- 否则，如果 $\textit{num}[i]$ 是 $5$，标记我们找到了 $5$。
- 如果循环中没有返回，则最后返回 $n$ 或者 $n-1$，取决于我们是否找到了 $0$。

```py [sol-Python3]
class Solution:
    def minimumOperations(self, num: str) -> int:
        n = len(num)
        found0 = found5 = False
        for i in range(n - 1, -1, -1):
            c = num[i]
            if found0 and c in "05" or found5 and c in "27":
                return n - i - 2
            if c == '0':
                found0 = True
            elif c == '5':
                found5 = True
        return n - found0
```

```java [sol-Java]
class Solution {
    public int minimumOperations(String num) {
        int n = num.length();
        boolean found0 = false;
        boolean found5 = false;
        for (int i = n - 1; i >= 0; i--) {
            char c = num.charAt(i);
            if (found0 && (c == '0' || c == '5') ||
                found5 && (c == '2' || c == '7')) {
                return n - i - 2;
            }
            if (c == '0') {
                found0 = true;
            } else if (c == '5') {
                found5 = true;
            }
        }
        return found0 ? n - 1 : n;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minimumOperations(string num) {
        int n = num.length();
        bool found0 = false, found5 = false;
        for (int i = n - 1; i >= 0; i--) {
            char c = num[i];
            if (found0 && (c == '0' || c == '5') ||
                found5 && (c == '2' || c == '7')) {
                return n - i - 2;
            }
            if (c == '0') {
                found0 = true;
            } else if (c == '5') {
                found5 = true;
            }
        }
        return n - found0;
    }
};
```

```c [sol-C]
// 由于需要计算 strlen，C 语言需要遍历两次
int minimumOperations(char* num) {
    int n = strlen(num);
    bool found0 = false, found5 = false;
    for (int i = n - 1; i >= 0; i--) {
        char c = num[i];
        if (found0 && (c == '0' || c == '5') ||
            found5 && (c == '2' || c == '7')) {
            return n - i - 2;
        }
        if (c == '0') {
            found0 = 1;
        } else if (c == '5') {
            found5 = 1;
        }
    }
    return n - found0;
}
```

```go [sol-Go]
func minimumOperations(num string) int {
    n := len(num)
    var found0, found5 bool
    for i := n - 1; i >= 0; i-- {
        c := num[i]
        if found0 && (c == '0' || c == '5') ||
           found5 && (c == '2' || c == '7') {
            return n - i - 2
        }
        if c == '0' {
            found0 = true
        } else if c == '5' {
            found5 = true
        }
    }
    if found0 {
        return n - 1
    }
    return n
}
```

```js [sol-JavaScript]
var minimumOperations = function(num) {
    const n = num.length;
    let found0 = false, found5 = false;
    for (let i = n - 1; i >= 0; i--) {
        const c = num[i];
        if (found0 && (c === '0' || c === '5') ||
            found5 && (c === '2' || c === '7')) {
            return n - i - 2;
        }
        if (c === '0') {
            found0 = true;
        } else if (c === '5') {
            found5 = true;
        }
    }
    return found0 ? n - 1 : n;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn minimum_operations(num: String) -> i32 {
        let n = num.len() as i32;
        let mut found0 = false;
        let mut found5 = false;
        for (i, c) in num.bytes().enumerate().rev() {
            if found0 && (c == b'0' || c == b'5') ||
               found5 && (c == b'2' || c == b'7') {
                return n - i as i32 - 2;
            }
            if c == b'0' {
                found0 = true;
            } else if c == b'5' {
                found5 = true;
            }
        }
        if found0 { n - 1 } else { n }
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{num}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。仅用到若干额外变量。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口（定长/不定长/多指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心算法（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `五、思维题 / §5.8 分类讨论`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `五、思维题 / §5.8 分类讨论`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
