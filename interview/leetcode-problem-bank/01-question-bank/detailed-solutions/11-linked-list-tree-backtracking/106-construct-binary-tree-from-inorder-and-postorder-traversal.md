# 106. 从中序与后序遍历序列构造二叉树

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/construct-binary-tree-from-inorder-and-postorder-traversal/
- 题目 slug：`construct-binary-tree-from-inorder-and-postorder-traversal`
- 来源专题：链表、树与回溯
- 来源分类路径：二、二叉树 / §2.10 创建二叉树
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/construct-binary-tree-from-inorder-and-postorder-traversal/solutions/2647794/tu-jie-cong-on2-dao-onpythonjavacgojsrus-w8ny/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【图解】从 O(n^2) 到 O(n)（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/construct-binary-tree-from-inorder-and-postorder-traversal/solutions/2647794/tu-jie-cong-on2-dao-onpythonjavacgojsrus-w8ny/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tu-jie-cong-on2-dao-onpythonjavacgojsrus-w8ny`
- topic id：`2647794`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:39:45 +0800

**中序遍历**：按照「左子树-根-右子树」的顺序遍历二叉树。

**后序遍历**：按照「左子树-右子树-根」的顺序遍历二叉树。

我们来看看示例 1 是怎么生成这棵二叉树的。

![LC106-c.png](https://pic.leetcode.cn/1708475214-fiBgmD-LC106-c.png)

**递归边界**：如果 $\textit{postorder}$ 的长度是 $0$（此时 $\textit{inorder}$ 的长度也是 $0$），对应着空节点，返回空。

晕递归的同学推荐先看这期视频：[深入理解递归【基础算法精讲 09】](https://www.bilibili.com/video/BV1UD4y1Y769/)

## 写法一

```py [sol-Python3]
class Solution:
    def buildTree(self, inorder: List[int], postorder: List[int]) -> Optional[TreeNode]:
        if not postorder:  # 空节点
            return None
        left_size = inorder.index(postorder[-1])  # 左子树的大小
        left = self.buildTree(inorder[:left_size], postorder[:left_size])
        right = self.buildTree(inorder[left_size + 1:], postorder[left_size: -1])
        return TreeNode(postorder[-1], left, right)
