# 1771. 由子序列构造的最长回文串的长度

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximize-palindrome-length-from-subsequences/
- 题目 slug：`maximize-palindrome-length-from-subsequences`
- 来源专题：动态规划
- 来源分类路径：八、区间 DP / §8.1 最长回文子序列
- 难度分：2182
- 外部题解来源：https://leetcode.cn/problems/maximize-palindrome-length-from-subsequences/solutions/2285215/shi-pin-qiao-miao-zhuan-huan-516-bian-xi-jhrt/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[巧妙转换！516 变形题！（Python/Java/C++/Go）](https://leetcode.cn/problems/maximize-palindrome-length-from-subsequences/solutions/2285215/shi-pin-qiao-miao-zhuan-huan-516-bian-xi-jhrt/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`shi-pin-qiao-miao-zhuan-huan-516-bian-xi-jhrt`
- topic id：`2285215`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 视频讲解

请看 [区间 DP【基础算法精讲 22】](https://www.bilibili.com/video/BV1Gs4y1E7EU/)，制作不易，欢迎点赞关注~

## 第一个错误的做法

记 $s = \textit{word}_1 +\textit{word}_2$。看上去求出 $s$ 的 [516. 最长回文子序列](https://leetcode.cn/problems/longest-palindromic-subsequence/) 就行。但不幸的是，题目要求从 $\textit{word}_1$ 和 $\textit{word}_2$ 中选出的子序列**不能为空**。例如 $\textit{word}_1 = \texttt{a},\textit{word}_2 = \texttt{abcb}$，那么对 $s=\texttt{aabcb}$ 求最长回文子序列，得到的是 $\texttt{bcb}$，但这题要求的是 $\texttt{aa}$。

## 第二个错误的做法

怎么办？假设根据 [516 的题解中的代码](https://leetcode.cn/problems/longest-palindromic-subsequence/solution/shi-pin-jiao-ni-yi-bu-bu-si-kao-dong-tai-kgkg/) 把所有的 $f[i][j]$ 都求出来了，看上去遍历满足 $i < n_1$（这里 $n_1$ 为 $\textit{word}_1$ 的长度）和 $j\ge n_1$ 的 $i$ 和 $j$，求出 $f[i][j]$ 的最大值就行，但是 $f[i][j]$ 对应的子序列可能仅在 $\textit{word}_1$ 或 $\textit{word}_2$ 中。

## 正确做法

当 $s[i]=s[j]$ 时，算出的 $f[i][j]$ 一定包含 $s[i]$ 和 $s[j]$，所以应当在此处更新答案。

```py [sol-Python3]
class Solution:
    def longestPalindrome(self, word1: str, word2: str) -> int:
        s = word1 + word2
        ans, n = 0, len(s)
        f = [[0] * n for _ in range(n)]
        for i in range(n - 1, -1, -1):
            f[i][i] = 1
            for j in range(i + 1, n):
                if s[i] == s[j]:
                    # 这里的 2 是 s[i] 和 s[j]，所以 f[i][j] 一定包含 s[i] 和 s[j]
                    f[i][j] = f[i + 1][j - 1] + 2
                    if i < len(word1) <= j:
                        ans = max(ans, f[i][j])
                else:
                    f[i][j] = max(f[i + 1][j], f[i][j - 1])
        return ans
```

```java [sol-Java]
class Solution {
    public int longestPalindrome(String word1, String word2) {
        char[] s = (word1 + word2).toCharArray();
        int n = s.length;
        int[][] f = new int[n][n];

        int ans = 0;
        for (int i = n - 1; i >= 0; i--) {
            f[i][i] = 1;
            for (int j = i + 1; j < n; j++) {
                if (s[i] == s[j]) {
                    // 这里的 2 是 s[i] 和 s[j]，所以 f[i][j] 一定包含 s[i] 和 s[j]
                    f[i][j] = f[i + 1][j - 1] + 2;
                    if (i < word1.length() && j >= word1.length()) {
                        ans = Math.max(ans, f[i][j]);
                    }
                } else {
                    f[i][j] = Math.max(f[i + 1][j], f[i][j - 1]);
                }
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int longestPalindrome(string word1, string word2) {
        string s = word1 + word2;
        int n = s.length(), ans = 0;
        vector<vector<int>> f(n, vector<int>(n));
        for (int i = n - 1; i >= 0; i--) {
            f[i][i] = 1;
            for (int j = i + 1; j < n; j++) {
                if (s[i] == s[j]) {
                    // 这里的 2 是 s[i] 和 s[j]，所以 f[i][j] 一定包含 s[i] 和 s[j]
                    f[i][j] = f[i + 1][j - 1] + 2;
                    if (i < word1.length() && j >= word1.length()) {
                        ans = max(ans, f[i][j]);
                    }
                } else {
                    f[i][j] = max(f[i + 1][j], f[i][j - 1]);
                }
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func longestPalindrome(word1, word2 string) (ans int) {
    s := word1 + word2
    n := len(s)
    f := make([][]int, n)
    for i := range f {
        f[i] = make([]int, n)
    }
    for i := n - 1; i >= 0; i-- {
        f[i][i] = 1
        for j := i + 1; j < n; j++ {
            if s[i] == s[j] {
                // 这里的 2 是 s[i] 和 s[j]，所以 f[i][j] 一定包含 s[i] 和 s[j]
                f[i][j] = f[i+1][j-1] + 2
                if i < len(word1) && j >= len(word1) {
                    ans = max(ans, f[i][j])
                }
            } else {
                f[i][j] = max(f[i+1][j], f[i][j-1])
            }
        }
    }
    return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}((n+m)^2)$，其中 $n$ 为 $\textit{word}_1$ 的长度，$m$ 为 $\textit{word}_2$ 的长度。
- 空间复杂度：$\mathcal{O}((n+m)^2)$。

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
