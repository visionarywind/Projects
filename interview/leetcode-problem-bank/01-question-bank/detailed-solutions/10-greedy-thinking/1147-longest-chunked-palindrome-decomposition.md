# 1147. 段式回文

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/longest-chunked-palindrome-decomposition/
- 题目 slug：`longest-chunked-palindrome-decomposition`
- 来源专题：贪心与思维
- 来源分类路径：三、字符串贪心 / §3.2 回文串贪心
- 难度分：1912
- 外部题解来源：https://leetcode.cn/problems/longest-chunked-palindrome-decomposition/solutions/2221544/tu-jie-tan-xin-zuo-fa-yi-tu-miao-dong-py-huik/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【图解】贪心做法，一图秒懂！（Python/Java/C++/Go）](https://leetcode.cn/problems/longest-chunked-palindrome-decomposition/solutions/2221544/tu-jie-tan-xin-zuo-fa-yi-tu-miao-dong-py-huik/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tu-jie-tan-xin-zuo-fa-yi-tu-miao-dong-py-huik`
- topic id：`2221544`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:01:04 +0800

![1147-c.png](https://pic.leetcode.cn/1681140541-LZeEKh-1147-c.png)

### 答疑

**问**：如果短的长度超过了长的长度的一半呢？

**答**：这是不可能的。用反证法证明。假设短的长度超过了长的长度的一半，上图证明中还可以得到 ①=②，结合前面的假设，说明 ① 和 ② 有重叠，且重叠部分既是短的前缀，也是短的后缀。比如 $\text{ababa}\cdots\text{ababa}$ 中 ①=②=$\text{aba}$，重叠部分为 $\text{a}$，既是 $\text{aba}$ 的前缀，也是 $\text{aba}$ 的后缀。这说明短的 $\text{aba}$ 还可以继续分割出更短的子串 $\text{a}$，而不是作为一个无法分割的整体，矛盾，所以不会出现短的长度超过了长的长度的一半的情况。

### 递归写法

```py [sol1-Python3]
class Solution:
    def longestDecomposition(self, s: str) -> int:
        if s == "":
            return 0
        for i in range(1, len(s) // 2 + 1):  # 枚举前后缀长度
            if s[:i] == s[-i:]:  # 立刻分割
                return 2 + self.longestDecomposition(s[i:-i])
        return 1  # 无法分割
```

```java [sol1-Java]
class Solution {
    public int longestDecomposition(String s) {
        if (s.isEmpty())
            return 0;
        for (int i = 1, n = s.length(); i <= n / 2; ++i) // 枚举前后缀长度
            if (s.substring(0, i).equals(s.substring(n - i))) // 立刻分割
                return 2 + longestDecomposition(s.substring(i, n - i));
        return 1; // 无法分割
    }
}
```

```cpp [sol1-C++]
class Solution {
public:
    int longestDecomposition(string s) {
        if (s.empty())
            return 0;
        for (int i = 1, n = s.length(); i <= n / 2; ++i) // 枚举前后缀长度
            if (s.substr(0, i) == s.substr(n - i)) // 立刻分割
                return 2 + longestDecomposition(s.substr(i, n - i * 2));
        return 1; // 无法分割
    }
};
```

```go [sol1-Go]
func longestDecomposition(s string) int {
    if s == "" {
        return 0
    }
    for i, n := 1, len(s); i <= n/2; i++ { // 枚举前后缀长度
        if s[:i] == s[n-i:] { // 立刻分割
            return 2 + longestDecomposition(s[i:n-i])
        }
    }
    return 1 // 无法分割
}
```

### 迭代写法

```py [sol1-Python3]
class Solution:
    def longestDecomposition(self, s: str) -> int:
        ans = 0
        while s:
            i = 1
            while i <= len(s) // 2 and s[:i] != s[-i:]:  # 枚举前后缀
                i += 1
            if i > len(s) // 2:  # 无法分割
                ans += 1
                break
            ans += 2  # 分割出 s[:i] 和 s[-i:]
            s = s[i:-i]
        return ans
```

```java [sol1-Java]
class Solution {
    public int longestDecomposition(String s) {
        int ans = 0;
        while (!s.isEmpty()) {
            int i = 1, n = s.length();
            while (i <= n / 2 && !s.substring(0, i).equals(s.substring(n - i))) // 枚举前后缀
                ++i;
            if (i > n / 2) { // 无法分割
                ++ans;
                break;
            }
            ans += 2; // 分割出 s[:i] 和 s[n-i:]
            s = s.substring(i, n - i);
        }
        return ans;
    }
}
```

```cpp [sol1-C++]
class Solution {
public:
    int longestDecomposition(string s) {
        int ans = 0;
        while (!s.empty()) {
            int i = 1, n = s.length();
            while (i <= n / 2 && s.substr(0, i) != s.substr(n - i)) // 枚举前后缀
                ++i;
            if (i > n / 2) { // 无法分割
                ++ans;
                break;
            }
            ans += 2; // 分割出 s[:i] 和 s[n-i:]
            s = s.substr(i, n - i * 2);
        }
        return ans;
    }
};
```

```go [sol1-Go]
func longestDecomposition(s string) (ans int) {
    for s != "" {
        i, n := 1, len(s)
        for i <= n/2 && s[:i] != s[n-i:] { // 枚举前后缀
            i++
        }
        if i > n/2 { // 无法分割
            ans++
            break
        }
        ans += 2 // 分割出 s[:i] 和 s[n-i:]
        s = s[i : n-i]
    }
    return
}
```

### 复杂度分析

- 时间复杂度：$O(n^2)$，其中 $n$ 为字符串的长度。最坏情况下无法分割，需要执行 $O(n)$ 次长为 $O(n)$ 的字符串比较，所以时间复杂度为 $O(n^2)$。
- 空间复杂度：$O(n)$ 或 $O(1)$。Go 语言切片不会有拷贝，所以空间复杂度为 $O(1)$。当然，也可以手动比较字符串，这样无需生成子串。

---

欢迎关注[【biIibiIi@灵茶山艾府】](https://space.bilibili.com/206214)，高质量算法教学，持续更新中~

附：[每日一题·高质量题解精选](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

## 本地原创解析

### 1. 题意重述

本题来自 `三、字符串贪心 / §3.2 回文串贪心`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `三、字符串贪心 / §3.2 回文串贪心`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
