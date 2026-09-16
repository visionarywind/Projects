# 2312. 卖木头块

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/selling-pieces-of-wood/
- 题目 slug：`selling-pieces-of-wood`
- 来源专题：动态规划
- 来源分类路径：七、其他线性 DP / §7.6 多维 DP
- 难度分：2363
- 外部题解来源：https://leetcode.cn/problems/selling-pieces-of-wood/solutions/1611240/by-endlesscheng-mrmd/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[动态规划：枚举切割位置+循环优化（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/selling-pieces-of-wood/solutions/1611240/by-endlesscheng-mrmd/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`by-endlesscheng-mrmd`
- topic id：`1611240`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 寻找子问题

看示例 1，对于一个高为 $3$ 宽为 $5$ 的木块，**第一步**一共有 $6$ 种切割方案：

- 竖着切开，有 $4$ 种切法。
- 横着切开，有 $2$ 种切法。

比如横着切开，第一步可以分成一个高为 $2$ 宽为 $5$ 的木块，和一个高为 $1$ 宽为 $5$ 的木块。

这俩都是更小的木块，可以分别处理，接着切割（比如第一个横切，第二个竖切），这意味着我们要处理的问题都是「高为 $i$ 宽为 $j$ 的木块」。

## 状态定义

定义 $f[i][j]$ 表示切割一块高 $i$ 宽 $j$ 的木块，能得到的最多钱数。

分类讨论：

- 如果直接售卖，则收益为对应的 $\textit{price}$（如果存在的话）。
- 如果竖着切开，枚举切割位置（宽度）$k$，得到两个高为 $i$，宽分别为 $k$ 和 $j-k$ 的木块，最大收益为
    $$
    \max_{k=1}^{j-1} f[i][k]+f[i][j-k]
    $$
- 如果横着切开，枚举切割位置（高度）$k$，得到两个宽为 $j$，高分别为 $k$ 和 $i-k$ 的木块，最大收益为
    $$
    \max_{k=1}^{i-1} f[k][j]+f[i-k][j]
    $$

取上述三种情况的最大值，即为 $f[i][j]$。

答案：$f[m][n]$。

代码实现时，为了方便查询木块价格，可以用一个哈希表或数组记录高宽对应的木块价格。当然，这个做法是不必要的，后面会优化。

```Python [sol-Python3]
class Solution:
    def sellingWood(self, m: int, n: int, prices: List[List[int]]) -> int:
        pr = {(h, w): p for h, w, p in prices}
        f = [[0] * (n + 1) for _ in range(m + 1)]
        for i in range(1, m + 1):
            for j in range(1, n + 1):
                f[i][j] = max(pr.get((i, j), 0),
                              max((f[i][k] + f[i][j - k] for k in range(1, j)), default=0),  # 垂直切割
                              max((f[k][j] + f[i - k][j] for k in range(1, i)), default=0))  # 水平切割
        return f[m][n]
```

```java [sol-Java]
class Solution {
    public long sellingWood(int m, int n, int[][] prices) {
        int[][] pr = new int[m + 1][n + 1];
        for (int[] p : prices) {
            pr[p[0]][p[1]] = p[2];
        }

        long[][] f = new long[m + 1][n + 1];
        for (int i = 1; i <= m; i++) {
            for (int j = 1; j <= n; j++) {
                f[i][j] = pr[i][j];
                for (int k = 1; k < j; k++) f[i][j] = Math.max(f[i][j], f[i][k] + f[i][j - k]); // 垂直切割
                for (int k = 1; k < i; k++) f[i][j] = Math.max(f[i][j], f[k][j] + f[i - k][j]); // 水平切割
            }
        }
        return f[m][n];
    }
}
```

```C++ [sol-C++]
class Solution {
public:
    long long sellingWood(int m, int n, vector<vector<int>> &prices) {
        vector<vector<int>> pr(m + 1, vector<int>(n + 1));
        for (auto &p: prices) {
            pr[p[0]][p[1]] = p[2];
        }

        vector<vector<long long>> f(m + 1, vector<long long>(n + 1));
        for (int i = 1; i <= m; i++) {
            for (int j = 1; j <= n; j++) {
                f[i][j] = pr[i][j];
                for (int k = 1; k < j; k++) f[i][j] = max(f[i][j], f[i][k] + f[i][j - k]); // 垂直切割
                for (int k = 1; k < i; k++) f[i][j] = max(f[i][j], f[k][j] + f[i - k][j]); // 水平切割
            }
        }
        return f[m][n];
    }
};
```

