# 1329. 将矩阵按对角线排序

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/sort-the-matrix-diagonally/
- 题目 slug：`sort-the-matrix-diagonally`
- 来源专题：常用数据结构
- 来源分类路径：零、常用枚举技巧 / §0.3 遍历对角线
- 难度分：1548
- 外部题解来源：https://leetcode.cn/problems/sort-the-matrix-diagonally/solutions/2760094/dui-jiao-xian-pai-xu-fu-yuan-di-pai-xu-p-uts8/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[对角线排序（附原地排序）Python/Java/C++/Go/JS/Rust](https://leetcode.cn/problems/sort-the-matrix-diagonally/solutions/2760094/dui-jiao-xian-pai-xu-fu-yuan-di-pai-xu-p-uts8/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`dui-jiao-xian-pai-xu-fu-yuan-di-pai-xu-p-uts8`
- topic id：`2760094`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 16:46:02 +0800

一条对角线上的坐标（行列下标）满足什么性质？

比如示例 1 的对角线 $3,1,2$，坐标为分别为 $(0,1),(1,2),(2,3)$，它们都满足：行下标减列下标等于一个**定值** $k=-1$。

![lc1329.png](https://pic.leetcode.cn/1714094748-DbCJnG-lc1329.png)

考虑从右上第一条对角线（上图中的 $(0,3)$）开始，**一条一条地排序**，直到左下最后一条对角线（上图中的 $(2,0)$）结束。

设坐标为 $(i,j)$，设 $k=i-j$。

- 第一条对角线上只有一个点，坐标为 $(0,n-1)$，其 $k=1-n$。
- 最后一条对角线上也只有一个点，坐标为 $(m-1,0)$，其 $k=m-1$。
- 所以枚举对角线，就是枚举 $k$ 从 $1-n$ 到 $m-1$。

对于同一条对角线，知道行下标 $i$ 就知道列下标 $j=i-k$。

- $i$ 的最小值：令等式 $k=i-j$ 中的 $j=0$，可得 $i=k$，但 $i$ 必须是非负数，所以 $i$ 最小为 $\max(k, 0)$。
- $i$ 的最大值：令等式 $k=i-j$ 中的 $j=n-1$，可得 $i=k+n-1$，但 $i$ 至多为 $m-1$，所以 $i$ 最大为 $\min(k+n-1, m-1)$。
- 枚举 $i$，范围为左闭右开区间 $[\max(k, 0),\min(k+n, m))$。
- 把 $\textit{mat}[i][i-k]$ 加入一个数组，把数组从小到大排序后，再依次填入 $\textit{mat}[i][i-k]$，即完成了这条对角线的排序。

```py [sol-Python3]
class Solution:
    def diagonalSort(self, mat: List[List[int]]) -> List[List[int]]:
        m, n = len(mat), len(mat[0])
        for k in range(1 - n, m):  # k = i - j
            left_i, right_i = max(k, 0), min(k + n, m)
            a = sorted(mat[i][i - k] for i in range(left_i, right_i))
            for i in range(left_i, right_i):
                mat[i][i - k] = a[i - left_i]
        return mat
```

```java [sol-Java]
class Solution {
    public int[][] diagonalSort(int[][] mat) {
        int m = mat.length;
        int n = mat[0].length;
        int[] a = new int[Math.min(m, n)];
        for (int k = 1 - n; k < m; k++) { // k = i - j
            int leftI = Math.max(k, 0);
            int rightI = Math.min(k + n, m);
            for (int i = leftI; i < rightI; i++) {
                a[i - leftI] = mat[i][i - k];
            }
            Arrays.sort(a, 0, rightI - leftI);
            for (int i = leftI; i < rightI; i++) {
                mat[i][i - k] = a[i - leftI];
            }
        }
        return mat;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<vector<int>> diagonalSort(vector<vector<int>>& mat) {
        int m = mat.size(), n = mat[0].size();
        vector<int> a(min(m, n));
        for (int k = 1 - n; k < m; k++) { // k = i - j
            int left_i = max(k, 0), right_i = min(k + n, m);
            for (int i = left_i; i < right_i; i++) {
                a[i - left_i] = mat[i][i - k];
            }
            sort(a.begin(), a.begin() + (right_i - left_i));
            for (int i = left_i; i < right_i; i++) {
                mat[i][i - k] = a[i - left_i];
            }
        }
        return mat;
    }
};
```

```go [sol-Go]
func diagonalSort(mat [][]int) [][]int {
    m, n := len(mat), len(mat[0])
    arr := make([]int, min(m, n))
    for k := 1 - n; k < m; k++ { // k = i - j
        a := arr[:0]
        minI := max(k, 0)
        maxI := min(k+n, m)
        for i := minI; i < maxI; i++ {
            a = append(a, mat[i][i-k])
        }
        slices.Sort(a)
        for i := minI; i < maxI; i++ {
            mat[i][i-k] = a[i-minI]
        }
    }
    return mat
}
```

```js [sol-JavaScript]
var diagonalSort = function(mat) {
    const m = mat.length;
    const n = mat[0].length;
    for (let k = 1 - n; k < m; k++) { // k = i - j
        const left_i = Math.max(k, 0);
        const right_i = Math.min(k + n, m);
        const a = [];
        for (let i = left_i; i < right_i; i++) {
            a.push(mat[i][i - k]);
        }
        a.sort((a, b) => a - b);
        for (let i = left_i; i < right_i; i++) {
            mat[i][i - k] = a[i - left_i];
        }
    }
    return mat;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn diagonal_sort(mut mat: Vec<Vec<i32>>) -> Vec<Vec<i32>> {
        let m = mat.len() as i32;
        let n = mat[0].len() as i32;
        let mut a = vec![0; m.min(n) as usize];
        for k in 1 - n..m { // k = i - j
            let left_i = k.max(0) as usize;
            let right_i = (k + n).min(m) as usize;
            for i in left_i..right_i {
                a[i - left_i] = mat[i][(i as i32 - k) as usize];
            }
            a[..right_i - left_i].sort_unstable();
            for i in left_i..right_i {
                mat[i][(i as i32 - k) as usize] = a[i - left_i];
            }
        }
        mat
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(mn\log \min(m,n))$，其中 $m$ 和 $n$ 分别为 $\textit{mat}$ 的行数和列数。注意对角线的长度至多为 $\min(m,n)$。有 $\mathcal{O}(m+n)$ 条长为 $\mathcal{O}(\min(m,n))$ 的对角线，由于 $\mathcal{O}(m+n) = \mathcal{O}(\max(m,n))$ 且 $\max(m,n)\cdot \min(m,n) = mn$，所以时间复杂度为 $\mathcal{O}(mn\log \min(m,n))$。
- 空间复杂度：$\mathcal{O}(\min(m,n))$。

## 附：原地排序

无需创建额外数组的写法。

Go 语言可以将排序中关键的 `Less` 和 `Swap` 暴露出来，交由用户实现，底层传入待比较/交换的元素下标。这种灵活性可以高度定制排序算法的行为，即使是对角线这种特殊结构也能做到**原地排序**。

```go
type diagonalSorter struct {
    mat [][]int
    k   int
}

func (s diagonalSorter) Len() int {
    m, n := len(s.mat), len(s.mat[0])
    return min(s.k+n, m) - max(s.k, 0)
}

func (s diagonalSorter) Less(i, j int) bool {
    minI := max(s.k, 0)
    x := s.mat[minI+i][minI+i-s.k]
    y := s.mat[minI+j][minI+j-s.k]
    return x < y
}

func (s diagonalSorter) Swap(i, j int) {
    minI := max(s.k, 0)
    p := &s.mat[minI+i][minI+i-s.k]
    q := &s.mat[minI+j][minI+j-s.k]
    *p, *q = *q, *p
}

func diagonalSort(mat [][]int) [][]int {
    m, n := len(mat), len(mat[0])
    ds := diagonalSorter{mat: mat}
    for ds.k = 1 - n; ds.k < m; ds.k++ {
        sort.Sort(ds)
    }
    return mat
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(mn\log \min(m,n))$，其中 $m$ 和 $n$ 分别为 $\textit{mat}$ 的行数和列数。注意对角线的长度至多为 $\min(m,n)$。有 $\mathcal{O}(m+n)$ 条长为 $\mathcal{O}(\min(m,n))$ 的对角线，由于 $\mathcal{O}(m+n) = \mathcal{O}(\max(m,n))$ 且 $\max(m,n)\cdot \min(m,n) = mn$，所以时间复杂度为 $\mathcal{O}(mn\log \min(m,n))$。
- 空间复杂度：如果考虑排序中递归的栈开销就是 $\mathcal{O}(\log \min(m,n))$，忽略就是 $\mathcal{O}(1)$。

## 分类题单

1. [滑动窗口（定长/不定长/多指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（矩形系列/字典序最小/贡献法）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/贪心/脑筋急转弯）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

## 本地原创解析

### 1. 题意重述

本题来自 `零、常用枚举技巧 / §0.3 遍历对角线`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `零、常用枚举技巧 / §0.3 遍历对角线`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
