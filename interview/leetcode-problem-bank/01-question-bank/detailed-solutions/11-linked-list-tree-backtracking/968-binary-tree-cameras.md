# 968. 监控二叉树

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/binary-tree-cameras/
- 题目 slug：`binary-tree-cameras`
- 来源专题：链表、树与回溯
- 来源分类路径：二、二叉树 / §2.12 树形 DP
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/binary-tree-cameras/solutions/2452795/shi-pin-ru-he-si-kao-shu-xing-dpgai-chen-uqsf/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【视频】一个思路解决两种变形：一般树 / 点权树（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/binary-tree-cameras/solutions/2452795/shi-pin-ru-he-si-kao-shu-xing-dpgai-chen-uqsf/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`shi-pin-ru-he-si-kao-shu-xing-dpgai-chen-uqsf`
- topic id：`2452795`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:39:45 +0800

如标题所言，本题更一般的形式是：

- 改成一般树，不是二叉树。例如，如果是三叉树要怎么做？如果是四叉树要怎么做？
- 额外输入一个数组 $\textit{cost}$，表示每个节点有不同的点权，在节点 $x$ 上安装摄像头，需要花费 $\textit{cost}[x]$。本题相当于所有节点的 $\textit{cost}$ 均为 $1$。

学会树形 DP 后，可以用同样的方法解决这些变形题目。

### 视频讲解

