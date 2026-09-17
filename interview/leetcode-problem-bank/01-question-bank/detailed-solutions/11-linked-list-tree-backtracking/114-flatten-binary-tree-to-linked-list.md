# 114. 二叉树展开为链表

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/flatten-binary-tree-to-linked-list/
- 题目 slug：`flatten-binary-tree-to-linked-list`
- 来源专题：链表、树与回溯
- 来源分类路径：二、二叉树 / §2.14 链表+二叉树
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/flatten-binary-tree-to-linked-list/solutions/2992172/liang-chong-fang-fa-tou-cha-fa-fen-zhi-p-h9bg/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：头插法/分治（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/flatten-binary-tree-to-linked-list/solutions/2992172/liang-chong-fang-fa-tou-cha-fa-fen-zhi-p-h9bg/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`liang-chong-fang-fa-tou-cha-fa-fen-zhi-p-h9bg`
- topic id：`2992172`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:49:15 +0800

## 方法一：头插法

![lc114.jpg](https://pic.leetcode.cn/1731893896-gKXngM-lc114.jpg)

采用**头插法**构建链表，也就是从节点 $6$ 开始，在 $6$ 的前面插入 $5$，在 $5$ 的前面插入 $4$，依此类推。

为此，要按照 $6\to 5\to 4\to 3\to 2\to 1$ 的顺序访问节点。如何遍历二叉树，才能实现这个顺序？

既然 $1\to 2\to 3\to 4\to 5\to 6$ 是先序遍历，那么 $6\to 5\to 4\to 3\to 2\to 1$ 就是先序遍历的逆序，即按照**右 - 左 - 根**的顺序遍历二叉树。

遍历的同时执行头插法，把当前节点插在链表头节点 $\textit{head}$ 的前面，然后更新 $\textit{head}$ 为当前节点。一开始 $\textit{head}$ 是空节点。

```py [sol-Python3]
class Solution:
    head = None

    def flatten(self, root: Optional[TreeNode]) -> None:
        if root is None:
            return
        # 右 - 左 - 根
        self.flatten(root.right)
        self.flatten(root.left)
        root.left = None
        root.right = self.head  # 在头节点 head 的前面插入 root
        self.head = root  # 现在头节点是 root
```

```java [sol-Java]
class Solution {
    private TreeNode head;

    public void flatten(TreeNode root) {
        if (root == null) {
            return;
        }
        // 右 - 左 - 根
        flatten(root.right);
        flatten(root.left);
        root.left = null;
        root.right = head; // 在头节点 head 的前面插入 root
        head = root; // 现在头节点是 root
    }
}
```

```cpp [sol-C++]
class Solution {
    TreeNode* head = nullptr;

public:
    void flatten(TreeNode* root) {
        if (root == nullptr) {
            return;
        }
        // 右 - 左 - 根
        flatten(root->right);
        flatten(root->left);
        root->left = nullptr;
        root->right = head; // 在头节点 head 的前面插入 root
        head = root; // 现在头节点是 root
    }
};
```

```c [sol-C]
void flatten(struct TreeNode* root) {
    struct TreeNode* head = NULL;

    void dfs(struct TreeNode* node) {
        if (node == NULL) {
            return;
        }
        // 右 - 左 - 根
        dfs(node->right);
        dfs(node->left);
        node->left = NULL;
        node->right = head; // 在头节点 head 的前面插入 node
        head = node; // 现在头节点是 node
    }

    dfs(root);
}
```

```c [sol-C 写法二]
void dfs(struct TreeNode* node, struct TreeNode** head) {
    if (node == NULL) {
        return;
    }
    // 右 - 左 - 根
    dfs(node->right, head);
    dfs(node->left, head);
    node->left = NULL;
    node->right = *head; // 在头节点 head 的前面插入 node
    *head = node; // 现在头节点是 node
}

void flatten(struct TreeNode* root) {
    struct TreeNode* head = NULL;
    dfs(root, &head);
}
```

```go [sol-Go]
func flatten(root *TreeNode) {
    var head *TreeNode

    var dfs func(*TreeNode)
    dfs = func(node *TreeNode) {
        if node == nil {
            return
        }
        // 右 - 左 - 根
        dfs(node.Right)
        dfs(node.Left)
        node.Left = nil
        node.Right = head // 在头节点 head 的前面插入 node
        head = node       // 现在头节点是 node
    }

    dfs(root)
}
```

```js [sol-JavaScript]
var flatten = function(root) {
    let head = null;

    function dfs(node) {
        if (node === null) {
            return;
        }
        // 右 - 左 - 根
        dfs(node.right);
        dfs(node.left);
        node.left = null;
        node.right = head; // 在头节点 head 的前面插入 node
        head = node; // 现在头节点是 node
    }

    dfs(root);
};
```

```rust [sol-Rust]
use std::cell::RefCell;
use std::rc::Rc;

impl Solution {
    fn dfs(node: &Option<Rc<RefCell<TreeNode>>>, head: &mut Option<Rc<RefCell<TreeNode>>>) {
        if let Some(x) = node {
            let mut x = x.borrow_mut();
            // 右 - 左 - 根
            Self::dfs(&x.right, head);
            Self::dfs(&x.left, head);
            x.left = None;
            x.right = head.take(); // 在头节点 head 的前面插入 node
            *head = node.clone(); // 现在头节点是 node
        }
    }

    pub fn flatten(root: &mut Option<Rc<RefCell<TreeNode>>>) { // mut 可以去掉
        let mut head = None;
        Self::dfs(&root, &mut head);
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是二叉树的节点个数。
- 空间复杂度：$\mathcal{O}(n)$。递归需要 $\mathcal{O}(n)$ 的栈空间。

## 方法二：分治

方法一需要用到一个在 DFS 之外的变量 $\textit{head}$，能否只在 DFS 中解决呢？

![lc114.jpg](https://pic.leetcode.cn/1731893896-gKXngM-lc114.jpg)

考虑分治，假如我们计算出了 $\textit{root} = 1$ 左子树的链表 $2\to 3\to 4$，以及右子树的链表 $5\to 6$，那么接下来只需要穿针引线，把节点 $1$ 和两条链表连起来：

1. 先把 $2\to 3\to 4$ 和 $5\to 6$ 连起来，也就是左子树链表尾节点 $4$ 的 $\textit{right}$ 更新为节点 $5$（即 $\textit{root}.\textit{right}$），得到 $2\to 3\to 4\to 5\to 6$。
2. 然后把 $1$ 和 $2\to 3\to 4\to 5\to 6$ 连起来，也就是节点 $1$ 的 $\textit{right}$ 更新为节点 $2$（即 $\textit{root}.\textit{left}$），得到 $1\to 2\to 3\to 4\to 5\to 6$。
3. 最后把 $\textit{root}.\textit{left}$ 置为空。

上面的过程，我们需要知道左子树链表的尾节点 $4$。所以 DFS 需要返回链表的尾节点。

链表合并完成后，返回合并后的链表的尾节点，也就是右子树链表的尾节点。如果右子树是空的，则返回左子树链表的尾节点。如果左右子树都是空的，返回当前节点。

```py [sol-Python3]
class Solution:
    def flatten(self, root: Optional[TreeNode]) -> Optional[TreeNode]:
        if root is None:
            return None
        left_tail = self.flatten(root.left)
        right_tail = self.flatten(root.right)
        if left_tail:
            left_tail.right = root.right  # 左子树链表的尾节点 -> 右子树链表的头节点
            root.right = root.left  # root -> 左子树链表的头节点
            root.left = None
        return right_tail or left_tail or root
```

```java [sol-Java]
class Solution {
    public void flatten(TreeNode root) {
        dfs(root);
    }

    private TreeNode dfs(TreeNode root) {
        if (root == null) {
            return null;
        }
        TreeNode leftTail = dfs(root.left);
        TreeNode rightTail = dfs(root.right);
        if (leftTail != null) {
            leftTail.right = root.right; // 左子树链表的尾节点 -> 右子树链表的头节点
            root.right = root.left; // root -> 左子树链表的头节点
            root.left = null;
        }
        return rightTail != null ? rightTail : leftTail != null ? leftTail : root;
    }
}
```

```cpp [sol-C++]
class Solution {
    TreeNode* dfs(TreeNode* root) {
        if (root == nullptr) {
            return nullptr;
        }
        TreeNode* left_tail = dfs(root->left);
        TreeNode* right_tail = dfs(root->right);
        if (left_tail) {
            left_tail->right = root->right; // 左子树链表的尾节点 -> 右子树链表的头节点
            root->right = root->left; // root -> 左子树链表的头节点
            root->left = nullptr;
        }
        return right_tail ? right_tail : left_tail ? left_tail : root;
    }

public:
    void flatten(TreeNode* root) {
        dfs(root);
    }
};
```

```c [sol-C]
struct TreeNode* dfs(struct TreeNode* root) {
    if (root == NULL) {
        return NULL;
    }
    struct TreeNode* left_tail = dfs(root->left);
    struct TreeNode* right_tail = dfs(root->right);
    if (left_tail) {
        left_tail->right = root->right; // 左子树链表的尾节点 -> 右子树链表的头节点
        root->right = root->left; // root -> 左子树链表的头节点
        root->left = NULL;
    }
    return right_tail ? right_tail : left_tail ? left_tail : root;
}

void flatten(struct TreeNode* root) {
    dfs(root);
}
```

```go [sol-Go]
func dfs(root *TreeNode) *TreeNode {
    if root == nil {
        return nil
    }
    leftTail := dfs(root.Left)
    rightTail := dfs(root.Right)
    if leftTail != nil {
        leftTail.Right = root.Right // 左子树链表的尾节点 -> 右子树链表的头节点
        root.Right = root.Left      // root -> 左子树链表的头节点
        root.Left = nil
    }
    if rightTail != nil {
        return rightTail
    }
    if leftTail != nil {
        return leftTail
    }
    return root
}

func flatten(root *TreeNode) {
    dfs(root)
}
```

```js [sol-JavaScript]
var flatten = function(root) {
    if (root === null) {
        return null;
    }
    const leftTail = flatten(root.left);
    const rightTail = flatten(root.right);
    if (leftTail) {
        leftTail.right = root.right; // 左子树链表的尾节点 -> 右子树链表的头节点
        root.right = root.left; // root -> 左子树链表的头节点
        root.left = null;
    }
    return rightTail ?? leftTail ?? root;
};
```

```rust [sol-Rust]
use std::cell::RefCell;
use std::rc::Rc;

impl Solution {
    fn dfs(node: &Option<Rc<RefCell<TreeNode>>>) -> Option<Rc<RefCell<TreeNode>>> {
        if let Some(x) = node {
            let mut x = x.borrow_mut();
            let left_tail = Self::dfs(&x.left);
            let right_tail = Self::dfs(&x.right);
            if let Some(lt) = &left_tail {
                lt.borrow_mut().right = x.right.take(); // 左子树链表的尾节点 -> 右子树链表的头节点
                x.right = x.left.take(); // x -> 左子树链表的头节点
            }
            return right_tail.or(left_tail).or(node.clone());
        }
        None
    }

    pub fn flatten(root: &mut Option<Rc<RefCell<TreeNode>>>) { // mut 可以去掉
        Self::dfs(root);
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是二叉树的节点个数。
- 空间复杂度：$\mathcal{O}(n)$。递归需要 $\mathcal{O}(n)$ 的栈空间。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. 【本题相关】[链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `二、二叉树 / §2.14 链表+二叉树`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、二叉树 / §2.14 链表+二叉树`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
