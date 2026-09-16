# 2116. 判断一个括号字符串是否有效

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/check-if-a-parentheses-string-can-be-valid/
- 题目 slug：`check-if-a-parentheses-string-can-be-valid`
- 来源专题：常用数据结构
- 来源分类路径：三、栈 / §3.4 合法括号字符串（RBS）
- 难度分：2038
- 外部题解来源：https://leetcode.cn/problems/check-if-a-parentheses-string-can-be-valid/solutions/1178043/zheng-fan-liang-ci-bian-li-by-endlessche-z8ac/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[一次遍历（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/check-if-a-parentheses-string-can-be-valid/solutions/1178043/zheng-fan-liang-ci-bian-li-by-endlessche-z8ac/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`zheng-fan-liang-ci-bian-li-by-endlessche-z8ac`
- topic id：`1178043`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

首先，介绍处理有效括号字符串的通用思路。

在有效括号字符串的任意一个**前缀**中，左括号的数量都大于等于右括号的数量。例如有效括号字符串 $\texttt{(())()}$，它的前缀有 $\texttt{(()}$，$\texttt{(())(}$ 等，都满足这一性质。为什么？因为对于有效括号字符串的**前缀**来说，每个右括号的左边，必然有与之匹配的左括号，但左括号不一定有与之匹配的右括号。

根据这一性质，从左到右遍历字符串 $s$，统计未匹配的左括号的个数 $c$：遇到左括号就把 $c$ 加一，遇到右括号就把 $c$ 减一（匹配一个左括号）。如果任何时刻 $c$ 都不为负数，且最终 $c=0$，那么 $s$ 就是有效括号字符串。

例如 $s=\texttt{(())()}$，遍历 $s$ 的过程中，$c$ 是这样变化的

$$
1\to 2\to 1\to 0\to 1\to 0 
$$

本题可以修改括号，把可以修改的括号视作 $\texttt{?}$，例如示例 1 的 $s=\texttt{?)?)??}$。

下面通过一些例子，带大家发现题目的**本质**。

## 例一

如果 $s=\texttt{??????}$，考察在遍历 $s$ 的过程中，未匹配的左括号的个数 $c$ 如何变化。

注意左括号可以让 $c$ 加一，右括号可以让 $c$ 减一（但不能是负数）。

| $i$  | $s_i$  | $c$ 的取值范围  | 备注  | 
|---|---|---|---|
| $0$  | $\texttt{?}$  |  $\{1\}$ | 只能是左括号  |
| $1$  | $\texttt{?}$  |  $\{0,2\}$ | 可以是左括号也可以是右括号  |
| $2$  | $\texttt{?}$  |  $\{1,3\}$ | 同上（注意 $c$ 不能是负数） |
| $3$  | $\texttt{?}$  |  $\{0,2,4\}$ |  同上 |
| $4$  | $\texttt{?}$  |  $\{1,3,5\}$ |  同上 |
| $5$  | $\texttt{?}$  |  $\{0,2,4,6\}$ | 同上  |

最终 $c$ 能变成 $0$，说明我们可以把 $s$ 变成有效括号字符串，比如 $\texttt{(())()}$。

## 例二

如果 $s=\texttt{??())?))}$，考察在遍历 $s$ 的过程中，未匹配的左括号的个数 $c$ 如何变化。

| $i$  | $s_i$  | $c$ 的取值范围  | 备注  | 
|---|---|---|---|
| $0$  | $\texttt{?}$  |  $\{1\}$ | 只能是左括号  |
| $1$  | $\texttt{?}$  |  $\{0,2\}$ | 可以是左括号也可以是右括号  |
| $2$  | $\texttt{(}$  |  $\{1,3\}$ | 所有 $c$ 加一 |
| $3$  | $\texttt{)}$  |  $\{0,2\}$ |  所有 $c$ 减一 |
| $4$  | $\texttt{)}$  |  $\{1\}$ |  所有 $c$ 减一，去掉负数 |
| $5$  | $\texttt{?}$  |  $\{0,2\}$ | 可以是左括号也可以是右括号  |
| $6$  | $\texttt{)}$  |  $\{1\}$ | 所有 $c$ 减一，去掉负数  |
| $7$  | $\texttt{)}$  |  $\{0\}$ | 所有 $c$ 减一  |

最终 $c$ 能变成 $0$，说明我们可以把 $s$ 变成有效括号字符串。本例只有一种方案，即 $\texttt{((())())}$。

## 例三

如果 $s=\texttt{?))?}$，考察在遍历 $s$ 的过程中，未匹配的左括号的个数 $c$ 如何变化。

