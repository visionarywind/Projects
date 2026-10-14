# 1536. 排布二进制网格的最少交换次数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-swaps-to-arrange-a-binary-grid/
- 题目 slug：`minimum-swaps-to-arrange-a-binary-grid`
- 来源专题：贪心与思维
- 来源分类路径：一、贪心策略 / §1.4 从最左/最右开始贪心
- 难度分：1881
- 外部题解来源：https://leetcode.cn/problems/minimum-swaps-to-arrange-a-binary-grid/solutions/3904289/mou-pao-pai-xu-pythonjavaccgojsrust-by-e-m30j/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[冒泡排序（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/minimum-swaps-to-arrange-a-binary-grid/solutions/3904289/mou-pao-pai-xu-pythonjavaccgojsrust-by-e-m30j/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`mou-pao-pai-xu-pythonjavaccgojsrust-by-e-m30j`
- topic id：`3904289`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:54:06 +0800

设 $\textit{tailZeros}[i]$ 为 $\textit{grid}[i]$ 的尾零个数。示例 1 的 $\textit{tailZeros} = [0,1,2]$，我们要通过交换，让尾零个数变成 $[\ge 2,\ge 1,\ge 0]$。

问题等价于：

- 交换 $\textit{tailZeros}$ 中的相邻元素，使得 $\textit{tailZeros}[i]\ge n-1-i$，至少要交换多少次？

这可以贪心地做：从左到右，找第一个满足 $\textit{tailZeros}[i] \ge n-1$ 的数，交换到第一个位置；然后找第一个满足 $\textit{tailZeros}[i] \ge n-2$ 的数，交换到第二个位置；依此类推。

为什么要选最近的？假设下标 $p$ 和 $q$ 都可以换到下标 $0$。由于越靠后，$\textit{tailZeros}[i]\ge n-1-i$ 这个约束越**宽松**，所以能换到下标 $0$，也能换到后面所有的下标，所以下标 $p$ 和 $q$ 能发挥的作用是一样的。既然一样，谁离下标 $0$ 更近就选谁换过去。

由于 $\textit{tailZeros}[n-1]\ge 0$ 一定成立，所以外层循环只需循环到 $i=n-2$。

```py [sol-Python3]
class Solution:
    def minSwaps(self, grid: List[List[int]]) -> int:
        # 预处理每一行的尾零个数
        n = len(grid)
        tail_zeros = [n] * n
        for i in range(n):
            for j in range(n - 1, -1, -1):
                if grid[i][j]:
                    tail_zeros[i] = n - 1 - j
                    break

        ans = 0
        for i in range(n - 1):
            need_zeros = n - 1 - i
            for j in range(i, n):
                if tail_zeros[j] >= need_zeros:
                    ans += j - i
                    # 从 j 换到 i，原来 [i, j-1] 中的数据全体右移一位
                    tail_zeros[i + 1: j + 1] = tail_zeros[i: j]
                    break
            else:  # 没找到符合要求的 tail_zeros[j]
                return -1
        return ans
```

```java [sol-Java]
class Solution {
    public int minSwaps(int[][] grid) {
        // 预处理每一行的尾零个数
        int n = grid.length;
        int[] tailZeros = new int[n];
        for (int i = 0; i < n; i++) {
            tailZeros[i] = n;
            for (int j = n - 1; j >= 0; j--) {
                if (grid[i][j] == 1) {
                    tailZeros[i] = n - 1 - j;
                    break;
                }
            }
        }

        int ans = 0;
        next:
        for (int i = 0; i < n - 1; i++) {
            int needZeros = n - 1 - i;
            for (int j = i; j < n; j++) {
                if (tailZeros[j] >= needZeros) {
                    ans += j - i;
                    // 从 j 换到 i，原来 [i, j-1] 中的数据全体右移一位
                    System.arraycopy(tailZeros, i, tailZeros, i + 1, j - i);
                    continue next;
                }
            }
            return -1;
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minSwaps(vector<vector<int>>& grid) {
        // 预处理每一行的尾零个数
        int n = grid.size();
        vector<int> tail_zeros(n, n);
        for (int i = 0; i < n; i++) {
            for (int j = n - 1; j >= 0; j--) {
                if (grid[i][j]) {
                    tail_zeros[i] = n - 1 - j;
                    break;
                }
            }
        }

        int ans = 0;
        for (int i = 0; i < n - 1; i++) {
            int j = i;
            while (j < n && tail_zeros[j] < n - 1 - i) {
                j++;
            }
            if (j == n) {
                return -1;
            }
            ans += j - i;
            // 从 j 换到 i，原来 [i, j-1] 中的数据全体右移一位
            copy_backward(tail_zeros.begin() + i, tail_zeros.begin() + j, tail_zeros.begin() + j + 1);
        }
        return ans;
    }
};
```

