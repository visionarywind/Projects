# 199. 二叉树的右视图

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/binary-tree-right-side-view/
- 题目 slug：`binary-tree-right-side-view`
- 来源专题：链表、树与回溯
- 来源分类路径：二、二叉树 / §2.2 自顶向下 DFS（先序遍历）
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/binary-tree-right-side-view/solutions/2015061/ru-he-ling-huo-yun-yong-di-gui-lai-kan-s-r1nc/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：BFS / DFS（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/binary-tree-right-side-view/solutions/2015061/ru-he-ling-huo-yun-yong-di-gui-lai-kan-s-r1nc/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`ru-he-ling-huo-yun-yong-di-gui-lai-kan-s-r1nc`
- topic id：`2015061`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:30:25 +0800

## 方法一：BFS

**前置题目**：[102. 二叉树的层序遍历](https://leetcode.cn/problems/binary-tree-level-order-traversal/)，[视频讲解【基础算法精讲 13】](https://www.bilibili.com/video/BV1hG4y1277i/)。

套用 102 题的 BFS 模板，把每一层的最后一个节点值保存到答案中。

```py [sol-Python3]
class Solution:
    def rightSideView(self, root: Optional[TreeNode]) -> List[int]:
        if root is None:
            return []
        ans = []
        cur = [root]
        while cur:
            ans.append(cur[-1].val)
            nxt = []
            for node in cur:
                if node.left:  nxt.append(node.left)
                if node.right: nxt.append(node.right)
            cur = nxt
        return ans
```

```java [sol-Java]
class Solution {
    public List<Integer> rightSideView(TreeNode root) {
        if (root == null) {
            return List.of();
        }
        List<Integer> ans = new ArrayList<>();
        List<TreeNode> cur = List.of(root);
        while (!cur.isEmpty()) {
            ans.add(cur.getLast().val);
            List<TreeNode> nxt = new ArrayList<>();
            for (TreeNode node : cur) {
                if (node.left != null)  nxt.add(node.left);
                if (node.right != null) nxt.add(node.right);
            }
            cur = nxt;
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> rightSideView(TreeNode* root) {
        if (root == nullptr) {
            return {};
        }
        vector<int> ans;
        vector<TreeNode*> cur = {root};
        while (cur.size()) {
            ans.push_back(cur.back()->val);
            vector<TreeNode*> nxt;
            for (auto node : cur) {
                if (node->left)  nxt.push_back(node->left);
                if (node->right) nxt.push_back(node->right);
            }
            cur = move(nxt);
        }
        return ans;
    }
};
```

```go [sol-Go]
func rightSideView(root *TreeNode) (ans []int) {
	if root == nil {
		return
	}
	cur := []*TreeNode{root}
	for len(cur) > 0 {
		ans = append(ans, cur[len(cur)-1].Val)
		nxt := []*TreeNode{}
		for _, node := range cur {
			if node.Left != nil {
				nxt = append(nxt, node.Left)
			}
			if node.Right != nil {
				nxt = append(nxt, node.Right)
			}
		}
		cur = nxt
	}
	return
}
```

```js [sol-JavaScript]
var rightSideView = function(root) {
    if (root === null) {
        return [];
    }
    const ans = [];
    let cur = [root];
    while (cur.length) {
        ans.push(cur[cur.length - 1].val);
        const nxt = [];
        for (const node of cur) {
            if (node.left)  nxt.push(node.left);
            if (node.right) nxt.push(node.right);
        }
        cur = nxt;
    }
    return ans;
};
```

```rust [sol-Rust]
use std::rc::Rc;
use std::cell::RefCell;

impl Solution {
    pub fn right_side_view(root: Option<Rc<RefCell<TreeNode>>>) -> Vec<i32> {
        let mut ans = vec![];
        let mut cur = vec![];
        if let Some(x) = root {
            cur.push(x);
        }
        while !cur.is_empty() {
            ans.push(cur.last().unwrap().borrow().val);
            let mut nxt = vec![];
            for node in cur {
                let mut x = node.borrow_mut();
                if let Some(left) = x.left.take() {
                    nxt.push(left);
                }
                if let Some(right) = x.right.take() {
                    nxt.push(right);
                }
            }
            cur = nxt;
        }
        ans
    }
}
```

> **注**：也可以先把右儿子加入节点数组，这样 $\textit{cur}[0]$ 就是每层最右边的节点。

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为二叉树的节点个数。虽然写了个二重循环，但每个节点只会添加到列表中一次，所以总的循环次数是节点个数之和，即 $\mathcal{O}(n)$。
- 空间复杂度：$\mathcal{O}(n)$。满二叉树（每一层都填满）最后一层有大约 $n/2$ 个节点，因此节点数组中最多有 $\mathcal{O}(n)$ 个元素，所以空间复杂度是 $\mathcal{O}(n)$ 的。

## 方法二：DFS

**视频讲解**：[【基础算法精讲 10】](https://www.bilibili.com/video/BV18M411z7bb/)，欢迎点赞关注~

先递归右子树，再递归左子树，当某个深度首次到达时，对应的节点就在右视图中。

```py [sol-Python3]
class Solution:
    def rightSideView(self, root: Optional[TreeNode]) -> List[int]:
        ans = []

        def dfs(node: Optional[TreeNode], depth: int) -> None:
            if node is None:
                return
            if depth == len(ans):  # 这个深度首次遇到
                ans.append(node.val)
            dfs(node.right, depth + 1)  # 先递归右子树，保证首次遇到的一定是最右边的节点
            dfs(node.left, depth + 1)

        dfs(root, 0)
        return ans
```

```java [sol-Java]
class Solution {
    public List<Integer> rightSideView(TreeNode root) {
        List<Integer> ans = new ArrayList<>();
        dfs(root, 0, ans);
        return ans;
    }

    private void dfs(TreeNode root, int depth, List<Integer> ans) {
        if (root == null) {
            return;
        }
        if (depth == ans.size()) { // 这个深度首次遇到
            ans.add(root.val);
        }
        dfs(root.right, depth + 1, ans); // 先递归右子树，保证首次遇到的一定是最右边的节点
        dfs(root.left, depth + 1, ans);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> rightSideView(TreeNode* root) {
        vector<int> ans;

        auto dfs = [&](this auto&& dfs, TreeNode* node, int depth) -> void {
            if (node == nullptr) {
                return;
            }
            if (depth == ans.size()) { // 这个深度首次遇到
                ans.push_back(node->val);
            }
            dfs(node->right, depth + 1); // 先递归右子树，保证首次遇到的一定是最右边的节点
            dfs(node->left, depth + 1);
        };

        dfs(root, 0);
        return ans;
    }
};
```

```go [sol-Go]
func rightSideView(root *TreeNode) (ans []int) {
    var dfs func(*TreeNode, int)
    dfs = func(node *TreeNode, depth int) {
        if node == nil {
            return
        }
        if depth == len(ans) { // 这个深度首次遇到
            ans = append(ans, node.Val)
        }
        dfs(node.Right, depth+1) // 先递归右子树，保证首次遇到的一定是最右边的节点
        dfs(node.Left, depth+1)
    }

    dfs(root, 0)
    return
}
```

```js [sol-JavaScript]
var rightSideView = function(root) {
    const ans = [];
    function dfs(node, depth) {
        if (node === null) {
            return;
        }
        if (depth === ans.length) { // 这个深度首次遇到
            ans.push(node.val);
        }
        dfs(node.right, depth + 1); // 先递归右子树，保证首次遇到的一定是最右边的节点
        dfs(node.left, depth + 1);
    }

    dfs(root, 0);
    return ans;
};
```

```rust [sol-Rust]
use std::rc::Rc;
use std::cell::RefCell;

impl Solution {
    pub fn right_side_view(root: Option<Rc<RefCell<TreeNode>>>) -> Vec<i32> {
        fn dfs(node: &Option<Rc<RefCell<TreeNode>>>, depth: usize, ans: &mut Vec<i32>) {
            if let Some(node) = node {
                let node = node.borrow();
                if depth == ans.len() { // 这个深度首次遇到
                    ans.push(node.val);
                }
                dfs(&node.right, depth + 1, ans); // 先递归右子树，保证首次遇到的一定是最右边的节点
                dfs(&node.left, depth + 1, ans);
            }
        }

        let mut ans = vec![];
        dfs(&root, 0, &mut ans);
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是二叉树的节点个数。
- 空间复杂度：$\mathcal{O}(h)$，其中 $h$ 是二叉树的高度。递归需要 $\mathcal{O}(h)$ 的栈空间。最坏情况下，二叉树退化成一条链，递归需要 $\mathcal{O}(n)$ 的栈空间。

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

## 本地原创解析

### 1. 题意重述

本题来自 `二、二叉树 / §2.2 自顶向下 DFS（先序遍历）`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、二叉树 / §2.2 自顶向下 DFS（先序遍历）`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
