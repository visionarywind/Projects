# 123. 买卖股票的最佳时机 III

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/best-time-to-buy-and-sell-stock-iii/
- 题目 slug：`best-time-to-buy-and-sell-stock-iii`
- 来源专题：动态规划
- 来源分类路径：六、状态机 DP / §6.1 买卖股票
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/best-time-to-buy-and-sell-stock-iii/solutions/2974681/188-ti-k2-de-qing-kuang-pythonjavacgojsr-sg1z/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[188 题 k=2 的情况（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/best-time-to-buy-and-sell-stock-iii/solutions/2974681/188-ti-k2-de-qing-kuang-pythonjavacgojsr-sg1z/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`188-ti-k2-de-qing-kuang-pythonjavacgojsr-sg1z`
- topic id：`2974681`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

本题相当于 [188. 买卖股票的最佳时机 IV](https://leetcode.cn/problems/best-time-to-buy-and-sell-stock-iv/) 的 $k=2$ 的情况，做法是完全一样的，请看 [我的题解](https://leetcode.cn/problems/best-time-to-buy-and-sell-stock-iv/solutions/2201488/shi-pin-jiao-ni-yi-bu-bu-si-kao-dong-tai-kksg/)。

```py [sol-Python3]
class Solution:
    def maxProfit(self, prices: List[int]) -> int:
        k = 2
        f = [[-inf] * 2 for _ in range(k + 2)]
        for j in range(1, k + 2):
            f[j][0] = 0
        for p in prices:
            for j in range(k + 1, 0, -1):
                f[j][0] = max(f[j][0], f[j][1] + p)
                f[j][1] = max(f[j][1], f[j - 1][0] - p)
        return f[-1][0]
```

```java [sol-Java]
class Solution {
    public int maxProfit(int[] prices) {
        final int k = 2;
        int[][] f = new int[k + 2][2];
        for (int j = 1; j <= k + 1; j++) {
            f[j][1] = Integer.MIN_VALUE / 2; // 防止溢出
        }
        f[0][0] = Integer.MIN_VALUE / 2;
        for (int p : prices) {
            for (int j = k + 1; j > 0; j--) {
                f[j][0] = Math.max(f[j][0], f[j][1] + p);
                f[j][1] = Math.max(f[j][1], f[j - 1][0] - p);
            }
        }
        return f[k + 1][0];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxProfit(vector<int>& prices) {
        const int k = 2;
        vector<array<int, 2>> f(k + 2, {INT_MIN / 2, INT_MIN / 2});
        for (int j = 1; j <= k + 1; j++) {
            f[j][0] = 0;
        }
        for (int p : prices) {
            for (int j = k + 1; j > 0; j--) {
                f[j][0] = max(f[j][0], f[j][1] + p);
                f[j][1] = max(f[j][1], f[j - 1][0] - p);
            }
        }
        return f[k + 1][0];
    }
};
```

```go [sol-Go]
func maxProfit(prices []int) int {
    const k = 2
    f := [k + 2][2]int{}
    for j := 1; j <= k+1; j++ {
        f[j][1] = math.MinInt / 2 // 防止溢出
    }
    f[0][0] = math.MinInt / 2
    for _, p := range prices {
        for j := k + 1; j > 0; j-- {
            f[j][0] = max(f[j][0], f[j][1]+p)
            f[j][1] = max(f[j][1], f[j-1][0]-p)
        }
    }
    return f[k+1][0]
}
```

```js [sol-JavaScript]
var maxProfit = function(prices) {
    const k = 2;
    const f = Array.from({length: k + 2}, () => [-Infinity, -Infinity]);
    for (let j = 1; j <= k + 1; j++) {
        f[j][0] = 0;
    }
    for (const p of prices) {
        for (let j = k + 1; j > 0; j--) {
            f[j][0] = Math.max(f[j][0], f[j][1] + p);
            f[j][1] = Math.max(f[j][1], f[j - 1][0] - p);
        }
    }
    return f[k + 1][0];
};
```

```rust [sol-Rust]
impl Solution {
    pub fn max_profit(prices: Vec<i32>) -> i32 {
        const k: usize = 2;
        let mut f = vec![vec![0; 2]; k + 2];
        for j in 1..=k + 1 {
            f[j][1] = i32::MIN / 2; // 防止溢出
        }
        f[0][0] = i32::MIN / 2;
        for p in prices {
            for j in (1..=k + 1).rev() {
                f[j][0] = f[j][0].max(f[j][1] + p);
                f[j][1] = f[j][1].max(f[j - 1][0] - p);
            }
        }
        f[k + 1][0]
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{prices}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

## 股票买卖系列题目

- [121. 买卖股票的最佳时机](https://leetcode.cn/problems/best-time-to-buy-and-sell-stock/)
- [122. 买卖股票的最佳时机 II](https://leetcode.cn/problems/best-time-to-buy-and-sell-stock-ii/)
- [123. 买卖股票的最佳时机 III](https://leetcode.cn/problems/best-time-to-buy-and-sell-stock-iii/)
- [188. 买卖股票的最佳时机 IV](https://leetcode.cn/problems/best-time-to-buy-and-sell-stock-iv/)
- [309. 买卖股票的最佳时机含冷冻期](https://leetcode.cn/problems/best-time-to-buy-and-sell-stock-with-cooldown/)
- [714. 买卖股票的最佳时机含手续费](https://leetcode.cn/problems/best-time-to-buy-and-sell-stock-with-transaction-fee/)

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

## 股票买卖系列题目

- [121. 买卖股票的最佳时机](https://leetcode.cn/problems/best-time-to-buy-and-sell-stock/)
- [122. 买卖股票的最佳时机 II](https://leetcode.cn/problems/best-time-to-buy-and-sell-stock-ii/)
- [123. 买卖股票的最佳时机 III](https://leetcode.cn/problems/best-time-to-buy-and-sell-stock-iii/)
- [188. 买卖股票的最佳时机 IV](https://leetcode.cn/problems/best-time-to-buy-and-sell-stock-iv/)
- [309. 买卖股票的最佳时机含冷冻期](https://leetcode.cn/problems/best-time-to-buy-and-sell-stock-with-cooldown/)
- [714. 买卖股票的最佳时机含手续费](https://leetcode.cn/problems/best-time-to-buy-and-sell-stock-with-transaction-fee/)

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
