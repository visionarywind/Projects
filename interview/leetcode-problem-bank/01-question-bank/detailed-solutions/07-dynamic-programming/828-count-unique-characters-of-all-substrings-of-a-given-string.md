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
