# 440. 字典序的第K小数字

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/k-th-smallest-in-lexicographical-order/
- 题目 slug：`k-th-smallest-in-lexicographical-order`
- 来源专题：常用数据结构
- 来源分类路径：六、字典树（trie） / §6.2 进阶
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/k-th-smallest-in-lexicographical-order/solutions/3696254/zai-shi-cha-shu-shang-zhao-di-k-xiao-olo-vzkl/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[在十叉树上找第 k 小，O(log n) 数学优化（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/k-th-smallest-in-lexicographical-order/solutions/3696254/zai-shi-cha-shu-shang-zhao-di-k-xiao-olo-vzkl/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`zai-shi-cha-shu-shang-zhao-di-k-xiao-olo-vzkl`
- topic id：`3696254`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

理解 [字典树](https://leetcode.cn/problems/implement-trie-prefix-tree/solutions/2993894/cong-er-cha-shu-dao-er-shi-liu-cha-shu-p-xsj4/) 的思想对本题有帮助。

本题可以看成一棵**十叉树**，示例 1 如下图：

![lc440.png](https://pic.leetcode.cn/1749421620-SKNNOp-lc440.png){:width=430}

> **注**：根节点至多有 $9$ 个儿子，非根节点至多有 $10$ 个儿子。所以是十叉树。

## 核心思路

先序遍历这棵树，等价于从左到右遍历字典序列表。所以先序遍历访问到的第 $k$ 个节点就是答案。

每访问一个节点，就把 $k$ 减一。当 $k=0$ 时，当前节点就是答案。

从根节点的第一个儿子 $1$ 开始。由于该节点被我们访问了，先把 $k$ 减一。

设子树 $1$ 的大小（节点个数）为 $\textit{size}$。分类讨论：

- 如果 $\textit{size}\le k$，那么答案不在子树 $1$ 中。然后考虑下一个儿子 $2$，依此类推。访问到下一个儿子时，要把 $k$ 减去 $(\textit{size}-1)+1 = \textit{size}$，这里 $\textit{size}-1$ 是因为节点 $1$ 已经访问过了。
- 否则，答案在子树 $1$ 中。首先访问 $1$ 的儿子 $10$，把 $k$ 减一。依次考虑 $1$ 的儿子 $10,11,12,\ldots,19$，计算子树大小，比较子树大小与 $k$ 的大小关系，依此类推。

**问**：为什么判断条件是 $\textit{size}\le k$ 而不是 $\textit{size}<k$？

**答**：因为当我们访问到子树根节点时，就把 $k$ 减一了。这时再算子树大小，就把根节点重复统计了，所以是 $\textit{size}-1<k$，即 $\textit{size}\le k$。

## 如何计算子树大小

子树每层的节点个数是有规律的，可以**逐层统计**。

比如 $n=1234$，计算子树 $1$ 的大小：

1. 第一层，只有 $1$ 一个节点。
2. 第二层，最小是 $10$，最大是 $19$，有 $10$ 个节点。
3. 第三层，最小是 $100$，最大是 $199$，有 $100$ 个节点。
4. 第四层，最小是 $1000$，最大是 $n=1234$，有 $n-1000+1=235$ 个节点。

所以一共有 $1+10+100+235=346$ 个节点。

上述过程如何用代码实现？

每一层的最小值是好算的：$1\to 10\to 100\to 1000$。

最大值呢？看上去不太好算。

不妨改为 $2\to 20\to 200\to \min(2000,n+1)$，这个过程中的每个数减一，就是每一层的最大值。

## 优化前

```py [sol-Python3]
class Solution:
    def findKthNumber(self, n: int, k: int) -> int:
        # 逐层统计 node 子树大小
        def count_subtree_size(node: int) -> int:
            size = 0
            left, right = node, node + 1
            while left <= n:
                # 这一层的最小值是 left，最大值是 min(right, n + 1) - 1
                size += min(right, n + 1) - left
                left *= 10  # 继续，计算下一层
                right *= 10
            return size

        node = 1
        k -= 1  # 访问节点 node
        while k > 0:
            size = count_subtree_size(node)
            if size <= k:  # 向右，跳过 node 子树
                node += 1  # 访问 node 右侧兄弟节点
                k -= size  # 访问子树中的每个节点，以及新的 node 节点
            else:  # 向下，深入 node 子树
                node *= 10  # 访问 node 的第一个儿子
                k -= 1  # 访问新的 node 节点
        return node
```

```java [sol-Java]
class Solution {
    public int findKthNumber(int n, int k) {
        int node = 1;
        k--; // 访问节点 node
        while (k > 0) {
            int size = countSubtreeSize(n, node);
            if (size <= k) { // 向右，跳过 node 子树
                node++; // 访问 node 右侧兄弟节点
                k -= size; // 访问子树中的每个节点，以及新的 node 节点
            } else { // 向下，深入 node 子树
                node *= 10; // 访问 node 的第一个儿子
                k--; // 访问新的 node 节点
            }
        }
        return node;
    }

    // 逐层统计 node 子树大小
    private int countSubtreeSize(int n, int node) {
        // 子树大小不会超过 n，所以 size 用 int 类型
        // 但计算过程中的 left 和 right 会超过 int，所以用 long 类型
        int size = 0;
        long left = node;
        long right = node + 1;
        while (left <= n) {
            // 这一层的最小值是 left，最大值是 min(right, n + 1) - 1
            size += Math.min(right, n + 1) - left;
            left *= 10; // 继续，计算下一层
            right *= 10;
        }
        return size;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int findKthNumber(int n, int k) {
        // 逐层统计 node 子树大小
        auto count_subtree_size = [&](int node) -> int {
            // 子树大小不会超过 n，所以 size 用 int 类型
            // 但计算过程中的 left 和 right 会超过 int，所以用 long long 类型
            int size = 0;
            long long left = node, right = node + 1;
            while (left <= n) {
                // 这一层的最小值是 left，最大值是 min(right, n + 1) - 1
                size += min(right, n + 1LL) - left;
                left *= 10; // 继续，计算下一层
                right *= 10;
            }
            return size;
        };

        int node = 1;
        k--; // 访问节点 node
        while (k > 0) {
            int size = count_subtree_size(node);
            if (size <= k) { // 向右，跳过 node 子树
                node++; // 访问 node 右侧兄弟节点
                k -= size; // 访问子树中的每个节点，以及新的 node 节点
            } else { // 向下，深入 node 子树
                node *= 10; // 访问 node 的第一个儿子
                k--; // 访问新的 node 节点
            }
        }
        return node;
    }
};
```

```c [sol-C]
#define MIN(a, b) ((b) < (a) ? (b) : (a))

int findKthNumber(int n, int k) {
    int count_subtree_size(int node) {
        // 子树大小不会超过 n，所以 size 用 int 类型
        // 但计算过程中的 left 和 right 会超过 int，所以用 long long 类型
        int size = 0;
        long long left = node, right = node + 1;
        while (left <= n) {
            // 这一层的最小值是 left，最大值是 MIN(right, n + 1) - 1
            size += MIN(right, n + 1) - left;
            left *= 10; // 继续，计算下一层
            right *= 10;
        }
        return size;
    }

    int node = 1;
    k--; // 访问节点 node
    while (k > 0) {
        int size = count_subtree_size(node);
        if (size <= k) { // 向右，跳过 node 子树
            node++; // 访问 node 右侧兄弟节点
            k -= size; // 访问子树中的每个节点，以及新的 node 节点
        } else { // 向下，深入 node 子树
            node *= 10; // 访问 node 的第一个儿子
            k--; // 访问新的 node 节点
        }
    }
    return node;
}
```

```go [sol-Go]
func findKthNumber(n, k int) int {
    // 逐层统计 node 子树大小
    countSubtreeSize := func(node int) (size int) {
        left, right := node, node+1
        for left <= n {
            // 这一层的最小值是 left，最大值是 min(right, n+1) - 1
            size += min(right, n+1) - left
            left *= 10 // 继续，计算下一层
            right *= 10
        }
        return
    }

    node := 1
    k-- // 访问节点 node
    for k > 0 {
        size := countSubtreeSize(node)
        if size <= k { // 向右，跳过 node 子树
            node++    // 访问 node 右侧兄弟节点
            k -= size // 访问子树中的每个节点，以及新的 node 节点
        } else { // 向下，深入 node 子树
            node *= 10 // 访问 node 的第一个儿子
            k--        // 访问新的 node 节点
        }
    }
    return node
}
```

```js [sol-JavaScript]
var findKthNumber = function(n, k) {
    // 逐层统计 node 子树大小
    function countSubtreeSize(node) {
        let size = 0;
        let left = node, right = node + 1;
        while (left <= n) {
            // 这一层的最小值是 left，最大值是 min(right, n + 1) - 1
            size += Math.min(right, n + 1) - left;
            left *= 10; // 继续，计算下一层
            right *= 10;
        }
        return size;
    }

    let node = 1;
    k--; // 访问节点 node
    while (k > 0) {
        const size = countSubtreeSize(node);
        if (size <= k) { // 向右，跳过 node 子树
            node++; // 访问 node 右侧兄弟节点
            k -= size; // 访问子树中的每个节点，以及新的 node 节点
        } else { // 向下，深入 node 子树
            node *= 10; // 访问 node 的第一个儿子
            k--; // 访问新的 node 节点
        }
    }
    return node;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn find_kth_number(n: i32, mut k: i32) -> i32 {
        // 逐层统计 node 子树大小
        let count_subtree_size = |node: i32| -> i32 {
            let n = n as i64;
            // 子树大小不会超过 n，所以 size 用 i32 类型
            // 但计算过程中的 left 和 right 会超过 i32，所以用 i64 类型
            let mut size = 0;
            let mut left = node as i64;
            let mut right = node as i64 + 1;
            while left <= n {
                // 这一层的最小值是 left，最大值是 min(right, n + 1) - 1
                size += (right.min(n + 1) - left) as i32;
                left *= 10; // 继续，计算下一层
                right *= 10;
            }
            size
        };

        let mut node = 1;
        k -= 1; // 访问节点 node
        while k > 0 {
            let size = count_subtree_size(node);
            if size <= k {
                node += 1; // 访问 node 右侧兄弟节点
                k -= size; // 访问子树中的每个节点，以及新的 node 节点
            } else {
                node *= 10; // 访问 node 的第一个儿子
                k -= 1; // 访问新的 node 节点
            }
        }
        node
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(D\log^2 n)$，其中 $D=10$。整棵树有 $\mathcal{O}(\log n)$ 层，每层会遍历至多 $D$ 个节点，每个节点需要 $\mathcal{O}(\log n)$ 的时间计算子树大小。
- 空间复杂度：$\mathcal{O}(1)$。

## 优化

对于统计子树大小的算法，我举了一个 $n=1234$ 的例子，最终算出子树大小为 $1+10+100+235=346$。

这里面的 $1+10+100$ 是个等比数列之和，结果为 $11\cdots 1 = \dfrac{10^m-1}{9}$。其中 $m$ 为 $n$ 的十进制长度减去当前节点的深度（根节点的深度为 $0$）。注意我们一开始访问的是节点 $1$，不是根节点。

我们可以在访问节点的过程中，维护 $10^m$，这样只需要计算最后一层的节点个数。

```py [sol-Python3]
class Solution:
    def findKthNumber(self, n: int, k: int) -> int:
        pow10 = 1  # 直接跳到子树 1 最后一层，left 和 right 需要乘以 pow10
        x = n // 10
        while x > 0:
            pow10 = pow10 * 10
            x //= 10

        # 统计 node 子树大小，O(1)
        def count_subtree_size(node: int) -> int:
            size = (pow10 - 1) // 9  # 1+10+100+...
            left, right = node * pow10, (node + 1) * pow10
            if left <= n:
                size += min(right, n + 1) - left  # 余项
            return size

        node = 1
        k -= 1  # 访问节点 node
        while k > 0:
            size = count_subtree_size(node)
            if size <= k:  # 向右，跳过 node 子树
                node += 1  # 访问 node 右侧兄弟节点
                k -= size  # 访问子树中的每个节点，以及新的 node 节点
            else:  # 向下，深入 node 子树
                pow10 //= 10  # 子树层数减一
                node *= 10  # 访问 node 的第一个儿子
                k -= 1  # 访问新的 node 节点
        return node
```

```java [sol-Java]
class Solution {
    public int findKthNumber(int n, int k) {
        int pow10 = 1; // 直接跳到子树 1 最后一层，left 和 right 需要乘以 pow10
        for (int x = n / 10; x > 0; x /= 10) {
            pow10 *= 10;
        }

        int node = 1;
        k--; // 访问节点 node
        while (k > 0) {
            int size = countSubtreeSize(node, n, pow10);
            if (size <= k) { // 向右，跳过 node 子树
                node++; // 访问 node 右侧兄弟节点
                k -= size; // 访问子树中的每个节点，以及新的 node 节点
            } else { // 向下，深入 node 子树
                pow10 /= 10; // 子树层数减一
                node *= 10; // 访问 node 的第一个儿子
                k--; // 访问新的 node 节点
            }
        }
        return node;
    }

    // 统计 node 子树大小，O(1)
    private int countSubtreeSize(int node, int n, int pow10) {
        int size = (pow10 - 1) / 9; // 1+10+100+...
        long left = (long) node * pow10;
        long right = (long) (node + 1) * pow10;
        if (left <= n) {
            size += Math.min(right, n + 1) - left; // 余项
        }
        return size;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int findKthNumber(int n, int k) {
        int pow10 = 1; // 直接跳到子树 1 最后一层，left 和 right 需要乘以 pow10
        for (int x = n / 10; x > 0; x /= 10) {
            pow10 *= 10;
        }

        // 统计 node 子树大小，O(1)
        auto count_subtree_size = [&](int node) -> int {
            int size = (pow10 - 1) / 9; // 1+10+100+...
            long long left = 1LL * node * pow10;
            long long right = 1LL * (node + 1) * pow10;
            if (left <= n) {
                size += min(right, n + 1LL) - left; // 余项
            }
            return size;
        };

        int node = 1;
        k--; // 访问节点 node
        while (k > 0) {
            int size = count_subtree_size(node);
            if (size <= k) { // 向右，跳过 node 子树
                node++; // 访问 node 右侧兄弟节点
                k -= size; // 访问子树中的每个节点，以及新的 node 节点
            } else { // 向下，深入 node 子树
                pow10 /= 10; // 子树层数减一
                node *= 10; // 访问 node 的第一个儿子
                k--; // 访问新的 node 节点
            }
        }
        return node;
    }
};
```

```c [sol-C]
#define MIN(a, b) ((b) < (a) ? (b) : (a))

int findKthNumber(int n, int k) {
    int pow10 = 1; // 直接跳到子树 1 最后一层，left 和 right 需要乘以 pow10
    for (int x = n / 10; x > 0; x /= 10) {
        pow10 *= 10;
    }

    // 统计 node 子树大小，O(1)
    int count_subtree_size(int node) {
        int size = (pow10 - 1) / 9; // 1+10+100+...
        long long left = 1LL * node * pow10;
        long long right = 1LL * (node + 1) * pow10;
        if (left <= n) {
            size += MIN(right, n + 1) - left; // 余项
        }
        return size;
    }

    int node = 1;
    k--; // 访问节点 node
    while (k > 0) {
        int size = count_subtree_size(node);
        if (size <= k) { // 向右，跳过 node 子树
            node++; // 访问 node 右侧兄弟节点
            k -= size; // 访问子树中的每个节点，以及新的 node 节点
        } else { // 向下，深入 node 子树
            pow10 /= 10; // 子树层数减一
            node *= 10; // 访问 node 的第一个儿子
            k--; // 访问新的 node 节点
        }
    }
    return node;
}
```

```go [sol-Go]
func findKthNumber(n, k int) int {
    pow10 := 1 // 直接跳到子树 1 最后一层，left 和 right 需要乘以 pow10
    for x := n / 10; x > 0; x /= 10 {
        pow10 *= 10
    }

    // 统计 node 子树大小，O(1)
    countSubtreeSize := func(node int) int {
        size := (pow10 - 1) / 9 // 1+10+100+...
        left, right := node*pow10, (node+1)*pow10
        if left <= n {
            size += min(right, n+1) - left // 余项
        }
        return size
    }

    node := 1
    k-- // 访问节点 node
    for k > 0 {
        size := countSubtreeSize(node)
        if size <= k { // 向右，跳过 node 子树
            node++    // 访问 node 右侧兄弟节点
            k -= size // 访问子树中的每个节点，以及新的 node 节点
        } else { // 向下，深入 node 子树
            pow10 /= 10 // 子树层数减一
            node *= 10  // 访问 node 的第一个儿子
            k--         // 访问新的 node 节点
        }
    }
    return node
}
```

```js [sol-JavaScript]
var findKthNumber = function(n, k) {
    let pow10 = 1; // 直接跳到子树 1 最后一层，left 和 right 需要乘以 pow10
    for (let x = Math.floor(n / 10); x > 0; x = Math.floor(x / 10)) {
        pow10 *= 10;
    }

    // 统计 node 子树大小，O(1)
    function countSubtreeSize(node) {
        let size = (pow10 - 1) / 9; // 1+10+100+...
        const left = node * pow10;
        const right = (node + 1) * pow10;
        if (left <= n) {
            size += Math.min(right, n + 1) - left; // 余项
        }
        return size;
    }

    let node = 1;
    k--; // 访问节点 node
    while (k > 0) {
        const size = countSubtreeSize(node);
        if (size <= k) { // 向右，跳过 node 子树
            node++; // 访问 node 右侧兄弟节点
            k -= size; // 访问子树中的每个节点，以及新的 node 节点
        } else { // 向下，深入 node 子树
            pow10 /= 10; // 子树层数减一
            node *= 10; // 访问 node 的第一个儿子
            k--; // 访问新的 node 节点
        }
    }
    return node;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn find_kth_number(n: i32, mut k: i32) -> i32 {
        let mut pow10 = 1; // 直接跳到子树 1 最后一层，left 和 right 需要乘以 pow10
        let mut x = n / 10;
        while x > 0 {
            pow10 *= 10;
            x /= 10;
        }

        // 统计 node 子树大小，O(1)
        let count_subtree_size = |node: i32, pow10: i32| -> i32 {
            let n = n as i64;
            let mut size = (pow10 - 1) / 9; // 1+10+100+...
            let left = node as i64 * pow10 as i64;
            let right = (node + 1) as i64 * pow10 as i64;
            if left <= n {
                size += (right.min(n + 1) - left) as i32; // 余项
            }
            size
        };

        let mut node = 1;
        k -= 1; // 访问节点 node
        while k > 0 {
            let size = count_subtree_size(node, pow10);
            if size <= k { // 向右，跳过 node 子树
                node += 1; // 访问 node 右侧兄弟节点
                k -= size; // 访问子树中的每个节点，以及新的 node 节点
            } else { // 向下，深入 node 子树
                pow10 /= 10; // 子树层数减一
                node *= 10; // 访问 node 的第一个儿子
                k -= 1; // 访问新的 node 节点
            }
        }
        node
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(D\log n)$，其中 $D=10$。
- 空间复杂度：$\mathcal{O}(1)$。

## 还能优化？

1. 可以在每一层二分，计算要从第几个儿子往下。时间复杂度 $\mathcal{O}(\log D \log n)$。
2. 解不等式，直接求出要从第几个儿子往下。时间复杂度 $\mathcal{O}(\log n)$。

留给感兴趣的读者思考。

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

本题来自 `六、字典树（trie） / §6.2 进阶`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `六、字典树（trie） / §6.2 进阶`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
