# 687. 最长同值路径

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/longest-univalue-path/
- 题目 slug：`longest-univalue-path`
- 来源专题：动态规划
- 来源分类路径：十二、树形 DP / §12.1 树的直径
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/longest-univalue-path/solutions/2227160/shi-pin-che-di-zhang-wo-zhi-jing-dpcong-524j4/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【视频】彻底掌握直径 DP！从二叉树到一般树！（Python/Java/C++/Go）](https://leetcode.cn/problems/longest-univalue-path/solutions/2227160/shi-pin-che-di-zhang-wo-zhi-jing-dpcong-524j4/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`shi-pin-che-di-zhang-wo-zhi-jing-dpcong-524j4`
- topic id：`2227160`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 视频讲解

推荐先看 [深入理解递归【基础算法精讲 09】](https://www.bilibili.com/video/BV1UD4y1Y769/)。

前置知识：[543. 二叉树的直径](https://leetcode.cn/problems/diameter-of-binary-tree/)，视频讲解：[树形 DP【基础算法精讲 23】](https://www.bilibili.com/video/BV17o4y187h1/)。

## 思路

在 [543. 二叉树的直径](https://leetcode.cn/problems/diameter-of-binary-tree/) 的基础上修改：

- 如果当前节点和左子树的值不同，左子树的链长可以视作 $0$，否则就是左子树的链长。
- 如果当前节点和右子树的值不同，右子树的链长可以视作 $0$，否则就是右子树的链长；

## 常见错误

先判断节点值是否相同，不同则不往下递归的写法是错误的。

想一想，如果正确答案在更下面的子树当中呢？

**必须保证每个节点都访问到**。所以正确的写法是先递归，再判断。

```py [sol-Python3]
class Solution:
    def longestUnivaluePath(self, root: Optional[TreeNode]) -> int:
        ans = 0
        def dfs(node: Optional[TreeNode]) -> int:
            if node is None:
                return -1  # 下面 +1 后，对于叶子节点就刚好是 0
            l_len = dfs(node.left) + 1  # 左子树最大链长+1
            r_len = dfs(node.right) + 1  # 右子树最大链长+1
            if node.left and node.left.val != node.val: l_len = 0  # 视作 0
            if node.right and node.right.val != node.val: r_len = 0  # 视作 0
            nonlocal ans
            ans = max(ans, l_len + r_len)  # 两条链拼成路径
            return max(l_len, r_len)  # 当前子树最大链长
        dfs(root)
        return ans
```

```java [sol-Java]
class Solution {
    private int ans;

    public int longestUnivaluePath(TreeNode root) {
        dfs(root);
        return ans;
    }

    private int dfs(TreeNode node) {
        if (node == null) {
            return -1; // 下面 +1 后，对于叶子节点就刚好是 0
        }
        int lLen = dfs(node.left) + 1; // 左子树最大链长+1
        int rLen = dfs(node.right) + 1; // 右子树最大链长+1
        if (node.left != null && node.left.val != node.val) lLen = 0; // 链长视作 0
        if (node.right != null && node.right.val != node.val) rLen = 0; // 链长视作 0
        ans = Math.max(ans, lLen + rLen); // 两条链拼成路径
        return Math.max(lLen, rLen); // 当前子树最大链和
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int longestUnivaluePath(TreeNode* root) {
        int ans = 0;
        auto dfs = [&](this auto&& dfs, TreeNode* node) -> int {
            if (node == nullptr) {
                return -1; // 下面 +1 后，对于叶子节点就刚好是 0
            }
            int l_len = dfs(node->left) + 1; // 左子树最大链长+1
            int r_len = dfs(node->right) + 1; // 右子树最大链长+1
            if (node->left && node->left->val != node->val) l_len = 0; // 链长视作 0
            if (node->right && node->right->val != node->val) r_len = 0; // 链长视作 0
            ans = max(ans, l_len + r_len); // 两条链拼成路径
            return max(l_len, r_len); // 当前子树最大链长
        };
        dfs(root);
        return ans;
    }
};
```

```go [sol-Go]
func longestUnivaluePath(root *TreeNode) (ans int) {
    var dfs func(*TreeNode) int
    dfs = func(node *TreeNode) int {
        if node == nil {
            return -1 // 下面 +1 后，对于叶子节点就刚好是 0
        }
        lLen := dfs(node.Left) + 1  // 左子树最大链长+1
        rLen := dfs(node.Right) + 1 // 右子树最大链长+1
        if node.Left != nil && node.Left.Val != node.Val {
            lLen = 0 // 链长视作 0
        }
        if node.Right != nil && node.Right.Val != node.Val {
            rLen = 0 // 链长视作 0
        }
        ans = max(ans, lLen+rLen) // 两条链拼成路径
        return max(lLen, rLen)    // 当前子树最大链长
    }
    dfs(root)
    return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为二叉树的节点个数。
- 空间复杂度：$\mathcal{O}(n)$。最坏情况下，二叉树退化成一条链，递归需要 $\mathcal{O}(n)$ 的栈空间。

## 课上题目

- [543. 二叉树的直径](https://leetcode.cn/problems/diameter-of-binary-tree/)
- [124. 二叉树中的最大路径和](https://leetcode.cn/problems/binary-tree-maximum-path-sum/)
- [2246. 相邻字符不同的最长路径](https://leetcode.cn/problems/longest-path-with-different-adjacent-characters/)

## 课后作业

- [687. 最长同值路径](https://leetcode.cn/problems/longest-univalue-path/)
- [1617. 统计子树中城市之间最大距离](https://leetcode.cn/problems/count-subtrees-with-max-distance-between-cities/)
- [2539. 最大价值和与最小价值和的差值](https://leetcode.cn/problems/difference-between-maximum-and-minimum-price-sum/)

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
