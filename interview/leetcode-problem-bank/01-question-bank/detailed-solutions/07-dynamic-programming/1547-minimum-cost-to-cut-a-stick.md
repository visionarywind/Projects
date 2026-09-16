# 1547. 切棍子的最小成本

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-cost-to-cut-a-stick/
- 题目 slug：`minimum-cost-to-cut-a-stick`
- 来源专题：动态规划
- 来源分类路径：八、区间 DP / §8.2 区间 DP
- 难度分：2116
- 外部题解来源：https://leetcode.cn/problems/minimum-cost-to-cut-a-stick/solutions/2982639/jiao-ni-yi-bu-bu-si-kao-qu-jian-dpcong-j-f8px/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[教你一步步思考区间 DP：从记忆化搜索到递推（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/minimum-cost-to-cut-a-stick/solutions/2982639/jiao-ni-yi-bu-bu-si-kao-qu-jian-dpcong-j-f8px/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`jiao-ni-yi-bu-bu-si-kao-qu-jian-dpcong-j-f8px`
- topic id：`2982639`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 一、寻找子问题

示例 1 的 $\textit{cuts}=[1,3,4,5]$，为方便描述，把 $0$ 和 $n=7$ 也视作切开的位置（木棍端点），得到 $\textit{cuts}=[0,1,3,4,5,7]$。

我们要解决的问题（原问题）是：

- 切割一根左端点为 $\textit{cuts}[0]=0$，右端点为 $\textit{cuts}[5]=7$ 的棍子的最小成本。

第一刀切在哪？枚举：

- 在 $\textit{cuts}[1]=1$ 切一刀，木棍分成两段。第一段左端点为 $\textit{cuts}[0]=0$，右端点为 $\textit{cuts}[1]=1$；第二段左端点为 $\textit{cuts}[1]=1$，右端点为 $\textit{cuts}[5]=7$。
- 在 $\textit{cuts}[2]=3$ 切一刀，木棍分成两段。第一段左端点为 $\textit{cuts}[0]=0$，右端点为 $\textit{cuts}[2]=3$；第二段左端点为 $\textit{cuts}[2]=3$，右端点为 $\textit{cuts}[5]=7$。
- 在 $\textit{cuts}[3]=4$ 切一刀，木棍分成两段。第一段左端点为 $\textit{cuts}[0]=0$，右端点为 $\textit{cuts}[3]=4$；第二段左端点为 $\textit{cuts}[3]=4$，右端点为 $\textit{cuts}[5]=7$。
- 在 $\textit{cuts}[4]=5$ 切一刀，木棍分成两段。第一段左端点为 $\textit{cuts}[0]=0$，右端点为 $\textit{cuts}[4]=5$；第二段左端点为 $\textit{cuts}[4]=5$，右端点为 $\textit{cuts}[5]=7$。

接下来，继续计算这两段木棍各自的最小切割成本。同样地，枚举切割的位置。依此类推。

这些问题都是**和原问题相似的、规模更小的子问题**，可以用**递归**解决。

> 注：动态规划有「**选或不选**」和「**枚举选哪个**」两种基本思考方式。本题用到的是「枚举选哪个」。

## 二、状态定义与状态转移方程

根据上面的讨论，我们需要在递归过程中，知道当前切的这根棍子，左端点在哪，右端点在哪。

因此，定义状态为 $\textit{dfs}(i,j)$，表示切割一根左端点为 $\textit{cuts}[i]$，右端点为 $\textit{cuts}[j]$ 的棍子的最小成本。

枚举在 $\textit{cuts}[k]$ 处切一刀，其中 $k=i+1,i+2,\ldots,j-1$，木棍变成两段：

- 第一段左端点为 $\textit{cuts}[i]$，右端点为 $\textit{cuts}[k]$，切割这段木棍的最小成本为 $\textit{dfs}(i,k)$。
- 第二段左端点为 $\textit{cuts}[k]$，右端点为 $\textit{cuts}[j]$，切割这段木棍的最小成本为 $\textit{dfs}(k,j)$。
- 成本之和为 $\textit{dfs}(i,k) + \textit{dfs}(k,j)$，再算上切割之前木棍的长度 $\textit{cuts}[j] - \textit{cuts}[i]$，得到

$$
\textit{dfs}(i,k) + \textit{dfs}(k,j) + \textit{cuts}[j] - \textit{cuts}[i]
$$

