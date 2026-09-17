# 19. 删除链表的倒数第 N 个结点

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/remove-nth-node-from-end-of-list/
- 题目 slug：`remove-nth-node-from-end-of-list`
- 来源专题：链表、树与回溯
- 来源分类路径：一、链表 / §1.5 前后指针
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/remove-nth-node-from-end-of-list/solutions/2004057/ru-he-shan-chu-jie-dian-liu-fen-zhong-ga-xpfs/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【视频讲解】前后指针，简洁写法（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/remove-nth-node-from-end-of-list/solutions/2004057/ru-he-shan-chu-jie-dian-liu-fen-zhong-ga-xpfs/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`ru-he-shan-chu-jie-dian-liu-fen-zhong-ga-xpfs`
- topic id：`2004057`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:30:25 +0800

想象有一把长度固定的尺子，左端点在链表头部，右端点在正数第 $n$ 个节点。向右移动尺子，当尺子右端点到达链表末尾时，左端点就在倒数第 $n$ 个节点。

由于需要删除节点，我们需要找倒数第 $n$ 个节点的前一个节点（倒数第 $n+1$ 个节点），这样才能做删除操作。

**修改**：左端点在链表头部，右端点在正数第 $n+1$ 个节点。向右移动尺子，当尺子右端点到达链表末尾时，左端点就在倒数第 $n+1$ 个节点。

**细节**：如果 $n$ 等于链表长度呢？没有正数第 $n+1$ 个节点。难道要特判这种情况？

不需要。我们可以在头节点的前面插入一个哨兵节点（dummy node），把它当作链表的头节点，这样就有正数第 $n+1$ 个节点了。换句话说，**如果遇到需要删除头节点的题目，添加哨兵节点可以简化代码逻辑**，请记住这个技巧。

