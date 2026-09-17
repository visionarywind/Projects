# 1328. 破坏回文串

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/break-a-palindrome/
- 题目 slug：`break-a-palindrome`
- 来源专题：贪心与思维
- 来源分类路径：三、字符串贪心 / §3.1 字典序最小/最大
- 难度分：1474
- 外部题解来源：https://leetcode.cn/problems/break-a-palindrome/solutions/3082264/tan-xin-pythonjavaccgojsrust-by-endlessc-plbg/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[贪心（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/break-a-palindrome/solutions/3082264/tan-xin-pythonjavaccgojsrust-by-endlessc-plbg/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tan-xin-pythonjavaccgojsrust-by-endlessc-plbg`
- topic id：`3082264`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:01:04 +0800

下文把 $\textit{palindrome}$ 简称为 $s$。

$s$ 是回文串，要改成不是回文串，可以改任意一个不在正中心的字母，破坏回文串的对称性，让 $s$ 不是回文串。

要使字典序最小，改前面比改后面更好。

比如 $s=\texttt{abcba}$：

- 把左边的 $\texttt{b}$ 改成 $\texttt{a}$ 得到 $\texttt{aacba}$。
- 把右边的 $\texttt{b}$ 改成 $\texttt{a}$ 得到 $\texttt{abcaa}$。

可以发现，前面的字母没有变小，无论后面怎么改，都不是最优的。

**特殊情况**：

1. 如果 $s$ 的长度是 $1$，那么无论怎么改都是回文串，返回空串。
2. 如果 $s$ 除了回文中心都是字母 $\texttt{a}$，那么只能把最后一个字母改成 $\texttt{b}$，这样 $s$ 不是回文串，且字典序最小。

```py [sol-Python3]
class Solution:
    def breakPalindrome(self, s: str) -> str:
        n = len(s)
        if n == 1:
            return ""
        # 把第一个不等于 a 的字母改成 a
        # 只需找前一半，如果前一半没有不等于 a 的字母，那么后一半肯定也没有
        for i in range(n // 2):
            if s[i] != 'a':
                return s[:i] + 'a' + s[i + 1:]
        # 除了回文中心，全是 a
        return s[:-1] + 'b'  # 最后一个字母改成 b
```

```java [sol-Java]
class Solution {
    public String breakPalindrome(String palindrome) {
        int n = palindrome.length();
        if (n == 1) {
            return "";
        }
        // 把第一个不等于 a 的字母改成 a
        // 只需找前一半，如果前一半没有不等于 a 的字母，那么后一半肯定也没有
        char[] s = palindrome.toCharArray();
        for (int i = 0; i < n / 2; i++) {
            if (s[i] != 'a') {
                s[i] = 'a';
                return new String(s);
            }
        }
        // 除了回文中心，全是 a
        s[n - 1] = 'b'; // 最后一个字母改成 b
        return new String(s);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    string breakPalindrome(string s) {
        int n = s.size();
        if (n == 1) {
            return "";
        }
        // 把第一个不等于 a 的字母改成 a
        // 只需找前一半，如果前一半没有不等于 a 的字母，那么后一半肯定也没有
        for (int i = 0; i < n / 2; i++) {
            if (s[i] != 'a') {
                s[i] = 'a';
                return s;
            }
        }
        // 除了回文中心，全是 a
        s.back() = 'b'; // 最后一个字母改成 b
        return s;
    }
};
```

```c [sol-C]
char* breakPalindrome(char* s) {
    int n = strlen(s);
    if (n == 1) {
        return "";
    }
    // 把第一个不等于 a 的字母改成 a
    // 只需找前一半，如果前一半没有不等于 a 的字母，那么后一半肯定也没有
    for (int i = 0; i < n / 2; i++) {
        if (s[i] != 'a') {
            s[i] = 'a';
            return s;
        }
    }
    // 除了回文中心，全是 a
    s[n - 1] = 'b'; // 最后一个字母改成 b
    return s;
}
```

```go [sol-Go]
func breakPalindrome(palindrome string) string {
    n := len(palindrome)
    if n == 1 {
        return ""
    }
    // 把第一个不等于 a 的字母改成 a
    // 只需找前一半，如果前一半没有不等于 a 的字母，那么后一半肯定也没有
    s := []byte(palindrome)
    for i := range n / 2 {
        if s[i] != 'a' {
            s[i] = 'a'
            return string(s)
        }
    }
    // 除了回文中心，全是 a
    s[n-1] = 'b' // 最后一个字母改成 b
    return string(s)
}
```

```js [sol-JavaScript]
var breakPalindrome = function(palindrome) {
    const n = palindrome.length;
    if (n === 1) {
        return "";
    }
    // 把第一个不等于 a 的字母改成 a
    // 只需找前一半，如果前一半没有不等于 a 的字母，那么后一半肯定也没有
    const s = palindrome.split("");
    for (let i = 0; i < Math.floor(n / 2); i++) {
        if (s[i] !== 'a') {
            s[i] = 'a';
            return s.join("");
        }
    }
    // 除了回文中心，全是 a
    s[n - 1] = 'b'; // 最后一个字母改成 b
    return s.join("");
};
```

```rust [sol-Rust]
impl Solution {
    pub fn break_palindrome(mut palindrome: String) -> String {
        let n = palindrome.len();
        if n == 1 {
            return "".to_string();
        }
        let s = unsafe { palindrome.as_bytes_mut() };
        // 把第一个不等于 a 的字母改成 a
        // 只需找前一半，如果前一半没有不等于 a 的字母，那么后一半肯定也没有
        for i in 0..n / 2 {
            if s[i] != b'a' {
                s[i] = b'a';
                return palindrome;
            }
        }
        // 除了回文中心，全是 a
        s[n - 1] = b'b'; // 最后一个字母改成 b
        palindrome
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $s$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$ 或 $\mathcal{O}(1)$。如果可以原地修改字符串，则空间复杂度为 $\mathcal{O}(1)$。

更多相似题目，见下面贪心题单中的「**§3.1 字典序最小/最大**」。

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

本题来自 `三、字符串贪心 / §3.1 字典序最小/最大`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `三、字符串贪心 / §3.1 字典序最小/最大`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
