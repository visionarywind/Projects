# 2641. 二叉树的堂兄弟节点 II

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/cousins-in-binary-tree-ii/
- 题目 slug：`cousins-in-binary-tree-ii`
- 来源专题：链表、树与回溯
- 来源分类路径：二、二叉树 / §2.13 二叉树 BFS
- 难度分：1677
- 外部题解来源：https://leetcode.cn/problems/cousins-in-binary-tree-ii/solutions/2229010/bfssuan-liang-ci-pythonjavacgo-by-endles-b72a/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[BFS+算两次（Python/Java/C++/Go）](https://leetcode.cn/problems/cousins-in-binary-tree-ii/solutions/2229010/bfssuan-liang-ci-pythonjavacgo-by-endles-b72a/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`bfssuan-liang-ci-pythonjavacgo-by-endles-b72a`
- topic id：`2229010`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:49:15 +0800

## 前置知识：二叉树 BFS

下面代码用双数组实现 BFS，原理请看[【基础算法精讲 13】](https://www.bilibili.com/video/BV1hG4y1277i/)。

## 本题视频讲解

请看[【双周赛 102】](https://www.bilibili.com/video/BV1Es4y1N7v1/) 第三题。

## 提示 1

下文将具有相同父节点的节点互称为**兄弟节点**。

对于一个节点 $x$ 来说，它的所有**堂兄弟节点**值的和，等于 $x$ 这一层的所有节点值之和减去 $x$ 及其兄弟节点的值之和。

比如示例 1：

- $4$ 的左右儿子的节点值，都被更新成了 $7$，也就是左右儿子这一层的节点值之和 $1+10+7=18$，减去 $4$ 的左右儿子的节点值之和 $1+10=11$，得到 $7$。
- $9$ 的右儿子的节点值，被更新成了 $11$，也就是右儿子这一层的节点值之和 $1+10+7=18$，减去 $9$ 的右儿子的节点值 $7$，得到 $11$。

## 提示 2

怎么实现呢？

用 BFS 遍历二叉树，对于每一层：

- 首先，遍历当前层的每个节点，通过节点的左右儿子，计算下一层的节点值之和 $\textit{nextLevelSum}$。
- 然后，再次遍历当前层的每个节点 $x$，计算 $x$ 的左右儿子的节点值之和 $\textit{childrenSum}$，更新 $x$ 的左右儿子的节点值为 $\textit{nextLevelSum}-\textit{childrenSum}$。

```py [sol-Python3]
class Solution:
    def replaceValueInTree(self, root: Optional[TreeNode]) -> Optional[TreeNode]:
        root.val = 0
        q = [root]
        while q:
            tmp = q
            q = []

            # 计算下一层的节点值之和
            next_level_sum = 0
            for node in tmp:
                if node.left:
                    q.append(node.left)
                    next_level_sum += node.left.val
                if node.right:
                    q.append(node.right)
                    next_level_sum += node.right.val

            # 再次遍历，更新下一层的节点值
            for node in tmp:
                children_sum = (node.left.val if node.left else 0) + \
                               (node.right.val if node.right else 0)
                if node.left: node.left.val = next_level_sum - children_sum
                if node.right: node.right.val = next_level_sum - children_sum
        return root
```

```java [sol-Java]
class Solution {
    public TreeNode replaceValueInTree(TreeNode root) {
        root.val = 0;
        List<TreeNode> q = List.of(root);
        while (!q.isEmpty()) {
            List<TreeNode> tmp = q;
            q = new ArrayList<>();

            // 计算下一层的节点值之和
            int nextLevelSum = 0;
            for (TreeNode node : tmp) {
                if (node.left != null) {
                    q.add(node.left);
                    nextLevelSum += node.left.val;
                }
                if (node.right != null) {
                    q.add(node.right);
                    nextLevelSum += node.right.val;
                }
            }

            // 再次遍历，更新下一层的节点值
            for (TreeNode node : tmp) {
                int childrenSum = (node.left != null ? node.left.val : 0) +
                                  (node.right != null ? node.right.val : 0);
                if (node.left != null) node.left.val = nextLevelSum - childrenSum;
                if (node.right != null) node.right.val = nextLevelSum - childrenSum;
            }
        }
        return root;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    TreeNode *replaceValueInTree(TreeNode *root) {
        root->val = 0;
        vector<TreeNode*> q = {root};
        while (!q.empty()) {
            vector<TreeNode*> nxt;
            // 计算下一层的节点值之和
            int next_level_sum = 0;
            for (auto node : q) {
                if (node->left) {
                    nxt.push_back(node->left);
                    next_level_sum += node->left->val;
                }
                if (node->right) {
                    nxt.push_back(node->right);
                    next_level_sum += node->right->val;
                }
            }

            // 再次遍历，更新下一层的节点值
            for (auto node : q) {
                int children_sum = (node->left ? node->left->val : 0) +
                                   (node->right ? node->right->val : 0);
                if (node->left) node->left->val = next_level_sum - children_sum;
                if (node->right) node->right->val = next_level_sum - children_sum;
            }
            q = move(nxt);
        }
        return root;
    }
};
```

```go [sol-Go]
func replaceValueInTree(root *TreeNode) *TreeNode {
	root.Val = 0
	q := []*TreeNode{root}
	for len(q) > 0 {
		tmp := q
		q = nil
		
		// 计算下一层的节点值之和
		nextLevelSum := 0
		for _, node := range tmp {
			if node.Left != nil {
				q = append(q, node.Left)
				nextLevelSum += node.Left.Val
			}
			if node.Right != nil {
				q = append(q, node.Right)
				nextLevelSum += node.Right.Val
			}
		}

		// 再次遍历，更新下一层的节点值
		for _, node := range tmp {
			childrenSum := 0 // node 左右儿子的节点值之和
			if node.Left != nil {
				childrenSum += node.Left.Val
			}
			if node.Right != nil {
				childrenSum += node.Right.Val
			}
			// 更新 node 左右儿子的节点值
			if node.Left != nil {
				node.Left.Val = nextLevelSum - childrenSum
			}
			if node.Right != nil {
				node.Right.Val = nextLevelSum - childrenSum
			}
		}
	}
	return root
}
```

#### 复杂度分析

- 时间复杂度：$O(n)$，其中 $n$ 为二叉树的节点个数。
- 空间复杂度：$O(n)$。

[2023 下半年周赛题目总结](https://leetcode.cn/circle/discuss/lUu0KB/)

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