```go [sol-Go]
func sellingWood(m, n int, prices [][]int) int64 {
	pr := make([][]int, m+1)
	for i := range pr {
		pr[i] = make([]int, n+1)
	}
	for _, price := range prices {
		pr[price[0]][price[1]] = price[2]
	}

	f := make([][]int64, m+1)
	for i := 1; i <= m; i++ {
		f[i] = make([]int64, n+1)
		for j := 1; j <= n; j++ {
			f[i][j] = int64(pr[i][j])
			for k := 1; k < j; k++ { // 垂直切割，枚举宽度 k
				f[i][j] = max(f[i][j], f[i][k]+f[i][j-k])
			}
			for k := 1; k < i; k++ { // 水平切割，枚举高度 k
				f[i][j] = max(f[i][j], f[k][j]+f[i-k][j])
			}
		}
	}
	return f[m][n]
}
```

```js [sol-JavaScript]
var sellingWood = function(m, n, prices) {
    const pr = Array.from({length: m + 1}, () => Array(n + 1).fill(0));
    for (const [w, h, p] of prices) {
        pr[w][h] = p;
    }

    const f = Array.from({length: m + 1}, () => Array(n + 1).fill(0));
    for (let i = 1; i <= m; i++) {
        for (let j = 1; j <= n; j++) {
            f[i][j] = pr[i][j];
            for (let k = 1; k < j; k++) f[i][j] = Math.max(f[i][j], f[i][k] + f[i][j - k]); // 垂直切割
            for (let k = 1; k < i; k++) f[i][j] = Math.max(f[i][j], f[k][j] + f[i - k][j]); // 水平切割
        }
    }
    return f[m][n];
};
```

```rust [sol-Rust]
impl Solution {
    pub fn selling_wood(m: i32, n: i32, prices: Vec<Vec<i32>>) -> i64 {
        let m = m as usize;
        let n = n as usize;
        let mut pr = vec![vec![0; n + 1]; m + 1];
        for p in &prices {
            pr[p[0] as usize][p[1] as usize] = p[2];
        }

        let mut f = vec![vec![0; n + 1]; m + 1];
        for i in 1..=m {
            for j in 1..=n {
                f[i][j] = pr[i][j] as i64;
                for k in 1..j { // 垂直切割，枚举宽度 k
                    f[i][j] = f[i][j].max(f[i][k] + f[i][j - k]);
                }
                for k in 1..i { // 水平切割，枚举高度 k
                    f[i][j] = f[i][j].max(f[k][j] + f[i - k][j]);
                }
            }
        }
        f[m][n]
    }
}
```

## 优化

回顾一下，对于一个高为 $3$ 宽为 $5$ 的木块，**第一步**一共有 $6$ 种切割方案：

- 竖着切开，有 $4$ 种切法。
- 横着切开，有 $2$ 种切法。

但实际上，横着切开，虽然位置不同，但得到的结果是相同的，即一个高为 $2$ 宽为 $5$ 的木块，和一个高为 $1$ 宽为 $5$ 的木块，所以本质上只有 $1$ 种切法。对于竖切也同理，本质上只有 $2$ 种切法。

因此，枚举 $k$ 的时候，只需要枚举到一半的位置，宽度至多为 $\left\lfloor\dfrac{j}{2}\right\rfloor$，高度至多为 $\left\lfloor\dfrac{i}{2}\right\rfloor$。

此外，在计算递推之前，可以直接将 $\textit{prices}$ 记录到 $f$ 数组中。

```Python [sol-Python3]
class Solution:
    def sellingWood(self, m: int, n: int, prices: List[List[int]]) -> int:
        f = [[0] * (n + 1) for _ in range(m + 1)]
        for h, w, p in prices:
            f[h][w] = p
        for i in range(1, m + 1):
            for j in range(1, n + 1):
                f[i][j] = max(f[i][j],
                              max((f[i][k] + f[i][j - k] for k in range(1, j // 2 + 1)), default=0),  # 垂直切割
                              max((f[k][j] + f[i - k][j] for k in range(1, i // 2 + 1)), default=0))  # 水平切割
        return f[m][n]
```

