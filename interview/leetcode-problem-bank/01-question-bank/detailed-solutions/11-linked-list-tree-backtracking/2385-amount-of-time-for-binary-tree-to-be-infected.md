# 2385. 感染二叉树需要的总时间

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/amount-of-time-for-binary-tree-to-be-infected/
- 题目 slug：`amount-of-time-for-binary-tree-to-be-infected`
- 来源专题：链表、树与回溯
- 来源分类路径：二、二叉树 / §2.6 二叉树的直径
- 难度分：1711
- 外部题解来源：https://leetcode.cn/problems/amount-of-time-for-binary-tree-to-be-infected/solutions/2753470/cong-liang-ci-bian-li-dao-yi-ci-bian-li-tmt0x/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[从两次遍历到一次遍历（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/amount-of-time-for-binary-tree-to-be-infected/solutions/2753470/cong-liang-ci-bian-li-dao-yi-ci-bian-li-tmt0x/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`cong-liang-ci-bian-li-dao-yi-ci-bian-li-tmt0x`
- topic id：`2753470`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:39:45 +0800

## 题意

设 $v$ 是离 $\textit{start}$ 最远的节点，返回 $\textit{start}$ 到 $v$ 的距离。例如示例 1 中节点 $9$（或者节点 $2$）离 $\textit{start}=3$ 最远，二者距离为 $4$。

## 方法一：记录父节点 + DFS

首先从 $\textit{root}$ 出发 DFS 这棵树，找到节点值为 $\textit{start}$ 的节点 $\textit{startNode}$。DFS 的同时，用一个哈希表（或者数组）记录每个节点的父节点。

