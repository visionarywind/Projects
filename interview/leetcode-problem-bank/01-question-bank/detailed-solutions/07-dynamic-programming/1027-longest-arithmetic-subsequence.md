# 1027. 最长等差数列

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/longest-arithmetic-subsequence/
- 题目 slug：`longest-arithmetic-subsequence`
- 来源专题：动态规划
- 来源分类路径：七、其他线性 DP / §7.4 合法子序列 DP
- 难度分：1759
- 外部题解来源：https://leetcode.cn/problems/longest-arithmetic-subsequence/solutions/2239191/ji-yi-hua-sou-suo-di-tui-chang-shu-you-h-czvx/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种动态规划的定义方式（Python/Java/C++/Go）](https://leetcode.cn/problems/longest-arithmetic-subsequence/solutions/2239191/ji-yi-hua-sou-suo-di-tui-chang-shu-you-h-czvx/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`ji-yi-hua-sou-suo-di-tui-chang-shu-you-h-czvx`
- topic id：`2239191`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 16:46:02 +0800

## 前言

本题有两种思路：

- 考察子序列的最后两项。
- 枚举公差，然后考察子序列的最后一项。

方法一较为麻烦，想看简洁做法的同学可以直接看方法二。

## 方法一：考察子序列最后两项

### 前置知识：动态规划入门

