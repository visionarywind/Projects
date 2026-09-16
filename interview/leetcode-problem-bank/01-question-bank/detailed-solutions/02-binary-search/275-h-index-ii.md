# 275. H 指数 II

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/h-index-ii/
- 题目 slug：`h-index-ii`
- 来源专题：二分算法
- 来源分类路径：二、二分答案 / §2.2 求最大
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/h-index-ii/solutions/2504326/tu-jie-yi-tu-zhang-wo-er-fen-da-an-si-ch-d15k/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【图解】一图掌握二分答案，四种写法（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/h-index-ii/solutions/2504326/tu-jie-yi-tu-zhang-wo-er-fen-da-an-si-ch-d15k/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tu-jie-yi-tu-zhang-wo-er-fen-da-an-si-ch-d15k`
- topic id：`2504326`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

**题意**：找最大的 $h$，满足 $\textit{citations}[n-h]\ge h$。

![lc275-c.png](https://pic.leetcode.cn/1698492370-tnIhCf-lc275-c.png){:width=600px}

⚠**注意**：我们二分的是答案，是**引用次数**，不是 $\textit{citations}$ 的下标。

关于二分算法的原理，请看视频讲解[【基础算法精讲 04】](https://www.bilibili.com/video/BV1AP41137w7/)。

#### 闭区间写法

```py [sol-Python3]
class Solution:
    def hIndex(self, citations: List[int]) -> int:
        # 在区间 [left, right] 内询问
        left = 1
        right = len(citations)
        while left <= right:  # 区间不为空
            # 循环不变量：
            # left-1 的回答一定为「是」
            # right+1 的回答一定为「否」
            mid = (left + right) // 2
            # 引用次数最多的 mid 篇论文，引用次数均 >= mid
            if citations[-mid] >= mid:
                left = mid + 1  # 询问范围缩小到 [mid+1, right]
            else:
                right = mid - 1  # 询问范围缩小到 [left, mid-1]
        # 循环结束后 right 等于 left-1，回答一定为「是」
        # 根据循环不变量，right 现在是最大的回答为「是」的数
        return right
