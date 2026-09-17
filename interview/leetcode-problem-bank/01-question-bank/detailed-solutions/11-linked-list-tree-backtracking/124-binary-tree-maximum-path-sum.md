# 124. 二叉树中的最大路径和

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/binary-tree-maximum-path-sum/
- 题目 slug：`binary-tree-maximum-path-sum`
- 来源专题：链表、树与回溯
- 来源分类路径：二、二叉树 / §2.6 二叉树的直径
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/binary-tree-maximum-path-sum/solutions/2227021/shi-pin-che-di-zhang-wo-zhi-jing-dpcong-n9s91/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【视频】彻底掌握直径 DP！从二叉树到一般树！（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/binary-tree-maximum-path-sum/solutions/2227021/shi-pin-che-di-zhang-wo-zhi-jing-dpcong-n9s91/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`shi-pin-che-di-zhang-wo-zhi-jing-dpcong-n9s91`
- topic id：`2227021`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:39:45 +0800

## 视频讲解

**递归原理**：[深入理解递归【基础算法精讲 09】](https://www.bilibili.com/video/BV1UD4y1Y769/)。

**本题视频讲解**：[树形 DP【基础算法精讲 23】](https://www.bilibili.com/video/BV17o4y187h1/)，制作不易，欢迎点赞关注~

## 视频内容补充

本题有两个关键概念：

- **链**：从下面的某个节点（不一定是叶子）到当前节点的路径。把这条链的节点值之和，作为 $\textit{dfs}$ 的返回值。如果节点值之和是负数，则返回 $0$（和 $0$ 取最大值）。这个思想和 [53. 最大子数组和](https://leetcode.cn/problems/maximum-subarray/) 是一样的，如果左侧子数组的元素和是负数，就不和当前元素拼起来。
- **直径**：等价于由两条（或者一条）链拼成的路径。我们枚举每个 $\textit{node}$，假设直径在这里「**拐弯**」，也就是计算由左右两条从下面的某个节点（不一定是叶子）到 $\textit{node}$ 的链的节点值之和，去更新答案的最大值。

⚠**注意**：$\textit{dfs}$ 返回的是**链**的节点值之和，不是**直径**的节点值之和。

## 答疑

**问**：如果所有节点值都是负数，代码会算出什么结果？

**答**：在所有节点值都为负数的情况下，代码中的 `ans = max(ans, sum_l + node.val + sum_r)` 等价于 `ans = max(ans, node.val)`，我们求的是最大节点值（绝对值最小的负数）。这是符合题目要求的，因为在所有节点值都为负数的情况下，路径只有一个节点是最优的（毕竟节点越多，元素和越小）。类比 [53. 最大子数组和](https://leetcode.cn/problems/maximum-subarray/)，当数组元素都是负数的时候，答案就是 $\max(\textit{nums})$。

```py [sol-Python3]
class Solution:
    def maxPathSum(self, root: Optional[TreeNode]) -> int:
        ans = -inf

        def dfs(node: Optional[TreeNode]) -> int:
            if node is None:
                return 0  # 没有节点，和为 0
            sum_l = dfs(node.left)  # 左子树最大链和
            sum_r = dfs(node.right)  # 右子树最大链和
            nonlocal ans
            ans = max(ans, sum_l + node.val + sum_r)  # 左链 + node + 右链 = 路径
            return max(max(sum_l, sum_r) + node.val, 0)  # 当前子树最大链和（注意这里和 0 取最大值了）

        dfs(root)
        return ans
```

```java [sol-Java]
class Solution {
    private int ans = Integer.MIN_VALUE;

    public int maxPathSum(TreeNode root) {
        dfs(root);
        return ans;
    }

    private int dfs(TreeNode node) {
        if (node == null) {
            return 0; // 没有节点，和为 0
        }
        int sumL = dfs(node.left); // 左子树最大链和
        int sumR = dfs(node.right); // 右子树最大链和
        ans = Math.max(ans, sumL + node.val + sumR); // 左链 + node + 右链 = 路径
        return Math.max(Math.max(sumL, sumR) + node.val, 0); // 当前子树最大链和（注意这里和 0 取最大值了）
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxPathSum(TreeNode* root) {
        int ans = INT_MIN;

        auto dfs = [&](this auto&& dfs, TreeNode* node) -> int {
            if (node == nullptr) {
                return 0; // 没有节点，和为 0
            }
            int sum_l = dfs(node->left); // 左子树最大链和
            int sum_r = dfs(node->right); // 右子树最大链和
            ans = max(ans, sum_l + node->val + sum_r); // 左链 + node + 右链 = 路径
            return max(max(sum_l, sum_r) + node->val, 0); // 当前子树最大链和（注意这里和 0 取最大值了）
        };

        dfs(root);
        return ans;
    }
};
```

```c [sol-C]
int dfs(struct TreeNode* node, int* ans) {
    if (node == NULL) {
        return 0; // 没有节点，和为 0
    }
    int sum_l = dfs(node->left, ans); // 左子树最大链和
    int sum_r = dfs(node->right, ans); // 右子树最大链和
    *ans = MAX(*ans, sum_l + node->val + sum_r); // 左链 + node + 右链 = 路径
    return MAX(MAX(sum_l, sum_r) + node->val, 0); // 当前子树最大链和（注意这里和 0 取最大值了）
}

int maxPathSum(struct TreeNode* root) {
    int ans = INT_MIN; // 也可以写成全局变量
    dfs(root, &ans);
    return ans;
}
```

```go [sol-Go]
func maxPathSum(root *TreeNode) int {
    ans := math.MinInt

    var dfs func(*TreeNode) int
    dfs = func(node *TreeNode) int {
        if node == nil {
            return 0 // 没有节点，和为 0
        }
        sumL := dfs(node.Left)  // 左子树最大链和
        sumR := dfs(node.Right) // 右子树最大链和
        ans = max(ans, sumL+node.Val+sumR) // 左链 + node + 右链 = 路径
        return max(max(sumL, sumR)+node.Val, 0) // 当前子树最大链和（注意这里和 0 取最大值了）
    }

    dfs(root)
    return ans
}
```

```js [sol-JavaScript]
var maxPathSum = function(root) {
    let ans = -Infinity;

    function dfs(node) {
        if (node === null) {
            return 0; // 没有节点，和为 0
        }
        const sumL = dfs(node.left); // 左子树最大链和
        const sumR = dfs(node.right); // 右子树最大链和
        ans = Math.max(ans, sumL + node.val + sumR); // 左链 + node + 右链 = 路径
        return Math.max(Math.max(sumL, sumR) + node.val, 0); // 当前子树最大链和（注意这里和 0 取最大值了）
    }

    dfs(root);
    return ans;
};
```

```rust [sol-Rust]
use std::rc::Rc;
use std::cell::RefCell;

impl Solution {
    pub fn max_path_sum(root: Option<Rc<RefCell<TreeNode>>>) -> i32 {
        fn dfs(node: &Option<Rc<RefCell<TreeNode>>>, ans: &mut i32) -> i32 {
            let Some(node) = node else {
                return 0; // 没有节点，和为 0
            };
            let node = node.borrow();
            let sum_l = dfs(&node.left, ans); // 左子树最大链和
            let sum_r = dfs(&node.right, ans); // 右子树最大链和
            *ans = (*ans).max(sum_l + node.val + sum_r); // 左链 + node + 右链 = 路径
            0.max(sum_l.max(sum_r) + node.val) // 当前子树最大链和（注意这里和 0 取最大值了）
        }

        let mut ans = i32::MIN;
        dfs(&root, &mut ans);
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是二叉树的节点个数。
- 空间复杂度：$\mathcal{O}(h)$，其中 $h$ 是二叉树的高度。递归需要 $\mathcal{O}(h)$ 的栈空间。

## 课上题目

- [543. 二叉树的直径](https://leetcode.cn/problems/diameter-of-binary-tree/)
- [124. 二叉树中的最大路径和](https://leetcode.cn/problems/binary-tree-maximum-path-sum/)
- [2246. 相邻字符不同的最长路径](https://leetcode.cn/problems/longest-path-with-different-adjacent-characters/)

## 课后作业

- [687. 最长同值路径](https://leetcode.cn/problems/longest-univalue-path/)
- [1617. 统计子树中城市之间最大距离](https://leetcode.cn/problems/count-subtrees-with-max-distance-between-cities/)
- [2539. 最大价值和与最小价值和的差值](https://leetcode.cn/problems/difference-between-maximum-and-minimum-price-sum/)

## 分类题单

[如何科学刷题？](https://leetcode.cn/discuss/post/3141566/ru-he-ke-xue-shua-ti-by-endlesscheng-q3yd/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/discuss/post/3578981/ti-dan-hua-dong-chuang-kou-ding-chang-bu-rzz7/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/discuss/post/3579164/ti-dan-er-fen-suan-fa-er-fen-da-an-zui-x-3rqn/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/discuss/post/3579480/ti-dan-dan-diao-zhan-ju-xing-xi-lie-zi-d-u4hk/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/discuss/post/3580195/fen-xiang-gun-ti-dan-wang-ge-tu-dfsbfszo-l3pa/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/discuss/post/3580371/fen-xiang-gun-ti-dan-wei-yun-suan-ji-chu-nth4/)
6. [图论算法（DFS/BFS/拓扑排序/基环树/最短路/最小生成树/网络流）](https://leetcode.cn/discuss/post/3581143/fen-xiang-gun-ti-dan-tu-lun-suan-fa-dfsb-qyux/)
7. [动态规划（入门/背包/划分/状态机/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/discuss/post/3581838/fen-xiang-gun-ti-dan-dong-tai-gui-hua-ru-007o/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/discuss/post/3583665/fen-xiang-gun-ti-dan-chang-yong-shu-ju-j-bvmv/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/discuss/post/3584388/fen-xiang-gun-ti-dan-shu-xue-suan-fa-shu-gcai/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/discuss/post/3091107/fen-xiang-gun-ti-dan-tan-xin-ji-ben-tan-k58yb/)
11. [链表、树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/discuss/post/3142882/fen-xiang-gun-ti-dan-lian-biao-er-cha-sh-6srp/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/discuss/post/3144832/fen-xiang-gun-ti-dan-zi-fu-chuan-kmpzhan-ugt4/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `二、二叉树 / §2.6 二叉树的直径`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、二叉树 / §2.6 二叉树的直径`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
