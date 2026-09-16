# 2267. 检查是否有合法括号字符串路径

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/check-if-there-is-a-valid-parentheses-string-path/
- 题目 slug：`check-if-there-is-a-valid-parentheses-string-path`
- 来源专题：动态规划
- 来源分类路径：二、网格图 DP / §2.2 进阶
- 难度分：2085
- 外部题解来源：https://leetcode.cn/problems/check-if-there-is-a-valid-parentheses-string-path/solutions/1477302/tian-jia-zhuang-tai-hou-dfscpythonjavago-f287/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[添加状态后 DFS + 剪枝优化（Python/Java/C++/Go）](https://leetcode.cn/problems/check-if-there-is-a-valid-parentheses-string-path/solutions/1477302/tian-jia-zhuang-tai-hou-dfscpythonjavago-f287/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tian-jia-zhuang-tai-hou-dfscpythonjavago-f287`
- topic id：`1477302`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 提示 1

用一个变量 $c$ 表示括号字符串的平衡度：遇到左括号就 $+1$，遇到右括号就 $-1$。那么合法字符串等价于任意时刻 $c\ge 0$ 且最后 $c=0$。

## 提示 2

从起点到终点，往下走的次数是固定的，即 $m-1$ 次，往右走的次数也是固定的，即 $n-1$ 次，因此路径长度（字符串长度）是一个定值，即 $(m-1)+(n-1)+1 = m+n-1$。

极限情况下合法的字符串左半均为左括号，右半均为右括号，因此 $c$ 最大为 $\dfrac{m+n-1}{2}$。

## 提示 3

把进入格子**前**的 $c$ 值当作格子的附加状态，定义状态 $(x,y,c)$ 表示进入格子 $(x,y)$ 且进入**前**平衡度为 $c$。

由于一个格子至多有 $\dfrac{m+n-1}{2}+1=\dfrac{m+n+1}{2}$ 个不同的状态，整个网格图至多有 $\dfrac{mn(m+n+1)}{2}$ 个不同的状态。

## 提示 4

在这些状态上 DFS：

- 起点为 $(0,0,0)$，表示从左上角 $(0,0)$ 出发，初始 $c=0$；
- 终点为 $(m-1,n-1,1)$，表示到右下角 $(m-1,n-1)$ 结束，且进入前 $c=1$（因为右下角必须为右括号）；
- 根据当前格子的字符计算 $c$ 值，然后往下或往右移动，继续 DFS。

代码实现时，由于找到合法路径就返回 `true` 了，不会继续执行 `dfs`，若 `dfs(x,y,c)` 最后返回的是 `false`，那后续访问同一个状态时（再次调用 `dfs(x,y,c)`），仍然会得到 `false`。因此没必要重复访问同一个状态，可以用一个 $\textit{vis}$ 数组标记，遇到访问过的状态可以直接返回 `false`。

另外有一个比较强的优化：由于字符串左括号和右括号的数目必须相同，因此字符串的长度为偶数，所以 $m+n-1$ 必须是偶数（提示 2）。我们可以在 DFS 之前就预先判断这一要求是否成立。

```py [sol-Python3]
class Solution:
    def hasValidPath(self, grid: List[List[str]]) -> bool:
        m, n = len(grid), len(grid[0])
        if (m + n) % 2 == 0 or grid[0][0] == ')' or grid[m - 1][n - 1] == '(':
            return False

        @cache  # 效果类似 vis 数组
        def dfs(x: int, y: int, c: int) -> bool:
            if c > m - x + n - y - 1:  # 剪枝：即使后面都是 ')' 也不能将 c 减为 0
                return False
            if x == m - 1 and y == n - 1:  # 终点
                return c == 1  # 终点一定是 ')'
            c += 1 if grid[x][y] == '(' else -1
            return c >= 0 and (x < m - 1 and dfs(x + 1, y, c) or y < n - 1 and dfs(x, y + 1, c))  # 往下或者往右

        return dfs(0, 0, 0)  # 起点
```

