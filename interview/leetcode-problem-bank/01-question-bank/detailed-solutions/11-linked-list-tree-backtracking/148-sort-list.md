# 148. 排序链表

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/sort-list/
- 题目 slug：`sort-list`
- 来源专题：链表、树与回溯
- 来源分类路径：一、链表 / §1.9 分治
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/sort-list/solutions/2993518/liang-chong-fang-fa-fen-zhi-die-dai-mo-k-caei/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：分治/迭代，模块化设计，代码可读性高（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/sort-list/solutions/2993518/liang-chong-fang-fa-fen-zhi-die-dai-mo-k-caei/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`liang-chong-fang-fa-fen-zhi-die-dai-mo-k-caei`
- topic id：`2993518`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:30:25 +0800

## 前置题目

- [876. 链表的中间结点](https://leetcode.cn/problems/middle-of-the-linked-list/)
- [21. 合并两个有序链表](https://leetcode.cn/problems/merge-two-sorted-lists/)

## 方法一：归并排序（分治）

1. 找到**链表的中间结点** $\textit{head}_2$ 的**前一个节点**，并断开 $\textit{head}_2$ 与其前一个节点的连接。这样我们就把原链表均分成了两段更短的链表。原理见[【基础算法精讲 07】](https://www.bilibili.com/video/BV1KG4y1G7cu/)。
2. 分治，递归调用 $\texttt{sortList}$，分别排序 $\textit{head}$（只有前一半）和 $\textit{head}_2$。
3. 排序后，我们得到了两个有序链表，那么**合并两个有序链表**，得到排序后的链表，返回链表头节点。原理见 [我的题解](https://leetcode.cn/problems/merge-two-sorted-lists/solutions/2373691/liang-chong-fang-fa-die-dai-di-gui-pytho-wf75/)。

```py [sol-Python3]
class Solution:
    # 876. 链表的中间结点（快慢指针）
    def middleNode(self, head: Optional[ListNode]) -> Optional[ListNode]:
        slow = fast = head
        while fast and fast.next:
            pre = slow  # 记录 slow 的前一个节点
            slow = slow.next
            fast = fast.next.next
        pre.next = None  # 断开 slow 的前一个节点和 slow 的连接
        return slow

    # 21. 合并两个有序链表（双指针）
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
        cur.next = list1 if list1 else list2  # 拼接剩余链表
        return dummy.next

    def sortList(self, head: Optional[ListNode]) -> Optional[ListNode]:
        # 如果链表为空或者只有一个节点，无需排序
        if head is None or head.next is None:
            return head
        # 找到中间节点 head2，并断开 head2 与其前一个节点的连接
        # 比如 head=[4,2,1,3]，那么 middleNode 调用结束后 head=[4,2] head2=[1,3]
        head2 = self.middleNode(head)
        # 分治
        head = self.sortList(head)
        head2 = self.sortList(head2)
        # 合并
        return self.mergeTwoLists(head, head2)
```

```java [sol-Java]
class Solution {
    public ListNode sortList(ListNode head) {
        // 如果链表为空或者只有一个节点，无需排序
        if (head == null || head.next == null) {
            return head;
        }
        // 找到中间节点 head2，并断开 head2 与其前一个节点的连接
        // 比如 head=[4,2,1,3]，那么 middleNode 调用结束后 head=[4,2] head2=[1,3]
        ListNode head2 = middleNode(head);
        // 分治
        head = sortList(head);
        head2 = sortList(head2);
        // 合并
        return mergeTwoLists(head, head2);
    }

    // 876. 链表的中间结点（快慢指针）
    private ListNode middleNode(ListNode head) {
        ListNode pre = head;
        ListNode slow = head;
        ListNode fast = head;
        while (fast != null && fast.next != null) {
            pre = slow; // 记录 slow 的前一个节点
            slow = slow.next;
            fast = fast.next.next;
        }
        pre.next = null; // 断开 slow 的前一个节点和 slow 的连接
        return slow;
    }

    // 21. 合并两个有序链表（双指针）
    private ListNode mergeTwoLists(ListNode list1, ListNode list2) {
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
    // 876. 链表的中间结点（快慢指针）
    ListNode* middleNode(ListNode* head) {
        ListNode* pre = head;
        ListNode* slow = head;
        ListNode* fast = head;
        while (fast && fast->next) {
            pre = slow; // 记录 slow 的前一个节点
            slow = slow->next;
            fast = fast->next->next;
        }
        pre->next = nullptr; // 断开 slow 的前一个节点和 slow 的连接
        return slow;
    }

    // 21. 合并两个有序链表（双指针）
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

public:
    ListNode* sortList(ListNode* head) {
        // 如果链表为空或者只有一个节点，无需排序
        if (head == nullptr || head->next == nullptr) {
            return head;
        }
        // 找到中间节点 head2，并断开 head2 与其前一个节点的连接
        // 比如 head=[4,2,1,3]，那么 middleNode 调用结束后 head=[4,2] head2=[1,3]
        ListNode* head2 = middleNode(head);
        // 分治
        head = sortList(head);
        head2 = sortList(head2);
        // 合并
        return mergeTwoLists(head, head2);
    }
};
```

```c [sol-C]
// 876. 链表的中间结点（快慢指针）
struct ListNode* middleNode(struct ListNode* head) {
    struct ListNode* pre = head;
    struct ListNode* slow = head;
    struct ListNode* fast = head;
    while (fast && fast->next) {
        pre = slow; // 记录 slow 的前一个节点
        slow = slow->next;
        fast = fast->next->next;
    }
    pre->next = NULL; // 断开 slow 的前一个节点和 slow 的连接
    return slow;
}

// 21. 合并两个有序链表（双指针）
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

struct ListNode* sortList(struct ListNode* head) {
    // 如果链表为空或者只有一个节点，无需排序
    if (head == NULL || head->next == NULL) {
        return head;
    }
    // 找到中间节点 head2，并断开 head2 与其前一个节点的连接
    // 比如 head=[4,2,1,3]，那么 middleNode 调用结束后 head=[4,2] head2=[1,3]
    struct ListNode* head2 = middleNode(head);
    // 分治
    head = sortList(head);
    head2 = sortList(head2);
    // 合并
    return mergeTwoLists(head, head2);
}
```

```go [sol-Go]
// 876. 链表的中间结点（快慢指针）
func middleNode(head *ListNode) *ListNode {
    pre, slow, fast := head, head, head
    for fast != nil && fast.Next != nil {
        pre = slow // 记录 slow 的前一个节点
        slow = slow.Next
        fast = fast.Next.Next
    }
    pre.Next = nil  // 断开 slow 的前一个节点和 slow 的连接
    return slow
}

// 21. 合并两个有序链表（双指针）
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

func sortList(head *ListNode) *ListNode {
    // 如果链表为空或者只有一个节点，无需排序
    if head == nil || head.Next == nil {
        return head
    }
    // 找到中间节点 head2，并断开 head2 与其前一个节点的连接
    // 比如 head=[4,2,1,3]，那么 middleNode 调用结束后 head=[4,2] head2=[1,3]
    head2 := middleNode(head)
    // 分治
    head = sortList(head)
    head2 = sortList(head2)
    // 合并
    return mergeTwoLists(head, head2)
}
```

```js [sol-JavaScript]
// 876. 链表的中间结点（快慢指针）
function middleNode(head) {
    let pre = head, slow = head, fast = head;
    while (fast && fast.next) {
        pre = slow; // 记录 slow 的前一个节点
        slow = slow.next;
        fast = fast.next.next;
    }
    pre.next = null; // 断开 slow 的前一个节点和 slow 的连接
    return slow;
}

// 21. 合并两个有序链表（双指针）
function mergeTwoLists(list1, list2) {
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
}

var sortList = function(head) {
    // 如果链表为空或者只有一个节点，无需排序
    if (head === null || head.next === null) {
        return head;
    }
    // 找到中间节点 head2，并断开 head2 与其前一个节点的连接
    // 比如 head=[4,2,1,3]，那么 middleNode 调用结束后 head=[4,2] head2=[1,3]
    let head2 = middleNode(head);
    // 分治
    head = sortList(head);
    head2 = sortList(head2);
    // 合并
    return mergeTwoLists(head, head2);
};
```

```rust [sol-Rust]
impl Solution {
    // 876. 链表的中间结点（快慢指针）
    fn middle_node(head: &Option<Box<ListNode>>) -> Option<Box<ListNode>> {
        let mut fast = head;
        let mut slow = head;
        while fast.is_some() && fast.as_ref()?.next.is_some() {
            slow = &slow.as_ref()?.next;
            fast = &fast.as_ref()?.next.as_ref()?.next;
        }
        // 把 slow 从 &Option<Box<ListNode>> 强转成 &mut Option<Box<ListNode>>
        // 只读引用 -> 只读裸指针 -> 可变裸指针
        let mut slow = slow as *const Option<Box<ListNode>> as *mut Option<Box<ListNode>>;
        // 可变裸指针 -> 可变引用
        let slow = unsafe { &mut *slow };
        slow.take() // 断开中间节点和前一个节点的连接
    }

    // 21. 合并两个有序链表（双指针）
    fn merge_two_lists(mut list1: Option<Box<ListNode>>, mut list2: Option<Box<ListNode>>) -> Option<Box<ListNode>> {
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

    pub fn sort_list(head: Option<Box<ListNode>>) -> Option<Box<ListNode>> {
        // 如果链表为空或者只有一个节点，无需排序
        if head.is_none() || head.as_ref()?.next.is_none() {
            return head;
        }
        // 找到中间节点 head2，并断开 head2 与其前一个节点的连接
        // 比如 head=[4,2,1,3]，那么 middleNode 调用结束后 head=[4,2] head2=[1,3]
        let head2 = Self::middle_node(&head);
        // 分治
        let head = Self::sort_list(head);
        let head2 = Self::sort_list(head2);
        // 合并
        Self::merge_two_lists(head, head2)
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n)$，其中 $n$ 是链表长度。递归式 $T(n) = 2T(n/2) + \mathcal{O}(n)$，由主定理可得时间复杂度为 $\mathcal{O}(n\log n)$。从图形上理解，递归深度是 $\mathcal{O}(\log n)$，每一层的链表长度之和是 $\mathcal{O}(n)$。计算高为 $\mathcal{O}(\log n)$，底边长为 $\mathcal{O}(n)$ 的矩形面积，得到 $\mathcal{O}(n\log n)$。
- 空间复杂度：$\mathcal{O}(\log n)$。递归需要 $\mathcal{O}(\log n)$ 的栈开销。

## 方法二：归并排序（迭代）

方法一的归并是**自顶向下**计算，需要 $\mathcal{O}(\log n)$ 的递归栈开销。

方法二将其改成**自底向上**计算，空间复杂度优化成 $\mathcal{O}(1)$。

自底向上的意思是：

- 首先，归并长度为 $1$ 的子链表。例如 $[4,2,1,3]$，把第一个节点和第二个节点归并，第三个节点和第四个节点归并，得到 $[2,4,1,3]$。
- 然后，归并长度为 $2$ 的子链表。例如 $[2,4,1,3]$，把前两个节点和后两个节点归并，得到 $[1,2,3,4]$。
- 然后，归并长度为 $4$ 的子链表。
- 依此类推，直到归并的长度大于等于链表长度为止，此时链表已经是有序的了。

具体算法：

1. 遍历链表，获取链表长度 $\textit{length}$。
2. 初始化步长 $\textit{step}=1$。
3. 循环直到 $\textit{step} \ge \textit{length}$。
4. 每轮循环，从链表头节点开始。
5. 分割出两段长为 $\textit{step}$ 的链表，合并，把合并后的链表插到新链表的末尾。重复该步骤，直到链表遍历完毕。
6. 把 $\textit{step}$ 扩大一倍。回到第 $4$ 步。

具体细节见代码。

```py [sol-Python3]
class Solution:
    # 获取链表长度
    def getListLength(self, head: Optional[ListNode]) -> int:
        length = 0
        while head:
            length += 1
            head = head.next
        return length

    # 分割链表
    # 如果链表长度 <= size，不做任何操作，返回空节点
    # 如果链表长度 > size，把链表的前 size 个节点分割出来（断开连接），并返回剩余链表的头节点
    def splitList(self, head: Optional[ListNode], size: int) -> Optional[ListNode]:
        # 先找到 next_head 的前一个节点
        cur = head
        for _ in range(size - 1):
            if cur is None:
                break
            cur = cur.next

        # 如果链表长度 <= size
        if cur is None or cur.next is None:
            return None  # 不做任何操作，返回空节点

        next_head = cur.next
        cur.next = None  # 断开 next_head 的前一个节点和 next_head 的连接
        return next_head

    # 21. 合并两个有序链表（双指针）
    # 返回合并后的链表的头节点和尾节点
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
        while cur.next:
            cur = cur.next
        # 循环结束后，cur 是合并后的链表的尾节点
        return dummy.next, cur

    def sortList(self, head: Optional[ListNode]) -> Optional[ListNode]:
        length = self.getListLength(head)  # 获取链表长度
        dummy = ListNode(next=head)  # 用哨兵节点简化代码逻辑
        step = 1  # 步长（参与合并的链表长度）
        while step < length:
            new_list_tail = dummy  # 新链表的末尾
            cur = dummy.next  # 每轮循环的起始节点
            while cur:
                # 从 cur 开始，分割出两段长为 step 的链表，头节点分别为 head1 和 head2
                head1 = cur
                head2 = self.splitList(head1, step)
                cur = self.splitList(head2, step)  # 下一轮循环的起始节点
                # 合并两段长为 step 的链表
                head, tail = self.mergeTwoLists(head1, head2)
                # 合并后的头节点 head，插到 new_list_tail 的后面
                new_list_tail.next = head
                new_list_tail = tail  # tail 现在是新链表的末尾
            step *= 2
        return dummy.next
```

```java [sol-Java]
class Solution {
    public ListNode sortList(ListNode head) {
        int length = getListLength(head); // 获取链表长度
        ListNode dummy = new ListNode(0, head); // 用哨兵节点简化代码逻辑
        // step 为步长，即参与合并的链表长度
        for (int step = 1; step < length; step *= 2) {
            ListNode newListTail = dummy; // 新链表的末尾
            ListNode cur = dummy.next; // 每轮循环的起始节点
            while (cur != null) {
                // 从 cur 开始，分割出两段长为 step 的链表，头节点分别为 head1 和 head2
                ListNode head1 = cur;
                ListNode head2 = splitList(head1, step);
                cur = splitList(head2, step); // 下一轮循环的起始节点
                // 合并两段长为 step 的链表
                ListNode[] merged = mergeTwoLists(head1, head2);
                // 合并后的头节点 merged[0]，插到 newListTail 的后面
                newListTail.next = merged[0];
                newListTail = merged[1]; // merged[1] 现在是新链表的末尾
            }
        }
        return dummy.next;
    }

    // 获取链表长度
    private int getListLength(ListNode head) {
        int length = 0;
        while (head != null) {
            length++;
            head = head.next;
        }
        return length;
    }

    // 分割链表
    // 如果链表长度 <= size，不做任何操作，返回空节点
    // 如果链表长度 > size，把链表的前 size 个节点分割出来（断开连接），并返回剩余链表的头节点
    private ListNode splitList(ListNode head, int size) {
        // 先找到 nextHead 的前一个节点
        ListNode cur = head;
        for (int i = 0; i < size - 1 && cur != null; i++) {
            cur = cur.next;
        }

        // 如果链表长度 <= size
        if (cur == null || cur.next == null) {
            return null; // 不做任何操作，返回空节点
        }

        ListNode nextHead = cur.next;
        cur.next = null; // 断开 nextHead 的前一个节点和 nextHead 的连接
        return nextHead;
    }

    // 21. 合并两个有序链表（双指针）
    // 返回合并后的链表的头节点和尾节点
    private ListNode[] mergeTwoLists(ListNode list1, ListNode list2) {
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
        while (cur.next != null) {
            cur = cur.next;
        }
        // 循环结束后，cur 是合并后的链表的尾节点
        return new ListNode[]{dummy.next, cur};
    }
}
```

```cpp [sol-C++]
class Solution {
    // 获取链表长度
    int getListLength(ListNode* head) {
        int length = 0;
        while (head) {
            length++;
            head = head->next;
        }
        return length;
    }

    // 分割链表
    // 如果链表长度 <= size，不做任何操作，返回空节点
    // 如果链表长度 > size，把链表的前 size 个节点分割出来（断开连接），并返回剩余链表的头节点
    ListNode* splitList(ListNode* head, int size) {
        // 先找到 next_head 的前一个节点
        ListNode* cur = head;
        for (int i = 0; i < size - 1 && cur; i++) {
            cur = cur->next;
        }

        // 如果链表长度 <= size
        if (cur == nullptr || cur->next == nullptr) {
            return nullptr; // 不做任何操作，返回空节点
        }

        ListNode* next_head = cur->next;
        cur->next = nullptr; // 断开 next_head 的前一个节点和 next_head 的连接
        return next_head;
    }

    // 21. 合并两个有序链表（双指针）
    // 返回合并后的链表的头节点和尾节点
    pair<ListNode*, ListNode*> mergeTwoLists(ListNode* list1, ListNode* list2) {
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
        while (cur->next) {
            cur = cur->next;
        }
        // 循环结束后，cur 是合并后的链表的尾节点
        return {dummy.next, cur};
    }

public:
    ListNode* sortList(ListNode* head) {
        int length = getListLength(head); // 获取链表长度
        ListNode dummy(0, head); // 用哨兵节点简化代码逻辑
        // step 为步长，即参与合并的链表长度
        for (int step = 1; step < length; step *= 2) {
            ListNode* new_list_tail = &dummy; // 新链表的末尾
            ListNode* cur = dummy.next; // 每轮循环的起始节点
            while (cur) {
                // 从 cur 开始，分割出两段长为 step 的链表，头节点分别为 head1 和 head2
                ListNode* head1 = cur;
                ListNode* head2 = splitList(head1, step);
                cur = splitList(head2, step); // 下一轮循环的起始节点
                // 合并两段长为 step 的链表
                auto [head, tail] = mergeTwoLists(head1, head2);
                // 合并后的头节点 head，插到 new_list_tail 的后面
                new_list_tail->next = head;
                new_list_tail = tail; // tail 现在是新链表的末尾
            }
        }
        return dummy.next;
    }
};
```

```c [sol-C]
// 获取链表长度
int getListLength(struct ListNode* head) {
    int length = 0;
    while (head) {
        length++;
        head = head->next;
    }
    return length;
}

// 分割链表
// 如果链表长度 <= size，不做任何操作，返回空节点
// 如果链表长度 > size，把链表的前 size 个节点分割出来（断开连接），并返回剩余链表的头节点
struct ListNode* splitList(struct ListNode* head, int size) {
    // 先找到 next_head 的前一个节点
    struct ListNode* cur = head;
    for (int i = 0; i < size - 1 && cur; i++) {
        cur = cur->next;
    }

    // 如果链表长度 <= size
    if (cur == NULL || cur->next == NULL) {
        return NULL; // 不做任何操作，返回空节点
    }

    struct ListNode* next_head = cur->next;
    cur->next = NULL; // 断开 next_head 的前一个节点和 next_head 的连接
    return next_head;
}

// 21. 合并两个有序链表（双指针）
// 返回合并后的链表的头节点，尾节点保存在入参中
struct ListNode* mergeTwoLists(struct ListNode* list1, struct ListNode* list2, struct ListNode** tail) {
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
    while (cur->next) {
        cur = cur->next;
    }
    // 循环结束后，cur 是合并后的链表的尾节点
    *tail = cur;
    return dummy.next;
}

struct ListNode* sortList(struct ListNode* head) {
    int length = getListLength(head); // 获取链表长度
    struct ListNode dummy = {0, head}; // 用哨兵节点简化代码逻辑
    // step 为步长，即参与合并的链表长度
    for (int step = 1; step < length; step *= 2) {
        struct ListNode* new_list_tail = &dummy; // 新链表的末尾
        struct ListNode* cur = dummy.next; // 每轮循环的起始节点
        while (cur) {
            // 从 cur 开始，分割出两段长为 step 的链表，头节点分别为 head1 和 head2
            struct ListNode* head1 = cur;
            struct ListNode* head2 = splitList(head1, step);
            cur = splitList(head2, step); // 下一轮循环的起始节点
            // 合并两段长为 step 的链表
            // 合并后的头节点 head，插到 new_list_tail 的后面
            struct ListNode* tail;
            new_list_tail->next = mergeTwoLists(head1, head2, &tail);
            new_list_tail = tail; // tail 现在是新链表的末尾
        }
    }
    return dummy.next;
}
```

```go [sol-Go]
// 获取链表长度
func getListLength(head *ListNode) (length int) {
    for head != nil {
        length++
        head = head.Next
    }
    return
}

// 分割链表
// 如果链表长度 <= size，不做任何操作，返回空节点
// 如果链表长度 > size，把链表的前 size 个节点分割出来（断开连接），并返回剩余链表的头节点
func splitList(head *ListNode, size int) *ListNode {
    // 先找到 nextHead 的前一个节点
    cur := head
    for i := 0; i < size-1 && cur != nil; i++ {
        cur = cur.Next
    }

    // 如果链表长度 <= size
    if cur == nil || cur.Next == nil {
        return nil // 不做任何操作，返回空节点
    }

    nextHead := cur.Next
    cur.Next = nil // 断开 nextHead 的前一个节点和 nextHead 的连接
    return nextHead
}

// 21. 合并两个有序链表（双指针）
// 返回合并后的链表的头节点和尾节点
func mergeTwoLists(list1, list2 *ListNode) (head, tail *ListNode) {
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

    for cur.Next != nil {
        cur = cur.Next
    }
    // 循环结束后，cur 是合并后的链表的尾节点
    return dummy.Next, cur
}

func sortList(head *ListNode) *ListNode {
    length := getListLength(head) // 获取链表长度
    dummy := ListNode{Next: head} // 用哨兵节点简化代码逻辑
    // step 为步长，即参与合并的链表长度
    for step := 1; step < length; step *= 2 {
        newListTail := &dummy // 新链表的末尾
        cur := dummy.Next // 每轮循环的起始节点
        for cur != nil {
            // 从 cur 开始，分割出两段长为 step 的链表，头节点分别为 head1 和 head2
            head1 := cur
            head2 := splitList(head1, step)
            cur = splitList(head2, step) // 下一轮循环的起始节点
            // 合并两段长为 step 的链表
            head, tail := mergeTwoLists(head1, head2)
            // 合并后的头节点 head，插到 newListTail 的后面
            newListTail.Next = head
            newListTail = tail // tail 现在是新链表的末尾
        }
    }
    return dummy.Next
}
```

```js [sol-JavaScript]
// 获取链表长度
function getListLength(head) {
    let length = 0;
    while (head) {
        length++;
        head = head.next;
    }
    return length;
}

// 分割链表
// 如果链表长度 <= size，不做任何操作，返回空节点
// 如果链表长度 > size，把链表的前 size 个节点分割出来（断开连接），并返回剩余链表的头节点
function splitList(head, size) {
    // 先找到 nextHead 的前一个节点
    let cur = head;
    for (let i = 0; i < size - 1 && cur; i++) {
        cur = cur.next;
    }

    // 如果链表长度 <= size
    if (cur === null || cur.next === null) {
        return null; // 不做任何操作，返回空节点
    }

    const nextHead = cur.next;
    cur.next = null; // 断开 nextHead 的前一个节点和 nextHead 的连接
    return nextHead;
}

// 21. 合并两个有序链表（双指针）
// 返回合并后的链表的头节点和尾节点
function mergeTwoLists(list1, list2) {
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
    while (cur.next) {
        cur = cur.next;
    }
    // 循环结束后，cur 是合并后的链表的尾节点
    return [dummy.next, cur];
}

var sortList = function(head) {
    const length = getListLength(head); // 获取链表长度
    const dummy = new ListNode(0, head); // 用哨兵节点简化代码逻辑
    // step 为步长，即参与合并的链表长度
    for (let step = 1; step < length; step *= 2) {
        let newListTail = dummy; // 新链表的末尾
        let cur = dummy.next; // 每轮循环的起始节点
        while (cur) {
            // 从 cur 开始，分割出两段长为 step 的链表，头节点分别为 head1 和 head2
            const head1 = cur;
            const head2 = splitList(head1, step);
            cur = splitList(head2, step); // 下一轮循环的起始节点
            // 合并两段长为 step 的链表
            const [head, tail] = mergeTwoLists(head1, head2);
            // 合并后的头节点 head，插到 newListTail 的后面
            newListTail.next = head;
            newListTail = tail; // tail 现在是新链表的末尾
        }
    }
    return dummy.next;
};
```

```rust [sol-Rust]
impl Solution {
    // 获取链表长度
    fn get_list_length(mut head: &Option<Box<ListNode>>) -> i32 {
        let mut length = 0;
        while let Some(node) = head {
            length += 1;
            head = &node.next;
        }
        length
    }

    // 分割链表
    // 如果链表长度 <= size，不做任何操作，返回空节点
    // 如果链表长度 > size，把链表的前 size 个节点分割出来（断开连接），并返回剩余链表的头节点
    fn split_list(mut head: &Option<Box<ListNode>>, size: i32) -> Option<Box<ListNode>> {
        // 从 head 开始往后走 size 步
        for _ in 0..size {
            if let Some(node) = head {
                head = &node.next;
            } else {
                return None;
            }
        }
        // 把 head 从 &Option<Box<ListNode>> 强转成 &mut Option<Box<ListNode>>
        let mut node = head as *const Option<Box<ListNode>> as *mut Option<Box<ListNode>>;
        let node = unsafe { &mut *node };
        node.take() // 断开 node 和前一个节点的连接
    }

    // 21. 合并两个有序链表（双指针）
    fn merge_two_lists(mut list1: Option<Box<ListNode>>, mut list2: Option<Box<ListNode>>) -> Option<Box<ListNode>> {
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

    // 获取链表尾节点
    fn get_mut_tail(head: &Option<Box<ListNode>>) -> &mut Box<ListNode> {
        let mut cur = head.as_ref().unwrap();
        while cur.next.is_some() {
            cur = cur.next.as_ref().unwrap();
        }
        // 把 cur 从 &Box<ListNode> 强转成 &mut Box<ListNode>
        let mut cur = cur as *const Box<ListNode> as *mut Box<ListNode>;
        unsafe { &mut *cur }
    }

    pub fn sort_list(head: Option<Box<ListNode>>) -> Option<Box<ListNode>> {
        let length = Self::get_list_length(&head); // 获取链表长度
        let mut dummy = Box::new(ListNode { val: 0, next: head }); // 用哨兵节点简化代码逻辑
        let mut step = 1; // 步长（参与合并的链表长度）
        while step < length {
            let mut cur = dummy.next.take(); // 每轮循环的起始节点
            let mut new_list_tail = &mut dummy; // 新链表的末尾
            while cur.is_some() {
                // 从 cur 开始，分割出两段长为 step 的链表，头节点分别为 head1 和 head2
                let head1 = cur.take();
                let head2 = Self::split_list(&head1, step);
                cur = Self::split_list(&head2, step); // 下一轮循环的起始节点
                // 合并两段长为 step 的链表
                // 合并后的链表头节点，插到 new_list_tail 的后面
                new_list_tail.next = Self::merge_two_lists(head1, head2);
                new_list_tail = Self::get_mut_tail(&new_list_tail.next);
            }
            step *= 2;
        }
        dummy.next
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n)$，其中 $n$ 是链表长度。
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
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `一、链表 / §1.9 分治`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、链表 / §1.9 分治`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
