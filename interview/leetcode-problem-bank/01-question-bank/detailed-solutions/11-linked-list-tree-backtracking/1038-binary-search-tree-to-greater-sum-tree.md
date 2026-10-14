# 1038. 从二叉搜索树到更大和树

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/binary-search-tree-to-greater-sum-tree/
- 题目 slug：`binary-search-tree-to-greater-sum-tree`
- 来源专题：链表、树与回溯
- 来源分类路径：二、二叉树 / §2.5 有递有归
- 难度分：538
- 外部题解来源：https://leetcode.cn/problems/binary-search-tree-to-greater-sum-tree/solutions/2552797/jian-ji-xie-fa-li-yong-er-cha-sou-suo-sh-r5zm/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[简洁写法，利用二叉搜索树的性质（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/binary-search-tree-to-greater-sum-tree/solutions/2552797/jian-ji-xie-fa-li-yong-er-cha-sou-suo-sh-r5zm/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`jian-ji-xie-fa-li-yong-er-cha-sou-suo-sh-r5zm`
- topic id：`2552797`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:39:45 +0800

## 思路

为了算出节点值之和，必须先访问所有节点值大于当前节点值的节点。

![LC1038.png](https://pic.leetcode.cn/1701499044-hMIGSS-LC1038.png)

比如示例 1，为了算出根节点修改后的值，应当先把右子树的所有点遍历一遍（因为二叉搜索树右子树的节点值都大于根节点的值），得到右子树所有点的节点值之和，再加上根节点的值，即

$$
8+7+6+5+4 = 30
$$

这便是根节点修改后的值，即上图中根节点旁的蓝色数字。

这样就确定了递归的顺序：右子树-根-左子树。

## 算法

1. 初始化 $s=0$。
2. 从根节点开始递归，先递归右子树。
3. 右子树递归结束后，把当前节点的值加到 $s$ 中，然后用 $s$ 替换当前节点的值。
4. 然后递归左子树。
5. 递归边界：递归到空节点时返回。

晕递归的同学可以看 [深刻理解递归【基础算法精讲 09】](https://www.bilibili.com/video/BV1UD4y1Y769/)

```py [sol-Python3]
class Solution:
    def bstToGst(self, root: TreeNode) -> TreeNode:
        s = 0
        def dfs(node: TreeNode) -> None:
            if node is None:
                return
            dfs(node.right)  # 递归右子树
            # 递归结束后，s 就等于右子树的所有节点值之和
            nonlocal s
            s += node.val
            node.val = s  # 此时 s 就是 >= node.val 的所有数之和
            dfs(node.left)  # 递归左子树
        dfs(root)
        return root
```

```java [sol-Java]
class Solution {
    private int s = 0;

    public TreeNode bstToGst(TreeNode root) {
        dfs(root);
        return root;
    }

    private void dfs(TreeNode node) {
        if (node == null) {
            return;
        }
        dfs(node.right); // 递归右子树
        s += node.val;
        node.val = s; // 此时 s 就是 >= node.val 的所有数之和
        dfs(node.left); // 递归左子树
    }
}
```

```cpp [sol-C++]
class Solution {
private:
    int s = 0;

    void dfs(TreeNode *node) {
        if (node == nullptr) {
            return;
        }
        dfs(node->right); // 递归右子树
        s += node->val;
        node->val = s; // 此时 s 就是 >= node->val 的所有数之和
        dfs(node->left); // 递归左子树
    }

public:
    TreeNode *bstToGst(TreeNode *root) {
        dfs(root);
        return root;
    }
};
```

```go [sol-Go]
func bstToGst(root *TreeNode) *TreeNode {
    s := 0
    var dfs func(*TreeNode)
    dfs = func(node *TreeNode) {
        if node == nil {
            return
        }
        dfs(node.Right) // 递归右子树
        s += node.Val
        node.Val = s   // 此时 s 就是 >= node.Val 的所有数之和
        dfs(node.Left) // 递归左子树
    }
    dfs(root)
    return root
}
```

```js [sol-JavaScript]
var bstToGst = function (root) {
    let s = 0;
    function dfs(node) {
        if (node === null) {
            return;
        }
        dfs(node.right); // 递归右子树
        s += node.val;
        node.val = s; // 此时 s 就是 >= node.val 的所有数之和
        dfs(node.left); // 递归左子树
    }
    dfs(root);
    return root;
};
```

```rust [sol-Rust]
use std::rc::Rc;
use std::cell::RefCell;

impl Solution {
    pub fn bst_to_gst(root: Option<Rc<RefCell<TreeNode>>>) -> Option<Rc<RefCell<TreeNode>>> {
        fn dfs(node: Option<&Rc<RefCell<TreeNode>>>, s: &mut i32) {
            if let Some(x) = node {
                let mut x = x.borrow_mut();
                dfs(x.right.as_ref(), s); // 递归右子树
                *s += x.val;
                x.val = *s; // 此时 s 就是 >= node.val 的所有数之和
                dfs(x.left.as_ref(), s); // 递归左子树
            }
        }
        let mut s = 0;
        dfs(root.as_ref(), &mut s);
        root
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为二叉搜索树的节点个数。
- 空间复杂度：$\mathcal{O}(n)$。最坏情况下，二叉搜索树退化成一条链（注意题目没有保证它是**平衡**树），因此递归需要 $\mathcal{O}(n)$ 的栈空间。

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

更多精彩题解，请看 [往期题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)
