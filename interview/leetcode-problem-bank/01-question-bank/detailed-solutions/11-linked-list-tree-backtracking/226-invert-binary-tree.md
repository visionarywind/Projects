# 226. 翻转二叉树

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/invert-binary-tree/
- 题目 slug：`invert-binary-tree`
- 来源专题：链表、树与回溯
- 来源分类路径：二、二叉树 / §2.3 自底向上 DFS（后序遍历）
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/invert-binary-tree/solutions/2713610/shi-pin-shen-ru-li-jie-di-gui-pythonjava-zhqh/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种递归写法（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/invert-binary-tree/solutions/2713610/shi-pin-shen-ru-li-jie-di-gui-pythonjava-zhqh/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`shi-pin-shen-ru-li-jie-di-gui-pythonjava-zhqh`
- topic id：`2713610`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:30:25 +0800

## 要点

1. 对于根节点，它的左右**儿子**必须交换，即左儿子变成右儿子，右儿子变成左儿子。
2. 对于根节点的左右**子树**，也需要翻转其内部节点。这是一个和原问题相似的子问题，看完视频后，你知道，这可以用**递归**解决。

## 算法

1. 递归调用 `invertTree(root.left)`，获取到左子树翻转后的结果 `left`。
2. 递归调用 `invertTree(root.right)`，获取到右子树翻转后的结果 `right`。
3. 交换左右儿子，即更新 `root.left` 为 `right`，更新 `root.right` 为 `left`。
4. 返回 `root`。
5. 递归边界：如果 `root` 是空节点，返回空。

晕递归的同学，请看视频：[深入理解递归【基础算法精讲 09】](https://www.bilibili.com/video/BV1UD4y1Y769/)

## 写法一

```py [sol-Python3]
class Solution:
    def invertTree(self, root: Optional[TreeNode]) -> Optional[TreeNode]:
        if root is None:
            return None
        left = self.invertTree(root.left)  # 翻转左子树
        right = self.invertTree(root.right)  # 翻转右子树
        root.left = right  # 交换左右儿子
        root.right = left
        return root
```

```py [sol-Python3 直接交换]
class Solution:
    def invertTree(self, root: Optional[TreeNode]) -> Optional[TreeNode]:
        if root is None:
            return None
        root.right, root.left = self.invertTree(root.left), self.invertTree(root.right)
        return root
```

```java [sol-Java]
class Solution {
    public TreeNode invertTree(TreeNode root) {
        if (root == null) {
            return null;
        }
        TreeNode left = invertTree(root.left); // 翻转左子树
        TreeNode right = invertTree(root.right); // 翻转右子树
        root.left = right; // 交换左右儿子
        root.right = left;
        return root;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    TreeNode* invertTree(TreeNode* root) {
        if (root == nullptr) {
            return nullptr;
        }
        auto left = invertTree(root->left); // 翻转左子树
        auto right = invertTree(root->right); // 翻转右子树
        root->left = right; // 交换左右儿子
        root->right = left;
        return root;
    }
};
```

```c [sol-C]
struct TreeNode* invertTree(struct TreeNode* root) {
    if (root == NULL) {
        return NULL;
    }
    struct TreeNode* left = invertTree(root->left); // 翻转左子树
    struct TreeNode* right = invertTree(root->right); // 翻转右子树
    root->left = right; // 交换左右儿子
    root->right = left;
    return root;
}
```

```go [sol-Go]
func invertTree(root *TreeNode) *TreeNode {
    if root == nil {
        return nil
    }
    left := invertTree(root.Left) // 翻转左子树
    right := invertTree(root.Right) // 翻转右子树
    root.Left = right // 交换左右儿子
    root.Right = left
    return root
}
```

```js [sol-JavaScript]
var invertTree = function(root) {
    if (root === null) {
        return null;
    }
    const left = invertTree(root.left); // 翻转左子树
    const right = invertTree(root.right); // 翻转右子树
    root.left = right; // 交换左右儿子
    root.right = left;
    return root;
};
```

```rust [sol-Rust]
use std::rc::Rc;
use std::cell::RefCell;

impl Solution {
    pub fn invert_tree(root: Option<Rc<RefCell<TreeNode>>>) -> Option<Rc<RefCell<TreeNode>>> {
        if let Some(node) = root {
            let left = Self::invert_tree(node.borrow_mut().left.take()); // 翻转左子树
            let right = Self::invert_tree(node.borrow_mut().right.take()); // 翻转右子树
            node.borrow_mut().left = right; // 交换左右儿子
            node.borrow_mut().right = left;
            Some(node)
        } else {
            None
        }
    }
}
```

## 写法二

也可以先交换左右儿子，然后递归处理左右子树。

```py [sol-Python3]
class Solution:
    def invertTree(self, root: Optional[TreeNode]) -> Optional[TreeNode]:
        if root is None:
            return None
        root.left, root.right = root.right, root.left  # 交换左右儿子
        self.invertTree(root.left)  # 翻转左子树
        self.invertTree(root.right)  # 翻转右子树
        return root
```

```java [sol-Java]
class Solution {
    public TreeNode invertTree(TreeNode root) {
        if (root == null) {
            return null;
        }
        TreeNode tmp = root.left; // 交换左右儿子
        root.left = root.right;
        root.right = tmp;
        invertTree(root.left); // 翻转左子树
        invertTree(root.right); // 翻转右子树
        return root;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    TreeNode* invertTree(TreeNode* root) {
        if (root == nullptr) {
            return nullptr;
        }
        swap(root->left, root->right); // 交换左右儿子
        invertTree(root->left); // 翻转左子树
        invertTree(root->right); // 翻转右子树
        return root;
    }
};
```

```c [sol-C]
struct TreeNode* invertTree(struct TreeNode* root) {
    if (root == NULL) {
        return NULL;
    }
    struct TreeNode* tmp = root->left; // 交换左右儿子
    root->left = root->right;
    root->right = tmp;
    invertTree(root->left); // 翻转左子树
    invertTree(root->right); // 翻转右子树
    return root;
}
```

```go [sol-Go]
func invertTree(root *TreeNode) *TreeNode {
    if root == nil {
        return nil
    }
    root.Left, root.Right = root.Right, root.Left // 交换左右儿子
    invertTree(root.Left) // 翻转左子树
    invertTree(root.Right) // 翻转右子树
    return root
}
```

```js [sol-JavaScript]
var invertTree = function(root) {
    if (root === null) {
        return null;
    }
    [root.left, root.right] = [root.right, root.left]; // 交换左右儿子
    invertTree(root.left); // 翻转左子树
    invertTree(root.right); // 翻转右子树
    return root;
};
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是二叉树的节点个数。
- 空间复杂度：$\mathcal{O}(n)$。最坏情况下，二叉树退化成一条链，递归需要 $\mathcal{O}(n)$ 的栈空间。

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