具体请看视频讲解：[【基础算法精讲 08】](https://www.bilibili.com/video/BV1VP4y1Q71e/)，制作不易，欢迎点赞关注~

### 答疑

**问**：前后指针的做法为什么算作「一次遍历」？链表节点不是会被遍历两次吗？这个算法的优点是什么？

**答**：链表节点是会被遍历两次。其实我觉得这题本质上想考察的是，在不知道链表长度的前提下，设计一个算法，在「到达链表末尾的瞬间」就能知道倒数第 $n$ 个节点。这个算法的优点是，当 $n$ 比较小，且链表节点分配具有一定局部性时，前后指针的做法 cache miss 更少（相比一个指针跑两趟的算法）。

**问**：一般在做链表题时，什么时候要写 `while node`，什么时候要写 `while node.next`？

**答**：如果要遍历到最后一个节点，需要写 `while node`；如果要遍历到倒数第二个节点，需要写 `while node.next`。

```py [sol-Python3]
class Solution:
    def removeNthFromEnd(self, head: Optional[ListNode], n: int) -> Optional[ListNode]:
        # 由于可能会删除链表头部，用哨兵节点简化代码
        left = right = dummy = ListNode(next=head)
        for _ in range(n):
            right = right.next  # 右指针先向右走 n 步
        while right.next:
            left = left.next
            right = right.next  # 左右指针一起走
        left.next = left.next.next  # 左指针的下一个节点就是倒数第 n 个节点
        return dummy.next
```

```java [sol-Java]
class Solution {
    public ListNode removeNthFromEnd(ListNode head, int n) {
        // 由于可能会删除链表头部，用哨兵节点简化代码
        ListNode dummy = new ListNode(0, head);
        ListNode left = dummy;
        ListNode right = dummy;
        while (n-- > 0) {
            right = right.next; // 右指针先向右走 n 步
        }
        while (right.next != null) {
            left = left.next;
            right = right.next; // 左右指针一起走
        }
        left.next = left.next.next; // 左指针的下一个节点就是倒数第 n 个节点
        return dummy.next;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    ListNode* removeNthFromEnd(ListNode* head, int n) {
        // 由于可能会删除链表头部，用哨兵节点简化代码
        ListNode dummy{0, head};
        ListNode* left = &dummy;
        ListNode* right = &dummy;
        while (n--) {
            right = right->next; // 右指针先向右走 n 步
        }
        while (right->next) {
            left = left->next;
            right = right->next; // 左右指针一起走
        }
        // 左指针的下一个节点就是倒数第 n 个节点
        ListNode* nxt = left->next;
        left->next = left->next->next;
        delete nxt;
        return dummy.next;
    }
};
```

```c [sol-C]
struct ListNode* removeNthFromEnd(struct ListNode* head, int n) {
    // 由于可能会删除链表头部，用哨兵节点简化代码
    struct ListNode dummy = {0, head};
    struct ListNode* left = &dummy;
    struct ListNode* right = &dummy;
    while (n--) {
        right = right->next; // 右指针先向右走 n 步
    }
    while (right->next) {
        left = left->next;
        right = right->next; // 左右指针一起走
    }
    // 左指针的下一个节点就是倒数第 n 个节点
    struct ListNode* nxt = left->next;
    left->next = left->next->next;
    free(nxt);
    return dummy.next;
}
```

```go [sol-Go]
func removeNthFromEnd(head *ListNode, n int) *ListNode {
    // 由于可能会删除链表头部，用哨兵节点简化代码
    dummy := &ListNode{Next: head}
    left, right := dummy, dummy
    for ; n > 0; n-- {
        right = right.Next // 右指针先向右走 n 步
    }
    for right.Next != nil {
        left = left.Next
        right = right.Next // 左右指针一起走
    }
    left.Next = left.Next.Next // 左指针的下一个节点就是倒数第 n 个节点
    return dummy.Next
}
```

```js [sol-JavaScript]
var removeNthFromEnd = function(head, n) {
    // 由于可能会删除链表头部，用哨兵节点简化代码
    const dummy = new ListNode(0, head);
    let left = dummy;
    let right = dummy;
    while (n--) {
        right = right.next; // 右指针先向右走 n 步
    }
    while (right.next) {
        left = left.next;
        right = right.next; // 左右指针一起走
    }
    left.next = left.next.next; // 左指针的下一个节点就是倒数第 n 个节点
    return dummy.next;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn remove_nth_from_end(head: Option<Box<ListNode>>, n: i32) -> Option<Box<ListNode>> {
        let dummy = ListNode { val: 0, next: head };
        let mut left = &dummy;
        let mut right = &dummy;

        // 右指针先向右走 n 步
        for _ in 0..n {
            right = right.next.as_ref()?;
        }

        // 左右指针一起走
        while let Some(ref node) = right.next {
            left = left.next.as_ref()?;
            right = node;
        }

        // 删除倒数第 n 个节点
        // 这里需要把 left 从 &ListNode 强转成 &mut ListNode
        // 只读引用 -> 只读裸指针 -> 可变裸指针
        let mut left = left as *const ListNode as *mut ListNode;
        // 可变裸指针 -> 可变引用
        let left = unsafe { &mut *left };
        left.next = left.next.take()?.next;

        dummy.next
    }
}
```

```rust [sol-Rust 写法二]
impl Solution {
    pub fn remove_nth_from_end(head: Option<Box<ListNode>>, n: i32) -> Option<Box<ListNode>> {
        let dummy = ListNode { val: 0, next: head };
        let mut left = &dummy;
        let mut right = &dummy;

        // 右指针先向右走 n 步
        for _ in 0..n {
            right = right.next.as_ref()?;
        }

        // 左右指针一起走
        while let Some(ref node) = right.next {
            left = left.next.as_ref()?;
            right = node;
        }

        // 删除倒数第 n 个节点
        // 这里需要把 left 从 &ListNode 强转成 &mut ListNode
        #[allow(mutable_transmutes)]
        let left: &mut ListNode = unsafe { std::mem::transmute(left) };
        left.next = left.next.take()?.next;

        dummy.next
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(m)$，其中 $m$ 是链表的长度。
- 空间复杂度：$\mathcal{O}(1)$。

## 专题训练

见下面链表题单的「**§1.5 前后指针**」。

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

本题来自 `一、链表 / §1.5 前后指针`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、链表 / §1.5 前后指针`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
