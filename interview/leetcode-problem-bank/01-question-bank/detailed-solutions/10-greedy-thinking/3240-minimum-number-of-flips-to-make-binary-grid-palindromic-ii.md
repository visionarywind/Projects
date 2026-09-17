# 3240. 最少翻转次数使二进制矩阵回文 II

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-number-of-flips-to-make-binary-grid-palindromic-ii/
- 题目 slug：`minimum-number-of-flips-to-make-binary-grid-palindromic-ii`
- 来源专题：贪心与思维
- 来源分类路径：五、思维题 / §5.8 分类讨论
- 难度分：2080
- 外部题解来源：https://leetcode.cn/problems/minimum-number-of-flips-to-make-binary-grid-palindromic-ii/solutions/2868238/fen-lei-tao-lun-pythonjavacgo-by-endless-jl6a/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[分类讨论（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/minimum-number-of-flips-to-make-binary-grid-palindromic-ii/solutions/2868238/fen-lei-tao-lun-pythonjavacgo-by-endless-jl6a/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`fen-lei-tao-lun-pythonjavacgo-by-endless-jl6a`
- topic id：`2868238`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:13:14 +0800

## 分析：行和列都是回文

为方便描述，把 $\textit{grid}$ 简称为 $a$。

由于所有行和列都必须是回文的，所以要满足

$$
a[i][j] = a[i][n-1-j] = a[m-1-i][j] = a[m-1-i][n-1-j]
$$

也就是这四个数要么都是 $0$，要么都是 $1$。其中 $0\le i < \lfloor m/2 \rfloor,\ 0\le j < \lfloor n/2 \rfloor$。

设

$$
\textit{cnt}_1 = a[i][j] + a[i][n-1-j] + a[m-1-i][j] + a[m-1-i][n-1-j]
$$

把这四个数都变成 $0$ 需要翻转 $\textit{cnt}_1$ 次，都变成 $1$ 需要翻转 $4-\textit{cnt}_1$ 次。

两种情况取最小值，把

$$
\min(\textit{cnt}_1, 4-\textit{cnt}_1)
$$

加入答案。

## 分析：1 的数目被 4 整除

分类讨论：

- 如果 $m$ 和 $n$ 都是偶数：由于上面四个数四个数一组，都翻转成了 $0$ 或者 $1$，所以 $1$ 的数目能被 $4$ 整除自动成立。
- 如果 $m$ 是奇数，$n$ 是偶数：正中间一排需要翻转成回文的，且 $1$ 的数目需要能被 $4$ 整除。
- 如果 $m$ 是偶数，$n$ 是奇数：正中间一列需要翻转成回文的，且 $1$ 的数目需要能被 $4$ 整除。
- 如果 $m$ 和 $n$ 都是奇数：正中间一排和正中间一列都需要翻转成回文的，且 $1$ 的数目需要能被 $4$ 整除。除了正中央的格子以外，每个格子都有镜像位置，这些格子两个数两个数一组，都翻转成了 $0$ 或者 $1$。那么翻转之后，不考虑正中央的格子，$1$ 的数目就是偶数。所以正中央的格子必须是 $0$，否则最终 $1$ 的数目是奇数，无法被 $4$ 整除。

如何处理正中间一排和正中间一列，是本题的重点。

## 具体计算方法

首先，如果 $m$ 和 $n$ 都是奇数，那么正中央的格子 $(\lfloor m/2 \rfloor, \lfloor n/2 \rfloor)$ 必须是 $0$，把其元素值加入答案。

然后统计正中间一排（如果 $m$ 是奇数）和正中间一列（如果 $n$ 是奇数）的格子：

- 设 $\textit{diff}$ 为镜像位置不同的数对个数。注意统计的是数对。
- 设 $\textit{cnt}_1$ 为镜像位置相同的 $1$ 的个数。注意统计的是个数。

这 $\textit{diff}$ 对 $0$ 和 $1$ 必须翻转其中一个数，所以答案至少要增加 $\textit{diff}$。什么情况下，可以只增加 $\textit{diff}$？

分类讨论：

- 如果 $\textit{cnt}_1$ 是 $4$ 的倍数，那么只需把这 $\textit{diff}$ 对 $0$ 和 $1$ 中的 $1$ 全部变成 $0$，这样 $1$ 的个数就是 $4$ 的倍数了。所以把 $\textit{diff}$ 加入答案。
- 如果 $\textit{cnt}_1$ 不是 $4$ 的倍数，由于 $\textit{cnt}_1$ 是偶数，其除以 $4$ 必定余 $2$（注意这说明 $\textit{cnt}_1 \ge 2$）。继续讨论：
    - 如果 $\textit{diff} > 0$，把其中一对数都变成 $1$，其余 $\textit{diff}-1$ 对数全部变成 $0$，这样 $1$ 的个数就是 $4$ 的倍数了。所以同样地，把 $\textit{diff}$ 加入答案。
    - 如果 $\textit{diff} = 0$，我们只能把 $\textit{cnt}_1$ 中的两个 $1$ 变成 $0$，使得 $1$ 的个数是 $4$ 的倍数。所以把答案增加 $2$。

