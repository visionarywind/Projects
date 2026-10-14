# 110. 平衡二叉树

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/balanced-binary-tree/
- 题目 slug：`balanced-binary-tree`
- 来源专题：链表、树与回溯
- 来源分类路径：二、二叉树 / §2.3 自底向上 DFS（后序遍历）
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/balanced-binary-tree/solutions/2015068/ru-he-ling-huo-yun-yong-di-gui-lai-kan-s-c3wj/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【视频】如何灵活运用递归？（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/balanced-binary-tree/solutions/2015068/ru-he-ling-huo-yun-yong-di-gui-lai-kan-s-c3wj/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`ru-he-ling-huo-yun-yong-di-gui-lai-kan-s-c3wj`
- topic id：`2015068`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:30:25 +0800

看完这两期视频，让你对递归的理解更上一层楼：

- [【基础算法精讲 09】](https://www.bilibili.com/video/BV1UD4y1Y769/)
- [【基础算法精讲 10】](https://www.bilibili.com/video/BV18M411z7bb/)

## 答疑

**问**：代码中的 $-1$ 是怎么产生的？怎么返回的？

**答**：在某次递归中，发现左右子树高度绝对差大于 $1$，我们会返回 $-1$。这个 $-1$ 会一路向上不断返回，直到根节点。

## 写法一

```py [sol-Python3]
class Solution:
    def isBalanced(self, root: Optional[TreeNode]) -> bool:
        def get_height(node: Optional[TreeNode]) -> int:
            if node is None:
                return 0
            left_h = get_height(node.left)
            right_h = get_height(node.right)
            if left_h == -1 or right_h == -1 or abs(left_h - right_h) > 1:
                return -1
            return max(left_h, right_h) + 1
        return get_height(root) != -1
```

```java [sol-Java]
class Solution {
    public boolean isBalanced(TreeNode root) {
        return getHeight(root) != -1;
    }

    private int getHeight(TreeNode node) {
        if (node == null) {
            return 0;
        }
        int leftH = getHeight(node.left);
        int rightH = getHeight(node.right);
        if (leftH == -1 || rightH == -1 || Math.abs(leftH - rightH) > 1) {
            return -1;
        }
        return Math.max(leftH, rightH) + 1;
    }
}
```

```cpp [sol-C++]
class Solution {
    int get_height(TreeNode* node) {
        if (node == nullptr) {
            return 0;
        }
        int left_h = get_height(node->left);
        int right_h = get_height(node->right);
        if (left_h == -1 || right_h == -1 || abs(left_h - right_h) > 1) {
            return -1;
        }
        return max(left_h, right_h) + 1;
    }

public:
    bool isBalanced(TreeNode* root) {
        return get_height(root) != -1;
    }
};
```

```c [sol-C]
#define MAX(a, b) ((b) > (a) ? (b) : (a))

int getHeight(struct TreeNode* node) {
    if (node == NULL) {
        return 0;
    }
    int left_h = getHeight(node->left);
    int right_h = getHeight(node->right);
    if (left_h == -1 || right_h == -1 || abs(left_h - right_h) > 1) {
        return -1;
    }
    return MAX(left_h, right_h) + 1;
}

bool isBalanced(struct TreeNode* root) {
    return getHeight(root) != -1;
}
```

```go [sol-Go]
func getHeight(node *TreeNode) int {
    if node == nil {
        return 0
    }
    leftH := getHeight(node.Left)
    rightH := getHeight(node.Right)
    if leftH == -1 || rightH == -1 || abs(leftH-rightH) > 1 {
        return -1
    }
    return max(leftH, rightH) + 1
}

func isBalanced(root *TreeNode) bool {
    return getHeight(root) != -1
}

func abs(x int) int { if x < 0 { return -x }; return x }
```

```js [sol-JavaScript]
function getHeight(node) {
    if (node === null) {
        return 0;
    }
    const leftH = getHeight(node.left);
    const rightH = getHeight(node.right);
    if (leftH === -1 || rightH === -1 || Math.abs(leftH - rightH) > 1) {
        return -1;
    }
    return Math.max(leftH, rightH) + 1;
}

var isBalanced = function(root) {
    return getHeight(root) !== -1;
};
```

```rust [sol-Rust]
use std::rc::Rc;
use std::cell::RefCell;

impl Solution {
    pub fn is_balanced(root: Option<Rc<RefCell<TreeNode>>>) -> bool {
        fn get_height(node: &Option<Rc<RefCell<TreeNode>>>) -> i32 {
            let Some(node) = node else {
                return 0;
            };
            let node = node.borrow();
            let left_h = get_height(&node.left);
            let right_h = get_height(&node.right);
            if left_h == -1 || right_h == -1 || (left_h - right_h).abs() > 1 {
                return -1;
            }
            left_h.max(right_h) + 1
        }

        get_height(&root) != -1
    }
}
```

## 写法二

```py [sol-Python3]
class Solution:
    def isBalanced(self, root: Optional[TreeNode]) -> bool:
        def get_height(node: Optional[TreeNode]) -> int:
            if node is None:
                return 0
            left_h = get_height(node.left)
            if left_h == -1:
                return -1  # 提前退出，不再递归
            right_h = get_height(node.right)
            if right_h == -1 or abs(left_h - right_h) > 1:
                return -1
            return max(left_h, right_h) + 1
        return get_height(root) != -1
```

```java [sol-Java]
class Solution {
    public boolean isBalanced(TreeNode root) {
        return getHeight(root) != -1;
    }

    private int getHeight(TreeNode node) {
        if (node == null) {
            return 0;
        }
        int leftH = getHeight(node.left);
        if (leftH == -1) {
            return -1; // 提前退出，不再递归
        }
        int rightH = getHeight(node.right);
        if (rightH == -1 || Math.abs(leftH - rightH) > 1) {
            return -1;
        }
        return Math.max(leftH, rightH) + 1;
    }
}
```

```cpp [sol-C++]
class Solution {
    int get_height(TreeNode* node) {
        if (node == nullptr) {
            return 0;
        }
        int left_h = get_height(node->left);
        if (left_h == -1) {
            return -1; // 提前退出，不再递归
        }
        int right_h = get_height(node->right);
        if (right_h == -1 || abs(left_h - right_h) > 1) {
            return -1;
        }
        return max(left_h, right_h) + 1;
    }

public:
    bool isBalanced(TreeNode* root) {
        return get_height(root) != -1;
    }
};
```

```c [sol-C]
#define MAX(a, b) ((b) > (a) ? (b) : (a))

int getHeight(struct TreeNode* node) {
    if (node == NULL) {
        return 0;
    }

    int left_h = getHeight(node->left);
    if (left_h == -1) {
        return -1; // 提前退出，不再递归
    }

    int right_h = getHeight(node->right);
    if (right_h == -1 || abs(left_h - right_h) > 1) {
        return -1;
    }

    return MAX(left_h, right_h) + 1;
}

bool isBalanced(struct TreeNode* root) {
    return getHeight(root) != -1;
}
```

```go [sol-Go]
func getHeight(node *TreeNode) int {
    if node == nil {
        return 0
    }
    leftH := getHeight(node.Left)
    if leftH == -1 {
        return -1 // 提前退出，不再递归
    }
    rightH := getHeight(node.Right)
    if rightH == -1 || abs(leftH-rightH) > 1 {
        return -1
    }
    return max(leftH, rightH) + 1
}

func isBalanced(root *TreeNode) bool {
    return getHeight(root) != -1
}

func abs(x int) int { if x < 0 { return -x }; return x }
```

```js [sol-JavaScript]
function getHeight(node) {
    if (node === null) {
        return 0;
    }
    const leftH = getHeight(node.left);
    if (leftH === -1) {
        return -1; // 提前退出，不再递归
    }
    const rightH = getHeight(node.right);
    if (rightH === -1 || Math.abs(leftH - rightH) > 1) {
        return -1;
    }
    return Math.max(leftH, rightH) + 1;
}

var isBalanced = function(root) {
    return getHeight(root) !== -1;
};
```

```rust [sol-Rust]
use std::rc::Rc;
use std::cell::RefCell;

impl Solution {
    pub fn is_balanced(root: Option<Rc<RefCell<TreeNode>>>) -> bool {
        fn get_height(node: &Option<Rc<RefCell<TreeNode>>>) -> i32 {
            let Some(node) = node else {
                return 0;
            };
            let node = node.borrow();
            let left_h = get_height(&node.left);
            if left_h == -1 {
                return -1; // 提前退出，不再递归
            }
            let right_h = get_height(&node.right);
            if right_h == -1 || (left_h - right_h).abs() > 1 {
                return -1;
            }
            left_h.max(right_h) + 1
        }

        get_height(&root) != -1
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是二叉树的节点个数。
- 空间复杂度：$\mathcal{O}(n)$。最坏情况下，二叉树退化成一条链，递归需要 $\mathcal{O}(n)$ 的栈空间。

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
