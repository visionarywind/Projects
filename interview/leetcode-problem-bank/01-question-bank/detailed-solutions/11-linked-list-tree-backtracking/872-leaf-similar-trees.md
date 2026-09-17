# 872. 叶子相似的树

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/leaf-similar-trees/
- 题目 slug：`leaf-similar-trees`
- 来源专题：链表、树与回溯
- 来源分类路径：二、二叉树 / §2.1 遍历二叉树
- 难度分：1288
- 外部题解来源：https://leetcode.cn/problems/leaf-similar-trees/solutions/3741528/jian-dan-ti-jian-dan-zuo-pythonjavacgojs-b8ns/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[简单题，简单做（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/leaf-similar-trees/solutions/3741528/jian-dan-ti-jian-dan-zuo-pythonjavacgojs-b8ns/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`jian-dan-ti-jian-dan-zuo-pythonjavacgojs-b8ns`
- topic id：`3741528`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:30:25 +0800

按照同样的 DFS 顺序遍历这两棵二叉树，比如都按照先左子树再右子树的方式。

遍历到叶子时，把节点值加到一个列表中。

最后判断两个列表是否相同。

```py [sol-Python3]
class Solution:
    def leafValues(self, root: Optional[TreeNode]) -> List[int]:
        res = []
        def dfs(node: Optional[TreeNode]) -> None:
            if node is None:  # 空节点
                return
            if node.left is None and node.right is None:  # 叶子
                res.append(node.val)
                return
            dfs(node.left)
            dfs(node.right)
        dfs(root)
        return res

    def leafSimilar(self, root1: Optional[TreeNode], root2: Optional[TreeNode]) -> bool:
        return self.leafValues(root1) == self.leafValues(root2)
```

```java [sol-Java]
class Solution {
    public boolean leafSimilar(TreeNode root1, TreeNode root2) {
        return leafValues(root1).equals(leafValues(root2));
    }

    private List<Integer> leafValues(TreeNode root) {
        List<Integer> res = new ArrayList<>();
        dfs(root, res);
        return res;
    }

    private void dfs(TreeNode node, List<Integer> res) {
        if (node == null) { // 空节点
            return;
        }
        if (node.left == null && node.right == null) { // 叶子
            res.add(node.val);
            return;
        }
        dfs(node.left, res);
        dfs(node.right, res);
    }
}
```

```cpp [sol-C++]
class Solution {
    vector<int> leafValues(TreeNode* root) {
        vector<int> res;
        auto dfs = [&](this auto&& dfs, TreeNode* node) -> void {
            if (node == nullptr) { // 空节点
                return;
            }
            if (node->left == nullptr && node->right == nullptr) { // 叶子
                res.push_back(node->val);
                return;
            }
            dfs(node->left);
            dfs(node->right);
        };
        dfs(root);
        return res;
    }

public:
    bool leafSimilar(TreeNode* root1, TreeNode* root2) {
        return leafValues(root1) == leafValues(root2);
    }
};
```

```go [sol-Go]
func leafValues(root *TreeNode) (res []int) {
    var dfs func(*TreeNode)
    dfs = func(node *TreeNode) {
        if node == nil { // 空节点
            return
        }
        if node.Left == nil && node.Right == nil { // 叶子
            res = append(res, node.Val)
            return
        }
        dfs(node.Left)
        dfs(node.Right)
    }
    dfs(root)
    return
}

func leafSimilar(root1, root2 *TreeNode) bool {
    return slices.Equal(leafValues(root1), leafValues(root2))
}
```

```js [sol-JavaScript]
var leafValues = function(root) {
    const res = [];
    function dfs(node) {
        if (node === null) { // 空节点
            return;
        }
        if (node.left === null && node.right === null) { // 叶子
            res.push(node.val);
            return;
        }
        dfs(node.left);
        dfs(node.right);
    }
    dfs(root);
    return res;
};

var leafSimilar = function(root1, root2) {
    return _.isEqual(leafValues(root1), leafValues(root2));
};
```

```rust [sol-Rust]
use std::rc::Rc;
use std::cell::RefCell;

impl Solution {
    fn leaf_values(root: Option<Rc<RefCell<TreeNode>>>) -> Vec<i32> {
        let mut res = vec![];
        fn dfs(node: &Option<Rc<RefCell<TreeNode>>>, res: &mut Vec<i32>) {
            if let Some(n) = node {
                let n = n.borrow();
                if n.left.is_none() && n.right.is_none() { // 叶子
                    res.push(n.val);
                    return;
                }
                dfs(&n.left, res);
                dfs(&n.right, res);
            }
        }
        dfs(&root, &mut res);
        res
    }

    pub fn leaf_similar(root1: Option<Rc<RefCell<TreeNode>>>, root2: Option<Rc<RefCell<TreeNode>>>) -> bool {
        Self::leaf_values(root1) == Self::leaf_values(root2)
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n+m)$，其中 $n$ 和 $m$ 分别是两棵二叉树的大小（节点个数）。
- 空间复杂度：$\mathcal{O}(n+m)$。

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
11. 【本题相关】[链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `二、二叉树 / §2.1 遍历二叉树`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、二叉树 / §2.1 遍历二叉树`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
