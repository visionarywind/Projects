# 206. 反转链表

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/reverse-linked-list/
- 题目 slug：`reverse-linked-list`
- 来源专题：链表、树与回溯
- 来源分类路径：一、链表 / §1.4 反转链表
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/reverse-linked-list/solutions/1992225/you-xie-cuo-liao-yi-ge-shi-pin-jiang-tou-o5zy/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：递归 / 迭代，本质是尾插法和头插法（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/reverse-linked-list/solutions/1992225/you-xie-cuo-liao-yi-ge-shi-pin-jiang-tou-o5zy/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`you-xie-cuo-liao-yi-ge-shi-pin-jiang-tou-o5zy`
- topic id：`1992225`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:30:25 +0800

## 方法一：递归（尾插法）

递归递归，有递有归。

我们先「递」到链表的末尾节点，作为新链表的头节点。然后在「归」的过程中，一个一个地把节点插在新链表的末尾。

新链表的末尾节点在哪？就是当前节点的 $\textit{next}$。具体实现如下。

```py [sol-Python3]
class Solution:
    # 首先「递」到链表末尾，把末尾节点作为新链表的头节点 rev_head
    # 然后在「归」的过程中，把经过的节点依次插在新链表的末尾（尾插法）
    def reverseList(self, head: Optional[ListNode]) -> Optional[ListNode]:
        # 判断 head is None 是为了兼容一开始链表就是空的情况
        if head is None or head.next is None:
            return head  # 链表末尾，即下面的 rev_head
        rev_head = self.reverseList(head.next)  # 「递」到链表末尾，拿到新链表的头节点
        tail = head.next  # 在「归」的过程中，head.next 就是新链表的末尾
        tail.next = head  # 把 head 插在新链表的末尾
        head.next = None  # 如果不写这行，新链表的末尾两个节点成环，这俩节点互相指向对方
        return rev_head
```

```java [sol-Java]
class Solution {
    // 首先「递」到链表末尾，把末尾节点作为新链表的头节点 revHead
    // 然后在「归」的过程中，把经过的节点依次插在新链表的末尾（尾插法）
    public ListNode reverseList(ListNode head) {
        // 判断 head == null 是为了兼容一开始链表就是空的情况
        if (head == null || head.next == null) {
            return head; // 链表末尾，即下面的 revHead
        }
        ListNode revHead = reverseList(head.next); // 「递」到链表末尾，拿到新链表的头节点
        ListNode tail = head.next; // 在「归」的过程中，head.next 就是新链表的末尾
        tail.next = head; // 把 head 插在新链表的末尾
        head.next = null; // 如果不写这行，新链表的末尾两个节点成环，这俩节点互相指向对方
        return revHead;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    // 首先「递」到链表末尾，把末尾节点作为新链表的头节点 rev_head
    // 然后在「归」的过程中，把经过的节点依次插在新链表的末尾（尾插法）
    ListNode* reverseList(ListNode* head) {
        // 判断 head == nullptr 是为了兼容一开始链表就是空的情况
        if (head == nullptr || head->next == nullptr) {
            return head; // 链表末尾，即下面的 rev_head
        }
        auto rev_head = reverseList(head->next); // 「递」到链表末尾，拿到新链表的头节点
        ListNode* tail = head->next; // 在「归」的过程中，head->next 就是新链表的末尾
        tail->next = head; // 把 head 插在新链表的末尾
        head->next = nullptr; // 如果不写这行，新链表的末尾两个节点成环，这俩节点互相指向对方
        return rev_head;
    }
};
```

```c [sol-C]
// 首先「递」到链表末尾，把末尾节点作为新链表的头节点 rev_head
// 然后在「归」的过程中，把经过的节点依次插在新链表的末尾（尾插法）
struct ListNode* reverseList(struct ListNode* head) {
    // 判断 head == NULL 是为了兼容一开始链表就是空的情况
    if (head == NULL || head->next == NULL) {
        return head; // 链表末尾，即下面的 rev_head
    }
    struct ListNode* rev_head = reverseList(head->next); // 「递」到链表末尾，拿到新链表的头节点
    struct ListNode* tail = head->next; // 在「归」的过程中，head->next 就是新链表的末尾
    tail->next = head; // 把 head 插在新链表的末尾
    head->next = NULL; // 如果不写这行，新链表的末尾两个节点成环，这俩节点互相指向对方
    return rev_head;
}
```

```go [sol-Go]
// 首先「递」到链表末尾，把末尾节点作为新链表的头节点 revHead
// 然后在「归」的过程中，把经过的节点依次插在新链表的末尾（尾插法）
func reverseList(head *ListNode) *ListNode {
    // 判断 head == nil 是为了兼容一开始链表就是空的情况
    if head == nil || head.Next == nil { 
        return head // 链表末尾，即下面的 revHead
    }
    revHead := reverseList(head.Next) // 「递」到链表末尾，拿到新链表的头节点
    tail := head.Next // 在「归」的过程中，head.Next 就是新链表的末尾
    tail.Next = head // 把 head 插在新链表的末尾
    head.Next = nil // 如果不写这行，新链表的末尾两个节点成环，这俩节点互相指向对方
    return revHead
}
```

