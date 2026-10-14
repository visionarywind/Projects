# 1014. 最佳观光组合

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/best-sightseeing-pair/
- 题目 slug：`best-sightseeing-pair`
- 来源专题：常用数据结构
- 来源分类路径：零、常用枚举技巧 / §0.1 枚举右，维护左 / §0.1.1 基础
- 难度分：1730
- 外部题解来源：https://leetcode.cn/problems/best-sightseeing-pair/solutions/2836414/mei-ju-you-wei-hu-zuo-jian-ji-xie-fa-pyt-x385/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[枚举右，维护左，简洁写法（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/best-sightseeing-pair/solutions/2836414/mei-ju-you-wei-hu-zuo-jian-ji-xie-fa-pyt-x385/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`mei-ju-you-wei-hu-zuo-jian-ji-xie-fa-pyt-x385`
- topic id：`2836414`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

下文把 $\textit{values}$ 简记为 $v$。

得分

$$
v_i + v_j + i - j
$$

可以变形为

$$
(v_i + i) + (v_j - j)
$$

我们可以枚举 $j$，同时维护在 $j$ 左边的 $v_i + i$ 的最大值 $\textit{mx}$，用 $\textit{mx} + v_j - j$ 更新答案的最大值。

示例 1 的计算过程如下：

|  $j$ | $v_j$ | $v_j + j$  |  $\textit{mx}$ | $v_j - j$ | $\textit{mx} + v_j - j$  |
|---|---|---|---|---|---|
| $0$ | $8$ |  $8$ |  $-\infty$ | $8$ |  $-\infty$  |
| $1$ | $1$ |  $2$ |  $8$ |  $0$ | $8$  |
| $2$ | $5$ |  $7$ |  $8$ |  $3$ | $11$  |
| $3$ | $2$ |  $5$ |  $8$ |  $-1$ | $7$  |
| $4$ | $6$ |  $10$ |  $8$ | $2$ |  $10$  |

$j=2$ 时可以计算出最大的得分 $11$。

注意 $j=0$ 时，由于 $j$ 左边没有数，所以 $\textit{mx}$ 初始化成 $-\infty$。

不过，把 $\textit{mx}$ 初始化成 $0$ 也是可以的。由于题目保证数组中至少有两个数且 $v_i \ge 1$，所以答案至少为 $v_0 + v_1 + 0 - 1 \ge v_0$。把 $\textit{mx}$ 初始化成 $0$ 会导致答案一开始更新为 $v_0$，由于我们不会计算出比 $v_0$ 还小的答案，所以把 $\textit{mx}$ 初始化成 $0$ 在本题数据范围下是正确的。

也可以把 $\textit{mx}$ 初始化成 $v_0 + 0 = v_0$，然后从 $j=1$ 开始遍历数组。

⚠**注意**：要先更新答案，再更新 $\textit{mx}$。这可以保证更新答案时 $\textit{mx}$ 表示的是 $j$ **左边**的 $v_i + i$ 的最大值，$v_j + j$ 尚未更新到 $\textit{mx}$ 中。

```py [sol-Python3]
class Solution:
    def maxScoreSightseeingPair(self, values: List[int]) -> int:
        ans = mx = 0  # mx 表示 j 左边的 values[i] + i 的最大值
        for j, v in enumerate(values):
            ans = max(ans, mx + v - j)
            mx = max(mx, v + j)
        return ans
```

```java [sol-Java]
class Solution {
    public int maxScoreSightseeingPair(int[] values) {
        int ans = 0;
        int mx = values[0]; // j 左边的 values[i] + i 的最大值
        for (int j = 1; j < values.length; j++) {
            ans = Math.max(ans, mx + values[j] - j);
            mx = Math.max(mx, values[j] + j);
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxScoreSightseeingPair(vector<int>& values) {
        int ans = 0, mx = values[0]; // mx 表示 j 左边的 values[i] + i 的最大值
        for (int j = 1; j < values.size(); j++) {
            ans = max(ans, mx + values[j] - j);
            mx = max(mx, values[j] + j);
        }
        return ans;
    }
};
```

```c [sol-C]
#define MAX(a, b) ((b) > (a) ? (b) : (a))

int maxScoreSightseeingPair(int* values, int valuesSize) {
    int ans = 0, mx = values[0]; // mx 表示 j 左边的 values[i] + i 的最大值
    for (int j = 1; j < valuesSize; j++) {
        ans = MAX(ans, mx + values[j] - j);
        mx = MAX(mx, values[j] + j);
    }
    return ans;
}
```

```go [sol-Go]
func maxScoreSightseeingPair(values []int) (ans int) {
    mx := 0 // j 左边的 values[i] + i 的最大值
    for j, v := range values {
        ans = max(ans, mx+v-j)
        mx = max(mx, v+j)
    }
    return
}
```

```js [sol-JavaScript]
var maxScoreSightseeingPair = function(values) {
    let ans = 0, mx = values[0]; // mx 表示 j 左边的 values[i] + i 的最大值
    for (let j = 1; j < values.length; j++) {
        ans = Math.max(ans, mx + values[j] - j);
        mx = Math.max(mx, values[j] + j);
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn max_score_sightseeing_pair(values: Vec<i32>) -> i32 {
        let mut ans = 0;
        let mut mx = 0; // j 左边的 values[i] + i 的最大值
        for (j, &v) in values.iter().enumerate() {
            ans = ans.max(mx + v - j as i32);
            mx = mx.max(v + j as i32);
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{values}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

## 思考题

所有 $v_i + v_j + i - j$ 之和是多少？其中 $i<j$。

欢迎在评论区分享你的思路/代码。

## 相似题目

见 [数据结构题单](https://leetcode.cn/circle/discuss/mOr1u6/) 的第零章：**枚举右，维护左**。

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

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)
