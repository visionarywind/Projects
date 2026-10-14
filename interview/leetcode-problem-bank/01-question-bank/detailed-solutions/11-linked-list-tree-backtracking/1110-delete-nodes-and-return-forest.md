# 1110. 删点成林

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/delete-nodes-and-return-forest/
- 题目 slug：`delete-nodes-and-return-forest`
- 来源专题：链表、树与回溯
- 来源分类路径：二、二叉树 / §2.4 自底向上 DFS：删点
- 难度分：1511
- 外部题解来源：https://leetcode.cn/problems/delete-nodes-and-return-forest/solutions/2289131/he-shi-ji-lu-da-an-pythonjavacgo-by-endl-lpcd/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[简洁写法！（Python/Java/C++/Go）](https://leetcode.cn/problems/delete-nodes-and-return-forest/solutions/2289131/he-shi-ji-lu-da-an-pythonjavacgo-by-endl-lpcd/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`he-shi-ji-lu-da-an-pythonjavacgo-by-endl-lpcd`
- topic id：`2289131`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:39:45 +0800

## 一、思考

如何快速判断节点值是否在 $\textit{toDelete}$ 中？

如果当前节点被删除，但是左儿子（右儿子）没被删除，意味着什么？

## 二、解惑

把 $\textit{toDelete}$ 全部丢到一个哈希表 $s$ 中，这样可以 $\mathcal{O}(1)$ 判断节点值是否在 $\textit{toDelete}$ 中。

如果当前节点被删除，但是左儿子（右儿子）没被删除，意味着左儿子（右儿子）是一棵树的根节点，加入答案。

## 三、算法

写一个 DFS（后序遍历）：

1. 更新左儿子（右儿子）为递归左儿子（右儿子）的返回值。
2. 如果当前节点被删除，那么就检查左儿子（右儿子）是否被删除，如果没被删除，就加入答案。
3. 如果当前节点被删除，返回空节点，否则返回当前节点。
4. 最后，如果根节点没被删除，把根节点加入答案。

晕递归的同学，可以看视频讲解[【基础算法精讲 09】](https://www.bilibili.com/video/BV1UD4y1Y769/)，制作不易，欢迎点赞~

```py [sol-Python3]
class Solution:
    def delNodes(self, root: Optional[TreeNode], to_delete: List[int]) -> List[TreeNode]:
        ans = []
        s = set(to_delete)
        def dfs(node: Optional[TreeNode]) -> Optional[TreeNode]:
            if node is None: return None
            node.left = dfs(node.left)
            node.right = dfs(node.right)
            if node.val not in s: return node
            if node.left: ans.append(node.left)
            if node.right: ans.append(node.right)
            return None
        if dfs(root): ans.append(root)
        return ans
```

```java [sol-Java]
class Solution {
    public List<TreeNode> delNodes(TreeNode root, int[] toDelete) {
        List<TreeNode> ans = new ArrayList<TreeNode>();
        Set<Integer> s = new HashSet<Integer>();
        for (int x : toDelete) s.add(x);
        if (dfs(ans, s, root) != null) ans.add(root);
        return ans;
    }

    private TreeNode dfs(List<TreeNode> ans, Set<Integer> s, TreeNode node) {
        if (node == null) return null;
        node.left = dfs(ans, s, node.left);
        node.right = dfs(ans, s, node.right);
        if (!s.contains(node.val)) return node;
        if (node.left != null) ans.add(node.left);
        if (node.right != null) ans.add(node.right);
        return null;
    }
}
```

```cpp [sol-C++]
class Solution {
    vector<TreeNode*> ans;
    unordered_set<int> s;

    TreeNode* dfs(TreeNode *node) {
        if (node == nullptr) return nullptr;
        node->left = dfs(node->left);
        node->right = dfs(node->right);
        if (!s.count(node->val)) return node;
        if (node->left) ans.push_back(node->left);
        if (node->right) ans.push_back(node->right);
        return nullptr;
    }

public:
    vector<TreeNode*> delNodes(TreeNode *root, vector<int> &to_delete) {
        for (int x : to_delete) s.insert(x);
        if (dfs(root)) ans.push_back(root);
        return ans;
    }
};
```

```go [sol-Go]
func delNodes(root *TreeNode, toDelete []int) (ans []*TreeNode) {
    set := make(map[int]struct{}, len(toDelete))
    for _, x := range toDelete {
        set[x] = struct{}{}
    }
    var dfs func(*TreeNode) *TreeNode
    dfs = func(node *TreeNode) *TreeNode {
        if node == nil {
            return nil
        }
        node.Left = dfs(node.Left)
        node.Right = dfs(node.Right)
        if _, ok := set[node.Val]; !ok {
            return node
        }
        if node.Left != nil {
            ans = append(ans, node.Left)
        }
        if node.Right != nil {
            ans = append(ans, node.Right)
        }
        return nil
    }
    if dfs(root) != nil {
        ans = append(ans, root)
    }
    return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n+m)$，其中 $n$ 为二叉树的节点个数，$m$ 为 $\textit{toDelete}$ 的长度。每个节点都会递归恰好一次。
- 空间复杂度：$\mathcal{O}(n+m)$。最坏情况下，二叉树是一条链，递归需要 $\mathcal{O}(n)$ 的栈空间。

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

更多精彩题解，请看 [往期题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)
