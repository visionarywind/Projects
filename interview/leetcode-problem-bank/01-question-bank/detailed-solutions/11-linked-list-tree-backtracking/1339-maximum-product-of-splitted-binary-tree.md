# 1339. 分裂二叉树的最大乘积

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-product-of-splitted-binary-tree/
- 题目 slug：`maximum-product-of-splitted-binary-tree`
- 来源专题：链表、树与回溯
- 来源分类路径：二、二叉树 / §2.3 自底向上 DFS（后序遍历）
- 难度分：1675
- 外部题解来源：https://leetcode.cn/problems/maximum-product-of-splitted-binary-tree/solutions/3870465/liang-ci-dfshuo-zhe-ti-qian-bao-cun-zi-s-uo4x/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两次 DFS，或者提前保存子树和（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/maximum-product-of-splitted-binary-tree/solutions/3870465/liang-ci-dfshuo-zhe-ti-qian-bao-cun-zi-s-uo4x/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`liang-ci-dfshuo-zhe-ti-qian-bao-cun-zi-s-uo4x`
- topic id：`3870465`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:39:45 +0800

第一次 DFS：计算整棵树的点权和 $\textit{total}$。

第二次 DFS：计算子树点权和 $s$，那么删除当前节点到其父节点的边后，另一部分的和就是 $\textit{total} - s$，二者乘积为

$$
s\cdot(\textit{total} - s)
$$

用其更新答案的最大值。

由于本题保证点权是非负，我们无需判断当前节点是根节点的特殊情况（无父节点），此时上式为 $0$，不影响答案。

> 如果有负数点权，就需要跳过当前节点是根节点的情况。

也可以在第一次 DFS 时，把子树和保存到一个列表中，第二次只需遍历这个列表，见 Python3 的第一份代码。

```py [sol-Python3]
class Solution:
    def maxProduct(self, root: Optional[TreeNode]) -> int:
        def dfs(node: Optional[TreeNode]) -> int:
            if node is None:
                return 0
            s = node.val + dfs(node.left) + dfs(node.right)
            sub_sum.append(s)
            return s

        sub_sum = []
        total = dfs(root)

        ans = max(s * (total - s) for s in sub_sum)
        return ans % 1_000_000_007
```

```py [sol-Python3 两次 DFS]
class Solution:
    def maxProduct(self, root: Optional[TreeNode]) -> int:
        def dfs1(node: Optional[TreeNode]) -> int:
            if node is None:
                return 0
            return node.val + dfs1(node.left) + dfs1(node.right)

        def dfs2(node: Optional[TreeNode]) -> int:
            if node is None:
                return 0
            s = node.val + dfs2(node.left) + dfs2(node.right)
            nonlocal ans
            ans = max(ans, s * (total - s))
            return s

        total = dfs1(root)

        ans = 0
        dfs2(root)

        return ans % 1_000_000_007
```

```java [sol-Java]
class Solution {
    private static final int MOD = 1_000_000_007;
    private long ans = 0;

    public int maxProduct(TreeNode root) {
        int total = dfs1(root);
        dfs2(root, total);
        return (int) (ans % MOD);
    }

    private int dfs1(TreeNode node) {
        if (node == null) {
            return 0;
        }
        return node.val + dfs1(node.left) + dfs1(node.right);
    }

    private int dfs2(TreeNode node, int total) {
        if (node == null) {
            return 0;
        }
        int s = node.val + dfs2(node.left, total) + dfs2(node.right, total);
        ans = Math.max(ans, (long) s * (total - s));
        return s;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxProduct(TreeNode* root) {
        auto dfs1 = [&](this auto&& dfs1, TreeNode* node) -> int {
            if (node == nullptr) {
                return 0;
            }
            return node->val + dfs1(node->left) + dfs1(node->right);
        };
        long long total = dfs1(root);

        long long ans = 0;
        auto dfs2 = [&](this auto&& dfs2, TreeNode* node) -> int {
            if (node == nullptr) {
                return 0;
            }
            int s = node->val + dfs2(node->left) + dfs2(node->right);
            ans = max(ans, s * (total - s));
            return s;
        };
        dfs2(root);

        return ans % 1'000'000'007;
    }
};
```

```c [sol-C]
#define MAX(a, b) ((b) > (a) ? (b) : (a))

int maxProduct(struct TreeNode* root) {
    int dfs1(struct TreeNode* node) {
        if (node == NULL) {
            return 0;
        }
        return node->val + dfs1(node->left) + dfs1(node->right);
    }

    long long total = dfs1(root);
    long long ans = 0;

    int dfs2(struct TreeNode* node) {
        if (node == NULL) {
            return 0;
        }
        int s = node->val + dfs2(node->left) + dfs2(node->right);
        ans = MAX(ans, s * (total - s));
        return s;
    }

    dfs2(root);
    return ans % 1000000007;
}
```

```go [sol-Go]
func maxProduct(root *TreeNode) (ans int) {
	var dfs1 func(*TreeNode) int
	dfs1 = func(node *TreeNode) int {
		if node == nil {
			return 0
		}
		return node.Val + dfs1(node.Left) + dfs1(node.Right)
	}
	total := dfs1(root)

	var dfs2 func(*TreeNode) int
	dfs2 = func(node *TreeNode) int {
		if node == nil {
			return 0
		}
		s := node.Val + dfs2(node.Left) + dfs2(node.Right)
		ans = max(ans, s*(total-s))
		return s
	}
	dfs2(root)

	return ans % 1_000_000_007
}
```

```js [sol-JavaScript]
var maxProduct = function(root) {
    function dfs1(node) {
        if (node === null) {
            return 0;
        }
        return node.val + dfs1(node.left) + dfs1(node.right);
    }

    function dfs2(node) {
        if (node === null) {
            return 0;
        }
        const s = node.val + dfs2(node.left) + dfs2(node.right);
        ans = Math.max(ans, s * (total - s));
        return s;
    }

    const total = dfs1(root);

    let ans = 0;
    dfs2(root);

    return ans % 1_000_000_007;
};
```

```rust [sol-Rust]
use std::rc::Rc;
use std::cell::RefCell;

impl Solution {
    pub fn max_product(root: Option<Rc<RefCell<TreeNode>>>) -> i32 {
        fn dfs1(node: &Option<Rc<RefCell<TreeNode>>>) -> i32 {
            if let Some(n) = node {
                let n = n.borrow();
                n.val + dfs1(&n.left) + dfs1(&n.right)
            } else {
                0
            }
        }

        fn dfs2(node: &Option<Rc<RefCell<TreeNode>>>, total: i32, ans: &mut i64) -> i32 {
            if let Some(n) = node {
                let n = n.borrow();
                let s = n.val + dfs2(&n.left, total, ans) + dfs2(&n.right, total, ans);
                *ans = (*ans).max(s as i64 * (total - s) as i64);
                s
            } else {
                0
            }
        }

        let total = dfs1(&root);

        let mut ans = 0;
        dfs2(&root, total, &mut ans);

        (ans % 1_000_000_007) as _
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是二叉树的节点个数。
- 空间复杂度：$\mathcal{O}(h)$，其中 $h$ 是二叉树的高度。递归需要消耗 $\mathcal{O}(h)$ 的栈空间。

## 专题训练

见下面树题单的「**§2.3 自底向上 DFS**」。

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

本题来自 `二、二叉树 / §2.3 自底向上 DFS（后序遍历）`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、二叉树 / §2.3 自底向上 DFS（后序遍历）`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
