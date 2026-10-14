# 100. 相同的树

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/same-tree/
- 题目 slug：`same-tree`
- 来源专题：链表、树与回溯
- 来源分类路径：二、二叉树 / §2.3 自底向上 DFS（后序遍历）
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/same-tree/solutions/2015056/ru-he-ling-huo-yun-yong-di-gui-lai-kan-s-empk/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【视频】如何灵活运用递归？（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/same-tree/solutions/2015056/ru-he-ling-huo-yun-yong-di-gui-lai-kan-s-empk/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`ru-he-ling-huo-yun-yong-di-gui-lai-kan-s-empk`
- topic id：`2015056`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:30:25 +0800

## 视频讲解

看完这两期视频，让你对递归的理解更上一层楼！

递归原理：[【基础算法精讲 09】](https://www.bilibili.com/video/BV1UD4y1Y769/)

本题讲解：[【基础算法精讲 10】](https://www.bilibili.com/video/BV18M411z7bb/)

```py [sol-Python3]
class Solution:
    def isSameTree(self, p: Optional[TreeNode], q: Optional[TreeNode]) -> bool:
        if p is None or q is None:
            return p is q  # 必须都是 None
        return p.val == q.val and self.isSameTree(p.left, q.left) and self.isSameTree(p.right, q.right)
```

```java [sol-Java]
class Solution {
    public boolean isSameTree(TreeNode p, TreeNode q) {
        if (p == null || q == null) {
            return p == q; // 必须都是 null
        }
        return p.val == q.val && isSameTree(p.left, q.left) && isSameTree(p.right, q.right);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    bool isSameTree(TreeNode* p, TreeNode* q) {
        if (p == nullptr || q == nullptr) {
            return p == q; // 必须都是 nullptr
        }
        return p->val == q->val && isSameTree(p->left, q->left) && isSameTree(p->right, q->right);
    }
};
```

```c [sol-C]
bool isSameTree(struct TreeNode* p, struct TreeNode* q) {
    if (p == NULL || q == NULL) {
        return p == q; // 必须都是 NULL
    }
    return p->val == q->val && isSameTree(p->left, q->left) && isSameTree(p->right, q->right);
}
```

```go [sol-Go]
func isSameTree(p, q *TreeNode) bool {
    if p == nil || q == nil {
        return p == q // 必须都是 nil
    }
    return p.Val == q.Val && isSameTree(p.Left, q.Left) && isSameTree(p.Right, q.Right)
}
```

```js [sol-JavaScript]
var isSameTree = function(p, q) {
    if (p === null || q === null) {
        return p === q; // 必须都是 null
    }
    return p.val === q.val && isSameTree(p.left, q.left) && isSameTree(p.right, q.right);
};
```

```rust [sol-Rust]
use std::rc::Rc;
use std::cell::RefCell;

impl Solution {
    pub fn is_same_tree(p: Option<Rc<RefCell<TreeNode>>>, q: Option<Rc<RefCell<TreeNode>>>) -> bool {
        match (p, q) {
            (Some(p), Some(q)) => {
                let p = p.borrow();
                let q = q.borrow();
                p.val == q.val &&
                Self::is_same_tree(p.left.clone(), q.left.clone()) &&
                Self::is_same_tree(p.right.clone(), q.right.clone())
            }
            (None, None) => true,
            _ => false,
        }
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(\min(n,m))$，其中 $n$ 为 $p$ 的节点个数，$m$ 为 $q$ 的节点个数。
- 空间复杂度：$\mathcal{O}(\min(n,m))$。最坏情况下，二叉树退化成一条链，递归需要 $\mathcal{O}(\min(n,m))$ 的栈空间。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. 【本题相关】[链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)
