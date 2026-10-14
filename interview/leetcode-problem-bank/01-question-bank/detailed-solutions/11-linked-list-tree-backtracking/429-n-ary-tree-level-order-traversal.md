# 429. N 叉树的层序遍历

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/n-ary-tree-level-order-traversal/
- 题目 slug：`n-ary-tree-level-order-traversal`
- 来源专题：链表、树与回溯
- 来源分类路径：二、二叉树 / §2.15 N 叉树
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/n-ary-tree-level-order-traversal/solutions/2642410/liang-chong-bfs-xie-fa-shuang-shu-zu-dui-a4hd/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种 BFS 写法：双数组/队列（Python/Java/C++/Go/JS）](https://leetcode.cn/problems/n-ary-tree-level-order-traversal/solutions/2642410/liang-chong-bfs-xie-fa-shuang-shu-zu-dui-a4hd/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`liang-chong-bfs-xie-fa-shuang-shu-zu-dui-a4hd`
- topic id：`2642410`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:49:15 +0800

请看视频讲解[【基础算法精讲 13】](https://www.bilibili.com/video/BV1hG4y1277i/)，制作不易，欢迎点赞~

本题只需要在 [102. 二叉树的层序遍历](https://leetcode.cn/problems/binary-tree-level-order-traversal/) 的基础上，把访问左右子树改成遍历 $\textit{children}$ 列表。

## 方法一：两个数组

```py [sol-Python3]
class Solution:
    def levelOrder(self, root: 'Node') -> List[List[int]]:
        if root is None:
            return []
        ans = []
        q = [root]
        while q:
            ans.append([node.val for node in q])
            q = [c for node in q for c in node.children]
        return ans
```

```java [sol-Java]
class Solution {
    public List<List<Integer>> levelOrder(Node root) {
        if (root == null) return List.of();
        List<List<Integer>> ans = new ArrayList<>();
        List<Node> cur = List.of(root);
        while (!cur.isEmpty()) {
            List<Node> nxt = new ArrayList<>();
            List<Integer> vals = new ArrayList<>(cur.size()); // 预分配空间
            for (Node node : cur) {
                vals.add(node.val);
                nxt.addAll(node.children);
            }
            ans.add(vals);
            cur = nxt;
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<vector<int>> levelOrder(Node *root) {
        if (root == nullptr) return {};
        vector<vector<int>> ans;
        vector<Node*> cur = {root};
        while (cur.size()) {
            vector<Node*> nxt;
            vector<int> vals;
            for (auto node : cur) {
                vals.push_back(node->val);
                nxt.insert(nxt.end(), node->children.begin(), node->children.end());
            }
            ans.emplace_back(vals);
            cur = move(nxt);
        }
        return ans;
    }
};
```

```go [sol-Go]
func levelOrder(root *Node) (ans [][]int) {
    if root == nil {
        return
    }
    cur := []*Node{root}
    for len(cur) > 0 {
        nxt := []*Node{}
        vals := make([]int, len(cur)) // 预分配空间
        for i, node := range cur {
            vals[i] = node.Val
            nxt = append(nxt, node.Children...)
        }
        ans = append(ans, vals)
        cur = nxt
    }
    return
}
```

```js [sol-JavaScript]
var levelOrder = function(root) {
    if (root === null) return [];
    let ans = [];
    let cur = [root];
    while (cur.length) {
        let nxt = [];
        let vals = [];
        for (const node of cur) {
            vals.push(node.val);
            nxt.push(...node.children);
        }
        ans.push(vals);
        cur = nxt;
    }
    return ans;
};
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 N 叉树的节点个数。
- 空间复杂度：$\mathcal{O}(n)$。最坏情况下，$\textit{root}$ 有 $n-1$ 个子节点。

## 方法二：一个队列

```py [sol-Python3]
class Solution:
    def levelOrder(self, root: 'Node') -> List[List[int]]:
        if root is None:
            return []
        ans = []
        q = deque([root])
        while q:
            vals = []
            for _ in range(len(q)):
                node = q.popleft()
                vals.append(node.val)
                q.extend(node.children)
            ans.append(vals)
        return ans
```

```java [sol-Java]
class Solution {
    public List<List<Integer>> levelOrder(Node root) {
        if (root == null) return List.of();
        List<List<Integer>> ans = new ArrayList<>();
        Queue<Node> q = new ArrayDeque<>();
        q.add(root);
        while (!q.isEmpty()) {
            int n = q.size();
            List<Integer> vals = new ArrayList<>(n); // 预分配空间
            while (n-- > 0) {
                Node node = q.poll();
                vals.add(node.val);
                q.addAll(node.children);
            }
            ans.add(vals);
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<vector<int>> levelOrder(Node *root) {
        if (root == nullptr) return {};
        vector<vector<int>> ans;
        queue<Node*> q;
        q.push(root);
        while (!q.empty()) {
            vector<int> vals;
            for (int n = q.size(); n--;) {
                auto node = q.front();
                q.pop();
                vals.push_back(node->val);
                for (auto c : node->children) {
                    q.push(c);
                }
            }
            ans.emplace_back(vals);
        }
        return ans;
    }
};
```

```go [sol-Go]
func levelOrder(root *Node) (ans [][]int) {
    if root == nil {
        return
    }
    q := []*Node{root}
    for len(q) > 0 {
        n := len(q)
        vals := make([]int, n) // 预分配空间
        for i := range vals {
            node := q[0]
            q = q[1:]
            vals[i] = node.Val
            q = append(q, node.Children...)
        }
        ans = append(ans, vals)
    }
    return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 N 叉树的节点个数。
- 空间复杂度：$\mathcal{O}(n)$。最坏情况下，$\textit{root}$ 有 $n-1$ 个子节点。
