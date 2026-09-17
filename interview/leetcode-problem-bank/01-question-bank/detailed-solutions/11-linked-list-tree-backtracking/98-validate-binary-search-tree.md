# 98. 验证二叉搜索树

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/validate-binary-search-tree/
- 题目 slug：`validate-binary-search-tree`
- 来源专题：链表、树与回溯
- 来源分类路径：二、二叉树 / §2.9 二叉搜索树
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/validate-binary-search-tree/solutions/2020306/qian-xu-zhong-xu-hou-xu-san-chong-fang-f-yxvh/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【视频】前序中序后序，三种方法，一个视频讲透！（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/validate-binary-search-tree/solutions/2020306/qian-xu-zhong-xu-hou-xu-san-chong-fang-f-yxvh/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`qian-xu-zhong-xu-hou-xu-san-chong-fang-f-yxvh`
- topic id：`2020306`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:39:45 +0800

## 视频讲解

请看[【基础算法精讲 11】](https://www.bilibili.com/video/BV14G411P7C1/)，制作不易，欢迎点赞关注~

## 方法一：前序遍历

$\textit{dfs}$ 额外传入两个参数，分别表示从根到当前节点路径上的最小值和最大值。

当前节点的值必须在最小值和最大值之间（不能等于）。原理见视频。

### 答疑

**问**：为什么 Java 等语言要用 $\texttt{long}$ 类型？题目不是只有 $\texttt{int}$ 类型吗？

**答**：虽然题目是 $\texttt{int}$ 类型，但开始递归的时候，$\textit{left}$ 需要比所有节点值都要小，$\textit{right}$ 需要比所有节点值都要大，如果节点值刚好是 $\texttt{int}$ 的最小值/最大值，就没有这样的 $\textit{left}$ 和 $\textit{right}$ 了，所以需要用 $\texttt{long}$ 类型。

```py [sol-Python3]
class Solution:
    def isValidBST(self, root: Optional[TreeNode], left=-inf, right=inf) -> bool:
        if root is None:
            return True
        x = root.val
        return left < x < right and \
               self.isValidBST(root.left, left, x) and \
               self.isValidBST(root.right, x, right)
```

```java [sol-Java]
class Solution {
    public boolean isValidBST(TreeNode root) {
        return isValidBST(root, Long.MIN_VALUE, Long.MAX_VALUE);
    }

    private boolean isValidBST(TreeNode node, long left, long right) {
        if (node == null) {
            return true;
        }
        long x = node.val;
        return left < x && x < right &&
               isValidBST(node.left, left, x) &&
               isValidBST(node.right, x, right);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    bool isValidBST(TreeNode* root, long long left = LLONG_MIN, long long right = LLONG_MAX) {
        if (root == nullptr) {
            return true;
        }
        long long x = root->val;
        return left < x && x < right &&
               isValidBST(root->left, left, x) &&
               isValidBST(root->right, x, right);
    }
};
```

```c [sol-C]
bool dfs(struct TreeNode* node, long long left, long long right) {
    if (node == NULL) {
        return true;
    }
    long long x = node->val;
    return left < x && x < right &&
           dfs(node->left, left, x) &&
           dfs(node->right, x, right);
}

bool isValidBST(struct TreeNode* root) {
    return dfs(root, LLONG_MIN, LLONG_MAX);
}
```

```go [sol-Go]
func dfs(node *TreeNode, left, right int) bool {
    if node == nil {
        return true
    }
    x := node.Val
    return left < x && x < right &&
        dfs(node.Left, left, x) &&
        dfs(node.Right, x, right)
}

func isValidBST(root *TreeNode) bool {
    return dfs(root, math.MinInt, math.MaxInt)
}
```

```js [sol-JavaScript]
var isValidBST = function(root, left=-Infinity, right=Infinity) {
    if (root == null) {
        return true;
    }
    const x = root.val;
    return left < x && x < right &&
           isValidBST(root.left, left, x) &&
           isValidBST(root.right, x, right);
};
```

```rust [sol-Rust]
use std::rc::Rc;
use std::cell::RefCell;

impl Solution {
    pub fn is_valid_bst(root: Option<Rc<RefCell<TreeNode>>>) -> bool {
        fn dfs(node: &Option<Rc<RefCell<TreeNode>>>, left: i64, right: i64) -> bool {
            let Some(node) = node else {
                return true;
            };
            let node = node.borrow();
            let x = node.val as i64;
            left < x && x < right && dfs(&node.left, left, x) && dfs(&node.right, x, right)
        }

        dfs(&root, i64::MIN, i64::MAX)
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为二叉搜索树的节点个数。
- 空间复杂度：$\mathcal{O}(n)$。最坏情况下，二叉搜索树退化成一条链（注意题目没有保证它是**平衡**树），因此递归需要 $\mathcal{O}(n)$ 的栈空间。

## 方法二：中序遍历

本题是二叉搜索树，中序遍历是自然的做法。

中序遍历时，可以把二叉搜索树看成一个有序数组。

怎么判断一个数组是有序数组？比较相邻元素的大小即可。

### 答疑

**问**：如何证明，如果二叉树的中序遍历是严格递增的，那么二叉树一定是二叉搜索树？

**答**：已知条件为，中序遍历是严格递增的。我们要证明这棵二叉树是二叉搜索树。对于这棵二叉树的任意节点 $x$，中序遍历中的在 $x$ 左边的点都是遍历过的点，这包含 $x$ 的左子树，所以 $x$ 的左子树的节点值都严格小于 $x$ 的节点值。中序遍历中的在 $x$ 右边的点都是未遍历过的点，这包含 $x$ 的右子树，所以 $x$ 的右子树的节点值都严格大于 $x$ 的节点值。所以这棵二叉树的每个节点都满足二叉搜索树的性质，所以这棵二叉树是二叉搜索树。

```py [sol-Python3]
class Solution:
    pre = -inf

    def isValidBST(self, root: Optional[TreeNode]) -> bool:
        if root is None:
            return True
        if not self.isValidBST(root.left):  # 左
            return False
        if root.val <= self.pre:  # 中
            return False
        self.pre = root.val
        return self.isValidBST(root.right)  # 右
```

```java [sol-Java]
class Solution {
    private long pre = Long.MIN_VALUE;

    public boolean isValidBST(TreeNode root) {
        if (root == null) {
            return true;
        }
        if (!isValidBST(root.left)) { // 左
            return false;
        }
        if (root.val <= pre) { // 中
            return false;
        }
        pre = root.val;
        return isValidBST(root.right); // 右
    }
}
```

```cpp [sol-C++]
class Solution {
    long long pre = LLONG_MIN;
public:
    bool isValidBST(TreeNode* root) {
        if (root == nullptr) {
            return true;
        }
        if (!isValidBST(root->left)) { // 左
            return false;
        }
        if (root->val <= pre) { // 中
            return false;
        }
        pre = root->val;
        return isValidBST(root->right); // 右
    }
};
```

```c [sol-C]
bool dfs(struct TreeNode* root, long long* pre) {
    if (root == NULL) {
        return true;
    }
    if (!dfs(root->left, pre)) { // 左
        return false;
    }
    if (root->val <= *pre) { // 中
        return false;
    }
    *pre = root->val;
    return dfs(root->right, pre); // 右
}

bool isValidBST(struct TreeNode* root) {
    long long pre = LLONG_MIN;
    return dfs(root, &pre);
}
```

```go [sol-Go]
func isValidBST(root *TreeNode) bool {
    pre := math.MinInt

    var dfs func(*TreeNode) bool
    dfs = func(node *TreeNode) bool {
        if node == nil {
            return true
        }
        if !dfs(node.Left) { // 左
            return false
        }
        if node.Val <= pre { // 中
            return false
        }
        pre = node.Val
        return dfs(node.Right) // 右
    }

    return dfs(root)
}
```

```js [sol-JavaScript]
var isValidBST = function(root) {
    let pre = -Infinity;

    function dfs(node) {
        if (node == null) {
            return true;
        }
        if (!dfs(node.left)) { // 左
            return false;
        }
        if (node.val <= pre) { // 中
            return false;
        }
        pre = node.val;
        return dfs(node.right); // 右
    }

    return dfs(root);
};
```

```rust [sol-Rust]
use std::rc::Rc;
use std::cell::RefCell;

impl Solution {
    pub fn is_valid_bst(root: Option<Rc<RefCell<TreeNode>>>) -> bool {
        fn dfs(node: &Option<Rc<RefCell<TreeNode>>>, pre: &mut i64) -> bool {
            let Some(node) = node else {
                return true;
            };
            let node = node.borrow();
            if !dfs(&node.left, pre) { // 左
                return false;
            }
            if node.val as i64 <= *pre { // 中
                return false;
            }
            *pre = node.val as i64;
            dfs(&node.right, pre) // 右
        }

        let mut pre = i64::MIN;
        dfs(&root, &mut pre)
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为二叉搜索树的节点个数。
- 空间复杂度：$\mathcal{O}(n)$。最坏情况下，二叉搜索树退化成一条链（注意题目没有保证它是**平衡**树），因此递归需要 $\mathcal{O}(n)$ 的栈空间。

## 方法三：后序遍历

$\textit{dfs}$ 返回子树的最小值和最大值，供上面的节点判断是否为二叉搜索树。

```py [sol-Python3]
class Solution:
    def isValidBST(self, root: Optional[TreeNode]) -> bool:
        def dfs(node: Optional[TreeNode]) -> Tuple:
            if node is None:
                return inf, -inf
            l_min, l_max = dfs(node.left)
            r_min, r_max = dfs(node.right)
            x = node.val
            # 也可以在递归完左子树之后立刻判断，如果发现不是二叉搜索树，就不用递归右子树了
            if x <= l_max or x >= r_min:
                return -inf, inf
            return min(l_min, x), max(r_max, x)

        return dfs(root)[1] != inf
```

```java [sol-Java]
class Solution {
    public boolean isValidBST(TreeNode root) {
        return dfs(root)[1] != Long.MAX_VALUE;
    }

    private long[] dfs(TreeNode node) {
        if (node == null) {
            return new long[]{Long.MAX_VALUE, Long.MIN_VALUE};
        }
        long[] left = dfs(node.left);
        long[] right = dfs(node.right);
        long x = node.val;
        // 也可以在递归完左子树之后立刻判断，如果发现不是二叉搜索树，就不用递归右子树了
        if (x <= left[1] || x >= right[0]) {
            return new long[]{Long.MIN_VALUE, Long.MAX_VALUE};
        }
        return new long[]{Math.min(left[0], x), Math.max(right[1], x)};
    }
}
```

```cpp [sol-C++]
class Solution {
    pair<long long, long long> dfs(TreeNode* node) {
        if (node == nullptr) {
            return {LLONG_MAX, LLONG_MIN};
        }
        auto[l_min, l_max] = dfs(node->left);
        auto[r_min, r_max] = dfs(node->right);
        long long x = node->val;
        // 也可以在递归完左子树之后立刻判断，如果发现不是二叉搜索树，就不用递归右子树了
        if (x <= l_max || x >= r_min) {
            return {LLONG_MIN, LLONG_MAX};
        }
        return {min(l_min, x), max(r_max, x)};
    }

public:
    bool isValidBST(TreeNode* root) {
        return dfs(root).second != LLONG_MAX;
    }
};
```

```c [sol-C]
#define MIN(a, b) ((b) < (a) ? (b) : (a))
#define MAX(a, b) ((b) > (a) ? (b) : (a))

typedef struct {
    long long min; // 子树最小值
    long long max; // 子树最大值
} Pair;

Pair dfs(struct TreeNode* node) {
    if (node == NULL) {
        return (Pair) {LLONG_MAX, LLONG_MIN};
    }
    Pair l = dfs(node->left);
    Pair r = dfs(node->right);
    long long x = node->val;
    // 也可以在递归完左子树之后立刻判断，如果发现不是二叉搜索树，就不用递归右子树了
    if (x <= l.max || x >= r.min) {
        return (Pair) {LLONG_MIN, LLONG_MAX};
    }
    return (Pair) {MIN(l.min, x), MAX(r.max, x)};
}

bool isValidBST(struct TreeNode* root) {
    return dfs(root).max != LLONG_MAX;
}
```

```go [sol-Go]
func dfs(node *TreeNode) (int, int) {
    if node == nil {
        return math.MaxInt, math.MinInt
    }
    lMin, lMax := dfs(node.Left)
    rMin, rMax := dfs(node.Right)
    x := node.Val
    // 也可以在递归完左子树之后立刻判断，如果发现不是二叉搜索树，就不用递归右子树了
    if x <= lMax || x >= rMin {
        return math.MinInt, math.MaxInt
    }
    return min(lMin, x), max(rMax, x)
}

func isValidBST(root *TreeNode) bool {
    _, mx := dfs(root)
    return mx != math.MaxInt
}
```

```js [sol-JavaScript]
var isValidBST = function(root) {
    function dfs(node) {
        if (node == null) {
            return [Infinity, -Infinity];
        }
        const [lMin, lMax] = dfs(node.left);
        const [rMin, rMax] = dfs(node.right);
        const x = node.val;
        // 也可以在递归完左子树之后立刻判断，如果发现不是二叉搜索树，就不用递归右子树了
        if (x <= lMax || x >= rMin) {
            return [-Infinity, Infinity];
        }
        return [Math.min(lMin, x), Math.max(rMax, x)];
    }

    return dfs(root)[1] !== Infinity;
};
```

```rust [sol-Rust]
use std::rc::Rc;
use std::cell::RefCell;

impl Solution {
    pub fn is_valid_bst(root: Option<Rc<RefCell<TreeNode>>>) -> bool {
        fn dfs(node: &Option<Rc<RefCell<TreeNode>>>) -> (i64, i64) {
            let Some(node) = node else {
                return (i64::MAX, i64::MIN);
            };
            let node = node.borrow();
            let (l_min, l_max) = dfs(&node.left);
            let (r_min, r_max) = dfs(&node.right);
            let x = node.val as i64;
            // 也可以在递归完左子树之后立刻判断，如果发现不是二叉搜索树，就不用递归右子树了
            if x <= l_max || x >= r_min {
                (i64::MIN, i64::MAX)
            } else {
                (l_min.min(x), r_max.max(x))
            }
        }

        dfs(&root).1 != i64::MAX
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为二叉搜索树的节点个数。
- 空间复杂度：$\mathcal{O}(n)$。最坏情况下，二叉搜索树退化成一条链（注意题目没有保证它是**平衡**树），因此递归需要 $\mathcal{O}(n)$ 的栈空间。

## 点评

- **前序遍历**在某些数据下不需要递归到叶子节点就能返回（比如根节点左儿子的值大于根节点的值，左儿子就不会继续往下递归了），而中序遍历和后序遍历至少要递归到一个叶子节点。从这个角度上来说，前序遍历是最快的。
- **中序遍历**很好地利用了二叉搜索树的性质，使用到的变量最少。
- **后序遍历**的思想是最通用的，即自底向上计算子问题的过程。**想要学好动态规划的话，请务必掌握自底向上的思想**。

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

本题来自 `二、二叉树 / §2.9 二叉搜索树`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、二叉树 / §2.9 二叉搜索树`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