| $i$  | $s_i$  | $c$ 的取值范围  | 备注  | 
|---|---|---|---|
| $0$  | $\texttt{?}$  |  $\{1\}$ | 只能是左括号  |
| $1$  | $\texttt{)}$  |  $\{0\}$ | 所有 $c$ 减一  |
| $2$  | $\texttt{)}$  |  $\varnothing$ | 所有 $c$ 减一，去掉负数 |

遍历到 $s_2$ 的时候，$c$ 的取值范围为空，说明无法把 $\texttt{?))?}$ 变成有效括号字符串。

## 例四

如果 $s=\texttt{?((?}$，考察在遍历 $s$ 的过程中，未匹配的左括号的个数 $c$ 如何变化。

| $i$  | $s_i$  | $c$ 的取值范围  | 备注  | 
|---|---|---|---|
| $0$  | $\texttt{?}$  |  $\{1\}$ | 只能是左括号  |
| $1$  | $\texttt{(}$  |  $\{2\}$ | 所有 $c$ 加一 |
| $2$  | $\texttt{(}$  |  $\{3\}$ | 所有 $c$ 加一 |
| $3$  | $\texttt{?}$  |  $\{2,4\}$ | 可以是左括号也可以是右括号 |

由于最终 $c$ 的取值范围不包含 $0$，我们无法把 $\texttt{?((?}$ 变成有效括号字符串。

## 总结

在遍历 $s$ 的过程中，维护 $c$ 的**取值范围**。如果最终 $c$ 的取值范围不为空，且包含 $0$，那么可以把 $s$ 变成有效括号字符串。

我们可以用代码**模拟**上述流程。但是，真的需要维护一个集合吗？

注意到，集合中的数都是连续的奇数或者连续的偶数（可以用数学归纳法证明），不存在 $\{1,5,7\}$ 这种情况。所以只需要维护集合中的最小值和最大值。

## 算法

遍历 $s$ 的过程中，维护 $c$ 的取值范围中的最小值 $\textit{mn}$ 和最大值 $\textit{mx}$：

- 如果 $s_i=\texttt{(}$，那么把 $\textit{mn}$ 和 $\textit{mx}$ 都加一。
- 如果 $s_i=\texttt{)}$，那么把 $\textit{mn}$ 和 $\textit{mx}$ 都减一。如果 $\textit{mx}<0$，去掉负数后集合为空，说明无法把 $s$ 变成有效括号字符串。如果 $\textit{mn}<0$，那么把 $\textit{mn}$ 改成 $1$。比如 $\{0,2,4\}$ 都减一变成 $\{-1,1,3\}$，去掉负数变成 $\{1,3\}$，其中最小的数是 $1$。
- 如果 $s_i=\texttt{?}$，这个问号可以是左括号，把 $\textit{mx}$ 加一；也可以是右括号，把 $\textit{mn}$ 减一。如果 $\textit{mn}=-1$，此时 $c$ 的取值范围都是奇数，把 $\textit{mn}$ 改成 $1$。

如果最终 $\textit{mn}=0$，说明最终 $c$ 能变成 $0$，我们可以把 $s$ 变成有效括号字符串。

**小优化**：如果 $s$ 的长度是奇数，那么一定有个单独的括号无法匹配，直接返回 $\texttt{false}$。

## 答疑

**问**：为什么要在 $\textit{mn}<0$ 的时候把 $\textit{mn}$ 置为 $1$，不写这个行不行？

**答**：不行。因为可能会有这样的情况：不写，负数后面不断增加，变成 $0$ 了，最终返回 $\texttt{true}$；写的话就无法变成 $0$，最终返回 $\texttt{false}$。

```py [sol-Python3]
class Solution:
    def canBeValid(self, s: str, locked: str) -> bool:
        if len(s) % 2:
            return False
        mn = mx = 0
        for b, lock in zip(s, locked):
            if lock == '1':  # 不能改
                d = 1 if b == '(' else -1
                mx += d
                if mx < 0:  # c 不能为负
                    return False
                mn += d
            else:  # 可以改
                mx += 1  # 改成左括号，c 加一
                mn -= 1  # 改成右括号，c 减一
            if mn < 0:  # c 不能为负
                mn = 1  # 此时 c 的取值范围都是奇数，最小的正奇数是 1
        return mn == 0  # 说明最终 c 能是 0
```

```java [sol-Java]
class Solution {
    public boolean canBeValid(String s, String locked) {
        if (s.length() % 2 > 0) {
            return false;
        }
        int mn = 0;
        int mx = 0;
        for (int i = 0; i < s.length(); i++) {
            if (locked.charAt(i) == '1') { // 不能改
                int d = s.charAt(i) == '(' ? 1 : -1;
                mx += d;
                if (mx < 0) { // c 不能为负
                    return false;
                }
                mn += d;
            } else { // 可以改
                mx++; // 改成左括号，c 加一
                mn--; // 改成右括号，c 减一
            }
            if (mn < 0) { // c 不能为负
                mn = 1; // 此时 c 的取值范围都是奇数，最小的正奇数是 1
            }
        }
        return mn == 0; // 说明最终 c 能是 0
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    bool canBeValid(string s, string locked) {
        if (s.size() % 2) {
            return false;
        }
        int mn = 0, mx = 0;
        for (int i = 0; i < s.size(); i++) {
            if (locked[i] == '1') { // 不能改
                int d = s[i] == '(' ? 1 : -1;
                mx += d;
                if (mx < 0) { // c 不能为负
                    return false;
                }
                mn += d;
            } else { // 可以改
                mx++; // 改成左括号，c 加一
                mn--; // 改成右括号，c 减一
            }
            if (mn < 0) { // c 不能为负
                mn = 1; // 此时 c 的取值范围都是奇数，最小的正奇数是 1
            }
        }
        return mn == 0; // 说明最终 c 能是 0
    }
};
```

```c [sol-C]
bool canBeValid(char* s, char* locked) {
    int mn = 0, mx = 0;
    for (int i = 0; locked[i]; i++) {
        if (locked[i] == '1') { // 不能改
            int d = s[i] == '(' ? 1 : -1;
            mx += d;
            if (mx < 0) { // c 不能为负
                return false;
            }
            mn += d;
        } else { // 可以改
            mx++; // 改成左括号，c 加一
            mn--; // 改成右括号，c 减一
        }
        if (mn < 0) { // c 不能为负
            mn = 1; // 此时 c 的取值范围都是奇数，最小的正奇数是 1
        }
    }
    return mn == 0; // 说明最终 c 能是 0
}
```

```go [sol-Go]
func canBeValid(s, locked string) bool {
    if len(s)%2 > 0 {
        return false
    }
    mn, mx := 0, 0
    for i, lock := range locked {
        if lock == '1' { // 不能改
            d := 1 - int(s[i]%2*2) // 左括号是 1，右括号是 -1
            mx += d
            if mx < 0 { // c 不能为负
                return false
            }
            mn += d
        } else { // 可以改
            mx++ // 改成左括号，c 加一
            mn-- // 改成右括号，c 减一
        }
        if mn < 0 { // c 不能为负
            mn = 1 // 此时 c 的取值范围都是奇数，最小的正奇数是 1
        }
    }
    return mn == 0 // 说明最终 c 能是 0
}
```

```js [sol-JavaScript]
var canBeValid = function(s, locked) {
    if (s.length % 2 > 0) {
        return false;
    }
    let mn = 0, mx = 0;
    for (let i = 0; i < s.length; i++) {
        if (locked[i] === '1') { // 不能改
            const d = s[i] === '(' ? 1 : -1;
            mx += d;
            if (mx < 0) { // c 不能为负
                return false;
            }
            mn += d;
        } else { // 可以改
            mx++; // 改成左括号，c 加一
            mn--; // 改成右括号，c 减一
        }
        if (mn < 0) { // c 不能为负
            mn = 1; // 此时 c 的取值范围都是奇数，最小的正奇数是 1
        }
    }
    return mn === 0; // 说明最终 c 能是 0
};
```

```rust [sol-Rust]
impl Solution {
    pub fn can_be_valid(s: String, locked: String) -> bool {
        if s.len() % 2 > 0 {
            return false;
        }
        let mut mn = 0;
        let mut mx = 0;
        for (b, lock) in s.bytes().zip(locked.bytes()) {
            if lock == b'1' { // 不能改
                let d = if b == b'(' { 1 } else { -1 };
                mx += d;
                if mx < 0 { // c 不能为负
                    return false;
                }
                mn += d;
            } else { // 可以改
                mx += 1; // 改成左括号，c 加一
                mn -= 1; // 改成右括号，c 减一
            }
            if mn < 0 { // c 不能为负
                mn = 1; // 此时 c 的取值范围都是奇数，最小的正奇数是 1
            }
        }
        mn == 0 // 说明最终 c 能是 0
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $s$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

## 进阶问题

修改方案是否唯一？有多少种修改方案？

见 [CF1709C. Recover an RBS](https://codeforces.com/problemset/problem/1709/C)。

## 专题训练

见下面数据结构题单中的「**§3.4 合法括号字符串（RBS）**」。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. 【本题相关】[常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `三、栈 / §3.4 合法括号字符串（RBS）`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `三、栈 / §3.4 合法括号字符串（RBS）`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
