# 2196. 根据描述创建二叉树

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/create-binary-tree-from-descriptions/
- 题目 slug：`create-binary-tree-from-descriptions`
- 来源专题：链表、树与回溯
- 来源分类路径：二、二叉树 / §2.10 创建二叉树
- 难度分：1644
- 外部题解来源：https://leetcode.cn/problems/create-binary-tree-from-descriptions/solutions/1314483/jian-tu-dfs-by-endlesscheng-vl17/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[哈希表建树 + 位运算优化找根（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/create-binary-tree-from-descriptions/solutions/1314483/jian-tu-dfs-by-endlesscheng-vl17/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`jian-tu-dfs-by-endlesscheng-vl17`
- topic id：`1314483`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:39:45 +0800

题目输入的是节点整数值，不是节点。我们需要手动创建 $\texttt{TreeNode}$ 节点。

比如有两条边 $50\to 20$ 和 $20\to 15$。创建 $50$ 和 $20$ 的 $\texttt{TreeNode}$ 节点后，对于 $20\to 15$，由于 $20$ 的节点之前创建过，我们需要知道整数 $20$ 对应的 $\texttt{TreeNode}$ 节点是什么，所以需要一个从整数（节点值）映射到 $\texttt{TreeNode}$ 节点的哈希表 $\textit{nodes}$。

最后，我们需要返回二叉树的根节点。怎么判断谁是二叉树的根？

用一个哈希集合 $\textit{children}$ 记录哪些节点值有父节点。遍历哈希表 $\textit{nodes}$，不在 $\textit{children}$ 中的节点即为根节点。

## 优化前

```py [sol-Python3]
class Solution:
    def createBinaryTree(self, descriptions: List[List[int]]) -> Optional[TreeNode]:
        nodes = {}  # val -> TreeNode
        children = set()

        # 建树
        for x, y, is_left in descriptions:
            if x not in nodes:
                nodes[x] = TreeNode(x)
            if y not in nodes:
                nodes[y] = TreeNode(y)
            if is_left:
                nodes[x].left = nodes[y]
            else:
                nodes[x].right = nodes[y]
            children.add(y)  # y 不是根节点

        for x, node in nodes.items():
            if x not in children:  # node 是根节点
                return node
        # 测试用例保证可以构造出有效的二叉树
```

```java [sol-Java]
class Solution {
    public TreeNode createBinaryTree(int[][] descriptions) {
        int n = descriptions.length;
        Map<Integer, TreeNode> nodes = new HashMap<>(n + 1, 1); // 预分配空间
        Set<Integer> children = new HashSet<>(n, 1);

        // 建树
        for (int[] d : descriptions) {
            int x = d[0], y = d[1];
            nodes.computeIfAbsent(x, _ -> new TreeNode(x));
            nodes.computeIfAbsent(y, _ -> new TreeNode(y));
            if (d[2] == 1) {
                nodes.get(x).left = nodes.get(y);
            } else {
                nodes.get(x).right = nodes.get(y);
            }
            children.add(y); // y 不是根节点
        }

        for (Map.Entry<Integer, TreeNode> e : nodes.entrySet()) {
            if (!children.contains(e.getKey())) { // e.getValue() 是根节点
                return e.getValue();
            }
        }

        // 测试用例保证可以构造出有效的二叉树
        throw new IllegalArgumentException("descriptions is not a valid binary tree");
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    TreeNode* createBinaryTree(vector<vector<int>>& descriptions) {
        int n = descriptions.size();
        unordered_map<int, TreeNode*> nodes;
        nodes.reserve(n + 1); // 预分配空间
        unordered_set<int> children;
        children.reserve(n); // 预分配空间

        // 建树
        for (auto& d : descriptions) {
            int x = d[0], y = d[1];
            if (!nodes.contains(x)) {
                nodes[x] = new TreeNode(x);
            }
            if (!nodes.contains(y)) {
                nodes[y] = new TreeNode(y);
            }
            if (d[2]) {
                nodes[x]->left = nodes[y];
            } else {
                nodes[x]->right = nodes[y];
            }
            children.insert(y); // y 不是根节点
        }

        for (auto& [x, node] : nodes) {
            if (!children.contains(x)) { // node 是根节点
                return node;
            }
        }

        // 测试用例保证可以构造出有效的二叉树
        throw invalid_argument("descriptions is not a valid binary tree");
    }
};
```