```js [sol-JavaScript]
// 首先「递」到链表末尾，把末尾节点作为新链表的头节点 revHead
// 然后在「归」的过程中，把经过的节点依次插在新链表的末尾（尾插法）
var reverseList = function(head) {
    // 判断 head === null 是为了兼容一开始链表就是空的情况
    if (head === null || head.next === null) {
        return head; // 链表末尾，即下面的 revHead
    }
    const revHead = reverseList(head.next); // 「递」到链表末尾，拿到新链表的头节点
    const tail = head.next; // 在「归」的过程中，head.next 就是新链表的末尾
    tail.next = head; // 把 head 插在新链表的末尾
    head.next = null; // 如果不写这行，新链表的末尾两个节点成环，这俩节点互相指向对方
    return revHead;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn reverse_list(head: Option<Box<ListNode>>) -> Option<Box<ListNode>> {
        fn reverse(node: Option<Box<ListNode>>, pre: Option<Box<ListNode>>) -> Option<Box<ListNode>> {
            if let Some(mut cur) = node {
                let nxt = cur.next.take();
                cur.next = pre;
                return reverse(nxt, Some(cur));
            }
            pre
        }
        reverse(head, None)
    }
}
```

#### 答疑

**问**：为什么不写 `head.next = null` 的代码，会提示「超出内存限制」？这应该是超时呀？

**答**：这和力扣的判题机制有关，评测机会先把链表转成字符串，再去比对答案。这会遍历链表，如果链表有环，生成的字符串会无限延长，在超时之前就超出内存限制了。

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为链表节点个数。
- 空间复杂度：$\mathcal{O}(n)$。递归需要 $\mathcal{O}(n)$ 的栈空间。

## 方法二：迭代（头插法）

视频讲解：[【基础算法精讲 06】](https://www.bilibili.com/video/BV1sd4y1x7KN/)，制作不易，欢迎点赞~

**简单理解**：比如链表为 $1\to 2\to 3$。创建一个新的空链表，然后用**头插法**依次把节点 $1,2,3$ 插到这个新链表的头部，就得到了链表 $3\to 2\to 1$，这正是反转后的链表。

头插法的意思是，把一个节点 $\textit{node}$ 指向链表头节点（$\textit{node}.\textit{next}$ 更新为链表头节点），那么 $\textit{node}$ 就插在了链表的左侧，新链表的头节点为 $\textit{node}$。

对于链表 $1\to 2\to 3$，结合代码来说，顺序为：

- 第一轮循环结束后，得到链表 $1$。
- 第二轮循环结束后，得到链表 $2\to 1$。
- 第三轮循环结束后，得到链表 $3\to 2\to 1$。

> **注**：代码每轮循环结束后，$\textit{pre}$ 表示最新得到的链表。

```py [sol-Python3]
class Solution:
    def reverseList(self, head: Optional[ListNode]) -> Optional[ListNode]:
        pre = None
        cur = head
        while cur:
            nxt = cur.next
            cur.next = pre  # 把 cur 插在 pre 链表的前面（头插法）
            pre = cur
            cur = nxt
        return pre
```

```java [sol-Java]
class Solution {
    public ListNode reverseList(ListNode head) {
        ListNode pre = null;
        ListNode cur = head;
        while (cur != null) {
            ListNode nxt = cur.next;
            cur.next = pre; // 把 cur 插在 pre 链表的前面（头插法）
            pre = cur;
            cur = nxt;
        }
        return pre;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    ListNode* reverseList(ListNode* head) {
        ListNode* pre = nullptr;
        ListNode* cur = head;
        while (cur) {
            ListNode* nxt = cur->next;
            cur->next = pre; // 把 cur 插在 pre 链表的前面（头插法）
            pre = cur;
            cur = nxt;
        }
        return pre;
    }
};
```

```c [sol-C]
struct ListNode* reverseList(struct ListNode* head) {
    struct ListNode* pre = NULL;
    struct ListNode* cur = head;
    while (cur) {
        struct ListNode* nxt = cur->next;
        cur->next = pre; // 把 cur 插在 pre 链表的前面（头插法）
        pre = cur;
        cur = nxt;
    }
    return pre;
}
```

```go [sol-Go]
func reverseList(head *ListNode) *ListNode {
    var pre, cur *ListNode = nil, head
    for cur != nil {
        nxt := cur.Next
        cur.Next = pre // 把 cur 插在 pre 链表的前面（头插法）
        pre = cur
        cur = nxt
    }
    return pre
}
```

```js [sol-JavaScript]
var reverseList = function(head) {
    let pre = null, cur = head;
    while (cur) {
        const nxt = cur.next;
        cur.next = pre; // 把 cur 插在 pre 链表的前面（头插法）
        pre = cur;
        cur = nxt;
    }
    return pre;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn reverse_list(head: Option<Box<ListNode>>) -> Option<Box<ListNode>> {
        let mut pre = None;
        let mut cur = head;
        while let Some(mut node) = cur {
            let nxt = node.next;
            node.next = pre; // 把 node 插在 pre 链表的前面（头插法）
            pre = Some(node);
            cur = nxt;
        }
        pre
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为链表节点个数。
- 空间复杂度：$\mathcal{O}(1)$。

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

本题来自 `一、链表 / §1.4 反转链表`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、链表 / §1.4 反转链表`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
