# 624. 数组列表中的最大距离

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-distance-in-arrays/
- 题目 slug：`maximum-distance-in-arrays`
- 来源专题：贪心与思维
- 来源分类路径：一、贪心策略 / §1.1 从最小/最大开始贪心
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/maximum-distance-in-arrays/solutions/3067679/mei-ju-you-wei-hu-zuo-pythonjavaccgojsru-wtgb/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[枚举右，维护左（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/maximum-distance-in-arrays/solutions/3067679/mei-ju-you-wei-hu-zuo-pythonjavaccgojsru-wtgb/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`mei-ju-you-wei-hu-zuo-pythonjavaccgojsru-wtgb`
- topic id：`3067679`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:46:10 +0800

**题意**：选择两个不同的数组 $A$ 和 $B$，然后从 $A$ 中选一个数 $x$，$B$ 中选一个数 $y$，最大化 $|x-y|$。

如果固定 $A$ 和 $B$，那么 $|x-y|$ 最大是多少？

- 如果 $x\ge y$，那么 $|x-y|=x-y$。贪心地想，$x$ 越大越好，$y$ 越小越好。所以 $x$ 应该取 $\max(A)$，$y$ 应该取 $\min(B)$。
- 如果 $x< y$，那么 $|x-y|=y-x$。贪心地想，$x$ 越小越好，$y$ 越大越好。所以 $x$ 应该取 $\min(A)$，$y$ 应该取 $\max(B)$。

所以我们**只需考虑每个数组的最小值和最大值**。由于题目保证数组是升序的，所以数组第一个数就是最小值，最后一个数就是最大值。

既然要选两个数组，我们可以枚举 $A=\textit{arrays}[i]$ 作为右边那个数组，然后维护左边所选数组的第一个数的最小值 $\textit{mn}$ 和最后一个数的最大值 $\textit{mx}$。

数组 $A$ 和左边的数组各选一个数，能形成的最大差值为

$$
\max(|A[n-1]-\textit{mn}|, |\textit{mx} - A[0]|)
$$

其中 $n$ 为 $A$ 的长度。用上式更新答案的最大值。

## 优化

实际上，不需要算绝对值，只需要计算

$$
\max(A[n-1]-\textit{mn}, \textit{mx} - A[0])
$$

如果出现 $A[n-1]-\textit{mn} < 0$，也就是 $A[n-1] < \textit{mn}$ 的情况，那么有

$$
A[0] \le A[n-1] < \textit{mn} \le \textit{mx}
$$

所以有 $\textit{mx} - A[0]\ge \textit{mn} - A[n-1] =  |A[n-1]-\textit{mn}| > 0$。我们既不需要算绝对值，又保证了 $\textit{mx} - A[0]$ 大于或等于 $|A[n-1]-\textit{mn}|$。

对于 $\textit{mx} - A[0] < 0$ 的情况同理。

## 答疑

**问**：为什么要先更新答案 $\textit{ans}$，再更新 $\textit{mn}$ 和 $\textit{mx}$？

**答**：看代码，我们先更新 $\textit{ans}$，此时 $\textit{mn}$ 和 $\textit{mx}$ 还未涉及到当前数组 $a$ 中的元素。如果反过来，先更新 $\textit{mn}$ 和 $\textit{mx}$，可能会出现 $\textit{mx}-a[0]$ 中的 $\textit{mx}$ 是 $a$ 的最后一个元素的情况，这意味着我们在数组 $a$ 中选了两个数，不符合题目要求。

**问**：为什么代码没有考虑当前数组与**右边**的数组呢？

**答**：无需考虑。比如当前遍历到数组 $a$，右边还有个数组 $b$，那么后面遍历到数组 $b$ 的时候，我们会站在 $b$ 的视角，计算 $b$ 和 $a$ 之间怎么选数字，并不会漏算。

```py [sol-Python3]
class Solution:
    def maxDistance(self, arrays: List[List[int]]) -> int:
        ans = 0
        mn, mx = inf, -inf
        for a in arrays:
            ans = max(ans, a[-1] - mn, mx - a[0])
            mn = min(mn, a[0])
            mx = max(mx, a[-1])
        return ans
```

```java [sol-Java]
class Solution {
    public int maxDistance(List<List<Integer>> arrays) {
        int ans = 0;
        int mn = Integer.MAX_VALUE / 2; // 防止减法溢出
        int mx = Integer.MIN_VALUE / 2;
        for (List<Integer> a : arrays) {
            int x = a.get(0);
            int y = a.get(a.size() - 1);
            ans = Math.max(ans, Math.max(y - mn, mx - x));
            mn = Math.min(mn, x);
            mx = Math.max(mx, y);
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxDistance(vector<vector<int>>& arrays) {
        int ans = 0;
        int mn = INT_MAX / 2, mx = INT_MIN / 2; // 防止减法溢出
        for (auto& a : arrays) {
            ans = max({ans, a.back() - mn, mx - a[0]});
            mn = min(mn, a[0]);
            mx = max(mx, a.back());
        }
        return ans;
    }
};
```

```c [sol-C]
#define MIN(a, b) ((b) < (a) ? (b) : (a))
#define MAX(a, b) ((b) > (a) ? (b) : (a))

int maxDistance(int** arrays, int arraysSize, int* arraysColSize) {
    int ans = 0;
    int mn = INT_MAX / 2, mx = INT_MIN / 2; // 防止减法溢出
    for (int i = 0; i < arraysSize; i++) {
        int x = arrays[i][0], y = arrays[i][arraysColSize[i] - 1];
        ans = MAX(ans, MAX(y - mn, mx - x));
        mn = MIN(mn, x);
        mx = MAX(mx, y);
    }
    return ans;
}
```

```go [sol-Go]
func maxDistance(arrays [][]int) (ans int) {
    mn, mx := math.MaxInt/2, math.MinInt/2 // 防止减法溢出
    for _, a := range arrays {
        x, y := a[0], a[len(a)-1]
        ans = max(ans, y-mn, mx-x)
        mn = min(mn, x)
        mx = max(mx, y)
    }
    return
}
```

```js [sol-JavaScript]
var maxDistance = function(arrays) {
    let ans = 0;
    let mn = Infinity, mx = -Infinity;
    for (const a of arrays) {
        ans = Math.max(ans, a[a.length - 1] - mn, mx - a[0]);
        mn = Math.min(mn, a[0]);
        mx = Math.max(mx, a[a.length - 1]);
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn max_distance(arrays: Vec<Vec<i32>>) -> i32 {
        let mut ans = 0;
        let mut mn = i32::MAX / 2; // 防止减法溢出
        let mut mx = i32::MIN / 2;
        for a in arrays {
            let x = a[0];
            let y = a[a.len() - 1];
            ans = ans.max(y - mn).max(mx - x);
            mn = mn.min(x);
            mx = mx.max(y);
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(m)$，其中 $m$ 是 $\textit{arrays}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

## 专题训练

见下面数据结构题单的「**§0.1 枚举右，维护左**」。

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
