# 74. 搜索二维矩阵

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/search-a-2d-matrix/
- 题目 slug：`search-a-2d-matrix`
- 来源专题：二分算法
- 来源分类路径：一、二分查找 / §1.2 进阶
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/search-a-2d-matrix/solutions/2783931/liang-chong-fang-fa-er-fen-cha-zhao-pai-39d74/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[把二维矩阵拉成一维数组，二分查找（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/search-a-2d-matrix/solutions/2783931/liang-chong-fang-fa-er-fen-cha-zhao-pai-39d74/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`liang-chong-fang-fa-er-fen-cha-zhao-pai-39d74`
- topic id：`2783931`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

由于矩阵的每一行是递增的，且每行的第一个数大于前一行的最后一个数，如果把矩阵每一行拼在一起，我们可以得到一个递增数组。

![lc74.jpg](https://pic.leetcode.cn/1716174344-pbyCKq-lc74.jpg)

例如示例 1，三行拼在一起得

$$
a = [1,3,5,7,10,11,16,20,23,30,34,60]
$$

由于这是一个有序数组，我们可以用二分查找判断 $\textit{target}$ 是否在 $\textit{matrix}$ 中。

代码实现时，并不需要真的拼成一个长为 $mn$ 的数组 $a$，而是将 $a[i]$ 转换成矩阵中的行号和列号。例如示例 1，$i=9$ 对应的 $a[i]=30$，由于矩阵有 $n=4$ 列，所以 $a[i]$ 在 $\left\lfloor\dfrac{i}{n}\right\rfloor=2$ 行，在 $i\bmod n = 1$ 列。

一般地，有

$$
a[i] = \textit{matrix}[\lfloor i/n\rfloor][i\bmod n]
$$

关于二分查找的原理，请看视频讲解：[二分查找 红蓝染色法【基础算法精讲 04】](https://www.bilibili.com/video/BV1AP41137w7/)

下面用的开区间二分，其它写法也可以。

```py [sol-Py3]
class Solution:
    def searchMatrix(self, matrix: List[List[int]], target: int) -> bool:
        m, n = len(matrix), len(matrix[0])
        left, right = -1, m * n
        while left + 1 < right:
            mid = (left + right) // 2
            x = matrix[mid // n][mid % n]
            if x == target:
                return True
            if x < target:
                left = mid
            else:
                right = mid
        return False
```

```py [sol-Py3 库函数]
class Solution:
    def searchMatrix(self, matrix: List[List[int]], target: int) -> bool:
        m, n = len(matrix), len(matrix[0])
        # 注：range 不是 list，是一个对象，创建 range 和下标访问都是 O(1) 的
        i = bisect_left(range(m * n), target, key=lambda i: matrix[i // n][i % n])
        return i < m * n and matrix[i // n][i % n] == target
```

```java [sol-Java]
class Solution {
    public boolean searchMatrix(int[][] matrix, int target) {
        int m = matrix.length;
        int n = matrix[0].length;
        int left = -1;
        int right = m * n;
        while (left + 1 < right) {
            int mid = (left + right) >>> 1;
            int x = matrix[mid / n][mid % n];
            if (x == target) {
                return true;
            }
            if (x < target) {
                left = mid;
            } else {
                right = mid;
            }
        }
        return false;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    bool searchMatrix(vector<vector<int>>& matrix, int target) {
        int m = matrix.size(), n = matrix[0].size();
        int left = -1, right = m * n;
        while (left + 1 < right) {
            int mid = left + (right - left) / 2;
            int x = matrix[mid / n][mid % n];
            if (x == target) {
                return true;
            }
            (x < target ? left : right) = mid;
        }
        return false;
    }
};
```

```c [sol-C]
bool searchMatrix(int** matrix, int matrixSize, int* matrixColSize, int target) {
    int m = matrixSize, n = matrixColSize[0];
    int left = -1, right = m * n;
    while (left + 1 < right) {
        int mid = left + (right - left) / 2;
        int x = matrix[mid / n][mid % n];
        if (x == target) {
            return true;
        }
        if (x < target) {
            left = mid;
        } else {
            right = mid;
        }
    }
    return false;
}
```

```go [sol-Go]
func searchMatrix(matrix [][]int, target int) bool {
    m, n := len(matrix), len(matrix[0])
    left, right := -1, m*n
    for left+1 < right {
        mid := left + (right-left)/2
        x := matrix[mid/n][mid%n]
        if x == target {
            return true
        }
        if x < target {
            left = mid
        } else {
            right = mid
        }
    }
    return false
}
```

```go [sol-Go 库函数]
func searchMatrix(matrix [][]int, target int) bool {
    m, n := len(matrix), len(matrix[0])
    i := sort.Search(m*n, func(i int) bool { return matrix[i/n][i%n] >= target })
    return i < m*n && matrix[i/n][i%n] == target
}
```

```js [sol-JS]
var searchMatrix = function(matrix, target) {
    const m = matrix.length, n = matrix[0].length;
    let left = -1, right = m * n;
    while (left + 1 < right) {
        const mid = Math.floor((left + right) / 2);
        const x = matrix[Math.floor(mid / n)][mid % n];
        if (x === target) {
            return true;
        }
        if (x < target) {
            left = mid;
        } else {
            right = mid;
        }
    }
    return false;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn search_matrix(matrix: Vec<Vec<i32>>, target: i32) -> bool {
        let m = matrix.len();
        let n = matrix[0].len();
        let mut left = 0;
        let mut right = m * n;
        while left < right { // 左闭右开
            let mid = left + (right - left) / 2;
            let x = matrix[mid / n][mid % n];
            if x == target {
                return true;
            }
            if x < target {
                left = mid + 1;
            } else {
                right = mid;
            }
        }
        return false;
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(\log (mn))$，其中 $m$ 和 $n$ 分别为 $\textit{matrix}$ 的行数和列数。
- 空间复杂度：$\mathcal{O}(1)$。

## 变形题

去掉「每行的第一个整数大于前一行的最后一个整数」的性质，添加「每列的元素从上到下升序排列」的性质，怎么做？

这题是 [240. 搜索二维矩阵 II](https://leetcode.cn/problems/search-a-2d-matrix-ii/)，请看 [我的题解](https://leetcode.cn/problems/search-a-2d-matrix-ii/solutions/2783938/tu-jie-pai-chu-fa-yi-tu-miao-dong-python-kytg/)。

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

本题来自 `一、二分查找 / §1.2 进阶`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、二分查找 / §1.2 进阶`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
