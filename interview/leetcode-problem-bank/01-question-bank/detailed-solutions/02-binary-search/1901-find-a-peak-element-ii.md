# 1901. 寻找峰值 II

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/find-a-peak-element-ii/
- 题目 slug：`find-a-peak-element-ii`
- 来源专题：二分算法
- 来源分类路径：四、其他
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/find-a-peak-element-ii/solutions/2571587/tu-jie-li-yong-xing-zui-da-zhi-pan-duan-r4e0n/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【图解】利用行最大值判断峰顶位置，附二分题单（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/find-a-peak-element-ii/solutions/2571587/tu-jie-li-yong-xing-zui-da-zhi-pan-duan-r4e0n/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tu-jie-li-yong-xing-zui-da-zhi-pan-duan-r4e0n`
- topic id：`2571587`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

先来看一个暴力点的做法。

**人往高处走**：从左上角出发，每次往四周比当前位置大的数字走，直到走到一个峰顶。

> 注意题目保证相邻数字不同，对于每个数字，要么它小于四周的某个数字，要么它大于四周的所有数字，不存在相等的情况。

这样做虽然可以找到峰顶，但在最坏情况下的时间复杂度是 $\mathcal{O}(mn)$，不满足题目要求。如下图，我们可能会走出一条蛇形路径。

![LC1901-hack.png](https://pic.leetcode.cn/1702885188-aYSvyq-LC1901-hack.png){:width=350}

能否像 [162. 寻找峰值](https://leetcode.cn/problems/find-peak-element/) 那样二分呢？请看下图。

![LC1901-2-c.png](https://pic.leetcode.cn/1702891554-taVdQU-LC1901-2-c.png)

综上所述，我们可以二分包含峰顶的**行号** $i$：

- 如果 $\textit{mat}[i]$ 的最大值比它下面的相邻数字**小**，则存在一个峰顶，其行号大于 $i$。缩小二分范围，更新二分区间左端点 $\textit{left}$。
- 如果 $\textit{mat}[i]$ 的最大值比它下面的相邻数字**大**，则存在一个峰顶，其行号小于或等于 $i$。缩小二分范围，更新二分区间右端点 $\textit{right}$。

## 实现细节

关于二分算法的原理，请看视频：[二分查找 红蓝染色法【基础算法精讲 04】](https://www.bilibili.com/video/BV1AP41137w7/)。

对于本题，如果每次二分，都是 $\textit{mat}[i]$ 的最大值比它下面的相邻数字小，那么最后会判断出峰顶行号大于 $m-2$，此时可以直接确定**最后一行必然包含峰顶**。

这意味着 $m-1$ **不需要在初始二分范围内**，初始二分范围为 $[0,m-2]$。在这个范围中二分，如果每次都更新的是区间左端点 $\textit{left}$，那么最后得到的二分结果必然是 $m-1$。

如果在 $[0,m-1]$ 中二分，还需要额外判断 $i+1$ 是否越界。在 $[0,m-2]$ 中二分可以避免越界判断。

这一技巧同样可以用在 [162. 寻找峰值](https://leetcode.cn/problems/find-peak-element/)，只需要在闭区间 $[0,n-2]$ 中二分，即开区间 $(-1,n-1)$，[代码见此](https://leetcode.cn/problems/find-peak-element/solution/by-endlesscheng-9ass/)。

#### 写法一：闭区间

```py [sol-Python3]
class Solution:
    def findPeakGrid(self, mat: List[List[int]]) -> List[int]:
        left, right = 0, len(mat) - 2
        while left <= right:
            i = (left + right) // 2
            mx = max(mat[i])
            if mx > mat[i + 1][mat[i].index(mx)]:
                right = i - 1  # 峰顶行号 <= i
            else:
                left = i + 1  # 峰顶行号 > i
        return [left, mat[left].index(max(mat[left]))]
```

```java [sol-Java]
class Solution {
    public int[] findPeakGrid(int[][] mat) {
        int left = 0;
        int right = mat.length - 2;
        while (left <= right) {
            int i = (left + right) >>> 1;
            int j = indexOfMax(mat[i]);
            if (mat[i][j] > mat[i + 1][j]) {
                right = i - 1; // 峰顶行号 <= i
            } else {
                left = i + 1; // 峰顶行号 > i
            }
        }
        return new int[]{left, indexOfMax(mat[left])};
    }

