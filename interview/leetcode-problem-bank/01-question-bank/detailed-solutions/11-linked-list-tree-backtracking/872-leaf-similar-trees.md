# 872. 叶子相似的树

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/leaf-similar-trees/
- 题目 slug：`leaf-similar-trees`
- 来源专题：链表、树与回溯
- 来源分类路径：二、二叉树 / §2.1 遍历二叉树
- 难度分：1288
- 外部题解来源：https://leetcode.cn/problems/leaf-similar-trees/solutions/3741528/jian-dan-ti-jian-dan-zuo-pythonjavacgojs-b8ns/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[简单题，简单做（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/leaf-similar-trees/solutions/3741528/jian-dan-ti-jian-dan-zuo-pythonjavacgojs-b8ns/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`jian-dan-ti-jian-dan-zuo-pythonjavacgojs-b8ns`
- topic id：`3741528`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:30:25 +0800

按照同样的 DFS 顺序遍历这两棵二叉树，比如都按照先左子树再右子树的方式。

遍历到叶子时，把节点值加到一个列表中。

最后判断两个列表是否相同。

```py [sol-Python3]
class Solution:
    def leafValues(self, root: Optional[TreeNode]) -> List[int]:
        res = []
        def dfs(node: Optional[TreeNode]) -> None:
            if node is None:  # 空节点
                return
            if node.left is None and node.right is None:  # 叶子
                res.append(node.val)
                return
            dfs(node.left)
            dfs(node.right)
        dfs(root)
        return res

    def leafSimilar(self, root1: Optional[TreeNode], root2: Optional[TreeNode]) -> bool:
        return self.leafValues(root1) == self.leafValues(root2)
```

```java [sol-Java]
class Solution {
    public boolean leafSimilar(TreeNode root1, TreeNode root2) {
        return leafValues(root1).equals(leafValues(root2));
    }

    private List<Integer> leafValues(TreeNode root) {
        List<Integer> res = new ArrayList<>();
        dfs(root, res);
        return res;
    }

    private void dfs(TreeNode node, List<Integer> res) {
        if (node == null) { // 空节点
            return;
        }
        if (node.left == null && node.right == null) { // 叶子
            res.add(node.val);
            return;
        }
        dfs(node.left, res);
        dfs(node.right, res);
    }
}
```

```cpp [sol-C++]
class Solution {
    vector<int> leafValues(TreeNode* root) {
        vector<int> res;
        auto dfs = [&](this auto&& dfs, TreeNode* node) -> void {
            if (node == nullptr) { // 空节点
                return;
            }
            if (node->left == nullptr && node->right == nullptr) { // 叶子
                res.push_back(node->val);
                return;
            }
            dfs(node->left);
            dfs(node->right);
        };
        dfs(root);
        return res;
    }

public:
    bool leafSimilar(TreeNode* root1, TreeNode* root2) {
        return leafValues(root1) == leafValues(root2);
    }
};
```

```go [sol-Go]
func leafValues(root *TreeNode) (res []int) {
    var dfs func(*TreeNode)
    dfs = func(node *TreeNode) {
        if node == nil { // 空节点
            return
        }
        if node.Left == nil && node.Right == nil { // 叶子
            res = append(res, node.Val)
            return
        }
        dfs(node.Left)
        dfs(node.Right)
    }
    dfs(root)
    return
}

func leafSimilar(root1, root2 *TreeNode) bool {
    return slices.Equal(leafValues(root1), leafValues(root2))
}
```

```js [sol-JavaScript]
var leafValues = function(root) {
    const res = [];
    function dfs(node) {
        if (node === null) { // 空节点
            return;
        }
        if (node.left === null && node.right === null) { // 叶子
            res.push(node.val);
            return;
        }
        dfs(node.left);
        dfs(node.right);
    }
    dfs(root);
    return res;
};

var leafSimilar = function(root1, root2) {
    return _.isEqual(leafValues(root1), leafValues(root2));
};
```

```rust [sol-Rust]
use std::rc::Rc;
use std::cell::RefCell;

impl Solution {
    fn leaf_values(root: Option<Rc<RefCell<TreeNode>>>) -> Vec<i32> {
        let mut res = vec![];
        fn dfs(node: &Option<Rc<RefCell<TreeNode>>>, res: &mut Vec<i32>) {
            if let Some(n) = node {
                let n = n.borrow();
                if n.left.is_none() && n.right.is_none() { // 叶子
                    res.push(n.val);
                    return;
                }
                dfs(&n.left, res);
                dfs(&n.right, res);
            }
        }
        dfs(&root, &mut res);
        res
    }

    pub fn leaf_similar(root1: Option<Rc<RefCell<TreeNode>>>, root2: Option<Rc<RefCell<TreeNode>>>) -> bool {
        Self::leaf_values(root1) == Self::leaf_values(root2)
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n+m)$，其中 $n$ 和 $m$ 分别是两棵二叉树的大小（节点个数）。
- 空间复杂度：$\mathcal{O}(n+m)$。

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
11. 【本题相关】[链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)
