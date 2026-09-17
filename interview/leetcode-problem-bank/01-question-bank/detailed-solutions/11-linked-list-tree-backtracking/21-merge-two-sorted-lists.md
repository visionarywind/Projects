# 21. 合并两个有序链表

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/merge-two-sorted-lists/
- 题目 slug：`merge-two-sorted-lists`
- 来源专题：链表、树与回溯
- 来源分类路径：一、链表 / §1.8 合并链表
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/merge-two-sorted-lists/solutions/2373691/liang-chong-fang-fa-die-dai-di-gui-pytho-wf75/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：迭代（尾插法）/ 递归（头插法）Python/Java/C++/C/Go/JS/Rust](https://leetcode.cn/problems/merge-two-sorted-lists/solutions/2373691/liang-chong-fang-fa-die-dai-di-gui-pytho-wf75/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`liang-chong-fang-fa-die-dai-di-gui-pytho-wf75`
- topic id：`2373691`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:30:25 +0800

## 方法一：迭代（尾插法）

### 前置知识

- [认识链表 & 哨兵技巧【基础算法精讲 06】](https://www.bilibili.com/video/BV1sd4y1x7KN/)
- [88. 合并两个有序数组](https://leetcode.cn/problems/merge-sorted-array/)

### 思路

创建一个哨兵节点，作为合并后的新链表头节点的前一个节点。这样可以避免单独处理头节点，也无需特判链表为空的情况，从而简化代码。

比较 $\textit{list}_1$ 和 $\textit{list}_2$ 的节点值，如果 $\textit{list}_1$ 的节点值小，则把 $\textit{list}_1$ 加到新链表的末尾，然后把 $\textit{list}_1$ 替换成它的下一个节点。如果 $\textit{list}_2$ 的节点值小则同理。如果两个节点值一样，那么把谁加到新链表的末尾都是一样的，不妨规定把 $\textit{list}_2$ 加到新链表末尾。

重复上述过程，直到其中一个链表为空。

循环结束后，其中一个链表可能还有剩余的节点，将剩余部分加到新链表的末尾。

最后，返回新链表的头节点，即哨兵节点的下一个节点。

```py [sol-Python3]
class Solution:
    def mergeTwoLists(self, list1: Optional[ListNode], list2: Optional[ListNode]) -> Optional[ListNode]:
        cur = dummy = ListNode()  # 用哨兵节点简化代码逻辑
        while list1 and list2:
            if list1.val < list2.val:
                cur.next = list1  # 把 list1 加到新链表中
                list1 = list1.next
            else:  # 注：相等的情况加哪个节点都是可以的
                cur.next = list2  # 把 list2 加到新链表中
                list2 = list2.next
            cur = cur.next
        cur.next = list1 or list2  # 拼接剩余链表
        return dummy.next
```

```java [sol-Java]
class Solution {
    public ListNode mergeTwoLists(ListNode list1, ListNode list2) {
        ListNode dummy = new ListNode(); // 用哨兵节点简化代码逻辑
        ListNode cur = dummy; // cur 指向新链表的末尾
        while (list1 != null && list2 != null) {
            if (list1.val < list2.val) {
                cur.next = list1; // 把 list1 加到新链表中
                list1 = list1.next;
            } else { // 注：相等的情况加哪个节点都是可以的
                cur.next = list2; // 把 list2 加到新链表中
                list2 = list2.next;
            }
            cur = cur.next;
        }
        cur.next = list1 != null ? list1 : list2; // 拼接剩余链表
        return dummy.next;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    ListNode* mergeTwoLists(ListNode* list1, ListNode* list2) {
        ListNode dummy; // 用哨兵节点简化代码逻辑
        ListNode* cur = &dummy; // cur 指向新链表的末尾
        while (list1 && list2) {
            if (list1->val < list2->val) {
                cur->next = list1; // 把 list1 加到新链表中
                list1 = list1->next;
            } else { // 注：相等的情况加哪个节点都是可以的
                cur->next = list2; // 把 list2 加到新链表中
                list2 = list2->next;
            }
            cur = cur->next;
        }
        cur->next = list1 ? list1 : list2; // 拼接剩余链表
        return dummy.next;
    }
};
```

```c [sol-C]
struct ListNode* mergeTwoLists(struct ListNode* list1, struct ListNode* list2) {
    struct ListNode dummy; // 用哨兵节点简化代码逻辑
    struct ListNode* cur = &dummy; // cur 指向新链表的末尾
    while (list1 && list2) {
        if (list1->val < list2->val) {
            cur->next = list1; // 把 list1 加到新链表中
            list1 = list1->next;
        } else { // 注：相等的情况加哪个节点都是可以的
            cur->next = list2; // 把 list2 加到新链表中
            list2 = list2->next;
        }
        cur = cur->next;
    }
    cur->next = list1 ? list1 : list2; // 拼接剩余链表
    return dummy.next;
}
```

```go [sol-Go]
func mergeTwoLists(list1, list2 *ListNode) *ListNode {
    dummy := ListNode{} // 用哨兵节点简化代码逻辑
    cur := &dummy // cur 指向新链表的末尾
    for list1 != nil && list2 != nil {
        if list1.Val < list2.Val {
            cur.Next = list1 // 把 list1 加到新链表中
            list1 = list1.Next
        } else { // 注：相等的情况加哪个节点都是可以的
            cur.Next = list2 // 把 list2 加到新链表中
            list2 = list2.Next
        }
        cur = cur.Next
    }
    // 拼接剩余链表
    if list1 != nil {
        cur.Next = list1
    } else {
        cur.Next = list2
    }
    return dummy.Next
}
```

```js [sol-JavaScript]
var mergeTwoLists = function(list1, list2) {
    const dummy = new ListNode(); // 用哨兵节点简化代码逻辑
    let cur = dummy; // cur 指向新链表的末尾
    while (list1 && list2) {
        if (list1.val < list2.val) {
            cur.next = list1; // 把 list1 加到新链表中
            list1 = list1.next;
        } else { // 注：相等的情况加哪个节点都是可以的
            cur.next = list2; // 把 list2 加到新链表中
            list2 = list2.next;
        }
        cur = cur.next;
    }
    cur.next = list1 ?? list2; // 拼接剩余链表
    return dummy.next;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn merge_two_lists(mut list1: Option<Box<ListNode>>, mut list2: Option<Box<ListNode>>) -> Option<Box<ListNode>> {
        let mut dummy = ListNode::new(0); // 用哨兵节点简化代码逻辑
        let mut cur = &mut dummy; // cur 指向新链表的末尾
        while let (Some(node1), Some(node2)) = (&list1, &list2) {
            if node1.val < node2.val {
                cur.next = list1.take(); // 把 list1 加到新链表中
                cur = cur.next.as_mut()?;
                list1 = cur.next.take();
            } else { // 注：相等的情况加哪个节点都是可以的
                cur.next = list2.take(); // 把 list2 加到新链表中
                cur = cur.next.as_mut()?;
                list2 = cur.next.take();
            };
        }
        cur.next = list1.or(list2); // 拼接剩余链表
        dummy.next
    }
}
```

```rust [sol-Rust 写法二]
impl Solution {
    pub fn merge_two_lists(mut list1: Option<Box<ListNode>>, mut list2: Option<Box<ListNode>>) -> Option<Box<ListNode>> {
        let mut dummy = ListNode::new(0); // 用哨兵节点简化代码逻辑
        let mut cur = &mut dummy; // cur 指向新链表的末尾
        while let (Some(node1), Some(node2)) = (&list1, &list2) {
            let mut lst = if node1.val < node2.val { &mut list1 } else { &mut list2 };
            cur.next = lst.take(); // 把 lst 加到新链表中
            cur = cur.next.as_mut()?;
            *lst = cur.next.take();
        }
        cur.next = list1.or(list2); // 拼接剩余链表
        dummy.next
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n+m)$，其中 $n$ 为 $\textit{list}_1$ 的长度，$m$ 为 $\textit{list}_2$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。仅用到若干额外变量。

## 方法二：递归（头插法）

### 前置知识

[如何理解递归？计算机是怎么执行递归的？【基础算法精讲 09】](https://www.bilibili.com/video/BV1UD4y1Y769/)

### 思路

直接把 `mergeTwoLists` 当作递归函数：

- 递归边界：如果其中一个链表为空，直接返回另一个链表作为合并后的结果。
- 如果两个链表都不为空，则比较两个链表当前节点的值，选择较小的节点插在前面。
  - 如果 $\textit{list}_1$ 的节点值更小，那么取出 $\textit{list}_1$，递归调用 `mergeTwoLists(list1.next, list2)`，拿到递归返回的链表，把 $\textit{list}_1$ 插在前面。
  - 如果 $\textit{list}_2$ 的节点值更小，那么取出 $\textit{list}_2$，递归调用 `mergeTwoLists(list1, list2.next)`，拿到递归返回的链表，把 $\textit{list}_2$ 插在前面。

```py [sol-Python3]
class Solution:
    def mergeTwoLists(self, list1: Optional[ListNode], list2: Optional[ListNode]) -> Optional[ListNode]:
        if list1 is None: return list2  # 注：如果都为空则返回空
        if list2 is None: return list1
        if list1.val < list2.val:
            list1.next = self.mergeTwoLists(list1.next, list2)
            return list1
        list2.next = self.mergeTwoLists(list1, list2.next)
        return list2
```

```java [sol-Java]
class Solution {
    public ListNode mergeTwoLists(ListNode list1, ListNode list2) {
        if (list1 == null) return list2; // 注：如果都为空则返回空
        if (list2 == null) return list1;
        if (list1.val < list2.val) {
            list1.next = mergeTwoLists(list1.next, list2);
            return list1;
        }
        list2.next = mergeTwoLists(list1, list2.next);
        return list2;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    ListNode* mergeTwoLists(ListNode* list1, ListNode* list2) {
        if (list1 == nullptr) return list2; // 注：如果都为空则返回空
        if (list2 == nullptr) return list1;
        if (list1->val < list2->val) {
            list1->next = mergeTwoLists(list1->next, list2);
            return list1;
        }
        list2->next = mergeTwoLists(list1, list2->next);
        return list2;
    }
};
```

```c [sol-C]
struct ListNode* mergeTwoLists(struct ListNode* list1, struct ListNode* list2) {
    if (list1 == NULL) return list2; // 注：如果都为空则返回空
    if (list2 == NULL) return list1;
    if (list1->val < list2->val) {
        list1->next = mergeTwoLists(list1->next, list2);
        return list1;
    }
    list2->next = mergeTwoLists(list1, list2->next);
    return list2;
}
```

```go [sol-Go]
func mergeTwoLists(list1, list2 *ListNode) *ListNode {
    if list1 == nil {
        return list2 // 注：如果都为空则返回空
    }
    if list2 == nil {
        return list1
    }
    if list1.Val < list2.Val {
        list1.Next = mergeTwoLists(list1.Next, list2)
        return list1
    }
    list2.Next = mergeTwoLists(list1, list2.Next)
    return list2
}
```

```js [sol-JavaScript]
var mergeTwoLists = function(list1, list2) {
    if (list1 === null) return list2; // 注：如果都为空则返回空
    if (list2 === null) return list1;
    if (list1.val < list2.val) {
        list1.next = mergeTwoLists(list1.next, list2);
        return list1;
    }
    list2.next = mergeTwoLists(list1, list2.next);
    return list2;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn merge_two_lists(list1: Option<Box<ListNode>>, list2: Option<Box<ListNode>>) -> Option<Box<ListNode>> {
        match (list1, list2) {
            (None, None) => None, // 如果两个链表都为空，返回空
            (Some(node1), None) => Some(node1), // 如果第二个链表为空，返回第一个链表
            (None, Some(node2)) => Some(node2), // 如果第一个链表为空，返回第二个链表
            (Some(mut node1), Some(mut node2)) => {
                if node1.val < node2.val {
                    node1.next = Self::merge_two_lists(node1.next, Some(node2));
                    Some(node1)
                } else {
                    node2.next = Self::merge_two_lists(Some(node1), node2.next);
                    Some(node2)
                }
            }
        }
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n+m)$，其中 $n$ 为 $\textit{list}_1$ 的长度，$m$ 为 $\textit{list}_2$ 的长度。
- 空间复杂度：$\mathcal{O}(n+m)$。递归需要 $\mathcal{O}(n+m)$ 的栈空间。

## 思考题

如果只保留两个有序链表中的**相同元素**（求交集），要怎么做？注意，如果第一个链表有 $3$ 个 $1$，第二个链表有 $2$ 个 $1$，那么答案链表中有 $2$ 个 $1$。

欢迎在评论区发表你的思路/代码。

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

本题来自 `一、链表 / §1.8 合并链表`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、链表 / §1.8 合并链表`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
