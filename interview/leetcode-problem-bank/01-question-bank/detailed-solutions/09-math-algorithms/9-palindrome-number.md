# 9. 回文数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/palindrome-number/
- 题目 slug：`palindrome-number`
- 来源专题：数学算法
- 来源分类路径：七、杂项 / §7.1 回文数
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/palindrome-number/solutions/3682487/bi-guan-fang-ti-jie-shao-xun-huan-yi-ci-02nkc/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[详细解释，比官方题解少循环一次（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/palindrome-number/solutions/3682487/bi-guan-fang-ti-jie-shao-xun-huan-yi-ci-02nkc/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`bi-guan-fang-ti-jie-shao-xun-huan-yi-ci-02nkc`
- topic id：`3682487`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:37:43 +0800

## 前置知识一：如何遍历一个整数

如果不把 $x$ 转成字符串，要怎么做？

我们可以不断地取 $x$ 的最低位（模 $10$），去掉 $x$ 的最低位（除以 $10$），直到 $x=0$。

例如 $x=123$：

1. 通过 $x\bmod 10$ 取到个位数 $3$，然后把 $x$ 除以 $10$（下取整），得到 $x=12$。
2. 再次 $x\bmod 10$ 取到十位数 $2$，然后把 $x$ 除以 $10$（下取整），得到 $x=1$。
3. 最后 $x\bmod 10$ 取到百位数 $1$，然后把 $x$ 除以 $10$（下取整），得到 $x=0$。此时完成遍历，退出循环。

## 前置知识二：如何反转一个整数

比如我们现在有一个数 $56$，如何把 $7$ 加到 $56$ 的末尾？

把 $56$ 乘以 $10$，再加上 $7$，就得到了 $567$。

一般地，如果要把数字 $b$ 加到整数 $a$ 的末尾，我们可以计算 $a\cdot 10 + b$。

如果要把 $x=123$ 反转，我们可以按照前置知识一中的方法，从低到高遍历 $x$ 的每一位，即 $3,2,1$。

初始化 $\textit{rev}=0$，依次把 $3,2,1$ 加到 $\textit{rev}$ 的末尾，即：

1. 更新 $\textit{rev}$ 为 $\textit{rev}\cdot 10 + 3 = 3$。
2. 更新 $\textit{rev}$ 为 $\textit{rev}\cdot 10 + 2 = 32$。
3. 更新 $\textit{rev}$ 为 $\textit{rev}\cdot 10 + 1 = 321$。
4. 最终得到了 $x$ 反转后的结果 $\textit{rev}=321$。

## 本题思路

暴力想法是，根据前置知识二，把 $x$ 整体反转，得到 $\textit{rev}$，然后判断 $\textit{rev}$ 和 $x$ 是否相等。

但实际上，对于回文数来说，只需判断 $x$ 的左半部分是否等于右半部分就行。

#### 例一

比如 $x=1221$，计算过程如下：

1. 初始化 $\textit{rev}=0$。
2. 计算 $x\bmod 10 = 1$，把 $1$ 加到 $\textit{rev}$ 的末尾，然后把 $x$ 除以 $10$（下取整），现在 $\textit{rev}=1$，$x=122$。
3. 计算 $x\bmod 10 = 2$，把 $2$ 加到 $\textit{rev}$ 的末尾，然后把 $x$ 除以 $10$（下取整），现在 $\textit{rev}=12$，$x=12$。
4. $\textit{rev}=x$，说明 $x$ 是（长度为偶数的）回文数。

#### 例二

比如 $x=121$，计算过程如下：

1. 初始化 $\textit{rev}=0$。
2. 计算 $x\bmod 10 = 1$，把 $1$ 加到 $\textit{rev}$ 的末尾，然后把 $x$ 除以 $10$（下取整），现在 $\textit{rev}=1$，$x=12$。
3. $\textit{rev} = \left\lfloor\dfrac{x}{10}\right\rfloor$，说明 $x$ 是（长度为奇数的）回文数。

#### 算法

1. 初始化 $\textit{rev}=0$。
2. 如果 $\textit{rev} < \left\lfloor\dfrac{x}{10}\right\rfloor$，循环执行第三步。
3. 每次循环，更新 $\textit{rev}$ 为 $\textit{rev}\cdot 10 + x\bmod 10$，然后把 $x$ 除以 $10$（下取整）。
4. 循环结束后，如果 $\textit{rev}=x$，说明 $x$ 是（长度为偶数的）回文数；如果 $\textit{rev} = \left\lfloor\dfrac{x}{10}\right\rfloor$，说明 $x$ 是（长度为奇数的）回文数。其余情况，$x$ 不是回文数。

**注**：当 $x$ 是奇数长度回文数时，这个做法比官方题解少循环一次。

#### 细节

特判 $x<0$ 的情况，此时 $x$ 一定不是回文数。

特判 $x>0$ 且 $x$ 个位数是 $0$ 的情况，由于 $x$ 的最高位一定不是 $0$，所以 $x$ 一定不是回文数。

```py [sol-Python3]
class Solution:
    def isPalindrome(self, x: int) -> bool:
        if x < 0 or x > 0 and x % 10 == 0:
            return False
        rev = 0
        while rev < x // 10:
            rev = rev * 10 + x % 10
            x //= 10
        return rev == x or rev == x // 10
```

```py [sol-Python3 字符串]
class Solution:
    def isPalindrome(self, x: int) -> bool:
        s = str(x)
        return s == s[::-1]  # s 是回文串
```

```java [sol-Java]
class Solution {
    public boolean isPalindrome(int x) {
        if (x < 0 || x > 0 && x % 10 == 0) {
            return false;
        }
        int rev = 0;
        while (rev < x / 10) {
            rev = rev * 10 + x % 10;
            x /= 10;
        }
        return rev == x || rev == x / 10;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    bool isPalindrome(int x) {
        if (x < 0 || x > 0 && x % 10 == 0) {
            return false;
        }
        int rev = 0;
        while (rev < x / 10) {
            rev = rev * 10 + x % 10;
            x /= 10;
        }
        return rev == x || rev == x / 10;
    }
};
```

```c [sol-C]
bool isPalindrome(int x) {
    if (x < 0 || x > 0 && x % 10 == 0) {
        return false;
    }
    int rev = 0;
    while (rev < x / 10) {
        rev = rev * 10 + x % 10;
        x /= 10;
    }
    return rev == x || rev == x / 10;
}
```

```go [sol-Go]
func isPalindrome(x int) bool {
    if x < 0 || x > 0 && x%10 == 0 {
        return false
    }
    rev := 0
    for rev < x/10 {
        rev = rev*10 + x%10
        x /= 10
    }
    return rev == x || rev == x/10
}
```

```js [sol-JavaScript]
var isPalindrome = function(x) {
    if (x < 0 || x > 0 && x % 10 === 0) {
        return false;
    }
    let rev = 0;
    while (rev < Math.floor(x / 10)) {
        rev = rev * 10 + x % 10;
        x = Math.floor(x / 10);
    }
    return rev === x || rev === Math.floor(x / 10);
};
```

```rust [sol-Rust]
impl Solution {
    pub fn is_palindrome(mut x: i32) -> bool {
        if x < 0 || x > 0 && x % 10 == 0 {
            return false;
        }
        let mut rev = 0;
        while rev < x / 10 {
            rev = rev * 10 + x % 10;
            x /= 10;
        }
        rev == x || rev == x / 10
    }
}
```

#### 复杂度分析

- 时间复杂度：如果 $x\le 0$ 则时间复杂度为 $\mathcal{O}(1)$，否则时间复杂度为 $\mathcal{O}(\log x)$。
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

本题来自 `七、杂项 / §7.1 回文数`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `七、杂项 / §7.1 回文数`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
