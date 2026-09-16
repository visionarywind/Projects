# 2906. 构造乘积矩阵

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/construct-product-matrix/
- 题目 slug：`construct-product-matrix`
- 来源专题：动态规划
- 来源分类路径：专题：前后缀分解
- 难度分：2075
- 外部题解来源：https://leetcode.cn/problems/construct-product-matrix/solutions/2483137/zhou-sai-chang-kao-qian-hou-zhui-fen-jie-21hr/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[前后缀分解（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/construct-product-matrix/solutions/2483137/zhou-sai-chang-kao-qian-hou-zhui-fen-jie-21hr/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`zhou-sai-chang-kao-qian-hou-zhui-fen-jie-21hr`
- topic id：`2483137`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

请先完成本题的**一维版本**：[238. 除了自身以外数组的乘积](https://leetcode.cn/problems/product-of-array-except-self/)。

把矩阵平铺成一维数组，就是 238 题了。我们需要算出每个数左边所有数的乘积，以及右边所有数的乘积。

先算出从 $\textit{grid}[i][j]$ 的下一个元素开始，到最后一个元素 $\textit{grid}[n-1][m-1]$ 的乘积，记作 $\textit{suf}[i][j]$。这可以从最后一个数 $\textit{grid}[n-1][m-1]$ 开始，倒着遍历 $\textit{grid}$ 得到。

然后算出从第一个数 $\textit{grid}[0][0]$ 开始，到 $\textit{grid}[i][j]$ 的上一个元素的乘积，记作 $\textit{pre}[i][j]$。这可以从第一行第一列开始，正着遍历得到。

那么

$$
p[i][j] = \textit{pre}[i][j]\cdot \textit{suf}[i][j]
$$

代码实现时，可以先初始化 $p[i][j]=\textit{suf}[i][j]$，然后在计算 $\textit{pre}[i][j]$ 的过程中，把 $\textit{pre}[i][j]$ 乘到 $\textit{p}[i][j]$ 中，就得到了最终答案。这样写的话，$\textit{pre}$ 和 $\textit{suf}$ 可以直接用单个变量表示，无需创建数组。

代码实现时，注意取模。为什么可以在**中途取模**？原理见 [模运算的世界：当加减乘除遇上取模](https://leetcode.cn/circle/discuss/mDfnkW/)。

```py [sol-Python3]
class Solution:
    def constructProductMatrix(self, grid: List[List[int]]) -> List[List[int]]:
        MOD = 12345
        n, m = len(grid), len(grid[0])
        p = [[0] * m for _ in range(n)]

        suf = 1  # 后缀乘积
        for i in range(n - 1, -1, -1):
            for j in range(m - 1, -1, -1):
                p[i][j] = suf  # p[i][j] 先初始化成后缀乘积
                suf = suf * grid[i][j] % MOD

        pre = 1  # 前缀乘积
        for i, row in enumerate(grid):
            for j, x in enumerate(row):
                p[i][j] = p[i][j] * pre % MOD  # 乘上前缀乘积
                pre = pre * x % MOD

        return p
```

```java [sol-Java]
class Solution {
    public int[][] constructProductMatrix(int[][] grid) {
        final int MOD = 12345;
        int n = grid.length;
        int m = grid[0].length;
        int[][] p = new int[n][m];

        long suf = 1; // 后缀乘积
        for (int i = n - 1; i >= 0; i--) {
            for (int j = m - 1; j >= 0; j--) {
                p[i][j] = (int) suf; // p[i][j] 先初始化成后缀乘积
                suf = suf * grid[i][j] % MOD;
            }
        }

        long pre = 1; // 前缀乘积
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < m; j++) {
                p[i][j] = (int) (p[i][j] * pre % MOD); // 乘上前缀乘积
                pre = pre * grid[i][j] % MOD;
            }
        }

        return p;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<vector<int>> constructProductMatrix(vector<vector<int>>& grid) {
        constexpr int MOD = 12345;
        int n = grid.size(), m = grid[0].size();
        vector p(n, vector<int>(m));

        long long suf = 1; // 后缀乘积
        for (int i = n - 1; i >= 0; i--) {
            for (int j = m - 1; j >= 0; j--) {
                p[i][j] = suf; // p[i][j] 先初始化成后缀乘积
                suf = suf * grid[i][j] % MOD;
            }
        }

        long long pre = 1; // 前缀乘积
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < m; j++) {
                p[i][j] = p[i][j] * pre % MOD; // 乘上前缀乘积
                pre = pre * grid[i][j] % MOD;
            }
        }

        return p;
    }
};
```

```c [sol-C]
int** constructProductMatrix(int** grid, int gridSize, int* gridColSize, int* returnSize, int** returnColumnSizes) {
    const int MOD = 12345;
    int n = gridSize, m = gridColSize[0];
    int** p = malloc(n * sizeof(int*));
    *returnSize = n;
    *returnColumnSizes = malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) {
        p[i] = malloc(m * sizeof(int));
        (*returnColumnSizes)[i] = m;
    }

    long long suf = 1; // 后缀乘积
    for (int i = n - 1; i >= 0; i--) {
        for (int j = m - 1; j >= 0; j--) {
            p[i][j] = suf; // p[i][j] 先初始化成后缀乘积
            suf = suf * grid[i][j] % MOD;
        }
    }

    long long pre = 1; // 前缀乘积
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            p[i][j] = p[i][j] * pre % MOD; // 乘上前缀乘积
            pre = pre * grid[i][j] % MOD;
        }
    }

    return p;
}
```

```go [sol-Go]
func constructProductMatrix(grid [][]int) [][]int {
	const mod = 12345
	n, m := len(grid), len(grid[0])
	p := make([][]int, n)
	suf := 1 // 后缀乘积
	for i := n - 1; i >= 0; i-- {
		p[i] = make([]int, m)
		for j := m - 1; j >= 0; j-- {
			p[i][j] = suf // p[i][j] 先初始化成后缀乘积
			suf = suf * grid[i][j] % mod
		}
	}

	pre := 1 // 前缀乘积
	for i, row := range grid {
		for j, x := range row {
			p[i][j] = p[i][j] * pre % mod // 乘上前缀乘积
			pre = pre * x % mod
		}
	}
	return p
}
```

```js [sol-JavaScript]
var constructProductMatrix = function(grid) {
    const MOD = 12345;
    const n = grid.length, m = grid[0].length;
    const p = Array.from({ length: n }, () => Array(m).fill(0));

    let suf = 1; // 后缀乘积
    for (let i = n - 1; i >= 0; i--) {
        for (let j = m - 1; j >= 0; j--) {
            p[i][j] = suf; // p[i][j] 先初始化成后缀乘积
            suf = suf * grid[i][j] % MOD;
        }
    }

    let pre = 1; // 前缀乘积
    for (let i = 0; i < n; i++) {
        for (let j = 0; j < m; j++) {
            p[i][j] = p[i][j] * pre % MOD; // 乘上前缀乘积
            pre = pre * grid[i][j] % MOD;
        }
    }

    return p;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn construct_product_matrix(grid: Vec<Vec<i32>>) -> Vec<Vec<i32>> {
        const MOD: i64 = 12345;
        let n = grid.len();
        let m = grid[0].len();
        let mut p = vec![vec![0; m]; n];

        let mut suf = 1; // 后缀乘积
        for i in (0..n).rev() {
            for j in (0..m).rev() {
                p[i][j] = suf as i32; // p[i][j] 先初始化成后缀乘积
                suf = suf * grid[i][j] as i64 % MOD;
            }
        }

        let mut pre = 1; // 前缀乘积
        for i in 0..n {
            for j in 0..m {
                p[i][j] = (p[i][j] as i64 * pre % MOD) as i32; // 乘上前缀乘积
                pre = pre * grid[i][j] as i64 % MOD;
            }
        }

        p
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(nm)$，其中 $n$ 和 $m$ 分别是 $\textit{grid}$ 的行数和列数。
- 空间复杂度：$\mathcal{O}(1)$。返回值不计入。

## 专题训练

见下面动态规划题单的「**专题：前后缀分解**」。

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

本题来自 `专题：前后缀分解`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `专题：前后缀分解`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
