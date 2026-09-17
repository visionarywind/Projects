# 404. 左叶子之和

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/sum-of-left-leaves/
- 题目 slug：`sum-of-left-leaves`
- 来源专题：链表、树与回溯
- 来源分类路径：二、二叉树 / §2.1 遍历二叉树
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/sum-of-left-leaves/solutions/3751068/jian-dan-ti-jian-dan-zuo-pythonjavaccgoj-w8st/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种 DFS 写法（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/sum-of-left-leaves/solutions/3751068/jian-dan-ti-jian-dan-zuo-pythonjavaccgoj-w8st/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`jian-dan-ti-jian-dan-zuo-pythonjavaccgoj-w8st`
- topic id：`3751068`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:30:25 +0800

## 写法一

在遍历二叉树的过程中，如果当前节点的左儿子是叶子，即**左儿子的左右儿子均为空**，那么把左儿子的节点值加到答案中。

晕递归的同学，请先看视频：[深入理解递归【基础算法精讲 09】](https://www.bilibili.com/video/BV1UD4y1Y769/)。

```py [sol-Python3]
class Solution:
    def sumOfLeftLeaves(self, root: Optional[TreeNode]) -> int:
        def dfs(node: Optional[TreeNode]) -> None:
            if node is None:
                return

            # 递归，寻找左叶子
            dfs(node.left)
            dfs(node.right)

            left = node.left  # 当前节点的左儿子
            if left and left.left is None and left.right is None:  # 当前节点的左儿子是叶子
                nonlocal ans
                ans += left.val  # 累加节点值

        ans = 0
        dfs(root)
        return ans
```

```java [sol-Java]
class Solution {
    private int ans = 0;

    public int sumOfLeftLeaves(TreeNode root) {
        dfs(root);
        return ans;
    }

    private void dfs(TreeNode node) {
        if (node == null) {
            return;
        }

        // 递归，寻找左叶子
        dfs(node.left);
        dfs(node.right);

        TreeNode left = node.left; // 当前节点的左儿子
        if (left != null && left.left == null && left.right == null) { // 当前节点的左儿子是叶子
            ans += left.val; // 累加节点值
        }
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int sumOfLeftLeaves(TreeNode* root) {
        int ans = 0;

        auto dfs = [&](this auto&& dfs, TreeNode* node) -> void {
            if (node == nullptr) {
                return;
            }

            // 递归，寻找左叶子
            dfs(node->left);
            dfs(node->right);

            TreeNode* left = node->left; // 当前节点的左儿子
            if (left && left->left == nullptr && left->right == nullptr) { // 当前节点的左儿子是叶子
                ans += left->val; // 累加节点值
            }
        };

        dfs(root);
        return ans;
    }
};
```

```c [sol-C]
int sumOfLeftLeaves(struct TreeNode* root) {
    int ans = 0;

    void dfs(struct TreeNode* node) {
        if (node == NULL) {
            return;
        }

        // 递归，寻找左叶子
        dfs(node->left);
        dfs(node->right);

        struct TreeNode* left = node->left; // 当前节点的左儿子
        if (left && left->left == NULL && left->right == NULL) { // 当前节点的左儿子是叶子
            ans += left->val; // 累加节点值
        }
    }

    dfs(root);
    return ans;
}
```

```c [sol-C 写法二]
void dfs(struct TreeNode* node, int* ans) {
    if (node == NULL) {
        return;
    }

    // 递归，寻找左叶子
    dfs(node->left, ans);
    dfs(node->right, ans);

    struct TreeNode* left = node->left; // 当前节点的左儿子
    if (left && left->left == NULL && left->right == NULL) { // 当前节点的左儿子是叶子
        *ans += left->val; // 累加节点值
    }
}

int sumOfLeftLeaves(struct TreeNode* root) {
    int ans = 0;
    dfs(root, &ans);
    return ans;
}
```

```go [sol-Go]
func sumOfLeftLeaves(root *TreeNode) (ans int) {
    var dfs func(*TreeNode)
    dfs = func(node *TreeNode) {
        if node == nil {
            return
        }

        // 递归，寻找左叶子
        dfs(node.Left)
        dfs(node.Right)

        left := node.Left // 当前节点的左儿子
        if left != nil && left.Left == nil && left.Right == nil { // 当前节点的左儿子是叶子
            ans += left.Val // 累加节点值
        }
    }

    dfs(root)
    return
}
```

```js [sol-JavaScript]
var sumOfLeftLeaves = function(root) {
    let ans = 0;

    function dfs(node) {
        if (node === null) {
            return;
        }

        // 递归，寻找左叶子
        dfs(node.left);
        dfs(node.right);

        const left = node.left; // 当前节点的左儿子
        if (left && left.left === null && left.right === null) { // 当前节点的左儿子是叶子
            ans += left.val; // 累加节点值
        }
    }

    dfs(root);
    return ans;
};
```

```rust [sol-Rust]
use std::rc::Rc;
use std::cell::RefCell;

impl Solution {
    pub fn sum_of_left_leaves(root: Option<Rc<RefCell<TreeNode>>>) -> i32 {
        fn dfs(node: &Option<Rc<RefCell<TreeNode>>>, ans: &mut i32) {
            if let Some(n) = node {
                let n = n.borrow();

                // 递归，寻找左叶子
                dfs(&n.left, ans);
                dfs(&n.right, ans);

                if let Some(left) = &n.left { // 当前节点的左儿子
                    let left = left.borrow();
                    if left.left.is_none() && left.right.is_none() { // 当前节点的左儿子是叶子
                        *ans += left.val; // 累加节点值
                    }
                }
            }
        }

        let mut ans = 0;
        dfs(&root, &mut ans);
        ans
    }
}
```

## 写法二

直接把 $\texttt{sumOfLeftLeaves}$ 当作递归函数使用。

```py [sol-Python3]
class Solution:
    def sumOfLeftLeaves(self, root: TreeNode) -> int:
        if root is None:  # 空节点
            return 0

        # 递归左右子树，收集左右子树中的「节点的左儿子是叶子」的叶子节点值之和
        ans = self.sumOfLeftLeaves(root.left) + self.sumOfLeftLeaves(root.right)

        left = root.left  # 当前节点的左儿子
        if left and left.left is None and left.right is None:  # 当前节点的左儿子是叶子
            ans += left.val  # 累加节点值

        return ans
```

```java [sol-Java]
class Solution {
    public int sumOfLeftLeaves(TreeNode root) {
        if (root == null) { // 空节点
            return 0;
        }

        // 递归左右子树，收集左右子树中的「节点的左儿子是叶子」的叶子节点值之和
        int sum = sumOfLeftLeaves(root.left) + sumOfLeftLeaves(root.right);

        TreeNode left = root.left; // 当前节点的左儿子
        if (left != null && left.left == null && left.right == null) { // 当前节点的左儿子是叶子
            sum += left.val; // 累加节点值
        }

        return sum;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int sumOfLeftLeaves(TreeNode* root) {
        if (root == nullptr) { // 空节点
            return 0;
        }

        // 递归左右子树，收集左右子树中的「节点的左儿子是叶子」的叶子节点值之和
        int sum = sumOfLeftLeaves(root->left) + sumOfLeftLeaves(root->right);

        TreeNode* left = root->left; // 当前节点的左儿子
        if (left && left->left == nullptr && left->right == nullptr) { // 当前节点的左儿子是叶子
            sum += left->val; // 累加节点值
        }

        return sum;
    }
};
```

```c [sol-C]
int sumOfLeftLeaves(struct TreeNode* root) {
    if (root == NULL) { // 空节点
        return 0;
    }

    // 递归左右子树，收集左右子树中的「节点的左儿子是叶子」的叶子节点值之和
    int sum = sumOfLeftLeaves(root->left) + sumOfLeftLeaves(root->right);

    struct TreeNode* left = root->left; // 当前节点的左儿子
    if (left && left->left == NULL && left->right == NULL) { // 当前节点的左儿子是叶子
        sum += left->val; // 累加节点值
    }

    return sum;
}
```

```go [sol-Go]
func sumOfLeftLeaves(root *TreeNode) int {
    if root == nil { // 空节点
        return 0
    }

    // 递归左右子树，收集左右子树中的「左儿子是叶子」的节点值之和
    sum := sumOfLeftLeaves(root.Left) + sumOfLeftLeaves(root.Right)

    left := root.Left // 当前节点的左儿子
    if left != nil && left.Left == nil && left.Right == nil { // 当前节点的左儿子是叶子
        sum += left.Val // 累加节点值
    }

    return sum
}
```

```js [sol-JavaScript]
var sumOfLeftLeaves = function(root) {
    if (root === null) { // 空节点
        return 0;
    }

    // 递归左右子树，收集左右子树中的「节点的左儿子是叶子」的叶子节点值之和
    let sum = sumOfLeftLeaves(root.left) + sumOfLeftLeaves(root.right);

    const left = root.left; // 当前节点的左儿子
    if (left && left.left === null && left.right === null) { // 当前节点的左儿子是叶子
        sum += left.val; // 累加节点值
    }

    return sum;
};
```

```rust [sol-Rust]
use std::rc::Rc;
use std::cell::RefCell;

impl Solution {
    pub fn sum_of_left_leaves(root: Option<Rc<RefCell<TreeNode>>>) -> i32 {
        fn dfs(node: &Option<Rc<RefCell<TreeNode>>>) -> i32 {
            if let Some(n) = node {
                let n = n.borrow();
                // 递归左右子树，收集左右子树中的「节点的左儿子是叶子」的叶子节点值之和
                let mut sum = dfs(&n.left) + dfs(&n.right);

                if let Some(left) = &n.left { // 当前节点的左儿子
                    let left = left.borrow();
                    if left.left.is_none() && left.right.is_none() { // 当前节点的左儿子是叶子
                        sum += left.val; // 累加节点值
                    }
                }

                sum
            } else {
                0 // 空节点
            }
        }
        dfs(&root)
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是二叉树的节点个数。
- 空间复杂度：$\mathcal{O}(h)$，其中 $h$ 是二叉树的高度。递归需要 $\mathcal{O}(h)$ 的栈空间。

## 分类题单

[如何科学刷题？](https://leetcode.cn/discuss/post/3141566/ru-he-ke-xue-shua-ti-by-endlesscheng-q3yd/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/discuss/post/3578981/ti-dan-hua-dong-chuang-kou-ding-chang-bu-rzz7/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/discuss/post/3579164/ti-dan-er-fen-suan-fa-er-fen-da-an-zui-x-3rqn/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/discuss/post/3579480/ti-dan-dan-diao-zhan-ju-xing-xi-lie-zi-d-u4hk/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/discuss/post/3580195/fen-xiang-gun-ti-dan-wang-ge-tu-dfsbfszo-l3pa/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/discuss/post/3580371/fen-xiang-gun-ti-dan-wei-yun-suan-ji-chu-nth4/)
6. [图论算法（DFS/BFS/拓扑排序/基环树/最短路/最小生成树/网络流）](https://leetcode.cn/discuss/post/3581143/fen-xiang-gun-ti-dan-tu-lun-suan-fa-dfsb-qyux/)
7. [动态规划（入门/背包/划分/状态机/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/discuss/post/3581838/fen-xiang-gun-ti-dan-dong-tai-gui-hua-ru-007o/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/discuss/post/3583665/fen-xiang-gun-ti-dan-chang-yong-shu-ju-j-bvmv/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/discuss/post/3584388/fen-xiang-gun-ti-dan-shu-xue-suan-fa-shu-gcai/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/discuss/post/3091107/fen-xiang-gun-ti-dan-tan-xin-ji-ben-tan-k58yb/)
11. [链表、树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/discuss/post/3142882/fen-xiang-gun-ti-dan-lian-biao-er-cha-sh-6srp/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/discuss/post/3144832/fen-xiang-gun-ti-dan-zi-fu-chuan-kmpzhan-ugt4/)

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
