# 2245. 转角路径的乘积中最多能有几个尾随零

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-trailing-zeros-in-a-cornered-path/
- 题目 slug：`maximum-trailing-zeros-in-a-cornered-path`
- 来源专题：常用数据结构
- 来源分类路径：一、前缀和 / §1.5 进阶
- 难度分：2037
- 外部题解来源：https://leetcode.cn/problems/maximum-trailing-zeros-in-a-cornered-path/solutions/1427617/by-endlesscheng-7z5a/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[前缀和 + 从上往下/从下往上 + 左拐/右拐（Python/Java/C++/Go）](https://leetcode.cn/problems/maximum-trailing-zeros-in-a-cornered-path/solutions/1427617/by-endlesscheng-7z5a/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`by-endlesscheng-7z5a`
- topic id：`1427617`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

尾零的个数就是路径上的数的因子 $2$ 的个数和，与因子 $5$ 的个数之和的较小值。
 
那么数越多越好，路径的起点和终点都应该在边界上。

预处理每一行的因子的前缀和，然后枚举所有的路径：

- 从上往下走，枚举左拐/右拐；
- 从下往上走，枚举左拐/右拐。

所有路径上的 $\min(s_2,s_5)$ 的最大值即为答案，这里 $s_2$ 为路径上的因子 $2$ 的个数之和，$s_5$ 为路径上的因子 $5$ 的个数之和。

```python [sol1-Python3]
c2, c5 = [0] * 1001, [0] * 1001
for i in range(2, 1001):  # 预处理：递推算出每个数的因子 2 的个数和因子 5 的个数
    if i % 2 == 0: c2[i] = c2[i // 2] + 1
    if i % 5 == 0: c5[i] = c5[i // 5] + 1

class Solution:
    def maxTrailingZeros(self, grid: List[List[int]]) -> int:
        m, n = len(grid), len(grid[0])
        s = [[None] * (n + 1) for _ in range(m)]
        for i, row in enumerate(grid):
            s[i][0] = (0, 0)
            for j, v in enumerate(row):  # 计算 grid 每行因子 2 和 5 的前缀和
                s[i][j + 1] = (s[i][j][0] + c2[v], s[i][j][1] + c5[v])

        ans = 0
        for j, col in enumerate(zip(*grid)):
            s2 = s5 = 0
            for i, v in enumerate(col):  # 从上往下，枚举左拐还是右拐
                s2 += c2[v]
                s5 += c5[v]
                ans = max(ans, min(s2 + s[i][j][0], s5 + s[i][j][1]),
                               min(s2 + s[i][n][0] - s[i][j + 1][0], s5 + s[i][n][1] - s[i][j + 1][1]))
            s2 = s5 = 0
            for i in range(m - 1, -1, -1):  # 从下往上，枚举左拐还是右拐
                s2 += c2[col[i]]
                s5 += c5[col[i]]
                ans = max(ans, min(s2 + s[i][j][0], s5 + s[i][j][1]),
                               min(s2 + s[i][n][0] - s[i][j + 1][0], s5 + s[i][n][1] - s[i][j + 1][1]))
        return ans
```

```java [sol1-Java]
class Solution {
    static int[][] c25 = new int[1001][2];

    static {
        // 预处理：递推算出每个数的因子 2 的个数和因子 5 的个数
        for (var i = 2; i <= 1000; i++) {
            if (i % 2 == 0) c25[i][0] = c25[i / 2][0] + 1;
            if (i % 5 == 0) c25[i][1] = c25[i / 5][1] + 1;
        }
    }

    public int maxTrailingZeros(int[][] grid) {
        int m = grid.length, n = grid[0].length, ans = 0;
        var s = new int[m][n + 1][2];
        for (var i = 0; i < m; i++)
            for (var j = 0; j < n; j++) {
                s[i][j + 1][0] = s[i][j][0] + c25[grid[i][j]][0]; // 每行的因子 2 的前缀和
                s[i][j + 1][1] = s[i][j][1] + c25[grid[i][j]][1]; // 每行的因子 5 的前缀和
            }

        for (var j = 0; j < n; j++) {
            for (int i = 0, s2 = 0, s5 = 0; i < m; i++) { // 从上往下，枚举左拐还是右拐
                s2 += c25[grid[i][j]][0];
                s5 += c25[grid[i][j]][1];
                ans = Math.max(ans, Math.max(Math.min(s2 + s[i][j][0], s5 + s[i][j][1]), 
                                             Math.min(s2 + s[i][n][0] - s[i][j + 1][0], s5 + s[i][n][1] - s[i][j + 1][1])));
            }
            for (int i = m - 1, s2 = 0, s5 = 0; i >= 0; i--) { // 从下往上，枚举左拐还是右拐
                s2 += c25[grid[i][j]][0];
                s5 += c25[grid[i][j]][1];
                ans = Math.max(ans, Math.max(Math.min(s2 + s[i][j][0], s5 + s[i][j][1]), 
                                             Math.min(s2 + s[i][n][0] - s[i][j + 1][0], s5 + s[i][n][1] - s[i][j + 1][1])));
            }
        }
        return ans;
    }
}
```

