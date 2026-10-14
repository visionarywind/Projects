# 1738. 找出第 K 大的异或坐标值

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/find-kth-largest-xor-coordinate-value/
- 题目 slug：`find-kth-largest-xor-coordinate-value`
- 来源专题：常用数据结构
- 来源分类路径：一、前缀和 / §1.6 二维前缀和
- 难度分：1671
- 外部题解来源：https://leetcode.cn/problems/find-kth-largest-xor-coordinate-value/solutions/2790359/liang-chong-fang-fa-er-wei-qian-zhui-yi-689bf/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：二维前缀异或和/列前缀异或和（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/find-kth-largest-xor-coordinate-value/solutions/2790359/liang-chong-fang-fa-er-wei-qian-zhui-yi-689bf/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`liang-chong-fang-fa-er-wei-qian-zhui-yi-689bf`
- topic id：`2790359`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 方法一：二维前缀异或和

类似二维前缀和，定义 $s[i+1][j+1]$ 表示左上角在 $(0,0)$，右下角在 $(i,j)$ 的子矩阵异或和，按照[【图解】一张图秒懂二维前缀和](https://leetcode.cn/problems/range-sum-query-2d-immutable/solution/tu-jie-yi-zhang-tu-miao-dong-er-wei-qian-84qp/) 中的做法，有

$$
s[i+1][j+1] = s[i+1][j] \oplus s[i][j+1] \oplus s[i][j] \oplus \textit{matrix}[i][j]
$$

由于一个数异或自己等于 $0$，$s[i+1][j] \oplus s[i][j+1]$ 会导致 $s[i][j]$ 这部分被抵消掉，所以要异或进来。

```py [sol-Python3]
class Solution:
    def kthLargestValue(self, matrix: List[List[int]], k: int) -> int:
        m, n = len(matrix), len(matrix[0])
        s = [[0] * (n + 1) for _ in range(m + 1)]
        for i, row in enumerate(matrix):
            for j, x in enumerate(row):
                s[i + 1][j + 1] = s[i + 1][j] ^ s[i][j + 1] ^ s[i][j] ^ x
        return sorted(x for row in s[1:] for x in row[1:])[-k]
```

```java [sol-Java]
class Solution {
    public int kthLargestValue(int[][] matrix, int k) {
        int m = matrix.length;
        int n = matrix[0].length;
        int[] a = new int[m * n];
        int[][] s = new int[m + 1][n + 1];
        int idx = 0;
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                s[i + 1][j + 1] = s[i + 1][j] ^ s[i][j + 1] ^ s[i][j] ^ matrix[i][j];
                a[idx++] = s[i + 1][j + 1];
            }
        }
        Arrays.sort(a);
        return a[idx - k];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int kthLargestValue(vector<vector<int>>& matrix, int k) {
        int m = matrix.size(), n = matrix[0].size();
        vector<int> a;
        vector<vector<int>> s(m + 1, vector<int>(n + 1));
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                s[i + 1][j + 1] = s[i + 1][j] ^ s[i][j + 1] ^ s[i][j] ^ matrix[i][j];
            }
            a.insert(a.end(), s[i + 1].begin() + 1, s[i + 1].end());
        }
        ranges::nth_element(a, a.end() - k);
        return a[a.size() - k];
    }
};
```

```go [sol-Go]
func kthLargestValue(matrix [][]int, k int) int {
    m, n := len(matrix), len(matrix[0])
    a := make([]int, 0, m*n) // 预分配空间
    s := make([][]int, m+1)
    for i := range s {
        s[i] = make([]int, n+1)
    }
    for i, row := range matrix {
        for j, x := range row {
            s[i+1][j+1] = s[i+1][j] ^ s[i][j+1] ^ s[i][j] ^ x
        }
        a = append(a, s[i+1][1:]...)
    }
    slices.Sort(a)
    return a[len(a)-k]
}
```

```js [sol-JavaScript]
var kthLargestValue = function(matrix, k) {
    const m = matrix.length, n = matrix[0].length;
    const a = []
    const s = Array.from({ length: m + 1 }, () => Array(n + 1).fill(0));
    for (let i = 0; i < m; i++) {
        for (let j = 0; j < n; j++) {
            s[i + 1][j + 1] = s[i + 1][j] ^ s[i][j + 1] ^ s[i][j] ^ matrix[i][j];
            a.push(s[i + 1][j + 1]);
        }
    }
    a.sort((a, b) => b - a);
    return a[k - 1];
};
```

```rust [sol-Rust]
impl Solution {
    pub fn kth_largest_value(matrix: Vec<Vec<i32>>, k: i32) -> i32 {
        let m = matrix.len();
        let n = matrix[0].len();
        let mut a = Vec::with_capacity(m * n);
        let mut s = vec![vec![0; n + 1]; m + 1];
        for (i, row) in matrix.iter().enumerate() {
            for (j, &x) in row.iter().enumerate() {
                s[i + 1][j + 1] = s[i + 1][j] ^ s[i][j + 1] ^ s[i][j] ^ matrix[i][j];
            }
            a.extend_from_slice(&s[i + 1][1..n + 1]);
        }
        *a.select_nth_unstable(m * n - k as usize).1
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(mn\log (mn))$ 或 $\mathcal{O}(mn)$，其中 $m$ 和 $n$ 分别为 $\textit{matrix}$ 的行数和列数。快速选择算法可以做到 $\mathcal{O}(mn)$。
- 空间复杂度：$\mathcal{O}(mn)$。

## 方法二：列前缀异或和

遍历 $\textit{matrix}$ 的同时，计算 $\textit{colSum}[j]$，表示第 $j$ 列的**当前遍历过的元素**的异或和。

那么左上角在 $(0,0)$，右下角在 $(i,j)$ 的子矩阵异或和，就是

$$
\textit{colSum}[0]\oplus\textit{colSum}[1]\oplus\cdots\oplus\textit{colSum}[j]
$$

这也可以一边遍历 $\textit{matrix}$ 一边计算。

```py [sol-Python3]
class Solution:
    def kthLargestValue(self, matrix: List[List[int]], k: int) -> int:
        a = []
        col_sum = [0] * len(matrix[0])
        for row in matrix:
            s = 0
            for j, x in enumerate(row):
                col_sum[j] ^= x
                s ^= col_sum[j]
                a.append(s)
        a.sort()
        return a[-k]
```

```java [sol-Java]
class Solution {
    public int kthLargestValue(int[][] matrix, int k) {
        int m = matrix.length;
        int n = matrix[0].length;
        int[] a = new int[m * n];
        int[] colSum = new int[n];
        int i = 0;
        for (int[] row : matrix) {
            int s = 0;
            for (int j = 0; j < row.length; j++) {
                colSum[j] ^= row[j];
                s ^= colSum[j];
                a[i++] = s;
            }
        }
        Arrays.sort(a);
        return a[i - k];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int kthLargestValue(vector<vector<int>>& matrix, int k) {
        vector<int> a, col_sum(matrix[0].size());
        for (auto& row : matrix) {
            int s = 0;
            for (int j = 0; j < row.size(); j++) {
                col_sum[j] ^= row[j];
                s ^= col_sum[j];
                a.push_back(s);
            }
        }
        ranges::nth_element(a, a.end() - k);
        return a[a.size() - k];
    }
};
```

```go [sol-Go]
func kthLargestValue(matrix [][]int, k int) int {
    m, n := len(matrix), len(matrix[0])
    a := make([]int, 0, m*n) // 预分配空间
    colSum := make([]int, n)
    for _, row := range matrix {
        s := 0
        for j, x := range row {
            colSum[j] ^= x
            s ^= colSum[j]
            a = append(a, s)
        }
    }
    slices.Sort(a)
    return a[len(a)-k]
}
```

```js [sol-JavaScript]
var kthLargestValue = function(matrix, k) {
    const a = [];
    const colSum = Array(matrix[0].length).fill(0);
    for (const row of matrix) {
        let s = 0;
        for (let j = 0; j < row.length; j++) {
            colSum[j] ^= row[j];
            s ^= colSum[j];
            a.push(s);
        }
    }
    a.sort((x, y) => y - x);
    return a[k - 1];
};
```

```rust [sol-Rust]
impl Solution {
    pub fn kth_largest_value(matrix: Vec<Vec<i32>>, k: i32) -> i32 {
        let m = matrix.len();
        let n = matrix[0].len();
        let mut a = Vec::with_capacity(m * n);
        let mut col_sum = vec![0; n];
        for row in matrix {
            let mut s = 0;
            for (j, &x) in row.iter().enumerate() {
                col_sum[j] ^= x;
                s ^= col_sum[j];
                a.push(s);
            }
        }
        *a.select_nth_unstable(m * n - k as usize).1
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(mn\log (mn))$ 或 $\mathcal{O}(mn)$，其中 $m$ 和 $n$ 分别为 $\textit{matrix}$ 的行数和列数。快速选择算法可以做到 $\mathcal{O}(mn)$。
- 空间复杂度：$\mathcal{O}(mn)$。

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
