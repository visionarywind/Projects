# 129. 求根节点到叶节点数字之和

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/sum-root-to-leaf-numbers/
- 题目 slug：`sum-root-to-leaf-numbers`
- 来源专题：链表、树与回溯
- 来源分类路径：二、二叉树 / §2.2 自顶向下 DFS（先序遍历）
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/sum-root-to-leaf-numbers/solutions/2730644/jian-ji-xie-fa-pythonjavacgojsrust-by-en-gbu9/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种写法：没有返回值 / 有返回值（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/sum-root-to-leaf-numbers/solutions/2730644/jian-ji-xie-fa-pythonjavacgojsrust-by-en-gbu9/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`jian-ji-xie-fa-pythonjavacgojsrust-by-en-gbu9`
- topic id：`2730644`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:30:25 +0800

![lc129.jpg](https://pic.leetcode.cn/1712628146-tSGFUi-lc129.jpg)

对于路径 $4\rightarrow 9\rightarrow 5$，我们可以按照如下方式生成数字 $495$。

1. 初始化 $x=0$。
2. 从 $4$ 开始递归，更新 $x = x \cdot 10 + 4 = 4$。
3. 向下递归到 $9$，更新 $x = x \cdot 10 + 9 = 49$。
4. 向下递归到 $5$，更新 $x = x \cdot 10 + 5 = 495$。

当我们递归到叶子节点时，把 $x$ 加到答案中。

为了实现上述算法，把 $x$ 作为 DFS 的参数，并在 DFS 中按照

$$
x = x\cdot 10 + \textit{node}.\textit{val}
$$

去更新 $x$。

晕递归的同学，可以看视频讲解[【基础算法精讲 09】](https://www.bilibili.com/video/BV1UD4y1Y769/)，制作不易，欢迎点赞~

## 写法一：没有返回值

```py [sol-Python3]
class Solution:
    def sumNumbers(self, root: Optional[TreeNode]) -> int:
        ans = 0
        def dfs(node: Optional[TreeNode], x: int) -> None:
            if node is None:
                return
            x = x * 10 + node.val
            if node.left is None and node.right is None:  # node 是叶子节点
                nonlocal ans
                ans += x
                return
            dfs(node.left, x)
            dfs(node.right, x)
        dfs(root, 0)
        return ans
```

```java [sol-Java]
class Solution {
    private int ans;

    public int sumNumbers(TreeNode root) {
        dfs(root, 0);
        return ans;
    }

    private void dfs(TreeNode node, int x) {
        if (node == null) {
            return;
        }
        x = x * 10 + node.val;
        if (node.left == null && node.right == null) { // node 是叶子节点
            ans += x;
            return;
        }
        dfs(node.left, x);
        dfs(node.right, x);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int sumNumbers(TreeNode* root) {
        int ans = 0;
        auto dfs = [&](this auto&& dfs, TreeNode* node, int x) -> void {
            if (node == nullptr) {
                return;
            }
            x = x * 10 + node->val;
            if (node->left == nullptr && node->right == nullptr) { // node 是叶子节点
                ans += x;
                return;
            }
            dfs(node->left, x);
            dfs(node->right, x);
        };
        dfs(root, 0);
        return ans;
    }
};
```

```c [sol-C]
int sumNumbers(struct TreeNode* root) {
    int ans = 0;
    void dfs(struct TreeNode* node, int x) {
        if (node == NULL) {
            return;
        }
        x = x * 10 + node->val;
        if (node->left == NULL && node->right == NULL) { // node 是叶子节点
            ans += x;
            return;
        }
        dfs(node->left, x);
        dfs(node->right, x);
    }
    dfs(root, 0);
    return ans;
}
```

```go [sol-Go]
func sumNumbers(root *TreeNode) (ans int) {
    var dfs func(*TreeNode, int)
    dfs = func(node *TreeNode, x int) {
        if node == nil {
            return
        }
        x = x*10 + node.Val
        if node.Left == nil && node.Right == nil { // node 是叶子节点
            ans += x
            return
        }
        dfs(node.Left, x)
        dfs(node.Right, x)
    }
    dfs(root, 0)
    return
}
```

```js [sol-JavaScript]
var sumNumbers = function(root) {
    let ans = 0;
    function dfs(node, x) {
        if (node === null) {
            return;
        }
        x = x * 10 + node.val;
        if (node.left === null && node.right === null) { // node 是叶子节点
            ans += x;
            return;
        }
        dfs(node.left, x);
        dfs(node.right, x);
    }
    dfs(root, 0)
    return ans;
};
```

```rust [sol-Rust]
use std::rc::Rc;
use std::cell::RefCell;

impl Solution {
    pub fn sum_numbers(root: Option<Rc<RefCell<TreeNode>>>) -> i32 {
        fn dfs(node: &Option<Rc<RefCell<TreeNode>>>, x: i32, ans: &mut i32) {
            if let Some(node) = node {
                let node = node.borrow();
                let x = x * 10 + node.val;
                if node.left.is_none() && node.right.is_none() { // node 是叶子节点
                    *ans += x;
                    return;
                }
                dfs(&node.left, x, ans);
                dfs(&node.right, x, ans);
            }
        }
        let mut ans = 0;
        dfs(&root, 0, &mut ans);
        ans
    }
}
```

## 写法二：有返回值

也可以把数字之和放到 DFS 的返回值中。对于部分语言，这样写可以直接递归调用 $\texttt{sumNumbers}$，更加简洁。

```py [sol-Python3]
class Solution:
    def sumNumbers(self, root: Optional[TreeNode], x=0) -> int:
        if root is None:
            return 0
        x = x * 10 + root.val
        if root.left is None and root.right is None:  # root 是叶子节点
            return x
        return self.sumNumbers(root.left, x) + self.sumNumbers(root.right, x)
```

```java [sol-Java]
class Solution {
    public int sumNumbers(TreeNode root) {
        return dfs(root, 0);
    }

    private int dfs(TreeNode node, int x) {
        if (node == null) {
            return 0;
        }
        x = x * 10 + node.val;
        if (node.left == null && node.right == null) { // node 是叶子节点
            return x;
        }
        return dfs(node.left, x) + dfs(node.right, x);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int sumNumbers(TreeNode* root, int x = 0) {
        if (root == nullptr) {
            return 0;
        }
        x = x * 10 + root->val;
        if (root->left == nullptr && root->right == nullptr) { // root 是叶子节点
            return x;
        }
        return sumNumbers(root->left, x) + sumNumbers(root->right, x);
    }
};
```

```c [sol-C]
int dfs(struct TreeNode* node, int x) {
    if (node == NULL) {
        return 0;
    }
    x = x * 10 + node->val;
    if (node->left == NULL && node->right == NULL) { // node 是叶子节点
        return x;
    }
    return dfs(node->left, x) + dfs(node->right, x);
}

int sumNumbers(struct TreeNode* root) {
    return dfs(root, 0);
}
```

```go [sol-Go]
func sumNumbers(root *TreeNode) int {
    return dfs(root, 0)
}

func dfs(root *TreeNode, x int) int {
    if root == nil {
        return 0
    }
    x = x*10 + root.Val
    if root.Left == nil && root.Right == nil { // root 是叶子节点
        return x
    }
    return dfs(root.Left, x) + dfs(root.Right, x)
}
```

```js [sol-JavaScript]
var sumNumbers = function(root, x = 0) {
    if (root === null) {
        return 0;
    }
    x = x * 10 + root.val;
    if (root.left === null && root.right === null) { // root 是叶子节点
        return x;
    }
    return sumNumbers(root.left, x) + sumNumbers(root.right, x);
};
```

```rust [sol-Rust]
use std::rc::Rc;
use std::cell::RefCell;

impl Solution {
    pub fn sum_numbers(root: Option<Rc<RefCell<TreeNode>>>) -> i32 {
        fn dfs(node: &Option<Rc<RefCell<TreeNode>>>, x: i32) -> i32 {
            if let Some(node) = node {
                let node = node.borrow();
                let x = x * 10 + node.val;
                if node.left.is_none() && node.right.is_none() { // node 是叶子节点
                    return x;
                }
                return dfs(&node.left, x) + dfs(&node.right, x);
            }
            0
        }
        dfs(&root, 0)
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为二叉树的节点个数。
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
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)