详见 [动态规划入门：从记忆化搜索到递推【基础算法精讲 17】](https://www.bilibili.com/video/BV1Xj411K7oF/)。

### 一、寻找子问题

对于动态规划问题，通常可以从「选或不选」和「枚举选哪个」这两个角度入手。

看到子序列，你可能想到了「选或不选」这个思路，但是本题要寻找的是等差子序列，假设我们确定了等差子序列的末项和公差，那么其它数也就确定了，所以寻找等差子序列更像是一件「枚举选哪个」的事情了。

为方便描述，下文将 $\textit{nums}$ 简记为 $a$，将最长等差子序列称作 LAS。

例如 $a=[9,4,7,2,10]$。假设 $a[4]=10$ 是 LAS 的最后一项，公差 $d=3$，那么倒数第二项就是 $10-3=7$，我们需要在前面找到 $7$ 的位置，如果有多个 $7$，则应该贪心取最靠右的，从而更有机会找到更长的 LAS。这样，问题就变成以 $7$ 结尾的公差为 $3$ 的 LAS 的长度。由于有很多相似的子问题，可以用递归解决。

> 先来试试定义成 $\textit{dfs}(i,d)$，表示以 $a[i]$ 结尾的公差为 $d$ 的 LAS 的长度。那么需要在前面找到 $a[j]=a[i]-d$，然后继续递归 $\textit{dfs}(j,d)$。
> 
> 如何找到 $a[i]-d$？
> - 暴力枚举：需要花费 $\mathcal{O}(n)$ 的时间。
> - 预处理相同元素的位置列表，然后在列表中二分查找：预处理 $\mathcal{O}(n)$，二分查找 $\mathcal{O}(\log n)$。
> 
> 无论如何，总是有多余的时间浪费在查找元素上了。

再来观察 $a=[9,4,7,2,10]$。对于 $a[2]=7$ 来说，它和前面的元素形成了公差分别为 $7-9=-2$ 和 $7-4=3$ 的 LAS，长度均为 $2$。对于 $a[4]=10$，它与 $a[2]=7$ 形成子序列时，由于已经知道以 $a[2]$ 结尾的公差为 $3$ 的 LAS 的长度为 $2$，所以立刻得出以 $a[4]$ 结尾的公差为 $3$ 的 LAS 的长度为 $2+1=3$。

那么把**所有**以 $a[i]$ 结尾的（至少有两个元素的）LAS 的公差及其长度都算出来，存到一个哈希表中，$a[i]$ 右边的数 $x$ 就可以直接去哈希表中查找公差 $d=x-a[i]$ 对应的 LAS 长度了。

因此我们换个角度，定义成 $\textit{dfs}(i)$，它返回上面说的哈希表。由于 $a[i]$ 和前面的元素至多形成 $i$ 个公差不同的 LAS，所以哈希表的大小至多为 $i$。

具体来说，对于 $\textit{dfs}(i)$，维护一个哈希表 $\textit{maxLen}$，枚举所有 $j<i$，设公差 $d=a[i]-a[j]$，则更新

$$
\textit{maxLen}[d] = \max(\textit{maxLen}[d], \textit{dfs}(j)[d] + 1)
$$

> 注：考虑到 $j$ 越大 $\textit{dfs}(j)[d]$ 也越大，所以代码实现时可以倒序遍历 $j$，对 $\textit{maxLen}[d]$ 只更新一次。这样执行用时更短。

如果 $d$ 不在 $\textit{dfs}(j)$ 的返回值中，则 $\textit{dfs}(j)[d]=1$，相当于 $a[j]$ 是 LAS 的首项。

递归边界：$i=0$ 时，返回空哈希表，因为 $a[0]$ 左边没有元素。注意我们必须要把公差算出来，不考虑只有一个元素的 LAS。

递归入口：计算从 $\textit{dfs}(1)$ 到 $\textit{dfs}(n-1)$ 的所有哈希表中的 LAS 的长度的最大值。

```py [sol1-Python3]
class Solution:
    def longestArithSeqLength(self, a: List[int]) -> int:
        @cache  # 缓存装饰器，避免重复计算 dfs 的结果
        def dfs(i: int) -> dict[int, int]:
            # i=0 时不会进入循环，返回空哈希表
            max_len = {}
            for j in range(i - 1, -1, -1):
                d = a[i] - a[j]  # 公差
                if d not in max_len:
                    max_len[d] = dfs(j).get(d, 1) + 1
            return max_len
        return max(max(dfs(i).values()) for i in range(1, len(a)))
```

```java [sol1-Java]
class Solution {
    private Map<Integer, Integer>[] maxLen;
    private int[] a;
    private int ans;

    public int longestArithSeqLength(int[] nums) {
        a = nums;
        int n = a.length;
        maxLen = new HashMap[n];
        for (int i = 1; i < n; ++i)
            dfs(i);
        return ans;
    }

    private Map<Integer, Integer> dfs(int i) {
        if (maxLen[i] != null) return maxLen[i]; // 之前算过了
        // i=0 时不会进入循环
        maxLen[i] = new HashMap<>();
        for (int j = i - 1; j >= 0; --j) {
            int d = a[i] - a[j]; // 公差
            if (!maxLen[i].containsKey(d)) {
                maxLen[i].put(d, dfs(j).getOrDefault(d, 1) + 1);
                ans = Math.max(ans, maxLen[i].get(d));
            }
        }
        return maxLen[i];
    }
}
```

```cpp [sol1-C++]
class Solution {
public:
    int longestArithSeqLength(vector<int> &a) {
        int ans = 0, n = a.size();
        unordered_map<int, int> max_len[n];
        function<void(int)> dfs = [&](int i) {
            if (!max_len[i].empty()) return; // 之前算过了
            for (int j = i - 1; j >= 0; --j) {
                int d = a[i] - a[j]; // 公差
                if (!max_len[i].count(d)) {
                    dfs(j); // 下面直接用 max_len[j] 拿到结果
                    auto it = max_len[j].find(d);
                    max_len[i][d] = it != max_len[j].end() ? it->second + 1 : 2;
                    ans = max(ans, max_len[i][d]);
                }
            }
        };
        for (int i = 1; i < n; ++i)
            dfs(i);
        return ans;
    }
};
```

```go [sol1-Go]
func longestArithSeqLength(a []int) (ans int) {
    n := len(a)
    maxLen := make([]map[int]int, n)
    var dfs func(int) map[int]int
    dfs = func(i int) map[int]int {
        if maxLen[i] != nil { // 之前算过了
            return maxLen[i]
        }
        maxLen[i] = map[int]int{}
        for j := i - 1; j >= 0; j-- {
            d := a[i] - a[j] // 公差
            if maxLen[i][d] == 0 {
                maxLen[i][d] = dfs(j)[d] + 1 // 默认的 1 在下面返回时加上
                ans = max(ans, maxLen[i][d])
            }
        }
        return maxLen[i]
    }
    for i := 1; i < n; i++ {
        dfs(i)
    }
    return ans + 1
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2)$，其中 $n$ 为 $a$ 的长度。动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。这里状态个数为 $\mathcal{O}(n)$，单个状态的计算时间为 $\mathcal{O}(n)$，因此时间复杂度为 $\mathcal{O}(n^2)$。
- 空间复杂度：$\mathcal{O}(n^2)$。有 $\mathcal{O}(n)$ 个状态，每个状态需要 $\mathcal{O}(n)$ 的空间。

### 二、1:1 翻译成递推

我们可以去掉递归中的「递」，只保留「归」的部分，即自底向上计算。

通用做法：

- $\textit{dfs}$ 改成 $f$ 数组；
- 递归改成循环（每个参数都对应一层循环）；
- 递归边界改成 $f$ 数组的初始值。

> 相当于原来是用递归去计算每个状态，现在是按照某个顺序去枚举并计算每个状态。

由于递归中 $i$ 不会为负，所以无需做任何下标的调整，照搬上面的代码就完事了。

```py [sol21-Python3]
class Solution:
    def longestArithSeqLength(self, a: List[int]) -> int:
        f = [{} for _ in range(len(a))]
        for i, x in enumerate(a):
            for j in range(i - 1, -1, -1):
                d = x - a[j]  # 公差
                if d not in f[i]:
                    f[i][d] = f[j].get(d, 1) + 1
        return max(max(d.values()) for d in f[1:])
```

```java [sol21-Java]
class Solution {
    public int longestArithSeqLength(int[] a) {
        int ans = 0, n = a.length;
        Map<Integer, Integer>[] f = new HashMap[n];
        Arrays.setAll(f, e -> new HashMap<>());
        for (int i = 1; i < n; ++i)
            for (int j = i - 1; j >= 0; --j) {
                int d = a[i] - a[j]; // 公差
                if (!f[i].containsKey(d)) {
                    f[i].put(d, f[j].getOrDefault(d, 1) + 1);
                    ans = Math.max(ans, f[i].get(d));
                }
            }
        return ans;
    }
}
```

```cpp [sol21-C++]
class Solution {
public:
    int longestArithSeqLength(vector<int> &a) {
        int ans = 0, n = a.size();
        unordered_map<int, int> f[n];
        for (int i = 1; i < n; ++i)
            for (int j = i - 1; j >= 0; --j) {
                int d = a[i] - a[j]; // 公差
                if (!f[i].count(d)) {
                    auto it = f[j].find(d);
                    f[i][d] = it != f[j].end() ? it->second + 1 : 2;
                    ans = max(ans, f[i][d]);
                }
            }
        return ans;
    }
};
```

```go [sol21-Go]
func longestArithSeqLength(a []int) (ans int) {
    n := len(a)
    f := make([]map[int]int, n)
    f[0] = map[int]int{}
    for i := 1; i < n; i++ {
        f[i] = map[int]int{}
        for j := i - 1; j >= 0; j-- {
            d := a[i] - a[j] // 公差
            if f[i][d] == 0 {
                f[i][d] = f[j][d] + 1 // 默认的 1 在下面返回时加上
                ans = max(ans, f[i][d])
            }
        }
    }
    return ans + 1
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2)$，其中 $n$ 为 $a$ 的长度。动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。这里状态个数为 $\mathcal{O}(n)$，单个状态的计算时间为 $\mathcal{O}(n)$，因此时间复杂度为 $\mathcal{O}(n^2)$。
- 空间复杂度：$\mathcal{O}(n^2)$。有 $\mathcal{O}(n)$ 个状态，每个状态需要 $\mathcal{O}(n)$ 的空间。

### 三、常数优化

由于值域比较小，还可以用数组代替哈希表。

设 $\textit{maxD}=\max(a)-\min(a)$，那么公差范围在 $[-\textit{maxD},\textit{maxD}]$，这一共有 $2\cdot\textit{maxD}+1$ 个数。代码实现时可以简单地用 $2\cdot 500 + 1 = 1001$ 代替。

> 注：该优化在 Python 上并不明显。

```py [sol22-Python3]
class Solution:
    def longestArithSeqLength(self, a: List[int]) -> int:
        ans, max_d = 0, max(a) - min(a)
        f = [[0] * (max_d * 2 + 1) for _ in range(len(a))]
        for i, x in enumerate(a):
            for j in range(i - 1, -1, -1):
                d = x - a[j]  # 公差
                if f[i][d] == 0:
                    f[i][d] = f[j][d] + 1  # 默认的 1 在下面返回时加上
                    if f[i][d] > ans:
                        ans = f[i][d]
        return ans + 1
```

```java [sol22-Java]
class Solution {
    public int longestArithSeqLength(int[] a) {
        int ans = 0, n = a.length;
        var f = new int[n][1001];
        for (int i = 1; i < n; ++i)
            for (int j = i - 1; j >= 0; --j) {
                int d = a[i] - a[j] + 500; // +500 防止出现负数
                if (f[i][d] == 0) {
                    f[i][d] = f[j][d] + 1; // 默认的 1 在下面返回时加上
                    ans = Math.max(ans, f[i][d]);
                }
            }
        return ans + 1;
    }
}
```

```cpp [sol22-C++]
class Solution {
public:
    int longestArithSeqLength(vector<int> &a) {
        int ans = 0, n = a.size(), f[n][1001];
        memset(f, 0, sizeof(f));
        for (int i = 1; i < n; ++i)
            for (int j = i - 1; j >= 0; --j) {
                int d = a[i] - a[j] + 500; // +500 防止出现负数
                if (f[i][d] == 0) {
                    f[i][d] = f[j][d] + 1; // 默认的 1 在下面返回时加上
                    ans = max(ans, f[i][d]);
                }
            }
        return ans + 1;
    }
};
```

```go [sol22-Go]
func longestArithSeqLength(a []int) (ans int) {
    n := len(a)
    f := make([][1001]int, n)
    for i := 1; i < n; i++ {
        for j := i - 1; j >= 0; j-- {
            d := a[i] - a[j] + 500 // +500 防止出现负数
            if f[i][d] == 0 {
                f[i][d] = f[j][d] + 1 // 默认的 1 在下面返回时加上
                ans = max(ans, f[i][d])
            }
        }
    }
    return ans + 1
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n(n+D))$，其中 $n$ 为 $a$ 的长度，$D=\max(a)-\min(a)$。初始化需要 $\mathcal{O}(nD)$。动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。这里状态个数为 $\mathcal{O}(n)$，单个状态的计算时间为 $\mathcal{O}(n)$，因此时间复杂度为 $\mathcal{O}(nD+n^2)=\mathcal{O}(n(n+D))$。
- 空间复杂度：$\mathcal{O}(nD)$。有 $\mathcal{O}(n)$ 个状态，每个状态需要 $\mathcal{O}(D)$ 的空间。

## 方法二：枚举公差，考察子序列最后一项

设 $m=\max(\textit{nums})$。

枚举公差，即子序列相邻两项的差 $d=-m,-(m-1),\cdots,-1,0,1,\cdots,m-1,m$。

> **注**：也可以计算 $\textit{nums}$ 的最小值，得到更小的公差范围，这里为方便编程直接认为最小值为 $0$。

如果知道了子序列的最后一项（假设是 $x$），那么子序列的倒数第二项就是 $x-d$。

从左到右遍历 $\textit{nums}$，遍历的同时，维护一个数组 $f[x]$，表示最后一项为 $x$ 的子序列的长度。

我们可以在「最后一项为 $x-d$ 的子序列」的末尾添加 $x$，那么「最后一项为 $x$ 的子序列」的长度会增加 $1$，即

$$
f[x] = f[x-d] + 1
$$

注意上式必须满足 $0\le x-d\le m$，如果不满足则 $x$ 单独形成一个长为 $1$ 的子序列，即 $f[x]=1$。

综上所述，有

$$
f[x] =
\begin{cases}
f[x-d] + 1, & 0\le x-d\le m     \\
1, & \text{otherwise}     \\
\end{cases}
$$

遍历结束后（或者遍历中），用 $f[i]$ 更新答案的最大值。

```py [sol-Python3]
class Solution:
    def longestArithSeqLength(self, nums: List[int]) -> int:
        ans = 0
        m = max(nums)
        for d in range(-m, m + 1):
            f = [0] * (m + 1)
            for x in nums:
                f[x] = f[x - d] + 1 if 0 <= x - d <= m else 1
            ans = max(ans, max(f))
        return ans
```

```java [sol-Java]
class Solution {
    public int longestArithSeqLength(int[] nums) {
        int ans = 0;
        int m = 0;
        for (int x : nums) {
            m = Math.max(m, x);
        }
        for (int d = -m; d <= m; d++) {
            int[] f = new int[m + 1];
            for (int x : nums) {
                f[x] = 0 <= x - d && x - d <= m ? f[x - d] + 1 : 1;
                ans = Math.max(ans, f[x]);
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int longestArithSeqLength(vector<int>& nums) {
        int ans = 0;
        int m = ranges::max(nums);
        for (int d = -m; d <= m; d++) {
            vector<int> f(m + 1);
            for (int x : nums) {
                f[x] = 0 <= x - d && x - d <= m ? f[x - d] + 1 : 1;
                ans = max(ans, f[x]);
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func longestArithSeqLength(nums []int) int {
    ans := 2
    m := slices.Max(nums)
    f := make([]int, m+1)
    for d := -m; d <= m; d++ {
        clear(f)
        for _, x := range nums {
            if 0 <= x-d && x-d <= m {
                f[x] = f[x-d] + 1
                ans = max(ans, f[x])
            } else {
                f[x] = 1
            }
        }
    }
    return ans
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(m(m+n))$，其中 $n$ 是 $\textit{nums}$ 的长度，$m=\max(\textit{nums})$。注意创建大小为 $m$ 的数组需要 $\mathcal{O}(m)$ 的时间。
- 空间复杂度：$\mathcal{O}(m)$。这比方法一更优。

#### 专题训练

见动态规划题单的「**§7.4 合法子序列 DP**」。

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

## 本地原创解析

### 1. 题意重述

本题来自 `七、其他线性 DP / §7.4 合法子序列 DP`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `七、其他线性 DP / §7.4 合法子序列 DP`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
