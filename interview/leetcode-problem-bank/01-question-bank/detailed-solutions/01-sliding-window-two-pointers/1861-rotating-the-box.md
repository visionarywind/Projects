# 1861. 旋转盒子

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/rotating-the-box/
- 题目 slug：`rotating-the-box`
- 来源专题：滑动窗口与双指针
- 来源分类路径：三、单序列双指针 / §3.2 相向双指针
- 难度分：1537
- 外部题解来源：https://leetcode.cn/problems/rotating-the-box/solutions/778595/mei-xing-fen-duan-tong-ji-by-endlesschen-umie/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：正序遍历 / 倒序遍历（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/rotating-the-box/solutions/778595/mei-xing-fen-duan-tong-ji-by-endlesschen-umie/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`mei-xing-fen-duan-tong-ji-by-endlesschen-umie`
- topic id：`778595`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 方法一：正序遍历

$\textit{boxGrid}$ 的每一行互相独立，可以分别计算。

单独看每一行，我们需要知道每个障碍物（$\texttt{*}$）的左边有多少个石头（$\texttt{#}$）。

具体地，设当前障碍物到上一个障碍物之间有 $\textit{cnt}$ 个石头。那么旋转后，当前障碍物的左边有连续 $\textit{cnt}$ 个石头。据此：

- 在遍历过程中，统计石头的个数 $\textit{cnt}$。
- 如果下一个格子是障碍物，或者当前格子是最后一个格子，那么从当前格子往前填入连续 $\textit{cnt}$ 个石头，并重置计数器 $\textit{cnt}=0$。

**细节**：第 $i$ 行的格子旋转后在倒数第 $i$ 列，第 $j$ 列的格子旋转后在第 $j$ 行。所以 $(i,j)$ 旋转后位于 $(j,m-1-i)$。

```py [sol-Python3]
class Solution:
    def rotateTheBox(self, boxGrid: list[list[str]]) -> list[list[str]]:
        m, n = len(boxGrid), len(boxGrid[0])
        ans = [[''] * m for _ in range(n)]

        for i, row in enumerate(boxGrid):
            cnt = 0
            for j, ch in enumerate(row):
                if ch == '#':  # 石头
                    cnt += 1
                    ch = '.'  # 先把石头清空
                ans[j][-1 - i] = ch
                if j == n - 1 or row[j + 1] == '*':  # 下一个格子是障碍物
                    # 石头垂直掉落后，从 j 往前 cnt 个格子都是石头
                    for k in range(j, j - cnt, -1):
                        ans[k][-1 - i] = '#'
                    cnt = 0  # 重置计数器

        return ans
```

```java [sol-Java]
class Solution {
    public char[][] rotateTheBox(char[][] boxGrid) {
        int m = boxGrid.length;
        int n = boxGrid[0].length;
        char[][] ans = new char[n][m];

        for (int i = 0; i < m; i++) {
            char[] row = boxGrid[i];
            int cnt = 0;
            for (int j = 0; j < n; j++) {
                char ch = row[j];
                if (ch == '#') { // 石头
                    cnt++;
                    ch = '.'; // 先把石头清空
                }
                ans[j][m - 1 - i] = ch;
                if (j == n - 1 || row[j + 1] == '*') { // 下一个格子是障碍物
                    // 石头垂直掉落后，从 j 往前 cnt 个格子都是石头
                    for (int k = j; k > j - cnt; k--) {
                        ans[k][m - 1 - i] = '#';
                    }
                    cnt = 0; // 重置计数器
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
    vector<vector<char>> rotateTheBox(vector<vector<char>>& boxGrid) {
        int m = boxGrid.size(), n = boxGrid[0].size();
        vector ans(n, vector<char>(m));

        for (int i = 0; i < m; i++) {
            auto& row = boxGrid[i];
            int cnt = 0;
            for (int j = 0; j < n; j++) {
                char ch = row[j];
                if (ch == '#') { // 石头
                    cnt++;
                    ch = '.'; // 先把石头清空
                }
                ans[j][m - 1 - i] = ch;
                if (j == n - 1 || row[j + 1] == '*') { // 下一个格子是障碍物
                    // 石头垂直掉落后，从 j 往前 cnt 个格子都是石头
                    for (int k = j; k > j - cnt; k--) {
                        ans[k][m - 1 - i] = '#';
                    }
                    cnt = 0; // 重置计数器
                }
            }
        }

        return ans;
    }
};
```

```c [sol-C]
char** rotateTheBox(char** boxGrid, int boxGridSize, int* boxGridColSize, int* returnSize, int** returnColumnSizes) {
    int m = boxGridSize, n = boxGridColSize[0];
    char** ans = malloc(n * sizeof(char*));
    *returnColumnSizes = malloc(n * sizeof(int));
    *returnSize = n;
    for (int i = 0; i < n; i++) {
        ans[i] = malloc(m * sizeof(char));
        (*returnColumnSizes)[i] = m;
    }

    for (int i = 0; i < m; i++) {
        char* row = boxGrid[i];
        int cnt = 0;
        for (int j = 0; j < n; j++) {
            char ch = row[j];
            if (ch == '#') { // 石头
                cnt++;
                ch = '.'; // 先把石头清空
            }
            ans[j][m - 1 - i] = ch;
            if (j == n - 1 || row[j + 1] == '*') { // 下一个格子是障碍物
                // 石头垂直掉落后，从 j 往前 cnt 个格子都是石头
                for (int k = j; k > j - cnt; k--) {
                    ans[k][m - 1 - i] = '#';
                }
                cnt = 0; // 重置计数器
            }
        }
    }

    return ans;
}
```

```go [sol-Go]
func rotateTheBox(boxGrid [][]byte) [][]byte {
	m, n := len(boxGrid), len(boxGrid[0])
	ans := make([][]byte, n)
	for i := range ans {
		ans[i] = make([]byte, m)
	}

	for i, row := range boxGrid {
		cnt := 0
		for j, ch := range row {
			if ch == '#' { // 石头
				cnt++
				ch = '.' // 先把石头清空
			}
			ans[j][m-1-i] = ch
			if j == n-1 || row[j+1] == '*' { // 下一个格子是障碍物
				// 石头垂直掉落后，从 j 往前 cnt 个格子都是石头
				for k := j; k > j-cnt; k-- {
					ans[k][m-1-i] = '#'
				}
				cnt = 0 // 重置计数器
			}
		}
	}

	return ans
}
```

```js [sol-JavaScript]
var rotateTheBox = function(boxGrid) {
    const m = boxGrid.length, n = boxGrid[0].length;
    const ans = Array.from({ length: n }, () => Array(m));

    for (let i = 0; i < m; i++) {
        const row = boxGrid[i];
        let cnt = 0;
        for (let j = 0; j < n; j++) {
            let ch = row[j];
            if (ch === '#') { // 石头
                cnt++;
                ch = '.'; // 先把石头清空
            }
            ans[j][m - 1 - i] = ch;
            if (j === n - 1 || row[j + 1] === '*') { // 下一个格子是障碍物
                // 石头垂直掉落后，从 j 往前 cnt 个格子都是石头
                for (let k = j; k > j - cnt; k--) {
                    ans[k][m - 1 - i] = '#';
                }
                cnt = 0; // 重置计数器
            }
        }
    }

    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn rotate_the_box(box_grid: Vec<Vec<char>>) -> Vec<Vec<char>> {
        let m = box_grid.len();
        let n = box_grid[0].len();
        let mut ans = vec![vec!['\0'; m]; n];

        for (i, row) in box_grid.iter().enumerate() {
            let mut cnt = 0;
            for (j, &ch) in row.into_iter().enumerate() {
                let mut ch = ch;
                if ch == '#' { // 石头
                    cnt += 1;
                    ch = '.'; // 先把石头清空
                }
                ans[j][m - 1 - i] = ch;
                if j == n - 1 || row[j + 1] == '*' { // 下一个格子是障碍物
                    // 石头垂直掉落后，从 j 往前 cnt 个格子都是石头
                    for k in j - cnt + 1..=j {
                        ans[k][m - 1 - i] = '#';
                    }
                    cnt = 0; // 重置计数器
                }
            }
        }

        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(mn)$，其中 $m$ 和 $n$ 分别是 $\textit{boxGrid}$ 的行数和列数。
- 空间复杂度：$\mathcal{O}(1)$。返回值不计入。

## 方法二：倒序遍历 + 双指针

对于每一行 $\textit{row}$，倒着遍历，我们可以直接确定每个石头落入的位置：

- 如果 $\textit{row}[j]$ 是障碍物，那么它左边最近的石头，在旋转后掉落到 $\textit{row}[j-1]$。我们用一个变量 $k$ 维护石头掉落后的位置，如果 $\textit{row}[j]$ 是障碍物，那么更新 $\textit{k} = j-1$。注：如果 $\textit{row}[j]$ 左边最近的不是石头而是障碍物，那么 $k$ 会继续更新，无需担心石头落到错误的位置。
- 如果 $\textit{row}[j]$ 是石头，那么它掉落到 $\textit{row}[\textit{k}]$。然后把 $k$ 减一，表示左边下一块石头掉落后的位置。

```py [sol-Python3]
class Solution:
    def rotateTheBox(self, boxGrid: list[list[str]]) -> list[list[str]]:
        m, n = len(boxGrid), len(boxGrid[0])
        ans = [['.'] * m for _ in range(n)]

        for i, row in enumerate(boxGrid):
            k = n - 1
            for j in range(n - 1, -1, -1):
                if row[j] == '*':  # 障碍物
                    ans[j][-1 - i] = '*'
                    k = j - 1  # 障碍物左边最近的石头，在旋转后掉落到 j-1
                elif row[j] == '#':  # 石头
                    ans[k][-1 - i] = '#'  # 旋转后，石头掉落到 k
                    k -= 1

        return ans
```

```java [sol-Java]
class Solution {
    public char[][] rotateTheBox(char[][] boxGrid) {
        int m = boxGrid.length;
        int n = boxGrid[0].length;
        char[][] ans = new char[n][m];
        for (char[] row : ans) {
            Arrays.fill(row, '.');
        }

        for (int i = 0; i < m; i++) {
            char[] row = boxGrid[i];
            int k = n - 1;
            for (int j = n - 1; j >= 0; j--) {
                if (row[j] == '*') { // 障碍物
                    ans[j][m - 1 - i] = '*';
                    k = j - 1; // 障碍物左边最近的石头，在旋转后掉落到 j-1
                } else if (row[j] == '#') { // 石头
                    ans[k][m - 1 - i] = '#'; // 旋转后，石头掉落到 k
                    k--;
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
    vector<vector<char>> rotateTheBox(vector<vector<char>>& boxGrid) {
        int m = boxGrid.size(), n = boxGrid[0].size();
        vector ans(n, vector<char>(m, '.'));

        for (int i = 0; i < m; i++) {
            auto& row = boxGrid[i];
            int k = n - 1;
            for (int j = n - 1; j >= 0; j--) {
                if (row[j] == '*') { // 障碍物
                    ans[j][m - 1 - i] = '*';
                    k = j - 1; // 障碍物左边最近的石头，在旋转后掉落到 j-1
                } else if (row[j] == '#') { // 石头
                    ans[k][m - 1 - i] = '#'; // 旋转后，石头掉落到 k
                    k--;
                }
            }
        }

        return ans;
    }
};
```

```c [sol-C]
char** rotateTheBox(char** boxGrid, int boxGridSize, int* boxGridColSize, int* returnSize, int** returnColumnSizes) {
    int m = boxGridSize, n = boxGridColSize[0];
    char** ans = malloc(n * sizeof(char*));
    *returnColumnSizes = malloc(n * sizeof(int));
    *returnSize = n;
    for (int i = 0; i < n; i++) {
        ans[i] = malloc(m * sizeof(char));
        memset(ans[i], '.', m * sizeof(char));
        (*returnColumnSizes)[i] = m;
    }

    for (int i = 0; i < m; i++) {
        char* row = boxGrid[i];
        int k = n - 1;
        for (int j = n - 1; j >= 0; j--) {
            if (row[j] == '*') { // 障碍物
                ans[j][m - 1 - i] = '*';
                k = j - 1; // 障碍物左边最近的石头，在旋转后掉落到 j-1
            } else if (row[j] == '#') { // 石头
                ans[k][m - 1 - i] = '#'; // 旋转后，石头掉落到 k
                k--;
            }
        }
    }

    return ans;
}
```

```go [sol-Go]
func rotateTheBox(boxGrid [][]byte) [][]byte {
	m, n := len(boxGrid), len(boxGrid[0])
	ans := make([][]byte, n)
	for i := range ans {
		ans[i] = bytes.Repeat([]byte{'.'}, m)
	}

	for i, row := range boxGrid {
		k := n - 1
		for j := n - 1; j >= 0; j-- {
			if row[j] == '*' { // 障碍物
				ans[j][m-1-i] = '*'
				k = j - 1 // 障碍物左边最近的石头，在旋转后掉落到 j-1
			} else if row[j] == '#' { // 石头
				ans[k][m-1-i] = '#' // 旋转后，石头掉落到 k
				k--
			}
		}
	}

	return ans
}
```

```js [sol-JavaScript]
var rotateTheBox = function(boxGrid) {
    const m = boxGrid.length, n = boxGrid[0].length;
    const ans = Array.from({ length: n }, () => Array(m).fill('.'));

    for (let i = 0; i < m; i++) {
        const row = boxGrid[i];
        let k = n - 1;
        for (let j = n - 1; j >= 0; j--) {
            if (row[j] === '*') { // 障碍物
                ans[j][m - 1 - i] = '*';
                k = j - 1; // 障碍物左边最近的石头，在旋转后掉落到 j-1
            } else if (row[j] === '#') { // 石头
                ans[k][m - 1 - i] = '#'; // 旋转后，石头掉落到 k
                k--;
            }
        }
    }

    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn rotate_the_box(box_grid: Vec<Vec<char>>) -> Vec<Vec<char>> {
        let m = box_grid.len();
        let n = box_grid[0].len();
        let mut ans = vec![vec!['.'; m]; n];

        for (i, row) in box_grid.into_iter().enumerate() {
            let mut k = n - 1;
            for (j, ch) in row.into_iter().enumerate().rev() {
                if ch == '*' { // 障碍物
                    ans[j][m - 1 - i] = '*';
                    k = j - 1; // 障碍物左边最近的石头，在旋转后掉落到 j-1
                } else if ch == '#' { // 石头
                    ans[k][m - 1 - i] = '#'; // 旋转后，石头掉落到 k
                    k -= 1;
                }
            }
        }

        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(mn)$，其中 $m$ 和 $n$ 分别是 $\textit{boxGrid}$ 的行数和列数。
- 空间复杂度：$\mathcal{O}(1)$。返回值不计入。

## 专题训练

见下面双指针题单的「**六、分组循环**」。

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

## 方法一：正序遍历

$\textit{boxGrid}$ 的每一行互相独立，可以分别计算。

单独看每一行，我们需要知道每个障碍物（$\texttt{*}$）的左边有多少个石头（$\texttt{#}$）。

具体地，设当前障碍物到上一个障碍物之间有 $\textit{cnt}$ 个石头。那么旋转后，当前障碍物的左边有连续 $\textit{cnt}$ 个石头。据此：

- 在遍历过程中，统计石头的个数 $\textit{cnt}$。
- 如果下一个格子是障碍物，或者当前格子是最后一个格子，那么从当前格子往前填入连续 $\textit{cnt}$ 个石头，并重置计数器 $\textit{cnt}=0$。

**细节**：第 $i$ 行的格子旋转后在倒数第 $i$ 列，第 $j$ 列的格子旋转后在第 $j$ 行。所以 $(i,j)$ 旋转后位于 $(j,m-1-i)$。

```py [sol-Python3]
class Solution:
    def rotateTheBox(self, boxGrid: list[list[str]]) -> list[list[str]]:
        m, n = len(boxGrid), len(boxGrid[0])
        ans = [[''] * m for _ in range(n)]

        for i, row in enumerate(boxGrid):
            cnt = 0
            for j, ch in enumerate(row):
                if ch == '#':  # 石头
                    cnt += 1
                    ch = '.'  # 先把石头清空
                ans[j][-1 - i] = ch
                if j == n - 1 or row[j + 1] == '*':  # 下一个格子是障碍物
                    # 石头垂直掉落后，从 j 往前 cnt 个格子都是石头
                    for k in range(j, j - cnt, -1):
                        ans[k][-1 - i] = '#'
                    cnt = 0  # 重置计数器

        return ans
```

```java [sol-Java]
class Solution {
    public char[][] rotateTheBox(char[][] boxGrid) {
        int m = boxGrid.length;
        int n = boxGrid[0].length;
        char[][] ans = new char[n][m];

        for (int i = 0; i < m; i++) {
            char[] row = boxGrid[i];
            int cnt = 0;
            for (int j = 0; j < n; j++) {
                char ch = row[j];
                if (ch == '#') { // 石头
                    cnt++;
                    ch = '.'; // 先把石头清空
                }
                ans[j][m - 1 - i] = ch;
                if (j == n - 1 || row[j + 1] == '*') { // 下一个格子是障碍物
                    // 石头垂直掉落后，从 j 往前 cnt 个格子都是石头
                    for (int k = j; k > j - cnt; k--) {
                        ans[k][m - 1 - i] = '#';
                    }
                    cnt = 0; // 重置计数器
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
    vector<vector<char>> rotateTheBox(vector<vector<char>>& boxGrid) {
        int m = boxGrid.size(), n = boxGrid[0].size();
        vector ans(n, vector<char>(m));

        for (int i = 0; i < m; i++) {
            auto& row = boxGrid[i];
            int cnt = 0;
            for (int j = 0; j < n; j++) {
                char ch = row[j];
                if (ch == '#') { // 石头
                    cnt++;
                    ch = '.'; // 先把石头清空
                }
                ans[j][m - 1 - i] = ch;
                if (j == n - 1 || row[j + 1] == '*') { // 下一个格子是障碍物
                    // 石头垂直掉落后，从 j 往前 cnt 个格子都是石头
                    for (int k = j; k > j - cnt; k--) {
                        ans[k][m - 1 - i] = '#';
                    }
                    cnt = 0; // 重置计数器
                }
            }
        }

        return ans;
    }
};
```

```c [sol-C]
char** rotateTheBox(char** boxGrid, int boxGridSize, int* boxGridColSize, int* returnSize, int** returnColumnSizes) {
    int m = boxGridSize, n = boxGridColSize[0];
    char** ans = malloc(n * sizeof(char*));
    *returnColumnSizes = malloc(n * sizeof(int));
    *returnSize = n;
    for (int i = 0; i < n; i++) {
        ans[i] = malloc(m * sizeof(char));
        (*returnColumnSizes)[i] = m;
    }

    for (int i = 0; i < m; i++) {
        char* row = boxGrid[i];
        int cnt = 0;
        for (int j = 0; j < n; j++) {
            char ch = row[j];
            if (ch == '#') { // 石头
                cnt++;
                ch = '.'; // 先把石头清空
            }
            ans[j][m - 1 - i] = ch;
            if (j == n - 1 || row[j + 1] == '*') { // 下一个格子是障碍物
                // 石头垂直掉落后，从 j 往前 cnt 个格子都是石头
                for (int k = j; k > j - cnt; k--) {
                    ans[k][m - 1 - i] = '#';
                }
                cnt = 0; // 重置计数器
            }
        }
    }

    return ans;
}
```

```go [sol-Go]
func rotateTheBox(boxGrid [][]byte) [][]byte {
	m, n := len(boxGrid), len(boxGrid[0])
	ans := make([][]byte, n)
	for i := range ans {
		ans[i] = make([]byte, m)
	}

	for i, row := range boxGrid {
		cnt := 0
		for j, ch := range row {
			if ch == '#' { // 石头
				cnt++
				ch = '.' // 先把石头清空
			}
			ans[j][m-1-i] = ch
			if j == n-1 || row[j+1] == '*' { // 下一个格子是障碍物
				// 石头垂直掉落后，从 j 往前 cnt 个格子都是石头
				for k := j; k > j-cnt; k-- {
					ans[k][m-1-i] = '#'
				}
				cnt = 0 // 重置计数器
			}
		}
	}

	return ans
}
```

```js [sol-JavaScript]
var rotateTheBox = function(boxGrid) {
    const m = boxGrid.length, n = boxGrid[0].length;
    const ans = Array.from({ length: n }, () => Array(m));

    for (let i = 0; i < m; i++) {
        const row = boxGrid[i];
        let cnt = 0;
        for (let j = 0; j < n; j++) {
            let ch = row[j];
            if (ch === '#') { // 石头
                cnt++;
                ch = '.'; // 先把石头清空
            }
            ans[j][m - 1 - i] = ch;
            if (j === n - 1 || row[j + 1] === '*') { // 下一个格子是障碍物
                // 石头垂直掉落后，从 j 往前 cnt 个格子都是石头
                for (let k = j; k > j - cnt; k--) {
                    ans[k][m - 1 - i] = '#';
                }
                cnt = 0; // 重置计数器
            }
        }
    }

    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn rotate_the_box(box_grid: Vec<Vec<char>>) -> Vec<Vec<char>> {
        let m = box_grid.len();
        let n = box_grid[0].len();
        let mut ans = vec![vec!['\0'; m]; n];

        for (i, row) in box_grid.iter().enumerate() {
            let mut cnt = 0;
            for (j, &ch) in row.into_iter().enumerate() {
                let mut ch = ch;
                if ch == '#' { // 石头
                    cnt += 1;
                    ch = '.'; // 先把石头清空
                }
                ans[j][m - 1 - i] = ch;
                if j == n - 1 || row[j + 1] == '*' { // 下一个格子是障碍物
                    // 石头垂直掉落后，从 j 往前 cnt 个格子都是石头
                    for k in j - cnt + 1..=j {
                        ans[k][m - 1 - i] = '#';
                    }
                    cnt = 0; // 重置计数器
                }
            }
        }

        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(mn)$，其中 $m$ 和 $n$ 分别是 $\textit{boxGrid}$ 的行数和列数。
- 空间复杂度：$\mathcal{O}(1)$。返回值不计入。

## 方法二：倒序遍历 + 双指针

对于每一行 $\textit{row}$，倒着遍历，我们可以直接确定每个石头落入的位置：

- 如果 $\textit{row}[j]$ 是障碍物，那么它左边最近的石头，在旋转后掉落到 $\textit{row}[j-1]$。我们用一个变量 $k$ 维护石头掉落后的位置，如果 $\textit{row}[j]$ 是障碍物，那么更新 $\textit{k} = j-1$。注：如果 $\textit{row}[j]$ 左边最近的不是石头而是障碍物，那么 $k$ 会继续更新，无需担心石头落到错误的位置。
- 如果 $\textit{row}[j]$ 是石头，那么它掉落到 $\textit{row}[\textit{k}]$。然后把 $k$ 减一，表示左边下一块石头掉落后的位置。

```py [sol-Python3]
class Solution:
    def rotateTheBox(self, boxGrid: list[list[str]]) -> list[list[str]]:
        m, n = len(boxGrid), len(boxGrid[0])
        ans = [['.'] * m for _ in range(n)]

        for i, row in enumerate(boxGrid):
            k = n - 1
            for j in range(n - 1, -1, -1):
                if row[j] == '*':  # 障碍物
                    ans[j][-1 - i] = '*'
                    k = j - 1  # 障碍物左边最近的石头，在旋转后掉落到 j-1
                elif row[j] == '#':  # 石头
                    ans[k][-1 - i] = '#'  # 旋转后，石头掉落到 k
                    k -= 1

        return ans
```

```java [sol-Java]
class Solution {
    public char[][] rotateTheBox(char[][] boxGrid) {
        int m = boxGrid.length;
        int n = boxGrid[0].length;
        char[][] ans = new char[n][m];
        for (char[] row : ans) {
            Arrays.fill(row, '.');
        }

        for (int i = 0; i < m; i++) {
            char[] row = boxGrid[i];
            int k = n - 1;
            for (int j = n - 1; j >= 0; j--) {
                if (row[j] == '*') { // 障碍物
                    ans[j][m - 1 - i] = '*';
                    k = j - 1; // 障碍物左边最近的石头，在旋转后掉落到 j-1
                } else if (row[j] == '#') { // 石头
                    ans[k][m - 1 - i] = '#'; // 旋转后，石头掉落到 k
                    k--;
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
    vector<vector<char>> rotateTheBox(vector<vector<char>>& boxGrid) {
        int m = boxGrid.size(), n = boxGrid[0].size();
        vector ans(n, vector<char>(m, '.'));

        for (int i = 0; i < m; i++) {
            auto& row = boxGrid[i];
            int k = n - 1;
            for (int j = n - 1; j >= 0; j--) {
                if (row[j] == '*') { // 障碍物
                    ans[j][m - 1 - i] = '*';
                    k = j - 1; // 障碍物左边最近的石头，在旋转后掉落到 j-1
                } else if (row[j] == '#') { // 石头
                    ans[k][m - 1 - i] = '#'; // 旋转后，石头掉落到 k
                    k--;
                }
            }
        }

        return ans;
    }
};
```

```c [sol-C]
char** rotateTheBox(char** boxGrid, int boxGridSize, int* boxGridColSize, int* returnSize, int** returnColumnSizes) {
    int m = boxGridSize, n = boxGridColSize[0];
    char** ans = malloc(n * sizeof(char*));
    *returnColumnSizes = malloc(n * sizeof(int));
    *returnSize = n;
    for (int i = 0; i < n; i++) {
        ans[i] = malloc(m * sizeof(char));
        memset(ans[i], '.', m * sizeof(char));
        (*returnColumnSizes)[i] = m;
    }

    for (int i = 0; i < m; i++) {
        char* row = boxGrid[i];
        int k = n - 1;
        for (int j = n - 1; j >= 0; j--) {
            if (row[j] == '*') { // 障碍物
                ans[j][m - 1 - i] = '*';
                k = j - 1; // 障碍物左边最近的石头，在旋转后掉落到 j-1
            } else if (row[j] == '#') { // 石头
                ans[k][m - 1 - i] = '#'; // 旋转后，石头掉落到 k
                k--;
            }
        }
    }

    return ans;
}
```

```go [sol-Go]
func rotateTheBox(boxGrid [][]byte) [][]byte {
	m, n := len(boxGrid), len(boxGrid[0])
	ans := make([][]byte, n)
	for i := range ans {
		ans[i] = bytes.Repeat([]byte{'.'}, m)
	}

	for i, row := range boxGrid {
		k := n - 1
		for j := n - 1; j >= 0; j-- {
			if row[j] == '*' { // 障碍物
				ans[j][m-1-i] = '*'
				k = j - 1 // 障碍物左边最近的石头，在旋转后掉落到 j-1
			} else if row[j] == '#' { // 石头
				ans[k][m-1-i] = '#' // 旋转后，石头掉落到 k
				k--
			}
		}
	}

	return ans
}
```

```js [sol-JavaScript]
var rotateTheBox = function(boxGrid) {
    const m = boxGrid.length, n = boxGrid[0].length;
    const ans = Array.from({ length: n }, () => Array(m).fill('.'));

    for (let i = 0; i < m; i++) {
        const row = boxGrid[i];
        let k = n - 1;
        for (let j = n - 1; j >= 0; j--) {
            if (row[j] === '*') { // 障碍物
                ans[j][m - 1 - i] = '*';
                k = j - 1; // 障碍物左边最近的石头，在旋转后掉落到 j-1
            } else if (row[j] === '#') { // 石头
                ans[k][m - 1 - i] = '#'; // 旋转后，石头掉落到 k
                k--;
            }
        }
    }

    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn rotate_the_box(box_grid: Vec<Vec<char>>) -> Vec<Vec<char>> {
        let m = box_grid.len();
        let n = box_grid[0].len();
        let mut ans = vec![vec!['.'; m]; n];

        for (i, row) in box_grid.into_iter().enumerate() {
            let mut k = n - 1;
            for (j, ch) in row.into_iter().enumerate().rev() {
                if ch == '*' { // 障碍物
                    ans[j][m - 1 - i] = '*';
                    k = j - 1; // 障碍物左边最近的石头，在旋转后掉落到 j-1
                } else if ch == '#' { // 石头
                    ans[k][m - 1 - i] = '#'; // 旋转后，石头掉落到 k
                    k -= 1;
                }
            }
        }

        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(mn)$，其中 $m$ 和 $n$ 分别是 $\textit{boxGrid}$ 的行数和列数。
- 空间复杂度：$\mathcal{O}(1)$。返回值不计入。

## 专题训练

见下面双指针题单的「**六、分组循环**」。

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

## 本地原创解析

### 1. 题意重述

本题来自 `三、单序列双指针 / §3.2 相向双指针`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

### 2. 暴力思路与瓶颈

直接枚举所有候选并逐个重新计算属性，通常会产生 $O(nk)$、$O(n^2)$ 或更高复杂度。瓶颈在于相邻候选之间有大量重复计算。

### 3. 关键观察

相邻状态通常只差少量元素或一个转移边界。只要把重复计算沉淀为可增量维护的统计量、单调结构、状态转移或图搜索标记，就能显著降低复杂度。

### 4. 算法设计

1. 根据题目约束确定窗口、前缀、二分、栈、图搜索、动态规划或数学变换的核心状态。
2. 初始化边界状态。
3. 按来源分类的套路推进枚举或转移，并在状态合法时更新答案。
4. 对边界不足、空状态、重复元素、负数、溢出、取模和不可达状态单独处理。

### 5. 正确性说明

枚举或转移过程覆盖所有合法候选；维护量在每一步与当前候选状态保持一致；答案只在候选合法或状态最优性成立时更新，因此最终结果等于所有合法候选的最优值、计数或可行性判断。

### 6. 复杂度分析

- 时间复杂度：依据具体题解正文确认；常见为 $O(n)$、$O(n\log n)$、$O(nm)$ 或状态数乘转移数。
- 空间复杂度：依据维护状态确认；常见为 $O(1)$、$O(k)$、$O(n)$ 或 DP/图状态规模。

### 7. C++17 实现

```cpp
#include <bits/stdc++.h>
using namespace std;

class Solution {
public:
    // TODO: 根据题目签名补全。当前批次先建立详解结构，代码需按题面签名复核。
};
```

### 8. 样例推演

当前本地层不复制题面样例。导入授权题解正文后，应结合正文中的示例或手工构造小样例，列出状态变化和答案更新时机。

### 9. 易错点

- 更新答案前必须确认当前状态已经合法。
- 删除、回退或转移状态时不要漏更新计数、和、频率表、访问标记或单调结构。
- 若题目含负数、重复值、空集合、取模、长整型溢出或特殊图结构，需单独核对边界。

### 10. 扩展解析

同一分类下的题目通常共享维护框架，差异主要在状态定义和合法性条件。复盘时应总结“状态是什么、何时合法、如何转移、答案如何更新”。

### 11. 同类题迁移

回到来源分类 `三、单序列双指针 / §3.2 相向双指针`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
