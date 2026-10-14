# 116. 填充每个节点的下一个右侧节点指针

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/populating-next-right-pointers-in-each-node/
- 题目 slug：`populating-next-right-pointers-in-each-node`
- 来源专题：链表、树与回溯
- 来源分类路径：二、二叉树 / §2.14 链表+二叉树
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/populating-next-right-pointers-in-each-node/solutions/2510369/san-chong-fang-fa-dfsbfsbfslian-biao-fu-5alnq/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[三种方法：DFS/BFS/BFS+链表（Python/Java/C++/Go/JS）](https://leetcode.cn/problems/populating-next-right-pointers-in-each-node/solutions/2510369/san-chong-fang-fa-dfsbfsbfslian-biao-fu-5alnq/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`san-chong-fang-fa-dfsbfsbfslian-biao-fu-5alnq`
- topic id：`2510369`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:49:15 +0800

**注**：本文介绍的是通用方法，除了可以解决本题，还可以解决更一般的 [117. 填充每个节点的下一个右侧节点指针 II](https://leetcode.cn/problems/populating-next-right-pointers-in-each-node-ii/)。

## 方法一：DFS

#### 前置知识

请看视频：[二叉树 DFS【基础算法精讲 09】](https://www.bilibili.com/video/BV1UD4y1Y769/)

#### 思路

![lc116.png](https://pic.leetcode.cn/1698969186-VfTugd-lc116.png)

DFS 这棵树，从根节点 $1$ 出发，向左递归到 $2$，再向左递归到 $4$。

这三个节点正好是每一层的第一个节点（类似链表头），用一个数组 $\textit{pre}$ 记录，即 $\textit{pre}[0]$ 为节点 $1$，$\textit{pre}[1]$ 为节点 $2$，$\textit{pre}[2]$ 为节点 $4$。$\textit{pre}$ 的下标就是节点的**深度**。

继续递归到 $5$（深度为 $2$），从 $\textit{pre}[2]$ 中拿到节点 $4$，把 $4$ 的 $\textit{next}$ 指向 $5$。然后更新 $\textit{pre}[2]$ 为节点 $5$，这样在后面递归到节点 $6$ 时，就可以从 $\textit{pre}[2]$ 中拿到节点 $5$，把 $5$ 的 $\textit{next}$ 指向 $6$ 了。

#### 算法

1. 创建一个空数组 $\textit{pre}$（因为一开始不知道二叉树有多深）。
2. DFS 这棵二叉树，递归参数为当前节点 $\textit{node}$，以及当前节点的深度 $\textit{depth}$。每往下递归一层，就把 $\textit{depth}$ 加一。
3. 如果 $\textit{depth}$ 等于 $\textit{pre}$ 数组的长度，说明 $\textit{node}$ 是这一层最左边的节点，把 $\textit{node}$ 添加到 $\textit{pre}$ 的末尾。
4. 否则，把 $\textit{pre}[\textit{depth}]$ 的 $\textit{next}$ 指向 $\textit{node}$，然后更新 $\textit{pre}[\textit{depth}]$ 为 $\textit{node}$。
5. 递归边界：如果 $\textit{node}$ 是空节点，直接返回。
6. 递归入口：$\textit{dfs}(\textit{root},0)$。
7. 最后返回 $\textit{root}$。

```py [sol-Python3]
class Solution:
    def connect(self, root: 'Node') -> 'Node':
        pre = []
        def dfs(node: 'Node', depth: int) -> None:
            if node is None:
                return
            if depth == len(pre):  # node 是这一层最左边的节点
                pre.append(node)
            else:  # pre[depth] 是 node 左边的节点
                pre[depth].next = node  # node 左边的节点指向 node
                pre[depth] = node
            dfs(node.left, depth + 1)
            dfs(node.right, depth + 1)
        dfs(root, 0)  # 根节点的深度为 0
        return root
```

```java [sol-Java]
class Solution {
    private final List<Node> pre = new ArrayList<>();

    public Node connect(Node root) {
        dfs(root, 0); // 根节点的深度为 0
        return root;
    }

    private void dfs(Node node, int depth) {
        if (node == null) {
            return;
        }
        if (depth == pre.size()) { // node 是这一层最左边的节点
            pre.add(node);
        } else { // pre[depth] 是 node 左边的节点
            pre.get(depth).next = node; // node 左边的节点指向 node
            pre.set(depth, node);
        }
        dfs(node.left, depth + 1);
        dfs(node.right, depth + 1);
    }
}
```

```cpp [sol-C++]
class Solution {
    vector<Node*> pre;
public:
    Node* connect(Node* root) {
        dfs(root, 0); // 根节点的深度为 0
        return root;
    }

    void dfs(Node* node, int depth) {
        if (node == nullptr) {
            return;
        }
        if (depth == pre.size()) { // node 是这一层最左边的节点
            pre.push_back(node);
        } else { // pre[depth] 是 node 左边的节点
            pre[depth]->next = node; // node 左边的节点指向 node
            pre[depth] = node;
        }
        dfs(node->left, depth + 1);
        dfs(node->right, depth + 1);
    }
};
```

```go [sol-Go]
func connect(root *Node) *Node {
    pre := []*Node{}
    var dfs func(*Node, int)
    dfs = func(node *Node, depth int) {
        if node == nil {
            return
        }
        if depth == len(pre) { // node 是这一层最左边的节点
            pre = append(pre, node)
        } else { // pre[depth] 是 node 左边的节点
            pre[depth].Next = node // node 左边的节点指向 node
            pre[depth] = node
        }
        dfs(node.Left, depth+1)
        dfs(node.Right, depth+1)
    }
    dfs(root, 0) // 根节点的深度为 0
    return root
}
```

```js [sol-JavaScript]
var connect = function(root) {
    const pre = [];
    function dfs(node, depth) {
        if (node === null) {
            return;
        }
        if (depth === pre.length) { // node 是这一层最左边的节点
            pre.push(node);
        } else { // pre[depth] 是 node 左边的节点
            pre[depth].next = node; // node 左边的节点指向 node
            pre[depth] = node;
        }
        dfs(node.left, depth + 1);
        dfs(node.right, depth + 1);
    }
    dfs(root, 0); // 根节点的深度为 0
    return root;
};
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为二叉树的节点个数。
- 空间复杂度：$\mathcal{O}(h)$，其中 $h$ 为二叉树的高度。由于本题是完美二叉树（满二叉树），所以 $h=\mathcal{O}(\log n)$。

## 方法二：BFS

#### 前置知识

请看视频：[二叉树 BFS【基础算法精讲 13】](https://www.bilibili.com/video/BV1hG4y1277i/)

#### 思路

如果 BFS 这棵二叉树，做法就更加简单了：只需要把每一层的节点，从左到右依次用 $\textit{next}$ 指针连接起来。

注意特判 $\textit{root}$ 为空的情况。

```py [sol-Python3]
class Solution:
    def connect(self, root: 'Node') -> 'Node':
        if root is None:
            return None
        q = [root]
        while q:
            # 从左到右依次连接
            for x, y in pairwise(q):
                x.next = y
            # 准备下一层的节点
            tmp = q
            q = []
            for node in tmp:
                if node.left:  q.append(node.left)
                if node.right: q.append(node.right)
        return root
```

```java [sol-Java]
class Solution {
    public Node connect(Node root) {
        if (root == null) {
            return null;
        }
        List<Node> q = List.of(root);
        while (!q.isEmpty()) {
            List<Node> tmp = q;
            q = new ArrayList<>();
            for (int i = 0; i < tmp.size(); i++) {
                Node node = tmp.get(i);
                if (i > 0) { // 连接同一层的两个相邻节点
                    tmp.get(i - 1).next = node;
                }
                if (node.left != null) {
                    q.add(node.left);
                }
                if (node.right != null) {
                    q.add(node.right);
                }
            }
        }
        return root;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    Node* connect(Node* root) {
        if (root == nullptr) {
            return nullptr;
        }
        vector<Node*> q = {root};
        while (!q.empty()) {
            vector<Node*> nxt;
            for (int i = 0; i < q.size(); i++) {
                Node* node = q[i];
                if (i) { // 连接同一层的两个相邻节点
                    q[i - 1]->next = node;
                }
                if (node->left) {
                    nxt.push_back(node->left);
                }
                if (node->right) {
                    nxt.push_back(node->right);
                }
            }
            q = move(nxt);
        }
        return root;
    }
};
```

```go [sol-Go]
func connect(root *Node) *Node {
    if root == nil {
        return nil
    }
    q := []*Node{root}
    for q != nil {
        tmp := q
        q = nil
        for i, node := range tmp {
            if i > 0 { // 连接同一层的两个相邻节点
                tmp[i-1].Next = node
            }
            if node.Left != nil {
                q = append(q, node.Left)
            }
            if node.Right != nil {
                q = append(q, node.Right)
            }
        }
    }
    return root
}
```

```js [sol-JavaScript]
var connect = function(root) {
    if (root === null) {
        return null;
    }
    let q = [root];
    while (q.length) {
        const tmp = q;
        q = [];
        for (let i = 0; i < tmp.length; i++) {
            const node = tmp[i];
            if (i) { // 连接同一层的两个相邻节点
                tmp[i - 1].next = node;
            }
            if (node.left) {
                q.push(node.left);
            }
            if (node.right) {
                q.push(node.right);
            }
        }
    }
    return root;
};
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为二叉树的节点个数。
- 空间复杂度：$\mathcal{O}(n)$。

## 方法三：BFS+链表

#### 思路

既然每一层都连接成一个链表了，那么知道链表头，就能访问这一层的所有节点。

所以在 BFS 的时候，可以一边遍历当前层的节点，一边把下一层的节点连接起来。这样就无需存储下一层的节点了，只需要拿到下一层链表的头节点。

#### 算法

1. 从第一层开始（第一层只有一个 $\textit{root}$ 节点），每次循环：
2. 遍历当前层的链表节点，通过节点的 $\textit{left}$ 和 $\textit{right}$ 得到下一层的节点。
3. 把下一层的节点从左到右连接成一个链表。
4. 拿到下一层链表的头节点，进入下一轮循环。

代码实现时，可以用一个**哨兵节点**来表示「第一个节点之前的节点」，从而减少一些关于空节点的判断逻辑。具体请看[【基础算法精讲 06】](https://www.bilibili.com/video/BV1sd4y1x7KN/)

```py [sol-Python3]
class Solution:
    def connect(self, root: 'Node') -> 'Node':
        cur = root
        while cur:
            nxt = dummy = Node()  # 下一层的链表
            while cur:  # 遍历当前层的链表
                if cur.left:
                    nxt.next = cur.left  # 下一层的相邻节点连起来
                    nxt = cur.left
                if cur.right:
                    nxt.next = cur.right  # 下一层的相邻节点连起来
                    nxt = cur.right
                cur = cur.next  # 当前层链表的下一个节点
            cur = dummy.next  # 下一层链表的头节点
        return root
```

```java [sol-Java]
class Solution {
    public Node connect(Node root) {
        Node dummy = new Node();
        Node cur = root;
        while (cur != null) {
            dummy.next = null;
            Node nxt = dummy; // 下一层的链表
            while (cur != null) { // 遍历当前层的链表
                if (cur.left != null) {
                    nxt.next = cur.left; // 下一层的相邻节点连起来
                    nxt = cur.left;
                }
                if (cur.right != null) {
                    nxt.next = cur.right; // 下一层的相邻节点连起来
                    nxt = cur.right;
                }
                cur = cur.next; // 当前层链表的下一个节点
            }
            cur = dummy.next; // 下一层链表的头节点
        }
        return root;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    Node* connect(Node* root) {
        Node dummy;
        Node* cur = root;
        while (cur) {
            dummy.next = nullptr;
            Node* nxt = &dummy; // 下一层的链表
            while (cur) { // 遍历当前层的链表
                if (cur->left) {
                    nxt->next = cur->left; // 下一层的相邻节点连起来
                    nxt = cur->left;
                }
                if (cur->right) {
                    nxt->next = cur->right; // 下一层的相邻节点连起来
                    nxt = cur->right;
                }
                cur = cur->next; // 当前层链表的下一个节点
            }
            cur = dummy.next; // 下一层链表的头节点
        }
        return root;
    }
};
```

```go [sol-Go]
func connect(root *Node) *Node {
    dummy := &Node{}
    cur := root
    for cur != nil {
        dummy.Next = nil
        nxt := dummy // 下一层的链表
        for cur != nil { // 遍历当前层的链表
            if cur.Left != nil {
                nxt.Next = cur.Left // 下一层的相邻节点连起来
                nxt = cur.Left
            }
            if cur.Right != nil {
                nxt.Next = cur.Right // 下一层的相邻节点连起来
                nxt = cur.Right
            }
            cur = cur.Next // 当前层链表的下一个节点
        }
        cur = dummy.Next // 下一层链表的头节点
    }
    return root
}
```

```js [sol-JavaScript]
var connect = function(root) {
    const dummy = new Node();
    let cur = root;
    while (cur) {
        dummy.next = null;
        let nxt = dummy; // 下一层的链表
        while (cur) { // 遍历当前层的链表
            if (cur.left) {
                nxt.next = cur.left; // 下一层的相邻节点连起来
                nxt = cur.left;
            }
            if (cur.right) {
                nxt.next = cur.right; // 下一层的相邻节点连起来
                nxt = cur.right;
            }
            cur = cur.next; // 当前层链表的下一个节点
        }
        cur = dummy.next; // 下一层链表的头节点
    }
    return root;
};
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为二叉树的节点个数。
- 空间复杂度：$\mathcal{O}(1)$。只用到若干额外变量。

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
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)
