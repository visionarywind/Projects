# 1975. 最大方阵和

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-matrix-sum/
- 题目 slug：`maximum-matrix-sum`
- 来源专题：贪心与思维
- 来源分类路径：五、思维题 / §5.2 脑筋急转弯
- 难度分：1648
- 外部题解来源：https://leetcode.cn/problems/maximum-matrix-sum/solutions/950421/go-tan-xin-by-endlesscheng-opwm/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[脑筋急转弯 + 分类讨论（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/maximum-matrix-sum/solutions/950421/go-tan-xin-by-endlesscheng-opwm/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`go-tan-xin-by-endlesscheng-opwm`
- topic id：`950421`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:07:12 +0800

虽然题目规定我们只能操作相邻的元素，但我们可以通过多次操作，把任意两个元素都乘以 $-1$。

![lc1975c.png](https://pic.leetcode.cn/1766921852-WPYsWd-lc1975c.png)

每次操作，恰好改变两个数的正负号。

多次操作，恰好改变偶数个数的正负号。

分类讨论：

- 如果 $\textit{matrix}$ 有偶数个负数，那么可以把所有数都变成非负数。
- 如果 $\textit{matrix}$ 有奇数个负数且没有 $0$，那么最终必然剩下奇数个负数。剩下一个负数是最优的。贪心地，选择 $\textit{matrix}$ 中的绝对值最小的数，给它加上负号。
- 如果 $\textit{matrix}$ 有奇数个负数且有 $0$，那么可以把一个 $0$ 和最终剩下的一个负数操作一次，从而把所有数都变成非负数。

代码实现时，无需特判是否有 $0$。如果有 $0$，那么代码中的 $\textit{mn}=0$，对 $\textit{total}$ 无影响。

```py [sol-Python3]
class Solution:
    def maxMatrixSum(self, matrix: List[List[int]]) -> int:
        total = neg_cnt = 0
        mn = inf
        for row in matrix:
            for x in row:
                if x < 0:
                    neg_cnt += 1
                    x = -x  # 先把负数都变成正数
                mn = min(mn, x)
                total += x

        if neg_cnt % 2:
            total -= mn * 2  # 给绝对值最小的数添加负号
        return total
```

```java [sol-Java]
class Solution {
    public long maxMatrixSum(int[][] matrix) {
        long total = 0;
        int negCnt = 0;
        int mn = Integer.MAX_VALUE;
        for (int[] row : matrix) {
            for (int x : row) {
                if (x < 0) {
                    negCnt++;
                    x = -x; // 先把负数都变成正数
                }
                mn = Math.min(mn, x);
                total += x;
            }
        }

        if (negCnt % 2 > 0) {
            total -= mn * 2; // 给绝对值最小的数添加负号
        }
        return total;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long maxMatrixSum(vector<vector<int>>& matrix) {
        long long total = 0;
        int neg_cnt = 0;
        int mn = INT_MAX;
        for (auto& row : matrix) {
            for (int x : row) {
                if (x < 0) {
                    neg_cnt++;
                    x = -x; // 先把负数都变成正数
                }
                mn = min(mn, x);
                total += x;
            }
        }

        if (neg_cnt % 2) {
            total -= mn * 2; // 给绝对值最小的数添加负号
        }
        return total;
    }
};
```

```c [sol-C]
#define MIN(a, b) ((b) < (a) ? (b) : (a))

long long maxMatrixSum(int** matrix, int matrixSize, int* matrixColSize) {
    long long total = 0;
    int neg_cnt = 0;
    int mn = INT_MAX;
    for (int i = 0; i < matrixSize; i++) {
        for (int j = 0; j < matrixColSize[i]; j++) {
            int x = matrix[i][j];
            if (x < 0) {
                neg_cnt++;
                x = -x; // 先把负数都变成正数
            }
            mn = MIN(mn, x);
            total += x;
        }
    }

    if (neg_cnt % 2) {
        total -= mn * 2; // 给绝对值最小的数添加负号
    }
    return total;
}
```

```go [sol-Go]
func maxMatrixSum(matrix [][]int) int64 {
	total, negCnt, mn := 0, 0, math.MaxInt
	for _, row := range matrix {
		for _, x := range row {
			if x < 0 {
				negCnt++
				x = -x // 先把负数都变成正数
			}
			mn = min(mn, x)
			total += x
		}
	}

	if negCnt%2 > 0 {
		total -= mn * 2 // 给绝对值最小的数添加负号
	}
	return int64(total)
}
```

```js [sol-JavaScript]
var maxMatrixSum = function(matrix) {
    let total = 0;
    let negCnt = 0;
    let mn = Infinity;
    for (const row of matrix) {
        for (let x of row) {
            if (x < 0) {
                negCnt++;
                x = -x; // 先把负数都变成正数
            }
            mn = Math.min(mn, x);
            total += x;
        }
    }

    if (negCnt % 2) {
        total -= mn * 2; // 给绝对值最小的数添加负号
    }
    return total;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn max_matrix_sum(matrix: Vec<Vec<i32>>) -> i64 {
        let mut total = 0;
        let mut neg_cnt = 0;
        let mut mn = i32::MAX;
        for row in matrix {
            for mut x in row {
                if x < 0 {
                    neg_cnt += 1;
                    x = -x; // 先把负数都变成正数
                }
                mn = mn.min(x);
                total += x as i64;
            }
        }

        if neg_cnt % 2 > 0 {
            total -= (mn * 2) as i64; // 给绝对值最小的数添加负号
        }
        total
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(mn)$，其中 $m$ 和 $n$ 分别是 $\textit{matrix}$ 的行数和列数。
- 空间复杂度：$\mathcal{O}(1)$。

## 专题训练

见下面贪心与思维题单的「**§5.2 脑筋急转弯**」。

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

本题来自 `五、思维题 / §5.2 脑筋急转弯`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `五、思维题 / §5.2 脑筋急转弯`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