枚举 $k=i+1,i+2,\ldots,j-1$，所有成本取最小值，得

$$
\textit{dfs}(i,j) = \min\limits_{k=i+1}^{j-1} \textit{dfs}(i,k) + \textit{dfs}(k,j) + \textit{cuts}[j] - \textit{cuts}[i]
$$

其中 $\textit{cuts}[j] - \textit{cuts}[i]$ 与 $k$ 无关，可以提到循环外面。

**递归边界**：$\textit{dfs}(i,i+1)=0$。此时木棍中没有要切割的位置，所以切割成本为 $0$。

**递归入口**：$\textit{dfs}(0,m-1)$，也就是答案。其中 $m$ 是添加了 $0$ 和 $n$ 之后的 $\textit{cuts}$ 数组的长度。

## 三、递归搜索 + 保存递归返回值 = 记忆化搜索

考虑到整个递归过程中有大量重复递归调用（递归入参相同）。由于递归函数没有副作用，同样的入参无论计算多少次，算出来的结果都是一样的，因此可以用**记忆化搜索**来优化：

- 如果一个状态（递归入参）是第一次遇到，那么可以在返回前，把状态及其结果记到一个 $\textit{memo}$ 数组中。
- 如果一个状态不是第一次遇到（$\textit{memo}$ 中保存的结果不等于 $\textit{memo}$ 的初始值），那么可以直接返回 $\textit{memo}$ 中保存的结果。

**注意**：$\textit{memo}$ 数组的**初始值**一定不能等于要记忆化的值！例如初始值设置为 $0$，并且要记忆化的 $\textit{dfs}(i,j)$ 也等于 $0$，那就没法判断 $0$ 到底表示第一次遇到这个状态，还是表示之前遇到过了，从而导致记忆化失效。一般把初始值设置为 $-1$。本题由于 $\textit{cuts}[j] - \textit{cuts}[i] > 0$，所以除了递归边界以外，$\textit{dfs}$ 的返回值均为正数，所以也可以把初始值设置为 $0$。

> Python 用户可以无视上面这段，直接用 `@cache` 装饰器。

