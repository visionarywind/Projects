# 105. 从前序与中序遍历序列构造二叉树

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/construct-binary-tree-from-preorder-and-inorder-traversal/
- 题目 slug：`construct-binary-tree-from-preorder-and-inorder-traversal`
- 来源专题：链表、树与回溯
- 来源分类路径：二、二叉树 / §2.10 创建二叉树
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/construct-binary-tree-from-preorder-and-inorder-traversal/solutions/2646359/tu-jie-cong-on2-dao-onpythonjavacgojsrus-aob8/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【图解】从 O(n^2) 到 O(n)（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/construct-binary-tree-from-preorder-and-inorder-traversal/solutions/2646359/tu-jie-cong-on2-dao-onpythonjavacgojsrus-aob8/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tu-jie-cong-on2-dao-onpythonjavacgojsrus-aob8`
- topic id：`2646359`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:39:45 +0800

**前序遍历**：按照「根-左子树-右子树」的顺序遍历二叉树。

**中序遍历**：按照「左子树-根-右子树」的顺序遍历二叉树。

我们来看看示例 1 是怎么生成这棵二叉树的。

![lc105-c.png](https://pic.leetcode.cn/1707907886-ICkiSC-lc105-c.png)

**递归边界**：如果 $\textit{preorder}$ 的长度是 $0$，对应着空节点，返回空。

晕递归的同学推荐先看这期视频：[深入理解递归【基础算法精讲 09】](https://www.bilibili.com/video/BV1UD4y1Y769/)

## 写法一

```py [sol-Python3]
class Solution:
    def buildTree(self, preorder: List[int], inorder: List[int]) -> Optional[TreeNode]:
        if not preorder:  # 空节点
            return None
        left_size = inorder.index(preorder[0])  # 左子树的大小
        left = self.buildTree(preorder[1: 1 + left_size], inorder[:left_size])
        right = self.buildTree(preorder[1 + left_size:], inorder[1 + left_size:])
        return TreeNode(preorder[0], left, right)
```

```java [sol-Java]
class Solution {
    public TreeNode buildTree(int[] preorder, int[] inorder) {
        int n = preorder.length;
        if (n == 0) { // 空节点
            return null;
        }
        int leftSize = indexOf(inorder, preorder[0]); // 左子树的大小
        int[] pre1 = Arrays.copyOfRange(preorder, 1, 1 + leftSize);
        int[] pre2 = Arrays.copyOfRange(preorder, 1 + leftSize, n);
        int[] in1 = Arrays.copyOfRange(inorder, 0, leftSize);
        int[] in2 = Arrays.copyOfRange(inorder, 1 + leftSize, n);
        TreeNode left = buildTree(pre1, in1);
        TreeNode right = buildTree(pre2, in2);
        return new TreeNode(preorder[0], left, right);
    }

    // 返回 x 在 a 中的下标，保证 x 一定在 a 中
    private int indexOf(int[] a, int x) {
        for (int i = 0; ; i++) {
            if (a[i] == x) {
                return i;
            }
        }
    }
}
```

```cpp [sol-C++]
class Solution {
    // 用 span 可以避免拷贝，不用 span 的写法见【C++ 写法二】
    TreeNode* build(span<int> preorder, span<int> inorder) {
        if (preorder.empty()) { // 空节点
            return nullptr;
        }
        int left_size = ranges::find(inorder, preorder[0]) - inorder.begin(); // 左子树的大小
        TreeNode* left = build(preorder.subspan(1, left_size), inorder.subspan(0, left_size));
        TreeNode* right = build(preorder.subspan(1 + left_size), inorder.subspan(1 + left_size));
        return new TreeNode(preorder[0], left, right);
    }

public:
    TreeNode* buildTree(vector<int>& preorder, vector<int>& inorder) {
        return build(preorder, inorder);
    }
};
```

```cpp [sol-C++ 写法二]
class Solution {
public:
    TreeNode* buildTree(vector<int>& preorder, vector<int>& inorder) {
        if (preorder.empty()) { // 空节点
            return nullptr;
        }
        int left_size = ranges::find(inorder, preorder[0]) - inorder.begin(); // 左子树的大小
        vector<int> pre1(preorder.begin() + 1, preorder.begin() + 1 + left_size);
        vector<int> pre2(preorder.begin() + 1 + left_size, preorder.end());
        vector<int> in1(inorder.begin(), inorder.begin() + left_size);
        vector<int> in2(inorder.begin() + 1 + left_size, inorder.end());
        TreeNode* left = buildTree(pre1, in1);
        TreeNode* right = buildTree(pre2, in2);
        return new TreeNode(preorder[0], left, right);
    }
};
```

```go [sol-Go]
func buildTree(preorder, inorder []int) *TreeNode {
    n := len(preorder)
    if n == 0 { // 空节点
        return nil
    }
    leftSize := slices.Index(inorder, preorder[0]) // 左子树的大小
    left := buildTree(preorder[1:1+leftSize], inorder[:leftSize])
    right := buildTree(preorder[1+leftSize:], inorder[1+leftSize:])
    return &TreeNode{preorder[0], left, right}
}
```

```js [sol-JavaScript]
var buildTree = function(preorder, inorder) {
    const n = preorder.length;
    if (n === 0) { // 空节点
        return null;
    }
    const leftSize = inorder.indexOf(preorder[0]); // 左子树的大小
    const pre1 = preorder.slice(1, 1 + leftSize);
    const pre2 = preorder.slice(1 + leftSize);
    const in1 = inorder.slice(0, leftSize);
    const in2 = inorder.slice(1 + leftSize, n);
    const left = buildTree(pre1, in1);
    const right = buildTree(pre2, in2);
    return new TreeNode(preorder[0], left, right);
};
```

```rust [sol-Rust]
use std::rc::Rc;
use std::cell::RefCell;