```java [sol-Java]
class Solution {
    private int m, n;
    private char[][] grid;
    private boolean[][][] vis;

    public boolean hasValidPath(char[][] grid) {
        m = grid.length;
        n = grid[0].length;
        if ((m + n) % 2 == 0 || grid[0][0] == ')' || grid[m - 1][n - 1] == '(') {
            return false;
        }

        this.grid = grid;
        vis = new boolean[m][n][(m + n + 1) / 2];
        return dfs(0, 0, 0); // 起点
    }

    private boolean dfs(int x, int y, int c) {
        if (c > m - x + n - y - 1) { // 剪枝：即使后面都是 ')' 也不能将 c 减为 0
            return false;
        }
        if (x == m - 1 && y == n - 1) { // 终点
            return c == 1; // 终点一定是 ')'
        }
        if (vis[x][y][c]) {
            return false;
        }
        vis[x][y][c] = true;
        c += grid[x][y] == '(' ? 1 : -1;
        return c >= 0 && (x < m - 1 && dfs(x + 1, y, c) || y < n - 1 && dfs(x, y + 1, c)); // 往下或者往右
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    bool hasValidPath(vector<vector<char>>& grid) {
        int m = grid.size(), n = grid[0].size();
        if ((m + n) % 2 == 0 || grid[0][0] == ')' || grid[m - 1][n - 1] == '(') {
            return false;
        }

        bool vis[m][n][(m + n + 1) / 2];
        memset(vis, 0, sizeof(vis));

        auto dfs = [&](this auto&& dfs, int x, int y, int c) -> bool {
            if (c > m - x + n - y - 1) { // 剪枝：即使后面都是 ')' 也不能将 c 减为 0
                return false;
            }
            if (x == m - 1 && y == n - 1) { // 终点
                return c == 1; // 终点一定是 ')'
            }
            if (vis[x][y][c]) {
                return false;
            }
            vis[x][y][c] = true;
            c += grid[x][y] == '(' ? 1 : -1;
            return c >= 0 && (x < m - 1 && dfs(x + 1, y, c) || y < n - 1 && dfs(x, y + 1, c)); // 往下或者往右
        };

        return dfs(0, 0, 0); // 起点
    }
};
```

