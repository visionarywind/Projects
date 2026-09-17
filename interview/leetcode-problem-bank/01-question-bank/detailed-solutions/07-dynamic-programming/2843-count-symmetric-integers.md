# 2843. 统计对称整数的数目

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/count-symmetric-integers/
- 题目 slug：`count-symmetric-integers`
- 来源专题：动态规划
- 来源分类路径：十、数位 DP / §10.1 统计合法元素的数目
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/count-symmetric-integers/solutions/2424088/mei-ju-by-endlesscheng-oo2d/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：枚举 / 数位 DP（Python/Java/C++/Go）](https://leetcode.cn/problems/count-symmetric-integers/solutions/2424088/mei-ju-by-endlesscheng-oo2d/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`mei-ju-by-endlesscheng-oo2d`
- topic id：`2424088`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 16:46:02 +0800

## 方法一：枚举

枚举 $[\textit{low},\textit{high}]$ 中的整数 $i$。

设 $i$ 的十进制字符串为 $s$。如果 $s$ 的长度为偶数，且 $s$ 左半字符 ASCII 值之和等于 $s$ 右半字符 ASCII 值之和，那么 $i$ 是一个对称整数，答案加一。

```py [sol-Python3]
class Solution:
    def countSymmetricIntegers(self, low: int, high: int) -> int:
        ans = 0
        for i in range(low, high + 1):
            s = str(i)
            n = len(s)
            if n % 2 == 0 and sum(map(ord, s[:n // 2])) == sum(map(ord, s[n // 2:])):
                ans += 1
        return ans
```

```java [sol-Java]
class Solution {
    public int countSymmetricIntegers(int low, int high) {
        int ans = 0;
        for (int i = low; i <= high; i++) {
            char[] s = Integer.toString(i).toCharArray();
            int n = s.length;
            if (n % 2 > 0) {
                continue;
            }
            int diff = 0;
            for (int j = 0; j < n / 2; j++) {
                diff += s[j];
            }
            for (int j = n / 2; j < n; j++) {
                diff -= s[j];
            }
            if (diff == 0) {
                ans++;
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int countSymmetricIntegers(int low, int high) {
        int ans = 0;
        for (int i = low; i <= high; i++) {
            string s = to_string(i);
            int n = s.size();
            if (n % 2 == 0 && reduce(s.begin(), s.begin() + n / 2) == reduce(s.begin() + n / 2, s.end())) {
                ans++;
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func countSymmetricIntegers(low, high int) (ans int) {
    for i := low; i <= high; i++ {
        s := strconv.Itoa(i)
        n := len(s)
        if n%2 > 0 {
            continue
        }
        diff := 0
        for _, c := range s[:n/2] {
            diff += int(c)
        }
        for _, c := range s[n/2:] {
            diff -= int(c)
        }
        if diff == 0 {
            ans++
        }
    }
    return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}((\textit{high} - \textit{low})\log \textit{high})$。
- 空间复杂度：$\mathcal{O}(\log \textit{high})$。

## 方法二：上下界数位 DP

如果 $\textit{high} = 10^{18}$，方法一就超时了，怎么办？

**前置知识**：

[数位 DP v1.0 视频讲解](https://www.bilibili.com/video/BV1rS4y1s721/)

[数位 DP v2.0 视频讲解](https://www.bilibili.com/video/BV1Fg4y1Q7wv/)（上下界数位 DP）

**状态定义**：$\textit{dfs}(i, \textit{start}, \textit{diff},\textit{limitLow},\textit{limitHigh})$ 表示构造第 $i$ 位及其之后数位的合法方案数，其余参数的含义为：

- $\textit{start}$ 表示从左往右第一个非零数字的下标。用于判断现在处于对称整数的左半还是右半。
- $\textit{diff}$ 表示左半数字与右半数字之差。如果最终 $\textit{diff}=0$，则说明我们成功构造出一个对称整数。
- $\textit{limitHigh}$ 表示当前是否受到了 $\textit{high}$ 的约束（我们要构造的数字不能超过 $\textit{high}$）。若为真，则第 $i$ 位填入的数字至多为 $\textit{high}[i]$，否则至多为 $9$，这个数记作 $\textit{hi}$。如果在受到约束的情况下填了 $\textit{hi}$，那么后续填入的数字仍会受到 $\textit{high}$ 的约束。例如 $\textit{high}=123$，那么 $i=0$ 填的是 $1$ 的话，$i=1$ 的这一位至多填 $2$。
- $\textit{limitLow}$ 表示当前是否受到了 $\textit{low}$ 的约束（我们要构造的数字不能低于 $\textit{low}$）。若为真，则第 $i$ 位填入的数字至少为 $\textit{low}[i]$，否则至少为 $0$，这个数记作 $\textit{lo}$。如果在受到约束的情况下填了 $\textit{lo}$，那么后续填入的数字仍会受到 $\textit{low}$ 的约束。

**状态转移**：

- 如果前面没有填数字，且剩余数位个数是奇数，那么当前数位不能填数字（因为对称整数的长度必须是偶数），往后递归。在这种情况下，如果 $\textit{lo}>0$，那么必须填数字，但这不合法，直接返回 $0$。
- 否则，枚举第 $i$ 位填数字 $d=\textit{lo},\textit{lo}+1,\ldots,\textit{hi}$。如果之前没有填过数字，且当前填的数字大于 $0$，那么记录 $\textit{start}=i$。如果 $i< \dfrac{\textit{start}+n}{2}$，说明我们在左半，把 $\textit{diff}$ 增加 $d$，否则把 $\textit{diff}$ 减少 $d$。

**递归终点**：

- 如果 $\textit{diff}<0$，说明我们当前在右半，且右半元素和已经大于左半元素和了，一定不合法，返回 $0$。
- $i=n$ 时，如果 $\textit{diff}=0$，说明我们成功构造出一个对称整数，返回 $1$，否则返回 $0$。

**递归入口**：$\textit{dfs}(0, -1, 0, \texttt{true}, \texttt{true})$，表示：

- 从最高位开始。
- 一开始没有填任何数字。
- 一开始要受到 $\textit{low}$ 和 $\textit{high}$ 的约束（否则就可以随意填了，这肯定不行）。

```py [sol-Python3]
class Solution:
    def countSymmetricIntegers(self, low: int, high: int) -> int:
        high = list(map(int, str(high)))  # 避免在 dfs 中频繁调用 int()
        n = len(high)
        low = list(map(int, str(low).zfill(n)))  # 补前导零，和 high 对齐

        @cache
        def dfs(i: int, start: int, diff: int, limit_low: bool, limit_high: bool) -> int:
            if diff < 0:
                return 0
            if i == n:
                return 1 if diff == 0 else 0

            lo = low[i] if limit_low else 0
            hi = high[i] if limit_high else 9

            # 如果前面没有填数字，且剩余数位个数是奇数，那么当前数位不能填数字
            if start < 0 and (n - i) % 2:
                # 如果必须填数字（lo > 0），不合法，返回 0
                return 0 if lo else dfs(i + 1, start, diff, True, False)

            res = 0
            is_left = start < 0 or i < (start + n) // 2
            for d in range(lo, hi + 1):
                res += dfs(i + 1,
                           i if start < 0 and d else start,  # 记录第一个填数字的位置
                           diff + (d if is_left else -d),  # 左半 + 右半 -
                           limit_low and d == lo,
                           limit_high and d == hi)
            return res

        return dfs(0, -1, 0, True, True)
```

```java [sol-Java]
class Solution {
    private char[] lowS, highS;
    private int n, m, diffLh;
    private int[][][] memo;

    public int countSymmetricIntegers(int low, int high) {
        lowS = String.valueOf(low).toCharArray();
        highS = String.valueOf(high).toCharArray();
        n = highS.length;
        diffLh = n - lowS.length;

        // dfs 中的 start <= diffLh，左半元素和 <= floor(n/2) * 9
        memo = new int[n][diffLh + 1][n / 2 * 9 + 1];
        for (int[][] mat : memo) {
            for (int[] row : mat) {
                Arrays.fill(row, -1);
            }
        }
        return dfs(0, -1, 0, true, true);
    }

    private int dfs(int i, int start, int diff, boolean limitLow, boolean limitHigh) {
        if (diff < 0) {
            return 0;
        }
        if (i == n) {
            return diff == 0 ? 1 : 0;
        }

        // start 当 isNum 用
        if (start != -1 && !limitLow && !limitHigh && memo[i][start][diff] != -1) {
            return memo[i][start][diff];
        }

        int lo = limitLow && i >= diffLh ? lowS[i - diffLh] - '0' : 0;
        int hi = limitHigh ? highS[i] - '0' : 9;

        // 如果前面没有填数字，且剩余数位个数是奇数，那么当前数位不能填数字
        if (start < 0 && (n - i) % 2 > 0) {
            // 如果必须填数字（lo > 0），不合法，返回 0
            return lo > 0 ? 0 : dfs(i + 1, start, diff, true, false);
        }

        int res = 0;
        boolean isLeft = start < 0 || i < (start + n) / 2;
        for (int d = lo; d <= hi; d++) {
            res += dfs(i + 1,
                       start < 0 && d > 0 ? i : start, // 记录第一个填数字的位置
                       diff + (isLeft ? d : -d), // 左半 +，右半 -
                       limitLow && d == lo,
                       limitHigh && d == hi);
        }

        if (start != -1 && !limitLow && !limitHigh) {
            memo[i][start][diff] = res;
        }
        return res;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int countSymmetricIntegers(int low, int high) {
        string low_s = to_string(low), high_s = to_string(high);
        int n = high_s.size();
        int diff_lh = n - low_s.size();

        // dfs 中的 start <= diff_lh，左半元素和 <= floor(n/2) * 9
        vector memo(n, vector(diff_lh + 1, vector<int>(n / 2 * 9 + 1, -1)));
        auto dfs = [&](this auto&& dfs, int i, int start, int diff, bool limit_low, bool limit_high) -> int {
            if (diff < 0) {
                return 0;
            }
            if (i == n) {
                return diff == 0;
            }

            // start 当 is_num 用
            if (start != -1 && !limit_low && !limit_high && memo[i][start][diff] != -1) {
                return memo[i][start][diff];
            }

            int lo = limit_low && i >= diff_lh ? low_s[i - diff_lh] - '0' : 0;
            int hi = limit_high ? high_s[i] - '0' : 9;

            // 如果前面没有填数字，且剩余数位个数是奇数，那么当前数位不能填数字
            if (start < 0 && (n - i) % 2) {
                // 如果必须填数字（lo > 0），不合法，返回 0
                return lo > 0 ? 0 : dfs(i + 1, start, diff, true, false);
            }

            int res = 0;
            bool is_left = start < 0 || i < (start + n) / 2;
            for (int d = lo; d <= hi; d++) {
                res += dfs(i + 1,
                           start < 0 && d > 0 ? i : start, // 记录第一个填数字的位置
                           diff + (is_left ? d : -d), // 左半 +，右半 -
                           limit_low && d == lo,
                           limit_high && d == hi);
            }

            if (start != -1 && !limit_low && !limit_high) {
                memo[i][start][diff] = res;
            }
            return res;
        };
        return dfs(0, -1, 0, true, true);
    }
};
```

```go [sol-Go]
func countSymmetricIntegers(low, high int) int {
    lowS := strconv.Itoa(low)
    highS := strconv.Itoa(high)
    n := len(highS)
    diffLH := n - len(lowS)

    memo := make([][][]int, n)
    for i := range memo {
        memo[i] = make([][]int, diffLH+1) // start <= diffLH
        for j := range memo[i] {
            memo[i][j] = make([]int, n/2*9+1) // 左半元素和 <= floor(n/2) * 9
            for k := range memo[i][j] {
                memo[i][j][k] = -1
            }
        }
    }
    var dfs func(int, int, int, bool, bool) int
    dfs = func(i, start, diff int, limitLow, limitHigh bool) (res int) {
        if diff < 0 {
            return
        }
        if i == n {
            if diff == 0 {
                return 1
            }
            return
        }

        // start 当 isNum 用
        if start != -1 && !limitLow && !limitHigh {
            p := &memo[i][start][diff]
            if *p != -1 {
                return *p
            }
            defer func() { *p = res }()
        }

        lo := 0
        if limitLow && i >= diffLH {
            lo = int(lowS[i-diffLH] - '0')
        }
        hi := 9
        if limitHigh {
            hi = int(highS[i] - '0')
        }

        // 如果前面没有填数字，且剩余数位个数是奇数，那么当前数位不能填数字
        if start < 0 && (n-i)%2 > 0 {
            if lo > 0 {
                return 0 // 必须填数字但 lo > 0，不合法
            }
            return dfs(i+1, start, diff, true, false)
        }

        isLeft := start < 0 || i < (start+n)/2
        for d := lo; d <= hi; d++ {
            newStart := start
            if start < 0 && d > 0 {
                newStart = i // 记录第一个填数字的位置
            }
            newDiff := diff
            if isLeft {
                newDiff += d
            } else {
                newDiff -= d
            }
            res += dfs(i+1, newStart, newDiff, limitLow && d == lo, limitHigh && d == hi)
        }
        return
    }
    return dfs(0, -1, 0, true, true)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(D^2n^3)$，其中 $D=10$，$n=\mathcal{O}(\log \textit{high})$。由于每个状态只会计算一次，动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题 $i$ 有 $\mathcal{O}(n)$ 个，$\textit{start}$ 有 $\mathcal{O}(n)$ 个，$\textit{diff}$ 有 $\mathcal{O}(Dn)$ 个，所以状态个数为 $\mathcal{O}(Dn^3)$，单个状态的计算时间为 $\mathcal{O}(D)$，所以总的时间复杂度为 $\mathcal{O}(D^2n^3)$。
- 空间复杂度：$\mathcal{O}(Dn^3)$。保存多少状态，就需要多少空间。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/circle/discuss/0viNMK/)
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

## 本地原创解析

### 1. 题意重述

本题来自 `十、数位 DP / §10.1 统计合法元素的数目`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `十、数位 DP / §10.1 统计合法元素的数目`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
