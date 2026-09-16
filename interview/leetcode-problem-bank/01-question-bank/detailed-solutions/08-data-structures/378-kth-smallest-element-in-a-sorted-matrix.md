# 378. 有序矩阵中第 K 小的元素

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/kth-smallest-element-in-a-sorted-matrix/
- 题目 slug：`kth-smallest-element-in-a-sorted-matrix`
- 来源专题：常用数据结构
- 来源分类路径：五、堆（优先队列） / §5.3 第 K 小/大
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/kth-smallest-element-in-a-sorted-matrix/solutions/3699846/tu-jie-di-k-xiao-da-wen-ti-de-tong-yong-teznd/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【图解】第 k 小/大问题的通用转化方法（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/kth-smallest-element-in-a-sorted-matrix/solutions/3699846/tu-jie-di-k-xiao-da-wen-ti-de-tong-yong-teznd/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tu-jie-di-k-xiao-da-wen-ti-de-tong-yong-teznd`
- topic id：`3699846`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

第 $k$ 小/大问题的**通用转化方法**：

- 第 $k$ 小等价于：求最小的 $x$，满足 $\le x$ 的数**至少**有 $k$ 个。（注意是至少不是恰好）
- 第 $k$ 大等价于：求最大的 $x$，满足 $\ge x$ 的数**至少**有 $k$ 个。

$x$ 越大，越能找到 $k$ 个数；$x$ 越小，越不能找到 $k$ 个数。据此，可以**二分猜答案**。关于二分算法的原理，请看 [二分查找 红蓝染色法【基础算法精讲 04】](https://www.bilibili.com/video/BV1AP41137w7/)。

现在本题转化成一个判定性问题：

- 给定整数 $\textit{mx}$，统计有序矩阵中 $\le \textit{mx}$ 的元素个数 $\textit{cnt}$，判断是否满足 $\textit{cnt}\ge k$。

如何高效统计 $\textit{cnt}$ 呢？

做法和 [240. 搜索二维矩阵 II](https://leetcode.cn/problems/search-a-2d-matrix-ii/) 是一样的，都可以用**双指针**实现：

![lc378-c.png](https://pic.leetcode.cn/1749859762-kBcveG-lc378-c.png){:width=550}

## 细节

下面代码采用开区间二分，这仅仅是二分的一种写法，使用闭区间或者半闭半开区间都是可以的，喜欢哪种写法就用哪种。

- 开区间左端点初始值：$\textit{matrix}[0][0]-1$。不存在比最小值还小的数，也就是有 $0$ 个数 $\le \textit{matrix}[0][0]-1$，由于 $0 < k$，所以无法满足要求。
- 开区间右端点初始值：$\textit{matrix}[n-1][n-1]$。有 $n^2$ 个数 $\le \textit{matrix}[n-1][n-1]$，由于 $n^2\ge k$，所以一定满足要求。

对于开区间写法，简单来说 `check(mid) == true` 时更新的是谁，最后就返回谁。相比其他二分写法，开区间写法不需要思考加一减一等细节，更简单。推荐使用开区间写二分。

## 答疑

**问**：为什么二分结束后，答案 $\textit{ans}$ 一定在矩阵中？

**答**：反证法，假设 $\textit{ans}$ 不在矩阵中。二分结束后，我们知道矩阵中 $\le \textit{ans}$ 的数至少有 $k$ 个。如果 $\textit{ans}$ 不在矩阵中，那么 $\le \textit{ans}-1$ 的数至少有 $k$ 个，即 $\text{check}(\textit{ans}-1)=\texttt{true}$。但根据循环不变量，二分结束后 $\text{check}(\textit{ans}-1)=\texttt{false}$，矛盾。故原命题成立。

```py [sol-Py3]
class Solution:
    def kthSmallest(self, matrix: List[List[int]], k: int) -> int:
        n = len(matrix)

        def check(mx: int) -> bool:
            cnt = 0  # matrix 中的 <= mx 的元素个数
            i, j = 0, n - 1  # 从右上角开始
            while i < n and j >= 0 and cnt < k:
                if matrix[i][j] > mx:
                    j -= 1  # 排除第 j 列
                else:
                    cnt += j + 1  # 从 matrix[i][0] 到 matrix[i][j] 都 <= mx
                    i += 1
            return cnt >= k

        left, right = matrix[0][0] - 1, matrix[-1][-1]
        while left + 1 < right:
            mid = (left + right) // 2
            if check(mid):
                right = mid
            else:
                left = mid
        return right