impl Solution {
    pub fn build_tree(preorder: Vec<i32>, inorder: Vec<i32>) -> Option<Rc<RefCell<TreeNode>>> {
        if preorder.is_empty() { // 空节点
            return None;
        }
        let left_size = inorder.iter().position(|&x| x == preorder[0]).unwrap(); // 左子树的大小
        let pre1 = preorder[1..1 + left_size].to_vec();
        let pre2 = preorder[1 + left_size..].to_vec();
        let in1 = inorder[..left_size].to_vec();
        let in2 = inorder[1 + left_size..inorder.len()].to_vec();
        let left = Self::build_tree(pre1, in1);
        let right = Self::build_tree(pre2, in2);
        Some(Rc::new(RefCell::new(TreeNode { val: preorder[0], left, right })))
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2)$，其中 $n$ 为 $\textit{preorder}$ 的长度。最坏情况下二叉树是一条链，我们需要递归 $\mathcal{O}(n)$ 次，每次都需要 $\mathcal{O}(n)$ 的时间查找 $\textit{preorder}[0]$ 和复制数组。
- 空间复杂度：$\mathcal{O}(n^2)$。

## 写法二

上面的写法有两个优化点：

1. 用一个哈希表（或者数组）预处理 $\textit{inorder}$ 每个元素的下标，这样就可以 $\mathcal{O}(1)$ 查到 $\textit{preorder}[0]$ 在 $\textit{inorder}$ 的位置，从而 $\mathcal{O}(1)$ 知道左子树的大小。
2. 把递归参数改成子数组下标区间（**左闭右开区间**）的左右端点，从而避免复制数组。

```py [sol-Python3]
class Solution:
    def buildTree(self, preorder: List[int], inorder: List[int]) -> Optional[TreeNode]:
        index = {x: i for i, x in enumerate(inorder)}

        # 根据 preorder[pre_l:pre_r] 和 inorder[in_l:in_r] 生成二叉树，其中 in_r 没用到，可以省略
        def dfs(pre_l: int, pre_r: int, in_l: int) -> Optional[TreeNode]:
            if pre_l == pre_r:  # 空节点
                return None
            left_size = index[preorder[pre_l]] - in_l  # 左子树的大小
            left = dfs(pre_l + 1, pre_l + 1 + left_size, in_l)
            right = dfs(pre_l + 1 + left_size, pre_r, in_l + 1 + left_size)
            return TreeNode(preorder[pre_l], left, right)

        return dfs(0, len(preorder), 0)  # 左闭右开区间
```

```java [sol-Java]
class Solution {
    public TreeNode buildTree(int[] preorder, int[] inorder) {
        int n = preorder.length;
        Map<Integer, Integer> index = HashMap.newHashMap(n); // 预分配空间
        for (int i = 0; i < n; i++) {
            index.put(inorder[i], i);
        }
        return dfs(0, n, 0, preorder, index); // 左闭右开区间
    }

    // 根据 preorder 的子数组 [preL,preR) 和 inorder 的子数组 [inL,inR) 生成二叉树，其中 inR 没用到，可以省略
    private TreeNode dfs(int preL, int preR, int inL, int[] preorder, Map<Integer, Integer> index) {
        if (preL == preR) { // 空节点
            return null;
        }
        int leftSize = index.get(preorder[preL]) - inL; // 左子树的大小
        TreeNode left = dfs(preL + 1, preL + 1 + leftSize, inL, preorder, index);
        TreeNode right = dfs(preL + 1 + leftSize, preR, inL + 1 + leftSize, preorder, index);
        return new TreeNode(preorder[preL], left, right);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    TreeNode* buildTree(vector<int>& preorder, vector<int>& inorder) {
        int n = preorder.size();
        unordered_map<int, int> index;
        for (int i = 0; i < n; i++) {
            index[inorder[i]] = i;
        }

        // 根据 preorder 的子数组 [pre_l,pre_r) 和 inorder 的子数组 [in_l,in_r) 生成二叉树，其中 in_r 没用到，可以省略
        auto dfs = [&](this auto&& dfs, int pre_l, int pre_r, int in_l) -> TreeNode* {
            if (pre_l == pre_r) { // 空节点
                return nullptr;
            }
            int left_size = index[preorder[pre_l]] - in_l; // 左子树的大小
            TreeNode* left = dfs(pre_l + 1, pre_l + 1 + left_size, in_l);
            TreeNode* right = dfs(pre_l + 1 + left_size, pre_r, in_l + 1 + left_size);
            return new TreeNode(preorder[pre_l], left, right);
        };
        return dfs(0, n, 0); // 左闭右开区间
    }
};
```

```go [sol-Go]
func buildTree(preorder, inorder []int) *TreeNode {
    n := len(preorder)
    index := make(map[int]int, n)
    for i, x := range inorder {
        index[x] = i
    }

    // 根据 preorder[preL:preR] 和 inorder[inL:inR] 生成二叉树，其中 inR 没用到，可以省略
    var dfs func(int, int, int) *TreeNode
    dfs = func(preL, preR, inL int) *TreeNode {
        if preL == preR { // 空节点
            return nil
        }
        leftSize := index[preorder[preL]] - inL // 左子树的大小
        left := dfs(preL+1, preL+1+leftSize, inL)
        right := dfs(preL+1+leftSize, preR, inL+1+leftSize)
        return &TreeNode{preorder[preL], left, right}
    }
    return dfs(0, n, 0) // 左闭右开区间
}
```

```js [sol-JavaScript]
var buildTree = function(preorder, inorder) {
    const n = preorder.length;
    const index = new Map();
    for (let i = 0; i < n; i++) {
        index.set(inorder[i], i);
    }

    // 根据 preorder 的子数组 [preL,preR) 和 inorder 的子数组 [inL,inR) 生成二叉树，其中 inR 没用到，可以省略
    function dfs(preL, preR, inL) {
        if (preL === preR) { // 空节点
            return null;
        }
        const leftSize = index.get(preorder[preL]) - inL; // 左子树的大小
        const left = dfs(preL + 1, preL + 1 + leftSize, inL);
        const right = dfs(preL + 1 + leftSize, preR, inL + 1 + leftSize);
        return new TreeNode(preorder[preL], left, right);
    }

    return dfs(0, n, 0); // 左闭右开区间
};
```

```rust [sol-Rust]
use std::rc::Rc;
use std::cell::RefCell;
use std::collections::HashMap;

impl Solution {
    pub fn build_tree(preorder: Vec<i32>, inorder: Vec<i32>) -> Option<Rc<RefCell<TreeNode>>> {
        let n = preorder.len();
        let mut index = HashMap::with_capacity(n);
        for (i, &x) in inorder.iter().enumerate() {
            index.insert(x, i);
        }

        // 根据 preorder[pre_l..pre_r] 和 inorder[in_l..in_r] 生成二叉树，其中 in_r 没用到，可以省略
        fn dfs(pre_l: usize, pre_r: usize, in_l: usize, preorder: &[i32], index: &HashMap<i32, usize>) -> Option<Rc<RefCell<TreeNode>>> {
            if pre_l == pre_r {
                return None;
            }
            let left_size = index[&preorder[pre_l]] - in_l;
            let left = dfs(pre_l + 1, pre_l + 1 + left_size, in_l, preorder, index);
            let right = dfs(pre_l + 1 + left_size, pre_r, in_l + 1 + left_size, preorder, index);
            Some(Rc::new(RefCell::new(TreeNode { val: preorder[pre_l], left, right })))
        }
        dfs(0, n, 0, &preorder, &index)
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{preorder}$ 的长度。递归 $\mathcal{O}(n)$ 次，每次只需要 $\mathcal{O}(1)$ 的时间。
- 空间复杂度：$\mathcal{O}(n)$。

> 注：由于哈希表常数比数组大，实际运行效率可能不如写法一。

## 构造系列

这三题都可以用本文讲的套路解决。

- [105. 从前序与中序遍历序列构造二叉树](https://leetcode.cn/problems/construct-binary-tree-from-preorder-and-inorder-traversal/)
- [106. 从中序与后序遍历序列构造二叉树](https://leetcode.cn/problems/construct-binary-tree-from-inorder-and-postorder-traversal/)
- [889. 根据前序和后序遍历构造二叉树](https://leetcode.cn/problems/construct-binary-tree-from-preorder-and-postorder-traversal/)

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
