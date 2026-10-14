# 600. 不含连续 1 的非负整数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/non-negative-integers-without-consecutive-ones/
- 题目 slug：`non-negative-integers-without-consecutive-ones`
- 来源专题：动态规划
- 来源分类路径：十、数位 DP / §10.1 统计合法元素的数目
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/non-negative-integers-without-consecutive-ones/solutions/1750941/by-endlesscheng-1egu/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[数位 DP（Python/Java/C++/Go）](https://leetcode.cn/problems/non-negative-integers-without-consecutive-ones/solutions/1750941/by-endlesscheng-1egu/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`by-endlesscheng-1egu`
- topic id：`1750941`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

视频讲解：请看 [数位 DP 通用模板](https://www.bilibili.com/video/BV1rS4y1s721/?t=20m05s)，从 20:05 开始。

定义 $\textit{dfs}(i, \textit{pre}_1, \textit{isLimit}, \textit{isNum})$ 表示构造第 $i$ 位及其之后数位的合法方案数，其余参数的含义为：

- $\textit{pre}_1$ 表示前一个比特位是否为 $1$。如果是 $1$，则当前比特位不能填 $1$。
- $\textit{isLimit}$ 表示当前是否受到了 $n$ 的约束。若为真，则第 $i$ 位填入的数字至多为 `n >> i & 1`，否则可以是 $1$。设填入的数字至多为 $\textit{up}$，如果在 $\textit{isLimit}$ 为真的情况下填了 $\textit{up}$，那么后续填入的数字仍会受到 $n$ 的约束。
- $\textit{isNum}$ 表示 $i$ 前面的数位是否填了数字。若为假，则当前位可以跳过（不填数字），或者要填入的数字至少为 $1$；若为真，则要填入的数字可以从 $0$ 开始。本题前导零对答案无影响，$\textit{isNum}$ 可以省略。

代码实现时，由于 $(i,\textit{pre}_1,\texttt{true})$ 这种状态在整个递归过程中至多出现一次，没必要记忆化。或者说，我们只需要记忆化 $(i,\textit{pre}_1,\texttt{false})$ 这样的状态，也就是在 $\textit{isLimit}=\texttt{false}$ 时才去记忆化。

### 答疑

**问**：什么情况下必须要有 $\textit{isNum}$ 参数？

**答**：考虑这样一个问题，计算 $[0,n]$ 中，有多少个数 $x$ 满足：统计 $x$ 的每个数位，要求 $0,1,2,\ldots,9$ 的出现次数都是偶数。这里如果把前导零也统计进去的话，就会和 $x$ 中的 $0$ 混在一起了，没法判断 $x$ 中的 $0$ 是否出现了偶数次。

```py [sol-Python3]
class Solution:
    def findIntegers(self, n: int) -> int:
        @cache  # 缓存装饰器，避免重复计算 dfs 的结果（一行代码实现记忆化）
        def dfs(i: int, pre1: bool, is_limit: bool) -> int:
            if i < 0:
                return 1
            up = n >> i & 1 if is_limit else 1
            res = dfs(i - 1, False, is_limit and up == 0)  # 填 0
            if not pre1 and up == 1:  # 可以填 1
                res += dfs(i - 1, True, is_limit)  # 填 1
            return res
        return dfs(n.bit_length() - 1, False, True)  # 从高位到低位
```

```java [sol-Java]
class Solution {
    public int findIntegers(int n) {
        int m = Integer.SIZE - Integer.numberOfLeadingZeros(n);
        int[][] memo = new int[m][2];
        for (int[] row : memo) {
            Arrays.fill(row, -1); // -1 表示没有计算过
        }
        return dfs(m - 1, 0, true, n, memo); // 从高位到低位
    }

    // pre 表示前一个比特位填的数
    private int dfs(int i, int pre, boolean isLimit, int n, int[][] memo) {
        if (i < 0) {
            return 1;
        }
        if (!isLimit && memo[i][pre] >= 0) { // 之前计算过
            return memo[i][pre];
        }
        int up = isLimit ? n >> i & 1 : 1;
        int res = dfs(i - 1, 0, isLimit && up == 0, n, memo); // 填 0
        if (pre == 0 && up == 1) { // 可以填 1
            res += dfs(i - 1, 1, isLimit, n, memo); // 填 1
        }
        if (!isLimit) {
            memo[i][pre] = res; // 记忆化
        }
        return res;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int findIntegers(int n) {
        int m = bit_width((unsigned) n) - 1; // n 的最高位
        vector<array<int, 2>> memo(m + 1, {-1, -1}); // -1 表示没有计算过
        auto dfs = [&](this auto&& dfs, int i, bool pre1, bool is_limit) -> int {
            if (i < 0) {
                return 1;
            }
            if (!is_limit && memo[i][pre1] >= 0) { // 之前计算过
                return memo[i][pre1];
            }
            int up = is_limit ? n >> i & 1 : 1;
            int res = dfs(i - 1, false, is_limit && up == 0); // 填 0
            if (!pre1 && up == 1) { // 可以填 1
                res += dfs(i - 1, true, is_limit); // 填 1
            }
            if (!is_limit) {
                memo[i][pre1] = res; // 记忆化
            }
            return res;
        };
        return dfs(m, false, true); // 从高位到低位
    }
};
```

```go [sol-Go]
func findIntegers(n int) int {
    m := bits.Len(uint(n))
    memo := make([][2]int, m)
    for i := range memo {
        memo[i] = [2]int{-1, -1} // -1 表示没有计算过
    }

    // pre 表示前一个比特位填的数
    var dfs func(int, int, bool) int
    dfs = func(i, pre int, isLimit bool) (res int) {
        if i < 0 {
            return 1
        }
        if !isLimit {
            p := &memo[i][pre]
            if *p >= 0 { // 之前计算过
                return *p
            }
            defer func() { *p = res }() // 记忆化
        }
        up := 1
        if isLimit {
            up = n >> i & 1
        }
        res = dfs(i-1, 0, isLimit && up == 0) // 填 0
        if pre == 0 && up == 1 { // 可以填 1
            res += dfs(i-1, 1, isLimit) // 填 1
        }
        return
    }
    return dfs(m-1, 0, true) // 从高位到低位
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(\log n)$。由于每个状态只会计算一次，动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题状态个数等于 $\mathcal{O}(\log n)$，单个状态的计算时间为 $\mathcal{O}(1)$，所以动态规划的时间复杂度为 $\mathcal{O}(\log n)$。
- 空间复杂度：$\mathcal{O}(\log n)$。保存多少状态，就需要多少空间。

更多相似题目，见下面 DP 题单中的「**数位 DP**」。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. 【本题相关】[动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)
