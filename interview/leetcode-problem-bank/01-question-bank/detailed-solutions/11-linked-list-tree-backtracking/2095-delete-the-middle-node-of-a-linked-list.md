# 2095. 删除链表的中间节点

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/delete-the-middle-node-of-a-linked-list/
- 题目 slug：`delete-the-middle-node-of-a-linked-list`
- 来源专题：链表、树与回溯
- 来源分类路径：一、链表 / §1.6 快慢指针
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/delete-the-middle-node-of-a-linked-list/solutions/1139401/kuai-man-zhi-zhen-by-endlesscheng-jodm/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[龟兔赛跑（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/delete-the-middle-node-of-a-linked-list/solutions/1139401/kuai-man-zhi-zhen-by-endlesscheng-jodm/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`kuai-man-zhi-zhen-by-endlesscheng-jodm`
- topic id：`1139401`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:30:25 +0800

**前置题目**：[876. 链表的中间结点](https://leetcode.cn/problems/middle-of-the-linked-list/)，[我的题解](https://leetcode.cn/problems/middle-of-the-linked-list/solutions/1999265/mei-xiang-ming-bai-yi-ge-shi-pin-jiang-t-wzwm/)。

为了删除链表的中间节点，我们需要让慢指针少走一步，移动到中间节点的前一个节点。怎么让慢指针少走一步？

## 写法一

可以先让快指针走两步，少循环一次，这样慢指针就少走一步了。

特判只有一个节点的情况（快指针没法走两步），返回空节点。

```py [sol-Python3]
class Solution:
    def deleteMiddle(self, head: Optional[ListNode]) -> Optional[ListNode]:
        if head.next is None:  # 只有一个节点
            return None

        # 876. 链表的中间结点
        # 本题先让快指针走两步，这样慢指针少走一步，刚好落在中间节点的前一个节点
        slow = head
        fast = head.next.next
        while fast and fast.next:
            slow = slow.next
            fast = fast.next.next

        slow.next = slow.next.next  # 删除 slow 的下一个节点
        return head
```

```java [sol-Java]
class Solution {
    public ListNode deleteMiddle(ListNode head) {
        if (head.next == null) { // 只有一个节点
            return null;
        }

        // 876. 链表的中间结点
        // 本题先让快指针走两步，这样慢指针少走一步，刚好落在中间节点的前一个节点
        ListNode slow = head;
        ListNode fast = head.next.next;
        while (fast != null && fast.next != null) {
            slow = slow.next;
            fast = fast.next.next;
        }

        slow.next = slow.next.next; // 删除 slow 的下一个节点
        return head;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    ListNode* deleteMiddle(ListNode* head) {
        if (head->next == nullptr) { // 只有一个节点
            return nullptr;
        }

        // 876. 链表的中间结点
        // 本题先让快指针走两步，这样慢指针少走一步，刚好落在中间节点的前一个节点
        ListNode* slow = head;
        ListNode* fast = head->next->next;
        while (fast && fast->next) {
            slow = slow->next;
            fast = fast->next->next;
        }

        slow->next = slow->next->next; // 删除 slow 的下一个节点
        return head;
    }
};
```

```c [sol-C]
struct ListNode* deleteMiddle(struct ListNode* head) {
    if (head->next == NULL) { // 只有一个节点
        return NULL;
    }

    // 876. 链表的中间结点
    // 本题先让快指针走两步，这样慢指针少走一步，刚好落在中间节点的前一个节点
    struct ListNode* slow = head;
    struct ListNode* fast = head->next->next;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }

    slow->next = slow->next->next; // 删除 slow 的下一个节点
    return head;
}
```

```go [sol-Go]
func deleteMiddle(head *ListNode) *ListNode {
	if head.Next == nil { // 只有一个节点
		return nil
	}

	// 876. 链表的中间结点
	// 本题先让快指针走两步，这样慢指针少走一步，刚好落在中间节点的前一个节点
	slow := head
	fast := head.Next.Next
	for fast != nil && fast.Next != nil {
		slow = slow.Next
		fast = fast.Next.Next
	}

	slow.Next = slow.Next.Next // 删除 slow 的下一个节点
	return head
}
```

```js [sol-JavaScript]
var deleteMiddle = function(head) {
    if (head.next === null) { // 只有一个节点
        return null;
    }

    // 876. 链表的中间结点
    // 本题先让快指针走两步，这样慢指针少走一步，刚好落在中间节点的前一个节点
    let slow = head;
    let fast = head.next.next;
    while (fast && fast.next) {
        slow = slow.next;
        fast = fast.next.next;
    }

    slow.next = slow.next.next; // 删除 slow 的下一个节点
    return head;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn delete_middle(head: Option<Box<ListNode>>) -> Option<Box<ListNode>> {
        if head.as_ref()?.next.is_none() { // 只有一个节点
            return None;
        }

        // 876. 链表的中间结点
        // 本题先让快指针走两步，这样慢指针少走一步，刚好落在中间节点的前一个节点
        let mut slow = &head;
        let mut fast = &head.as_ref()?.next.as_ref()?.next;
        while fast.is_some() && fast.as_ref()?.next.is_some() {
            slow = &slow.as_ref()?.next;
            fast = &fast.as_ref()?.next.as_ref()?.next;
        }

        // 只读引用 -> 只读裸指针 -> 可变裸指针
        let mut slow = slow as *const Option<Box<ListNode>> as *mut Option<Box<ListNode>>;
        // 可变裸指针 -> 可变引用
        let slow = unsafe { &mut *slow };
        slow.as_mut()?.next = slow.as_mut()?.next.take()?.next; // 删除 slow 的下一个节点

        head
    }
}
```

## 写法二

在链表前面插入一个 $\textit{dummy}$ 节点，让慢指针改从 $\textit{dummy}$ 开始移动。循环次数和 876 题是一样的，但由于慢指针的出发位置往前移了一个节点，所以最后慢指针会停在中间节点的前一个节点。

```py [sol-Python3]
class Solution:
    def deleteMiddle(self, head: Optional[ListNode]) -> Optional[ListNode]:
        # 876. 链表的中间结点
        slow = dummy = ListNode(next=head)
        fast = head
        while fast and fast.next:
            slow = slow.next
            fast = fast.next.next

        slow.next = slow.next.next  # 删除 slow 的下一个节点
        return dummy.next
```

```java [sol-Java]
class Solution {
    public ListNode deleteMiddle(ListNode head) {
        ListNode dummy = new ListNode(0, head);

        // 876. 链表的中间结点
        ListNode slow = dummy;
        ListNode fast = head;
        while (fast != null && fast.next != null) {
            slow = slow.next;
            fast = fast.next.next;
        }

        slow.next = slow.next.next; // 删除 slow 的下一个节点
        return dummy.next;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    ListNode* deleteMiddle(ListNode* head) {
        ListNode dummy = {0, head};

        // 876. 链表的中间结点
        ListNode* slow = &dummy;
        ListNode* fast = head;
        while (fast && fast->next) {
            slow = slow->next;
            fast = fast->next->next;
        }

        slow->next = slow->next->next; // 删除 slow 的下一个节点
        return dummy.next;
    }
};
```

```c [sol-C]
struct ListNode* deleteMiddle(struct ListNode* head) {
    struct ListNode dummy = {0, head};

    // 876. 链表的中间结点
    struct ListNode* slow = &dummy;
    struct ListNode* fast = head;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }

    slow->next = slow->next->next; // 删除 slow 的下一个节点
    return dummy.next;
}
```

```go [sol-Go]
func deleteMiddle(head *ListNode) *ListNode {
	dummy := ListNode{Next: head}

	// 876. 链表的中间结点
	slow := &dummy
	fast := head
	for fast != nil && fast.Next != nil {
		slow = slow.Next
		fast = fast.Next.Next
	}

	slow.Next = slow.Next.Next // 删除 slow 的下一个节点
	return dummy.Next
}
```

```js [sol-JavaScript]
var deleteMiddle = function(head) {
    const dummy = new ListNode(0, head);

    // 876. 链表的中间结点
    let slow = dummy;
    let fast = head;
    while (fast && fast.next) {
        slow = slow.next;
        fast = fast.next.next;
    }

    slow.next = slow.next.next; // 删除 slow 的下一个节点
    return dummy.next;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn delete_middle(head: Option<Box<ListNode>>) -> Option<Box<ListNode>> {
        // 这里 move 了 head，下面的 fast 只能从 dummy 中获取 head
        let mut dummy = Some(Box::new(ListNode { val: 0, next: head }));

        // 876. 链表的中间结点
        let mut slow = &dummy;
        let mut fast = &dummy.as_ref()?.next; // head
        while fast.is_some() && fast.as_ref()?.next.is_some() {
            slow = &slow.as_ref()?.next;
            fast = &fast.as_ref()?.next.as_ref()?.next;
        }

        // 只读引用 -> 只读裸指针 -> 可变裸指针
        let mut slow = slow as *const Option<Box<ListNode>> as *mut Option<Box<ListNode>>;
        // 可变裸指针 -> 可变引用
        let slow = unsafe { &mut *slow };
        slow.as_mut()?.next = slow.as_mut()?.next.take()?.next; // 删除 slow 的下一个节点

        dummy?.next
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是链表的长度。
- 空间复杂度：$\mathcal{O}(1)$。

## 专题训练

见下面链表题单的「**§1.6 快慢指针**」。

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

本题来自 `一、链表 / §1.6 快慢指针`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、链表 / §1.6 快慢指针`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
