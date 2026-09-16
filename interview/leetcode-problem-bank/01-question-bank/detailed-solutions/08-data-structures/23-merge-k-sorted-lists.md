# 23. 合并 K 个升序链表

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/merge-k-sorted-lists/
- 题目 slug：`merge-k-sorted-lists`
- 来源专题：常用数据结构
- 来源分类路径：五、堆（优先队列） / §5.2 进阶
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/merge-k-sorted-lists/solutions/2384305/liang-chong-fang-fa-zui-xiao-dui-fen-zhi-zbzx/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[三种方法：最小堆/分治/迭代（Python/Java/C++/Go/JS）](https://leetcode.cn/problems/merge-k-sorted-lists/solutions/2384305/liang-chong-fang-fa-zui-xiao-dui-fen-zhi-zbzx/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`liang-chong-fang-fa-zui-xiao-dui-fen-zhi-zbzx`
- topic id：`2384305`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 方法一：最小堆

合并后的第一个节点 $\textit{first}$，一定是某个链表的**头节点**（因为链表已按升序排列）。

合并后的第二个节点，可能是某个链表的头节点，也可能是 $\textit{first}$ 的下一个节点。

例如有三个链表 `1->2->5, 3->4->6, 4->5->6`，找到第一个节点 `1` 之后，第二个节点不是另一个链表的头节点，而是节点 `1` 的下一个节点 `2`。

按照这个过程继续思考，每当我们找到一个节点值最小的节点 $x$，就把节点 $x.\textit{next}$ 加入「可能是最小节点」的集合中。

因此，我们需要一个数据结构，它支持：

- 从数据结构中找到并移除最小节点。
- 插入节点。

这可以用**最小堆**实现。初始把所有链表的头节点入堆，然后不断弹出堆中最小节点 $x$，如果 $x.\textit{next}$ 不为空就加入堆中。循环直到堆为空。把弹出的节点按顺序拼接起来，就得到了答案。

实现时，可以用**哨兵节点**简化代码，具体请看[【基础算法精讲 06】](https://www.bilibili.com/video/BV1sd4y1x7KN/)。

```py [sol-Python3]
# 让堆可以比较节点大小（不改 __lt__ 的写法见 Python3 写法二）
ListNode.__lt__ = lambda a, b: a.val < b.val  

class Solution:
    def mergeKLists(self, lists: List[Optional[ListNode]]) -> Optional[ListNode]:
        cur = dummy = ListNode()  # 哨兵节点，作为合并后链表头节点的前一个节点
        h = [head for head in lists if head]  # 把所有非空链表的头节点入堆
        heapify(h)  # 堆化
        while h:  # 循环直到堆为空
            node = heappop(h)  # 剩余节点中的最小节点
            if node.next:  # 下一个节点不为空
                heappush(h, node.next)  # 下一个节点有可能是最小节点，入堆
            cur.next = node  # 把 node 添加到新链表的末尾
            cur = cur.next  # 准备合并下一个节点
        return dummy.next  # 哨兵节点的下一个节点就是新链表的头节点
```

```py [sol-Python3 写法二]
class Solution:
    def mergeKLists(self, lists: List[Optional[ListNode]]) -> Optional[ListNode]:
        cur = dummy = ListNode()  # 哨兵节点，作为合并后链表头节点的前一个节点
        # 额外保存 i，这样即使 val 相同也不会去比 ListNode，避免报错）
        h = [(head.val, i, head) for i, head in enumerate(lists) if head]  # 把所有非空链表的头节点入堆
        heapify(h)  # 堆化
        while h:  # 循环直到堆为空
            _, i, node = heappop(h)  # 剩余节点中的最小节点
            if node.next:  # 下一个节点不为空
                heappush(h, (node.next.val, i, node.next))  # 下一个节点有可能是最小节点，入堆
            cur.next = node  # 把 node 添加到新链表的末尾
            cur = cur.next  # 准备合并下一个节点
        return dummy.next  # 哨兵节点的下一个节点就是新链表的头节点
```

```java [sol-Java]
class Solution {
    public ListNode mergeKLists(ListNode[] lists) {
        PriorityQueue<ListNode> pq = new PriorityQueue<>((a, b) -> a.val - b.val);
        for (ListNode head : lists) {
            if (head != null) {
                pq.offer(head); // 把所有非空链表的头节点入堆
            }
        }

        ListNode dummy = new ListNode(); // 哨兵节点，作为合并后链表头节点的前一个节点
        ListNode cur = dummy;
        while (!pq.isEmpty()) { // 循环直到堆为空
            ListNode node = pq.poll(); // 剩余节点中的最小节点
            if (node.next != null) { // 下一个节点不为空
                pq.offer(node.next); // 下一个节点有可能是最小节点，入堆
            }
            cur.next = node; // 把 node 添加到新链表的末尾
            cur = cur.next; // 准备合并下一个节点
        }
        return dummy.next; // 哨兵节点的下一个节点就是新链表的头节点
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    ListNode* mergeKLists(vector<ListNode*>& lists) {
        auto cmp = [](const ListNode* a, const ListNode* b) {
            return a->val > b->val; // 最小堆
        };
        priority_queue<ListNode*, vector<ListNode*>, decltype(cmp)> pq;
        for (auto head : lists) {
            if (head) {
                pq.push(head); // 把所有非空链表的头节点入堆
            }
        }

        ListNode dummy{}; // 哨兵节点，作为合并后链表头节点的前一个节点
        auto cur = &dummy;
        while (!pq.empty()) { // 循环直到堆为空
            auto node = pq.top(); // 剩余节点中的最小节点
            pq.pop();
            if (node->next) { // 下一个节点不为空
                pq.push(node->next); // 下一个节点有可能是最小节点，入堆
            }
            cur->next = node; // 把 node 添加到新链表的末尾
            cur = cur->next; // 准备合并下一个节点
        }
        return dummy.next; // 哨兵节点的下一个节点就是新链表的头节点
    }
};
```

```go [sol-Go]
func mergeKLists(lists []*ListNode) *ListNode {
    h := hp{}
    for _, head := range lists {
        if head != nil {
            h = append(h, head) // 把所有非空链表的头节点入堆
        }
    }
    heap.Init(&h) // 堆化

    dummy := &ListNode{} // 哨兵节点，作为合并后链表头节点的前一个节点
    cur := dummy
    for len(h) > 0 { // 循环直到堆为空
        node := heap.Pop(&h).(*ListNode) // 剩余节点中的最小节点
        if node.Next != nil { // 下一个节点不为空
            heap.Push(&h, node.Next) // 下一个节点有可能是最小节点，入堆
        }
        cur.Next = node // 把 node 添加到新链表的末尾
        cur = cur.Next // 准备合并下一个节点
    }
    return dummy.Next // 哨兵节点的下一个节点就是新链表的头节点
}

type hp []*ListNode
func (h hp) Len() int           { return len(h) }
func (h hp) Less(i, j int) bool { return h[i].Val < h[j].Val } // 最小堆
func (h hp) Swap(i, j int)      { h[i], h[j] = h[j], h[i] }
func (h *hp) Push(v any)        { *h = append(*h, v.(*ListNode)) }
func (h *hp) Pop() any          { a := *h; v := a[len(a)-1]; *h = a[:len(a)-1]; return v }
```

```js [sol-JavaScript]
var mergeKLists = function(lists) {
    const pq = new MinPriorityQueue(node => node.val);
    for (const head of lists) {
        if (head) {
            pq.enqueue(head); // 把所有非空链表的头节点入堆
        }
    }

    const dummy = new ListNode(); // 哨兵节点，作为合并后链表头节点的前一个节点
    let cur = dummy;
    while (!pq.isEmpty()) { // 循环直到堆为空
        const node = pq.dequeue(); // 剩余节点中的最小节点
        if (node.next) { // 下一个节点不为空
            pq.enqueue(node.next); // 下一个节点有可能是最小节点，入堆
        }
        cur.next = node; // 把 node 添加到新链表的末尾
        cur = cur.next; // 准备合并下一个节点
    }
    return dummy.next; // 哨兵节点的下一个节点就是新链表的头节点
};
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(L\log m)$，其中 $m$ 为 $\textit{lists}$ 的长度，$L$ 为所有链表的长度之和。
- 空间复杂度：$\mathcal{O}(m)$。堆中至多有 $m$ 个元素。

## 方法二：分治

暴力做法是，按照 [21. 合并两个有序链表](https://leetcode.cn/problems/merge-two-sorted-lists/) 的 [题解思路](https://leetcode.cn/problems/merge-two-sorted-lists/solution/liang-chong-fang-fa-die-dai-di-gui-pytho-wf75)，先合并前两个链表，再把得到的新链表和第三个链表合并，再和第四个链表合并，依此类推。

但是这种做法，平均每个节点会参与到 $\mathcal{O}(m)$ 次合并中（用 $(1+2+\cdots +m)/m$ 粗略估计），所以总的时间复杂度为 $\mathcal{O}(L\cdot m)$。

一个巧妙的思路是，把 $\textit{lists}$ 一分为二（尽量**均分**），先合并前一半的链表，再合并后一半的链表，然后把这两个链表合并成最终的链表。如何合并前一半的链表呢？我们可以继续一分为二。如此分下去直到只有一个链表，此时无需合并。

我们可以写一个递归来完成上述逻辑，如果你对递归头晕，请看[【基础算法精讲 09】](https://www.bilibili.com/video/BV1UD4y1Y769/)。

按照一分为二再合并的逻辑，递归像是在后序遍历一棵平衡二叉树。由于平衡树的高度是 $\mathcal{O}(\log m)$，所以每个链表节点只会出现在 $\mathcal{O}(\log m)$ 次合并中！这样就做到了更快的 $\mathcal{O}(L\log m)$ 时间。

### 写法一：递归

```py [sol-Python3]
class Solution:
    # 21. 合并两个有序链表
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

    def mergeKLists(self, lists: List[Optional[ListNode]]) -> Optional[ListNode]:
        m = len(lists)
        if m == 0:
            return None
        if m == 1:
            return lists[0]  # 无需合并，直接返回
        left = self.mergeKLists(lists[:m // 2])  # 合并左半部分
        right = self.mergeKLists(lists[m // 2:])  # 合并右半部分
        return self.mergeTwoLists(left, right)  # 最后把左半和右半合并
```

```java [sol-Java]
class Solution {
    public ListNode mergeKLists(ListNode[] lists) {
        return mergeKLists(lists, 0, lists.length);
    }

    // 合并从 lists[i] 到 lists[j-1] 的链表
    private ListNode mergeKLists(ListNode[] lists, int i, int j) {
        int m = j - i;
        if (m == 0) {
            return null; // 注意输入的 lists 可能是空的
        }
        if (m == 1) {
            return lists[i]; // 无需合并，直接返回
        }
        ListNode left = mergeKLists(lists, i, i + m / 2); // 合并左半部分
        ListNode right = mergeKLists(lists, i + m / 2, j); // 合并右半部分
        return mergeTwoLists(left, right); // 最后把左半和右半合并
    }

    // 21. 合并两个有序链表
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
    // 21. 合并两个有序链表
    ListNode* mergeTwoLists(ListNode* list1, ListNode* list2) {
        ListNode dummy{}; // 用哨兵节点简化代码逻辑
        auto cur = &dummy; // cur 指向新链表的末尾
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

    // 合并从 lists[i] 到 lists[j-1] 的链表
    ListNode* mergeKLists(vector<ListNode*>& lists, int i, int j) {
        int m = j - i;
        if (m == 0) {
            return nullptr; // 注意输入的 lists 可能是空的
        }
        if (m == 1) {
            return lists[i]; // 无需合并，直接返回
        }
        auto left = mergeKLists(lists, i, i + m / 2); // 合并左半部分
        auto right = mergeKLists(lists, i + m / 2, j); // 合并右半部分
        return mergeTwoLists(left, right); // 最后把左半和右半合并
    }

public:
    ListNode* mergeKLists(vector<ListNode*>& lists) {
        return mergeKLists(lists, 0, lists.size());
    }
};
```

```go [sol-Go]
// 21. 合并两个有序链表
func mergeTwoLists(list1, list2 *ListNode) *ListNode {
    dummy := &ListNode{} // 用哨兵节点简化代码逻辑
    cur := dummy // cur 指向新链表的末尾
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

func mergeKLists(lists []*ListNode) *ListNode {
    m := len(lists)
    if m == 0 { // 注意输入的 lists 可能是空的
        return nil
    }
    if m == 1 { // 无需合并，直接返回
        return lists[0]
    }
    left := mergeKLists(lists[:m/2]) // 合并左半部分
    right := mergeKLists(lists[m/2:]) // 合并右半部分
    return mergeTwoLists(left, right) // 最后把左半和右半合并
}
```

```js [sol-JavaScript]
// 21. 合并两个有序链表
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
    cur.next = list1 ? list1 : list2; // 拼接剩余链表
    return dummy.next;
};

var mergeKLists = function(lists) {
    // 合并从 lists[i] 到 lists[j-1] 的链表
    function dfs(i, j) {
        const m = j - i;
        if (m === 0) {
            return null; // 注意输入的 lists 可能是空的
        }
        if (m === 1) {
            return lists[i]; // 无需合并，直接返回
        }
        const left = dfs(i, i + (m >> 1)); // 合并左半部分
        const right = dfs(i + (m >> 1), j); // 合并右半部分
        return mergeTwoLists(left, right); // 最后把左半和右半合并
    }
    return dfs(0, lists.length);
};
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(L\log m)$，其中 $m$ 为 $\textit{lists}$ 的长度，$L$ 为所有链表的长度之和。每个节点参与链表合并的次数为 $\mathcal{O}(\log m)$ 次，一共有 $L$ 个节点，所以总的时间复杂度为 $\mathcal{O}(L\log m)$。
- 空间复杂度：$\mathcal{O}(\log m)$。递归深度为 $\mathcal{O}(\log m)$，需要 $\mathcal{O}(\log m)$ 的栈空间。Python 忽略切片产生的额外空间。

### 写法二：迭代

直接自底向上合并链表：

- 两两合并：把 $\textit{lists}[0]$ 和 $\textit{lists}[1]$ 合并，合并后的链表保存在 $\textit{lists}[0]$ 中；把 $\textit{lists}[2]$ 和 $\textit{lists}[3]$ 合并，合并后的链表保存在 $\textit{lists}[2]$ 中；依此类推。
- 四四合并：把 $\textit{lists}[0]$ 和 $\textit{lists}[2]$ 合并（相当于合并前四条链表），合并后的链表保存在 $\textit{lists}[0]$ 中；把 $\textit{lists}[4]$ 和 $\textit{lists}[6]$ 合并，合并后的链表保存在 $\textit{lists}[4]$ 中；依此类推。
- 八八合并：把 $\textit{lists}[0]$ 和 $\textit{lists}[4]$ 合并（相当于合并前八条链表），合并后的链表保存在 $\textit{lists}[0]$ 中；把 $\textit{lists}[8]$ 和 $\textit{lists}[12]$ 合并，合并后的链表保存在 $\textit{lists}[8]$ 中；依此类推。
- 依此类推，直到所有链表都合并到 $\textit{lists}[0]$ 中。最后返回 $\textit{lists}[0]$。

```py [sol-Python3]
class Solution:
    # 21. 合并两个有序链表
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

    def mergeKLists(self, lists: List[Optional[ListNode]]) -> Optional[ListNode]:
        m = len(lists)
        if m == 0:
            return None
        step = 1
        while step < m:
            for i in range(0, m - step, step * 2):
                lists[i] = self.mergeTwoLists(lists[i], lists[i + step])
            step *= 2
        return lists[0]
```

```java [sol-Java]
class Solution {
    public ListNode mergeKLists(ListNode[] lists) {
        int m = lists.length;
        if (m == 0) {
            return null;
        }
        for (int step = 1; step < m; step *= 2) {
            for (int i = 0; i < m - step; i += step * 2) {
                lists[i] = mergeTwoLists(lists[i], lists[i + step]);
            }
        }
        return lists[0];
    }

    // 21. 合并两个有序链表
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
    // 21. 合并两个有序链表
    ListNode* mergeTwoLists(ListNode* list1, ListNode* list2) {
        ListNode dummy{}; // 用哨兵节点简化代码逻辑
        auto cur = &dummy; // cur 指向新链表的末尾
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
    ListNode* mergeKLists(vector<ListNode*>& lists) {
        int m = lists.size();
        if (m == 0) {
            return nullptr;
        }
        for (int step = 1; step < m; step *= 2) {
            for (int i = 0; i < m - step; i += step * 2) {
                lists[i] = mergeTwoLists(lists[i], lists[i + step]);
            }
        }
        return lists[0];
    }
};
```

```go [sol-Go]
// 21. 合并两个有序链表
func mergeTwoLists(list1, list2 *ListNode) *ListNode {
    dummy := &ListNode{} // 用哨兵节点简化代码逻辑
    cur := dummy // cur 指向新链表的末尾
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

func mergeKLists(lists []*ListNode) *ListNode {
    m := len(lists)
    if m == 0 {
        return nil
    }
    for step := 1; step < m; step *= 2 {
        for i := 0; i < m-step; i += step * 2 {
            lists[i] = mergeTwoLists(lists[i], lists[i+step])
        }
    }
    return lists[0]
}
```

```js [sol-JavaScript]
// 21. 合并两个有序链表
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
    cur.next = list1 ? list1 : list2; // 拼接剩余链表
    return dummy.next;
};

var mergeKLists = function(lists) {
    const m = lists.length;
    if (m === 0) {
        return null;
    }
    for (let step = 1; step < m; step *= 2) {
        for (let i = 0; i < m - step; i += step * 2) {
            lists[i] = mergeTwoLists(lists[i], lists[i + step]);
        }
    }
    return lists[0];
};
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(L\log m)$，其中 $m$ 为 $\textit{lists}$ 的长度，$L$ 为所有链表的长度之和。外层关于 $\textit{step}$ 的循环是 $\mathcal{O}(\log m)$ 次，内层相当于把每个链表节点都遍历了一遍，是 $\mathcal{O}(L)$ 的，所以总的时间复杂度为 $\mathcal{O}(L\log m)$。
- 空间复杂度：$\mathcal{O}(1)$。

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

## 方法一：最小堆

合并后的第一个节点 $\textit{first}$，一定是某个链表的**头节点**（因为链表已按升序排列）。

合并后的第二个节点，可能是某个链表的头节点，也可能是 $\textit{first}$ 的下一个节点。

例如有三个链表 `1->2->5, 3->4->6, 4->5->6`，找到第一个节点 `1` 之后，第二个节点不是另一个链表的头节点，而是节点 `1` 的下一个节点 `2`。

按照这个过程继续思考，每当我们找到一个节点值最小的节点 $x$，就把节点 $x.\textit{next}$ 加入「可能是最小节点」的集合中。

因此，我们需要一个数据结构，它支持：

- 从数据结构中找到并移除最小节点。
- 插入节点。

这可以用**最小堆**实现。初始把所有链表的头节点入堆，然后不断弹出堆中最小节点 $x$，如果 $x.\textit{next}$ 不为空就加入堆中。循环直到堆为空。把弹出的节点按顺序拼接起来，就得到了答案。

实现时，可以用**哨兵节点**简化代码，具体请看[【基础算法精讲 06】](https://www.bilibili.com/video/BV1sd4y1x7KN/)。

```py [sol-Python3]
# 让堆可以比较节点大小（不改 __lt__ 的写法见 Python3 写法二）
ListNode.__lt__ = lambda a, b: a.val < b.val  

class Solution:
    def mergeKLists(self, lists: List[Optional[ListNode]]) -> Optional[ListNode]:
        cur = dummy = ListNode()  # 哨兵节点，作为合并后链表头节点的前一个节点
        h = [head for head in lists if head]  # 把所有非空链表的头节点入堆
        heapify(h)  # 堆化
        while h:  # 循环直到堆为空
            node = heappop(h)  # 剩余节点中的最小节点
            if node.next:  # 下一个节点不为空
                heappush(h, node.next)  # 下一个节点有可能是最小节点，入堆
            cur.next = node  # 把 node 添加到新链表的末尾
            cur = cur.next  # 准备合并下一个节点
        return dummy.next  # 哨兵节点的下一个节点就是新链表的头节点
```

```py [sol-Python3 写法二]
class Solution:
    def mergeKLists(self, lists: List[Optional[ListNode]]) -> Optional[ListNode]:
        cur = dummy = ListNode()  # 哨兵节点，作为合并后链表头节点的前一个节点
        # 额外保存 i，这样即使 val 相同也不会去比 ListNode，避免报错）
        h = [(head.val, i, head) for i, head in enumerate(lists) if head]  # 把所有非空链表的头节点入堆
        heapify(h)  # 堆化
        while h:  # 循环直到堆为空
            _, i, node = heappop(h)  # 剩余节点中的最小节点
            if node.next:  # 下一个节点不为空
                heappush(h, (node.next.val, i, node.next))  # 下一个节点有可能是最小节点，入堆
            cur.next = node  # 把 node 添加到新链表的末尾
            cur = cur.next  # 准备合并下一个节点
        return dummy.next  # 哨兵节点的下一个节点就是新链表的头节点
```

```java [sol-Java]
class Solution {
    public ListNode mergeKLists(ListNode[] lists) {
        PriorityQueue<ListNode> pq = new PriorityQueue<>((a, b) -> a.val - b.val);
        for (ListNode head : lists) {
            if (head != null) {
                pq.offer(head); // 把所有非空链表的头节点入堆
            }
        }

        ListNode dummy = new ListNode(); // 哨兵节点，作为合并后链表头节点的前一个节点
        ListNode cur = dummy;
        while (!pq.isEmpty()) { // 循环直到堆为空
            ListNode node = pq.poll(); // 剩余节点中的最小节点
            if (node.next != null) { // 下一个节点不为空
                pq.offer(node.next); // 下一个节点有可能是最小节点，入堆
            }
            cur.next = node; // 把 node 添加到新链表的末尾
            cur = cur.next; // 准备合并下一个节点
        }
        return dummy.next; // 哨兵节点的下一个节点就是新链表的头节点
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    ListNode* mergeKLists(vector<ListNode*>& lists) {
        auto cmp = [](const ListNode* a, const ListNode* b) {
            return a->val > b->val; // 最小堆
        };
        priority_queue<ListNode*, vector<ListNode*>, decltype(cmp)> pq;
        for (auto head : lists) {
            if (head) {
                pq.push(head); // 把所有非空链表的头节点入堆
            }
        }

        ListNode dummy{}; // 哨兵节点，作为合并后链表头节点的前一个节点
        auto cur = &dummy;
        while (!pq.empty()) { // 循环直到堆为空
            auto node = pq.top(); // 剩余节点中的最小节点
            pq.pop();
            if (node->next) { // 下一个节点不为空
                pq.push(node->next); // 下一个节点有可能是最小节点，入堆
            }
            cur->next = node; // 把 node 添加到新链表的末尾
            cur = cur->next; // 准备合并下一个节点
        }
        return dummy.next; // 哨兵节点的下一个节点就是新链表的头节点
    }
};
```

```go [sol-Go]
func mergeKLists(lists []*ListNode) *ListNode {
    h := hp{}
    for _, head := range lists {
        if head != nil {
            h = append(h, head) // 把所有非空链表的头节点入堆
        }
    }
    heap.Init(&h) // 堆化

    dummy := &ListNode{} // 哨兵节点，作为合并后链表头节点的前一个节点
    cur := dummy
    for len(h) > 0 { // 循环直到堆为空
        node := heap.Pop(&h).(*ListNode) // 剩余节点中的最小节点
        if node.Next != nil { // 下一个节点不为空
            heap.Push(&h, node.Next) // 下一个节点有可能是最小节点，入堆
        }
        cur.Next = node // 把 node 添加到新链表的末尾
        cur = cur.Next // 准备合并下一个节点
    }
    return dummy.Next // 哨兵节点的下一个节点就是新链表的头节点
}

type hp []*ListNode
func (h hp) Len() int           { return len(h) }
func (h hp) Less(i, j int) bool { return h[i].Val < h[j].Val } // 最小堆
func (h hp) Swap(i, j int)      { h[i], h[j] = h[j], h[i] }
func (h *hp) Push(v any)        { *h = append(*h, v.(*ListNode)) }
func (h *hp) Pop() any          { a := *h; v := a[len(a)-1]; *h = a[:len(a)-1]; return v }
```

```js [sol-JavaScript]
var mergeKLists = function(lists) {
    const pq = new MinPriorityQueue(node => node.val);
    for (const head of lists) {
        if (head) {
            pq.enqueue(head); // 把所有非空链表的头节点入堆
        }
    }

    const dummy = new ListNode(); // 哨兵节点，作为合并后链表头节点的前一个节点
    let cur = dummy;
    while (!pq.isEmpty()) { // 循环直到堆为空
        const node = pq.dequeue(); // 剩余节点中的最小节点
        if (node.next) { // 下一个节点不为空
            pq.enqueue(node.next); // 下一个节点有可能是最小节点，入堆
        }
        cur.next = node; // 把 node 添加到新链表的末尾
        cur = cur.next; // 准备合并下一个节点
    }
    return dummy.next; // 哨兵节点的下一个节点就是新链表的头节点
};
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(L\log m)$，其中 $m$ 为 $\textit{lists}$ 的长度，$L$ 为所有链表的长度之和。
- 空间复杂度：$\mathcal{O}(m)$。堆中至多有 $m$ 个元素。

## 方法二：分治

暴力做法是，按照 [21. 合并两个有序链表](https://leetcode.cn/problems/merge-two-sorted-lists/) 的 [题解思路](https://leetcode.cn/problems/merge-two-sorted-lists/solution/liang-chong-fang-fa-die-dai-di-gui-pytho-wf75)，先合并前两个链表，再把得到的新链表和第三个链表合并，再和第四个链表合并，依此类推。

但是这种做法，平均每个节点会参与到 $\mathcal{O}(m)$ 次合并中（用 $(1+2+\cdots +m)/m$ 粗略估计），所以总的时间复杂度为 $\mathcal{O}(L\cdot m)$。

一个巧妙的思路是，把 $\textit{lists}$ 一分为二（尽量**均分**），先合并前一半的链表，再合并后一半的链表，然后把这两个链表合并成最终的链表。如何合并前一半的链表呢？我们可以继续一分为二。如此分下去直到只有一个链表，此时无需合并。

我们可以写一个递归来完成上述逻辑，如果你对递归头晕，请看[【基础算法精讲 09】](https://www.bilibili.com/video/BV1UD4y1Y769/)。

按照一分为二再合并的逻辑，递归像是在后序遍历一棵平衡二叉树。由于平衡树的高度是 $\mathcal{O}(\log m)$，所以每个链表节点只会出现在 $\mathcal{O}(\log m)$ 次合并中！这样就做到了更快的 $\mathcal{O}(L\log m)$ 时间。

### 写法一：递归

```py [sol-Python3]
class Solution:
    # 21. 合并两个有序链表
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

    def mergeKLists(self, lists: List[Optional[ListNode]]) -> Optional[ListNode]:
        m = len(lists)
        if m == 0:
            return None
        if m == 1:
            return lists[0]  # 无需合并，直接返回
        left = self.mergeKLists(lists[:m // 2])  # 合并左半部分
        right = self.mergeKLists(lists[m // 2:])  # 合并右半部分
        return self.mergeTwoLists(left, right)  # 最后把左半和右半合并
```

```java [sol-Java]
class Solution {
    public ListNode mergeKLists(ListNode[] lists) {
        return mergeKLists(lists, 0, lists.length);
    }

    // 合并从 lists[i] 到 lists[j-1] 的链表
    private ListNode mergeKLists(ListNode[] lists, int i, int j) {
        int m = j - i;
        if (m == 0) {
            return null; // 注意输入的 lists 可能是空的
        }
        if (m == 1) {
            return lists[i]; // 无需合并，直接返回
        }
        ListNode left = mergeKLists(lists, i, i + m / 2); // 合并左半部分
        ListNode right = mergeKLists(lists, i + m / 2, j); // 合并右半部分
        return mergeTwoLists(left, right); // 最后把左半和右半合并
    }

    // 21. 合并两个有序链表
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
    // 21. 合并两个有序链表
    ListNode* mergeTwoLists(ListNode* list1, ListNode* list2) {
        ListNode dummy{}; // 用哨兵节点简化代码逻辑
        auto cur = &dummy; // cur 指向新链表的末尾
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

    // 合并从 lists[i] 到 lists[j-1] 的链表
    ListNode* mergeKLists(vector<ListNode*>& lists, int i, int j) {
        int m = j - i;
        if (m == 0) {
            return nullptr; // 注意输入的 lists 可能是空的
        }
        if (m == 1) {
            return lists[i]; // 无需合并，直接返回
        }
        auto left = mergeKLists(lists, i, i + m / 2); // 合并左半部分
        auto right = mergeKLists(lists, i + m / 2, j); // 合并右半部分
        return mergeTwoLists(left, right); // 最后把左半和右半合并
    }

public:
    ListNode* mergeKLists(vector<ListNode*>& lists) {
        return mergeKLists(lists, 0, lists.size());
    }
};
```

```go [sol-Go]
// 21. 合并两个有序链表
func mergeTwoLists(list1, list2 *ListNode) *ListNode {
    dummy := &ListNode{} // 用哨兵节点简化代码逻辑
    cur := dummy // cur 指向新链表的末尾
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

func mergeKLists(lists []*ListNode) *ListNode {
    m := len(lists)
    if m == 0 { // 注意输入的 lists 可能是空的
        return nil
    }
    if m == 1 { // 无需合并，直接返回
        return lists[0]
    }
    left := mergeKLists(lists[:m/2]) // 合并左半部分
    right := mergeKLists(lists[m/2:]) // 合并右半部分
    return mergeTwoLists(left, right) // 最后把左半和右半合并
}
```

```js [sol-JavaScript]
// 21. 合并两个有序链表
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
    cur.next = list1 ? list1 : list2; // 拼接剩余链表
    return dummy.next;
};

var mergeKLists = function(lists) {
    // 合并从 lists[i] 到 lists[j-1] 的链表
    function dfs(i, j) {
        const m = j - i;
        if (m === 0) {
            return null; // 注意输入的 lists 可能是空的
        }
        if (m === 1) {
            return lists[i]; // 无需合并，直接返回
        }
        const left = dfs(i, i + (m >> 1)); // 合并左半部分
        const right = dfs(i + (m >> 1), j); // 合并右半部分
        return mergeTwoLists(left, right); // 最后把左半和右半合并
    }
    return dfs(0, lists.length);
};
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(L\log m)$，其中 $m$ 为 $\textit{lists}$ 的长度，$L$ 为所有链表的长度之和。每个节点参与链表合并的次数为 $\mathcal{O}(\log m)$ 次，一共有 $L$ 个节点，所以总的时间复杂度为 $\mathcal{O}(L\log m)$。
- 空间复杂度：$\mathcal{O}(\log m)$。递归深度为 $\mathcal{O}(\log m)$，需要 $\mathcal{O}(\log m)$ 的栈空间。Python 忽略切片产生的额外空间。

### 写法二：迭代

直接自底向上合并链表：

- 两两合并：把 $\textit{lists}[0]$ 和 $\textit{lists}[1]$ 合并，合并后的链表保存在 $\textit{lists}[0]$ 中；把 $\textit{lists}[2]$ 和 $\textit{lists}[3]$ 合并，合并后的链表保存在 $\textit{lists}[2]$ 中；依此类推。
- 四四合并：把 $\textit{lists}[0]$ 和 $\textit{lists}[2]$ 合并（相当于合并前四条链表），合并后的链表保存在 $\textit{lists}[0]$ 中；把 $\textit{lists}[4]$ 和 $\textit{lists}[6]$ 合并，合并后的链表保存在 $\textit{lists}[4]$ 中；依此类推。
- 八八合并：把 $\textit{lists}[0]$ 和 $\textit{lists}[4]$ 合并（相当于合并前八条链表），合并后的链表保存在 $\textit{lists}[0]$ 中；把 $\textit{lists}[8]$ 和 $\textit{lists}[12]$ 合并，合并后的链表保存在 $\textit{lists}[8]$ 中；依此类推。
- 依此类推，直到所有链表都合并到 $\textit{lists}[0]$ 中。最后返回 $\textit{lists}[0]$。

```py [sol-Python3]
class Solution:
    # 21. 合并两个有序链表
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

    def mergeKLists(self, lists: List[Optional[ListNode]]) -> Optional[ListNode]:
        m = len(lists)
        if m == 0:
            return None
        step = 1
        while step < m:
            for i in range(0, m - step, step * 2):
                lists[i] = self.mergeTwoLists(lists[i], lists[i + step])
            step *= 2
        return lists[0]
```

```java [sol-Java]
class Solution {
    public ListNode mergeKLists(ListNode[] lists) {
        int m = lists.length;
        if (m == 0) {
            return null;
        }
        for (int step = 1; step < m; step *= 2) {
            for (int i = 0; i < m - step; i += step * 2) {
                lists[i] = mergeTwoLists(lists[i], lists[i + step]);
            }
        }
        return lists[0];
    }

    // 21. 合并两个有序链表
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
    // 21. 合并两个有序链表
    ListNode* mergeTwoLists(ListNode* list1, ListNode* list2) {
        ListNode dummy{}; // 用哨兵节点简化代码逻辑
        auto cur = &dummy; // cur 指向新链表的末尾
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
    ListNode* mergeKLists(vector<ListNode*>& lists) {
        int m = lists.size();
        if (m == 0) {
            return nullptr;
        }
        for (int step = 1; step < m; step *= 2) {
            for (int i = 0; i < m - step; i += step * 2) {
                lists[i] = mergeTwoLists(lists[i], lists[i + step]);
            }
        }
        return lists[0];
    }
};
```

```go [sol-Go]
// 21. 合并两个有序链表
func mergeTwoLists(list1, list2 *ListNode) *ListNode {
    dummy := &ListNode{} // 用哨兵节点简化代码逻辑
    cur := dummy // cur 指向新链表的末尾
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

func mergeKLists(lists []*ListNode) *ListNode {
    m := len(lists)
    if m == 0 {
        return nil
    }
    for step := 1; step < m; step *= 2 {
        for i := 0; i < m-step; i += step * 2 {
            lists[i] = mergeTwoLists(lists[i], lists[i+step])
        }
    }
    return lists[0]
}
```

```js [sol-JavaScript]
// 21. 合并两个有序链表
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
    cur.next = list1 ? list1 : list2; // 拼接剩余链表
    return dummy.next;
};

var mergeKLists = function(lists) {
    const m = lists.length;
    if (m === 0) {
        return null;
    }
    for (let step = 1; step < m; step *= 2) {
        for (let i = 0; i < m - step; i += step * 2) {
            lists[i] = mergeTwoLists(lists[i], lists[i + step]);
        }
    }
    return lists[0];
};
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(L\log m)$，其中 $m$ 为 $\textit{lists}$ 的长度，$L$ 为所有链表的长度之和。外层关于 $\textit{step}$ 的循环是 $\mathcal{O}(\log m)$ 次，内层相当于把每个链表节点都遍历了一遍，是 $\mathcal{O}(L)$ 的，所以总的时间复杂度为 $\mathcal{O}(L\log m)$。
- 空间复杂度：$\mathcal{O}(1)$。

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

## 本地原创解析

### 1. 题意重述

本题来自 `五、堆（优先队列） / §5.2 进阶`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `五、堆（优先队列） / §5.2 进阶`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
