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
