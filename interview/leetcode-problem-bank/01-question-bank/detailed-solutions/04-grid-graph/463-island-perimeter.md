# 463. 岛屿的周长

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/island-perimeter/
- 题目 slug：`island-perimeter`
- 来源专题：网格图
- 来源分类路径：一、网格图 DFS
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/island-perimeter/solutions/3756201/nao-jin-ji-zhuan-wan-pythonjavaccgojsrus-met3/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[脑筋急转弯（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/island-perimeter/solutions/3756201/nao-jin-ji-zhuan-wan-pythonjavaccgojsrus-met3/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`nao-jin-ji-zhuan-wan-pythonjavaccgojsrus-met3`
- topic id：`3756201`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

做过 [200. 岛屿数量](https://leetcode.cn/problems/number-of-islands/) 的同学，可能看到题目会去思考 DFS 岛屿，统计周长的方法。

但实际上不需要这么麻烦。

![lc463.png](https://pic.leetcode.cn/1755487833-EhbVOQ-lc463.png)

周长是什么？陆地边缘（上图黄线）的长度。

陆地是什么？$\textit{grid}[i][j]=1$ 的格子。

边缘怎么判定？看图，黄色的边的两侧，一边是陆地，另一边出界或者是水。我们可以枚举陆地格子上下左右四个方向的相邻格子，如果出界或者是水，那么就找到了一条黄色的边。

> 注意题目保证没有内陆湖，我们统计的一定是岛屿的边缘。

```py [sol-Python3]
class Solution:
    def islandPerimeter(self, grid: List[List[int]]) -> int:
        m, n = len(grid), len(grid[0])
        ans = 0
        for i, row in enumerate(grid):
            for j, x in enumerate(row):
                if x == 0:
                    continue
                if i == 0 or grid[i - 1][j] == 0:  # 上：出界或者是水
                    ans += 1
                if i == m - 1 or grid[i + 1][j] == 0:  # 下：出界或者是水
                    ans += 1
                if j == 0 or row[j - 1] == 0:  # 左：出界或者是水
                    ans += 1
                if j == n - 1 or row[j + 1] == 0:  # 右：出界或者是水
                    ans += 1
        return ans
```

```java [sol-Java]
class Solution {
    public int islandPerimeter(int[][] grid) {
        int m = grid.length;
        int n = grid[0].length;
        int ans = 0;
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                if (grid[i][j] == 0) {
                    continue;
                }
                if (i == 0 || grid[i - 1][j] == 0) { // 上：出界或者是水
                    ans++;
                }
                if (i == m - 1 || grid[i + 1][j] == 0) { // 下：出界或者是水
                    ans++;
                }
                if (j == 0 || grid[i][j - 1] == 0) { // 左：出界或者是水
                    ans++;
                }
                if (j == n - 1 || grid[i][j + 1] == 0) { // 右：出界或者是水
                    ans++;
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
    int islandPerimeter(vector<vector<int>>& grid) {
        int m = grid.size(), n = grid[0].size();
        int ans = 0;
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                if (grid[i][j] == 0) {
                    continue;
                }
                if (i == 0 || grid[i - 1][j] == 0) { // 上：出界或者是水
                    ans++;
                }
                if (i == m - 1 || grid[i + 1][j] == 0) { // 下：出界或者是水
                    ans++;
                }
                if (j == 0 || grid[i][j - 1] == 0) { // 左：出界或者是水
                    ans++;
                }
                if (j == n - 1 || grid[i][j + 1] == 0) { // 右：出界或者是水
                    ans++;
                }
            }
        }
        return ans;
    }
};
```

```c [sol-C]
int islandPerimeter(int** grid, int gridSize, int* gridColSize) {
    int m = gridSize, n = gridColSize[0];
    int ans = 0;
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            if (grid[i][j] == 0) {
                continue;
            }
            if (i == 0 || grid[i - 1][j] == 0) { // 上：出界或者是水
                ans++;
            }
            if (i == m - 1 || grid[i + 1][j] == 0) { // 下：出界或者是水
                ans++;
            }
            if (j == 0 || grid[i][j - 1] == 0) { // 左：出界或者是水
                ans++;
            }
            if (j == n - 1 || grid[i][j + 1] == 0) { // 右：出界或者是水
                ans++;
            }
        }
    }
    return ans;
}
```

```go [sol-Go]
func islandPerimeter(grid [][]int) (ans int) {
    m, n := len(grid), len(grid[0])
    for i, row := range grid {
        for j, x := range row {
            if x == 0 {
                continue
            }
            if i == 0 || grid[i-1][j] == 0 { // 上：出界或者是水
                ans++
            }
            if i == m-1 || grid[i+1][j] == 0 { // 下：出界或者是水
                ans++
            }
            if j == 0 || row[j-1] == 0 { // 左：出界或者是水
                ans++
            }
            if j == n-1 || row[j+1] == 0 { // 右：出界或者是水
                ans++
            }
        }
    }
    return
}
```

```js [sol-JavaScript]
var islandPerimeter = function(grid) {
    const m = grid.length, n = grid[0].length;
    let ans = 0;
    for (let i = 0; i < m; i++) {
        for (let j = 0; j < n; j++) {
            if (grid[i][j] === 0) {
                continue;
            }
            if (i === 0 || grid[i - 1][j] === 0) { // 上：出界或者是水
                ans++;
            }
            if (i === m - 1 || grid[i + 1][j] === 0) { // 下：出界或者是水
                ans++;
            }
            if (j === 0 || grid[i][j - 1] === 0) { // 左：出界或者是水
                ans++;
            }
            if (j === n - 1 || grid[i][j + 1] === 0) { // 右：出界或者是水
                ans++;
            }
        }
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn island_perimeter(grid: Vec<Vec<i32>>) -> i32 {
        let m = grid.len();
        let n = grid[0].len();
        let mut ans = 0;
        for (i, row) in grid.iter().enumerate() {
            for (j, &x) in row.iter().enumerate() {
                if x == 0 {
                    continue;
                }
                if i == 0 || grid[i - 1][j] == 0 { // 上：出界或者是水
                    ans += 1;
                }
                if i == m - 1 || grid[i + 1][j] == 0 { // 下：出界或者是水
                    ans += 1;
                }
                if j == 0 || grid[i][j - 1] == 0 { // 左：出界或者是水
                    ans += 1;
                }
                if j == n - 1 || grid[i][j + 1] == 0 { // 右：出界或者是水
                    ans += 1;
                }
            }
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(mn)$，其中 $m$ 和 $n$ 分别为 $\textit{grid}$ 的行数和列数。
- 空间复杂度：$\mathcal{O}(1)$。

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
