# 257. 二叉树的所有路径

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/binary-tree-paths/
- 题目 slug：`binary-tree-paths`
- 来源专题：链表、树与回溯
- 来源分类路径：二、二叉树 / §2.7 回溯
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/binary-tree-paths/solutions/3038189/liang-chong-xie-fa-lu-jing-wei-can-shu-h-q2wz/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种写法：路径为参数 / 回溯（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/binary-tree-paths/solutions/3038189/liang-chong-xie-fa-lu-jing-wei-can-shu-h-q2wz/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`liang-chong-xie-fa-lu-jing-wei-can-shu-h-q2wz`
- topic id：`3038189`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:39:45 +0800

## 方法一：递归，路径为参数

递归二叉树的过程中，额外传入字符串参数 $\textit{path}$，初始为空串。

分类讨论：

- 如果当前节点是空节点，什么也不做，返回。
- 否则，先把节点值（字符串形式）加到 $\textit{path}$ 的末尾。
- 如果当前节点是叶子节点，把 $\textit{path}$ 加到答案。
- 如果当前节点不是叶子节点，那么后续必然还会在 $\textit{path}$ 后加入新的节点值。在此之前，把 $\texttt{->}$ 加到 $\textit{path}$ 的末尾。

晕递归的同学，请先看视频：[深入理解递归【基础算法精讲 09】](https://www.bilibili.com/video/BV1UD4y1Y769/)。

```py [sol-Python3]
class Solution:
    def binaryTreePaths(self, root: Optional[TreeNode]) -> List[str]:
        ans = []

        def dfs(node: Optional[TreeNode], path: str) -> None:
            if node is None:
                return
            path += str(node.val)
            if node.left is None and node.right is None:  # 叶子节点
                ans.append(path)
                return
            path += "->"
            dfs(node.left, path)
            dfs(node.right, path)

        dfs(root, "")
        return ans
```

```java [sol-Java]
class Solution {
    public List<String> binaryTreePaths(TreeNode root) {
        List<String> ans = new ArrayList<>();
        dfs(root, "", ans);
        return ans;
    }

    // 把 String 改成 StringBuilder 更快，见右边的【Java 写法二】
    private void dfs(TreeNode node, String path, List<String> ans) {
        if (node == null) {
            return;
        }
        path += node.val;
        if (node.left == null && node.right == null) { // 叶子节点
            ans.add(path);
            return;
        }
        path += "->";
        dfs(node.left, path, ans);
        dfs(node.right, path, ans);
    }
}
```

```java [sol-Java 写法二]
class Solution {
    public List<String> binaryTreePaths(TreeNode root) {
        List<String> ans = new ArrayList<>();
        dfs(root, new StringBuilder(), ans);
        return ans;
    }

    private void dfs(TreeNode node, StringBuilder path, List<String> ans) {
        if (node == null) {
            return;
        }
        int len = path.length(); // 记录当前长度，用于回溯
        path.append(node.val);
        if (node.left == null && node.right == null) { // 叶子节点
            ans.add(path.toString());
        } else {
            path.append("->");
            dfs(node.left, path, ans);
            dfs(node.right, path, ans);
        }
        path.setLength(len); // 回溯，恢复现场
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<string> binaryTreePaths(TreeNode* root) {
        vector<string> ans;
        auto dfs = [&](this auto&& dfs, TreeNode* node, string path) -> void {
            if (node == nullptr) {
                return;
            }
            path += to_string(node->val);
            if (node->left == nullptr && node->right == nullptr) { // 叶子节点
                ans.push_back(path);
                return;
            }
            path += "->";
            dfs(node->left, path);
            dfs(node->right, path);
        };
        dfs(root, "");
        return ans;
    }
};
```

```go [sol-Go]
func binaryTreePaths(root *TreeNode) (ans []string) {
    var dfs func(*TreeNode, string)
    dfs = func(node *TreeNode, path string) {
        if node == nil {
            return
        }
        path += strconv.Itoa(node.Val)
        if node.Left == nil && node.Right == nil { // 叶子节点
            ans = append(ans, path)
            return
        }
        path += "->"
        dfs(node.Left, path)
        dfs(node.Right, path)
    }
    dfs(root, "")
    return ans
}
```

```js [sol-JavaScript]
var binaryTreePaths = function(root) {
    const ans = [];
    function dfs(node, path) {
        if (node === null) {
            return;
        }
        path += node.val;
        if (node.left === null && node.right === null) { // 叶子节点
            ans.push(path);
            return;
        }
        path += "->";
        dfs(node.left, path);
        dfs(node.right, path);
    }
    dfs(root, "");
    return ans;
};
```

```rust [sol-Rust]
use std::rc::Rc;
use std::cell::RefCell;

impl Solution {
    pub fn binary_tree_paths(root: Option<Rc<RefCell<TreeNode>>>) -> Vec<String> {
        fn dfs(node: &Option<Rc<RefCell<TreeNode>>>, mut path: String, ans: &mut Vec<String>) {
            if let Some(node) = node {
                let node = node.borrow();
                path += &node.val.to_string();
                if node.left.is_none() && node.right.is_none() { // 叶子节点
                    ans.push(path);
                    return;
                }
                path += "->";
                dfs(&node.left, path.clone(), ans);
                dfs(&node.right, path, ans);
            }
        }

        let mut ans = vec![];
        dfs(&root, "".to_string(), &mut ans);
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2\log U)$，其中 $n$ 是二叉树的节点个数，$U$ 是节点绝对值的最大值。最坏情况下，二叉树是扫帚形，即一条 $\mathcal{O}(n)$ 个节点的链加上一棵包含 $\mathcal{O}(n)$ 个叶子的（完全）二叉树。在这种情况下，由于每个节点值的字符串的长度为 $\mathcal{O}(\log U)$，所以路径长度为 $\mathcal{O}(n\log U)$。我们会把 $\mathcal{O}(n)$ 个长为 $\mathcal{O}(n\log U)$ 的 $\textit{path}$ 加入答案。
- 空间复杂度：$\mathcal{O}(n^2\log U)$。所有 $\textit{path}$ 加起来一共需要 $\mathcal{O}(n^2\log U)$ 的空间。

## 方法二：回溯，路径为外部变量

把 $\textit{path}$ 声明为 DFS 外的变量。

在这个递归过程中：

1. 如果没有递归到叶子节点，我们会先递归左子树，然后递归右子树。
2. 递归完了左子树，就要**倒回去**，递归右子树。
3. 倒回去的过程中，之前加到 $\textit{path}$ 中的数据（在左子树中）是垃圾数据，要及时清除掉（恢复现场）。

```py [sol-Python3]
class Solution:
    def binaryTreePaths(self, root: Optional[TreeNode]) -> List[str]:
        ans = []
        path = []

        def dfs(node: Optional[TreeNode]) -> None:
            if node is None:
                return
            path.append(str(node.val))
            if node.left is None and node.right is None:  # 叶子节点
                ans.append("->".join(path))
            else:
                dfs(node.left)
                dfs(node.right)
            path.pop()  # 恢复现场，撤销上面的 path.append(str(node.val))

        dfs(root)
        return ans
```

```java [sol-Java]
class Solution {
    public List<String> binaryTreePaths(TreeNode root) {
        List<String> ans = new ArrayList<>();
        List<String> path = new ArrayList<>();
        dfs(root, ans, path);
        return ans;
    }

    private void dfs(TreeNode node, List<String> ans, List<String> path) {
        if (node == null) {
            return;
        }
        path.add(String.valueOf(node.val));
        if (node.left == null && node.right == null) { // 叶子节点
            ans.add(String.join("->", path));
        } else {
            dfs(node.left, ans, path);
            dfs(node.right, ans, path);
        }
        // 恢复现场，撤销上面的 path.add(String.valueOf(node.val));
        path.removeLast(); // Java 旧版本请用 path.remove(path.size() - 1);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<string> binaryTreePaths(TreeNode* root) {
        vector<string> ans;
        vector<string> path;
        auto dfs = [&](this auto&& dfs, TreeNode* node) -> void {
            if (node == nullptr) {
                return;
            }
            path.push_back(to_string(node->val));
            if (node->left == nullptr && node->right == nullptr) { // 叶子节点
                string joined_path;
                for (int i = 0; i < path.size(); i++) {
                    if (i > 0) {
                        joined_path += "->";
                    }
                    joined_path += path[i];
                }
                ans.push_back(joined_path);
            } else {
                dfs(node->left);
                dfs(node->right);
            }
            path.pop_back(); // 恢复现场，撤销上面的 path.push_back(to_string(node->val));
        };
        dfs(root);
        return ans;
    }
};
```

```go [sol-Go]
func binaryTreePaths(root *TreeNode) (ans []string) {
    path := []string{}
    var dfs func(*TreeNode)
    dfs = func(node *TreeNode) {
        if node == nil {
            return
        }
        path = append(path, strconv.Itoa(node.Val))
        if node.Left == nil && node.Right == nil { // 叶子节点
            ans = append(ans, strings.Join(path, "->"))
        } else {
            dfs(node.Left)
            dfs(node.Right)
        }
        path = path[:len(path)-1] // 恢复现场，撤销上面的 path = append(path, strconv.Itoa(node.Val))
    }
    dfs(root)
    return ans
}
```

```js [sol-JavaScript]
var binaryTreePaths = function(root) {
    const ans = [];
    const path = [];
    function dfs(node) {
        if (node === null) {
            return;
        }
        path.push(node.val.toString());
        if (node.left === null && node.right === null) { // 叶子节点
            ans.push(path.join("->"));
        } else {
            dfs(node.left);
            dfs(node.right);
        }
        path.pop(); // 恢复现场，撤销上面的 path.push(node.val.toString());
    }
    dfs(root);
    return ans;
};
```

```rust [sol-Rust]
use std::rc::Rc;
use std::cell::RefCell;

impl Solution {
    pub fn binary_tree_paths(root: Option<Rc<RefCell<TreeNode>>>) -> Vec<String> {
        fn dfs(node: &Option<Rc<RefCell<TreeNode>>>, path: &mut Vec<String>, ans: &mut Vec<String>) {
            if let Some(node) = node {
                let node = node.borrow();
                path.push(node.val.to_string());
                if node.left.is_none() && node.right.is_none() { // 叶子节点
                    ans.push(path.join("->"));
                } else {
                    dfs(&node.left, path, ans);
                    dfs(&node.right, path, ans);
                }
                path.pop(); // 恢复现场，撤销上面的 path.push(node.val.to_string());
            }
        }

        let mut ans = vec![];
        let mut path = vec![];
        dfs(&root, &mut path, &mut ans);
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2\log U)$，其中 $n$ 是二叉树的节点个数，$U$ 是节点绝对值的最大值。最坏情况下，二叉树是扫帚形，即一条 $\mathcal{O}(n)$ 个节点的链加上一棵包含 $\mathcal{O}(n)$ 个叶子的（完全）二叉树。在这种情况下，由于每个节点值的字符串的长度为 $\mathcal{O}(\log U)$，所以路径长度为 $\mathcal{O}(n\log U)$。我们会把 $\mathcal{O}(n)$ 个长为 $\mathcal{O}(n\log U)$ 的 $\textit{path}$ 加入答案。
- 空间复杂度：$\mathcal{O}(n\log U)$。返回值不计入。$\textit{path}$ 需要 $\mathcal{O}(n\log U)$ 的空间。

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

## 本地原创解析

### 1. 题意重述

本题来自 `二、二叉树 / §2.7 回溯`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、二叉树 / §2.7 回溯`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