```

```java [sol-Java]
class Solution {
    public int hIndex(int[] citations) {
        // 在区间 [left, right] 内询问
        int n = citations.length;
        int left = 1;
        int right = n;
        while (left <= right) { // 区间不为空
            // 循环不变量：
            // left-1 的回答一定为「是」
            // right+1 的回答一定为「否」
            int mid = (left + right) >>> 1;
            // 引用次数最多的 mid 篇论文，引用次数均 >= mid
            if (citations[n - mid] >= mid) {
                left = mid + 1; // 询问范围缩小到 [mid+1, right]
            } else {
                right = mid - 1; // 询问范围缩小到 [left, mid-1]
            }
        }
        // 循环结束后 right 等于 left-1，回答一定为「是」
        // 根据循环不变量，right 现在是最大的回答为「是」的数
        return right;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int hIndex(vector<int> &citations) {
        // 在区间 [left, right] 内询问
        int n = citations.size();
        int left = 1;
        int right = n;
        while (left <= right) { // 区间不为空
            // 循环不变量：
            // left-1 的回答一定为「是」
            // right+1 的回答一定为「否」
            int mid = (left + right) / 2; // left+(right-left)/2
            // 引用次数最多的 mid 篇论文，引用次数均 >= mid
            if (citations[n - mid] >= mid) {
                left = mid + 1; // 询问范围缩小到 [mid+1, right]
            } else {
                right = mid - 1; // 询问范围缩小到 [left, mid-1]
            }
        }
        // 循环结束后 right 等于 left-1，回答一定为「是」
        // 根据循环不变量，right 现在是最大的回答为「是」的数
        return right;
    }
};
```

```go [sol-Go]
func hIndex(citations []int) int {
    // 在区间 [left, right] 内询问
    left, right := 1, len(citations)
    for left <= right { // 区间不为空
        // 循环不变量：
        // left-1 的回答一定为「是」
        // right+1 的回答一定为「否」
        mid := (left + right) / 2
        // 引用次数最多的 mid 篇论文，引用次数均 >= mid
        if citations[len(citations)-mid] >= mid {
            left = mid + 1 // 询问范围缩小到 [mid+1, right]
        } else {
            right = mid - 1 // 询问范围缩小到 [left, mid-1]
        }
    }
    // 循环结束后 right 等于 left-1，回答一定为「是」
    // 根据循环不变量，right 现在是最大的回答为「是」的数
    return right
}
```

```js [sol-JavaScript]
var hIndex = function(citations) {
    // 在区间 [left, right] 内询问
    const n = citations.length;
    let left = 1;
    let right = n;
    while (left <= right) { // 区间不为空
        // 循环不变量：
        // left-1 的回答一定为「是」
        // right+1 的回答一定为「否」
        const mid = Math.floor((left + right) / 2);
        // 引用次数最多的 mid 篇论文，引用次数均 >= mid
        if (citations[n - mid] >= mid) {
            left = mid + 1; // 询问范围缩小到 [mid+1, right]
        } else {
            right = mid - 1; // 询问范围缩小到 [left, mid-1]
        }
    }
    // 循环结束后 right 等于 left-1，回答一定为「是」
    // 根据循环不变量，right 现在是最大的回答为「是」的数
    return right;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn h_index(citations: Vec<i32>) -> i32 {
        // 在区间 [left, right] 内询问
        let n = citations.len();
        let mut left = 1;
        let mut right = n;
        while left <= right { // 区间不为空
            // 循环不变量：
            // left-1 的回答一定为「是」
            // right+1 的回答一定为「否」
            let mid = (left + right) / 2;
            // 引用次数最多的 mid 篇论文，引用次数均 >= mid
            if citations[n - mid] >= mid as i32 {
                left = mid + 1; // 询问范围缩小到 [mid+1, right]
            } else {
                right = mid - 1; // 询问范围缩小到 [left, mid-1]
            }
        }
        // 循环结束后 right 等于 left-1，回答一定为「是」
        // 根据循环不变量，right 现在是最大的回答为「是」的数
        right as i32
    }
}
```

#### 左闭右开区间写法

```py [sol-Python3]
class Solution:
    def hIndex(self, citations: List[int]) -> int:
        # 在区间 [left, right) 内询问
        left = 1
        right = len(citations) + 1
        while left < right:  # 区间不为空
            # 循环不变量：
            # left-1 的回答一定为「是」
            # right 的回答一定为「否」
            mid = (left + right) // 2
            # 引用次数最多的 mid 篇论文，引用次数均 >= mid
            if citations[-mid] >= mid:
                left = mid + 1  # 询问范围缩小到 [mid+1, right)
            else:
                right = mid  # 询问范围缩小到 [left, mid)
        # 根据循环不变量，left-1 现在是最大的回答为「是」的数
        return left - 1
```

```java [sol-Java]
class Solution {
    public int hIndex(int[] citations) {
        // 在区间 [left, right) 内询问
        int n = citations.length;
        int left = 1;
        int right = n + 1;
        while (left < right) { // 区间不为空
            // 循环不变量：
            // left-1 的回答一定为「是」
            // right 的回答一定为「否」
            int mid = (left + right) >>> 1;
            // 引用次数最多的 mid 篇论文，引用次数均 >= mid
            if (citations[n - mid] >= mid) {
                left = mid + 1; // 询问范围缩小到 [mid+1, right)
            } else {
                right = mid; // 询问范围缩小到 [left, mid)
            }
        }
        // 根据循环不变量，left-1 现在是最大的回答为「是」的数
        return left - 1;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int hIndex(vector<int> &citations) {
        // 在区间 [left, right) 内询问
        int n = citations.size();
        int left = 1;
        int right = n + 1;
        while (left < right) { // 区间不为空
            // 循环不变量：
            // left-1 的回答一定为「是」
            // right 的回答一定为「否」
            int mid = (left + right) / 2;
            // 引用次数最多的 mid 篇论文，引用次数均 >= mid
            if (citations[n - mid] >= mid) {
                left = mid + 1; // 询问范围缩小到 [mid+1, right)
            } else {
                right = mid; // 询问范围缩小到 [left, mid)
            }
        }
        // 根据循环不变量，left-1 现在是最大的回答为「是」的数
        return left - 1;
    }
};
```

```go [sol-Go]
func hIndex(citations []int) int {
    // 在区间 [left, right) 内询问
    left, right := 1, len(citations)+1
    for left < right { // 区间不为空
        // 循环不变量：
        // left-1 的回答一定为「是」
        // right 的回答一定为「否」
        mid := (left + right) / 2
        // 引用次数最多的 mid 篇论文，引用次数均 >= mid
        if citations[len(citations)-mid] >= mid {
            left = mid + 1 // 询问范围缩小到 [mid+1, right)
        } else {
            right = mid // 询问范围缩小到 [left, mid)
        }
    }
    // 根据循环不变量，left-1 现在是最大的回答为「是」的数
    return left - 1
}
```

```js [sol-JavaScript]
var hIndex = function(citations) {
    // 在区间 [left, right) 内询问
    const n = citations.length;
    let left = 1;
    let right = n + 1;
    while (left < right) { // 区间不为空
        // 循环不变量：
        // left-1 的回答一定为「是」
        // right 的回答一定为「否」
        const mid = Math.floor((left + right) / 2);
        // 引用次数最多的 mid 篇论文，引用次数均 >= mid
        if (citations[n - mid] >= mid) {
            left = mid + 1; // 询问范围缩小到 [mid+1, right)
        } else {
            right = mid; // 询问范围缩小到 [left, mid)
        }
    }
    // 根据循环不变量，left-1 现在是最大的回答为「是」的数
    return left - 1;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn h_index(citations: Vec<i32>) -> i32 {
        // 在区间 [left, right) 内询问
        let n = citations.len();
        let mut left = 1;
        let mut right = n + 1;
        while left < right { // 区间不为空
            // 循环不变量：
            // left-1 的回答一定为「是」
            // right 的回答一定为「否」
            let mid = (left + right) / 2;
            // 引用次数最多的 mid 篇论文，引用次数均 >= mid
            if citations[n - mid] >= mid as i32 {
                left = mid + 1; // 询问范围缩小到 [mid+1, right)
            } else {
                right = mid; // 询问范围缩小到 [left, mid)
            }
        }
        // 根据循环不变量，left-1 现在是最大的回答为「是」的数
        left as i32 - 1
    }
}
```

#### 左开右闭区间写法

```py [sol-Python3]
class Solution:
    def hIndex(self, citations: List[int]) -> int:
        # 在区间 (left, right] 内询问
        left = 0
        right = len(citations)
        while left < right:  # 区间不为空
            # 循环不变量：
            # left 的回答一定为「是」
            # right+1 的回答一定为「否」
            mid = (left + right + 1) // 2  # 保证 mid 在二分区间内
            # 引用次数最多的 mid 篇论文，引用次数均 >= mid
            if citations[-mid] >= mid:
                left = mid  # 询问范围缩小到 (mid, right]
            else:
                right = mid - 1  # 询问范围缩小到 (left, mid-1]
        # 根据循环不变量，left 现在是最大的回答为「是」的数
        return left
```

```java [sol-Java]
class Solution {
    public int hIndex(int[] citations) {
        // 在区间 (left, right] 内询问
        int n = citations.length;
        int left = 0;
        int right = n;
        while (left < right) { // 区间不为空
            // 循环不变量：
            // left 的回答一定为「是」
            // right+1 的回答一定为「否」
            int mid = (left + right + 1) >>> 1; // 保证 mid 在二分区间内
            // 引用次数最多的 mid 篇论文，引用次数均 >= mid
            if (citations[n - mid] >= mid) {
                left = mid; // 询问范围缩小到 (mid, right]
            } else {
                right = mid - 1; // 询问范围缩小到 (left, mid-1]
            }
        }
        // 根据循环不变量，left 现在是最大的回答为「是」的数
        return left;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int hIndex(vector<int> &citations) {
        // 在区间 (left, right] 内询问
        int n = citations.size();
        int left = 0;
        int right = n;
        while (left < right) { // 区间不为空
            // 循环不变量：
            // left 的回答一定为「是」
            // right+1 的回答一定为「否」
            int mid = (left + right + 1) / 2; // 保证 mid 在二分区间内
            // 引用次数最多的 mid 篇论文，引用次数均 >= mid
            if (citations[n - mid] >= mid) {
                left = mid; // 询问范围缩小到 (mid, right]
            } else {
                right = mid - 1; // 询问范围缩小到 (left, mid-1]
            }
        }
        // 根据循环不变量，left 现在是最大的回答为「是」的数
        return left;
    }
};
```

```go [sol-Go]
func hIndex(citations []int) int {
    // 在区间 (left, right] 内询问
    left, right := 0, len(citations)
    for left < right { // 区间不为空
        // 循环不变量：
        // left 的回答一定为「是」
        // right+1 的回答一定为「否」
        mid := (left + right + 1) / 2 // 保证 mid 在二分区间内
        // 引用次数最多的 mid 篇论文，引用次数均 >= mid
        if citations[len(citations) - mid] >= mid {
            left = mid // 询问范围缩小到 (mid, right]
        } else {
            right = mid - 1 // 询问范围缩小到 (left, mid-1]
        }
    }
    // 根据循环不变量，left 现在是最大的回答为「是」的数
    return left
}
```

```js [sol-JavaScript]
var hIndex = function(citations) {
    // 在区间 (left, right] 内询问
    const n = citations.length;
    let left = 0;
    let right = n;
    while (left < right) { // 区间不为空
        // 循环不变量：
        // left 的回答一定为「是」
        // right+1 的回答一定为「否」
        const mid = Math.ceil((left + right) / 2); // 保证 mid 在二分区间内
        // 引用次数最多的 mid 篇论文，引用次数均 >= mid
        if (citations[n - mid] >= mid) {
            left = mid; // 询问范围缩小到 (mid, right]
        } else {
            right = mid - 1; // 询问范围缩小到 (left, mid-1]
        }
    }
    // 根据循环不变量，left 现在是最大的回答为「是」的数
    return left;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn h_index(citations: Vec<i32>) -> i32 {
        // 在区间 (left, right] 内询问
        let n = citations.len();
        let mut left = 0;
        let mut right = n;
        while left < right { // 区间不为空
            // 循环不变量：
            // left 的回答一定为「是」
            // right+1 的回答一定为「否」
            let mid = (left + right + 1) / 2; // 保证 mid 在二分区间内
            // 引用次数最多的 mid 篇论文，引用次数均 >= mid
            if citations[n - mid] >= mid as i32 {
                left = mid; // 询问范围缩小到 (mid, right]
            } else {
                right = mid - 1; // 询问范围缩小到 (left, mid-1]
            }
        }
        // 根据循环不变量，left 现在是最大的回答为「是」的数
        left as i32
    }
}
```

#### 开区间写法

```py [sol-Python3]
class Solution:
    def hIndex(self, citations: List[int]) -> int:
        # 在区间 (left, right) 内询问
        left = 0
        right = len(citations) + 1
        while left + 1 < right:  # 区间不为空
            # 循环不变量：
            # left 的回答一定为「是」
            # right 的回答一定为「否」
            mid = (left + right) // 2
            # 引用次数最多的 mid 篇论文，引用次数均 >= mid
            if citations[-mid] >= mid:
                left = mid  # 询问范围缩小到 (mid, right)
            else:
                right = mid  # 询问范围缩小到 (left, mid)
        # 根据循环不变量，left 现在是最大的回答为「是」的数
        return left
```

```java [sol-Java]
class Solution {
    public int hIndex(int[] citations) {
        // 在区间 (left, right) 内询问
        int n = citations.length;
        int left = 0;
        int right = n + 1;
        while (left + 1 < right) { // 区间不为空
            // 循环不变量：
            // left 的回答一定为「是」
            // right 的回答一定为「否」
            int mid = (left + right) >>> 1;
            // 引用次数最多的 mid 篇论文，引用次数均 >= mid
            if (citations[n - mid] >= mid) {
                left = mid; // 询问范围缩小到 (mid, right)
            } else {
                right = mid; // 询问范围缩小到 (left, mid)
            }
        }
        // 根据循环不变量，left 现在是最大的回答为「是」的数
        return left;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int hIndex(vector<int> &citations) {
        // 在区间 (left, right) 内询问
        int n = citations.size();
        int left = 0;
        int right = n + 1;
        while (left + 1 < right) { // 区间不为空
            // 循环不变量：
            // left 的回答一定为「是」
            // right 的回答一定为「否」
            int mid = (left + right) / 2;
            // 引用次数最多的 mid 篇论文，引用次数均 >= mid
            if (citations[n - mid] >= mid) {
                left = mid; // 询问范围缩小到 (mid, right)
            } else {
                right = mid; // 询问范围缩小到 (left, mid)
            }
        }
        // 根据循环不变量，left 现在是最大的回答为「是」的数
        return left;
    }
};
```

```go [sol-Go]
func hIndex(citations []int) int {
    // 在区间 (left, right) 内询问
    left, right := 0, len(citations) + 1
    for left + 1 < right { // 区间不为空
        // 循环不变量：
        // left 的回答一定为「是」
        // right 的回答一定为「否」
        mid := (left + right) / 2
        // 引用次数最多的 mid 篇论文，引用次数均 >= mid
        if citations[len(citations) - mid] >= mid {
            left = mid // 询问范围缩小到 (mid, right)
        } else {
            right = mid // 询问范围缩小到 (left, mid)
        }
    }
    // 根据循环不变量，left 现在是最大的回答为「是」的数
    return left
}
```

```js [sol-JavaScript]
var hIndex = function(citations) {
    // 在区间 (left, right) 内询问
    const n = citations.length;
    let left = 0;
    let right = n + 1;
    while (left + 1 < right) { // 区间不为空
        // 循环不变量：
        // left 的回答一定为「是」
        // right 的回答一定为「否」
        const mid = Math.floor((left + right) / 2);
        // 引用次数最多的 mid 篇论文，引用次数均 >= mid
        if (citations[n - mid] >= mid) {
            left = mid; // 询问范围缩小到 (mid, right)
        } else {
            right = mid; // 询问范围缩小到 (left, mid)
        }
    }
    // 根据循环不变量，left 现在是最大的回答为「是」的数
    return left;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn h_index(citations: Vec<i32>) -> i32 {
        // 在区间 (left, right) 内询问
        let n = citations.len();
        let mut left = 0;
        let mut right = n + 1;
        while left + 1 < right { // 区间不为空
            // 循环不变量：
            // left 的回答一定为「是」
            // right 的回答一定为「否」
            let mid = (left + right) / 2;
            // 引用次数最多的 mid 篇论文，引用次数均 >= mid
            if citations[n - mid] >= mid as i32 {
                left = mid; // 询问范围缩小到 (mid, right)
            } else {
                right = mid; // 询问范围缩小到 (left, mid)
            }
        }
        // 根据循环不变量，left 现在是最大的回答为「是」的数
        left as i32
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(\log n)$，其中 $n$ 是 $\textit{citations}$ 的长度。每次循环，都会把二分范围缩短一半，所以会循环 $\mathcal{O}(\log n)$ 次，时间复杂度为 $\mathcal{O}(\log n)$。
- 空间复杂度：$\mathcal{O}(1)$。

## 附：其他写法

二分的初始范围可以进一步缩小至

$$
[\min(\textit{citations}[0], n) + 1, \min(\textit{citations}[n-1], n)]
$$

```py
class Solution:
    def hIndex(self, citations: List[int]) -> int:
        n = len(citations)
        # 在区间 (left, right) 内询问
        left = min(citations[0], n)
        right = min(citations[-1], n) + 1
        while left + 1 < right:  # 区间不为空
            # 循环不变量：
            # left 的回答一定为「是」
            # right 的回答一定为「否」
            mid = (left + right) // 2
            # 引用次数最多的 mid 篇论文，引用次数均 >= mid
            if citations[-mid] >= mid:
                left = mid  # 询问范围缩小到 (mid, right)
            else:
                right = mid  # 询问范围缩小到 (left, mid)
        # 根据循环不变量，left 现在是最大的回答为「是」的数
        return left
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(\log \min(n,U))$，其中 $n$ 是 $\textit{citations}$ 的长度，$U=\textit{citations}[n-1]$。
- 空间复杂度：$\mathcal{O}(1)$。

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

本题来自 `二、二分答案 / §2.2 求最大`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、二分答案 / §2.2 求最大`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
