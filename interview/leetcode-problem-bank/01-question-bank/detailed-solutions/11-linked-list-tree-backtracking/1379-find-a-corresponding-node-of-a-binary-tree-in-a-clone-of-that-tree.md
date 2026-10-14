# 1379. 找出克隆二叉树中的相同节点

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/find-a-corresponding-node-of-a-binary-tree-in-a-clone-of-that-tree/
- 题目 slug：`find-a-corresponding-node-of-a-binary-tree-in-a-clone-of-that-tree`
- 来源专题：链表、树与回溯
- 来源分类路径：二、二叉树 / §2.3 自底向上 DFS（后序遍历）
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/find-a-corresponding-node-of-a-binary-tree-in-a-clone-of-that-tree/solutions/2721724/jian-ji-xie-fa-pythonjavacjs-by-endlessc-3vri/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[简洁写法（Python/Java/C++/JS）](https://leetcode.cn/problems/find-a-corresponding-node-of-a-binary-tree-in-a-clone-of-that-tree/solutions/2721724/jian-ji-xie-fa-pythonjavacjs-by-endlessc-3vri/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`jian-ji-xie-fa-pythonjavacjs-by-endlessc-3vri`
- topic id：`2721724`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:30:25 +0800

我们直接在 $\texttt{getTargetCopy}$ 内部调用它自己，执行递归（先序遍历）：

1. 如果 $\textit{original}$ 是空节点，返回空。
2. 如果 $\textit{original} = \textit{target}$（注意这里比较的是节点，不是节点值），说明我们找到了对应的节点，返回 $\textit{cloned}$。
3. 否则递归 $\textit{original}$ 和 $\textit{cloned}$ 的左子树，如果返回值 $\textit{leftRes}$ 不为空，说明 $\textit{target}$ 在左子树中，返回 $\textit{leftRes}$。
4. 否则递归 $\textit{original}$ 和 $\textit{cloned}$ 的右子树，由于题目保证 $\textit{target}$ 一定在二叉树中，所以直接返回递归右子树的返回值。

晕递归的同学可以看 [深刻理解递归【基础算法精讲 09】](https://www.bilibili.com/video/BV1UD4y1Y769/)

注：由于我们比较的是节点而不是节点值（例如 C++ 比较的是地址），所以下面的代码也适用于树中有值相同节点的情况（本题的进阶问题）。

```py [sol-Python3]
class Solution:
    def getTargetCopy(self, original: TreeNode, cloned: TreeNode, target: TreeNode) -> TreeNode:
        if original is None or original is target:
            return cloned
        return self.getTargetCopy(original.left, cloned.left, target) or \
               self.getTargetCopy(original.right, cloned.right, target)
```

```java [sol-Java]
class Solution {
    public TreeNode getTargetCopy(TreeNode original, TreeNode cloned, TreeNode target) {
        if (original == null || original == target) {
            return cloned;
        }
        TreeNode leftRes = getTargetCopy(original.left, cloned.left, target);
        if (leftRes != null) {
            return leftRes; // 已经找到 target，无需递归右子树
        }
        return getTargetCopy(original.right, cloned.right, target);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    TreeNode *getTargetCopy(TreeNode *original, TreeNode *cloned, TreeNode *target) {
        if (original == nullptr || original == target) {
            return cloned;
        }
        auto left_res = getTargetCopy(original->left, cloned->left, target);
        if (left_res) {
            return left_res; // 已经找到 target，无需递归右子树
        }
        return getTargetCopy(original->right, cloned->right, target);
    }
};
```

```js [sol-JavaScript]
var getTargetCopy = function(original, cloned, target) {
    if (original === null || original === target) {
        return cloned;
    }
    return getTargetCopy(original.left, cloned.left, target) ??
           getTargetCopy(original.right, cloned.right, target);
};
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为二叉树的节点个数。
- 空间复杂度：$\mathcal{O}(n)$。最坏情况下，二叉树退化成一条链，递归需要 $\mathcal{O}(n)$ 的栈空间。

## 分类题单

1. [滑动窗口（定长/不定长/多指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（矩形系列/字典序最小/贡献法）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/贪心/脑筋急转弯）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)

更多题单，点我个人主页 - 讨论发布。

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

[往期题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)
