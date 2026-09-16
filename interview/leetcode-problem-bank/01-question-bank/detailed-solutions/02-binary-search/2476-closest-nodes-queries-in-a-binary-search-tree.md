# 2476. 二叉搜索树最近节点查询

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/closest-nodes-queries-in-a-binary-search-tree/
- 题目 slug：`closest-nodes-queries-in-a-binary-search-tree`
- 来源专题：二分算法
- 来源分类路径：一、二分查找 / §1.2 进阶
- 难度分：1597
- 外部题解来源：https://leetcode.cn/problems/closest-nodes-queries-in-a-binary-search-tree/solutions/2651916/zhong-xu-bian-li-er-fen-cha-zhao-pythonj-4ic0/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[中序遍历+二分查找（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/closest-nodes-queries-in-a-binary-search-tree/solutions/2651916/zhong-xu-bian-li-er-fen-cha-zhao-pythonj-4ic0/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`zhong-xu-bian-li-er-fen-cha-zhao-pythonj-4ic0`
- topic id：`2651916`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

首先，题目没说二叉搜索树是**平衡**的，最坏情况下这棵树是一条链，此时单次询问的复杂度是 $\mathcal{O}(n)$ 的，其中 $n$ 为二叉搜索树的节点个数。

为了加快回答询问的速度，可以通过一次 [94. 二叉树的中序遍历](https://leetcode.cn/problems/binary-tree-inorder-traversal/) 得到有一个**严格递增**数组 $a$，再在 $a$ 上做二分查找，就可以做到单次询问 $\mathcal{O}(\log n)$ 的时间了。

关于二分查找的原理，请看[【基础算法精讲 04】](https://www.bilibili.com/video/BV1AP41137w7/)。

设 $j$ 是大于等于 $q=\textit{queries}_i$ 的第一个数的下标，如果不存在则 $j=n$。

对于 $\textit{max}_i$：

- 如果 $j<n$，那么 $\textit{max}_i = a[j]$。
- 否则 $\textit{max}_i = -1$。

对于 $\textit{min}_i$：

- 如果 $j<n$ 且 $a[j] = q$，那么 $\textit{min}_i = a[j]$。
- 否则如果 $j>0$，那么 $\textit{min}_i = a[j-1]$。
- 否则 $\textit{min}_i = -1$。

```py [sol-Python3]
class Solution:
    def closestNodes(self, root: Optional[TreeNode], queries: List[int]) -> List[List[int]]:
        a = []
        def dfs(node: Optional[TreeNode]) -> None:
            if node is None:
                return
            dfs(node.left)
            a.append(node.val)
            dfs(node.right)
        dfs(root)

        n = len(a)
        ans = []
        for q in queries:
            j = bisect_left(a, q)
            mx = a[j] if j < n else -1
            if j == n or a[j] != q:  # a[j]>q, a[j-1]<q
                j -= 1
            mn = a[j] if j >= 0 else -1
            ans.append([mn, mx])
        return ans
```

```java [sol-Java]
class Solution {
    public List<List<Integer>> closestNodes(TreeNode root, List<Integer> queries) {
        List<Integer> arr = new ArrayList<>();
        dfs(root, arr);

        int n = arr.size();
        int[] a = new int[n];
        for (int i = 0; i < n; i++) {
            a[i] = arr.get(i); // 转成数组，效率更高
        }

        List<List<Integer>> ans = new ArrayList<>(queries.size()); // 预分配空间
        for (int q : queries) {
            int j = lowerBound(a, q);
            int mx = j == n ? -1 : a[j];
            if (j == n || a[j] != q) { // a[j]>q, a[j-1]<q
                j--;
            }
            int mn = j < 0 ? -1 : a[j];
            ans.add(List.of(mn, mx));
        }
        return ans;
    }

    private void dfs(TreeNode node, List<Integer> a) {
        if (node == null) {
            return;
        }
        dfs(node.left, a);
        a.add(node.val);
        dfs(node.right, a);
    }

    // 见 https://www.bilibili.com/video/BV1AP41137w7/
    private int lowerBound(int[] a, int target) {
        int left = -1, right = a.length; // 开区间 (left, right)
        while (left + 1 < right) { // 区间不为空
            int mid = (left + right) >>> 1; // 比 /2 快
            if (a[mid] >= target) {
                right = mid; // 范围缩小到 (left, mid)
            } else {
                left = mid; // 范围缩小到 (mid, right)
            }
        }
        return right;
    }
}
```

```cpp [sol-C++]
class Solution {
    vector<int> a;

    void dfs(TreeNode *node) {
        if (node == nullptr) {
            return;
        }
        dfs(node->left);
        a.push_back(node->val);
        dfs(node->right);
    };

public:
    vector<vector<int>> closestNodes(TreeNode *root, vector<int> &queries) {
        dfs(root);
        int n = a.size();
        vector<vector<int>> ans;
        for (int q : queries) {
            int j = ranges::lower_bound(a, q) - a.begin();
            int mx = j < n ? a[j] : -1;
            if (j == n || a[j] != q) { // a[j]>q, a[j-1]<q
                j--;
            }
            int mn = j >= 0 ? a[j] : -1;
            ans.push_back({mn, mx});
        }
        return ans;
    }
};
```

```go [sol-Go]
func closestNodes(root *TreeNode, queries []int) [][]int {
	a := []int{}
	var dfs func(*TreeNode)
	dfs = func(node *TreeNode) {
		if node == nil {
			return
		}
		dfs(node.Left)
		a = append(a, node.Val)
		dfs(node.Right)
	}
	dfs(root)

	ans := make([][]int, len(queries))
	for i, q := range queries {
		mn, mx := -1, -1
		j, ok := slices.BinarySearch(a, q)
		if j < len(a) {
			mx = a[j]
		}
		if !ok { // a[j]>q, a[j-1]<q
			j--
		}
		if j >= 0 {
			mn = a[j]
		}
		ans[i] = []int{mn, mx}
	}
	return ans
}
```

```js [sol-JavaScript]
var closestNodes = function(root, queries) {
    const a = [];
    function dfs(node) {
        if (node === null) {
            return;
        }
        dfs(node.left);
        a.push(node.val);
        dfs(node.right);
    }
    dfs(root);

    const n = a.length;
    const ans = [];
    for (const q of queries) {
        let j = lowerBound(a, q);
        const mx = j < n ? a[j] : -1;
        if (j === n || a[j] !== q) { // a[j]>q, a[j-1]<q
            j--;
        }
        const mn = j >= 0 ? a[j] : -1;
        ans.push([mn, mx]);
    }
    return ans;
};

// 见 https://www.bilibili.com/video/BV1AP41137w7/
var lowerBound = function(a, target) {
    let left = -1, right = a.length; // 开区间 (left, right)
    while (left + 1 < right) { // 区间不为空
        const mid = Math.floor((left + right) / 2);
        if (a[mid] >= target) {
            right = mid; // 范围缩小到 (left, mid)
        } else {
            left = mid; // 范围缩小到 (mid, right)
        }
    }
    return right;
}
```

```rust [sol-Rust]
use std::rc::Rc;
use std::cell::RefCell;

impl Solution {
    pub fn closest_nodes(root: Option<Rc<RefCell<TreeNode>>>, queries: Vec<i32>) -> Vec<Vec<i32>> {
        fn dfs(node: Option<&Rc<RefCell<TreeNode>>>, a: &mut Vec<i32>) {
            if let Some(x) = node {
                let x = x.borrow();
                dfs(x.left.as_ref(), a);
                a.push(x.val);
                dfs(x.right.as_ref(), a);
            }
        }
        let mut a = Vec::new();
        dfs(root.as_ref(), &mut a);

        let n = a.len();
        let mut ans = Vec::new();
        for q in queries {
            let mut j = a.partition_point(|&x| x < q);
            let mx = if j < n { a[j] } else { -1 };
            let mn = if j < n && a[j] == q { q } else if j > 0 { a[j - 1] } else { -1 };
            ans.push(vec![mn, mx]);
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n + q\log n)$，其中 $n$ 为二叉搜索树的节点个数，$q$ 为 $\textit{queries}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。返回值的空间不计入。

[【题单】二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)

## 本地原创解析

### 1. 题意重述

本题来自 `一、二分查找 / §1.2 进阶`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、二分查找 / §1.2 进阶`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