```cpp [sol1-C++]
int c25[1001][2];
int init = []() {
    for (int i = 2; i <= 1000; ++i) { // 预处理：递推算出每个数的因子 2 的个数和因子 5 的个数
        if (i % 2 == 0) c25[i][0] = c25[i / 2][0] + 1;
        if (i % 5 == 0) c25[i][1] = c25[i / 5][1] + 1;
    }
    return 0;
}();

class Solution {
public:
    int maxTrailingZeros(vector<vector<int>> &grid) {
        int m = grid.size(), n = grid[0].size(), ans = 0;
        int s[m][n + 1][2];
        for (int i = 0; i < m; ++i) {
            s[i][0][0] = s[i][0][1] = 0;
            for (int j = 0; j < n; ++j) {
                s[i][j + 1][0] = s[i][j][0] + c25[grid[i][j]][0]; // 每行的因子 2 的前缀和
                s[i][j + 1][1] = s[i][j][1] + c25[grid[i][j]][1]; // 每行的因子 5 的前缀和
            }
        }

        for (int j = 0; j < n; ++j) {
            for (int i = 0, s2 = 0, s5 = 0; i < m; ++i) { // 从上往下，枚举左拐还是右拐
                s2 += c25[grid[i][j]][0];
                s5 += c25[grid[i][j]][1];
                ans = max(ans, max(min(s2 + s[i][j][0], s5 + s[i][j][1]),
                                   min(s2 + s[i][n][0] - s[i][j + 1][0], s5 + s[i][n][1] - s[i][j + 1][1])));
            }
            for (int i = m - 1, s2 = 0, s5 = 0; i >= 0; --i) { // 从下往上，枚举左拐还是右拐
                s2 += c25[grid[i][j]][0];
                s5 += c25[grid[i][j]][1];
                ans = max(ans, max(min(s2 + s[i][j][0], s5 + s[i][j][1]),
                                   min(s2 + s[i][n][0] - s[i][j + 1][0], s5 + s[i][n][1] - s[i][j + 1][1])));
            }
        }
        return ans;
    }
};
```

```go [sol1-Go]
var c25 [1001][2]int

func init() {
	// 预处理：递推算出每个数的因子 2 的个数和因子 5 的个数
	for i := 2; i <= 1000; i++ {
		if i%2 == 0 { c25[i][0] = c25[i/2][0] + 1 }
		if i%5 == 0 { c25[i][1] = c25[i/5][1] + 1 }
	}
}

func maxTrailingZeros(grid [][]int) (ans int) {
	m, n := len(grid), len(grid[0])
	s := make([][][2]int, m)
	for i, row := range grid {
		s[i] = make([][2]int, n+1)
		for j, v := range row {
			s[i][j+1][0] = s[i][j][0] + c25[v][0] // 每行的因子 2 的前缀和
			s[i][j+1][1] = s[i][j][1] + c25[v][1] // 每行的因子 5 的前缀和
		}
	}

	for j := 0; j < n; j++ {
		s2, s5 := 0, 0
		for i, row := range grid { // 从上往下，枚举左拐还是右拐
			s2 += c25[row[j]][0]
			s5 += c25[row[j]][1]
			ans = max(ans, max(min(s2+s[i][j][0], s5+s[i][j][1]), 
                               min(s2+s[i][n][0]-s[i][j+1][0], s5+s[i][n][1]-s[i][j+1][1])))
		}
		s2, s5 = 0, 0
		for i := m - 1; i >= 0; i-- { // 从下往上，枚举左拐还是右拐
			s2 += c25[grid[i][j]][0]
			s5 += c25[grid[i][j]][1]
			ans = max(ans, max(min(s2+s[i][j][0], s5+s[i][j][1]),
                               min(s2+s[i][n][0]-s[i][j+1][0], s5+s[i][n][1]-s[i][j+1][1])))
		}
	}
	return
}

func max(a, b int) int { if a < b { return b }; return a }
func min(a, b int) int { if a > b { return b }; return a }
```
