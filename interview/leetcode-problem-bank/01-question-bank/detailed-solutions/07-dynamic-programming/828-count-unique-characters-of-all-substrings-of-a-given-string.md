# 828. 统计子串中的唯一字符

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/count-unique-characters-of-all-substrings-of-a-given-string/
- 题目 slug：`count-unique-characters-of-all-substrings-of-a-given-string`
- 来源专题：动态规划
- 来源分类路径：七、其他线性 DP / §7.3 子数组 DP
- 难度分：2034
- 外部题解来源：https://leetcode.cn/problems/count-unique-characters-of-all-substrings-of-a-given-string/solutions/1804144/by-endlesscheng-ko4z/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[一次遍历：用变化量来思考（Python/Java/C++/Go）](https://leetcode.cn/problems/count-unique-characters-of-all-substrings-of-a-given-string/solutions/1804144/by-endlesscheng-ko4z/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`by-endlesscheng-ko4z`
- topic id：`1804144`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 16:46:02 +0800

#### 提示 1-1
 
将**所有子串**按照其末尾字符的下标分组。
 
#### 提示 1-2

考虑两组**相邻**的子串：以 $s[i-1]$ 结尾的子串、以 $s[i]$ 结尾的子串。

#### 提示 1-3

以 $s[i]$ 结尾的子串，可以看成是以 $s[i-1]$ 结尾的子串，在末尾添加上 $s[i]$ 组成。

**上面这一串提示是思考子串统计类问题的通用技巧之一。**

#### 提示 2-1

从左往右遍历 $s$，考虑将 $s[i]$ 添加到以 $s[i-1]$ 结尾的子串的末尾。添加后，这些以 $s[i-1]$ 结尾的子串的 `countUniqueChars` 值会如何变化？

#### 提示 2-2

![828.png](https://pic.leetcode.cn/1662366002-WaJwUl-828.png)

以 $s=\text{BCADEAFGA}$ 为例，当遍历到最后一个 $\text{A}$ 时：

- $\text{A}$ 可以单独作为一个子串，其 `countUniqueChars` 值为 $1$；
- 往子串 $\text{G}$ 和 $\text{FG}$ 的末尾添加 $\text{A}$，由于 $\text{A}$ 不在这些子串中，因此这些子串的 `countUniqueChars` 值都会**增加** $1$；
- 往子串 $\text{AFG}$、$\text{EAFG}$ 和 $\text{DEAFG}$ 的末尾添加 $\text{A}$，由于 $\text{A}$ 已经在这些子串中且恰好出现一次，添加后 $\text{A}$ 重复出现，因此这些子串的 `countUniqueChars` 值都会**减少** $1$；
- 往子串 $\text{ADEAFG}$、$\text{CADEAFG}$ 和 $\text{BCADEAFG}$ 的末尾添加 $\text{A}$，由于 $\text{A}$ 已经在这些子串中且不止出现一次，因此添加 $\text{A}$ 不会改变这些子串的 `countUniqueChars` 值。

据此，我们在从左往右遍历 $s$ 的同时，对每个字母 $s[i]$ 记录其**上一次**出现的下标 $\textit{last}_0[s[i]]$ 和**上上一次**出现的下标 $\textit{last}_1[s[i]]$。通过上面的例子，我们可以算出从「以 $s[i-1]$ 结尾的子串」到「以 $s[i]$ 结尾的子串」，`countUniqueChars` 值的和，**增加/减少**了多少：

- 增加了 $i-\textit{last}_0[s[i]]$（**注意 $s[i]$ 单独作为一个子串，贡献了 $1$**）；
- 减少了 $\textit{last}_0[s[i]]-\textit{last}_1[s[i]]$。

二者相加，总的变化量为

$$
i-2\cdot\textit{last}_0[s[i]]+\textit{last}_1[s[i]]
$$

特别地，如果 $\textit{last}_0[s[i]]$ 或 $\textit{last}_1[s[i]]$ 不存在，可以视作 $-1$，从而保证上式的正确性。

#### 提示 2-3

模拟上述过程，遍历 $s$ 的过程中用一个变量 $\textit{total}$ 维护「以 $s[i]$ 结尾的子串的 `countUniqueChars` 值的和」，累加遍历中的 $\textit{total}$，即为答案（根据提示 1-1）。

```py [sol1-Python3]
class Solution:
    def uniqueLetterString(self, s: str) -> int:
        ans = total = 0
        last0, last1 = {}, {}
        for i, c in enumerate(s):
            total += i - 2 * last0.get(c, -1) + last1.get(c, -1)
            ans += total
            last1[c] = last0.get(c, -1)
            last0[c] = i
        return ans
```

```java [sol1-Java]
class Solution {
    public int uniqueLetterString(String s) {
        int ans = 0, total = 0;
        int[] last0 = new int[26], last1 = new int[26];
        Arrays.fill(last0, -1);
        Arrays.fill(last1, -1);
        for (var i = 0; i < s.length(); i++) {
            var c = s.charAt(i) - 'A';
            total += i - 2 * last0[c] + last1[c];
            ans += total;
            last1[c] = last0[c];
            last0[c] = i;
        }
        return ans;
    }
}
```

```cpp [sol1-C++]
class Solution {
public:
    int uniqueLetterString(string s) {
        int ans = 0, total = 0, last0[26], last1[26];
        memset(last0, -1, sizeof(last0));
        memset(last1, -1, sizeof(last1));
        for (int i = 0; i < s.length(); ++i) {
            char c = s[i] - 'A';
            total += i - 2 * last0[c] + last1[c];
            ans += total;
            last1[c] = last0[c];
            last0[c] = i;
        }
        return ans;
    }
};
```

```go [sol1-Go]
func uniqueLetterString(s string) (ans int) {
    last0, last1, total := [26]int{}, [26]int{}, 0
    for i := range last0 {
        last0[i] = -1
        last1[i] = -1
    }
    for i, c := range s {
        c -= 'A'
        total += i - 2*last0[c] + last1[c]
        ans += total
        last1[c] = last0[c]
        last0[c] = i
    }
    return
}
```

#### 复杂度分析

- 时间复杂度：$O(n)$，其中 $n$ 为 $s$ 的长度。
- 空间复杂度：$O(|\Sigma|)$，其中 $|\Sigma|$ 为字符集合的大小，本题中字符均为大写字母，所以 $|\Sigma|=26$。

#### 相似题目

- [2262. 字符串的总引力](https://leetcode.cn/problems/total-appeal-of-a-string/) | [题解](https://leetcode.cn/problems/total-appeal-of-a-string/solution/by-endlesscheng-g405/)


#### 最后

欢迎关注我的B站频道：[灵茶山艾府](https://space.bilibili.com/206214)，定期更新算法讲解视频哦~

## 本地原创解析

### 1. 题意重述

本题来自 `七、其他线性 DP / §7.3 子数组 DP`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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
    int maxVowels(string s, int k) {
        auto isVowel = [](char c) {
            return c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u';
        };
        int cur = 0, ans = 0;
        for (int i = 0; i < (int)s.size(); ++i) {
            cur += isVowel(s[i]);
            if (i >= k) cur -= isVowel(s[i - k]);
            if (i + 1 >= k) ans = max(ans, cur);
        }
        return ans;
    }
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

回到来源分类 `七、其他线性 DP / §7.3 子数组 DP`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