```c [sol-C]
int minSwaps(int** grid, int gridSize, int* gridColSize) {
    // 预处理每一行的尾零个数
    int n = gridSize;
    int* tail_zeros = malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) {
        tail_zeros[i] = n;
        for (int j = n - 1; j >= 0; j--) {
            if (grid[i][j]) {
                tail_zeros[i] = n - 1 - j;
                break;
            }
        }
    }

    int ans = 0;
    for (int i = 0; i < n - 1; i++) {
        int j = i;
        while (j < n && tail_zeros[j] < n - 1 - i) {
            j++;
        }
        if (j == n) {
            ans = -1;
            break;
        }
        ans += j - i;
        // 从 j 换到 i，原来 [i, j-1] 中的数据全体右移一位
        memmove(tail_zeros + i + 1, tail_zeros + i, (j - i) * sizeof(int));
    }

    free(tail_zeros);
    return ans;
}
```

```go [sol-Go]
func minSwaps(grid [][]int) (ans int) {
	// 预处理每一行的尾零个数
	n := len(grid)
	tailZeros := make([]int, n)
	for i, row := range grid {
		tailZeros[i] = n
		for j := n - 1; j >= 0; j-- {
			if row[j] == 1 {
				tailZeros[i] = n - 1 - j
				break
			}
		}
	}

next:
	for i := range n - 1 {
		needZeros := n - 1 - i
		for j := i; j < n; j++ {
			if tailZeros[j] >= needZeros {
				ans += j - i
				// 从 j 换到 i，原来 [i, j-1] 中的数据全体右移一位
				copy(tailZeros[i+1:j+1], tailZeros[i:j])
				continue next
			}
		}
		return -1
	}
	return
}
```

```js [sol-JavaScript]
var minSwaps = function(grid) {
    // 预处理每一行的尾零个数
    const n = grid.length;
    const tailZeros = Array(n).fill(n);
    for (let i = 0; i < n; i++) {
        for (let j = n - 1; j >= 0; j--) {
            if (grid[i][j]) {
                tailZeros[i] = n - 1 - j;
                break;
            }
        }
    }

    let ans = 0;
    for (let i = 0; i < n - 1; i++) {
        let j = i;
        while (j < n && tailZeros[j] < n - 1 - i) {
            j++;
        }
        if (j === n) {
            return -1;
        }
        ans += j - i;
        // 从 j 换到 i，原来 [i, j-1] 中的数据全体右移一位
        tailZeros.copyWithin(i + 1, i, j);
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn min_swaps(grid: Vec<Vec<i32>>) -> i32 {
        // 预处理每一行的尾零个数
        let n = grid.len();
        let mut tail_zeros = vec![n; n];
        for (i, row) in grid.into_iter().enumerate() {
            for (j, x) in row.into_iter().enumerate().rev() {
                if x == 1 {
                    tail_zeros[i] = n - 1 - j;
                    break;
                }
            }
        }

        let mut ans = 0;
        for i in 0..n - 1 {
            let mut j = i;
            while j < n && tail_zeros[j] < n - 1 - i {
                j += 1;
            }
            if j == n {
                return -1;
            }
            ans += j - i;
            // 从 j 换到 i，原来 [i, j-1] 中的数据全体右移一位
            tail_zeros.copy_within(i..j, i + 1);
        }
        ans as _
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2)$，其中 $n$ 是 $\textit{grid}$ 的行数和列数。
- 空间复杂度：$\mathcal{O}(n)$。

## 专题训练

见下面贪心题单的「**§1.4 从最左/最右开始贪心**」。

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
