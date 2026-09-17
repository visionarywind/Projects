# 94. 二叉树的中序遍历

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/binary-tree-inorder-traversal/
- 题目 slug：`binary-tree-inorder-traversal`
- 来源专题：链表、树与回溯
- 来源分类路径：二、二叉树 / §2.1 遍历二叉树
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/binary-tree-inorder-traversal/solutions/3844031/tu-jie-morris-bian-li-xian-suo-er-cha-sh-cm2s/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【图解】Morris 遍历（线索二叉树），一图秒懂！（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/binary-tree-inorder-traversal/solutions/3844031/tu-jie-morris-bian-li-xian-suo-er-cha-sh-cm2s/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tu-jie-morris-bian-li-xian-suo-er-cha-sh-cm2s`
- topic id：`3844031`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:30:25 +0800

什么是中序遍历？

二叉树有三种常见的遍历方式：

- **前序遍历**：根-左-右。先获取根节点值，再访问根的左子树，最后访问根的右子树。
- **中序遍历**：左-根-右。先访问根的左子树，再获取根节点值，最后访问根的右子树。
- **后序遍历**：左-右-根。先访问根的左子树，再访问根的右子树，最后获取根节点值。

对于中序遍历，我们有如下递归代码。不了解递归的同学，可以看视频讲解[【基础算法精讲 09】](https://www.bilibili.com/video/BV1UD4y1Y769/)。

```py [sol-Python3]
class Solution:
    def inorderTraversal(self, root: Optional[TreeNode]) -> List[int]:
        def dfs(node: Optional[TreeNode]) -> None:
            if node is None:
                return
            dfs(node.left)       # 左
            ans.append(node.val) # 根（这行代码移到前面就是前序，移到后面就是后序）
            dfs(node.right)      # 右

        ans = []
        dfs(root)
        return ans
```

```java [sol-Java]
class Solution {
    public List<Integer> inorderTraversal(TreeNode root) {
        List<Integer> ans = new ArrayList<>();
        dfs(ans, root);
        return ans;
    }