然后从 $\textit{startNode}$ 出发 DFS 这棵树，求出 [104. 二叉树的最大深度](https://leetcode.cn/problems/maximum-depth-of-binary-tree/)，即为答案（把 $\textit{startNode}$ 的深度当作 $0$）。注意除了递归左右儿子以外，还需要递归父节点。为避免重复访问节点，可以添加一个递归参数 $\textit{from}$，表示当前节点是从节点 $\textit{from}$ 过来的，我们不去重复访问节点 $\textit{from}$。

关于 104 题的讲解，见 [深刻理解递归【基础算法精讲 09】](https://www.bilibili.com/video/BV1UD4y1Y769/)。

这种方法需要遍历二叉树两次。

```py [sol-Python3]
class Solution:
    def amountOfTime(self, root: Optional[TreeNode], start: int) -> int:
        fa = {}
        start_node = None
        def dfs(node: Optional[TreeNode], from_: Optional[TreeNode]) -> None:
            if node is None:
                return
            fa[node] = from_  # 记录每个节点的父节点
            if node.val == start:  # 找到 start
                nonlocal start_node
                start_node = node
            dfs(node.left, node)
            dfs(node.right, node)
        dfs(root, None)

        def maxDepth(node: Optional[TreeNode], from_: TreeNode) -> int:
            if node is None:
                return -1  # 注意这里是 -1，因为 start 的深度为 0
            return max(maxDepth(x, node) for x in (node.left, node.right, fa[node]) if x != from_) + 1
        return maxDepth(start_node, start_node)
```

```java [sol-Java]
class Solution {
    private TreeNode startNode;
    private final Map<TreeNode, TreeNode> fa = new HashMap<>();

    public int amountOfTime(TreeNode root, int start) {
        dfs(root, null, start);
        return maxDepth(startNode, startNode);
    }

    private void dfs(TreeNode node, TreeNode from, int start) {
        if (node == null) {
            return;
        }
        fa.put(node, from); // 记录每个节点的父节点
        if (node.val == start) {
            startNode = node; // 找到 start
        }
        dfs(node.left, node, start);
        dfs(node.right, node, start);
    }

    private int maxDepth(TreeNode node, TreeNode from) {
        if (node == null) {
            return -1; // 注意这里是 -1，因为 start 的深度为 0
        }
        int res = -1;
        if (node.left != from) {
            res = Math.max(res, maxDepth(node.left, node));
        }
        if (node.right != from) {
            res = Math.max(res, maxDepth(node.right, node));
        }
        if (fa.get(node) != from) {
            res = Math.max(res, maxDepth(fa.get(node), node));
        }
        return res + 1;
    }
}
```

```cpp [sol-C++]
TreeNode* fa[100001]; // 哈希表会超时，改用数组

class Solution {
    int start;
    TreeNode* start_node;

    void dfs(TreeNode* node, TreeNode* from) {
        if (node == nullptr) {
            return;
        }
        fa[node->val] = from; // 记录每个节点的父节点
        if (node->val == start) {
            start_node = node; // 找到 start
        }
        dfs(node->left, node);
        dfs(node->right, node);
    }

    int maxDepth(TreeNode* node, TreeNode* from) {
        if (node == nullptr) {
            return -1; // 注意这里是 -1，因为 start 的深度为 0
        }
        int res = -1;
        if (node->left != from) {
            res = max(res, maxDepth(node->left, node));
        }
        if (node->right != from) {
            res = max(res, maxDepth(node->right, node));
        }
        if (fa[node->val] != from) {
            res = max(res, maxDepth(fa[node->val], node));
        }
        return res + 1;
    }

public:
    int amountOfTime(TreeNode* root, int start) {
        this->start = start;
        dfs(root, nullptr);
        return maxDepth(start_node, start_node);
    }
};
```

```go [sol-Go]
func amountOfTime(root *TreeNode, start int) int {
    fa := map[*TreeNode]*TreeNode{}
    var startNode *TreeNode
    var dfs func(*TreeNode, *TreeNode)
    dfs = func(node, from *TreeNode) {
        if node == nil {
            return
        }
        fa[node] = from // 记录每个节点的父节点
        if node.Val == start {
            startNode = node // 找到 start
        }
        dfs(node.Left, node)
        dfs(node.Right, node)
    }
    dfs(root, nil)

    var maxDepth func(*TreeNode, *TreeNode) int
    maxDepth = func(node, from *TreeNode) int {
        if node == nil {
            return -1 // 注意这里是 -1，因为 start 的深度为 0
        }
        res := -1
        if node.Left != from {
            res = max(res, maxDepth(node.Left, node))
        }
        if node.Right != from {
            res = max(res, maxDepth(node.Right, node))
        }
        if fa[node] != from {
            res = max(res, maxDepth(fa[node], node))
        }
        return res + 1
    }
    return maxDepth(startNode, startNode)
}
```

```js [sol-JavaScript]
var amountOfTime = function(root, start) {
    const fa = new Map(); // 定义一个空对象用于存储父节点信息
    let startNode; // 用于存储找到的起始节点
    function dfs(node, from) {
        if (node === null) {
            return;
        }
        fa.set(node, from); // 记录每个节点的父节点
        if (node.val === start) {
            startNode = node; // 找到 start
        }
        dfs(node.left, node);
        dfs(node.right, node);
    }
    dfs(root, null);

    function maxDepth(node, from) {
        if (node === null) {
            return -1; // 注意这里是 -1，因为 start 的深度为 0
        }
        let res = -1;
        if (node.left !== from) {
            res = Math.max(res, maxDepth(node.left, node));
        }
        if (node.right !== from) {
            res = Math.max(res, maxDepth(node.right, node));
        }
        if (fa.get(node) !== from) {
            res = Math.max(res, maxDepth(fa.get(node), node));
        }
        return res + 1;
    }
    return maxDepth(startNode, startNode);
};
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为二叉树的节点个数。
- 空间复杂度：$\mathcal{O}(n)$。

## 方法二：树的直径

![lc2385.png](https://pic.leetcode.cn/1713429504-OFesJF-lc2385.png)

以示例 1 为例，我们可以把问题拆分成两个问题。

**第一个问题**：以 $3$ 为根节点的子树的 [104. 二叉树的最大深度](https://leetcode.cn/problems/maximum-depth-of-binary-tree/)，其中节点 $3$ 的深度为 $0$。

如下图，这棵子树的最大深度是 $1$，也就是 $3$ **往下走**的最远距离。

![lc2385-1.png](https://pic.leetcode.cn/1713429911-nskLxP-lc2385-1.png)

**第二个问题**：去掉 $3$ 的所有子孙节点，此时 $3$ 变成了一个叶子节点。我们相当于计算其中一个端点固定为 $3$ 的 [543. 二叉树的直径](https://leetcode.cn/problems/diameter-of-binary-tree/)。

如下图，若固定直径的一个端点为 $3$，则这条直径可以是 $3\text{-}1\text{-}5\text{-}4\text{-}9$，长度（边的个数）为 $4$。相当于从 $3$ 出发**往上走**，在某个点「拐弯」后再往下走。

![lc2385-2.png](https://pic.leetcode.cn/1713429661-YvNpgk-lc2385-2.png)

这两个问题的答案取最大值 $\max(1,4)=4$，即为原问题的答案。

关于 543 题的讲解，请看 [树的直径【基础算法精讲 23】](https://www.bilibili.com/video/BV17o4y187h1/)。

本题算法如下：

1. 递归时，除了返回当前子树的最大链长加一，还需要返回一个布尔值，表示当前子树是否包含 $\textit{start}$。
2. 如果当前节点是空节点，返回 $0$ 和 $\texttt{false}$。
3. 设左子树的返回的链长为 $\textit{lLen}$，右子树返回的链长为 $\textit{rLen}$。
4. 如果当前节点值等于 $\textit{start}$，初始化答案为 $\max(\textit{lLen},\textit{rLen})$，即子树 $\textit{start}$ 的最大深度。然后返回 $1$ 和 $\texttt{true}$。
5. 如果左右子树都不包含 $\textit{start}$，返回 $\max(\textit{lLen},\textit{rLen}) + 1$。
6. 如果左子树或右子树包含 $\textit{start}$，像计算直径那样，用 $\textit{lLen}+\textit{rLen}$ 更新答案的最大值。如果左子树包含 $\textit{start}$，则返回 $\textit{lLen}$ 和 $\texttt{true}$，否则返回 $\textit{rLen}$ 和 $\texttt{true}$。这种返回方式可以保证 $\textit{lLen}+\textit{rLen}$ 一定是端点为 $\textit{start}$ 的直径长度。

```py [sol-Python3]
class Solution:
    def amountOfTime(self, root: Optional[TreeNode], start: int) -> int:
        ans = 0
        def dfs(node: Optional[TreeNode]) -> (int, bool):
            if node is None:
                return 0, False
            l_len, l_found = dfs(node.left)
            r_len, r_found = dfs(node.right)
            nonlocal ans
            if node.val == start:
                # 计算子树 start 的最大深度
                # 注意这里和方法一的区别，max 后面没有 +1，所以算出的也是最大深度
                ans = max(l_len, r_len)
                return 1, True  # 找到了 start
            if l_found or r_found:
                # 只有在左子树或右子树包含 start 时，才能更新答案
                ans = max(ans, l_len + r_len)  # 两条链拼成直径
                # 保证 start 是直径端点
                return (l_len if l_found else r_len) + 1, True
            return max(l_len, r_len) + 1, False
        dfs(root)
        return ans
```

```java [sol-Java]
class Solution {
    private int ans;

    public int amountOfTime(TreeNode root, int start) {
        dfs(root, start);
        return ans;
    }

    private int[] dfs(TreeNode node, int start) {
        if (node == null) {
            return new int[]{0, 0};
        }
        int[] leftRes = dfs(node.left, start);
        int[] rightRes = dfs(node.right, start);
        int lLen = leftRes[0], lFound = leftRes[1];
        int rLen = rightRes[0], rFound = rightRes[1];
        if (node.val == start) {
            // 计算子树 start 的最大深度
            // 注意这里和方法一的区别，max 后面没有 +1，所以算出的也是最大深度
            ans = Math.max(lLen, rLen);
            return new int[]{1, 1}; // 找到了 start
        }
        if (lFound == 1 || rFound == 1) {
            // 只有在左子树或右子树包含 start 时，才能更新答案
            ans = Math.max(ans, lLen + rLen); // 两条链拼成直径
            // 保证 start 是直径端点
            return new int[]{(lFound == 1 ? lLen : rLen) + 1, 1};
        }
        return new int[]{Math.max(lLen, rLen) + 1, 0};
    }
}
```

```cpp [sol-C++]
class Solution {
    int ans = 0, start;

    pair<int, bool> dfs(TreeNode* node) {
        if (node == nullptr) {
            return {0, false};
        }
        auto [l_len, l_found] = dfs(node->left);
        auto [r_len, r_found] = dfs(node->right);
        if (node->val == start) {
            // 计算子树 start 的最大深度
            // 注意这里和方法一的区别，max 后面没有 +1，所以算出的也是最大深度
            ans = max(l_len, r_len);
            return {1, true}; // 找到了 start
        }
        if (l_found || r_found) {
            // 只有在左子树或右子树包含 start 时，才能更新答案
            ans = max(ans, l_len + r_len); // 两条链拼成直径
            // 保证 start 是直径端点
            return {(l_found ? l_len : r_len) + 1, true};
        }
        return {max(l_len, r_len) + 1, false};
    }

public:
    int amountOfTime(TreeNode* root, int start) {
        this->start = start;
        dfs(root);
        return ans;
    }
};
```

```go [sol-Go]
func amountOfTime(root *TreeNode, start int) (ans int) {
    var dfs func(*TreeNode) (int, bool)
    dfs = func(node *TreeNode) (int, bool) {
        if node == nil {
            return 0, false
        }
        lLen, lFound := dfs(node.Left)
        rLen, rFound := dfs(node.Right)
        if node.Val == start {
            // 计算子树 start 的最大深度
            // 注意这里和方法一的区别，max 后面没有 +1，所以算出的也是最大深度
            ans = max(lLen, rLen)
            return 1, true // 找到了 start
        }
        if lFound || rFound {
            // 只有在左子树或右子树包含 start 时，才能更新答案
            ans = max(ans, lLen+rLen) // 两条链拼成直径
            // 保证 start 是直径端点
            if lFound {
                return lLen + 1, true
            }
            return rLen + 1, true
        }
        return max(lLen, rLen) + 1, false
    }
    dfs(root)
    return
}
```

```js [sol-JavaScript]
var amountOfTime = function(root, start) {
    let ans = 0;
    function dfs(node) {
        if (node === null) {
            return [0, false];
        }
        const [lLen, lFound] = dfs(node.left);
        const [rLen, rFound] = dfs(node.right);
        if (node.val === start) {
            // 计算子树 start 的最大深度
            // 注意这里和方法一的区别，max 后面没有 +1，所以算出的也是最大深度
            ans = Math.max(lLen, rLen);
            return [1, true]; // 找到了 start
        }
        if (lFound || rFound) {
            // 只有在左子树或右子树包含 start 时，才能更新答案
            ans = Math.max(ans, lLen + rLen); // 两条链拼成直径
            // 保证 start 是直径端点
            return [(lFound ? lLen : rLen) + 1, true];
        }
        return [Math.max(lLen, rLen) + 1, false];
    }
    dfs(root);
    return ans;
};
```

```rust [sol-Rust]
use std::rc::Rc;
use std::cell::RefCell;

impl Solution {
    pub fn amount_of_time(root: Option<Rc<RefCell<TreeNode>>>, start: i32) -> i32 {
        fn dfs(node: Option<&Rc<RefCell<TreeNode>>>, start: i32, ans: &mut i32) -> (i32, bool) {
            if let Some(x) = node {
                let x = x.borrow();
                let (l_len, l_found) = dfs(x.left.as_ref(), start, ans);
                let (r_len, r_found) = dfs(x.right.as_ref(), start, ans);
                if x.val == start {
                    // 计算子树 start 的最大深度
                    // 注意这里和方法一的区别，max 后面没有 +1，所以算出的也是最大深度
                    *ans = l_len.max(r_len);
                    return (1, true); // 找到了 start
                }
                if l_found || r_found {
                    // 只有在左子树或右子树包含 start 时，才能更新答案
                    *ans = (*ans).max(l_len + r_len); // 两条链拼成直径
                    // 保证 start 是直径端点
                    return ((if l_found { l_len } else { r_len }) + 1, true);
                }
                return (l_len.max(r_len) + 1, false);
            }
            (0, false)
        }
        let mut ans = 0;
        dfs(root.as_ref(), start, &mut ans);
        ans
    }
}
```

## 另一种写法

上面的写法 DFS 会返回两个值，能否只返回一个值呢？

可以的，去掉布尔值，把**负数**利用起来：

- 如果子树不含 $\textit{start}$，将原来的返回值改为相反数（负数）。
- 如果子树包含 $\textit{start}$，返回值不变。

```py [sol-Python3]
class Solution:
    def amountOfTime(self, root: Optional[TreeNode], start: int) -> int:
        ans = 0
        def dfs(node: Optional[TreeNode]) -> int:
            if node is None:
                return 0
            l_len = dfs(node.left)
            r_len = dfs(node.right)
            nonlocal ans
            if node.val == start:
                # 计算子树 start 的最大深度
                ans = -min(l_len, r_len)  # 负负得正
                return 1  # 用正数表示找到了 start
            if l_len > 0 or r_len > 0:
                # 只有在左子树或右子树包含 start 时，才能更新答案
                ans = max(ans, abs(l_len) + abs(r_len))  # 两条链拼成直径
                return max(l_len, r_len) + 1  # max 会自动取到正数
            return min(l_len, r_len) - 1  # 用负数表示没有找到 start
        dfs(root)
        return ans
```

```java [sol-Java]
class Solution {
    private int ans;

    public int amountOfTime(TreeNode root, int start) {
        dfs(root, start);
        return ans;
    }

    private int dfs(TreeNode node, int start) {
        if (node == null) {
            return 0;
        }
        int lLen = dfs(node.left, start);
        int rLen = dfs(node.right, start);
        if (node.val == start) {
            // 计算子树 start 的最大深度
            ans = -Math.min(lLen, rLen); // 负负得正
            return 1; // 用正数表示找到了 start
        }
        if (lLen > 0 || rLen > 0) {
            // 只有在左子树或右子树包含 start 时，才能更新答案
            ans = Math.max(ans, Math.abs(lLen) + Math.abs(rLen)); // 两条链拼成直径
            return Math.max(lLen, rLen) + 1; // max 会自动取到正数
        }
        return Math.min(lLen, rLen) - 1; // 用负数表示没有找到 start
    }
}
```

```cpp [sol-C++]
class Solution {
    int ans = 0, start;

    int dfs(TreeNode* node) {
        if (node == nullptr) {
            return 0;
        }
        int l_len = dfs(node->left);
        int r_len = dfs(node->right);
        if (node->val == start) {
            // 计算子树 start 的最大深度
            ans = -min(l_len, r_len); // 负负得正
            return 1; // 用正数表示找到了 start
        }
        if (l_len > 0 || r_len > 0) {
            // 只有在左子树或右子树包含 start 时，才能更新答案
            ans = max(ans, abs(l_len) + abs(r_len)); // 两条链拼成直径
            return max(l_len, r_len) + 1; // max 会自动取到正数
        }
        return min(l_len, r_len) - 1; // 用负数表示没有找到 start
    }

public:
    int amountOfTime(TreeNode* root, int start) {
        this->start = start;
        dfs(root);
        return ans;
    }
};
```

```go [sol-Go]
func amountOfTime(root *TreeNode, start int) (ans int) {
    var dfs func(*TreeNode) int
    dfs = func(node *TreeNode) int {
        if node == nil {
            return 0
        }
        lLen := dfs(node.Left)
        rLen := dfs(node.Right)
        if node.Val == start {
            // 计算子树 start 的最大深度
            ans = -min(lLen, rLen) // 负负得正
            return 1 // 用正数表示找到了 start
        }
        if lLen > 0 || rLen > 0 {
            // 只有在左子树或右子树包含 start 时，才能更新答案
            ans = max(ans, abs(lLen)+abs(rLen)) // 两条链拼成直径
            return max(lLen, rLen) + 1 // max 会自动取到正数
        }
        return min(lLen, rLen) - 1 // 用负数表示没有找到 start
    }
    dfs(root)
    return
}

func abs(x int) int { if x < 0 { return -x }; return x }
```

```js [sol-JavaScript]
var amountOfTime = function(root, start) {
    let ans = 0;
    function dfs(node) {
        if (node === null) {
            return 0;
        }
        const lLen = dfs(node.left);
        const rLen = dfs(node.right);
        if (node.val === start) {
            // 计算子树 start 的最大深度
            ans = -Math.min(lLen, rLen); // 负负得正
            return 1; // 用正数表示找到了 start
        }
        if (lLen > 0 || rLen > 0) {
            // 只有在左子树或右子树包含 start 时，才能更新答案
            ans = Math.max(ans, Math.abs(lLen) + Math.abs(rLen)); // 两条链拼成直径
            return Math.max(lLen, rLen) + 1; // max 会自动取到正数
        }
        return Math.min(lLen, rLen) - 1; // 用负数表示没有找到 start
    }
    dfs(root);
    return ans;
};
```

```rust [sol-Rust]
use std::rc::Rc;
use std::cell::RefCell;

impl Solution {
    pub fn amount_of_time(root: Option<Rc<RefCell<TreeNode>>>, start: i32) -> i32 {
        fn dfs(node: Option<&Rc<RefCell<TreeNode>>>, start: i32, ans: &mut i32) -> i32 {
            if let Some(x) = node {
                let x = x.borrow();
                let l_len = dfs(x.left.as_ref(), start, ans);
                let r_len = dfs(x.right.as_ref(), start, ans);
                if x.val == start {
                    // 计算子树 start 的最大深度
                    *ans = -l_len.min(r_len); // 负负得正
                    return 1; // 用正数表示找到了 start
                }
                if l_len > 0 || r_len > 0 {
                    // 只有在左子树或右子树包含 start 时，才能更新答案
                    *ans = (*ans).max(l_len.abs() + r_len.abs()); // 两条链拼成直径
                    return l_len.max(r_len) + 1; // max 会自动取到正数
                }
                return l_len.min(r_len) - 1 // 用负数表示没有找到 start
            }
            0
        }
        let mut ans = 0;
        dfs(root.as_ref(), start, &mut ans);
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为二叉树的节点个数。
- 空间复杂度：$\mathcal{O}(n)$。最坏情况下，二叉树退化成一条链，递归需要 $\mathcal{O}(n)$ 的栈空间。

## 相似题目

- [863. 二叉树中所有距离为 K 的结点](https://leetcode.cn/problems/all-nodes-distance-k-in-binary-tree/)

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

本题来自 `二、二叉树 / §2.6 二叉树的直径`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、二叉树 / §2.6 二叉树的直径`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
