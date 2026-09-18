# 字符串

- 来源 URL：https://leetcode.cn/circle/discuss/SJFwQI/
- 来源标题：分享丨【算法题单】字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组）
- 抓取时间：2026-09-18 18:26:08 +0800
- 授权状态：authorized-by-user-confirmation
- 导入状态：preview

## 授权导入：专题/分类解析

> 本节按用户确认的授权导入前提保存来源专题中的分类说明、套路说明和学习建议。题目清单本身仍以 `01-question-bank/by-source-category/` 为准。

### 一、KMP（前缀的后缀）

[KMP 原理讲解](https://www.zhihu.com/question/21923021/answer/37475572)
定义 $s$ 的真前缀为不等于 $s$ 的前缀，$s$ 的真后缀为不等于 $s$ 的后缀。
定义 $s$ 的 $\text{border}$ 为既是 $s$ 的真前缀又是 $s$ 的真后缀的字符串。例如在 $s=\texttt{aabcaa}$ 中，$\texttt{a}$ 和 $\texttt{aa}$ 都是 $s$ 的 $\text{border}$。
对于模式串 $p$ 的每个前缀 $p[:i]$，计算这个前缀的最长 $\text{border}$ 长度，记在 $\pi$ 数组中。
利用 $\pi$ 数组，可以快速计算模式串 $p$ 出现在文本串 $t$ 的哪些位置上。
> **注**：$\pi$ 数组的定义来自《算法导论》。国内数据结构教材通常定义为 $\textit{next}$ 数组，有的教材下标是从 $1$ 开始的，也有从 $0$ 开始的（但在数组最前面插入了一个 $0$），请读者注意甄别。总的来说，思路是共通的，只是实现上略有区别。
```py [sol-Python3]
# 在文本串 text 中查找模式串 pattern，返回所有成功匹配的位置（pattern[0] 在 text 中的下标）
def kmp_search(text: str, pattern: str) -> List[int]:
    m = len(pattern)
    pi = [0] * m
    for i in range(1, m):

### 二、Z 函数（后缀的前缀）

> **注**：在国内算法竞赛圈，这个算法也叫**扩展 KMP**。
对于字符串 $s$，定义 $z[i]$ 表示后缀 $s[i:]$ 与 $s$ 的 LCP（最长公共前缀）的长度，其中 $s[i:]$ 表示从 $s[i]$ 到 $s[n-1]$ 的子串。
常用技巧是构造字符串 $\textit{pattern} + \textit{s}$ 或者 $\textit{pattern} + \texttt{#} + \textit{s}$，如果发现 $z[m+i]\ge m$（$m$ 是 $\textit{pattern}$ 的长度），则说明从 $s[i]$ 开始的子串与 $\textit{pattern}$ 匹配。
所以上面的一些 KMP 题目（子串匹配相关的），也可以用 Z 函数解决。读者可以尝试用 Z 函数解决 [28. 找出字符串中第一个匹配项的下标](https://leetcode.cn/problems/find-the-index-of-the-first-occurrence-in-a-string/)。
```py [sol-Python3]
# 计算并返回 z 数组，其中 z[i] = |LCP(s[i:], s)|
def calc_z(s: str) -> List[int]:
    n = len(s)
    z = [0] * n
    box_l = box_r = 0
    for i in range(1, n):
        if i <= box_r:

### 三、Manacher 算法（回文串）

Manacher 算法可以计算以 $s[i]$（或者 $s[i]$ 和 $s[i+1]$）为回文中心的最长回文子串的长度。
- 判断任意子串是否为回文串。
- 计算从 $s[i]$ 开始的最长回文子串的长度。
- 计算以 $s[i]$ 结尾的最长回文子串的长度。
> **注**：Z 函数和 Manacher 算法都会用到类似 Z-box 的概念，在学习时，可以对比体会。
Manacher 算法模板：
```py [sol-Python3]
class Manacher:
    def __init__(self, s: str):
        # 将 s 改造为 t，这样就不需要讨论 len(s) 的奇偶性，因为新串 t 的每个回文子串都是奇回文串（都有回文中心）
        # s 和 t 的下标转换关系：
        # (si+1)*2 = ti

### 四、字符串哈希

本题单的大多数题目都可以用字符串哈希解决。
推荐先把 [2156. 查找给定哈希值的子串](https://leetcode.cn/problems/find-substring-with-given-hash-value/) 和 [3756. 连接非零数字并乘以其数字和 II](https://leetcode.cn/problems/concatenate-non-zero-digits-and-multiply-by-sum-ii/) 做了，对理解**多项式哈希**的计算方法有帮助。
**模板代码**见 [我的题解](https://leetcode.cn/problems/construct-string-with-minimum-cost/solutions/2833949/hou-zhui-shu-zu-by-endlesscheng-32h9/)，包含单模哈希和双模哈希。
**小技巧**：我们可以用字符串哈希比较两个子串的字典序大小。做法是二分长度，计算最长公共前缀（LCP），然后比较 LCP 的下一个字母（一定不同，或者不存在），即可判断两个子串谁大谁小。时间复杂度：$\mathcal{O}(\log n)$。见 3722 题。

### 五、最小表示法

定义**循环左移**操作：把字符串 $s$ 的第一个字符 $s[0]$ 移除，添加到 $s$ 的末尾。例如 $\texttt{abcd}$ 操作一次后得到 $\texttt{bcda}$。
问题：你可以执行任意次循环左移操作，计算你能得到的字典序最小的字符串。
**注**：任意次循环左移操作后，得到的字符串叫做 $s$ 的**循环同构串**。
```py [sol-Python3]
# 返回 s 的字典序最小的循环同构串
# 时间复杂度 O(|s|)，证明见代码末尾的注释
def smallestRepresentation(s: str) -> str:
    n = len(s)
    i = 0  # 始终指向当前最小子串的首字母下标
    j = 1  # 指向需要和 i 比较的子串的首字母下标
    while j < n:
        # 暴力比较：是 i 开头的字典序小，还是 j 开头的字典序小？

### 六、字典树

- 见 [数据结构题单](https://leetcode.cn/circle/discuss/mOr1u6/) 第六章。

### 七、AC 自动机

AC 自动机 = 字典树 + KMP。
由于这些题目也可以用其他算法（字符串哈希等）解决，难度分仅供参考。

### 八、后缀数组/后缀自动机

由于这些题目也可以用其他算法（字符串哈希等）解决，难度分仅供参考。

### 九、子序列自动机

上面都是和**子串**相关的算法，本节是和**子序列**相关的算法：子序列自动机。
虽然名字有些高大上，但实际上只是预处理 $\ge i$ 的最近字母 $\textit{c}$ 的下标而已。
见 [讲解](https://leetcode.cn/problems/is-subsequence/solutions/2813031/jian-ji-xie-fa-pythonjavaccgojsrust-by-e-mz22/) 中的「进阶问题」。

### 关联题单

- [数据结构题单](https://leetcode.cn/circle/discuss/mOr1u6/) 中的「**六、字典树（trie）**」。
- [滑动窗口与双指针题单](https://leetcode.cn/circle/discuss/0viNMK/) 中的「**§4.2 判断子序列**」。
- [数学题单](https://leetcode.cn/circle/discuss/IYT3ss/) 中的「**§7.5 多项式**」。
- [贪心题单](https://leetcode.cn/circle/discuss/g6KTKL/) 中的「**三、字符串贪心**」。

### 算法题单

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)
欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)
如果你发现有题目可以补充进来，欢迎评论反馈。

## 授权题解链接索引

| 分类路径 | 标题 | 链接 | 导入状态 |
|---|---|---|---|
| 四、字符串哈希 | 我的题解 | https://leetcode.cn/problems/construct-string-with-minimum-cost/solutions/2833949/hou-zhui-shu-zu-by-endlesscheng-32h9/ | pending-fetch |
| 八、后缀数组/后缀自动机 | 评论 | https://leetcode.cn/problems/substring-xor-queries/solutions/2107060/yu-chu-li-suo-you-s-zhong-de-shu-zi-by-e-yxl2/comments/1922325/ | pending-fetch |
| 九、子序列自动机 | 讲解 | https://leetcode.cn/problems/is-subsequence/solutions/2813031/jian-ji-xie-fa-pythonjavaccgojsrust-by-e-mz22/ | pending-fetch |

## 本地原创增强解析

字符串 应优先沿用来源页的标题层级学习：先做低难度分题目，用同一套路反复观察边界条件；再回到进阶题处理多约束、多状态或跨专题组合。

### 分类层级说明

本专题的本地分类路径完全来自来源页面的 Markdown 标题层级，后续单题详解会回链到这些路径。

### 建议刷题节奏

先完成每个小节前 3–5 题，确认模板和边界；再按难度分上升补齐。遇到看完题解仍无法复述关键观察的题，先标记复盘，不阻塞下一小节。
