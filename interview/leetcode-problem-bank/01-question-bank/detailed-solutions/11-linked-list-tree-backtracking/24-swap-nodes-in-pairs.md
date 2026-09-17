# 24. 两两交换链表中的节点

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/swap-nodes-in-pairs/
- 题目 slug：`swap-nodes-in-pairs`
- 来源专题：链表、树与回溯
- 来源分类路径：一、链表 / §1.4 反转链表
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/swap-nodes-in-pairs/solutions/2374872/tu-jie-die-dai-di-gui-yi-zhang-tu-miao-d-51ap/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【图解】迭代/递归，一张图秒懂！（Python/Java/C++/C/Go/JS）](https://leetcode.cn/problems/swap-nodes-in-pairs/solutions/2374872/tu-jie-die-dai-di-gui-yi-zhang-tu-miao-d-51ap/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tu-jie-die-dai-di-gui-yi-zhang-tu-miao-d-51ap`
- topic id：`2374872`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:30:25 +0800

## 方法一：迭代

### 前置知识

[认识链表 & 哨兵技巧【基础算法精讲 06】](https://www.bilibili.com/video/BV1sd4y1x7KN/)

![lc24-c.png](https://pic.leetcode.cn/1691121590-SWAYuj-lc24-c.png)

```py [sol-Python3]
class Solution:
    def swapPairs(self, head: Optional[ListNode]) -> Optional[ListNode]:
        node0 = dummy = ListNode(next=head)  # 用哨兵节点简化代码逻辑
        node1 = head
        while node1 and node1.next:  # 至少有两个节点
            node2 = node1.next
            node3 = node2.next

            node0.next = node2  # 0 -> 2
            node2.next = node1  # 2 -> 1
            node1.next = node3  # 1 -> 3

            node0 = node1  # 下一轮交换，0 是 1
            node1 = node3  # 下一轮交换，1 是 3
        return dummy.next  # 返回新链表的头节点
```

```java [sol-Java]
class Solution {
    public ListNode swapPairs(ListNode head) {
        ListNode dummy = new ListNode(0, head); // 用哨兵节点简化代码逻辑
        ListNode node0 = dummy;
        ListNode node1 = head;
        while (node1 != null && node1.next != null) { // 至少有两个节点
            ListNode node2 = node1.next;
            ListNode node3 = node2.next;

            node0.next = node2; // 0 -> 2
            node2.next = node1; // 2 -> 1
            node1.next = node3; // 1 -> 3

            node0 = node1; // 下一轮交换，0 是 1
            node1 = node3; // 下一轮交换，1 是 3
        }
        return dummy.next; // 返回新链表的头节点
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    ListNode* swapPairs(ListNode* head) {
        ListNode dummy(0, head); // 用哨兵节点简化代码逻辑
        ListNode* node0 = &dummy;
        ListNode* node1 = head;
        while (node1 && node1->next) { // 至少有两个节点
            ListNode* node2 = node1->next;
            ListNode* node3 = node2->next;

            node0->next = node2; // 0 -> 2
            node2->next = node1; // 2 -> 1
            node1->next = node3; // 1 -> 3

            node0 = node1; // 下一轮交换，0 是 1
            node1 = node3; // 下一轮交换，1 是 3
        }
        return dummy.next; // 返回新链表的头节点
    }
};
```

```c [sol-C]
struct ListNode* swapPairs(struct ListNode* head) {
    struct ListNode dummy = {0, head}; // 用哨兵节点简化代码逻辑
    struct ListNode* node0 = &dummy;
    struct ListNode* node1 = head;
    while (node1 && node1->next) { // 至少有两个节点
        struct ListNode* node2 = node1->next;
        struct ListNode* node3 = node2->next;

        node0->next = node2; // 0 -> 2
        node2->next = node1; // 2 -> 1
        node1->next = node3; // 1 -> 3

        node0 = node1; // 下一轮交换，0 是 1
        node1 = node3; // 下一轮交换，1 是 3
    }
    return dummy.next; // 返回新链表的头节点
}
```

```go [sol-Go]
func swapPairs(head *ListNode) *ListNode {
    dummy := &ListNode{Next: head} // 用哨兵节点简化代码逻辑
    node0 := dummy
    node1 := head
    for node1 != nil && node1.Next != nil { // 至少有两个节点
        node2 := node1.Next
        node3 := node2.Next

        node0.Next = node2 // 0 -> 2
        node2.Next = node1 // 2 -> 1
        node1.Next = node3 // 1 -> 3

        node0 = node1 // 下一轮交换，0 是 1
        node1 = node3 // 下一轮交换，1 是 3
    }
    return dummy.Next // 返回新链表的头节点
}
```

```js [sol-JavaScript]
var swapPairs = function(head) {
    const dummy = new ListNode(0, head); // 用哨兵节点简化代码逻辑
    let node0 = dummy;
    let node1 = head;
    while (node1 && node1.next) { // 至少有两个节点
        const node2 = node1.next;
        const node3 = node2.next;

        node0.next = node2; // 0 -> 2
        node2.next = node1; // 2 -> 1
        node1.next = node3; // 1 -> 3

        node0 = node1; // 下一轮交换，0 是 1
        node1 = node3; // 下一轮交换，1 是 3
    }
    return dummy.next; // 返回新链表的头节点
};
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为链表长度。
- 空间复杂度：$\mathcal{O}(1)$。仅用到若干额外变量。

## 方法二：递归

### 前置知识

[一看递归就晕？带你理解递归的本质！【基础算法精讲 09】](https://www.bilibili.com/video/BV1UD4y1Y769/)

### 思路

和方法一类似。这里直接用 `swapPairs` 当作递归函数：

- 递归边界：如果 $\textit{head}$ 或者 $\textit{head}.\textit{next}$ 为空，说明剩余节点不足两个，无需交换，返回 $\textit{head}$。
- 先交换以 $\textit{node}_3$ 为头节点的链表，即递归调用 `swapPairs(node3)`。
- 把 $\textit{node}_1$ 指向递归返回的链表头。
- 把 $\textit{node}_2$ 指向 $\textit{node}_1$。
- 返回 $\textit{node}_2$，作为交换后的链表头节点。

```py [sol-Python3]
class Solution:
    def swapPairs(self, head: Optional[ListNode]) -> Optional[ListNode]:
        if head is None or head.next is None:  # 递归边界
            return head  # 不足两个节点，无需交换

        node1 = head
        node2 = head.next
        node3 = node2.next

        node1.next = self.swapPairs(node3)  # 1 指向递归返回的链表头
        node2.next = node1  # 2 指向 1

        return node2  # 返回交换后的链表头节点
```

```java [sol-Java]
class Solution {
    public ListNode swapPairs(ListNode head) {
        if (head == null || head.next == null) {
            return head;
        }

        ListNode node1 = head;
        ListNode node2 = head.next;
        ListNode node3 = node2.next;

        node1.next = swapPairs(node3); // 1 指向递归返回的链表头
        node2.next = node1; // 2 指向 1

        return node2; // 返回交换后的链表头节点
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    ListNode* swapPairs(ListNode* head) {
        if (head == nullptr || head->next == nullptr) {
            return head;
        }

        ListNode* node1 = head;
        ListNode* node2 = head->next;
        ListNode* node3 = node2->next;

        node1->next = swapPairs(node3); // 1 指向递归返回的链表头
        node2->next = node1; // 2 指向 1

        return node2; // 返回交换后的链表头节点
    }
};
```

```c [sol-C]
struct ListNode* swapPairs(struct ListNode* head) {
    if (head == NULL || head->next == NULL) {
        return head;
    }

    struct ListNode* node1 = head;
    struct ListNode* node2 = head->next;
    struct ListNode* node3 = node2->next;

    node1->next = swapPairs(node3); // 1 指向递归返回的链表头
    node2->next = node1; // 2 指向 1

    return node2; // 返回交换后的链表头节点
}
```

```go [sol-Go]
func swapPairs(head *ListNode) *ListNode {
    if head == nil || head.Next == nil {
        return head
    }

    node1 := head
    node2 := head.Next
    node3 := node2.Next

    node1.Next = swapPairs(node3) // 1 指向递归返回的链表头
    node2.Next = node1            // 2 指向 1

    return node2 // 返回交换后的链表头节点
}
```

```js [sol-JavaScript]
var swapPairs = function(head) {
    if (head === null || head.next === null) {
        return head;
    }

    const node1 = head;
    const node2 = head.next;
    const node3 = node2.next;

    node1.next = swapPairs(node3); // 1 指向递归返回的链表头
    node2.next = node1; // 2 指向 1

    return node2; // 返回交换后的链表头节点
};
```

```rust [sol-Rust]
impl Solution {
    pub fn swap_pairs(head: Option<Box<ListNode>>) -> Option<Box<ListNode>> {
        if head.as_ref()?.next.is_none() {
            return head;
        }

        let mut node1 = head;
        let mut node2 = node1.as_mut()?.next.take();
        let node3 = node2.as_mut()?.next.take();

        node1.as_mut()?.next = Self::swap_pairs(node3); // 1 指向递归返回的链表头
        node2.as_mut()?.next = node1; // 2 指向 1

        node2 // 返回交换后的链表头节点
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为链表长度。
- 空间复杂度：$\mathcal{O}(n)$。递归需要 $\mathcal{O}(n)$ 的栈空间。

## 思考题

如果改成每 $k$ 个节点一组反转链表呢？

这题是 [25. K 个一组翻转链表](https://leetcode.cn/problems/reverse-nodes-in-k-group/)，请看 [视频讲解](https://www.bilibili.com/video/BV1sd4y1x7KN/)。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与一般树（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/circle/discuss/K0n2gO/)
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
