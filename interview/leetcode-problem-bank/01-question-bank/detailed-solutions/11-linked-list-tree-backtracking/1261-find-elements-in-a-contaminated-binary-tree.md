# 1261. 在受污染的二叉树中查找元素

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/find-elements-in-a-contaminated-binary-tree/
- 题目 slug：`find-elements-in-a-contaminated-binary-tree`
- 来源专题：链表、树与回溯
- 来源分类路径：二、二叉树 / §2.16 其他
- 难度分：1440
- 外部题解来源：https://leetcode.cn/problems/find-elements-in-a-contaminated-binary-tree/solutions/2681672/liang-chong-fang-fa-ha-xi-biao-wei-yun-s-6m7w/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：哈希表/位运算（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/find-elements-in-a-contaminated-binary-tree/solutions/2681672/liang-chong-fang-fa-ha-xi-biao-wei-yun-s-6m7w/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`liang-chong-fang-fa-ha-xi-biao-wei-yun-s-6m7w`
- topic id：`2681672`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:49:15 +0800

## 方法一：哈希表

从 $\textit{root}$ 出发 DFS 这棵树，除了传入当前节点 $\textit{node}$，还传入需要还原的值 $\textit{val}$。

递归左儿子：传入 $\textit{val}\cdot 2 + 1$。

递归右儿子：传入 $\textit{val}\cdot 2 + 2$。

递归的同时，把还原后的节点值加到一个哈希表中。这样对于 $\texttt{find}$，只需要看 $\textit{target}$ 是否在哈希表中即可。

