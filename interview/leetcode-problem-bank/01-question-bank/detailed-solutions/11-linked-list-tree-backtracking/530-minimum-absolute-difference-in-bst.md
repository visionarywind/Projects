# 530. 二叉搜索树的最小绝对差

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-absolute-difference-in-bst/
- 题目 slug：`minimum-absolute-difference-in-bst`
- 来源专题：链表、树与回溯
- 来源分类路径：二、二叉树 / §2.9 二叉搜索树
- 难度分：1303
- 外部题解来源：https://leetcode.cn/problems/minimum-absolute-difference-in-bst/solutions/3038197/zhong-xu-bian-li-jian-ji-xie-fa-pythonja-3pwf/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[中序遍历，简洁写法（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/minimum-absolute-difference-in-bst/solutions/3038197/zhong-xu-bian-li-jian-ji-xie-fa-pythonja-3pwf/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`zhong-xu-bian-li-jian-ji-xie-fa-pythonja-3pwf`
- topic id：`3038197`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:39:45 +0800

示例 1，把数字排序后，得到 $[1,2,3,4,6]$。最小差值一定是相邻两个数的差值。

由于二叉搜索树的**中序遍历**得到的序列是有序的，我们可以计算中序遍历中的相邻两数的差值，取最小值，即为答案。

> **中序遍历**：左子树 $\to$ 根 $\to$ 右子树。

```py [sol-Python3]
class Solution:
    def getMinimumDifference(self, root: Optional[TreeNode]) -> int:
        ans = inf
        pre = -inf

        def dfs(node: Optional[TreeNode]) -> None:
            if node is None:
                return
            dfs(node.left)
            nonlocal ans, pre
            ans = min(ans, node.val - pre)
            pre = node.val
            dfs(node.right)

        dfs(root)
        return ans
```

```java [sol-Java]
class Solution {
    private int ans = Integer.MAX_VALUE;
    private int pre = Integer.MIN_VALUE / 2; // 防止减法溢出

    public int getMinimumDifference(TreeNode root) {
        dfs(root);
        return ans;
    }

    private void dfs(TreeNode node) {
        if (node == null) {
            return;
        }
        dfs(node.left);
        ans = Math.min(ans, node.val - pre);
        pre = node.val;
        dfs(node.right);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int getMinimumDifference(TreeNode* root) {
        int ans = INT_MAX;
        int pre = INT_MIN / 2; // 防止减法溢出

        auto dfs = [&](this auto&& dfs, TreeNode* node) -> void {
            if (node == nullptr) {
                return;
            }
            dfs(node->left);
            ans = min(ans, node->val - pre);
            pre = node->val;
            dfs(node->right);
        };

        dfs(root);
        return ans;
    }
};
```

```c [sol-C]
#define MIN(a, b) ((b) < (a) ? (b) : (a))

void dfs(struct TreeNode* node, int* ans, int* pre) {
    if (node == NULL) {
        return;
    }
    dfs(node->left, ans, pre);
    *ans = MIN(*ans, node->val - *pre);
    *pre = node->val;
    dfs(node->right, ans, pre);
}

int getMinimumDifference(struct TreeNode* root) {
    int ans = INT_MAX;
    int pre = INT_MIN / 2; // 防止减法溢出
    dfs(root, &ans, &pre);
    return ans;
}
```

```go [sol-Go]
func getMinimumDifference(root *TreeNode) int {
    ans := math.MaxInt
    pre := math.MinInt / 2 // 防止减法溢出

    var dfs func(*TreeNode)
    dfs = func(node *TreeNode) {
        if node == nil {
            return
        }
        dfs(node.Left)
        ans = min(ans, node.Val-pre)
        pre = node.Val
        dfs(node.Right)
    }

    dfs(root)
    return ans
}
```

```js [sol-JavaScript]
var getMinimumDifference = function(root) {
    let ans = Infinity;
    let pre = -Infinity;

    function dfs(node) {
        if (node === null) {
            return;
        }
        dfs(node.left);
        ans = Math.min(ans, node.val - pre);
        pre = node.val;
        dfs(node.right);
    }

    dfs(root);
    return ans;
};
```

```rust [sol-Rust]
use std::rc::Rc;
use std::cell::RefCell;

impl Solution {
    pub fn get_minimum_difference(root: Option<Rc<RefCell<TreeNode>>>) -> i32 {
        fn dfs(node: &Option<Rc<RefCell<TreeNode>>>, ans: &mut i32, pre: &mut i32) {
            if let Some(node) = node {
                let node = node.borrow();
                dfs(&node.left, ans, pre);
                *ans = (*ans).min(node.val - *pre);
                *pre = node.val;
                dfs(&node.right, ans, pre);
            }
        }

        let mut ans = i32::MAX;
        let mut pre = i32::MIN / 2; // 防止减法溢出
        dfs(&root, &mut ans, &mut pre);
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为二叉搜索树的节点个数。
- 空间复杂度：$\mathcal{O}(n)$。最坏情况下，二叉搜索树退化成一条链，递归需要 $\mathcal{O}(n)$ 的栈空间。

更多相似题目，见下面二叉树题单中的「**§2.9 二叉搜索树**」。

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