综上所述：

- 如果 $\textit{diff} > 0$，额外把 $\textit{diff}$ 加入答案。
- 如果 $\textit{diff} = 0$，额外把 $\textit{cnt}_1\bmod 4$ 加入答案。

具体请看 [视频讲解](https://www.bilibili.com/video/BV1F4421S7XU/) 第三题，欢迎点赞关注~

```py [sol-Python3]
class Solution:
    def minFlips(self, a: List[List[int]]) -> int:
        m, n = len(a), len(a[0])
        ans = 0
        for i in range(m // 2):
            row, row2 = a[i], a[-1 - i]
            for j in range(n // 2):
                cnt1 = row[j] + row[-1 - j] + row2[j] + row2[-1 - j]
                ans += min(cnt1, 4 - cnt1)  # 全为 1 或全为 0

        if m % 2 and n % 2:
            # 正中间的数必须是 0
            ans += a[m // 2][n // 2]

        diff = cnt1 = 0
        if m % 2:
            # 统计正中间这一排
            row = a[m // 2]
            for j in range(n // 2):
                if row[j] != row[-1 - j]:
                    diff += 1
                else:
                    cnt1 += row[j] * 2
        if n % 2:
            # 统计正中间这一列
            for i in range(m // 2):
                if a[i][n // 2] != a[-1 - i][n // 2]:
                    diff += 1
                else:
                    cnt1 += a[i][n // 2] * 2

        return ans + (diff if diff else cnt1 % 4)
```

```java [sol-Java]
class Solution {
    public int minFlips(int[][] a) {
        int m = a.length;
        int n = a[0].length;
        int ans = 0;
        for (int i = 0; i < m / 2; i++) {
            for (int j = 0; j < n / 2; j++) {
                int cnt1 = a[i][j] + a[i][n - 1 - j] + a[m - 1 - i][j] + a[m - 1 - i][n - 1 - j];
                ans += Math.min(cnt1, 4 - cnt1); // 全为 1 或全为 0
            }
        }

        if (m % 2 > 0 && n % 2 > 0) {
            // 正中间的数必须是 0
            ans += a[m / 2][n / 2];
        }

        int diff = 0;
        int cnt1 = 0;
        if (m % 2 > 0) {
            // 统计正中间这一排
            for (int j = 0; j < n / 2; j++) {
                if (a[m / 2][j] != a[m / 2][n - 1 - j]) {
                    diff++;
                } else {
                    cnt1 += a[m / 2][j] * 2;
                }
            }
        }
        if (n % 2 > 0) {
            // 统计正中间这一列
            for (int i = 0; i < m / 2; i++) {
                if (a[i][n / 2] != a[m - 1 - i][n / 2]) {
                    diff++;
                } else {
                    cnt1 += a[i][n / 2] * 2;
                }
            }
        }

        return ans + (diff > 0 ? diff : cnt1 % 4);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minFlips(vector<vector<int>>& a) {
        int m = a.size(), n = a[0].size();
        int ans = 0;
        for (int i = 0; i < m / 2; i++) {
            for (int j = 0; j < n / 2; j++) {
                int cnt1 = a[i][j] + a[i][n - 1 - j] + a[m - 1 - i][j] + a[m - 1 - i][n - 1 - j];
                ans += min(cnt1, 4 - cnt1); // 全为 1 或全为 0
            }
        }

        if (m % 2 && n % 2) {
            // 正中间的数必须是 0
            ans += a[m / 2][n / 2];
        }

        int diff = 0, cnt1 = 0;
        if (m % 2) {
            // 统计正中间这一排
            for (int j = 0; j < n / 2; j++) {
                if (a[m / 2][j] != a[m / 2][n - 1 - j]) {
                    diff++;
                } else {
                    cnt1 += a[m / 2][j] * 2;
                }
            }
        }
        if (n % 2) {
            // 统计正中间这一列
            for (int i = 0; i < m / 2; i++) {
                if (a[i][n / 2] != a[m - 1 - i][n / 2]) {
                    diff++;
                } else {
                    cnt1 += a[i][n / 2] * 2;
                }
            }
        }

        return ans + (diff ? diff : cnt1 % 4);
    }
};
```

```c [sol-C]
int minFlips(int** a, int gridSize, int* gridColSize) {
    int m = gridSize, n = gridColSize[0];
    int ans = 0;
    for (int i = 0; i < m / 2; i++) {
        for (int j = 0; j < n / 2; j++) {
            int cnt1 = a[i][j] + a[i][n - 1 - j] + a[m - 1 - i][j] + a[m - 1 - i][n - 1 - j];
            ans += cnt1 < 4 - cnt1 ? cnt1 : 4 - cnt1; // 全为 1 或全为 0
        }
    }

    if (m % 2 && n % 2) {
        // 正中间的数必须是 0
        ans += a[m / 2][n / 2];
    }

    int diff = 0, cnt1 = 0;
    if (m % 2) {
        // 统计正中间这一排
        for (int j = 0; j < n / 2; j++) {
            if (a[m / 2][j] != a[m / 2][n - 1 - j]) {
                diff++;
            } else {
                cnt1 += a[m / 2][j] * 2;
            }
        }
    }
    if (n % 2) {
        // 统计正中间这一列
        for (int i = 0; i < m / 2; i++) {
            if (a[i][n / 2] != a[m - 1 - i][n / 2]) {
                diff++;
            } else {
                cnt1 += a[i][n / 2] * 2;
            }
        }
    }
    
    return ans + (diff ? diff : cnt1 % 4);
}
```

```go [sol-Go]
func minFlips(a [][]int) (ans int) {
	m, n := len(a), len(a[0])
	for i, row := range a[:m/2] {
		row2 := a[m-1-i]
		for j, x := range row[:n/2] {
			cnt1 := x + row[n-1-j] + row2[j] + row2[n-1-j]
			ans += min(cnt1, 4-cnt1) // 全为 1 或全为 0
		}
	}

	if m%2 > 0 && n%2 > 0 {
		// 正中间的数必须是 0
		ans += a[m/2][n/2]
	}

	diff, cnt1 := 0, 0
	if m%2 > 0 {
		// 统计正中间这一排
		row := a[m/2]
		for j, x := range row[:n/2] {
			if x != row[n-1-j] {
				diff++
			} else {
				cnt1 += x * 2
			}
		}
	}
	if n%2 > 0 {
		// 统计正中间这一列
		for i, row := range a[:m/2] {
			if row[n/2] != a[m-1-i][n/2] {
				diff++
			} else {
				cnt1 += row[n/2] * 2
			}
		}
	}

	if diff > 0 {
		ans += diff
	} else {
		ans += cnt1 % 4
	}
	return
}
```

```js [sol-JavaScript]
var minFlips = function(a) {
    const m = a.length, n = a[0].length;
    let ans = 0;
    for (let i = 0; i < Math.floor(m / 2); i++) {
        for (let j = 0; j < Math.floor(n / 2); j++) {
            const cnt1 = a[i][j] + a[i][n - 1 - j] + a[m - 1 - i][j] + a[m - 1 - i][n - 1 - j];
            ans += Math.min(cnt1, 4 - cnt1); // 全为 1 或全为 0
        }
    }

    if (m % 2 && n % 2) {
        // 正中间的数必须是 0
        ans += a[Math.floor(m / 2)][Math.floor(n / 2)];
    }

    let diff = 0, cnt1 = 0;
    if (m % 2) {
        // 统计正中间这一排
        for (let j = 0; j < Math.floor(n / 2); j++) {
            if (a[Math.floor(m / 2)][j] !== a[Math.floor(m / 2)][n - 1 - j]) {
                diff++;
            } else {
                cnt1 += a[Math.floor(m / 2)][j] * 2;
            }
        }
    }
    if (n % 2) {
        // 统计正中间这一列
        for (let i = 0; i < Math.floor(m / 2); i++) {
            if (a[i][Math.floor(n / 2)] !== a[m - 1 - i][Math.floor(n / 2)]) {
                diff++;
            } else {
                cnt1 += a[i][Math.floor(n / 2)] * 2;
            }
        }
    }

    return ans + (diff ? diff : cnt1 % 4);
};
```

```rust [sol-Rust]
impl Solution {
    pub fn min_flips(a: Vec<Vec<i32>>) -> i32 {
        let m = a.len();
        let n = a[0].len();
        let mut ans = 0;
        for i in 0..m / 2 {
            for j in 0..n / 2 {
                let cnt1 = a[i][j] + a[i][n - 1 - j] + a[m - 1 - i][j] + a[m - 1 - i][n - 1 - j];
                ans += cnt1.min(4 - cnt1); // 全为 1 或全为 0
            }
        }

        if m % 2 == 1 && n % 2 == 1 {
            // 正中间的数必须是 0
            ans += a[m / 2][n / 2];
        }

        let mut diff = 0;
        let mut cnt1 = 0;
        if m % 2 == 1 {
            // 统计正中间这一排
            for j in 0..n / 2 {
                if a[m / 2][j] != a[m / 2][n - 1 - j] {
                    diff += 1;
                } else {
                    cnt1 += a[m / 2][j] * 2;
                }
            }
        }
        if n % 2 == 1 {
            // 统计正中间这一列
            for i in 0..m / 2 {
                if a[i][n / 2] != a[m - 1 - i][n / 2] {
                    diff += 1;
                } else {
                    cnt1 += a[i][n / 2] * 2;
                }
            }
        }

        ans + if diff != 0 { diff } else { cnt1 % 4 }
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(mn)$，其中 $m$ 和 $n$ 分别为 $\textit{grid}$ 的行数和列数。
- 空间复杂度：$\mathcal{O}(1)$。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与一般树（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `五、思维题 / §5.8 分类讨论`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `五、思维题 / §5.8 分类讨论`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