具体请看视频讲解 [动态规划入门：从记忆化搜索到递推【基础算法精讲 17】](https://www.bilibili.com/video/BV1Xj411K7oF/) 以及 [区间 DP【基础算法精讲 22】](https://www.bilibili.com/video/BV1Gs4y1E7EU/)。

```py [sol-Python3]
class Solution:
    def minCost(self, n: int, cuts: List[int]) -> int:
        cuts.sort()
        cuts = [0] + cuts + [n]

        @cache  # 缓存装饰器，避免重复计算 dfs 的结果（记忆化）
        def dfs(i: int, j: int) -> int:
            if i + 1 == j:  # 无需切割
                return 0
            # 枚举切割位置 cuts[k]
            return min(dfs(i, k) + dfs(k, j) for k in range(i + 1, j)) + cuts[j] - cuts[i]

        return dfs(0, len(cuts) - 1)
```

```java [sol-Java]
class Solution {
    public int minCost(int n, int[] cuts) {
        Arrays.sort(cuts);
        int m = cuts.length + 2;
        int[] newCuts = new int[m];
        System.arraycopy(cuts, 0, newCuts, 1, m - 2);
        newCuts[m - 1] = n;

        int[][] memo = new int[m][m];
        return dfs(0, m - 1, newCuts, memo);
    }

    private int dfs(int i, int j, int[] cuts, int[][] memo) {
        if (i + 1 == j) { // 无需切割
            return 0;
        }
        if (memo[i][j] > 0) { // 之前计算过
            return memo[i][j];
        }
        int res = Integer.MAX_VALUE;
        for (int k = i + 1; k < j; k++) {
            res = Math.min(res, dfs(i, k, cuts, memo) + dfs(k, j, cuts, memo));
        }
        return memo[i][j] = res + cuts[j] - cuts[i];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minCost(int n, vector<int>& cuts) {
        cuts.push_back(0);
        cuts.push_back(n);
        ranges::sort(cuts);

        int m = cuts.size();
        vector<vector<int>> memo(m, vector<int>(m));
        auto dfs = [&](auto& dfs, int i, int j) -> int {
            if (i + 1 == j) { // 无需切割
                return 0;
            }
            int& res = memo[i][j]; // 注意这里是引用
            if (res) { // 之前计算过
                return res;
            }
            res = INT_MAX;
            for (int k = i + 1; k < j; k++) {
                res = min(res, dfs(dfs, i, k) + dfs(dfs, k, j));
            }
            res += cuts[j] - cuts[i];
            return res;
        };
        return dfs(dfs, 0, m - 1);
    }
};
```

```go [sol-Go]
func minCost(n int, cuts []int) int {
    cuts = append(cuts, 0, n)
    slices.Sort(cuts)

    m := len(cuts)
    memo := make([][]int, m)
    for i := range memo {
        memo[i] = make([]int, m)
    }
    var dfs func(int, int) int
    dfs = func(i, j int) int {
        if i+1 == j { // 无需切割
            return 0
        }
        p := &memo[i][j]
        if *p > 0 { // 之前计算过
            return *p
        }
        res := math.MaxInt
        for k := i + 1; k < j; k++ {
            res = min(res, dfs(i, k)+dfs(k, j))
        }
        *p = res + cuts[j] - cuts[i] // 记忆化
        return *p
    }
    return dfs(0, m-1)
}
```

```js [sol-JavaScript]
var minCost = function(n, cuts) {
    cuts.push(0);
    cuts.push(n);
    cuts.sort((a, b) => a - b);

    const m = cuts.length;
    const memo = Array.from({ length: m }, () => Array(m));
    function dfs(i, j) {
        if (i + 1 === j) { // 无需切割
            return 0;
        }
        if (memo[i][j]) { // 之前计算过
            return memo[i][j];
        }
        let res = Infinity;
        for (let k = i + 1; k < j; k++) {
            res = Math.min(res, dfs(i, k) + dfs(k, j));
        }
        return memo[i][j] = res + cuts[j] - cuts[i];
    }
    return dfs(0, m - 1);
};
```

```rust [sol-Rust]
impl Solution {
    pub fn min_cost(n: i32, mut cuts: Vec<i32>) -> i32 {
        cuts.push(0);
        cuts.push(n);
        cuts.sort_unstable();

        fn dfs(i: usize, j: usize, cuts: &Vec<i32>, memo: &mut Vec<Vec<i32>>) -> i32 {
            if i + 1 == j { // 无需切割
                return 0;
            }
            if memo[i][j] > 0 { // 之前计算过
                return memo[i][j];
            }
            let mut res = i32::MAX;
            for k in i + 1..j {
                res = res.min(dfs(i, k, cuts, memo) + dfs(k, j, cuts, memo));
            }
            memo[i][j] = res + cuts[j] - cuts[i];
            memo[i][j]
        }
        let m = cuts.len();
        let mut memo = vec![vec![0; m]; m];
        dfs(0, m - 1, &cuts, &mut memo)
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(m^3)$，其中 $m$ 为 $\textit{cuts}$ 的长度。由于每个状态只会计算一次，动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题状态个数等于 $\mathcal{O}(m^2)$，单个状态的计算时间为 $\mathcal{O}(m)$，所以总的时间复杂度为 $\mathcal{O}(m^3)$。
- 空间复杂度：$\mathcal{O}(m^2)$。保存多少状态，就需要多少空间。

## 四、1:1 翻译成递推

我们可以去掉递归中的「递」，只保留「归」的部分，即自底向上计算。

具体来说，$f[i][j]$ 的定义和 $\textit{dfs}(i,j)$ 的定义是一样的，都表示切割一根左端点为 $\textit{cuts}[i]$，右端点为 $\textit{cuts}[j]$ 的棍子的最小成本。

相应的递推式（状态转移方程）也和 $\textit{dfs}$ 一样：

$$
f[i][j] = \min\limits_{k=i+1}^{j-1} f[i][k] + f[k][j] + \textit{cuts}[j] - \textit{cuts}[i]
$$

初始值 $f[i][i+1]=0$，翻译自递归边界 $\textit{dfs}(i,i+1)=0$。

答案为 $f[0][m-1]$，翻译自递归入口 $\textit{dfs}(0,m-1)$。

#### 答疑

**问**：如何思考循环顺序？什么时候要正序枚举，什么时候要倒序枚举？

**答**：这里有一个通用的做法：盯着状态转移方程，想一想，要计算 $f[i][j]$，必须先把 $f[k][j]$ 算出来，由于 $i<k$，那么只有 $i$ **从大到小**枚举才能做到。同理，必须先把同一行的 $f[i][k]$ 算出来，由于 $j>k$，那么只有 $j$ **从小到大**枚举才能做到。

```py [sol-Python3]
class Solution:
    def minCost(self, n: int, cuts: List[int]) -> int:
        cuts.sort()
        cuts = [0] + cuts + [n]

        m = len(cuts)
        f = [[0] * m for _ in range(m)]
        for i in range(m - 3, -1, -1):
            for j in range(i + 2, m):
                f[i][j] = min(f[i][k] + f[k][j] for k in range(i + 1, j)) + cuts[j] - cuts[i]
        return f[0][-1]
```

```java [sol-Java]
class Solution {
    public int minCost(int n, int[] cuts) {
        Arrays.sort(cuts);
        int m = cuts.length + 2;
        int[] newCuts = new int[m];
        System.arraycopy(cuts, 0, newCuts, 1, m - 2);
        newCuts[m - 1] = n;

        int[][] f = new int[m][m];
        for (int i = m - 3; i >= 0; i--) {
            for (int j = i + 2; j < m; j++) {
                int res = Integer.MAX_VALUE;
                for (int k = i + 1; k < j; k++) {
                    res = Math.min(res, f[i][k] + f[k][j]);
                }
                f[i][j] = res + newCuts[j] - newCuts[i];
            }
        }
        return f[0][m - 1];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minCost(int n, vector<int>& cuts) {
        cuts.push_back(0);
        cuts.push_back(n);
        ranges::sort(cuts);

        int m = cuts.size();
        vector<vector<int>> f(m, vector<int>(m));
        for (int i = m - 3; i >= 0; i--) {
            for (int j = i + 2; j < m; j++) {
                int res = INT_MAX;
                for (int k = i + 1; k < j; k++) {
                    res = min(res, f[i][k] + f[k][j]);
                }
                f[i][j] = res + cuts[j] - cuts[i];
            }
        }
        return f[0][m - 1];
    }
};
```

```go [sol-Go]
func minCost(n int, cuts []int) int {
    cuts = append(cuts, 0, n)
    slices.Sort(cuts)

    m := len(cuts)
    f := make([][]int, m)
    for i := range f {
        f[i] = make([]int, m)
    }
    for i := m - 3; i >= 0; i-- {
        for j := i + 2; j < m; j++ {
            res := math.MaxInt
            for k := i + 1; k < j; k++ {
                res = min(res, f[i][k]+f[k][j])
            }
            f[i][j] = res + cuts[j] - cuts[i]
        }
    }
    return f[0][m-1]
}
```

```js [sol-JavaScript]
var minCost = function(n, cuts) {
    cuts.push(0);
    cuts.push(n);
    cuts.sort((a, b) => a - b);

    const m = cuts.length;
    const f = Array.from({ length: m }, () => Array(m).fill(0));
    for (let i = m - 3; i >= 0; i--) {
        for (let j = i + 2; j < m; j++) {
            let res = Infinity;
            for (let k = i + 1; k < j; k++) {
                res = Math.min(res, f[i][k] + f[k][j]);
            }
            f[i][j] = res + cuts[j] - cuts[i];
        }
    }
    return f[0][m - 1];
};
```

```rust [sol-Rust]
impl Solution {
    pub fn min_cost(n: i32, mut cuts: Vec<i32>) -> i32 {
        cuts.push(0);
        cuts.push(n);
        cuts.sort_unstable();

        let m = cuts.len();
        let mut f = vec![vec![0; m]; m];
        for i in (0..m - 2).rev() {
            for j in i + 2..m {
                let mut res = i32::MAX;
                for k in i + 1..j {
                    res = res.min(f[i][k] + f[k][j]);
                }
                f[i][j] = res + cuts[j] - cuts[i];
            }
        }
        f[0][m - 1]
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(m^3)$，其中 $m$ 为 $\textit{cuts}$ 的长度。
- 空间复杂度：$\mathcal{O}(m^2)$。

更多相似题目，见 [动态规划题单](https://leetcode.cn/circle/discuss/tXLS3i/) 中的「**八、区间 DP**」。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与一般树（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/circle/discuss/K0n2gO/)
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

回到来源分类 `八、区间 DP / §8.2 区间 DP`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
