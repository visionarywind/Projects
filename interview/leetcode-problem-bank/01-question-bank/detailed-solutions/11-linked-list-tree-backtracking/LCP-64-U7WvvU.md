# LCP 64. 二叉树灯饰

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/U7WvvU/
- 题目 slug：`U7WvvU`
- 来源专题：链表、树与回溯
- 来源分类路径：二、二叉树 / §2.12 树形 DP
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/U7WvvU/solutions/1846995/shu-xing-dp-by-endlesscheng-isuo/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[树形 DP](https://leetcode.cn/problems/U7WvvU/solutions/1846995/shu-xing-dp-by-endlesscheng-isuo/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`shu-xing-dp-by-endlesscheng-isuo`
- topic id：`1846995`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:39:45 +0800

个人赛五道题目的 [视频讲解](https://www.bilibili.com/video/BV1zN4y1K762) 已出炉，欢迎点赞三连，在评论区分享你对这场比赛的看法~

---

定义状态 (当前节点，祖先节点开关 2 的切换次数的奇偶性，父节点是否切换了开关 3)，每个状态表示从当前状态出发，最少需要操作多少次开关，可以关闭子树所有节点的灯。

跑一个树形 DP。如果当前受到祖先节点的开关影响后，变成开灯状态，那么可以操作一个或三个开关：

- 操作开关 1；
- 操作开关 2；
- 操作开关 3；
- 操作开关 123；
- 这四种操作取最小值。

如果变成关灯状态，那么可以操作零个或两个开关：

- 不操作任何一个开关；
- 操作开关 12；
- 操作开关 13；
- 操作开关 23；
- 这四种操作取最小值。

```py [sol1-Python3]
class Solution:
    def closeLampInTree(self, root: TreeNode) -> int:
        @cache  # 记忆化搜索
        def dfs(node: TreeNode, switch2: bool, switch3: bool) -> int:
            if node is None:
                return 0
            if (node.val == 1) == (switch2 == switch3):  # 当前节点为开灯
                res1 = dfs(node.left, switch2, False) + dfs(node.right, switch2, False) + 1
                res2 = dfs(node.left, not switch2, False) + dfs(node.right, not switch2, False) + 1
                res3 = dfs(node.left, switch2, True) + dfs(node.right, switch2, True) + 1
                res123 = dfs(node.left, not switch2, True) + dfs(node.right, not switch2, True) + 3
                return min(res1, res2, res3, res123)
            else:  # 当前节点为关灯
                res0 = dfs(node.left, switch2, False) + dfs(node.right, switch2, False)
                res12 = dfs(node.left, not switch2, False) + dfs(node.right, not switch2, False) + 2
                res13 = dfs(node.left, switch2, True) + dfs(node.right, switch2, True) + 2
                res23 = dfs(node.left, not switch2, True) + dfs(node.right, not switch2, True) + 2
                return min(res0, res12, res13, res23)
        return dfs(root, False, False)
```

```go [sol1-Go]
func closeLampInTree(root *TreeNode) int {
    type tuple struct {
        node             *TreeNode
        switch2, switch3 bool
    }
    memo := map[tuple]int{} // 记忆化搜索
    var dfs func(*TreeNode, bool, bool) int
    dfs = func(node *TreeNode, switch2, switch3 bool) int {
        if node == nil {
            return 0
        }
        p := tuple{node, switch2, switch3}
        if res, ok := memo[p]; ok { // 之前计算过
            return res
        }
        if node.Val == 1 == (switch2 == switch3) { // 当前节点为开灯
            res1 := dfs(node.Left, switch2, false) + dfs(node.Right, switch2, false) + 1
            res2 := dfs(node.Left, !switch2, false) + dfs(node.Right, !switch2, false) + 1
            res3 := dfs(node.Left, switch2, true) + dfs(node.Right, switch2, true) + 1
            r123 := dfs(node.Left, !switch2, true) + dfs(node.Right, !switch2, true) + 3
            memo[p] = min(res1, res2, res3, r123)
        } else { // 当前节点为关灯
            res0 := dfs(node.Left, switch2, false) + dfs(node.Right, switch2, false)
            res12 := dfs(node.Left, !switch2, false) + dfs(node.Right, !switch2, false) + 2
            res13 := dfs(node.Left, switch2, true) + dfs(node.Right, switch2, true) + 2
            res23 := dfs(node.Left, !switch2, true) + dfs(node.Right, !switch2, true) + 2
            memo[p] = min(res0, res12, res13, res23)
        }
        return memo[p]
    }
    return dfs(root, false, false)
}
```

#### 复杂度分析

- 时间复杂度：$O(n)$，其中 $n$ 为二叉树的节点个数。
- 空间复杂度：$O(n)$。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与一般树（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

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
