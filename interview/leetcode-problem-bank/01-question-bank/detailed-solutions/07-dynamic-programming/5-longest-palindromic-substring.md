# 5. 最长回文子串

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/longest-palindromic-substring/
- 题目 slug：`longest-palindromic-substring`
- 来源专题：动态规划
- 来源分类路径：八、区间 DP / §8.2 区间 DP
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/longest-palindromic-substring/solutions/2958179/mo-ban-on-manacher-suan-fa-pythonjavacgo-t6cx/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：中心扩展法 / Manacher 算法（Python/Java/C++/Go）](https://leetcode.cn/problems/longest-palindromic-substring/solutions/2958179/mo-ban-on-manacher-suan-fa-pythonjavacgo-t6cx/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`mo-ban-on-manacher-suan-fa-pythonjavacgo-t6cx`
- topic id：`2958179`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 方法一：中心扩展法

最暴力的做法是，枚举所有子串，然后判断子串是否为回文串。由于有 $\mathcal{O}(n^2)$ 个子串，每个子串判断是否回文需要 $\mathcal{O}(n)$，这个做法的时间复杂度是 $\mathcal{O}(n^3)$，太慢了。

能不能 $\mathcal{O}(1)$ 判断一个子串是不是回文的？

比如子串 $\texttt{abcba}$，最左边和最右边的字母都是 $\texttt{a}$，如果中间的 $\texttt{bcb}$ 是回文串，那么我们就能 $\mathcal{O}(1)$ 地知道 $\texttt{abcba}$ 是回文串。对于子串 $\texttt{bcb}$ 来说，最左边和最右边的字母都是 $\texttt{b}$，如果中间的 $\texttt{c}$ 是回文串，那么我们就能 $\mathcal{O}(1)$ 地知道 $\texttt{bcb}$ 是回文串。显然 $\texttt{c}$ 是回文串，所以 $\texttt{bcb}$ 是回文串，所以 $\texttt{abcba}$ 是回文串。

既然如此，为什么不直接从 $\texttt{c}$ 开始**向外扩展**呢？

- $\texttt{c}$ 是回文串。
- 看看 $\texttt{c}$ 左右两边的字母是不是一样的，一样，那么 $\texttt{bcb}$ 是回文串。
- 继续，看看 $\texttt{bcb}$ 左右两边的字母是不是一样的，一样，那么 $\texttt{abcba}$ 是回文串。我们 $\mathcal{O}(1)$ 地判断出了一个子串是不是回文串！

这些子串的长度都是奇数，我们称其为**奇回文串**。

回文串还可以是偶数长度，我们称其为**偶回文串**。

比如子串 $\texttt{abccba}$，我们可以从中间的 $\texttt{cc}$ 开始：

- $\texttt{cc}$ 是回文串。
- 看看 $\texttt{cc}$ 左右两边的字母是不是一样的，一样，那么 $\texttt{bccb}$ 是回文串。
- 继续，看看 $\texttt{bccb}$ 左右两边的字母是不是一样的，一样，那么 $\texttt{abccba}$ 是回文串。

一般地，枚举 $i=0,1,2,\ldots,n-1$ 作为奇回文串的中心，向左右两侧扩展：

- 初始化 $l=r=i$。
- 如果 $s[l]= s[r]$，那么向左右两侧扩展，把 $l$ 减一，把 $r$ 加一，继续判断更长的子串是不是回文串。直到下标出界或者 $s[l]\ne s[r]$。
- 循环结束时，最后一轮循环的子串 $s[l+1]$ 到 $s[r-1]$ 是回文串，若其长度 $r-l-1$ 大于答案的长度，那么更新答案的左右端点为 $l+1$ 和 $r-1$，方便输出具体子串。

同理，枚举 $i$ 和 $i+1$ 作为偶回文串的中心，也就是初始化 $l=i$，$r=i+1$，其余做法同上。

### 写法一：奇偶分开判断

```py [sol-Python3]
class Solution:
    def longestPalindrome(self, s: str) -> str:
        n = len(s)
        ans_left = ans_right = 0

        # 奇回文串
        for i in range(n):
            l = r = i
            while l >= 0 and r < n and s[l] == s[r]:
                l -= 1
                r += 1
            # 循环结束后，s[l+1] 到 s[r-1] 是回文串
            if r - l - 1 > ans_right - ans_left:
                ans_left, ans_right = l + 1, r  # 左闭右开区间

        # 偶回文串
        for i in range(n - 1):
            l, r = i, i + 1
            while l >= 0 and r < n and s[l] == s[r]:
                l -= 1
                r += 1
            if r - l - 1 > ans_right - ans_left:
                ans_left, ans_right = l + 1, r  # 左闭右开区间

        return s[ans_left: ans_right]
```

```java [sol-Java]
class Solution {
    public String longestPalindrome(String S) {
        char[] s = S.toCharArray();
        int n = s.length;
        int ansLeft = 0;
        int ansRight = 0;

        // 奇回文串
        for (int i = 0; i < n; i++) {
            int l = i;
            int r = i;
            while (l >= 0 && r < n && s[l] == s[r]) {
                l--;
                r++;
            }
            // 循环结束后，s[l+1] 到 s[r-1] 是回文串
            if (r - l - 1 > ansRight - ansLeft) {
                ansLeft = l + 1;
                ansRight = r; // 左闭右开区间
            }
        }

        // 偶回文串
        for (int i = 0; i < n - 1; i++) {
            int l = i;
            int r = i + 1;
            while (l >= 0 && r < n && s[l] == s[r]) {
                l--;
                r++;
            }
            if (r - l - 1 > ansRight - ansLeft) {
                ansLeft = l + 1;
                ansRight = r; // 左闭右开区间
            }
        }

        return S.substring(ansLeft, ansRight);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    string longestPalindrome(string s) {
        int n = s.size();
        int ans_left = 0, ans_right = 0;

        // 奇回文串
        for (int i = 0; i < n; i++) {
            int l = i, r = i;
            while (l >= 0 && r < n && s[l] == s[r]) {
                l--;
                r++;
            }
            // 循环结束后，s[l+1] 到 s[r-1] 是回文串
            if (r - l - 1 > ans_right - ans_left) {
                ans_left = l + 1;
                ans_right = r; // 左闭右开区间
            }
        }

        // 偶回文串
        for (int i = 0; i < n - 1; i++) {
            int l = i, r = i + 1;
            while (l >= 0 && r < n && s[l] == s[r]) {
                l--;
                r++;
            }
            if (r - l - 1 > ans_right - ans_left) {
                ans_left = l + 1;
                ans_right = r; // 左闭右开区间
            }
        }

        return s.substr(ans_left, ans_right - ans_left);
    }
};
```

```go [sol-Go]
func longestPalindrome(s string) string {
    n := len(s)
    ansLeft, ansRight := 0, 0

    // 奇回文串
    for i := range n {
        l, r := i, i
        for l >= 0 && r < n && s[l] == s[r] {
            l--
            r++
        }
        if r-l-1 > ansRight-ansLeft {
            ansLeft = l + 1
            ansRight = r // 左闭右开区间
        }
    }

    // 偶回文串
    for i := range n - 1 {
        l, r := i, i+1
        for l >= 0 && r < n && s[l] == s[r] {
            l--
            r++
        }
        if r-l-1 > ansRight-ansLeft {
            ansLeft = l + 1
            ansRight = r // 左闭右开区间
        }
    }

    return s[ansLeft:ansRight]
}
```

### 写法二：合二为一

枚举 $i=0,1,2,\ldots, 2n-2$。

- 规定当 $i$ 是偶数时，使用枚举奇回文串的规则，即初始化 $l=r=\dfrac{i}{2}$。比如 $i=2$ 时 $l=r=1$。
- 规定当 $i$ 是奇数时，使用枚举偶回文串的规则，即初始化 $l=\left\lfloor\dfrac{i}{2}\right\rfloor$，$r=\left\lceil\dfrac{i}{2}\right\rceil$。比如 $i=1$ 时 $l=0$，$r=1$。

两种情况可以合并为：

- 初始化 $l=\left\lfloor\dfrac{i}{2}\right\rfloor$，$r=\left\lceil\dfrac{i}{2}\right\rceil = \left\lfloor\dfrac{i+1}{2}\right\rfloor$。

按照这个规则，可以恰好枚举到所有的奇回文串和偶回文串。

```py [sol-Python3]
class Solution:
    def longestPalindrome(self, s: str) -> str:
        n = len(s)
        ans_left = ans_right = 0

        for i in range(2 * n - 1):
            l, r = i // 2, (i + 1) // 2
            while l >= 0 and r < n and s[l] == s[r]:
                l -= 1
                r += 1
            # 循环结束后，s[l+1] 到 s[r-1] 是回文串
            if r - l - 1 > ans_right - ans_left:
                ans_left, ans_right = l + 1, r  # 左闭右开区间

        return s[ans_left: ans_right]
```

```java [sol-Java]
class Solution {
    public String longestPalindrome(String S) {
        char[] s = S.toCharArray();
        int n = s.length;
        int ansLeft = 0;
        int ansRight = 0;

        for (int i = 0; i < 2 * n - 1; i++) {
            int l = i / 2;
            int r = (i + 1) / 2;
            while (l >= 0 && r < n && s[l] == s[r]) {
                l--;
                r++;
            }
            // 循环结束后，s[l+1] 到 s[r-1] 是回文串
            if (r - l - 1 > ansRight - ansLeft) {
                ansLeft = l + 1;
                ansRight = r; // 左闭右开区间
            }
        }

        return S.substring(ansLeft, ansRight);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    string longestPalindrome(string s) {
        int n = s.size();
        int ans_left = 0, ans_right = 0;

        for (int i = 0; i < 2 * n - 1; i++) {
            int l = i / 2, r = (i + 1) / 2;
            while (l >= 0 && r < n && s[l] == s[r]) {
                l--;
                r++;
            }
            // 循环结束后，s[l+1] 到 s[r-1] 是回文串
            if (r - l - 1 > ans_right - ans_left) {
                ans_left = l + 1;
                ans_right = r; // 左闭右开区间
            }
        }

        return s.substr(ans_left, ans_right - ans_left);
    }
};
```

```go [sol-Go]
func longestPalindrome(s string) string {
    n := len(s)
    ansLeft, ansRight := 0, 0

    for i := range 2*n - 1 {
        l, r := i/2, (i+1)/2
        for l >= 0 && r < n && s[l] == s[r] {
            l--
            r++
        }
        // 循环结束后，s[l+1] 到 s[r-1] 是回文串
        if r-l-1 > ansRight-ansLeft {
            ansLeft = l + 1
            ansRight = r // 左闭右开区间
        }
    }

    return s[ansLeft:ansRight]
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2)$，其中 $n$ 是 $s$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

## 方法二：Manacher 算法

具体请看 [视频讲解](https://www.bilibili.com/video/BV1UcyYY4EnQ/)，欢迎点赞关注~

本题需要输出具体的最长回文子串，这需要我们在跑 Manacher 算法的过程中，维护最大的 $\textit{halfLen}[i]$ 对应的下标 $\textit{maxI}$。最后根据下标转换关系，算出 $t$ 中的最长回文子串在 $s$ 中的下标。具体见视频讲解和代码注释。

```py [sol-Python3]
class Solution:
    def longestPalindrome(self, s: str) -> str:
        # Manacher 模板
        # 将 s 改造为 t，这样就不需要讨论 len(s) 的奇偶性，因为新串 t 的每个回文子串都是奇回文串（都有回文中心）
        # s 和 t 的下标转换关系：
        # (si+1)*2 = ti
        # ti/2-1 = si
        # ti 为偶数，对应奇回文串（从 2 开始）
        # ti 为奇数，对应偶回文串（从 3 开始）
        t = "#".join("^" + s + "$")

        # 定义一个奇回文串的回文半径=(长度+1)/2，即保留回文中心，去掉一侧后的剩余字符串的长度
        # half_len[i] 表示在 t 上的以 t[i] 为回文中心的最长回文子串的回文半径
        # 即 [i-half_len[i]+1, i+half_len[i]-1] 是 t 上的一个回文子串
        half_len = [0] * (len(t) - 2)
        half_len[1] = 1

        # box_r 表示当前右边界下标最大的回文子串的右边界下标+1
        # box_m 为该回文子串的中心位置
        # 二者的关系为 box_r = box_m + half_len[box_m]
        box_m = box_r = max_i = 0
        for i in range(2, len(half_len)):
            hl = 1
            if i < box_r:
                # 记 i 关于 box_m 的对称位置 i'=box_m*2-i
                # 若以 i' 为中心的最长回文子串范围超出了以 box_m 为中心的回文串的范围
                # 则 half_len[i] 应先初始化为已知的回文半径 box_r-i，然后再继续暴力匹配
                # 否则 half_len[i] 与 half_len[i'] 相等
                hl = min(box_r - i, half_len[box_m * 2 - i])

            # 暴力扩展
            # 算法的复杂度取决于这部分执行的次数
            # 由于扩展之后 box_r 必然会更新（右移），且扩展的的次数就是 box_r 右移的次数
            # 因此算法的复杂度 = O(len(t)) = O(n)
            while t[i - hl] == t[i + hl]:
                hl += 1
                box_m, box_r = i, i + hl

            half_len[i] = hl
            if hl > half_len[max_i]:
                max_i = i

        hl = half_len[max_i]
        # 注意 t 上的最长回文子串的最左边和最右边都是 '#'
        # 所以要对应到 s，最长回文子串的下标是从 max_i-hl+2 到 max_i+hl-2
        # 结合上文的下标转换关系，得到其在 s 上的下标范围是从 (max_i-hl)/2 到 (max_i+hl)/2-2
        return s[(max_i - hl) // 2: (max_i + hl) // 2 - 1]
```

```java [sol-Java]
class Solution {
    public String longestPalindrome(String s) {
        // Manacher 模板
        // 将 s 改造为 t，这样就不需要讨论 s.length() 的奇偶性，因为新串 t 的每个回文子串都是奇回文串（都有回文中心）
        // s 和 t 的下标转换关系：
        // (si+1)*2 = ti
        // ti/2-1 = si
        // ti 为偶数，对应奇回文串（从 2 开始）
        // ti 为奇数，对应偶回文串（从 3 开始）
        int n = s.length();
        char[] t = new char[n * 2 + 3];
        Arrays.fill(t, '#');
        t[0] = '^';
        for (int i = 0; i < n; i++) {
            t[i * 2 + 2] = s.charAt(i);
        }
        t[n * 2 + 2] = '$';

        // 定义一个奇回文串的回文半径=(长度+1)/2，即保留回文中心，去掉一侧后的剩余字符串的长度
        // halfLen[i] 表示在 t 上的以 t[i] 为回文中心的最长回文子串的回文半径
        // 即 [i-halfLen[i]+1, i+halfLen[i]-1] 是 t 上的一个回文子串
        int[] halfLen = new int[t.length - 2];
        halfLen[1] = 1;

        // maxI 记录最长回文子串在 halfLen 中的下标
        int maxI = 0;
        // boxR 表示当前右边界下标最大的回文子串的右边界下标+1
        // boxM 为该回文子串的中心位置
        // 二者的关系为 boxR = boxM + halfLen[boxM]
        int boxM = 0;
        int boxR = 0;
        for (int i = 2; i < halfLen.length; i++) {
            int hl = 1;
            if (i < boxR) {
                // 记 i 关于 boxM 的对称位置 i'=boxM*2-i
                // 若以 i' 为中心的最长回文子串范围超出了以 boxM 为中心的回文串的范围
                // 则 halfLen[i] 应先初始化为已知的回文半径 boxR-i，然后再继续暴力匹配
                // 否则 halfLen[i] 与 halfLen[i'] 相等
                hl = Math.min(boxR - i, halfLen[boxM * 2 - i]);
            }

            // 暴力扩展
            // 算法的复杂度取决于这部分执行的次数
            // 由于扩展之后 boxR 必然会更新（右移），且扩展的的次数就是 boxR 右移的次数
            // 因此算法的复杂度 = O(t.length) = O(n)
            while (t[i - hl] == t[i + hl]) {
                hl++;
                boxM = i;
                boxR = i + hl;
            }

            halfLen[i] = hl;
            if (hl > halfLen[maxI]) {
                maxI = i;
            }
        }

        int hl = halfLen[maxI];
        // 注意 t 上的最长回文子串的最左边和最右边都是 '#'
        // 所以要对应到 s，最长回文子串的下标是从 (maxI-hl)/2 到 (maxI+hl)/2-2
        return s.substring((maxI - hl) / 2, (maxI + hl) / 2 - 1);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    string longestPalindrome(string s) {
        // Manacher 模板
        // 将 s 改造为 t，这样就不需要讨论 s.size() 的奇偶性，因为新串 t 的每个回文子串都是奇回文串（都有回文中心）
        // s 和 t 的下标转换关系：
        // (si+1)*2 = ti
        // ti/2-1 = si
        // ti 为偶数，对应奇回文串（从 2 开始）
        // ti 为奇数，对应偶回文串（从 3 开始）
        string t = "^";
        for (char c : s) {
            t += '#';
            t += c;
        }
        t += "#$";

        // 定义一个奇回文串的回文半径=(长度+1)/2，即保留回文中心，去掉一侧后的剩余字符串的长度
        // half_len[i] 表示在 t 上的以 t[i] 为回文中心的最长回文子串的回文半径
        // 即 [i-half_len[i]+1, i+half_len[i]-1] 是 t 上的一个回文子串
        vector<int> half_len(t.size() - 2);
        half_len[1] = 1;

        // box_r 表示当前右边界下标最大的回文子串的右边界下标+1
        // box_m 为该回文子串的中心位置
        // 二者的关系为 box_r = box_m + half_len[box_m]
        int box_m = 0, box_r = 0, max_i = 0;
        for (int i = 2; i < half_len.size(); i++) {
            int hl = 1;
            if (i < box_r) {
                // 记 i 关于 box_m 的对称位置 i'=box_m*2-i
                // 若以 i' 为中心的最长回文子串范围超出了以 box_m 为中心的回文串的范围
                // 则 half_len[i] 应先初始化为已知的回文半径 box_r-i，然后再继续暴力匹配
                // 否则 half_len[i] 与 half_len[i'] 相等
                hl = min(box_r - i, half_len[box_m * 2 - i]);
            }

            // 暴力扩展
            // 算法的复杂度取决于这部分执行的次数
            // 由于扩展之后 box_r 必然会更新（右移），且扩展的的次数就是 box_r 右移的次数
            // 因此算法的复杂度 = O(t.size()) = O(n)
            while (t[i - hl] == t[i + hl]) {
                hl++;
                box_m = i;
                box_r = i + hl;
            }

            half_len[i] = hl;
            if (hl > half_len[max_i]) {
                max_i = i;
            }
        }

        int hl = half_len[max_i];
        // 注意 t 上的最长回文子串的最左边和最右边都是 '#'
        // 所以要对应到 s，最长回文子串的下标是从 max_i-hl+2 到 max_i+hl-2
        // 结合上文的下标转换关系，得到其在 s 上的下标范围是从 (max_i-hl)/2 到 (max_i+hl)/2-2
        return s.substr((max_i - hl) / 2, hl - 1);
    }
};
```

```go [sol-Go]
func longestPalindrome(s string) string {
    // Manacher 模板
    // 将 s 改造为 t，这样就不需要讨论 len(s) 的奇偶性，因为新串 t 的每个回文子串都是奇回文串（都有回文中心）
    // s 和 t 的下标转换关系：
    // (si+1)*2 = ti
    // ti/2-1 = si
    // ti 为偶数，对应奇回文串（从 2 开始）
    // ti 为奇数，对应偶回文串（从 3 开始）
    n := len(s)
    t := append(make([]byte, 0, n*2+3), '^')
    for _, c := range s {
        t = append(t, '#', byte(c))
    }
    t = append(t, '#', '$')

    // 定义一个奇回文串的回文半径=(长度+1)/2，即保留回文中心，去掉一侧后的剩余字符串的长度
    // halfLen[i] 表示在 t 上的以 t[i] 为回文中心的最长回文子串的回文半径
    // 即 [i-halfLen[i]+1, i+halfLen[i]-1] 是 t 上的一个回文子串
    halfLen := make([]int, len(t)-2)
    halfLen[1] = 1

    // boxR 表示当前右边界下标最大的回文子串的右边界下标+1
    // boxM 为该回文子串的中心位置
    // 二者的关系为 boxR = boxM + halfLen[boxM]
    boxM, boxR, maxI := 0, 0, 0
    for i := 2; i < len(halfLen); i++ {
        hl := 1
        if i < boxR {
            // 记 i 关于 boxM 的对称位置 i'=boxM*2-i
            // 若以 i' 为中心的最长回文子串范围超出了以 boxM 为中心的回文串的范围
            // 则 halfLen[i] 应先初始化为已知的回文半径 boxR-i，然后再继续暴力匹配
            // 否则 halfLen[i] 与 halfLen[i'] 相等
            hl = min(boxR-i, halfLen[boxM*2-i])
        }

        // 暴力扩展
        for t[i-hl] == t[i+hl] {
            hl++
            boxM, boxR = i, i+hl
        }

        halfLen[i] = hl
        if hl > halfLen[maxI] {
            maxI = i
        }
    }

    hl := halfLen[maxI]
    // 注意 t 上的最长回文子串的最左边和最右边都是 '#'
    // 所以要对应到 s，最长回文子串的下标是从 (maxI-hl)/2 到 (maxI+hl)/2-2
    return s[(maxI-hl)/2 : (maxI+hl)/2-1]
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $s$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 专题训练

见下面字符串题单的「**三、Manacher 算法**」。

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
11. [链表、树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `八、区间 DP / §8.2 区间 DP`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `八、区间 DP / §8.2 区间 DP`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