```go [sol-Go]
func createBinaryTree(descriptions [][]int) *TreeNode {
	n := len(descriptions)
	nodes := make(map[int]*TreeNode, n+1) // 预分配空间
	children := make(map[int]bool, n)

	// 建树
	for _, d := range descriptions {
		x, y := d[0], d[1]
		if nodes[x] == nil {
			nodes[x] = &TreeNode{Val: x}
		}
		if nodes[y] == nil {
			nodes[y] = &TreeNode{Val: y}
		}
		if d[2] == 1 {
			nodes[x].Left = nodes[y]
		} else {
			nodes[x].Right = nodes[y]
		}
		children[y] = true // y 不是根节点
	}

	for x, node := range nodes {
		if !children[x] { // node 是根节点
			return node
		}
	}

	// 测试用例保证可以构造出有效的二叉树
	panic("descriptions is not a valid binary tree")
}
```

```js [sol-JavaScript]
var createBinaryTree = function(descriptions) {
    const nodes = new Map();
    const children = new Set();

    // 建树
    for (const [x, y, isLeft] of descriptions) {
        if (!nodes.has(x)) {
            nodes.set(x, new TreeNode(x));
        }
        if (!nodes.has(y)) {
            nodes.set(y, new TreeNode(y));
        }
        if (isLeft) {
            nodes.get(x).left = nodes.get(y);
        } else {
            nodes.get(x).right = nodes.get(y);
        }
        children.add(y); // y 不是根节点
    }

    for (const [x, node] of nodes) {
        if (!children.has(x)) { // node 是根节点
            return node;
        }
    }

    // 测试用例保证可以构造出有效的二叉树
    throw new Error("descriptions is not a valid binary tree");
};
```

```rust [sol-Rust]
use std::cell::RefCell;
use std::collections::{HashMap, HashSet};
use std::rc::Rc;

impl Solution {
    pub fn create_binary_tree(descriptions: Vec<Vec<i32>>) -> Option<Rc<RefCell<TreeNode>>> {
        let n = descriptions.len();
        let mut nodes = HashMap::with_capacity(n + 1); // 预分配空间
        let mut children = HashSet::with_capacity(n);

        // 建树
        for d in descriptions {
            let x = d[0];
            let y = d[1];
            nodes.entry(x).or_insert_with(|| Rc::new(RefCell::new(TreeNode::new(x))));
            nodes.entry(y).or_insert_with(|| Rc::new(RefCell::new(TreeNode::new(y))));
            if d[2] == 1 {
                nodes.get(&x)?.borrow_mut().left = nodes.get(&y).cloned();
            } else {
                nodes.get(&x)?.borrow_mut().right = nodes.get(&y).cloned();
            }
            children.insert(y); // y 不是根节点
        }

        for (x, node) in nodes {
            if !children.contains(&x) { // node 是根节点
                return Some(node);
            }
        }

        // 测试用例保证可以构造出有效的二叉树
        unreachable!()
    }
}
```

## 优化

把树上每个值异或一遍，再把所有儿子 $child_i$ 异或一遍。由于同一个值异或两次等于 $0$，所以最终异或和恰好等于根节点的值。

