# 1616. 分割两个字符串得到回文串

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/split-two-strings-to-make-palindrome/
- 题目 slug：`split-two-strings-to-make-palindrome`
- 来源专题：滑动窗口与双指针
- 来源分类路径：三、单序列双指针 / §3.2 相向双指针
- 难度分：1868
- 外部题解来源：https://leetcode.cn/problems/split-two-strings-to-make-palindrome/solutions/2175393/mei-xiang-ming-bai-yi-zhang-tu-miao-dong-imvy/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[没想明白？一张图秒懂！（Python/Java/C++/Go）](https://leetcode.cn/problems/split-two-strings-to-make-palindrome/solutions/2175393/mei-xiang-ming-bai-yi-zhang-tu-miao-dong-imvy/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`mei-xiang-ming-bai-yi-zhang-tu-miao-dong-imvy`
- topic id：`2175393`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

![1616-cut.png](https://pic.leetcode.cn/1679061173-ZLMgpw-1616-cut.png)

注意，双指针前后缀匹配结束后，两个指针指向的位置恰好就是接下来要判断的回文串的左右边界，所以这两块代码逻辑可以无缝对接。

```py [sol1-Python3]
class Solution:
    def checkPalindromeFormation(self, a: str, b: str) -> bool:
        # 如果 a_prefix + b_suffix 可以构成回文串则返回 True，否则返回 False
        def check(a: str, b: str) -> bool:
            i, j = 0, len(a) - 1  # 相向双指针
            while i < j and a[i] == b[j]:  # 前后缀尽量匹配
                i += 1
                j -= 1
            s, t = a[i: j + 1], b[i: j + 1]  # 中间剩余部分
            return s == s[::-1] or t == t[::-1]  # 判断是否为回文串
        return check(a, b) or check(b, a)
```

```java [sol1-Java]
class Solution {
    public boolean checkPalindromeFormation(String a, String b) {
        return check(a, b) || check(b, a);
    }

    // 如果 a_prefix + b_suffix 可以构成回文串则返回 true，否则返回 false
    private boolean check(String a, String b) {
        int i = 0, j = a.length() - 1; // 相向双指针
        while (i < j && a.charAt(i) == b.charAt(j)) { // 前后缀尽量匹配
            ++i;
            --j;
        }
        return isPalindrome(a, i, j) || isPalindrome(b, i, j);
    }

    // 如果从 s[i] 到 s[j] 是回文串则返回 true，否则返回 false
    private boolean isPalindrome(String s, int i, int j) {
        while (i < j && s.charAt(i) == s.charAt(j)) {
            ++i;
            --j;
        }
        return i >= j;
    }
}
```

```cpp [sol1-C++]
class Solution {
    // 如果从 s[i] 到 s[j] 是回文串则返回 true，否则返回 false
    bool isPalindrome(string &s, int i, int j) {
        while (i < j && s[i] == s[j])
            ++i, --j;
        return i >= j;
    }

    // 如果 a_prefix + b_suffix 可以构成回文串则返回 true，否则返回 false
    bool check(string &a, string &b) {
        int i = 0, j = a.length() - 1; // 相向双指针
        while (i < j && a[i] == b[j]) // 前后缀尽量匹配
            ++i, --j;
        return isPalindrome(a, i, j) || isPalindrome(b, i, j);
    }

public:
    bool checkPalindromeFormation(string &a, string &b) {
        return check(a, b) || check(b, a);
    }
};
```

```go [sol1-Go]
// 如果从 s[i] 到 s[j] 是回文串则返回 true，否则返回 false
func isPalindrome(s string, i, j int) bool {
    for i < j && s[i] == s[j] {
        i++
        j--
    }
    return i >= j
}

// 如果 a_prefix + b_suffix 可以构成回文串则返回 true，否则返回 false
func check(a, b string) bool {
    i, j := 0, len(a)-1 // 相向双指针
    for i < j && a[i] == b[j] { // 前后缀尽量匹配
        i++
        j--
    }
    return isPalindrome(a, i, j) || isPalindrome(b, i, j)
}

func checkPalindromeFormation(a, b string) bool {
    return check(a, b) || check(b, a)
}
```

### 复杂度分析

- 时间复杂度：$O(n)$，其中 $n$ 为 $a$ 的长度。
- 空间复杂度：$O(1)$。仅用到若干额外变量。

> 注：Python 也可以像其它语言那样手动判断回文，从而避免切片产生的额外空间。

---

附：我的 [每日一题·高质量题解精选](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)，已分类整理好。

欢迎关注[【biIibiIi@灵茶山艾府】](https://space.bilibili.com/206214)，高质量算法教学，持续更新中~
