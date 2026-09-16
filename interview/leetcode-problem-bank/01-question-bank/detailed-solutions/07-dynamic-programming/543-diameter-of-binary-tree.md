# 543. 二叉树的直径

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/diameter-of-binary-tree/
- 题目 slug：`diameter-of-binary-tree`
- 来源专题：动态规划
- 来源分类路径：十二、树形 DP / §12.1 树的直径
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/diameter-of-binary-tree/solutions/2227017/shi-pin-che-di-zhang-wo-zhi-jing-dpcong-taqma/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【视频】彻底掌握直径 DP，从二叉树到一般树（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/diameter-of-binary-tree/solutions/2227017/shi-pin-che-di-zhang-wo-zhi-jing-dpcong-taqma/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`shi-pin-che-di-zhang-wo-zhi-jing-dpcong-taqma`
- topic id：`2227017`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 视频讲解

本题视频讲解：[树形 DP【基础算法精讲 23】](https://www.bilibili.com/video/BV17o4y187h1/)，制作不易，欢迎点赞~

## 视频内容补充

设 $\textit{node}$ 是二叉树中的一个节点。

本题有两个关键概念：

- **链**：从 $\textit{node}$ 子树中的叶子节点到 $\textit{node}$ 的路径。把 $\textit{node}$ 子树中的最长链的长度，作为 $\textit{dfs}(\textit{node})$ 的返回值。
- **直径**：等价于由两条（或者一条）链拼成的路径。我们枚举二叉树的每个 $\textit{node}$，假设直径在这里「拐弯」，也就是计算由左右两条从下面的叶子节点到 $\textit{node}$ 的链的节点值之和，去更新答案的最大值。

⚠**注意**：直径可能在 $\textit{root}$ 下面的某个节点拐弯，不一定会经过 $\textit{root}$。

⚠**注意**：$\textit{dfs}(\textit{node})$ 返回的是**链**的长度，不是**直径**的长度。如果返回直径，那么在上面与其他的链继续拼接，得到的就不是直径了。

## 写法一

$\textit{dfs}(\textit{node})$ 返回的是 $\textit{node}$ 子树的最大链长，不包含 $\textit{node}$ 到其父节点的边。如果严格按照这个定义，空节点要返回 $-1$，这样叶子算出的链长才是 $0$。

如果你觉得这样写有些奇怪，可以看等价的写法二。

```py [sol-Python3]
class Solution:
    def diameterOfBinaryTree(self, root: Optional[TreeNode]) -> int:
        ans = 0

        # 返回 node 子树的最大链长
        def dfs(node: Optional[TreeNode]) -> int:
            if node is None:
                return -1  # 对于叶子来说，链长就是 -1+1=0
            l_len = dfs(node.left) + 1  # 左子树最大链长+1
            r_len = dfs(node.right) + 1  # 右子树最大链长+1
            nonlocal ans
            ans = max(ans, l_len + r_len)  # 两条链拼成路径
            return max(l_len, r_len)  # 当前子树最大链长

        dfs(root)
        return ans
```

```java [sol-Java]
class Solution {
    private int ans;

    public int diameterOfBinaryTree(TreeNode root) {
        dfs(root);
        return ans;
    }

    // 返回 node 子树的最大链长
    private int dfs(TreeNode node) {
        if (node == null) {
            return -1; // 对于叶子来说，链长就是 -1+1=0
        }
        int lLen = dfs(node.left) + 1; // 左子树最大链长+1
        int rLen = dfs(node.right) + 1; // 右子树最大链长+1
        ans = Math.max(ans, lLen + rLen); // 两条链拼成路径
        return Math.max(lLen, rLen); // 当前子树最大链长
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int diameterOfBinaryTree(TreeNode* root) {
        int ans = 0;

        // 返回 node 子树的最大链长
        auto dfs = [&](this auto&& dfs, TreeNode* node) -> int {
            if (node == nullptr) {
                return -1; // 对于叶子来说，链长就是 -1+1=0
            }
            int l_len = dfs(node->left) + 1; // 左子树最大链长+1
            int r_len = dfs(node->right) + 1; // 右子树最大链长+1
            ans = max(ans, l_len + r_len); // 两条链拼成路径
            return max(l_len, r_len); // 当前子树最大链长
        };

        dfs(root);
        return ans;
    }
};
```

```c [sol-C]
#define MAX(a, b) ((b) > (a) ? (b) : (a))

// 返回 node 子树的最大链长
int dfs(struct TreeNode* node, int* ans) {
    if (node == NULL) {
        return -1; // 对于叶子来说，链长就是 -1+1=0
    }
    int l_len = dfs(node->left, ans) + 1; // 左子树最大链长+1
    int r_len = dfs(node->right, ans) + 1; // 右子树最大链长+1
    *ans = MAX(*ans, l_len + r_len); // 两条链拼成路径
    return MAX(l_len, r_len); // 当前子树最大链长
}

int diameterOfBinaryTree(struct TreeNode* root) {
    int ans = 0;
    dfs(root, &ans);
    return ans;
}
```

```go [sol-Go]
func diameterOfBinaryTree(root *TreeNode) (ans int) {
    // 返回 node 子树的最大链长
    var dfs func(*TreeNode) int
    dfs = func(node *TreeNode) int {
        if node == nil {
            return -1 // 对于叶子来说，链长就是 -1+1=0
        }
        lLen := dfs(node.Left) + 1  // 左子树最大链长+1
        rLen := dfs(node.Right) + 1 // 右子树最大链长+1
        ans = max(ans, lLen+rLen)   // 两条链拼成路径
        return max(lLen, rLen)      // 当前子树最大链长
    }

    dfs(root)
    return
}
```

```js [sol-JavaScript]
var diameterOfBinaryTree = function(root) {
    let ans = 0;

    // 返回 node 子树的最大链长
    function dfs(node) {
        if (node === null) {
            return -1; // 对于叶子来说，链长就是 -1+1=0
        }
        const lLen = dfs(node.left) + 1; // 左子树最大链长+1
        const rLen = dfs(node.right) + 1; // 右子树最大链长+1
        ans = Math.max(ans, lLen + rLen); // 两条链拼成路径
        return Math.max(lLen, rLen); // 当前子树最大链长
    }

    dfs(root);
    return ans;
};
```

```rust [sol-Rust]
use std::rc::Rc;
use std::cell::RefCell;

impl Solution {
    pub fn diameter_of_binary_tree(root: Option<Rc<RefCell<TreeNode>>>) -> i32 {
        // 返回 node 子树的最大链长
        fn dfs(node: &Option<Rc<RefCell<TreeNode>>>, ans: &mut i32) -> i32 {
            let Some(node) = node else {
                return -1; // 对于叶子来说，链长就是 -1+1=0
            };
            let node = node.borrow();
            let l_len = dfs(&node.left, ans) + 1; // 左子树最大链长+1
            let r_len = dfs(&node.right, ans) + 1; // 右子树最大链长+1
            *ans = (*ans).max(l_len + r_len); // 两条链拼成路径
            l_len.max(r_len) // 当前子树最大链长
        }

        let mut ans = 0;
        dfs(&root, &mut ans);
        ans
    }
}
```

## 写法二

```py [sol-Python3]
class Solution:
    def diameterOfBinaryTree(self, root: Optional[TreeNode]) -> int:
        ans = 0

        def dfs(node: Optional[TreeNode]) -> int:
            if node is None:
                return 0
            l_len = dfs(node.left)
            r_len = dfs(node.right)
            nonlocal ans
            ans = max(ans, l_len + r_len)  # 两条链拼成路径
            return max(l_len, r_len) + 1

        dfs(root)
        return ans
```

```java [sol-Java]
class Solution {
    private int ans;

    public int diameterOfBinaryTree(TreeNode root) {
        dfs(root);
        return ans;
    }

    private int dfs(TreeNode node) {
        if (node == null) {
            return 0;
        }
        int lLen = dfs(node.left);
        int rLen = dfs(node.right);
        ans = Math.max(ans, lLen + rLen); // 两条链拼成路径
        return Math.max(lLen, rLen) + 1;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int diameterOfBinaryTree(TreeNode* root) {
        int ans = 0;

        auto dfs = [&](this auto&& dfs, TreeNode* node) -> int {
            if (node == nullptr) {
                return 0;
            }
            int l_len = dfs(node->left);
            int r_len = dfs(node->right);
            ans = max(ans, l_len + r_len); // 两条链拼成路径
            return max(l_len, r_len) + 1;
        };

        dfs(root);
        return ans;
    }
};
```

```c [sol-C]
#define MAX(a, b) ((b) > (a) ? (b) : (a))

int dfs(struct TreeNode* node, int* ans) {
    if (node == NULL) {
        return 0;
    }
    int l_len = dfs(node->left, ans);
    int r_len = dfs(node->right, ans);
    *ans = MAX(*ans, l_len + r_len); // 两条链拼成路径
    return MAX(l_len, r_len) + 1;
}

int diameterOfBinaryTree(struct TreeNode* root) {
    int ans = 0;
    dfs(root, &ans);
    return ans;
}
```

```go [sol-Go]
func diameterOfBinaryTree(root *TreeNode) (ans int) {
    var dfs func(*TreeNode) int
    dfs = func(node *TreeNode) int {
        if node == nil {
            return 0
        }
        lLen := dfs(node.Left)
        rLen := dfs(node.Right)
        ans = max(ans, lLen+rLen) // 两条链拼成路径
        return max(lLen, rLen) + 1
    }

    dfs(root)
    return
}
```

```js [sol-JavaScript]
var diameterOfBinaryTree = function(root) {
    let ans = 0;

    function dfs(node) {
        if (node === null) {
            return 0;
        }
        const lLen = dfs(node.left);
        const rLen = dfs(node.right);
        ans = Math.max(ans, lLen + rLen); // 两条链拼成路径
        return Math.max(lLen, rLen) + 1;
    }

    dfs(root);
    return ans;
};
```

```rust [sol-Rust]
use std::rc::Rc;
use std::cell::RefCell;

impl Solution {
    pub fn diameter_of_binary_tree(root: Option<Rc<RefCell<TreeNode>>>) -> i32 {
        fn dfs(node: &Option<Rc<RefCell<TreeNode>>>, ans: &mut i32) -> i32 {
            let Some(node) = node else {
                return 0;
            };
            let node = node.borrow();
            let l_len = dfs(&node.left, ans);
            let r_len = dfs(&node.right, ans);
            *ans = (*ans).max(l_len + r_len); // 两条链拼成路径
            l_len.max(r_len) + 1
        }

        let mut ans = 0;
        dfs(&root, &mut ans);
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是二叉树的节点个数。
- 空间复杂度：$\mathcal{O}(h)$，其中 $h$ 是二叉树的高度。递归需要 $\mathcal{O}(h)$ 的栈空间。

## 相似题目

见下面树题单的「**§2.6 二叉树的直径**」和「**§3.5 树的直径**」。

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

本题来自 `十二、树形 DP / §12.1 树的直径`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `十二、树形 DP / §12.1 树的直径`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
