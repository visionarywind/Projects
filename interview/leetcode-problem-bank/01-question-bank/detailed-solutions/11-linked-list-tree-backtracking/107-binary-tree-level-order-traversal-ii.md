# 107. 二叉树的层序遍历 II

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/binary-tree-level-order-traversal-ii/
- 题目 slug：`binary-tree-level-order-traversal-ii`
- 来源专题：链表、树与回溯
- 来源分类路径：二、二叉树 / §2.13 二叉树 BFS
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/binary-tree-level-order-traversal-ii/solutions/2640451/bfs-wei-shi-yao-yao-yong-dui-lie-yi-ge-s-wmul/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[BFS 为什么要用队列？一个视频讲透！（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/binary-tree-level-order-traversal-ii/solutions/2640451/bfs-wei-shi-yao-yao-yong-dui-lie-yi-ge-s-wmul/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`bfs-wei-shi-yao-yao-yong-dui-lie-yi-ge-s-wmul`
- topic id：`2640451`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:39:45 +0800

请看视频讲解[【基础算法精讲 13】](https://www.bilibili.com/video/BV1hG4y1277i/)，制作不易，欢迎点赞~

本题只需要在 [102. 二叉树的层序遍历](https://leetcode.cn/problems/binary-tree-level-order-traversal/) 的基础上，把答案反转即可。

## 方法一：两个数组

```py [sol-Python3]
class Solution:
    def levelOrderBottom(self, root: Optional[TreeNode]) -> List[List[int]]:
        if root is None:
            return []
        ans = []
        cur = [root]
        while cur:
            nxt = []
            vals = []
            for node in cur:
                vals.append(node.val)
                if node.left:  nxt.append(node.left)
                if node.right: nxt.append(node.right)
            cur = nxt
            ans.append(vals)
        return ans[::-1]
```

```java [sol-Java]
class Solution {
    public List<List<Integer>> levelOrderBottom(TreeNode root) {
        if (root == null) return List.of();
        List<List<Integer>> ans = new ArrayList<>();
        List<TreeNode> cur = List.of(root);
        while (!cur.isEmpty()) {
            List<TreeNode> nxt = new ArrayList<>();
            List<Integer> vals = new ArrayList<>(cur.size()); // 预分配空间
            for (TreeNode node : cur) {
                vals.add(node.val);
                if (node.left != null)  nxt.add(node.left);
                if (node.right != null) nxt.add(node.right);
            }
            cur = nxt;
            ans.add(vals);
        }
        Collections.reverse(ans);
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<vector<int>> levelOrderBottom(TreeNode *root) {
        if (root == nullptr) return {};
        vector<vector<int>> ans;
        vector<TreeNode*> cur{root};
        while (cur.size()) {
            vector<TreeNode*> nxt;
            vector<int> vals;
            for (auto node : cur) {
                vals.push_back(node->val);
                if (node->left)  nxt.push_back(node->left);
                if (node->right) nxt.push_back(node->right);
            }
            cur = move(nxt);
            ans.emplace_back(vals);
        }
        ranges::reverse(ans);
        return ans;
    }
};
```

```go [sol-Go]
func levelOrderBottom(root *TreeNode) (ans [][]int) {
    if root == nil {
        return
    }
    cur := []*TreeNode{root}
    for len(cur) > 0 {
        nxt := []*TreeNode{}
        vals := make([]int, len(cur)) // 预分配空间
        for i, node := range cur {
            vals[i] = node.Val
            if node.Left != nil {
                nxt = append(nxt, node.Left)
            }
            if node.Right != nil {
                nxt = append(nxt, node.Right)
            }
        }
        cur = nxt
        ans = append(ans, vals)
    }
    slices.Reverse(ans)
    return
}
```

```js [sol-JavaScript]
var levelOrderBottom = function(root) {
    if (root === null) return [];
    let ans = [];
    let cur = [root];
    while (cur.length) {
        let nxt = [];
        let vals = [];
        for (const node of cur) {
            vals.push(node.val);
            if (node.left)  nxt.push(node.left);
            if (node.right) nxt.push(node.right);
        }
        cur = nxt;
        ans.push(vals);
    }
    ans.reverse();
    return ans;
};
```

```rust [sol-Rust]
use std::rc::Rc;
use std::cell::RefCell;

impl Solution {
    pub fn level_order_bottom(root: Option<Rc<RefCell<TreeNode>>>) -> Vec<Vec<i32>> {
        let mut ans = Vec::new();
        let mut cur = Vec::new();
        if let Some(x) = root {
            cur.push(x);
        }
        while !cur.is_empty() {
            let mut nxt = Vec::new();
            let mut vals = Vec::with_capacity(cur.len()); // 预分配空间
            for node in cur {
                let mut x = node.borrow_mut();
                vals.push(x.val);
                if let Some(left) = x.left.take() {
                    nxt.push(left);
                }
                if let Some(right) = x.right.take() {
                    nxt.push(right);
                }
            }
            cur = nxt;
            ans.push(vals);
        }
        ans.reverse();
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为二叉树的节点个数。
- 空间复杂度：$\mathcal{O}(n)$。满二叉树（每一层都填满）最后一层有大约 $n/2$ 个节点，因此数组中最多有 $\mathcal{O}(n)$ 个元素，所以空间复杂度是 $\mathcal{O}(n)$ 的。

## 方法二：一个队列

```py [sol-Python3]
class Solution:
    def levelOrderBottom(self, root: Optional[TreeNode]) -> List[List[int]]:
        if root is None:
            return []
        ans = []
        q = deque([root])
        while q:
            vals = []
            for _ in range(len(q)):
                node = q.popleft()
                vals.append(node.val)
                if node.left:  q.append(node.left)
                if node.right: q.append(node.right)
            ans.append(vals)
        return ans[::-1]
```

```java [sol-Java]
class Solution {
    public List<List<Integer>> levelOrderBottom(TreeNode root) {
        if (root == null) return List.of();
        List<List<Integer>> ans = new ArrayList<>();
        Queue<TreeNode> q = new ArrayDeque<>();
        q.add(root);
        while (!q.isEmpty()) {
            int n = q.size();
            List<Integer> vals = new ArrayList<>(n); // 预分配空间
            while (n-- > 0) {
                TreeNode node = q.poll();
                vals.add(node.val);
                if (node.left != null)  q.add(node.left);
                if (node.right != null) q.add(node.right);
            }
            ans.add(vals);
        }
        Collections.reverse(ans);
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<vector<int>> levelOrderBottom(TreeNode *root) {
        if (root == nullptr) return {};
        vector<vector<int>> ans;
        queue<TreeNode*> q;
        q.push(root);
        while (!q.empty()) {
            vector<int> vals;
            for (int n = q.size(); n--;) {
                auto node = q.front();
                q.pop();
                vals.push_back(node->val);
                if (node->left)  q.push(node->left);
                if (node->right) q.push(node->right);
            }
            ans.emplace_back(vals);
        }
        ranges::reverse(ans);
        return ans;
    }
};
```

```go [sol-Go]
func levelOrderBottom(root *TreeNode) (ans [][]int) {
    if root == nil {
        return
    }
    q := []*TreeNode{root}
    for len(q) > 0 {
        n := len(q)
        vals := make([]int, n) // 预分配空间
        for i := range vals {
            node := q[0]
            q = q[1:]
            vals[i] = node.Val
            if node.Left != nil {
                q = append(q, node.Left)
            }
            if node.Right != nil {
                q = append(q, node.Right)
            }
        }
        ans = append(ans, vals)
    }
    slices.Reverse(ans)
    return
}
```

```rust [sol-Rust]
use std::rc::Rc;
use std::cell::RefCell;
use std::collections::VecDeque;

impl Solution {
    pub fn level_order_bottom(root: Option<Rc<RefCell<TreeNode>>>) -> Vec<Vec<i32>> {
        let mut ans = Vec::new();
        let mut q = VecDeque::new();
        if let Some(x) = root {
            q.push_back(x);
        }
        while !q.is_empty() {
            let n = q.len();
            let mut vals = Vec::with_capacity(n); // 预分配空间
            for _ in 0..n {
                if let Some(node) = q.pop_front() {
                    let mut x = node.borrow_mut();
                    vals.push(x.val);
                    if let Some(left) = x.left.take() {
                        q.push_back(left);
                    }
                    if let Some(right) = x.right.take() {
                        q.push_back(right);
                    }
                }
            }
            ans.push(vals);
        }
        ans.reverse();
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为二叉树的节点个数。
- 空间复杂度：$\mathcal{O}(n)$。满二叉树（每一层都填满）最后一层有大约 $n/2$ 个节点，因此队列中最多有 $\mathcal{O}(n)$ 个元素，所以空间复杂度是 $\mathcal{O}(n)$ 的。
