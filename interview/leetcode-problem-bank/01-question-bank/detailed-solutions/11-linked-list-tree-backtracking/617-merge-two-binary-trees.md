# 617. 合并二叉树

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/merge-two-binary-trees/
- 题目 slug：`merge-two-binary-trees`
- 来源专题：链表、树与回溯
- 来源分类路径：二、二叉树 / §2.3 自底向上 DFS（后序遍历）
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/merge-two-binary-trees/solutions/2387255/kan-dao-di-gui-jiu-yun-dai-ni-li-jie-di-leixm/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【视频】看到递归就晕？带你理解递归的本质！附题单（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/merge-two-binary-trees/solutions/2387255/kan-dao-di-gui-jiu-yun-dai-ni-li-jie-di-leixm/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`kan-dao-di-gui-jiu-yun-dai-ni-li-jie-di-leixm`
- topic id：`2387255`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:30:25 +0800

晕递归的同学，请看视频：[深入理解递归【基础算法精讲 09】](https://www.bilibili.com/video/BV1UD4y1Y769/)

分类讨论：

- 如果 $\textit{root}_1$ 是空的，无需合并，直接返回 $\textit{root}_2$。
- 如果 $\textit{root}_2$ 是空的，无需合并，直接返回 $\textit{root}_1$。
- 如果都不为空，那么将这两个节点的值相加，作为合并后节点的新值。然后递归合并 $\textit{root}_1.\textit{left}$ 与 $\textit{root}_2.\textit{left}$，得到合并后的左子树；递归合并 $\textit{root}_1.\textit{right}$ 与 $\textit{root}_2.\textit{right}$，得到合并后的右子树。最后返回合并后的节点。

## 写法一

```py [sol-Python3]
class Solution:
    def mergeTrees(self, root1: Optional[TreeNode], root2: Optional[TreeNode]) -> Optional[TreeNode]:
        if root1 is None: return root2
        if root2 is None: return root1
        return TreeNode(root1.val + root2.val,
            self.mergeTrees(root1.left, root2.left),    # 合并左子树
            self.mergeTrees(root1.right, root2.right))  # 合并右子树
```

```java [sol-Java]
class Solution {
    public TreeNode mergeTrees(TreeNode root1, TreeNode root2) {
        if (root1 == null) return root2;
        if (root2 == null) return root1;
        return new TreeNode(root1.val + root2.val,
            mergeTrees(root1.left, root2.left),    // 合并左子树
            mergeTrees(root1.right, root2.right)); // 合并右子树
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    TreeNode* mergeTrees(TreeNode* root1, TreeNode* root2) {
        if (root1 == nullptr) return root2;
        if (root2 == nullptr) return root1;
        return new TreeNode(root1->val + root2->val,
            mergeTrees(root1->left, root2->left),    // 合并左子树
            mergeTrees(root1->right, root2->right)); // 合并右子树
    }
};
```

```c [sol-C]
struct TreeNode* mergeTrees(struct TreeNode* root1, struct TreeNode* root2) {
    if (root1 == NULL) return root2;
    if (root2 == NULL) return root1;
    struct TreeNode* node = malloc(sizeof(struct TreeNode));
    node->val = root1->val + root2->val;
    node->left = mergeTrees(root1->left, root2->left);    // 合并左子树
    node->right = mergeTrees(root1->right, root2->right); // 合并右子树
    return node;
}
```

```go [sol-Go]
func mergeTrees(root1, root2 *TreeNode) *TreeNode {
    if root1 == nil {
        return root2
    }
    if root2 == nil {
        return root1
    }
    return &TreeNode{root1.Val + root2.Val,
        mergeTrees(root1.Left, root2.Left),   // 合并左子树
        mergeTrees(root1.Right, root2.Right)} // 合并右子树
}
```

```js [sol-JavaScript]
var mergeTrees = function(root1, root2) {
    if (root1 === null) return root2;
    if (root2 === null) return root1;
    return new TreeNode(root1.val + root2.val,
        mergeTrees(root1.left, root2.left),    // 合并左子树
        mergeTrees(root1.right, root2.right)); // 合并右子树
};
```

```rust [sol-Rust]
use std::rc::Rc;
use std::cell::RefCell;

impl Solution {
    pub fn merge_trees(root1: Option<Rc<RefCell<TreeNode>>>, root2: Option<Rc<RefCell<TreeNode>>>) -> Option<Rc<RefCell<TreeNode>>> {
        match (root1, root2) {
            (None, None) => None,
            (Some(node), None) | (None, Some(node)) => Some(node),
            (Some(node1), Some(node2)) => {
                let val = node1.borrow().val + node2.borrow().val;
                let left = Self::merge_trees(node1.borrow_mut().left.take(), node2.borrow_mut().left.take());    // 合并左子树
                let right = Self::merge_trees(node1.borrow_mut().right.take(), node2.borrow_mut().right.take()); // 合并右子树
                Some(Rc::new(RefCell::new(TreeNode { val, left, right })))
            }
        }
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(\min(n,m))$，其中 $n,m$ 分别为两棵二叉树的节点数。
- 空间复杂度：$\mathcal{O}(\min(n,m))$。

## 写法二

也可以在两个节点都不为空时，把新节点的 $\textit{val}$，$\textit{left}$ 和 $\textit{right}$ 直接保存到 $\textit{root}_1$ 中。

```py [sol-Python3]
class Solution:
    def mergeTrees(self, root1: Optional[TreeNode], root2: Optional[TreeNode]) -> Optional[TreeNode]:
        if root1 is None: return root2
        if root2 is None: return root1
        root1.val += root2.val
        root1.left = self.mergeTrees(root1.left, root2.left)     # 合并左子树
        root1.right = self.mergeTrees(root1.right, root2.right)  # 合并右子树
        return root1
```

```java [sol-Java]
class Solution {
    public TreeNode mergeTrees(TreeNode root1, TreeNode root2) {
        if (root1 == null) return root2;
        if (root2 == null) return root1;
        root1.val += root2.val;
        root1.left = mergeTrees(root1.left, root2.left);    // 合并左子树
        root1.right = mergeTrees(root1.right, root2.right); // 合并右子树
        return root1;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    TreeNode* mergeTrees(TreeNode* root1, TreeNode* root2) {
        if (root1 == nullptr) return root2;
        if (root2 == nullptr) return root1;
        root1->val += root2->val;
        root1->left = mergeTrees(root1->left, root2->left);    // 合并左子树
        root1->right = mergeTrees(root1->right, root2->right); // 合并右子树
        return root1;
    }
};
```

```c [sol-C]
struct TreeNode* mergeTrees(struct TreeNode* root1, struct TreeNode* root2) {
    if (root1 == NULL) return root2;
    if (root2 == NULL) return root1;
    root1->val += root2->val;
    root1->left = mergeTrees(root1->left, root2->left);    // 合并左子树
    root1->right = mergeTrees(root1->right, root2->right); // 合并右子树
    return root1;
}
```

```go [sol-Go]
func mergeTrees(root1, root2 *TreeNode) *TreeNode {
	if root1 == nil {
		return root2
	}
	if root2 == nil {
		return root1
	}
	root1.Val += root2.Val
	root1.Left = mergeTrees(root1.Left, root2.Left)    // 合并左子树
	root1.Right = mergeTrees(root1.Right, root2.Right) // 合并右子树
	return root1
}
```

```js [sol-JavaScript]
var mergeTrees = function(root1, root2) {
    if (root1 === null) return root2;
    if (root2 === null) return root1;
    root1.val += root2.val;
    root1.left = mergeTrees(root1.left, root2.left);    // 合并左子树
    root1.right = mergeTrees(root1.right, root2.right); // 合并右子树
    return root1;
};
```

```rust [sol-Rust]
use std::rc::Rc;
use std::cell::RefCell;

impl Solution {
    pub fn merge_trees(root1: Option<Rc<RefCell<TreeNode>>>, root2: Option<Rc<RefCell<TreeNode>>>) -> Option<Rc<RefCell<TreeNode>>> {
        match (root1, root2) {
            (None, None) => None,
            (Some(node), None) | (None, Some(node)) => Some(node),
            (Some(node1), Some(node2)) => {
                node1.borrow_mut().val += node2.borrow().val;
                let left = Self::merge_trees(node1.borrow_mut().left.take(), node2.borrow_mut().left.take());    // 合并左子树
                let right = Self::merge_trees(node1.borrow_mut().right.take(), node2.borrow_mut().right.take()); // 合并右子树
                node1.borrow_mut().left = left;
                node1.borrow_mut().right = right;
                Some(node1)
            }
        }
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(\min(n,m))$，其中 $n,m$ 分别为两棵二叉树的节点数。
- 空间复杂度：$\mathcal{O}(\min(n,m))$。递归需要 $\mathcal{O}(\min(n,m))$ 的栈空间。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与一般树（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `二、二叉树 / §2.3 自底向上 DFS（后序遍历）`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、二叉树 / §2.3 自底向上 DFS（后序遍历）`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