```

```java [sol-Java]
class Solution {
    public TreeNode buildTree(int[] inorder, int[] postorder) {
        int n = postorder.length;
        if (n == 0) { // 空节点
            return null;
        }
        int leftSize = indexOf(inorder, postorder[n - 1]); // 左子树的大小
        int[] in1 = Arrays.copyOfRange(inorder, 0, leftSize);
        int[] in2 = Arrays.copyOfRange(inorder, leftSize + 1, n);
        int[] post1 = Arrays.copyOfRange(postorder, 0, leftSize);
        int[] post2 = Arrays.copyOfRange(postorder, leftSize, n - 1);
        TreeNode left = buildTree(in1, post1);
        TreeNode right = buildTree(in2, post2);
        return new TreeNode(postorder[n - 1], left, right);
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
    TreeNode* build(span<int> inorder, span<int> postorder) {
        if (postorder.empty()) { // 空节点
            return nullptr;
        }
        int left_size = ranges::find(inorder, postorder.back()) - inorder.begin(); // 左子树的大小
        auto in1 = inorder.subspan(0, left_size);
        auto in2 = inorder.subspan(left_size + 1);
        auto post1 = postorder.subspan(0, left_size);
        auto post2 = postorder.subspan(left_size, postorder.size() - left_size - 1);
        TreeNode* left = build(in1, post1);
        TreeNode* right = build(in2, post2);
        return new TreeNode(postorder.back(), left, right);
    }

public:
    TreeNode* buildTree(vector<int>& inorder, vector<int>& postorder) {
        return build(inorder, postorder);
    }
};
```

```cpp [sol-C++ 写法二]
class Solution {
public:
    TreeNode* buildTree(vector<int>& inorder, vector<int>& postorder) {
        if (postorder.empty()) { // 空节点
            return nullptr;
        }
        int left_size = ranges::find(inorder, postorder.back()) - inorder.begin(); // 左子树的大小
        vector<int> in1(inorder.begin(), inorder.begin() + left_size);
        vector<int> in2(inorder.begin() + left_size + 1, inorder.end());
        vector<int> post1(postorder.begin(), postorder.begin() + left_size);
        vector<int> post2(postorder.begin() + left_size, postorder.end() - 1);
        TreeNode* left = buildTree(in1, post1);
        TreeNode* right = buildTree(in2, post2);
        return new TreeNode(postorder.back(), left, right);
    }
};
```

```go [sol-Go]
func buildTree(inorder, postorder []int) *TreeNode {
    n := len(postorder)
    if n == 0 { // 空节点
        return nil
    }
    leftSize := slices.Index(inorder, postorder[n-1]) // 左子树的大小
    left := buildTree(inorder[:leftSize], postorder[:leftSize])
    right := buildTree(inorder[leftSize+1:], postorder[leftSize:n-1])
    return &TreeNode{postorder[n-1], left, right}
}
```

```js [sol-JavaScript]
var buildTree = function(inorder, postorder) {
    const n = postorder.length;
    if (n === 0) { // 空节点
        return null;
    }
    const leftSize = inorder.indexOf(postorder[n - 1]); // 左子树的大小
    const in1 = inorder.slice(0, leftSize);
    const in2 = inorder.slice(leftSize + 1, n);
    const post1 = postorder.slice(0, leftSize);
    const post2 = postorder.slice(leftSize, n - 1);
    const left = buildTree(in1, post1);
    const right = buildTree(in2, post2);
    return new TreeNode(postorder[n - 1], left, right);
};
```

```rust [sol-Rust]
use std::rc::Rc;
use std::cell::RefCell;

impl Solution {
    pub fn build_tree(inorder: Vec<i32>, postorder: Vec<i32>) -> Option<Rc<RefCell<TreeNode>>> {
        if postorder.is_empty() { // 空节点
            return None;
        }
        let val = *postorder.last().unwrap();
        let left_size = inorder.iter().position(|&x| x == val).unwrap(); // 左子树的大小
        let in1 = inorder[..left_size].to_vec();
        let in2 = inorder[left_size + 1..].to_vec();
        let post1 = postorder[..left_size].to_vec();
        let post2 = postorder[left_size..postorder.len() - 1].to_vec();
        let left = Self::build_tree(in1, post1);
        let right = Self::build_tree(in2, post2);
        Some(Rc::new(RefCell::new(TreeNode { val, left, right })))
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2)$，其中 $n$ 为 $\textit{postorder}$ 的长度。最坏情况下二叉树是一条链，我们需要递归 $\mathcal{O}(n)$ 次，每次都需要 $\mathcal{O}(n)$ 的时间查找 $\textit{postorder}[n-1]$ 和复制数组。
- 空间复杂度：$\mathcal{O}(n^2)$。

## 写法二

上面的写法有两个优化点：

1. 用一个哈希表（或者数组）预处理 $\textit{inorder}$ 每个元素的下标，这样就可以 $\mathcal{O}(1)$ 查到 $\textit{postorder}[n-1]$ 在 $\textit{inorder}$ 的位置，从而 $\mathcal{O}(1)$ 知道左子树的大小。
2. 把递归参数改成子数组下标区间（**左闭右开区间**）的左右端点，从而避免复制数组。

```py [sol-Python3]
class Solution:
    def buildTree(self, inorder: List[int], postorder: List[int]) -> Optional[TreeNode]:
        index = {x: i for i, x in enumerate(inorder)}

        # 根据 inorder[in_l:in_r] 和 postorder[post_l:post_r] 生成二叉树，其中 in_r 没用到，可以省略
        def dfs(in_l: int, post_l: int, post_r: int) -> Optional[TreeNode]:
            if post_l == post_r:  # 空节点
                return None
            left_size = index[postorder[post_r - 1]] - in_l  # 左子树的大小
            left = dfs(in_l, post_l, post_l + left_size)
            right = dfs(in_l + left_size + 1, post_l + left_size, post_r - 1)
            return TreeNode(postorder[post_r - 1], left, right)

        return dfs(0, 0, len(postorder))  # 左闭右开区间
```

```java [sol-Java]
class Solution {
    public TreeNode buildTree(int[] inorder, int[] postorder) {
        int n = inorder.length;
        Map<Integer, Integer> index = HashMap.newHashMap(n); // 预分配空间
        for (int i = 0; i < n; i++) {
            index.put(inorder[i], i);
        }
        return dfs(0, 0, n, postorder, index); // 左闭右开区间
    }

    // 根据 inorder 的子数组 [inL,inR) 和 postorder 的子数组 [postL,postR) 生成二叉树，其中 inR 没用到，可以省略
    private TreeNode dfs(int inL, int postL, int postR, int[] postorder, Map<Integer, Integer> index) {
        if (postL == postR) { // 空节点
            return null;
        }
        int leftSize = index.get(postorder[postR - 1]) - inL; // 左子树的大小
        TreeNode left = dfs(inL, postL, postL + leftSize, postorder, index);
        TreeNode right = dfs(inL + leftSize + 1, postL + leftSize, postR - 1, postorder, index);
        return new TreeNode(postorder[postR - 1], left, right);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    TreeNode* buildTree(vector<int>& inorder, vector<int>& postorder) {
        int n = inorder.size();
        unordered_map<int, int> index;
        for (int i = 0; i < n; i++) {
            index[inorder[i]] = i;
        }

        // 根据 inorder 的子数组 [in_l,in_r) 和 postorder 的子数组 [post_l,post_r) 生成二叉树，其中 in_r 没用到，可以省略
        auto dfs = [&](this auto&& dfs, int in_l, int post_l, int post_r) -> TreeNode* {
            if (post_l == post_r) { // 空节点
                return nullptr;
            }
            int left_size = index[postorder[post_r - 1]] - in_l; // 左子树的大小
            TreeNode* left = dfs(in_l, post_l, post_l + left_size);
            TreeNode* right = dfs(in_l + left_size + 1, post_l + left_size, post_r - 1);
            return new TreeNode(postorder[post_r - 1], left, right);
        };
        return dfs(0, 0, n); // 左闭右开区间
    }
};
```

```go [sol-Go]
func buildTree(inorder, postorder []int) *TreeNode {
    n := len(inorder)
    index := make(map[int]int, n)
    for i, x := range inorder {
        index[x] = i
    }

    // 根据 inorder[inL:inR] 和 postorder[postL:postR] 生成二叉树，其中 inR 没用到，可以省略
    var dfs func(int, int, int) *TreeNode
    dfs = func(inL, postL, postR int) *TreeNode {
        if postL == postR { // 空节点
            return nil
        }
        leftSize := index[postorder[postR-1]] - inL // 左子树的大小
        left := dfs(inL, postL, postL+leftSize)
        right := dfs(inL+leftSize+1, postL+leftSize, postR-1)
        return &TreeNode{postorder[postR-1], left, right}
    }
    return dfs(0, 0, n) // 左闭右开区间
}
```

```js [sol-JavaScript]
var buildTree = function(inorder, postorder) {
    const n = inorder.length;
    const index = new Map();
    for (let i = 0; i < n; i++) {
        index.set(inorder[i], i);
    }

    // 根据 inorder 的子数组 [inL,inR) 和 postorder 的子数组 [postL,postR) 生成二叉树，其中 inR 没用到，可以省略
    function dfs(inL, postL, postR) {
        if (postL === postR) { // 空节点
            return null;
        }
        const leftSize = index.get(postorder[postR - 1]) - inL; // 左子树的大小
        const left = dfs(inL, postL, postL + leftSize);
        const right = dfs(inL + leftSize + 1, postL + leftSize, postR - 1);
        return new TreeNode(postorder[postR - 1], left, right);
    }
    return dfs(0, 0, n); // 左闭右开区间
};
```

```rust [sol-Rust]
use std::rc::Rc;
use std::cell::RefCell;
use std::collections::HashMap;

impl Solution {
    pub fn build_tree(inorder: Vec<i32>, postorder: Vec<i32>) -> Option<Rc<RefCell<TreeNode>>> {
        let n = inorder.len();
        let mut index = HashMap::with_capacity(n);
        for (i, &x) in inorder.iter().enumerate() {
            index.insert(x, i);
        }

        // 根据 inorder[in_l..in_r] 和 postorder[post_l..post_r] 生成二叉树，其中 in_r 没用到，可以省略
        fn dfs(in_l: usize, post_l: usize, post_r: usize, postorder: &[i32], index: &HashMap<i32, usize>) -> Option<Rc<RefCell<TreeNode>>> {
            if post_l == post_r { // 空节点
                return None;
            }
            let val = postorder[post_r - 1];
            let left_size = index[&val] - in_l;
            let left = dfs(in_l, post_l, post_l + left_size, postorder, index);
            let right = dfs(in_l + left_size + 1, post_l + left_size, post_r - 1, postorder, index);
            Some(Rc::new(RefCell::new(TreeNode { val, left, right })))
        }
        dfs(0, 0, n, &postorder, &index) // 左闭右开区间
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{inorder}$ 的长度。递归 $\mathcal{O}(n)$ 次，每次只需要 $\mathcal{O}(1)$ 的时间。
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
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. 【本题相关】[链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
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