请看[【基础算法精讲 25】](https://www.bilibili.com/video/BV1oF411U7qL/)，包含上述变形题的讲解。

```py [sol-Python3]
class Solution:
    def minCameraCover(self, root: Optional[TreeNode]) -> int:
        def dfs(node):
            if node is None:
                return inf, 0, 0  # 空节点不能安装摄像头，也无需被监控到
            l_choose, l_by_fa, l_by_children = dfs(node.left)
            r_choose, r_by_fa, r_by_children = dfs(node.right)
            choose = min(l_choose, l_by_fa) + min(r_choose, r_by_fa) + 1
            by_fa = min(l_choose, l_by_children) + min(r_choose, r_by_children)
            by_children = min(l_choose + r_by_children, l_by_children + r_choose, l_choose + r_choose)
            return choose, by_fa, by_children

        choose, _, by_children = dfs(root)  # 根节点没有父节点
        return min(choose, by_children)
```

```java [sol-Java]
class Solution {
    public int minCameraCover(TreeNode root) {
        int[] res = dfs(root);
        return Math.min(res[0], res[2]);
    }

    private int[] dfs(TreeNode node) {
        if (node == null) {
            return new int[]{Integer.MAX_VALUE / 2, 0, 0}; // 除 2 防止加法溢出
        }
        int[] left = dfs(node.left);
        int[] right = dfs(node.right);
        int choose = Math.min(left[0], left[1]) + Math.min(right[0], right[1]) + 1;
        int byFa = Math.min(left[0], left[2]) + Math.min(right[0], right[2]);
        int byChildren = Math.min(Math.min(left[0] + right[2], left[2] + right[0]), left[0] + right[0]);
        return new int[]{choose, byFa, byChildren};
    }
}
```

```cpp [sol-C++]
class Solution {
    tuple<int, int, int> dfs(TreeNode* node) {
        if (node == nullptr) {
            return {INT_MAX / 2, 0, 0}; // 除 2 防止加法溢出
        }
        auto [l_choose, l_by_fa, l_by_children] = dfs(node->left);
        auto [r_choose, r_by_fa, r_by_children] = dfs(node->right);
        int choose = min(l_choose, l_by_fa) + min(r_choose, r_by_fa) + 1;
        int by_fa = min(l_choose, l_by_children) + min(r_choose, r_by_children);
        int by_children = min({l_choose + r_by_children, l_by_children + r_choose, l_choose + r_choose});
        return {choose, by_fa, by_children};
    }

public:
    int minCameraCover(TreeNode* root) {
        auto [choose, _, by_children] = dfs(root);
        return min(choose, by_children);
    }
};
```

```c [sol-C]
typedef struct {
    int choose;
    int by_fa;
    int by_children;
} Tuple;

Tuple dfs(struct TreeNode* node) {
    if (node == NULL) {
        return (Tuple) {INT_MAX / 2, 0, 0}; // 除 2 防止加法溢出
    }

    Tuple left = dfs(node->left);
    Tuple right = dfs(node->right);

    int choose = MIN(left.choose, left.by_fa) + MIN(right.choose, right.by_fa) + 1;
    int by_fa = MIN(left.choose, left.by_children) + MIN(right.choose, right.by_children);
    int by_children = MIN(MIN(left.choose + right.by_children, left.by_children + right.choose), left.choose + right.choose);

    return (Tuple) {choose, by_fa, by_children};
}

int minCameraCover(struct TreeNode* root) {
    Tuple ans = dfs(root);
    return MIN(ans.choose, ans.by_children);
}
```

```go [sol-Go]
func dfs(node *TreeNode) (int, int, int) {
    if node == nil {
        return math.MaxInt / 2, 0, 0 // 除 2 防止加法溢出
    }
    lChoose, lByFa, lByChildren := dfs(node.Left)
    rChoose, rByFa, rByChildren := dfs(node.Right)
    choose := min(lChoose, lByFa) + min(rChoose, rByFa) + 1
    byFa := min(lChoose, lByChildren) + min(rChoose, rByChildren)
    byChildren := min(lChoose+rByChildren, lByChildren+rChoose, lChoose+rChoose)
    return choose, byFa, byChildren
}

func minCameraCover(root *TreeNode) int {
    choose, _, byChildren := dfs(root)
    return min(choose, byChildren)
}
```

```js [sol-JavaScript]
function dfs(node) {
    if (node === null) {
        return [Infinity, 0, 0];
    }
    const [lChoose, lByFa, lByChildren] = dfs(node.left);
    const [rChoose, rByFa, rByChildren] = dfs(node.right);
    const choose = Math.min(lChoose, lByFa) + Math.min(rChoose, rByFa) + 1;
    const byFa = Math.min(lChoose, lByChildren) + Math.min(rChoose, rByChildren);
    const byChildren = Math.min(lChoose + rByChildren, lByChildren + rChoose, lChoose + rChoose);
    return [choose, byFa, byChildren];
}

var minCameraCover = function(root) {
    const [choose, , byChildren] = dfs(root);
    return Math.min(choose, byChildren);
};
```

```rust [sol-Rust]
use std::rc::Rc;
use std::cell::RefCell;

impl Solution {
    pub fn min_camera_cover(root: Option<Rc<RefCell<TreeNode>>>) -> i32 {
        fn dfs(node: Option<&Rc<RefCell<TreeNode>>>) -> (i32, i32, i32) {
            let Some(x) = node else {
                return (i32::MAX / 2, 0, 0); // 除 2 防止加法溢出
            };
            let (l_choose, l_by_fa, l_by_children) = dfs(x.borrow().left.as_ref());
            let (r_choose, r_by_fa, r_by_children) = dfs(x.borrow().right.as_ref());
            let choose = i32::min(l_choose, l_by_fa) + i32::min(r_choose, r_by_fa) + 1;
            let by_fa = i32::min(l_choose, l_by_children) + i32::min(r_choose, r_by_children);
            let by_children = i32::min(i32::min(l_choose + r_by_children, l_by_children + r_choose), l_choose + r_choose);
            (choose, by_fa, by_children)
        }

        let (choose, _, by_children) = dfs(root.as_ref());
        i32::min(choose, by_children)
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是二叉树的节点个数。每个节点都会递归恰好一次。
- 空间复杂度：$\mathcal{O}(n)$。最坏情况下，二叉树是一条链，递归需要 $\mathcal{O}(n)$ 的栈空间。

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

本题来自 `二、二叉树 / §2.12 树形 DP`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、二叉树 / §2.12 树形 DP`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
