# 203. 移除链表元素

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/remove-linked-list-elements/
- 题目 slug：`remove-linked-list-elements`
- 来源专题：链表、树与回溯
- 来源分类路径：一、链表 / §1.2 删除节点
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/remove-linked-list-elements/solutions/2806456/tao-lu-ru-he-you-ya-di-shan-chu-lian-bia-ah8z/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【套路】如何优雅地删除链表节点？（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/remove-linked-list-elements/solutions/2806456/tao-lu-ru-he-you-ya-di-shan-chu-lian-bia-ah8z/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tao-lu-ru-he-you-ya-di-shan-chu-lian-bia-ah8z`
- topic id：`2806456`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:22:48 +0800

## 套路 1

要想删除节点 $\textit{node}$，必须在 $\textit{node}$ 的前一个节点执行删除操作。

例如链表 $1\rightarrow 2\rightarrow 3$，要想删除 $2$，必须在节点 $1$ 处操作，也就是把节点 $1$ 的 $\textit{next}$ 更新为节点 $3$。

## 套路 2

如果头节点可能被删除，那么要在头节点之前添加一个哨兵节点，这样我们无需特判头节点被删除的情况，从而简化代码逻辑。

或者说，根据套路 1，要想删除头节点，必须在头节点的前一个节点操作，所以要添加一个哨兵节点。

## 算法

1. 初始化哨兵节点 $\textit{dummy}$，其 $\textit{next}$ 为 $\textit{head}$。
2. 遍历链表，初始化 $\textit{cur}=\textit{dummy}$。
3. 循环直到 $\textit{cur}$ 的下一个节点为空。
4. 如果 $\textit{cur}$ 的下一个节点的值等于 $\textit{val}$，那么删除下一个节点，把 $\textit{cur}.\textit{next}$ 更新为 $\textit{cur}.\textit{next}.\textit{next}$。
5. 如果 $\textit{cur}$ 的下一个节点的值不等于 $\textit{val}$，那么不删除下一个节点，继续看下下一个节点是否要删除，即更新 $\textit{cur}$ 为 $\textit{cur}.\textit{next}$。
6. 循环结束，返回 $\textit{dummy}.\textit{next}$，即删除节点后的新链表的头节点。

## 答疑

**问**：为什么没有修改 $\textit{dummy}$，但 $\textit{dummy}.\textit{next}$ 却是新链表的头节点？如果删除了 $\textit{head}$，那么最后返回的是不是原链表的头节点？

**答**：注意初始化时，$\textit{cur}$ 和 $\textit{dummy}$ 都指向同一个节点，$\textit{cur}$ 和 $\textit{dummy}$ 只是同一个节点的引用，所以修改 $\textit{cur}.\textit{next}$ 也会同时修改 $\textit{dummy}.\textit{next}$。

**问**：为什么删除下一个节点后，不需要更新 $\textit{cur}$ 为 $\textit{cur}.\textit{next}$？

**答**：删除下一个节点后，$\textit{cur}.\textit{next}$ 的节点值也可能等于 $\textit{val}$，也需要删除，如果直接更新 $\textit{cur}$ 为 $\textit{cur}.\textit{next}$，就漏删了节点。

```py [sol-Python3]
class Solution:
    def removeElements(self, head: Optional[ListNode], val: int) -> Optional[ListNode]:
        cur = dummy = ListNode(next=head)
        while cur.next:
            if cur.next.val == val:
                cur.next = cur.next.next  # 删除下一个节点
            else:
                cur = cur.next  # 继续向后遍历链表
        return dummy.next
```

```java [sol-Java]
class Solution {
    public ListNode removeElements(ListNode head, int val) {
        ListNode dummy = new ListNode(0, head);
        ListNode cur = dummy;
        while (cur.next != null) {
            if (cur.next.val == val) {
                cur.next = cur.next.next; // 删除下一个节点
            } else {
                cur = cur.next; // 继续向后遍历链表
            }
        }
        return dummy.next;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    ListNode* removeElements(ListNode* head, int val) {
        ListNode dummy(0, head);
        auto cur = &dummy;
        while (cur->next) {
            auto nxt = cur->next;
            if (nxt->val == val) {
                cur->next = nxt->next; // 删除 nxt
                delete nxt; // 释放内存，避免内存泄漏
            } else {
                cur = nxt; // 继续向后遍历链表
            }
        }
        return dummy.next;
    }
};
```

```c [sol-C]
struct ListNode* removeElements(struct ListNode* head, int val) {
    struct ListNode dummy = {0, head};
    struct ListNode* cur = &dummy;
    while (cur->next) {
        struct ListNode* nxt = cur->next;
        if (nxt->val == val) {
            cur->next = nxt->next; // 删除 nxt
            free(nxt); // 释放内存，避免内存泄漏
        } else {
            cur = nxt; // 继续向后遍历链表
        }
    }
    return dummy.next;
}
```

```go [sol-Go]
func removeElements(head *ListNode, val int) *ListNode {
    dummy := ListNode{Next: head}
    cur := &dummy
    for cur.Next != nil {
        if cur.Next.Val == val {
            cur.Next = cur.Next.Next // 删除下一个节点
        } else {
            cur = cur.Next // 继续向后遍历链表
        }
    }
    return dummy.Next
}
```

```js [sol-JS]
var removeElements = function(head, val) {
    const dummy = new ListNode(0, head);
    let cur = dummy;
    while (cur.next) {
        if (cur.next.val === val) {
            cur.next = cur.next.next; // 删除下一个节点
        } else {
            cur = cur.next; // 继续向后遍历链表
        }
    }
    return dummy.next;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn remove_elements(head: Option<Box<ListNode>>, val: i32) -> Option<Box<ListNode>> {
        let mut dummy = Box::new(ListNode { val: 0, next: head });
        let mut cur = &mut dummy;
        while let Some(ref mut nxt) = cur.next {
            if nxt.val == val {
                cur.next = nxt.next.take(); // 删除下一个节点
            } else {
                cur = cur.next.as_mut().unwrap(); // 继续向后遍历链表
            }
        }
        dummy.next
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是链表的长度。
- 空间复杂度：$\mathcal{O}(1)$。

## 相似题目

本题是[【基础算法精讲 08】](https://www.bilibili.com/video/BV1VP4y1Q71e/) 的课后作业，相似题目还有：

- [237. 删除链表中的节点](https://leetcode.cn/problems/delete-node-in-a-linked-list/)
- [19. 删除链表的倒数第 N 个结点](https://leetcode.cn/problems/remove-nth-node-from-end-of-list/)
- [83. 删除排序链表中的重复元素](https://leetcode.cn/problems/remove-duplicates-from-sorted-list/)
- [82. 删除排序链表中的重复元素 II](https://leetcode.cn/problems/remove-duplicates-from-sorted-list-ii/)

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
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)
