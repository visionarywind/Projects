# 1003. 检查替换后的词是否有效

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/check-if-word-is-valid-after-substitutions/
- 题目 slug：`check-if-word-is-valid-after-substitutions`
- 来源专题：常用数据结构
- 来源分类路径：三、栈 / §3.3 邻项消除
- 难度分：1427
- 外部题解来源：https://leetcode.cn/problems/check-if-word-is-valid-after-substitutions/solutions/2253773/zhan-jian-ji-xie-fa-pythonjavacgo-by-end-i9o7/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[栈+简洁写法（Python/Java/C++/Go）](https://leetcode.cn/problems/check-if-word-is-valid-after-substitutions/solutions/2253773/zhan-jian-ji-xie-fa-pythonjavacgo-by-end-i9o7/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`zhan-jian-ji-xie-fa-pythonjavacgo-by-end-i9o7`
- topic id：`2253773`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

如果把问题改成「将字符串 `()` 插入到 $t$ 中的任意位置」，比如 `() -> (()) -> (()())`，是不是很眼熟？在 [20. 有效的括号](https://leetcode.cn/problems/valid-parentheses/) 中，我们用**栈**来判断括号是否合法：把左括号入栈，对于右括号，去判断栈顶是否为于其匹配的左括号。

对于本题来说，也可以用类似的方法完成：

- 字符 $\texttt{a}$：类似左括号，直接入栈。
- 字符 $\texttt{b}$：如果栈为空，或者栈顶不为 $\texttt{a}$，则返回 `false`，否则将栈顶修改为 $\texttt{b}$（或者出栈再入栈）。
- 字符 $\texttt{c}$：如果栈为空，或者栈顶不为 $\texttt{b}$，则返回 `false`，否则弹出栈顶，相当于找到了一个 $\texttt{abc}$。
- 代码实现时，$\texttt{b}$ 和 $\texttt{c}$ 的逻辑可以合并在一起，$\texttt{a}$ 和 $\texttt{b}$ 的入栈逻辑可以合并在一起。

循环结束后，如果栈为空，则返回 `true`，否则返回 `false`。

```py [sol1-Python3]
class Solution:
    def isValid(self, s: str) -> bool:
        st = []
        for c in map(ord, s):
            if c > ord('a') and (len(st) == 0 or c - st.pop() != 1):
                return False
            if c < ord('c'):
                st.append(c)
        return len(st) == 0
```

```java [sol1-Java]
class Solution {
    public boolean isValid(String S) {
        var s = S.toCharArray(); // 同时作为栈
        int i = 0; // i-1 表示栈顶下标，i=0 表示栈为空
        for (var c : s) {
            if (c > 'a' && (i == 0 || c - s[--i] != 1))
                return false;
            if (c < 'c')
                s[i++] = c; // 入栈
        }
        return i == 0;
    }
}
```

```cpp [sol1-C++]
class Solution {
public:
    bool isValid(string s) { // s 同时作为栈
        int i = 0; // i-1 表示栈顶下标，i=0 表示栈为空
        for (char c: s) {
            if (c > 'a' && (i == 0 || c - s[--i] != 1))
                return false;
            if (c < 'c')
                s[i++] = c; // 入栈
        }
        return i == 0;
    }
};
```

```go [sol1-Go]
func isValid(s string) bool {
    st := []rune{}
    for _, c := range s {
        if c > 'a' {
            if len(st) == 0 {
                return false
            }
            top := st[len(st)-1]
            st = st[:len(st)-1]
            if c-top != 1 {
                return false
            }
        }
        if c < 'c' {
            st = append(st, c)
        }
    }
    return len(st) == 0
}
```

### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $s$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$ 或 $\mathcal{O}(1)$。如果可以直接修改字符串（例如 C++），那么只需 $\mathcal{O}(1)$ 的额外空间。

---

[往期每日一题题解](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注[ biIibiIi@灵茶山艾府](https://space.bilibili.com/206214)，高质量算法教学，持续输出中~

## 本地原创解析

### 1. 题意重述

本题来自 `三、栈 / §3.3 邻项消除`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `三、栈 / §3.3 邻项消除`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
