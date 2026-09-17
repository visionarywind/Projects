# 1457. 二叉树中的伪回文路径

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/pseudo-palindromic-paths-in-a-binary-tree/
- 题目 slug：`pseudo-palindromic-paths-in-a-binary-tree`
- 来源专题：链表、树与回溯
- 来源分类路径：二、二叉树 / §2.7 回溯
- 难度分：1405
- 外部题解来源：https://leetcode.cn/problems/pseudo-palindromic-paths-in-a-binary-tree/solutions/2540903/yi-bu-bu-you-hua-cong-shu-zu-dao-wei-yun-hu0b/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[一步步优化：从数组到位运算（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/pseudo-palindromic-paths-in-a-binary-tree/solutions/2540903/yi-bu-bu-you-hua-cong-shu-zu-dao-wei-yun-hu0b/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`yi-bu-bu-you-hua-cong-shu-zu-dao-wei-yun-hu0b`
- topic id：`2540903`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:39:45 +0800

## 提示 1

题目允许把路径上的节点值重新排列。如果能排列成回文序列，路径上每个数字的**出现次数**需要满足什么性质？

## 提示 2

对于回文序列，按照长度的奇偶性分类讨论：

- 偶回文序列：左半边和右半边是一一对应的。比如 $11222211$，左半边有两个 $1$，右半边肯定也有两个 $1$。所以**每个数字的出现次数均为偶数**。对于长为偶数的路径，只要路径上的每个数字的出现次数均为偶数，就一定可以重新排列成回文序列。
- 奇回文序列：把正中间的那个数拿出来，就变成偶回文序列了。比如 $112232211$，拿出正中间的 $3$ 就和上面一样。所以**恰好有一个数字的出现次数为奇数，其余数字的出现次数均为偶数**。对于长为奇数的路径，只要路径上数字的出现次数满足该要求，就一定可以重新排列成回文序列。

如何维护出现次数的奇偶性？

有两种写法：数组 / 位运算。

#### 写法一：用数组维护

用一个数组 $p$ 维护路径上 $1$ 到 $9$ 每个数字出现次数的奇偶性。如果一个数出现偶数次，则记录为 $0$；如果出现奇数次，则记录为 $1$。

递归之前，所有数都出现 $0$ 次，所以数组元素都初始化成 $0$。

从根节点开始递归，假设我们遇到了 $5$，如果 $p[5]=0$，那么把 $p[5]$ 从 $0$ 改成 $1$，否则从 $1$ 改成 $0$，这可以用「异或 $1$」实现，即 `p[5] ^= 1`。

当递归到叶子时，如果发现数组 $p$ 中的 $1$ 的个数是 $0$ 个或者 $1$ 个，那么说明我们可以得到一个回文序列，返回 $1$，否则返回 $0$。