```

```py [sol-Py3 库函数]
class Solution:
    def kthSmallest(self, matrix: List[List[int]], k: int) -> int:
        n = len(matrix)

        def check(mx: int) -> bool:
            cnt = 0  # matrix 中的 <= mx 的元素个数
            i, j = 0, n - 1  # 从右上角开始
            while i < n and j >= 0 and cnt < k:
                if matrix[i][j] > mx:
                    j -= 1  # 排除第 j 列
                else:
                    cnt += j + 1  # 从 matrix[i][0] 到 matrix[i][j] 都 <= mx
                    i += 1
            return cnt >= k

        # 库函数是左闭右开区间
        left, right = matrix[0][0], matrix[-1][-1]
        return left + bisect_left(range(left, right), True, key=check)
```

```java [sol-Java]
class Solution {
    public int kthSmallest(int[][] matrix, int k) {
        int n = matrix.length;
        int left = matrix[0][0] - 1;
        int right = matrix[n - 1][n - 1];
        while (left + 1 < right) {
            int mid = left + (right - left) / 2;
            if (check(matrix, k, mid)) {
                right = mid;
            } else {
                left = mid;
            }
        }
        return right;
    }

    private boolean check(int[][] matrix, int k, int mx) {
        int n = matrix.length;
        int cnt = 0; // matrix 中的 <= mx 的元素个数
        int i = 0;
        int j = n - 1; // 从右上角开始
        while (i < n && j >= 0 && cnt < k) {
            if (matrix[i][j] > mx) {
                j--; // 排除第 j 列
            } else {
                cnt += j + 1; // 从 matrix[i][0] 到 matrix[i][j] 都 <= mx
                i++;
            }
        }
        return cnt >= k;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int kthSmallest(vector<vector<int>>& matrix, int k) {
        int n = matrix.size();

        auto check = [&](int mx) -> bool {
            int cnt = 0; // matrix 中的 <= mx 的元素个数
            int i = 0, j = n - 1; // 从右上角开始
            while (i < n && j >= 0 && cnt < k) {
                if (matrix[i][j] > mx) {
                    j--; // 排除第 j 列
                } else {
                    cnt += j + 1; // 从 matrix[i][0] 到 matrix[i][j] 都 <= mx
                    i++;
                }
            }
            return cnt >= k;
        };

        int left = matrix[0][0] - 1;
        int right = matrix[n - 1][n - 1];
        while (left + 1 < right) {
            int mid = left + (right - left) / 2;
            (check(mid) ? right : left) = mid;
        }
        return right;
    }
};
```

```c [sol-C]
int kthSmallest(int** matrix, int matrixSize, int* matrixColSize, int k) {
    int n = matrixSize;

    int check(int mx) {
        int cnt = 0; // matrix 中的 <= mx 的元素个数
        int i = 0, j = n - 1; // 从右上角开始
        while (i < n && j >= 0 && cnt < k) {
            if (matrix[i][j] > mx) {
                j--; // 排除第 j 列
            } else {
                cnt += j + 1; // 从 matrix[i][0] 到 matrix[i][j] 都 <= mx
                i++;
            }
        }
        return cnt >= k;
    }

    int left = matrix[0][0] - 1;
    int right = matrix[n - 1][n - 1];
    while (left + 1 < right) {
        int mid = left + (right - left) / 2;
        if (check(mid)) {
            right = mid;
        } else {
            left = mid;
        }
    }
    return right;
}
```

```go [sol-Go]
func kthSmallest(matrix [][]int, k int) int {
    n := len(matrix)

    check := func(mx int) bool {
        cnt := 0 // matrix 中的 <= mx 的元素个数
        i, j := 0, n-1 // 从右上角开始
        for i < n && j >= 0 && cnt < k {
            if matrix[i][j] > mx {
                j-- // 排除第 j 列
            } else {
                cnt += j + 1 // 从 matrix[i][0] 到 matrix[i][j] 都 <= mx
                i++
            }
        }
        return cnt >= k
    }

    left, right := matrix[0][0]-1, matrix[n-1][n-1]
    for left+1 < right {
        mid := left + (right-left)/2
        if check(mid) {
            right = mid
        } else {
            left = mid
        }
    }
    return right
}
```

```go [sol-Go 库函数]
func kthSmallest(matrix [][]int, k int) int {
    n := len(matrix)
    // 左闭右开区间
    left, right := matrix[0][0], matrix[n-1][n-1]
    ans := left + sort.Search(right-left, func(mx int) bool {
        mx += left
        cnt := 0 // matrix 中的 <= mx 的元素个数
        i, j := 0, n-1 // 从右上角开始
        for i < n && j >= 0 && cnt < k {
            if matrix[i][j] > mx {
                j-- // 排除第 j 列
            } else {
                cnt += j + 1 // 从 matrix[i][0] 到 matrix[i][j] 都 <= mx
                i++
            }
        }
        return cnt >= k
    })
    return ans
}
```

```js [sol-JavaScript]
var kthSmallest = function(matrix, k) {
    const n = matrix.length;

    function check(mx) {
        let cnt = 0; // matrix 中的 <= mx 的元素个数
        let i = 0, j = n - 1; // 从右上角开始
        while (i < n && j >= 0 && cnt < k) {
            if (matrix[i][j] > mx) {
                j--; // 排除第 j 列
            } else {
                cnt += j + 1; // 从 matrix[i][0] 到 matrix[i][j] 都 <= mx
                i++;
            }
        }
        return cnt >= k;
    }

    let left = matrix[0][0] - 1;
    let right = matrix[n - 1][n - 1];
    while (left + 1 < right) {
        const mid = Math.floor((left + right) / 2);
        if (check(mid)) {
            right = mid;
        } else {
            left = mid;
        }
    }
    return right;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn kth_smallest(matrix: Vec<Vec<i32>>, k: i32) -> i32 {
        let n = matrix.len();

        let check = |mx: i32| -> bool {
            let mut cnt = 0; // matrix 中的 <= mx 的元素个数
            let mut i = 0;
            let mut j = n as i32 - 1; // 从右上角开始
            while i < n && j >= 0 && cnt < k {
                if matrix[i][j as usize] > mx {
                    j -= 1; // 排除第 j 列
                } else {
                    cnt += j + 1; // 从 matrix[i][0] 到 matrix[i][j] 都 <= mx
                    i += 1;
                }
            }
            cnt >= k
        };

        let mut left = matrix[0][0] - 1;
        let mut right = matrix[n - 1][n - 1];
        while left + 1 < right {
            let mid = left + (right - left) / 2;
            if check(mid) {
                right = mid;
            } else {
                left = mid;
            }
        }
        right
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log U)$，其中 $n$ 是 $\textit{matrix}$ 的行数和列数，$U=\textit{matrix}[n-1][n-1] - \textit{matrix}[0][0]$。二分 $\mathcal{O}(\log U)$ 次，每次需要跑一个 $\mathcal{O}(n)$ 的双指针。
- 空间复杂度：$\mathcal{O}(1)$。

## 相似题目

- [668. 乘法表中第 K 小的数](https://leetcode.cn/problems/kth-smallest-number-in-multiplication-table/)
- [2040. 两个有序数组的第 K 小乘积](https://leetcode.cn/problems/kth-smallest-product-of-two-sorted-arrays/)

更多相似题目，见下面二分题单的「**§2.6 第 K 小/大**」。

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

本题来自 `五、堆（优先队列） / §5.3 第 K 小/大`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `五、堆（优先队列） / §5.3 第 K 小/大`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