晕递归的同学推荐先看 [深入理解递归【基础算法精讲 09】](https://www.bilibili.com/video/BV1UD4y1Y769/)

```py [sol-Python3]
class FindElements:
    def __init__(self, root: Optional[TreeNode]):
        s = set()
        def dfs(node: Optional[TreeNode], val: int) -> None:
            if node is None:
                return
            s.add(val)
            dfs(node.left, val * 2 + 1)
            dfs(node.right, val * 2 + 2)
        dfs(root, 0)
        self.s = s

    def find(self, target: int) -> bool:
        return target in self.s
```

```java [sol-Java]
class FindElements {
    private final Set<Integer> s = new HashSet<>();

    public FindElements(TreeNode root) {
        dfs(root, 0);
    }

    public boolean find(int target) {
        return s.contains(target);
    }

    private void dfs(TreeNode node, int val) {
        if (node == null) {
            return;
        }
        s.add(val);
        dfs(node.left, val * 2 + 1);
        dfs(node.right, val * 2 + 2);
    }
}
```

```cpp [sol-C++]
class FindElements {
    unordered_set<int> s;

    void dfs(TreeNode *node, int val) {
        if (node == nullptr) {
            return;
        }
        s.insert(val);
        dfs(node->left, val * 2 + 1);
        dfs(node->right, val * 2 + 2);
    }

public:
    FindElements(TreeNode *root) {
        dfs(root, 0);
    }

    bool find(int target) {
        return s.contains(target);
    }
};
```

```go [sol-Go]
type FindElements map[int]bool

func Constructor(root *TreeNode) FindElements {
    f := FindElements{}
    var dfs func(*TreeNode, int)
    dfs = func(node *TreeNode, val int) {
        if node == nil {
            return
        }
        f[val] = true
        dfs(node.Left, val*2+1)
        dfs(node.Right, val*2+2)
    }
    dfs(root, 0)
    return f
}

func (f FindElements) Find(target int) bool {
    return f[target]
}
```

```js [sol-JavaScript]
class FindElements {
    constructor(root) {
        const s = new Set();
        function dfs(node, val) {
            if (node === null) {
                return;
            }
            s.add(val);
            dfs(node.left, val * 2 + 1);
            dfs(node.right, val * 2 + 2);
        }
        dfs(root, 0);

        this.find = function(target) {
            return s.has(target);
        }
    }
}
```

```rust [sol-Rust]
use std::rc::Rc;
use std::cell::RefCell;
use std::collections::HashSet;

struct FindElements {
    s: HashSet<i32>,
}

impl FindElements {
    fn new(root: Option<Rc<RefCell<TreeNode>>>) -> Self {
        fn dfs(node: Option<&Rc<RefCell<TreeNode>>>, val: i32, s: &mut HashSet<i32>) {
            if let Some(x) = node {
                s.insert(val);
                let x = x.borrow();
                dfs(x.left.as_ref(), val * 2 + 1, s);
                dfs(x.right.as_ref(), val * 2 + 2, s);
            }
        }
        let mut s = HashSet::new();
        dfs(root.as_ref(), 0, &mut s);
        Self { s }
    }

    fn find(&self, target: i32) -> bool {
        self.s.contains(&target)
    }
}
```

#### 复杂度分析

- 时间复杂度：初始化为 $\mathcal{O}(n)$，其中 $n$ 为二叉树的节点个数。$\texttt{find}$ 为 $\mathcal{O}(1)$。
- 空间复杂度：$\mathcal{O}(n)$。

## 方法二：二进制即路径

![LC1261.png](https://pic.leetcode.cn/1709868343-GQROpC-LC1261.png)

下面代码用到了一些位运算的技巧，请看 [从集合论到位运算，常见位运算技巧分类总结！](https://leetcode.cn/circle/discuss/CaOJ45/)

```py [sol-Python3]
class FindElements:
    def __init__(self, root: Optional[TreeNode]):
        self.root = root

    def find(self, target: int) -> bool:
        target += 1
        cur = self.root  # 从根节点出发
        for i in range(target.bit_length() - 2, -1, -1):  # 从次高位开始枚举
            bit = (target >> i) & 1  # target 第 i 位的比特值
            cur = cur.right if bit else cur.left
            if cur is None:  # 走到空节点，说明 target 不在二叉树中
                return False
        return True  # 没有走到空节点，说明 target 在二叉树中
```

```java [sol-Java]
class FindElements {
    private TreeNode root;

    public FindElements(TreeNode root) {
        this.root = root;
    }

    public boolean find(int target) {
        target++;
        TreeNode cur = root; // 从根节点出发
        for (int i = 30 - Integer.numberOfLeadingZeros(target); i >= 0; i--) { // 从次高位开始枚举
            int bit = (target >> i) & 1; // target 第 i 位的比特值
            cur = bit == 0 ? cur.left : cur.right;
            if (cur == null) { // 走到空节点，说明 target 不在二叉树中
                return false;
            }
        }
        return true; // 没有走到空节点，说明 target 在二叉树中
    }
}
```

```cpp [sol-C++]
class FindElements {
    TreeNode *root;
public:
    FindElements(TreeNode *root) : root(root) {}

    bool find(int target) {
        target++;
        auto cur = root; // 从根节点出发
        for (int i = 30 - __builtin_clz(target); i >= 0; i--) { // 从次高位开始枚举
            int bit = target >> i & 1; // target 第 i 位的比特值
            cur = bit ? cur->right : cur->left;
            if (cur == nullptr) { // 走到空节点，说明 target 不在二叉树中
                return false;
            }
        }
        return true; // 没有走到空节点，说明 target 在二叉树中
    }
};
```

```go [sol-Go]
type FindElements struct {
    root *TreeNode
}

func Constructor(root *TreeNode) FindElements {
    return FindElements{root}
}

func (f FindElements) Find(target int) bool {
    target++
    cur := f.root // 从根节点出发
    for i := bits.Len(uint(target)) - 2; i >= 0; i-- { // 从次高位开始枚举
        bit := target >> i & 1 // target 第 i 位的比特值
        if bit == 0 {
            cur = cur.Left
        } else {
            cur = cur.Right
        }
        if cur == nil { // 走到空节点，说明 target 不在二叉树中
            return false
        }
    }
    return true // 没有走到空节点，说明 target 在二叉树中
}
```

```js [sol-JavaScript]
class FindElements {
    constructor(root) {
        this.root = root;
    }

    find(target) {
        target++;
        let cur = this.root; // 从根节点出发
        for (let i = 30 - Math.clz32(target); i >= 0; i--) { // 从次高位开始枚举
            const bit = (target >> i) & 1; // target 第 i 位的比特值
            cur = bit ? cur.right : cur.left;
            if (cur === null) { // 走到空节点，说明 target 不在二叉树中
                return false;
            }
        }
        return true; // 没有走到空节点，说明 target 在二叉树中
    }
}
```

```rust [sol-Rust]
use std::rc::Rc;
use std::cell::RefCell;

struct FindElements {
    root: Option<Rc<RefCell<TreeNode>>>,
}

impl FindElements {
    fn new(root: Option<Rc<RefCell<TreeNode>>>) -> Self {
        Self { root }
    }

    fn find(&self, target: i32) -> bool {
        let mut target = target + 1;
        let mut cur = self.root.clone(); // 从根节点出发
        for i in (0..target.ilog2()).rev() { // 从次高位开始枚举
            let bit = (target >> i) & 1; // target 第 i 位的比特值
            cur = if bit == 0 {
                cur.unwrap().borrow().left.clone()
            } else {
                cur.unwrap().borrow().right.clone()
            };
            if cur.is_none() { // 走到空节点，说明 target 不在二叉树中
                return false;
            }
        }
        true // 没有走到空节点，说明 target 在二叉树中
    }
}
```

#### 复杂度分析

- 时间复杂度：初始化为 $\mathcal{O}(1)$。$\texttt{find}$ 为 $\mathcal{O}(\min(h, \log \textit{target}))$，其中 $h$ 为二叉树的高度。
- 空间复杂度：$\mathcal{O}(1)$。

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

[往期题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

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
