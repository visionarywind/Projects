# 240. 搜索二维矩阵 II

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/search-a-2d-matrix-ii/
- 题目 slug：`search-a-2d-matrix-ii`
- 来源专题：滑动窗口与双指针
- 来源分类路径：三、单序列双指针 / §3.6 矩阵上的双指针
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/search-a-2d-matrix-ii/solutions/2783938/tu-jie-pai-chu-fa-yi-tu-miao-dong-python-kytg/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【图解】排除法，一图秒懂！（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/search-a-2d-matrix-ii/solutions/2783938/tu-jie-pai-chu-fa-yi-tu-miao-dong-python-kytg/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tu-jie-pai-chu-fa-yi-tu-miao-dong-python-kytg`
- topic id：`2783938`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

矩阵的四个角是最特殊的，左上角的值最小，右下角的值最大，另两个角的值居中。对于有序数据，从右上角或者左下角这种中间值开始，可以帮助我们排除不含答案的行列。

![lc240.png](https://pic.leetcode.cn/1716183468-zVwElx-lc240.png){:width=550}

> 注：也可以从左下角开始，方法类似。

**总结**：利用 $\textit{matrix}$ 行列有序的性质，我们可以用 $\mathcal{O}(1)$ 的时间获取 $\mathcal{O}(m)$ 或 $\mathcal{O}(n)$ 的信息。相比之下，$\mathcal{O}(mn)$ 的暴力查找（一个一个地找），每次花费 $\mathcal{O}(1)$ 的时间，只获取了 $\mathcal{O}(1)$ 的信息。

这就是为什么我们可以把 $\mathcal{O}(mn)$ 的暴力查找优化成 $\mathcal{O}(m+n)$。

```py [sol-Python3]
class Solution:
    def searchMatrix(self, matrix: List[List[int]], target: int) -> bool:
        m, n = len(matrix), len(matrix[0])
        i, j = 0, n - 1  # 从右上角开始
        while i < m and j >= 0:  # 还有剩余元素
            if matrix[i][j] == target:
                return True  # 找到 target
            if matrix[i][j] < target:
                i += 1  # 这一行剩余元素全部小于 target，排除
            else:
                j -= 1  # 这一列剩余元素全部大于 target，排除
        return False
```

```java [sol-Java]
class Solution {
    public boolean searchMatrix(int[][] matrix, int target) {
        int i = 0;
        int j = matrix[0].length - 1; // 从右上角开始
        while (i < matrix.length && j >= 0) { // 还有剩余元素
            if (matrix[i][j] == target) {
                return true; // 找到 target
            }
            if (matrix[i][j] < target) {
                i++; // 这一行剩余元素全部小于 target，排除
            } else {
                j--; // 这一列剩余元素全部大于 target，排除
            }
        }
        return false;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    bool searchMatrix(vector<vector<int>>& matrix, int target) {
        int m = matrix.size(), n = matrix[0].size();
        int i = 0, j = n - 1; // 从右上角开始
        while (i < m && j >= 0) { // 还有剩余元素
            if (matrix[i][j] == target) {
                return true; // 找到 target
            }
            if (matrix[i][j] < target) {
                i++; // 这一行剩余元素全部小于 target，排除
            } else {
                j--; // 这一列剩余元素全部大于 target，排除
            }
        }
        return false;
    }
};
```

```c [sol-C]
bool searchMatrix(int** matrix, int matrixSize, int* matrixColSize, int target) {
    int m = matrixSize, n = matrixColSize[0];
    int i = 0, j = n - 1; // 从右上角开始
    while (i < m && j >= 0) { // 还有剩余元素
        if (matrix[i][j] == target) {
            return true; // 找到 target
        }
        if (matrix[i][j] < target) {
            i++; // 这一行剩余元素全部小于 target，排除
        } else {
            j--; // 这一列剩余元素全部大于 target，排除
        }
    }
    return false;
}
```

```go [sol-Go]
func searchMatrix(matrix [][]int, target int) bool {
    m, n := len(matrix), len(matrix[0])
    i, j := 0, n-1 // 从右上角开始
    for i < m && j >= 0 { // 还有剩余元素
        if matrix[i][j] == target {
            return true // 找到 target
        }
        if matrix[i][j] < target {
            i++ // 这一行剩余元素全部小于 target，排除
        } else {
            j-- // 这一列剩余元素全部大于 target，排除
        }
    }
    return false
}
```

```js [sol-JavaScript]
var searchMatrix = function(matrix, target) {
    const m = matrix.length, n = matrix[0].length;
    let i = 0, j = n - 1; // 从右上角开始
    while (i < m && j >= 0) { // 还有剩余元素
        if (matrix[i][j] === target) {
            return true; // 找到 target
        }
        if (matrix[i][j] < target) {
            i++; // 这一行剩余元素全部小于 target，排除
        } else {
            j--; // 这一列剩余元素全部大于 target，排除
        }
    }
    return false;
};
```

```rust [sol-Rust]
use std::cmp::Ordering;

impl Solution {
    pub fn search_matrix(matrix: Vec<Vec<i32>>, target: i32) -> bool {
        let m = matrix.len();
        let n = matrix[0].len();
        let mut i = 0;
        let mut j = n - 1; // 从右上角开始
        while i < m && j < n { // 还有剩余元素
            match matrix[i][j].cmp(&target) {
                Ordering::Equal => return true, // 找到 target
                Ordering::Less => i += 1, // 这一行剩余元素全部小于 target，排除
                _ => j -= 1, // 这一列剩余元素全部大于 target，排除
            }
        }
        false
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(m+n)$，其中 $m$ 和 $n$ 分别为 $\textit{matrix}$ 的行数和列数。每次循环排除掉一行或者一列，一共 $m+n$ 行列，最坏情况下需要排除 $m+n-1$ 行列才能找到答案。
- 空间复杂度：$\mathcal{O}(1)$。

## 注

如果你想出了一个“更快”的二分做法，试试下面这个数据：

```py
n = 300
matrix = [[1] * (n - 1 - i) + [9] * (i + 1) for i in range(n)]
target = 5
```

二分做法在这个数据上的时间复杂度为 $\mathcal{O}(n\log n)$，甚至更慢了。

## 相似题目

推荐完成 [1351. 统计有序矩阵中的负数](https://leetcode.cn/problems/count-negative-numbers-in-a-sorted-matrix/)，加深对排除法的理解。

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