这个技巧来自 [136. 只出现一次的数字](https://leetcode.cn/problems/single-number/)，请看 [我的题解](https://leetcode.cn/problems/single-number/solutions/2481594/li-yong-yi-huo-de-xing-zhi-fu-ti-dan-pyt-oizc/)。

```py [sol-Python3]
class Solution:
    def createBinaryTree(self, descriptions: List[List[int]]) -> Optional[TreeNode]:
        nodes = {}  # val -> TreeNode
        root = 0

        for x, y, is_left in descriptions:
            if x not in nodes:
                nodes[x] = TreeNode(x)
                root ^= x
            if y not in nodes:
                nodes[y] = TreeNode(y)
                root ^= y
            if is_left:
                nodes[x].left = nodes[y]
            else:
                nodes[x].right = nodes[y]
            root ^= y

        return nodes[root]
```

```java [sol-Java]
class Solution {
    public TreeNode createBinaryTree(int[][] descriptions) {
        Map<Integer, TreeNode> nodes = new HashMap<>(descriptions.length + 1, 1); // 预分配空间
        int root = 0;

        for (int[] d : descriptions) {
            int x = d[0], y = d[1];
            if (!nodes.containsKey(x)) {
                nodes.put(x, new TreeNode(x));
                root ^= x;
            }
            if (!nodes.containsKey(y)) {
                nodes.put(y, new TreeNode(y));
                root ^= y;
            }
            if (d[2] == 1) {
                nodes.get(x).left = nodes.get(y);
            } else {
                nodes.get(x).right = nodes.get(y);
            }
            root ^= y;
        }

        return nodes.get(root);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    TreeNode* createBinaryTree(vector<vector<int>>& descriptions) {
        unordered_map<int, TreeNode*> nodes;
        nodes.reserve(descriptions.size() + 1); // 预分配空间
        int root = 0;

        for (auto& d : descriptions) {
            int x = d[0], y = d[1];
            if (!nodes.contains(x)) {
                nodes[x] = new TreeNode(x);
                root ^= x;
            }
            if (!nodes.contains(y)) {
                nodes[y] = new TreeNode(y);
                root ^= y;
            }
            if (d[2]) {
                nodes[x]->left = nodes[y];
            } else {
                nodes[x]->right = nodes[y];
            }
            root ^= y;
        }

        return nodes[root];
    }
};
```

```go [sol-Go]
func createBinaryTree(descriptions [][]int) *TreeNode {
	nodes := make(map[int]*TreeNode, len(descriptions)+1) // 预分配空间
	root := 0

	for _, d := range descriptions {
		x, y := d[0], d[1]
		if nodes[x] == nil {
			nodes[x] = &TreeNode{Val: x}
			root ^= x
		}
		if nodes[y] == nil {
			nodes[y] = &TreeNode{Val: y}
			root ^= y
		}
		if d[2] == 1 {
			nodes[x].Left = nodes[y]
		} else {
			nodes[x].Right = nodes[y]
		}
		root ^= y
	}

	return nodes[root]
}
```

```js [sol-JavaScript]
var createBinaryTree = function(descriptions) {
    const nodes = new Map();
    let root = 0;

    for (const [x, y, isLeft] of descriptions) {
        if (!nodes.has(x)) {
            nodes.set(x, new TreeNode(x));
            root ^= x;
        }
        if (!nodes.has(y)) {
            nodes.set(y, new TreeNode(y));
            root ^= y;
        }
        if (isLeft) {
            nodes.get(x).left = nodes.get(y);
        } else {
            nodes.get(x).right = nodes.get(y);
        }
        root ^= y;
    }

    return nodes.get(root);
};
```

```rust [sol-Rust]
use std::cell::RefCell;
use std::collections::HashMap;
use std::rc::Rc;

impl Solution {
    pub fn create_binary_tree(descriptions: Vec<Vec<i32>>) -> Option<Rc<RefCell<TreeNode>>> {
        let mut nodes = HashMap::with_capacity(descriptions.len() + 1); // 预分配空间
        let mut root = 0;

        for d in descriptions {
            let x = d[0];
            let y = d[1];
            nodes.entry(x).or_insert_with(|| { root ^= x; Rc::new(RefCell::new(TreeNode::new(x))) });
            nodes.entry(y).or_insert_with(|| { root ^= y; Rc::new(RefCell::new(TreeNode::new(y))) });
            if d[2] == 1 {
                nodes.get(&x)?.borrow_mut().left = nodes.get(&y).cloned();
            } else {
                nodes.get(&x)?.borrow_mut().right = nodes.get(&y).cloned();
            }
            root ^= y;
        }

        nodes.remove(&root)
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{descriptions}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 专题训练

见下面树题单的「**§2.10 创建二叉树**」。

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

本题来自 `二、二叉树 / §2.10 创建二叉树`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、二叉树 / §2.10 创建二叉树`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
