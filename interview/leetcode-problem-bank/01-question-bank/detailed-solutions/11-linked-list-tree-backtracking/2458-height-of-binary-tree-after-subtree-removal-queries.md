# 2458. 移除子树后的二叉树高度

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/height-of-binary-tree-after-subtree-removal-queries/
- 题目 slug：`height-of-binary-tree-after-subtree-removal-queries`
- 来源专题：链表、树与回溯
- 来源分类路径：二、二叉树 / §2.16 其他
- 难度分：2299
- 外部题解来源：https://leetcode.cn/problems/height-of-binary-tree-after-subtree-removal-queries/solutions/1936338/liang-bian-dfspythonjavacgo-by-endlessch-vvs4/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两遍 DFS（Python/Java/C++/Go）](https://leetcode.cn/problems/height-of-binary-tree-after-subtree-removal-queries/solutions/1936338/liang-bian-dfspythonjavacgo-by-endlessch-vvs4/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`liang-bian-dfspythonjavacgo-by-endlessch-vvs4`
- topic id：`1936338`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:49:15 +0800

[视频讲解](https://www.bilibili.com/video/BV1Em4y1c7Hc) 已出炉，欢迎点赞三连，在评论区分享你对这场周赛的看法~

---

既然是求树的高度，我们可以先跑一遍 DFS，求出每棵子树的高度 $\textit{height}$（这里定义成最长路径的**节点数**）。

然后再 DFS 一遍这棵树，同时维护当前节点深度 $\textit{depth}$（从 $0$ 开始），以及删除当前子树后剩余部分的树的高度 $\textit{restH}$（这里定义成最长路径的**边数**）。

具体做法如下：

- 往左走，递归前算一下从根节点到当前节点右子树最深节点的长度，即 $\textit{depth} + \textit{height}[\textit{node}.\textit{right}]$，与 $\textit{restH}$ 取最大值，然后往下递归；
- 往右走，递归前算一下从根节点到当前节点左子树最深节点的长度，即 $\textit{depth} + \textit{height}[\textit{node}.\textit{left}]$，与 $\textit{restH}$ 取最大值，然后往下递归。

每个节点的答案即为递归到该节点时的 $\textit{restH}$ 值。

代码实现时可以直接把答案覆盖到 $\textit{queries}$ 数组中。

```py [sol1-Python3]
class Solution:
    def treeQueries(self, root: Optional[TreeNode], queries: List[int]) -> List[int]:
        height = defaultdict(int)  # 每棵子树的高度
        def get_height(node: Optional[TreeNode]) -> int:
            if node is None: return 0
            height[node] = 1 + max(get_height(node.left), get_height(node.right))
            return height[node]
        get_height(root)

        res = [0] * (len(height) + 1)  # 每个节点的答案
        def dfs(node: Optional[TreeNode], depth: int, rest_h: int) -> None:
            if node is None: return
            depth += 1
            res[node.val] = rest_h
            dfs(node.left, depth, max(rest_h, depth + height[node.right]))
            dfs(node.right, depth, max(rest_h, depth + height[node.left]))
        dfs(root, -1, 0)

        for i, q in enumerate(queries):
            queries[i] = res[q]
        return queries
```

```java [sol1-Java]
class Solution {
    private Map<TreeNode, Integer> height = new HashMap<>(); // 每棵子树的高度
    private int[] res; // 每个节点的答案

    public int[] treeQueries(TreeNode root, int[] queries) {
        getHeight(root);
        height.put(null, 0); // 简化 dfs 的代码，这样不用写 getOrDefault
        res = new int[height.size()];
        dfs(root, -1, 0);
        for (var i = 0; i < queries.length; i++)
            queries[i] = res[queries[i]];
        return queries;
    }

    private int getHeight(TreeNode node) {
        if (node == null) return 0;
        var h = 1 + Math.max(getHeight(node.left), getHeight(node.right));
        height.put(node, h);
        return h;
    }

    private void dfs(TreeNode node, int depth, int restH) {
        if (node == null) return;
        ++depth;
        res[node.val] = restH;
        dfs(node.left, depth, Math.max(restH, depth + height.get(node.right)));
        dfs(node.right, depth, Math.max(restH, depth + height.get(node.left)));
    }
}
```

```cpp [sol1-C++]
class Solution {
public:
    vector<int> treeQueries(TreeNode *root, vector<int> &queries) {
        unordered_map<TreeNode*, int> height; // 每棵子树的高度
        function<int(TreeNode*)> get_height = [&](TreeNode *node) -> int {
            return node ? height[node] = 1 + max(get_height(node->left), get_height(node->right)) : 0;
        };
        get_height(root);

        int res[height.size() + 1]; // 每个节点的答案
        function<void(TreeNode*, int, int)> dfs = [&](TreeNode *node, int depth, int rest_h) {
            if (node == nullptr) return;
            ++depth;
            res[node->val] = rest_h;
            dfs(node->left, depth, max(rest_h, depth + height[node->right]));
            dfs(node->right, depth, max(rest_h, depth + height[node->left]));
        };
        dfs(root, -1, 0);

        for (auto &q : queries) q = res[q];
        return queries;
    }
};
```

```go [sol1-Go]
func treeQueries(root *TreeNode, queries []int) []int {
	height := map[*TreeNode]int{} // 每棵子树的高度
	var getHeight func(*TreeNode) int
	getHeight = func(node *TreeNode) int {
		if node == nil {
			return 0
		}
		height[node] = 1 + max(getHeight(node.Left), getHeight(node.Right))
		return height[node]
	}
	getHeight(root)

	res := make([]int, len(height)+1) // 每个节点的答案
	var dfs func(*TreeNode, int, int)
	dfs = func(node *TreeNode, depth, restH int) {
		if node == nil {
			return
		}
		depth++
		res[node.Val] = restH
		dfs(node.Left, depth, max(restH, depth+height[node.Right]))
		dfs(node.Right, depth, max(restH, depth+height[node.Left]))
	}
	dfs(root, -1, 0)

	for i, q := range queries {
		queries[i] = res[q]
	}
	return queries
}

func max(a, b int) int { if b > a { return b }; return a }
```

#### 复杂度分析

- 时间复杂度：$O(n)$，其中 $n$ 为二叉树的节点个数。
- 空间复杂度：$O(n)$。

## 本地原创解析

### 1. 题意重述

本题来自 `二、二叉树 / §2.16 其他`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、二叉树 / §2.16 其他`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