晕递归的同学可以看 [深刻理解递归](https://www.bilibili.com/video/BV1UD4y1Y769/)。

> 注：下面代码还可以额外用一个变量维护 $p$ 中 $1$ 的个数，从而省去统计 $1$ 的循环。

```py [sol-Python3]
class Solution:
    def pseudoPalindromicPaths(self, root: Optional[TreeNode]) -> int:
        p = [0] * 10

        def dfs(node: Optional[TreeNode]) -> int:
            if node is None:
                return 0
            p[node.val] ^= 1  # 修改 node.val 出现次数的奇偶性
            if node.left is None and node.right is None:  # node 是叶子节点
                res = 1 if sum(p) <= 1 else 0
            else:
                res = dfs(node.left) + dfs(node.right)
            # 恢复到递归 node 之前的状态（不做这一步就把 node.val 算到其它路径中了）
            p[node.val] ^= 1
            return res

        return dfs(root)
```

```java [sol-Java]
class Solution {
    public int pseudoPalindromicPaths(TreeNode root) {
        int[] p = new int[10];
        return dfs(root, p);
    }

    private int dfs(TreeNode node, int[] p) {
        if (node == null) {
            return 0;
        }
        p[node.val] ^= 1; // 修改 node.val 出现次数的奇偶性
        int res;
        if (node.left == null && node.right == null) { // node 是叶子节点
            res = count1(p) <= 1 ? 1 : 0;
        } else {
            res = dfs(node.left, p) + dfs(node.right, p);
        }
        // 恢复到递归 node 之前的状态（不做这一步就把 node.val 算到其它路径中了）
        p[node.val] ^= 1;
        return res;
    }

    private int count1(int[] p) {
        int cnt = 0;
        for (int x : p) {
            cnt += x;
        }
        return cnt;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int pseudoPalindromicPaths(TreeNode* root) {
        int p[10]{};
        auto dfs = [&](this auto&& dfs, TreeNode* node) {
            if (node == nullptr) {
                return 0;
            }
            p[node->val] ^= 1; // 修改 node->val 出现次数的奇偶性
            int res;
            if (node->left == nullptr && node->right == nullptr) { // node 是叶子节点
                res = reduce(p, p + 10) <= 1;
            } else {
                res = dfs(node->left) + dfs(node->right);
            }
            // 恢复到递归 node 之前的状态（不做这一步就把 node->val 算到其它路径中了）
            p[node->val] ^= 1;
            return res;
        };
        return dfs(root);
    }
};
```

```go [sol-Go]
func pseudoPalindromicPaths(root *TreeNode) int {
    p := make([]int, 10)
    return dfs(root, p)
}

func dfs(node *TreeNode, p []int) int {
    if node == nil {
        return 0
    }
    p[node.Val] ^= 1 // 修改 node.Val 出现次数的奇偶性
    // 返回前，恢复到递归 node 之前的状态（不做这一步就把 node.Val 算到其它路径中了）
    defer func() { p[node.Val] ^= 1 }()
    if node.Left == nil && node.Right == nil { // node 是叶子节点
        if sum(p) <= 1 {
            return 1
        }
        return 0
    }
    return dfs(node.Left, p) + dfs(node.Right, p)
}

func sum(p []int) (s int) {
    for _, x := range p {
        s += x
    }
    return
}
```

```js [sol-JavaScript]
var pseudoPalindromicPaths = function(root) {
    const p = Array(10).fill(0);

    function dfs(node) {
        if (node === null) {
            return 0;
        }
        p[node.val] ^= 1; // 修改 node.val 出现次数的奇偶性
        let res;
        if (node.left === null && node.right === null) { // node 是叶子节点
            res = _.sum(p) <= 1 ? 1 : 0;
        } else {
            res = dfs(node.left) + dfs(node.right);
        }
        // 恢复到递归 node 之前的状态（不做这一步就把 node.val 算到其它路径中了）
        p[node.val] ^= 1;
        return res;
    }

    return dfs(root);
};
```

```rust [sol-Rust]
use std::rc::Rc;
use std::cell::RefCell;

impl Solution {
    pub fn pseudo_palindromic_paths(root: Option<Rc<RefCell<TreeNode>>>) -> i32 {
        fn dfs(node: Option<&Rc<RefCell<TreeNode>>>, p: &mut [i32; 10]) -> i32 {
            if let Some(x) = node {
                let x = x.borrow();
                p[x.val as usize] ^= 1; // 修改 x.val 出现次数的奇偶性
                let res;
                if x.left.is_none() && x.right.is_none() { // x 是叶子节点
                    res = if p.iter().sum::<i32>() <= 1 { 1 } else { 0 };
                } else {
                    res = dfs(x.left.as_ref(), p) + dfs(x.right.as_ref(), p);
                };
                // 恢复到递归 x 之前的状态（不做这一步就把 x.val 算到其它路径中了）
                p[x.val as usize] ^= 1;
                return res;
            }
            0
        }
        let mut p = [0; 10];
        dfs(root.as_ref(), &mut p)
    }
}
```

#### 写法二：用位运算维护

由于 $p$ 中只有 $0$ 和 $1$，所以可以把 $p$ 压缩成一个二进制数 $\textit{mask}$，它的每个比特记录着 $0$ 和 $1$。

具体原理请看 [从集合论到位运算，常见位运算技巧分类总结！](https://leetcode.cn/circle/discuss/CaOJ45/)

如果 $\textit{mask}$ 中只有一个 $1$，那么去掉这个 $1$ 之后，$\textit{mask}$ 就变成 $0$ 了，所以可以用上面这篇文章中的「删除最小元素」的方法，判断

$$
\textit{mask} & (\textit{mask}-1) = 0
$$

是否成立，如果成立则说明 $\textit{mask}$ 中要么只有一个 $1$，要么全为 $0$。

```py [sol-Python3]
class Solution:
    def pseudoPalindromicPaths(self, root: Optional[TreeNode], mask=0) -> int:
        if root is None:
            return 0
        mask ^= 1 << root.val  # 修改 root.val 出现次数的奇偶性
        if root.left is None and root.right is None:  # root 是叶子节点
            return 1 if mask & (mask - 1) == 0 else 0
        return self.pseudoPalindromicPaths(root.left, mask) + \
               self.pseudoPalindromicPaths(root.right, mask)
```

```java [sol-Java]
class Solution {
    public int pseudoPalindromicPaths(TreeNode root) {
        return dfs(root, 0);
    }

    private int dfs(TreeNode root, int mask) {
        if (root == null) {
            return 0;
        }
        mask ^= 1 << root.val; // 修改 root.val 出现次数的奇偶性
        if (root.left == null && root.right == null) { // root 是叶子节点
            return (mask & (mask - 1)) == 0 ? 1 : 0;
        }
        return dfs(root.left, mask) + dfs(root.right, mask);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int pseudoPalindromicPaths(TreeNode* root, int mask = 0) {
        if (root == nullptr) {
            return 0;
        }
        mask ^= 1 << root->val; // 修改 root->val 出现次数的奇偶性
        if (root->left == nullptr && root->right == nullptr) { // root 是叶子节点
            return (mask & (mask - 1)) == 0;
        }
        return pseudoPalindromicPaths(root->left, mask) +
               pseudoPalindromicPaths(root->right, mask);
    }
};
```

```go [sol-Go]
func pseudoPalindromicPaths(root *TreeNode) int {
    return dfs(root, 0)
}

func dfs(root *TreeNode, mask int) int {
    if root == nil {
        return 0
    }
    mask ^= 1 << root.Val // 修改 root.Val 出现次数的奇偶性
    if root.Left == nil && root.Right == nil { // root 是叶子节点
        if mask&(mask-1) == 0 {
            return 1
        }
        return 0
    }
    return dfs(root.Left, mask) + dfs(root.Right, mask)
}
```

```js [sol-JavaScript]
var pseudoPalindromicPaths = function(root, mask = 0) {
    if (root === null) {
        return 0;
    }
    mask ^= 1 << root.val; // 修改 root.val 出现次数的奇偶性
    if (root.left === null && root.right === null) { // root 是叶子节点
        return (mask & (mask - 1)) === 0 ? 1 : 0;
    }
    return pseudoPalindromicPaths(root.left, mask) +
           pseudoPalindromicPaths(root.right, mask);
};
```

```rust [sol-Rust]
use std::rc::Rc;
use std::cell::RefCell;

impl Solution {
    pub fn pseudo_palindromic_paths(root: Option<Rc<RefCell<TreeNode>>>) -> i32 {
        fn dfs(node: Option<&Rc<RefCell<TreeNode>>>, mut mask: i32) -> i32 {
            if let Some(x) = node {
                let x = x.borrow();
                mask ^= 1 << x.val; // 修改 x.val 出现次数的奇偶性
                if x.left.is_none() && x.right.is_none() { // x 是叶子节点
                    return if (mask & (mask - 1)) == 0 { 1 } else { 0 };
                }
                return dfs(x.left.as_ref(), mask) + dfs(x.right.as_ref(), mask);
            }
            0
        }
        dfs(root.as_ref(), 0)
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为二叉树的节点个数。
- 空间复杂度：$\mathcal{O}(n)$。最坏情况下，二叉树退化成一条链，递归需要 $\mathcal{O}(n)$ 的栈空间。

## 思考题

如果改成从树中**任意**节点出发，到达**任意**节点，如何统计伪回文路径的个数？

如果改成路径上的边权是回文的，要怎么做？

这题是 [2791. 树中可以形成回文的路径数](https://leetcode.cn/problems/count-paths-that-can-form-a-palindrome-in-a-tree/)

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

本题来自 `二、二叉树 / §2.7 回溯`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、二叉树 / §2.7 回溯`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