    private int indexOfMax(int[] a) {
        int idx = 0;
        for (int i = 0; i < a.length; i++) {
            if (a[i] > a[idx]) {
                idx = i;
            }
        }
        return idx;
    }
}
```

```cpp [sol-C++]
class Solution {
    int indexOfMax(vector<int>& a) {
        return ranges::max_element(a) - a.begin();
    }

public:
    vector<int> findPeakGrid(vector<vector<int>> &mat) {
        int left = 0, right = mat.size() - 2;
        while (left <= right) {
            int i = left + (right - left) / 2;
            int j = indexOfMax(mat[i]);
            if (mat[i][j] > mat[i + 1][j]) {
                right = i - 1; // 峰顶行号 <= i
            } else {
                left = i + 1; // 峰顶行号 > i
            }
        }
        return {left, indexOfMax(mat[left])};
    }
};
```

```go [sol-Go]
func findPeakGrid(mat [][]int) []int {
    left, right := 0, len(mat)-2
    for left <= right {
        i := left + (right-left)/2
        j := indexOfMax(mat[i])
        if mat[i][j] > mat[i+1][j] {
            right = i - 1 // 峰顶行号 <= i
        } else {
            left = i + 1 // 峰顶行号 > i
        }
    }
    return []int{left, indexOfMax(mat[left])}
}

func indexOfMax(a []int) (idx int) {
    for i, x := range a {
        if x > a[idx] {
            idx = i
        }
    }
    return
}
```

```js [sol-JavaScript]
function indexOfMax(a) {
    let idx = 0;
    for (let i = 0; i < a.length; i++) {
        if (a[i] > a[idx]) {
            idx = i;
        }
    }
    return idx;
}

function findPeakGrid(mat) {
    let left = 0, right = mat.length - 2;
    while (left <= right) {
        const i = Math.floor((left + right) / 2);
        const j = indexOfMax(mat[i]);
        if (mat[i][j] > mat[i + 1][j]) {
            right = i - 1; // 峰顶行号 <= i
        } else {
            left = i + 1; // 峰顶行号 > i
        }
    }
    return [left, indexOfMax(mat[left])];
}
```

#### 写法二：左闭右开区间

```py [sol-Python3]
class Solution:
    def findPeakGrid(self, mat: List[List[int]]) -> List[int]:
        left, right = 0, len(mat) - 1
        while left < right:
            i = (left + right) // 2
            mx = max(mat[i])
            if mx > mat[i + 1][mat[i].index(mx)]:
                right = i  # 峰顶行号 <= i
            else:
                left = i + 1  # 峰顶行号 > i
        return [left, mat[left].index(max(mat[left]))]
```

```java [sol-Java]
class Solution {
    public int[] findPeakGrid(int[][] mat) {
        int left = 0;
        int right = mat.length - 1;
        while (left < right) {
            int i = (left + right) >>> 1;
            int j = indexOfMax(mat[i]);
            if (mat[i][j] > mat[i + 1][j]) {
                right = i; // 峰顶行号 <= i
            } else {
                left = i + 1; // 峰顶行号 > i
            }
        }
        return new int[]{left, indexOfMax(mat[left])};
    }

    private int indexOfMax(int[] a) {
        int idx = 0;
        for (int i = 0; i < a.length; i++) {
            if (a[i] > a[idx]) {
                idx = i;
            }
        }
        return idx;
    }
}
```

```cpp [sol-C++]
class Solution {
    int indexOfMax(vector<int>& a) {
        return ranges::max_element(a) - a.begin();
    }

public:
    vector<int> findPeakGrid(vector<vector<int>> &mat) {
        int left = 0, right = mat.size() - 1;
        while (left < right) {
            int i = left + (right - left) / 2;
            int j = indexOfMax(mat[i]);
            if (mat[i][j] > mat[i + 1][j]) {
                right = i; // 峰顶行号 <= i
            } else {
                left = i + 1; // 峰顶行号 > i
            }
        }
        return {left, indexOfMax(mat[left])};
    }
};
```

```go [sol-Go]
func findPeakGrid(mat [][]int) []int {
    left, right := 0, len(mat)-1
    for left < right {
        i := left + (right-left)/2
        j := indexOfMax(mat[i])
        if mat[i][j] > mat[i+1][j] {
            right = i // 峰顶行号 <= i
        } else {
            left = i + 1 // 峰顶行号 > i
        }
    }
    return []int{left, indexOfMax(mat[left])}
}

func indexOfMax(a []int) (idx int) {
    for i, x := range a {
        if x > a[idx] {
            idx = i
        }
    }
    return
}
```

```js [sol-JavaScript]
function indexOfMax(a) {
    let idx = 0;
    for (let i = 0; i < a.length; i++) {
        if (a[i] > a[idx]) {
            idx = i;
        }
    }
    return idx;
}