```java [sol-Java]
class Solution {
    public long sellingWood(int m, int n, int[][] prices) {
        long[][] f = new long[m + 1][n + 1];
        for (int[] p : prices) {
            f[p[0]][p[1]] = p[2];
        }
        for (int i = 1; i <= m; i++) {
            for (int j = 1; j <= n; j++) {
                for (int k = 1; k <= j / 2; k++) f[i][j] = Math.max(f[i][j], f[i][k] + f[i][j - k]); // 垂直切割
                for (int k = 1; k <= i / 2; k++) f[i][j] = Math.max(f[i][j], f[k][j] + f[i - k][j]); // 水平切割
            }
        }
        return f[m][n];
    }
}
```

```C++ [sol-C++]
class Solution {
public:
    long long sellingWood(int m, int n, vector<vector<int>> &prices) {
        vector<vector<long long>> f(m + 1, vector<long long>(n + 1));
        for (auto &p : prices) {
            f[p[0]][p[1]] = p[2];
        }
        for (int i = 1; i <= m; i++) {
            for (int j = 1; j <= n; j++) {
                for (int k = 1; k <= j / 2; k++) f[i][j] = max(f[i][j], f[i][k] + f[i][j - k]); // 垂直切割
                for (int k = 1; k <= i / 2; k++) f[i][j] = max(f[i][j], f[k][j] + f[i - k][j]); // 水平切割
            }
        }
        return f[m][n];
    }
};
```

```go [sol-Go]
func sellingWood(m, n int, prices [][]int) int64 {
	f := make([][]int64, m+1)
	for i := range f {
		f[i] = make([]int64, n+1)
	}
	for _, price := range prices {
		f[price[0]][price[1]] = int64(price[2])
	}
	for i := 1; i <= m; i++ {
		for j := 1; j <= n; j++ {
			for k := 1; k <= j/2; k++ { // 垂直切割，枚举宽度 k
				f[i][j] = max(f[i][j], f[i][k]+f[i][j-k])
			}
			for k := 1; k <= i/2; k++ { // 水平切割，枚举高度 k
				f[i][j] = max(f[i][j], f[k][j]+f[i-k][j])
			}
		}
	}
	return f[m][n]
}
```

```js [sol-JavaScript]
var sellingWood = function(m, n, prices) {
    const f = Array.from({length: m + 1}, () => Array(n + 1).fill(0));
    for (const [w, h, p] of prices) {
        f[w][h] = p;
    }
    for (let i = 1; i <= m; i++) {
        for (let j = 1; j <= n; j++) {
            for (let k = 1; k < j; k++) f[i][j] = Math.max(f[i][j], f[i][k] + f[i][j - k]); // 垂直切割
            for (let k = 1; k < i; k++) f[i][j] = Math.max(f[i][j], f[k][j] + f[i - k][j]); // 水平切割
        }
    }
    return f[m][n];
};
```

```rust [sol-Rust]
impl Solution {
    pub fn selling_wood(m: i32, n: i32, prices: Vec<Vec<i32>>) -> i64 {
        let m = m as usize;
        let n = n as usize;
        let mut f = vec![vec![0; n + 1]; m + 1];
        for p in &prices {
            f[p[0] as usize][p[1] as usize] = p[2] as i64;
        }
        for i in 1..=m {
            for j in 1..=n {
                for k in 1..j { // 垂直切割，枚举宽度 k
                    f[i][j] = f[i][j].max(f[i][k] + f[i][j - k]);
                }
                for k in 1..i { // 水平切割，枚举高度 k
                    f[i][j] = f[i][j].max(f[k][j] + f[i - k][j]);
                }
            }
        }
        f[m][n]
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(mn(m+n))$。
- 空间复杂度：$\mathcal{O}(mn)$。

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

[往期题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

## 本地原创解析

### 1. 题意重述

本题来自 `七、其他线性 DP / §7.6 多维 DP`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `七、其他线性 DP / §7.6 多维 DP`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
