# 动态规划

- 来源 URL：https://leetcode.cn/circle/discuss/tXLS3i/
- 来源标题：分享丨【算法题单】动态规划（入门/背包/划分/状态机/区间/状压/数位/树形/优化）
- 抓取时间：2026-09-18 18:26:08 +0800
- 授权状态：authorized-by-user-confirmation
- 导入状态：preview

## 授权导入：专题/分类解析

> 本节按用户确认的授权导入前提保存来源专题中的分类说明、套路说明和学习建议。题目清单本身仍以 `01-question-bank/by-source-category/` 为准。

### 前言

掌握动态规划（DP）是没有捷径的，咱们唯一能做的，就是投入时间猛猛刷题。好比学数学，只看书看视频而不做习题，是不能说学会的。
我能做的，是帮你节省找题的时间，并把这些题分类整理好。有着相同套路的题，一起做效率会更高，也更能领悟到 DP 的精髓。每个小节的题目已按照难度分排序（右侧数字为难度分）。
**记忆化搜索**是新手村神器（甚至可以用到游戏后期），推荐先看 [动态规划入门：从记忆化搜索到递推](https://www.bilibili.com/video/BV1Xj411K7oF/)。
> 但记忆化搜索并不是万能的，某些题目只有写成递推，才能结合数据结构等来优化时间复杂度，多数题目还可以优化空间复杂度。所以在写完记忆化搜索后，推荐把递推的代码也写一下。熟练之后直接写递推也可以。
有时候，思考「**最后一步发生了什么**」能启发我们找到子问题，进而得到状态定义和状态转移方程。

### 一、入门 DP / §1.2 打家劫舍 / 答疑

**问**：在 1:1 翻译的过程中，如何根据记忆化搜索，确定递推数组（DP 数组）的大小？为什么有时候要开 $n+1$ 大小的数组，有时候要开 $n+2$ 大小的数组？
**答**：看记忆化搜索的参数的范围（最小值和最大值）。例如 $i$ 最小是 $-1$（递归边界也算），最大是 $n-1$（递归入口），那么一共有 $n+1$ 个不同的 $i$，就需要开 $n+1$ 大小的 DP 数组。如果 $i$ 最小是 $-2$，最大是 $n-1$，一共有 $n+2$ 个不同的 $i$，就需要开 $n+2$ 大小的 DP 数组。

### 一、入门 DP / 思考题

完成本章后，请思考：什么时候要返回 $f[n]$，什么时候要返回 $\max(f)$？

### 二、网格图 DP

对于一些二维 DP（例如背包、最长公共子序列），如果把 DP 矩阵画出来，其实状态转移可以视作**在网格图上的移动**。所以在学习相对更抽象的二维 DP 之前，做一些形象的网格图 DP 会让后续的学习更轻松（比如 0-1 背包的空间优化写法为什么要倒序遍历）。

### 三、背包

讲解：[0-1 背包 完全背包【基础算法精讲 18】](https://www.bilibili.com/video/BV16Y411v7Y6/)

### 三、背包 / §3.1 0-1 背包

每个物品只能选一次，即要么选，要么不选。所以 0-1 背包是「选或不选」的代表。
> 关于「枚举选哪个」的代表，见本题单的「§4.2 最长递增子序列」。

### 三、背包 / §3.2 完全背包

物品可以重复选，无个数限制。

### 三、背包 / §3.3 多重背包（选做）

物品可以重复选，有个数限制。
注意求方案数的题目不能用二进制优化。比如从 $6$ 个相同物品中选 $3$ 个，只有一种选法。但按照二进制优化，会把 $6$ 分解为 $1+2+3$，有 $1+2$ 和 $3$ 两种选法。
如果要优化，可以考虑用 [同余前缀和](https://leetcode.cn/problems/count-of-sub-multisets-with-bounded-sum/solutions/2482876/duo-zhong-bei-bao-fang-an-shu-cong-po-su-f5ay/) 优化。

### 三、背包 / §3.4 分组背包

同一组内的物品至多/恰好选一个。

### 三、背包 / §3.5 树上背包（选做）

> **注**：目前力扣只有无依赖的背包，时间复杂度为 $\mathcal{O}(nW^2)$，其中 $n$ 是树的节点个数，$W$ 是背包容量。如果有依赖，可以优化到 $\mathcal{O}(nW)$。

### 四、经典线性 DP / §4.1 最长公共子序列（LCS）

讲解：[最长公共子序列 编辑距离【基础算法精讲 19】](https://www.bilibili.com/video/BV1TM4y1o7ug/)
一般定义 $f[i][j]$ 表示对 $(s[:i],t[:j])$ 的求解结果。

### 四、经典线性 DP / §4.1 最长公共子序列（LCS） / §4.1.2 进阶

思路和代码见 [评论](https://leetcode.cn/problems/maximize-number-of-subsequences-in-a-string/solutions/1352039/by-endlesscheng-yfyf/comments/2389140)。

### 四、经典线性 DP / §4.2 最长递增子序列（LIS）

讲解：[最长递增子序列【基础算法精讲 20】](https://www.bilibili.com/video/BV1ub411Q7sB/)

### 四、经典线性 DP / §4.2 最长递增子序列（LIS） / §4.2.2 进阶

给定整数 $k$，构造一个数组 $a$，使得 $a$ 恰好有 $k$ 个最长递增子序列。

### 五、划分型 DP / §5.1 判定能否划分

一般定义 $f[i]$ 表示长为 $i$ 的前缀 $a[:i]$ 能否划分。
枚举最后一个子数组的左端点 $L$，从 $f[L]$ 转移到 $f[i]$，并考虑 $a[L:i]$ 是否满足要求。

### 五、划分型 DP / §5.2 最优划分

计算最少（最多）可以划分出多少段、最优划分得分等。
一般定义 $f[i]$ 表示长为 $i$ 的前缀 $a[:i]$ 在题目约束下，分割出的最少（最多）子数组个数（或者定义成分割方案数）。
枚举最后一个子数组的左端点 $L$，从 $f[L]$ 转移到 $f[i]$，并考虑 $a[L:i]$ 对最优解的影响。

### 五、划分型 DP / §5.3 约束划分个数

将数组分成（恰好/至多）$k$ 个连续子数组，计算与这些子数组有关的最优值。
一般定义 $f[i][j]$ 表示将长为 $j$ 的前缀 $a[:j]$ 分成 $i$ 个连续子数组所得到的最优解。
枚举最后一个子数组的左端点 $L$，从 $f[i-1][L]$ 转移到 $f[i][j]$，并考虑 $a[L:j]$ 对最优解的影响。
> 注：对于恰好型划分 DP，可以通过控制内层循环的上下界，把时间复杂度从 $\mathcal{O}(nk)$ 优化至 $\mathcal{O}((n-k)k)$。例如 3473 题。

### 六、状态机 DP

一般定义 $f[i][j]$ 表示前缀 $a[:i]$ 在状态 $j$ 下的最优值。$j$ 一般很小。

### 六、状态机 DP / §6.1 买卖股票

[讲解【基础算法精讲 21】](https://www.bilibili.com/video/BV1ho4y1W7QK/)

### 七、其他线性 DP / §7.1 一维 DP

发生在前缀/后缀之间的转移，例如从 $f[i-1]$ 转移到 $f[i]$，或者从 $f[j]$ 转移到 $f[i]$。

### 七、其他线性 DP / §7.4 合法子序列 DP

计算合法子序列的最长长度、个数、元素和等。
一般定义 $f[x]$ 表示以元素 $x$ 结尾的合法子序列的最长长度/个数/元素和，从子序列的倒数第二个数转移过来。
注意这里的 $x$ 不是下标，是元素值。如果 $x$ 不是整数，或者值域范围很大，可以用哈希表代替数组。

### 七、其他线性 DP / §7.6 多维 DP

不会设计状态？那你可要好好刷这一节了。

### 八、区间 DP

讲解：[区间 DP【基础算法精讲 22】](https://www.bilibili.com/video/BV1Gs4y1E7EU/)
从数组的左右两端不断缩短，求解关于某段下标区间的最优值。
一般定义 $f[i][j]$ 表示下标区间 $[i,j]$ 的最优值。

### 八、区间 DP / §8.2 区间 DP

对于类似合法括号字符串（RBS）的消除问题，通常根据题意，会有如下性质：
满足上述性质的题目（例如 3563 题），可以用区间 DP 解决。
定义 $f(i,j)$ 表示消除 $s[i]$ 到 $s[j]$ 的最优值。
- 根据性质 2，可以把 $f(i,j)$ 缩小成子问题 $f(i+1,j-1)$。
- 根据性质 3，可以枚举子串 $B$ 的右端点，即枚举 $k=i+1,i+3,i+5,\ldots,j-2$，把 $f(i,j)$ 划分成子问题 $f(i,k)$ 和 $f(k+1,j)$。注意这里枚举 $k$ 的步长是 $2$，因为每次消除 $2$ 个字符，被消除的子串长度一定是偶数。
边界：$f(i+1,i)$，即空串。
答案：$f(0,n-1)$。
还有一些区间 DP 问题，见本题单的「**十四、博弈 DP**」。

### 九、状态压缩 DP（状压 DP） / §9.1 排列型状压 DP ① 相邻无关

- [教你一步步思考状压 DP：从记忆化搜索到递推](https://leetcode.cn/problems/beautiful-arrangement/solution/jiao-ni-yi-bu-bu-si-kao-zhuang-ya-dpcong-c6kd/)
- [从集合论到位运算，常见位运算技巧分类总结](https://leetcode.cn/circle/discuss/CaOJ45/)
暴力做法是枚举所有排列，对每个排列计算和题目有关的值，时间复杂度（通常来说）是 $\mathcal{O}(n!)$。可以解决 $n\le 10$ 的问题。 
状压 DP 可以把时间复杂度（通常来说）优化至 $\mathcal{O}(n\cdot 2^n)$。可以解决 $n\le 20$ 的问题。
一般有两种定义方式：
> 注：部分题目由于暴搜+剪枝也能过，难度分仅供参考。

### 九、状态压缩 DP（状压 DP） / §9.2 排列型状压 DP ② 相邻相关

一般定义 $f[S][i]$ 表示未选（或者已选）的集合为 $S$，且上一个填的元素（下标）为 $i$ 时，和题目有关的最优值。通过枚举当前位置要填的元素（下标）来转移。
时间复杂度（通常来说）是 $\mathcal{O}(n^2\cdot 2^n)$。

### 九、状态压缩 DP（状压 DP） / §9.3 旅行商问题（TSP）

本质上就是排列型 ②。

### 九、状态压缩 DP（状压 DP） / §9.4 子集状压 DP

一般定义 $f[S]$ 表示未选（或者已选）的集合为 $S$ 时，和题目有关的最优值。通过枚举 $S$（或者 $S$ 的补集 $\complement_US$）的子集来转移。
时间复杂度（通常来说）是 $\mathcal{O}(3^n)$，证明：
对于大小为 $n$ 的集合，它的大小为 $m$ 的子集有 $\binom n m$ 个，每个子集又有 $2^m$ 个子集。根据二项式定理，$\sum\limits_{m=0}^n \binom n m 2^m = (2+1)^n = 3^n$，所以「枚举子集的子集」的总体时间复杂度为 $\mathcal{O}(3^n)$。
> 注：某些网格图的题也可以用「选或不选」来做，从而减少状态个数，详见「§9.5 轮廓线 DP」。

### 九、状态压缩 DP（状压 DP） / §9.6 SOS DP

子集和 DP（Sum Over Subsets DP，SOS DP），国内算法竞赛圈一般叫高维前缀和。
[原理讲解（方法二）](https://leetcode.cn/problems/maximum-product-of-two-integers-with-no-common-bits/solutions/3768219/mo-ban-gao-wei-qian-zhui-he-sos-dppython-78fz/)
模板（从子集转移过来）：
```py [sol-Python3]
# 设 w 为 a[i] 的二进制最大长度
# 返回一个长为 2^w 的数组 f，其中 f[S] 表示 a 中是 S 的子集的元素个数（把二进制数视作集合）
# 时间复杂度 O(n + U log U)，其中 U = max(a)
def sos_dp(a: List[int]) -> List[int]:
    mx = max(a)
    w = mx.bit_length()  # 二进制长度上限
    u = 1 << w
    f = [0] * u

### 十、数位 DP

用于解决 $[\ell,r]$ 范围内有多少个符合要求的元素，或者符合要求的元素之和等问题。

### 十、数位 DP / §10.1 统计合法元素的数目

[数位 DP v1.0 模板讲解](https://www.bilibili.com/video/BV1rS4y1s721/?t=19m36s)
[数位 DP v2.0 模板讲解](https://www.bilibili.com/video/BV1Fg4y1Q7wv/?t=31m28s)（上下界数位 DP）
**问**：下面的代码，为什么只在 `!limitLow && !limitHigh` 成立时才记忆化？（Python 选手可以跳过这个问题）
**答**：记忆化的原理是，当我们再次遇到相同状态时，可以直接返回 $\textit{memo}$ 中保存的结果。数位 DP 本质是暴力枚举，枚举每个数位填什么。$\textit{low}$ 是我们枚举的第一个数，$\textit{high}$ 是我们枚举的最后一个数。所以「填入的数字组成了 $\textit{low}$」以及「填入的数字组成了 $\textit{high}$」在整个递归过程中只会枚举一次。状态 $(\ldots, \textit{limitLow}, \textit{limitHigh})$ 中的 $\textit{limitLow}$ 和 $\textit{limitHigh}$ 如果其中一个是 $\texttt{true}$，说明我们正在填 $\textit{low}$ 或者正在填 $\textit{high}$，**这样的状态只会出现一次，不会再次遇到**，所以不需要记忆化这种状态。
下面是数位 DP v2.1 模板。相比 v2.0，不需要写 $\textit{isNum}$ 参数（之前是否填过数字）。
> 注：只有上界约束的题目，相当于 $\textit{low}=0$ 或者 $\textit{low}=1$。
```py [sol-Python3]
# 代码示例：返回 [low, high] 中的恰好包含 target 个 0 的整数个数
# 比如 digitDP(0, 10, 1) == 2
# 要点：我们统计的是 0 的个数，需要区分「前导零」与「整数中的零」，前导零不能计入，而整数中的零需要计入
def digitDP(low: int, high: int, target: int) -> int:
    low_s = list(map(int, str(low)))  # 避免在 dfs 中频繁调用 int()

### 十、数位 DP / §10.2 统计合法元素的价值总和

每个元素 $x$ 都有一个相应的价值 $f(x)$，计算 $[\textit{low},\textit{high}]$ 中的满足题目约束的元素的价值总和。
例如 $f(x) = x$ 是计算满足题目约束的**元素和**，$f(x) = \text{digsum}(x)$ 是计算满足题目约束的元素的**数位和**。
模板（数位和）：
```py [sol-Python3]
# 计算在 [low, high] 中的整数 x 的数位和，满足 x 中的不同数字个数不超过 k
def digitDPContribution(low: int, high: int, k: int) -> int:
    low_s = list(map(int, str(low)))  # 避免在 dfs 中频繁调用 int()
    high_s = list(map(int, str(high)))
    n = len(high_s)
    diff_lh = n - len(low_s)
    # dfs 返回两个数：子树合法整数个数，子树数位总和
    def dfs(i: int, mask: int, limit_low: bool, limit_high: bool) -> Tuple[int, int]:

### 十一、优化 DP / §11.2 单调栈优化 DP

前置题单：[单调栈（矩形系列/字典序最小/贡献法）](https://leetcode.cn/circle/discuss/9oZFK9/)

### 十一、优化 DP / §11.3 单调队列优化 DP

一般用来维护一段转移来源的最值。

### 十一、优化 DP / §11.6 矩阵快速幂优化 DP

有两种类型的矩阵快速幂优化 DP：
时间复杂度一般为 $\mathcal{O}(k^3\log n)$。
**进阶做法**：先用 **Berlekamp-Massey 算法**找规律，得到线性递推式，然后用 **Kitamasa 算法**（或者 Bostan-Mori 算法）优化。
请看我的知乎科普文章：
- [Berlekamp-Massey 算法：如何预测数列的下一项？](https://zhuanlan.zhihu.com/p/1966417899825665440)
- [Kitamasa 算法：更快地计算线性递推的第 n 项](https://zhuanlan.zhihu.com/p/1964051212304364939)
具体例子见 3700 题 [我的题解的方法二](https://leetcode.cn/problems/number-of-zigzag-arrays-ii/solutions/3794101/ju-zhen-kuai-su-mi-you-hua-dppythonnumpy-77e7/)。
时间复杂度一般为 $\mathcal{O}(k^2\log n)$。

### 十一、优化 DP / §11.7 斜率优化 DP

也叫凸包优化/凸壳优化（CHT，Convex Hull Trick）。

### 十一、优化 DP / §11.8 WQS 二分优化 DP

本质是凸优化中的**拉格朗日松弛** (Lagrangian Relaxation)。
把最多选 $k$ 个物品的问题（时间复杂度高）转换成选任意个物品的问题（时间复杂度低）。
一般时间复杂度为 $\mathcal{O}(n\log U)$ 或者 $\mathcal{O}(n\log n)$。
此外「§5.3 约束划分个数」的部分题目也可以用 WQS 二分优化。

### 十一、优化 DP / §11.9 其他优化 DP

关于**倍增算法**，见 [题单](https://leetcode.cn/circle/discuss/K0n2gO/) 的「§3.8 最近公共祖先（LCA）、倍增算法」。

### 十二、树形 DP

**注**：可能有同学觉得树形 DP 没有重复访问同一个状态（重叠子问题），并不能算作 DP，而是算作普通的递归。这么说也有一定道理，不过考虑到思维方式和 DP 是一样的自底向上，所以仍然叫做树形 DP。此外，如果是自顶向下的递归做法，是存在重叠子问题的，一般要结合记忆化搜索实现。

### 十二、树形 DP / §12.1 树的直径

讲解：[树形 DP：树的直径【基础算法精讲 23】](https://www.bilibili.com/video/BV17o4y187h1/)
> 注：求直径也有两次 DFS 的做法。

### 十二、树形 DP / §12.2 树上最大独立集

讲解：[树形 DP：打家劫舍 III【基础算法精讲 24】](https://www.bilibili.com/video/BV1vu4y1f7dn/)

### 十二、树形 DP / §12.3 树上最小支配集

讲解：[树形 DP：监控二叉树【基础算法精讲 25】](https://www.bilibili.com/video/BV1oF411U7qL/)，包含 968 的变形题。

### 十二、树形 DP / §12.4 换根 DP

> 也叫二次扫描法。
设 $y$ 是 $x$ 的子节点。从「以 $x$ 为整棵树的根」变成「以 $y$ 为整棵树的根」，发生了什么？
[【图解】一张图秒懂换根 DP！](https://leetcode.cn/problems/sum-of-distances-in-tree/solution/tu-jie-yi-zhang-tu-miao-dong-huan-gen-dp-6bgb/)
**模板一：简单场景**（例如 3772 题）
```py [sol-Python3]
class Solution:
    def maxSubgraphScore(self, n: int, edges: List[List[int]], good: List[int]) -> List[int]:
        g = [[] for _ in range(n)]
        for x, y in edges:
            g[x].append(y)
            g[y].append(x)
        # sub_score[x] 表示（以 0 为根时）包含 x 的子树 x 的最大得分

### 十三、图 DP

另见[【题单】图论算法](https://leetcode.cn/circle/discuss/01LUak/) 中的「全源最短路：Floyd」，本质是多维 DP。

### 十五、概率 DP、期望 DP

- [九坤-04. 筹码游戏](https://leetcode.cn/contest/ubiquant2022/problems/I3Gm2h/)

### 专题：输出具体方案（打印方案）

注意这些题目和回溯的区别，某些回溯题目要求输出**所有**方案，这里只要求输出**一个**。

### 专题：前后缀分解

部分题目也可以用状态机 DP 或者分组循环（见双指针题单）解决。
如果涉及到的只是若干元素，而不是前缀/后缀这样的一段元素，也可以用「枚举右，维护左」思考，见数据结构题单。

### 算法题单

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)
欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)
如果你发现有题目可以补充进来，欢迎评论反馈。

## 授权题解链接索引

| 分类路径 | 标题 | 链接 | 导入状态 |
|---|---|---|---|
| 二、网格图 DP | 讲解 | https://leetcode.cn/problems/minimum-path-sum/solutions/3045828/jiao-ni-yi-bu-bu-si-kao-dpcong-ji-yi-hua-zfb2/ | pending-fetch |
| 三、背包 / §3.3 多重背包（选做） | 同余前缀和 | https://leetcode.cn/problems/count-of-sub-multisets-with-bounded-sum/solutions/2482876/duo-zhong-bei-bao-fang-an-shu-cong-po-su-f5ay/ | pending-fetch |
| 四、经典线性 DP / §4.1 最长公共子序列（LCS） / §4.1.2 进阶 | 评论 | https://leetcode.cn/problems/maximize-number-of-subsequences-in-a-string/solutions/1352039/by-endlesscheng-yfyf/comments/2389140/ | pending-fetch |
| 九、状态压缩 DP（状压 DP） / §9.6 SOS DP | 原理讲解（方法二） | https://leetcode.cn/problems/maximum-product-of-two-integers-with-no-common-bits/solutions/3768219/mo-ban-gao-wei-qian-zhui-he-sos-dppython-78fz/ | pending-fetch |
| 十一、优化 DP / §11.6 矩阵快速幂优化 DP | 讲解（含模板代码） | https://leetcode.cn/problems/climbing-stairs/solutions/2560716/jiao-ni-yi-bu-bu-si-kao-dong-tai-gui-hua-7zm1/ | pending-fetch |
| 十一、优化 DP / §11.6 矩阵快速幂优化 DP | 讲解（含模板代码） | https://leetcode.cn/problems/knight-dialer/solutions/3004116/jiao-ni-yi-bu-bu-si-kao-dpcong-ji-yi-hua-x06l/ | pending-fetch |
| 十一、优化 DP / §11.6 矩阵快速幂优化 DP | 我的题解的方法二 | https://leetcode.cn/problems/number-of-zigzag-arrays-ii/solutions/3794101/ju-zhen-kuai-su-mi-you-hua-dppythonnumpy-77e7/ | pending-fetch |
| 十一、优化 DP / §11.7 斜率优化 DP | 讲解 | https://leetcode.cn/problems/minimum-partition-score/solutions/3893573/hua-fen-xing-dp-xie-lu-you-hua-tu-bao-yo-5cb0/ | pending-fetch |

## 本地原创增强解析

动态规划 应优先沿用来源页的标题层级学习：先做低难度分题目，用同一套路反复观察边界条件；再回到进阶题处理多约束、多状态或跨专题组合。

### 分类层级说明

本专题的本地分类路径完全来自来源页面的 Markdown 标题层级，后续单题详解会回链到这些路径。

### 建议刷题节奏

先完成每个小节前 3–5 题，确认模板和边界；再按难度分上升补齐。遇到看完题解仍无法复述关键观察的题，先标记复盘，不阻塞下一小节。
