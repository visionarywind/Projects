# 1022. 从根到叶的二进制数之和

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/sum-of-root-to-leaf-binary-numbers/
- 题目 slug：`sum-of-root-to-leaf-binary-numbers`
- 来源专题：链表、树与回溯
- 来源分类路径：二、二叉树 / §2.2 自顶向下 DFS（先序遍历）
- 难度分：1462
- 外部题解来源：https://leetcode.cn/problems/sum-of-root-to-leaf-binary-numbers/solutions/3907197/liang-chong-dfs-xie-fa-pythonjavacgo-by-ljrze/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种 DFS 写法（Python/Java/C++/Go）](https://leetcode.cn/problems/sum-of-root-to-leaf-binary-numbers/solutions/3907197/liang-chong-dfs-xie-fa-pythonjavacgo-by-ljrze/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`liang-chong-dfs-xie-fa-pythonjavacgo-by-ljrze`
- topic id：`3907197`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:30:25 +0800

从大家熟悉的十进制说起。如何把路径 $1\to 2\to 3$ 变成十进制数 $123$？过程如下：

$$
0\xrightarrow{\times 10 + 1}1\xrightarrow{\times 10 + 2} 12\xrightarrow{\times 10 + 3}123
$$

二进制的做法类似。例如把路径 $1\to 0\to 1\to 1$ 变成二进制数 $1011$，过程如下：

$$
0\xrightarrow{\times 2 + 1}1\xrightarrow{\times 2 + 0} 10\xrightarrow{\times 2 + 1}101\xrightarrow{\times 2 + 1}1011
$$

其中 $\times 2$ 等价于左移一位，$+$ 也可以写成或运算。

我们可以对 $\textit{dfs}$ 额外添加参数 $\textit{num}$，表示在自顶向下递归的过程中，当前数字是 $\textit{num}$。每访问到一个新的节点 $\textit{node}$，就把 $\textit{num}$ 更新成 `num << 1 | node.val`。

如果 $\textit{node}$ 是叶子节点，把 $\textit{num}$ 加到答案中。

## 写法一

```py [sol-Python3]
class Solution:
    def sumRootToLeaf(self, root: Optional[TreeNode]) -> int:
        # 从根到 node（不含）的路径值为 num
        def dfs(node: Optional[TreeNode], num: int) -> None:
            nonlocal ans
            if node is None:
                return
            num = num << 1 | node.val
            if node.left is None and node.right is None:
                ans += num
                return
            dfs(node.left, num)
            dfs(node.right, num)

        ans = 0
        dfs(root, 0)
        return ans
```

```java [sol-Java]
class Solution {
    private int ans = 0;

    public int sumRootToLeaf(TreeNode root) {
        dfs(root, 0);
        return ans;
    }

    // 从根到 node（不含）的路径值为 num
    private void dfs(TreeNode node, int num) {
        if (node == null) {
            return;
        }
        num = num << 1 | node.val;
        if (node.left == null && node.right == null) {
            ans += num;
            return;
        }
        dfs(node.left, num);
        dfs(node.right, num);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int sumRootToLeaf(TreeNode* root) {
        int ans = 0;

        // 从根到 node（不含）的路径值为 num
        auto dfs = [&](this auto&& dfs, TreeNode* node, int num) -> void {
            if (node == nullptr) {
                return;
            }
            num = num << 1 | node->val;
            if (node->left == nullptr && node->right == nullptr) {
                ans += num;
                return;
            }
            dfs(node->left, num);
            dfs(node->right, num);
        };

        dfs(root, 0);
        return ans;
    }
};
```

```go [sol-Go]
func sumRootToLeaf(root *TreeNode) (ans int) {
	// 从根到 node（不含）的路径值为 num
	var dfs func(*TreeNode, int)
	dfs = func(node *TreeNode, num int) {
		if node == nil {
			return
		}
		num = num<<1 | node.Val
		if node.Left == nil && node.Right == nil {
			ans += num
			return
		}
		dfs(node.Left, num)
		dfs(node.Right, num)
	}
	dfs(root, 0)
	return
}
```

## 写法二

```py [sol-Python3]
class Solution:
    def sumRootToLeaf(self, root: Optional[TreeNode]) -> int:
        def dfs(node: Optional[TreeNode], num: int) -> int:
            if node is None:
                return 0
            num = num << 1 | node.val
            if node.left is None and node.right is None:
                return num
            return dfs(node.left, num) + dfs(node.right, num)

        return dfs(root, 0)
```

```java [sol-Java]
class Solution {
    public int sumRootToLeaf(TreeNode root) {
        return dfs(root, 0);
    }

    private int dfs(TreeNode node, int num) {
        if (node == null) {
            return 0;
        }
        num = num << 1 | node.val;
        if (node.left == null && node.right == null) {
            return num;
        }
        return dfs(node.left, num) + dfs(node.right, num);
    }
}
```

```cpp [sol-C++]
class Solution {
    int dfs(TreeNode* node, int num) {
        if (node == nullptr) {
            return 0;
        }
        num = num << 1 | node->val;
        if (node->left == nullptr && node->right == nullptr) {
            return num;
        }
        return dfs(node->left, num) + dfs(node->right, num);
    }

public:
    int sumRootToLeaf(TreeNode* root) {
        return dfs(root, 0);
    }
};
```

```go [sol-Go]
func dfs(node *TreeNode, num int) int {
	if node == nil {
		return 0
	}
	num = num<<1 | node.Val
	if node.Left == nil && node.Right == nil {
		return num
	}
	return dfs(node.Left, num) + dfs(node.Right, num)
}

func sumRootToLeaf(root *TreeNode) int {
	return dfs(root, 0)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是二叉树的节点个数。
- 空间复杂度：$\mathcal{O}(h)$，其中 $h$ 是二叉树的高度。递归需要 $\mathcal{O}(h)$ 的栈开销。

## 专题训练

见下面树题单的「**§2.2 自顶向下 DFS**」。

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

本题来自 `二、二叉树 / §2.2 自顶向下 DFS（先序遍历）`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、二叉树 / §2.2 自顶向下 DFS（先序遍历）`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