```go [sol-Go]
func hasValidPath(grid [][]byte) bool {
	m, n := len(grid), len(grid[0])
	if (m+n)%2 == 0 || grid[0][0] == ')' || grid[m-1][n-1] == '(' {
		return false
	}

	vis := make([][][]bool, m)
	for i := range vis {
		vis[i] = make([][]bool, n)
		for j := range vis[i] {
			vis[i][j] = make([]bool, (m+n+1)/2)
		}
	}
	var dfs func(int, int, int) bool
	dfs = func(x, y, c int) bool {
		if c > m-x+n-y-1 { // 剪枝：即使后面都是 ')' 也不能将 c 减为 0
			return false
		}
		if x == m-1 && y == n-1 { // 终点
			return c == 1 // 终点一定是 ')'
		}
		if vis[x][y][c] { // 重复访问
			return false
		}
		vis[x][y][c] = true
		if grid[x][y] == '(' {
			c++
		} else if c--; c < 0 { // 非法括号字符串
			return false
		}
		return x < m-1 && dfs(x+1, y, c) || y < n-1 && dfs(x, y+1, c) // 往下或者往右
	}
	return dfs(0, 0, 0) // 起点
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(mn(m+n))$，其中 $m$ 和 $n$ 分别为 $\textit{grid}$ 的行数和列数。每个状态至多访问一次。
- 空间复杂度：$\mathcal{O}(mn(m+n))$。

## 注

值得注意的是，DFS 的写法相比某些递推的写法要快 $10$ 倍以上，这是因为有很多状态是无法访问到的：比如 $(x=2,y=3,c=100)$ 这个状态就是不可达的，此时还没走几步，$c$ 不可能这么大。或者对于一些随机的网格图，$c$ 的值也会比较小。这种情况下 DFS 的优势就发挥出来了，DFS 可以十分自然地遍历到所有合法的状态，加上自带的剪枝效果，可以大大降低访问到的状态数。

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

## 提示 1

用一个变量 $c$ 表示括号字符串的平衡度：遇到左括号就 $+1$，遇到右括号就 $-1$。那么合法字符串等价于任意时刻 $c\ge 0$ 且最后 $c=0$。

## 提示 2

从起点到终点，往下走的次数是固定的，即 $m-1$ 次，往右走的次数也是固定的，即 $n-1$ 次，因此路径长度（字符串长度）是一个定值，即 $(m-1)+(n-1)+1 = m+n-1$。

极限情况下合法的字符串左半均为左括号，右半均为右括号，因此 $c$ 最大为 $\dfrac{m+n-1}{2}$。

## 提示 3

把进入格子**前**的 $c$ 值当作格子的附加状态，定义状态 $(x,y,c)$ 表示进入格子 $(x,y)$ 且进入**前**平衡度为 $c$。

由于一个格子至多有 $\dfrac{m+n-1}{2}+1=\dfrac{m+n+1}{2}$ 个不同的状态，整个网格图至多有 $\dfrac{mn(m+n+1)}{2}$ 个不同的状态。

## 提示 4

在这些状态上 DFS：

- 起点为 $(0,0,0)$，表示从左上角 $(0,0)$ 出发，初始 $c=0$；
- 终点为 $(m-1,n-1,1)$，表示到右下角 $(m-1,n-1)$ 结束，且进入前 $c=1$（因为右下角必须为右括号）；
- 根据当前格子的字符计算 $c$ 值，然后往下或往右移动，继续 DFS。

代码实现时，由于找到合法路径就返回 `true` 了，不会继续执行 `dfs`，若 `dfs(x,y,c)` 最后返回的是 `false`，那后续访问同一个状态时（再次调用 `dfs(x,y,c)`），仍然会得到 `false`。因此没必要重复访问同一个状态，可以用一个 $\textit{vis}$ 数组标记，遇到访问过的状态可以直接返回 `false`。

另外有一个比较强的优化：由于字符串左括号和右括号的数目必须相同，因此字符串的长度为偶数，所以 $m+n-1$ 必须是偶数（提示 2）。我们可以在 DFS 之前就预先判断这一要求是否成立。

```py [sol-Python3]
class Solution:
    def hasValidPath(self, grid: List[List[str]]) -> bool:
        m, n = len(grid), len(grid[0])
        if (m + n) % 2 == 0 or grid[0][0] == ')' or grid[m - 1][n - 1] == '(':
            return False

        @cache  # 效果类似 vis 数组
        def dfs(x: int, y: int, c: int) -> bool:
            if c > m - x + n - y - 1:  # 剪枝：即使后面都是 ')' 也不能将 c 减为 0
                return False
            if x == m - 1 and y == n - 1:  # 终点
                return c == 1  # 终点一定是 ')'
            c += 1 if grid[x][y] == '(' else -1
            return c >= 0 and (x < m - 1 and dfs(x + 1, y, c) or y < n - 1 and dfs(x, y + 1, c))  # 往下或者往右

        return dfs(0, 0, 0)  # 起点
```

```java [sol-Java]
class Solution {
    private int m, n;
    private char[][] grid;
    private boolean[][][] vis;

    public boolean hasValidPath(char[][] grid) {
        m = grid.length;
        n = grid[0].length;
        if ((m + n) % 2 == 0 || grid[0][0] == ')' || grid[m - 1][n - 1] == '(') {
            return false;
        }

        this.grid = grid;
        vis = new boolean[m][n][(m + n + 1) / 2];
        return dfs(0, 0, 0); // 起点
    }

