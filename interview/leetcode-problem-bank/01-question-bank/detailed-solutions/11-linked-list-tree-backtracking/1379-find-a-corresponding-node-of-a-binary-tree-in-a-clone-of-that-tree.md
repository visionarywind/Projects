# 1379. 找出克隆二叉树中的相同节点

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/find-a-corresponding-node-of-a-binary-tree-in-a-clone-of-that-tree/
- 题目 slug：`find-a-corresponding-node-of-a-binary-tree-in-a-clone-of-that-tree`
- 来源专题：链表、树与回溯
- 来源分类路径：二、二叉树 / §2.3 自底向上 DFS（后序遍历）
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/find-a-corresponding-node-of-a-binary-tree-in-a-clone-of-that-tree/solutions/2721724/jian-ji-xie-fa-pythonjavacjs-by-endlessc-3vri/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[简洁写法（Python/Java/C++/JS）](https://leetcode.cn/problems/find-a-corresponding-node-of-a-binary-tree-in-a-clone-of-that-tree/solutions/2721724/jian-ji-xie-fa-pythonjavacjs-by-endlessc-3vri/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`jian-ji-xie-fa-pythonjavacjs-by-endlessc-3vri`
- topic id：`2721724`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:30:25 +0800

我们直接在 $\texttt{getTargetCopy}$ 内部调用它自己，执行递归（先序遍历）：

1. 如果 $\textit{original}$ 是空节点，返回空。
2. 如果 $\textit{original} = \textit{target}$（注意这里比较的是节点，不是节点值），说明我们找到了对应的节点，返回 $\textit{cloned}$。
3. 否则递归 $\textit{original}$ 和 $\textit{cloned}$ 的左子树，如果返回值 $\textit{leftRes}$ 不为空，说明 $\textit{target}$ 在左子树中，返回 $\textit{leftRes}$。
4. 否则递归 $\textit{original}$ 和 $\textit{cloned}$ 的右子树，由于题目保证 $\textit{target}$ 一定在二叉树中，所以直接返回递归右子树的返回值。

晕递归的同学可以看 [深刻理解递归【基础算法精讲 09】](https://www.bilibili.com/video/BV1UD4y1Y769/)

注：由于我们比较的是节点而不是节点值（例如 C++ 比较的是地址），所以下面的代码也适用于树中有值相同节点的情况（本题的进阶问题）。

```py [sol-Python3]
class Solution:
    def getTargetCopy(self, original: TreeNode, cloned: TreeNode, target: TreeNode) -> TreeNode:
        if original is None or original is target:
            return cloned
        return self.getTargetCopy(original.left, cloned.left, target) or \
               self.getTargetCopy(original.right, cloned.right, target)
```

```java [sol-Java]
class Solution {
    public TreeNode getTargetCopy(TreeNode original, TreeNode cloned, TreeNode target) {
        if (original == null || original == target) {
            return cloned;
        }
        TreeNode leftRes = getTargetCopy(original.left, cloned.left, target);
        if (leftRes != null) {
            return leftRes; // 已经找到 target，无需递归右子树
        }
        return getTargetCopy(original.right, cloned.right, target);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    TreeNode *getTargetCopy(TreeNode *original, TreeNode *cloned, TreeNode *target) {
        if (original == nullptr || original == target) {
            return cloned;
        }
        auto left_res = getTargetCopy(original->left, cloned->left, target);
        if (left_res) {
            return left_res; // 已经找到 target，无需递归右子树
        }
        return getTargetCopy(original->right, cloned->right, target);
    }
};
```

```js [sol-JavaScript]
var getTargetCopy = function(original, cloned, target) {
    if (original === null || original === target) {
        return cloned;
    }
    return getTargetCopy(original.left, cloned.left, target) ??
           getTargetCopy(original.right, cloned.right, target);
};
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为二叉树的节点个数。
- 空间复杂度：$\mathcal{O}(n)$。最坏情况下，二叉树退化成一条链，递归需要 $\mathcal{O}(n)$ 的栈空间。

## 分类题单

1. [滑动窗口（定长/不定长/多指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（矩形系列/字典序最小/贡献法）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/贪心/脑筋急转弯）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)

更多题单，点我个人主页 - 讨论发布。

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

[往期题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

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
