# 589. N 叉树的前序遍历

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/n-ary-tree-preorder-traversal/
- 题目 slug：`n-ary-tree-preorder-traversal`
- 来源专题：链表、树与回溯
- 来源分类路径：二、二叉树 / §2.15 N 叉树
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/n-ary-tree-preorder-traversal/solutions/2643719/jian-dan-dfspythonjavacgojsrust-by-endle-0sf4/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[简单 DFS（Python/Java/C++/Go/JS）](https://leetcode.cn/problems/n-ary-tree-preorder-traversal/solutions/2643719/jian-dan-dfspythonjavacgojsrust-by-endle-0sf4/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`jian-dan-dfspythonjavacgojsrust-by-endle-0sf4`
- topic id：`2643719`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:49:15 +0800

对于二叉树来说，前序遍历的顺序为：根节点 - 递归左儿子（左子树） - 递归右儿子（右子树）。

推广到 N 叉树，顺序是先访问根节点，再依次递归其 $\textit{children}$ 数组中的节点（子树）。

不熟悉递归的同学可以看视频讲解：[深刻理解递归【基础算法精讲 09】](https://www.bilibili.com/video/BV1UD4y1Y769/)

```py [sol-Python3]
class Solution:
    def preorder(self, root: 'Node') -> List[int]:
        ans = []
        def dfs(node: 'Node') -> None:
            if node is None:
                return
            ans.append(node.val)
            for c in node.children:
                dfs(c)
        dfs(root)
        return ans
```

```java [sol-Java]
class Solution {
    public List<Integer> preorder(Node root) {
        List<Integer> ans = new ArrayList<>();
        dfs(root, ans);
        return ans;
    }

    private void dfs(Node node, List<Integer> ans) {
        if (node == null) {
            return;
        }
        ans.add(node.val);
        for (Node c : node.children) {
            dfs(c, ans);
        }
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> preorder(Node *root) {
        vector<int> ans;
        function<void(Node*)> dfs = [&](Node *node) {
            if (node == nullptr) {
                return;
            }
            ans.push_back(node->val);
            for (auto c : node->children) {
                dfs(c);
            }
        };
        dfs(root);
        return ans;
    }
};
```

```go [sol-Go]
func preorder(root *Node) (ans []int) {
    var dfs func(*Node)
    dfs = func(node *Node) {
        if node == nil {
            return
        }
        ans = append(ans, node.Val)
        for _, c := range node.Children {
            dfs(c)
        }
    }
    dfs(root)
    return
}
```

```js [sol-JavaScript]
var preorder = function(root) {
    const ans = [];
    function dfs(node) {
        if (node === null) {
            return;
        }
        ans.push(node.val);
        for (const c of node.children) {
            dfs(c);
        }
    }
    dfs(root);
    return ans;
};
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为树的节点个数。
- 空间复杂度：$\mathcal{O}(n)$。

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

[往期题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)