function findPeakGrid(mat) {
    let left = 0, right = mat.length - 1;
    while (left < right) {
        const i = Math.floor((left + right) / 2);
        const j = indexOfMax(mat[i]);
        if (mat[i][j] > mat[i + 1][j]) {
            right = i; // 峰顶行号 <= i
        } else {
            left = i + 1; // 峰顶行号 > i
        }
    }
    return [left, indexOfMax(mat[left])];
}
```

```rust [sol-Rust]
impl Solution {
    fn index_of_max(a: &Vec<i32>) -> usize {
        (0..a.len()).max_by_key(|&i| a[i]).unwrap()
    }

    pub fn find_peak_grid(mat: Vec<Vec<i32>>) -> Vec<i32> {
        let mut left = 0;
        let mut right = mat.len() - 1;
        while left < right {
            let i = left + (right - left) / 2;
            let j = Self::index_of_max(&mat[i]);
            if mat[i][j] > mat[i + 1][j] {
                right = i; // 峰顶行号 <= i
            } else {
                left = i + 1; // 峰顶行号 > i
            }
        }
        vec![left as i32, Self::index_of_max(&mat[left]) as i32]
    }
}
```

#### 写法三：开区间

```py [sol-Python3]
class Solution:
    def findPeakGrid(self, mat: List[List[int]]) -> List[int]:
        left, right = -1, len(mat) - 1
        while left + 1 < right:
            i = (left + right) // 2
            mx = max(mat[i])
            if mx > mat[i + 1][mat[i].index(mx)]:
                right = i  # 峰顶行号 <= i
            else:
                left = i  # 峰顶行号 > i
        return [right, mat[right].index(max(mat[right]))]
```

```java [sol-Java]
class Solution {
    public int[] findPeakGrid(int[][] mat) {
        int left = -1;
        int right = mat.length - 1;
        while (left + 1 < right) {
            int i = (left + right) >>> 1;
            int j = indexOfMax(mat[i]);
            if (mat[i][j] > mat[i + 1][j]) {
                right = i; // 峰顶行号 <= i
            } else {
                left = i; // 峰顶行号 > i
            }
        }
        return new int[]{right, indexOfMax(mat[right])};
    }

    private int indexOfMax(int[] a) {
        int idx = 0;
        for (int i = 0; i < a.length; i++) {
            if (a[i] > a[idx]) {
                idx = i;
            }
        }
        return idx;
    }
}
```

```cpp [sol-C++]
class Solution {
    int indexOfMax(vector<int>& a) {
        return ranges::max_element(a) - a.begin();
    }

public:
    vector<int> findPeakGrid(vector<vector<int>> &mat) {
        int left = -1, right = mat.size() - 1;
        while (left + 1 < right) {
            int i = left + (right - left) / 2;
            int j = indexOfMax(mat[i]);
            (mat[i][j] > mat[i + 1][j] ? right : left) = i;
        }
        return {right, indexOfMax(mat[right])};
    }
};
```

```go [sol-Go]
func findPeakGrid(mat [][]int) []int {
    left, right := -1, len(mat)-1
    for left+1 < right {
        i := left + (right-left)/2
        j := indexOfMax(mat[i])
        if mat[i][j] > mat[i+1][j] {
            right = i // 峰顶行号 <= i
        } else {
            left = i // 峰顶行号 > i
        }
    }
    return []int{right, indexOfMax(mat[right])}
}

func indexOfMax(a []int) (idx int) {
    for i, x := range a {
        if x > a[idx] {
            idx = i
        }
    }
    return
}
```

```js [sol-JavaScript]
function indexOfMax(a) {
    let idx = 0;
    for (let i = 0; i < a.length; i++) {
        if (a[i] > a[idx]) {
            idx = i;
        }
    }
    return idx;
}

function findPeakGrid(mat) {
    let left = -1, right = mat.length - 1;
    while (left + 1 < right) {
        const i = Math.floor((left + right) / 2);
        const j = indexOfMax(mat[i]);
        if (mat[i][j] > mat[i + 1][j]) {
            right = i; // 峰顶行号 <= i
        } else {
            left = i; // 峰顶行号 > i
        }
    }
    return [right, indexOfMax(mat[right])];
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log m)$，其中 $m$ 和 $n$ 分别为 $\textit{mat}$ 的行数和列数。需要二分 $\mathcal{O}(\log m)$ 次，每次二分需要 $\mathcal{O}(n)$ 的时间寻找 $\textit{mat}[i]$ 最大值的下标。
- 空间复杂度：$\mathcal{O}(1)$。仅用到若干额外变量。

更多相似题目，见下面的二分题单。

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