    private void dfs(List<Integer> ans, TreeNode node) {
        if (node == null) {
            return;
        }
        dfs(ans, node.left);  // 左
        ans.add(node.val);    // 根（这行代码移到前面就是前序，移到后面就是后序）
        dfs(ans, node.right); // 右
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> inorderTraversal(TreeNode* root) {
        vector<int> ans;

        // lambda 递归
        auto dfs = [&](this auto&& dfs, TreeNode* node) -> void {
            if (node == nullptr) {
                return;
            }
            dfs(node->left);          // 左
            ans.push_back(node->val); // 根（这行代码移到前面就是前序，移到后面就是后序）
            dfs(node->right);         // 右
        };

        dfs(root);
        return ans;
    }
};
```

```c [sol-C]
int* inorderTraversal(struct TreeNode* root, int* returnSize) {
    int* ans = malloc(100 * sizeof(int)); // 题目保证节点个数 <= 100
    *returnSize = 0;

    void dfs(struct TreeNode* node) {
        if (node == NULL) {
            return;
        }
        dfs(node->left);                  // 左
        ans[(*returnSize)++] = node->val; // 根（这行代码移到前面就是前序，移到后面就是后序）
        dfs(node->right);                 // 右
    }

    dfs(root);
    return ans;
}
```

```c [sol-C 写法二]
// 获取树的大小（节点个数）
int getSize(struct TreeNode* root) {
    if (root == NULL) {
        return 0;
    }
    return 1 + getSize(root->left) + getSize(root->right);
}

int* inorderTraversal(struct TreeNode* root, int* returnSize) {
    int* ans = malloc(getSize(root) * sizeof(int));
    *returnSize = 0;

    void dfs(struct TreeNode* node) {
        if (node == NULL) {
            return;
        }
        dfs(node->left);                  // 左
        ans[(*returnSize)++] = node->val; // 根（这行代码移到前面就是前序，移到后面就是后序）
        dfs(node->right);                 // 右
    }

    dfs(root);
    return ans;
}
```

```go [sol-Go]
func inorderTraversal(root *TreeNode) (ans []int) {
    var dfs func(*TreeNode)
    dfs = func(node *TreeNode) {
        if node == nil {
            return
        }
        dfs(node.Left)              // 左
        ans = append(ans, node.Val) // 根（这行代码移到前面就是前序，移到后面就是后序）
        dfs(node.Right)             // 右
    }
    dfs(root)
    return
}
```

```js [sol-JavaScript]
var inorderTraversal = function(root) {
    function dfs(node) {
        if (node === null) {
            return;
        }
        dfs(node.left);     // 左
        ans.push(node.val); // 根（这行代码移到前面就是前序，移到后面就是后序）
        dfs(node.right);    // 右
    }

    const ans = [];
    dfs(root);
    return ans;
};
```

```rust [sol-Rust]
use std::rc::Rc;
use std::cell::RefCell;

impl Solution {
    pub fn inorder_traversal(root: Option<Rc<RefCell<TreeNode>>>) -> Vec<i32> {
        fn dfs(node: &Option<Rc<RefCell<TreeNode>>>, ans: &mut Vec<i32>) {
            if let Some(node) = node {
                let n = node.borrow();
                dfs(&n.left, ans);  // 左
                ans.push(n.val);    // 根（这行代码移到前面就是前序，移到后面就是后序）
                dfs(&n.right, ans); // 右
            }
        }

        let mut ans = vec![];
        dfs(&root, &mut ans);
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是二叉树的节点个数。
- 空间复杂度：$\mathcal{O}(h)$，其中 $h$ 是二叉树的高度。递归需要 $\mathcal{O}(h)$ 的栈空间。返回值不计入。

能不能做到 $\mathcal{O}(1)$ 空间？不能写递归，也不能用栈模拟递归。

请看下图：

![lc94-2c.png](https://pic.leetcode.cn/1764305638-aNVMyr-lc94-2c.png){:width=900px}

```py [sol-Python3]
class Solution:
    def inorderTraversal(self, root: Optional[TreeNode]) -> List[int]:
        ans = []

        while root:
            if root.left:
                # 找 root 的前驱 pre：在中序遍历中，root 的上一个节点
                # 从 root.left 开始，一直向右走，直到走到尽头，或者遇到指向 root 的线索（回到 root 的路）
                pre = root.left
                while pre.right and pre.right is not root:
                    pre = pre.right

                # root 的左子树尚未访问
                if pre.right is None:
                    pre.right = root  # 建立线索（回到 root 的路），相当于把 pre.right 当作栈
                    root = root.left  # 访问左子树
                    continue

                # root 的左子树访问完毕，去掉线索，恢复原样
                pre.right = None  # 注：如果调用完 inorderTraversal 不再使用这棵二叉树，这行代码可以去掉

            # root 的左子树访问完毕
            ans.append(root.val)  # 记录当前节点的值
            root = root.right  # 如果有右子树就访问右子树，没有就顺着线索回到指向的节点

        return ans
```

```java [sol-Java]
class Solution {
    public List<Integer> inorderTraversal(TreeNode root) {
        List<Integer> ans = new ArrayList<>();

        while (root != null) {
            if (root.left != null) {
                // 找 root 的前驱 pre：在中序遍历中，root 的上一个节点
                // 从 root.left 开始，一直向右走，直到走到尽头，或者遇到指向 root 的线索（回到 root 的路）
                TreeNode pre = root.left;
                while (pre.right != null && pre.right != root) {
                    pre = pre.right;
                }

                // root 的左子树尚未访问
                if (pre.right == null) {
                    pre.right = root; // 建立线索（回到 root 的路），相当于把 pre.right 当作栈
                    root = root.left; // 访问左子树
                    continue;
                }

                // root 的左子树访问完毕，去掉线索，恢复原样
                pre.right = null; // 注：如果调用完 inorderTraversal 不再使用这棵二叉树，这行代码可以去掉
            }

            // root 的左子树访问完毕
            ans.add(root.val); // 记录当前节点的值
            root = root.right; // 如果有右子树就访问右子树，没有就顺着线索回到指向的节点
        }

        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> inorderTraversal(TreeNode* root) {
        vector<int> ans;

        while (root) {
            if (root->left) {
                // 找 root 的前驱 pre：在中序遍历中，root 的上一个节点
                // 从 root->left 开始，一直向右走，直到走到尽头，或者遇到指向 root 的线索（回到 root 的路）
                TreeNode* pre = root->left;
                while (pre->right && pre->right != root) {
                    pre = pre->right;
                }

                // root 的左子树尚未访问
                if (pre->right == nullptr) {
                    pre->right = root; // 建立线索（回到 root 的路），相当于把 pre->right 当作栈
                    root = root->left; // 访问左子树
                    continue;
                }

                // root 的左子树访问完毕，去掉线索，恢复原样
                pre->right = nullptr; // 注：如果调用完 inorderTraversal 不再使用这棵二叉树，这行代码可以去掉
            }

            // root 的左子树访问完毕
            ans.push_back(root->val); // 记录当前节点的值
            root = root->right; // 如果有右子树就访问右子树，没有就顺着线索回到指向的节点
        }

        return ans;
    }
};
```

```c [sol-C]
int* inorderTraversal(struct TreeNode* root, int* returnSize) {
    int* ans = malloc(100 * sizeof(int)); // 题目保证节点个数 <= 100
    *returnSize = 0;

    while (root) {
        if (root->left) {
            // 找 root 的前驱 pre：在中序遍历中，root 的上一个节点
            // 从 root->left 开始，一直向右走，直到走到尽头，或者遇到指向 root 的线索（回到 root 的路）
            struct TreeNode* pre = root->left;
            while (pre->right && pre->right != root) {
                pre = pre->right;
            }

            // root 的左子树尚未访问
            if (pre->right == NULL) {
                pre->right = root; // 建立线索（回到 root 的路），相当于把 pre->right 当作栈
                root = root->left; // 访问左子树
                continue;
            }

            // root 的左子树访问完毕，去掉线索，恢复原样
            pre->right = NULL; // 注：如果调用完 inorderTraversal 不再使用这棵二叉树，这行代码可以去掉
        }

        // root 的左子树访问完毕
        ans[(*returnSize)++] = root->val; // 记录当前节点的值
        root = root->right; // 如果有右子树就访问右子树，没有就顺着线索回到指向的节点
    }

    return ans;
}
```

```go [sol-Go]
func inorderTraversal(root *TreeNode) (ans []int) {
    for root != nil {
        if root.Left != nil {
            // 找 root 的前驱 pre：在中序遍历中，root 的上一个节点
            // 从 root.Left 开始，一直向右走，直到走到尽头，或者遇到指向 root 的线索（回到 root 的路）
            pre := root.Left
            for pre.Right != nil && pre.Right != root {
                pre = pre.Right
            }

            // root 的左子树尚未访问
            if pre.Right == nil {
                pre.Right = root // 建立线索（回到 root 的路），相当于把 pre.Right 当作栈
                root = root.Left // 访问左子树
                continue
            }

            // root 的左子树访问完毕，去掉线索，恢复原样
            pre.Right = nil // 注：如果调用完 inorderTraversal 不再使用这棵二叉树，这行代码可以去掉
        }

        // root 的左子树访问完毕
        ans = append(ans, root.Val) // 记录当前节点的值
        root = root.Right // 如果有右子树就访问右子树，没有就顺着线索回到指向的节点
    }
    return
}
```

```js [sol-JavaScript]
var inorderTraversal = function(root) {
    const ans = [];

    while (root) {
        if (root.left) {
            // 找 root 的前驱 pre：在中序遍历中，root 的上一个节点
            // 从 root.left 开始，一直向右走，直到走到尽头，或者遇到指向 root 的线索（回到 root 的路）
            let pre = root.left;
            while (pre.right && pre.right !== root) {
                pre = pre.right;
            }

            // root 的左子树尚未访问
            if (pre.right === null) {
                pre.right = root; // 建立线索（回到 root 的路），相当于把 pre.right 当作栈
                root = root.left; // 访问左子树
                continue;
            }

            // root 的左子树访问完毕，去掉线索，恢复原样
            pre.right = null; // 注：如果调用完 inorderTraversal 不再使用这棵二叉树，这行代码可以去掉
        }

        // root 的左子树访问完毕
        ans.push(root.val); // 记录当前节点的值
        root = root.right; // 如果有右子树就访问右子树，没有就顺着线索回到指向的节点
    }

    return ans;
};
```

```rust [sol-Rust]
use std::rc::Rc;
use std::cell::RefCell;

impl Solution {
    pub fn inorder_traversal(mut root: Option<Rc<RefCell<TreeNode>>>) -> Vec<i32> {
        let mut ans = vec![];

        while let Some(node) = root.clone() {
            let n = node.borrow();
            if let Some(ref left) = n.left {
                // 找 root 的前驱 pre：在中序遍历中，root 的上一个节点
                // 从 root.left 开始，一直向右走，直到走到尽头，或者遇到指向 root 的线索（回到 root 的路）
                let mut pre = left.clone();
                loop {
                    let right = pre.borrow().right.clone();
                    if right.is_none() || Rc::ptr_eq(right.as_ref().unwrap(), &node) {
                        break;
                    }
                    pre = right.unwrap();
                }

                let mut pre = pre.borrow_mut();
                // root 的左子树尚未访问
                if pre.right.is_none() {
                    pre.right = root.clone(); // 建立线索（回到 root 的路），相当于把 pre.right 当作栈
                    root = n.left.clone(); // 访问左子树
                    continue;
                }

                // root 的左子树访问完毕，去掉线索，恢复原样
                pre.right = None; // 注：如果调用完 inorder_traversal 不再使用这棵二叉树，这行代码可以去掉
            }

            // root 的左子树访问完毕
            ans.push(n.val); // 记录当前节点的值
            root = n.right.clone(); // 如果有右子树就访问右子树，没有就顺着线索回到指向的节点
        }

        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是二叉树的节点个数。虽然写了个二重循环，但二叉树的每条边至多访问三次：第一次寻找前驱、遍历二叉树、第二次寻找前驱。所以总的循环次数是 $\mathcal{O}(n)$。
- 空间复杂度：$\mathcal{O}(1)$。返回值不计入。

## 相似题目

读完本文后，读者可以尝试用 Morris 遍历实现另外两种二叉树的遍历：

- [144. 二叉树的前序遍历](https://leetcode.cn/problems/binary-tree-preorder-traversal/)
- [145. 二叉树的后序遍历](https://leetcode.cn/problems/binary-tree-postorder-traversal/)

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

本题来自 `二、二叉树 / §2.1 遍历二叉树`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、二叉树 / §2.1 遍历二叉树`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
