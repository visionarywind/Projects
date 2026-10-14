# 125. 验证回文串

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/valid-palindrome/
- 题目 slug：`valid-palindrome`
- 来源专题：滑动窗口与双指针
- 来源分类路径：三、单序列双指针 / §3.2 相向双指针
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/valid-palindrome/solutions/3053284/jian-dan-ti-jian-dan-zuo-pythonjavaccgoj-1za0/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[简单题，简单做（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/valid-palindrome/solutions/3053284/jian-dan-ti-jian-dan-zuo-pythonjavaccgoj-1za0/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`jian-dan-ti-jian-dan-zuo-pythonjavaccgoj-1za0`
- topic id：`3053284`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 算法

初始化一左一右两个指针 $i=0$，$j=n-1$。其中 $n$ 是 $s$ 的长度。

分类讨论：

- 如果 $s[i]$ 既不是字母也不是数字，右移左指针，也就是把 $i$ 加一。
- 如果 $s[j]$ 既不是字母也不是数字，左移右指针，也就是把 $j$ 减一。
- 否则，如果 $s[i]$ 和 $s[j]$ 转成小写后相等，那么把 $i$ 加一，把 $j$ 减一，继续判断。
- 否则，$s$ 不是回文串，返回 $\texttt{false}$。

循环直到 $i\ge j$ 为止。最后返回 $\texttt{true}$。

```py [sol-Py3]
class Solution:
    def isPalindrome(self, s: str) -> bool:
        i, j = 0, len(s) - 1
        while i < j:
            if not s[i].isalnum():
                i += 1
            elif not s[j].isalnum():
                j -= 1
            elif s[i].lower() == s[j].lower():
                i += 1
                j -= 1
            else:
                return False
        return True
```

```py [sol-Py3 库函数]
class Solution:
    def isPalindrome(self, s: str) -> bool:
        s = ''.join(filter(str.isalnum, s)).lower()
        return s == s[::-1]
```

```java [sol-Java]
class Solution {
    public boolean isPalindrome(String s) {
        int i = 0;
        int j = s.length() - 1;
        while (i < j) {
            if (!Character.isLetterOrDigit(s.charAt(i))) {
                i++;
            } else if (!Character.isLetterOrDigit(s.charAt(j))) {
                j--;
            } else if (Character.toLowerCase(s.charAt(i)) == Character.toLowerCase(s.charAt(j))) {
                i++;
                j--;
            } else {
                return false;
            }
        }
        return true;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    bool isPalindrome(string s) {
        int i = 0, j = s.size() - 1;
        while (i < j) {
            if (!isalnum(s[i])) {
                i++;
            } else if (!isalnum(s[j])) {
                j--;
            } else if (tolower(s[i]) == tolower(s[j])) {
                i++;
                j--;
            } else {
                return false;
            }
        }
        return true;
    }
};
```

```c [sol-C]
bool isPalindrome(char* s) {
    int i = 0, j = strlen(s) - 1;
    while (i < j) {
        if (!isalnum(s[i])) {
            i++;
        } else if (!isalnum(s[j])) {
            j--;
        } else if (tolower(s[i]) == tolower(s[j])) {
            i++;
            j--;
        } else {
            return false;
        }
    }
    return true;
}
```

```go [sol-Go]
func isPalindrome(s string) bool {
    i, j := 0, len(s)-1
    for i < j {
        if !unicode.IsLetter(rune(s[i])) && !unicode.IsDigit(rune(s[i])) {
            i++
        } else if !unicode.IsLetter(rune(s[j])) && !unicode.IsDigit(rune(s[j])) {
            j--
        } else if unicode.ToLower(rune(s[i])) == unicode.ToLower(rune(s[j])) {
            i++
            j--
        } else {
            return false
        }
    }
    return true
}
```

```js [sol-JS]
var isPalindrome = function(s) {
    let i = 0, j = s.length - 1;
    while (i < j) {
        if (!/[a-zA-Z0-9]/.test(s[i])) {
            i++;
        } else if (!/[a-zA-Z0-9]/.test(s[j])) {
            j--;
        } else if (s[i].toLowerCase() === s[j].toLowerCase()) {
            i++;
            j--;
        } else {
            return false;
        }
    }
    return true;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn is_palindrome(s: String) -> bool {
        let s = s.as_bytes();
        let mut i = 0;
        let mut j = s.len() - 1;
        while i < j {
            if !s[i].is_ascii_alphanumeric() {
                i += 1;
            } else if !s[j].is_ascii_alphanumeric() {
                j -= 1;
            } else if s[i].to_ascii_lowercase() == s[j].to_ascii_lowercase() {
                i += 1;
                j -= 1;
            } else {
                return false;
            }
        }
        true
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $s$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

更多相似题目，见下面双指针题单中的「**§3.1 相向双指针**」。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. 【本题相关】[滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/circle/discuss/0viNMK/)
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
