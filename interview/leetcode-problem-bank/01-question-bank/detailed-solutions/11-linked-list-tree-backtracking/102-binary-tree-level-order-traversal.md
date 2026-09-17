# 102. 二叉树的层序遍历

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/binary-tree-level-order-traversal/
- 题目 slug：`binary-tree-level-order-traversal`
- 来源专题：链表、树与回溯
- 来源分类路径：二、二叉树 / §2.13 二叉树 BFS
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/binary-tree-level-order-traversal/solutions/2049807/bfs-wei-shi-yao-yao-yong-dui-lie-yi-ge-s-xlpz/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[BFS 为什么要用队列？一个视频讲透！（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/binary-tree-level-order-traversal/solutions/2049807/bfs-wei-shi-yao-yao-yong-dui-lie-yi-ge-s-xlpz/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`bfs-wei-shi-yao-yao-yong-dui-lie-yi-ge-s-xlpz`
- topic id：`2049807`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:39:45 +0800

请看视频讲解[【基础算法精讲 13】](https://www.bilibili.com/video/BV1hG4y1277i/)，制作不易，欢迎点赞~

## 方法一：两个数组

```py [sol-Python3]
class Solution:
    def levelOrder(self, root: Optional[TreeNode]) -> List[List[int]]:
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
        return ans
```

```java [sol-Java]
class Solution {
    public List<List<Integer>> levelOrder(TreeNode root) {
        if (root == null) {
            return List.of();
        }
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
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<vector<int>> levelOrder(TreeNode* root) {
        if (root == nullptr) {
            return {};
        }
        vector<vector<int>> ans;
        vector<TreeNode*> cur = {root};
        while (cur.size()) {
            vector<TreeNode*> nxt;
            vector<int> vals;
            for (auto node : cur) {
                vals.push_back(node->val);
                if (node->left)  nxt.push_back(node->left);
                if (node->right) nxt.push_back(node->right);
            }
            cur = move(nxt);
            ans.push_back(move(vals));
        }
        return ans;
    }
};
```

```go [sol-Go]
func levelOrder(root *TreeNode) (ans [][]int) {
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
    return
}
```

```js [sol-JavaScript]
var levelOrder = function(root) {
    if (root === null) {
        return [];
    }
    const ans = [];
    let cur = [root];
    while (cur.length) {
        const nxt = [];
        const vals = [];
        for (const node of cur) {
            vals.push(node.val);
            if (node.left)  nxt.push(node.left);
            if (node.right) nxt.push(node.right);
        }
        cur = nxt;
        ans.push(vals);
    }
    return ans;
};
```

```rust [sol-Rust]
use std::rc::Rc;
use std::cell::RefCell;

impl Solution {
    pub fn level_order(root: Option<Rc<RefCell<TreeNode>>>) -> Vec<Vec<i32>> {
        let mut ans = vec![];
        let mut cur = vec![];
        if let Some(x) = root {
            cur.push(x);
        }
        while !cur.is_empty() {
            let mut nxt = vec![];
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
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为二叉树的节点个数。虽然写了个二重循环，但每个节点只会添加到列表中一次，所以总的循环次数是节点个数之和，即 $\mathcal{O}(n)$。
- 空间复杂度：$\mathcal{O}(n)$。满二叉树（每一层都填满）最后一层有大约 $n/2$ 个节点，因此数组中最多有 $\mathcal{O}(n)$ 个元素，所以空间复杂度是 $\mathcal{O}(n)$ 的。

## 方法二：一个队列

```py [sol-Python3]
class Solution:
    def levelOrder(self, root: Optional[TreeNode]) -> List[List[int]]:
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
        return ans
```

```java [sol-Java]
class Solution {
    public List<List<Integer>> levelOrder(TreeNode root) {
        if (root == null) {
            return List.of();
        }
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
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<vector<int>> levelOrder(TreeNode* root) {
        if (root == nullptr) {
            return {};
        }
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
            ans.push_back(move(vals));
        }
        return ans;
    }
};
```

```go [sol-Go]
func levelOrder(root *TreeNode) (ans [][]int) {
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
    return
}
```

```js [sol-JavaScript]
var levelOrder = function(root) {
    if (root === null) {
        return [];
    }
    const ans = [];
    const q = new Queue(); // 力扣支持 datastructures-js
    q.enqueue(root);
    while (!q.isEmpty()) {
        const vals = [];
        let n = q.size();
        while (n--) {
            const node = q.dequeue();
            vals.push(node.val);
            if (node.left) {
                q.enqueue(node.left);
            }
            if (node.right) {
                q.enqueue(node.right);
            }
        }
        ans.push(vals);
    }
    return ans;
};
```

```rust [sol-Rust]
use std::rc::Rc;
use std::cell::RefCell;
use std::collections::VecDeque;

impl Solution {
    pub fn level_order(root: Option<Rc<RefCell<TreeNode>>>) -> Vec<Vec<i32>> {
        let mut ans = vec![];
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
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为二叉树的节点个数。虽然写了个二重循环，但每个节点只会入队出队各一次，所以总的循环次数是节点个数之和，即 $\mathcal{O}(n)$。
- 空间复杂度：$\mathcal{O}(n)$。满二叉树（每一层都填满）最后一层有大约 $n/2$ 个节点，因此队列中最多有 $\mathcal{O}(n)$ 个元素，所以空间复杂度是 $\mathcal{O}(n)$ 的。

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
11. [链表、树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `二、二叉树 / §2.13 二叉树 BFS`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

### 2. 暴力思路与瓶颈

直接枚举所有候选并逐个重新计算属性，通常会产生 $O(nk)$、$O(n^2)$ 或更高复杂度。瓶颈在于相邻候选之间有大量重复计算。

### 3. 关键观察

相邻状态通常只差少量元素或一个转移边界。只要把重复计算沉淀为可增量维护的统计量、单调结构、状态转移或图搜索标记，就能显著降低复杂度。

### 4. 算法设计

1. 根据题目约束确定窗口、前缀、二分、栈、图搜索、动态规划或数学变换的核心状态。
2. 初始化边界状态。
3. 按来源分类的套路推进枚举或转移，并在状态合法时更新答案。
4. 对边界不足、空状态、重复元素、负数、溢出、取模和不可达状态单独处理。

### 5. 正确性说明

枚举或转移过程覆盖所有合法候选；维护量在每一步与当前候选状态保持一致；答案只在候选合法或状态最优性成立时更新，因此最终结果等于所有合法候选的最优值、计数或可行性判断。

### 6. 复杂度分析

- 时间复杂度：依据具体题解正文确认；常见为 $O(n)$、$O(n\log n)$、$O(nm)$ 或状态数乘转移数。
- 空间复杂度：依据维护状态确认；常见为 $O(1)$、$O(k)$、$O(n)$ 或 DP/图状态规模。

### 7. C++17 实现

```cpp
#include <bits/stdc++.h>
using namespace std;

class Solution {
public:
    // TODO: 根据题目签名补全。当前批次先建立详解结构，代码需按题面签名复核。
};
```

### 8. 样例推演

当前本地层不复制题面样例。导入授权题解正文后，应结合正文中的示例或手工构造小样例，列出状态变化和答案更新时机。

### 9. 易错点

- 更新答案前必须确认当前状态已经合法。
- 删除、回退或转移状态时不要漏更新计数、和、频率表、访问标记或单调结构。
- 若题目含负数、重复值、空集合、取模、长整型溢出或特殊图结构，需单独核对边界。

### 10. 扩展解析

同一分类下的题目通常共享维护框架，差异主要在状态定义和合法性条件。复盘时应总结“状态是什么、何时合法、如何转移、答案如何更新”。

### 11. 同类题迁移

回到来源分类 `二、二叉树 / §2.13 二叉树 BFS`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
