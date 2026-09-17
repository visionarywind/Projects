# 113. 路径总和 II

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/path-sum-ii/
- 题目 slug：`path-sum-ii`
- 来源专题：链表、树与回溯
- 来源分类路径：二、二叉树 / §2.7 回溯
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/path-sum-ii/solutions/3061294/hui-su-fu-chang-jian-wen-ti-ji-qi-jie-da-g8im/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[回溯，附常见问题及其解答（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/path-sum-ii/solutions/3061294/hui-su-fu-chang-jian-wen-ti-ji-qi-jie-da-g8im/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`hui-su-fu-chang-jian-wen-ti-ji-qi-jie-da-g8im`
- topic id：`3061294`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:39:45 +0800

在递归的同时，额外维护从根到当前节点的元素和 $s$。此外，在递归函数外部维护一个 $\textit{path}$ 列表，记录从根到当前节点路径上的所有节点。

递归逻辑：

1. 如果当前节点是空节点，直接返回。
2. 把当前节点加入 $\textit{path}$，同时把 $s$ 增加当前节点值。
3. 如果当前节点是叶子节点且 $s=\textit{targetSum}$，那么把路径 $\textit{path}$ 加入答案。也可以在一开始递归的时候传入 $\textit{left}=\textit{targetSum}$，在递归过程中不断地把 $\textit{left}$ 减去节点值。递归到叶节点的时候，如果发现 $\textit{left}=0$，那么把路径 $\textit{path}$ 加入答案。
4. 否则，继续递归左右子树。
5. 在递归返回之前，把我们在递归开头加入的节点，也就是当前 $\textit{path}$ 的最后一个节点，从 $\textit{path}$ 中去掉（恢复现场）。为什么要写这行代码？当我们递归完左子树，要递归右子树之前，$\textit{path}$ 中还保留着左子树的节点。如果不及时去掉，会导致最终加到答案中的 $\textit{path}$，既包含左子树的节点，又包含右子树的节点，这连「路径」都算不上。

### 答疑

**问**：为什么加入答案之前，要把 $\textit{path}$ 复制一份？

**答**：不复制的话 $\textit{ans}$ 保存的是同一个 $\textit{path}$ 的引用，修改 $\textit{path}$ 也会修改 $\textit{ans}$ 中的列表。

**问**：为什么递归参数 $\textit{left}$ 不需要「恢复现场」？

**答**：对于 `int` 这种基本类型的参数，在函数调用的时候会**复制**一份往下传递，`left -= node.val` 修改的仅仅是当前递归函数中的 $\textit{left}$ 参数，并不会影响到其他递归函数中的 $\textit{left}$。但是，如果把 $\textit{left}$ 放在递归函数外，执行 `left -= node.val` 就会影响全局了，这种情况下是需要恢复现场的。

```py [sol-Python3]
class Solution:
    def pathSum(self, root: Optional[TreeNode], targetSum: int) -> List[List[int]]:
        ans = []
        path = []

        def dfs(node: Optional[TreeNode], left: int) -> None:
            if node is None:
                return
            path.append(node.val)
            left -= node.val
            if node.left is None and node.right is None and left == 0:
                ans.append(path.copy())  # 也可以写 path[:]
            else:
                dfs(node.left, left)
                dfs(node.right, left)
            path.pop()  # 恢复现场

        dfs(root, targetSum)
        return ans
```

```java [sol-Java]
class Solution {
    public List<List<Integer>> pathSum(TreeNode root, int targetSum) {
        List<List<Integer>> ans = new ArrayList<>();
        List<Integer> path = new ArrayList<>();
        dfs(root, targetSum, path, ans);
        return ans;
    }

    private void dfs(TreeNode node, int left, List<Integer> path, List<List<Integer>> ans) {
        if (node == null) {
            return;
        }
        path.add(node.val);
        left -= node.val;
        if (node.left == null && node.right == null && left == 0) {
            ans.add(new ArrayList<>(path));
        } else {
            dfs(node.left, left, path, ans);
            dfs(node.right, left, path, ans);
        }
        path.remove(path.size() - 1);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<vector<int>> pathSum(TreeNode* root, int targetSum) {
        vector<vector<int>> ans;
        vector<int> path;

        auto dfs = [&](this auto&& dfs, TreeNode* node, int left) -> void {
            if (node == nullptr) {
                return;
            }
            path.push_back(node->val);
            left -= node->val;
            if (node->left == nullptr && node->right == nullptr && left == 0) {
                ans.push_back(path);
            } else {
                dfs(node->left, left);
                dfs(node->right, left);
            }
            path.pop_back(); // 恢复现场
        };

        dfs(root, targetSum);
        return ans;
    }
};
```

```go [sol-Go]
func pathSum(root *TreeNode, targetSum int) (ans [][]int) {
    path := []int{}

    var dfs func(*TreeNode, int)
    dfs = func(node *TreeNode, left int) {
        if node == nil {
            return
        }
        path = append(path, node.Val)
        left -= node.Val
        if node.Left == nil && node.Right == nil && left == 0 {
            ans = append(ans, slices.Clone(path))
        } else {
            dfs(node.Left, left)
            dfs(node.Right, left)
        }
        path = path[:len(path)-1] // 恢复现场
    }

    dfs(root, targetSum)
    return ans
}
```

```js [sol-JavaScript]
var pathSum = function(root, targetSum) {
    const ans = [];
    const path = [];

    function dfs(node, left) {
        if (node === null) {
            return;
        }
        path.push(node.val);
        left -= node.val;
        if (node.left === null && node.right === null && left === 0) {
            ans.push([...path]);
        } else {
            dfs(node.left, left);
            dfs(node.right, left);
        }
        path.pop(); // 恢复现场
    };

    dfs(root, targetSum);
    return ans;
};
```

```rust [sol-Rust]
use std::rc::Rc;
use std::cell::RefCell;

impl Solution {
    pub fn path_sum(root: Option<Rc<RefCell<TreeNode>>>, target_sum: i32) -> Vec<Vec<i32>> {
        fn dfs(node: &Option<Rc<RefCell<TreeNode>>>, mut left: i32, path: &mut Vec<i32>, ans: &mut Vec<Vec<i32>>) {
            if let Some(node) = node {
                let node = node.borrow();
                path.push(node.val);
                left -= node.val;
                if node.left.is_none() && node.right.is_none() && left == 0 {
                    ans.push(path.clone());
                } else {
                    dfs(&node.left, left, path, ans);
                    dfs(&node.right, left, path, ans);
                }
                path.pop();
            }
        }

        let mut ans = vec![];
        let mut path = vec![];
        dfs(&root, target_sum, &mut path, &mut ans);
        return ans;
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2)$，其中 $n$ 是二叉树的节点个数。对于「一条链 + 完全二叉树」这样的「扫帚型」二叉树，我们会在 $\mathcal{O}(n)$ 个叶子节点处，都去复制长为 $\mathcal{O}(n)$ 的 $\textit{path}$，所以总的时间复杂度为 $\mathcal{O}(n^2)$。
- 空间复杂度：$\mathcal{O}(n)$。返回值不计入。

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