    private boolean dfs(int x, int y, int c) {
        if (c > m - x + n - y - 1) { // 剪枝：即使后面都是 ')' 也不能将 c 减为 0
            return false;
        }
        if (x == m - 1 && y == n - 1) { // 终点
            return c == 1; // 终点一定是 ')'
        }
        if (vis[x][y][c]) {
            return false;
        }
        vis[x][y][c] = true;
        c += grid[x][y] == '(' ? 1 : -1;
        return c >= 0 && (x < m - 1 && dfs(x + 1, y, c) || y < n - 1 && dfs(x, y + 1, c)); // 往下或者往右
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    bool hasValidPath(vector<vector<char>>& grid) {
        int m = grid.size(), n = grid[0].size();
        if ((m + n) % 2 == 0 || grid[0][0] == ')' || grid[m - 1][n - 1] == '(') {
            return false;
        }

        bool vis[m][n][(m + n + 1) / 2];
        memset(vis, 0, sizeof(vis));

        auto dfs = [&](this auto&& dfs, int x, int y, int c) -> bool {
            if (c > m - x + n - y - 1) { // 剪枝：即使后面都是 ')' 也不能将 c 减为 0
                return false;
            }
            if (x == m - 1 && y == n - 1) { // 终点
                return c == 1; // 终点一定是 ')'
            }
            if (vis[x][y][c]) {
                return false;
            }
            vis[x][y][c] = true;
            c += grid[x][y] == '(' ? 1 : -1;
            return c >= 0 && (x < m - 1 && dfs(x + 1, y, c) || y < n - 1 && dfs(x, y + 1, c)); // 往下或者往右
        };

        return dfs(0, 0, 0); // 起点
    }
};
```

```go [sol-Go]
func hasValidPath(grid [][]byte) bool {
	m, n := len(grid), len(grid[0])
	if (m+n)%2 == 0 || grid[0][0] == ')' || grid[m-1][n-1] == '(' {
		return false
	}

	vis := make([][][]bool, m)
	for i := range vis {
		vis[i] = make([][]bool, n)
		for j := range vis[i] {
			vis[i][j] = make([]bool, (m+n+1)/2)
		}
	}
	var dfs func(int, int, int) bool
	dfs = func(x, y, c int) bool {
		if c > m-x+n-y-1 { // 剪枝：即使后面都是 ')' 也不能将 c 减为 0
			return false
		}
		if x == m-1 && y == n-1 { // 终点
			return c == 1 // 终点一定是 ')'
		}
		if vis[x][y][c] { // 重复访问
			return false
		}
		vis[x][y][c] = true
		if grid[x][y] == '(' {
			c++
		} else if c--; c < 0 { // 非法括号字符串
			return false
		}
		return x < m-1 && dfs(x+1, y, c) || y < n-1 && dfs(x, y+1, c) // 往下或者往右
	}
	return dfs(0, 0, 0) // 起点
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(mn(m+n))$，其中 $m$ 和 $n$ 分别为 $\textit{grid}$ 的行数和列数。每个状态至多访问一次。
- 空间复杂度：$\mathcal{O}(mn(m+n))$。

## 注

值得注意的是，DFS 的写法相比某些递推的写法要快 $10$ 倍以上，这是因为有很多状态是无法访问到的：比如 $(x=2,y=3,c=100)$ 这个状态就是不可达的，此时还没走几步，$c$ 不可能这么大。或者对于一些随机的网格图，$c$ 的值也会比较小。这种情况下 DFS 的优势就发挥出来了，DFS 可以十分自然地遍历到所有合法的状态，加上自带的剪枝效果，可以大大降低访问到的状态数。

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

## 本地原创解析

### 1. 题意重述

本题来自 `二、网格图 DP / §2.2 进阶`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、网格图 DP / §2.2 进阶`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
