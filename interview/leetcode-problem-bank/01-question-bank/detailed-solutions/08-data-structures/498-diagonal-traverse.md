# 498. 对角线遍历

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/diagonal-traverse/
- 题目 slug：`diagonal-traverse`
- 来源专题：常用数据结构
- 来源分类路径：零、常用枚举技巧 / §0.3 遍历对角线
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/diagonal-traverse/solutions/3762798/mo-ban-bian-li-dui-jiao-xian-pythonjavac-jnky/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【模板】遍历对角线（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/diagonal-traverse/solutions/3762798/mo-ban-bian-li-dui-jiao-xian-pythonjavac-jnky/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`mo-ban-bian-li-dui-jiao-xian-pythonjavac-jnky`
- topic id：`3762798`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 16:46:02 +0800

![lc498.jpg](https://pic.leetcode.cn/1756077218-jLFMlk-lc498.jpg)

对于每条对角线，行号 $i$ 加列号 $j$ 是一个定值。示例 1 正中间对角线的 $i+j$ 恒为 $2$。（可以回想一下 [51. N 皇后](https://leetcode.cn/problems/n-queens/) 的写法）

设 $k=i+j$，那么左上角那条对角线的 $k=0$，右下角那条对角线的 $k= (m-1)+(n-1) = m+n-2$。

枚举 $k=0,1,2,\dots,m+n-2$，就相当于在从左上到右下，**一条一条地枚举对角线**。

由于 $i+j=k$，知道 $j$ 就知道 $i$，所以我们只需要计算出每条对角线的 $j$ 的**最小值**和**最大值**，就可以开始遍历对角线了。

- 由于 $j=k-i$，当 $i=m-1$ 时 $j$ 取到最小值 $k-m+1$，但这个数不能是负数，所以最小的 $j$ 是 $\max(k-m+1,0)$。
- 由于 $j=k-i$，当 $i=0$ 时 $j$ 取到最大值 $k$，但这个数不能超过 $n-1$，所以最大的 $j$ 是 $\min(k, n - 1)$。

然后就可以**模拟**了：

1. 枚举 $k=0,1,2,\dots,m+n-2$。
2. 如果 $k$ 是偶数，我们从小到大枚举 $j$，否则从大到小枚举 $j$。其中 $j$ 的范围是 $[\max(k-m+1,0),\min(k, n - 1)]$。
3. 把 $\textit{mat}[k-j][j]$ 加入答案。

```py [sol-Python3]
class Solution:
    def findDiagonalOrder(self, mat: List[List[int]]) -> List[int]:
        m, n = len(mat), len(mat[0])
        ans = []
        for k in range(m + n - 1):
            min_j = max(k - m + 1, 0)
            max_j = min(k, n - 1)
            if k % 2 == 0:  # 偶数从左到右
                for j in range(min_j, max_j + 1):
                    ans.append(mat[k - j][j])
            else:  # 奇数从右到左
                for j in range(max_j, min_j - 1, -1):
                    ans.append(mat[k - j][j])
        return ans
```

```java [sol-Java]
class Solution {
    public int[] findDiagonalOrder(int[][] mat) {
        int m = mat.length;
        int n = mat[0].length;
        int[] ans = new int[m * n];
        int idx = 0;
        for (int k = 0; k < m + n - 1; k++) {
            int minJ = Math.max(k - m + 1, 0);
            int maxJ = Math.min(k, n - 1);
            if (k % 2 == 0) { // 偶数从左到右
                for (int j = minJ; j <= maxJ; j++) {
                    ans[idx++] = mat[k - j][j];
                }
            } else { // 奇数从右到左
                for (int j = maxJ; j >= minJ; j--) {
                    ans[idx++] = mat[k - j][j];
                }
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> findDiagonalOrder(vector<vector<int>>& mat) {
        int m = mat.size(), n = mat[0].size();
        vector<int> ans;
        ans.reserve(m * n); // 预分配空间
        for (int k = 0; k < m + n - 1; k++) {
            int min_j = max(k - m + 1, 0);
            int max_j = min(k, n - 1);
            if (k % 2 == 0) { // 偶数从左到右
                for (int j = min_j; j <= max_j; j++) {
                    ans.push_back(mat[k - j][j]);
                }
            } else { // 奇数从右到左
                for (int j = max_j; j >= min_j; j--) {
                    ans.push_back(mat[k - j][j]);
                }
            }
        }
        return ans;
    }
};
```

```c [sol-C]
#define MIN(a, b) ((b) < (a) ? (b) : (a))
#define MAX(a, b) ((b) > (a) ? (b) : (a))

int* findDiagonalOrder(int** mat, int matSize, int* matColSize, int* returnSize) {
    int m = matSize, n = matColSize[0];
    *returnSize = m * n;
    int* ans = malloc(m * n * sizeof(int));
    int idx = 0;
    for (int k = 0; k < m + n - 1; k++) {
        int min_j = MAX(k - m + 1, 0);
        int max_j = MIN(k, n - 1);
        if (k % 2 == 0) { // 偶数从左到右
            for (int j = min_j; j <= max_j; j++) {
                ans[idx++] = mat[k - j][j];
            }
        } else { // 奇数从右到左
            for (int j = max_j; j >= min_j; j--) {
                ans[idx++] = mat[k - j][j];
            }
        }
    }
    return ans;
}
```

```go [sol-Go]
func findDiagonalOrder(mat [][]int) []int {
    m, n := len(mat), len(mat[0])
    ans := make([]int, 0, m*n) // 预分配空间
    for k := range m + n - 1 {
        minJ := max(k-m+1, 0)
        maxJ := min(k, n-1)
        if k%2 == 0 { // 偶数从左到右
            for j := minJ; j <= maxJ; j++ {
                ans = append(ans, mat[k-j][j])
            }
        } else { // 奇数从右到左
            for j := maxJ; j >= minJ; j-- {
                ans = append(ans, mat[k-j][j])
            }
        }
    }
    return ans
}
```

```js [sol-JavaScript]
var findDiagonalOrder = function(mat) {
    const m = mat.length, n = mat[0].length;
    const ans = [];
    for (let k = 0; k < m + n - 1; k++) {
        const minJ = Math.max(k - m + 1, 0);
        const maxJ = Math.min(k, n - 1);
        if (k % 2 === 0) { // 偶数从左到右
            for (let j = minJ; j <= maxJ; j++) {
                ans.push(mat[k - j][j]);
            }
        } else { // 奇数从右到左
            for (let j = maxJ; j >= minJ; j--) {
                ans.push(mat[k - j][j]);
            }
        }
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn find_diagonal_order(mat: Vec<Vec<i32>>) -> Vec<i32> {
        let m = mat.len();
        let n = mat[0].len();
        let mut ans = Vec::with_capacity(m * n); // 预分配空间
        for k in 0..m + n - 1 {
            let min_j = k.saturating_sub(m - 1);
            let max_j = k.min(n - 1);
            if k % 2 == 0 { // 偶数从左到右
                for j in min_j..=max_j {
                    ans.push(mat[k - j][j]);
                }
            } else { // 奇数从右到左
                for j in (min_j..=max_j).rev() {
                    ans.push(mat[k - j][j]);
                }
            }
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(mn)$，其中 $m$ 和 $n$ 分别为 $\textit{grid}$ 的行数和列数。
- 空间复杂度：$\mathcal{O}(1)$。返回值不计入。

## 相似题目

- [3446. 按对角线进行矩阵排序](https://leetcode.cn/problems/sort-matrix-by-diagonals/) 1373
- [2711. 对角线上不同值的数量差](https://leetcode.cn/problems/difference-of-number-of-distinct-values-on-diagonals/) 1429
- [1329. 将矩阵按对角线排序](https://leetcode.cn/problems/sort-the-matrix-diagonally/) 1548
- [562. 矩阵中最长的连续1线段](https://leetcode.cn/problems/longest-line-of-consecutive-one-in-matrix/)（会员题）

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
