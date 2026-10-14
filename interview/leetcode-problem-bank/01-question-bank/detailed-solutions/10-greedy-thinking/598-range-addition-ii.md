# 598. 区间加法 II

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/range-addition-ii/
- 题目 slug：`range-addition-ii`
- 来源专题：贪心与思维
- 来源分类路径：五、思维题 / §5.2 脑筋急转弯
- 难度分：1400
- 外部题解来源：https://leetcode.cn/problems/range-addition-ii/solutions/3053429/nao-jin-ji-zhuan-wan-pythonjavaccgojsrus-7166/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[脑筋急转弯（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/range-addition-ii/solutions/3053429/nao-jin-ji-zhuan-wan-pythonjavaccgojsrus-7166/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`nao-jin-ji-zhuan-wan-pythonjavaccgojsrus-7166`
- topic id：`3053429`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:07:12 +0800

每个 $\textit{ops}[i]$ 会把一个子矩形中的数都加一，那么这些子矩形的**交集**中的数，被加一的次数最多，是最终矩阵中的最大整数。

矩形的交集仍然是矩形。

由于子矩形的左上角都是 $(0,0)$，所以交集矩形的左上角也是 $(0,0)$。

知道了交集矩形的右下角，就知道了交集的大小。

所有 $\textit{ops}[i][0]$ 的最小值 $\textit{minA}$，是交集的横坐标加一。

所有 $\textit{ops}[i][1]$ 的最小值 $\textit{minB}$，是交集的纵坐标加一。

那么交集的大小为

$$
\textit{minA}\cdot \textit{minB}
$$

如果 $\textit{ops}$ 是空的，那么交集大小就是原矩阵的大小 $mn$。

```py [sol-Python3]
class Solution:
    def maxCount(self, m: int, n: int, ops: List[List[int]]) -> int:
        min_a = min((op[0] for op in ops), default=m)
        min_b = min((op[1] for op in ops), default=n)
        return min_a * min_b
```

```java [sol-Java]
class Solution {
    public int maxCount(int m, int n, int[][] ops) {
        int minA = m;
        int minB = n;
        for (int[] op : ops) {
            minA = Math.min(minA, op[0]);
            minB = Math.min(minB, op[1]);
        }
        return minA * minB;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxCount(int m, int n, vector<vector<int>>& ops) {
        int min_a = m, min_b = n;
        for (auto& op : ops) {
            min_a = min(min_a, op[0]);
            min_b = min(min_b, op[1]);
        }
        return min_a * min_b;
    }
};
```

```c [sol-C]
#define MIN(a, b) ((b) < (a) ? (b) : (a))

int maxCount(int m, int n, int** ops, int opsSize, int* opsColSize) {
    int min_a = m, min_b = n;
    for (int i = 0; i < opsSize; i++) {
        min_a = MIN(min_a, ops[i][0]);
        min_b = MIN(min_b, ops[i][1]);
    }
    return min_a * min_b;
}
```

```go [sol-Go]
func maxCount(m, n int, ops [][]int) int {
    minA, minB := m, n
    for _, op := range ops {
        minA = min(minA, op[0])
        minB = min(minB, op[1])
    }
    return minA * minB
}
```

```js [sol-JavaScript]
var maxCount = function(m, n, ops) {
    let minA = m, minB = n;
    for (const [a, b] of ops) {
        minA = Math.min(minA, a);
        minB = Math.min(minB, b);
    }
    return minA * minB;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn max_count(m: i32, n: i32, ops: Vec<Vec<i32>>) -> i32 {
        let mut min_a = m;
        let mut min_b = n;
        for op in ops {
            min_a = min_a.min(op[0]);
            min_b = min_b.min(op[1]);
        }
        min_a * min_b
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(k)$，其中 $k$ 是 $\textit{ops}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

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
