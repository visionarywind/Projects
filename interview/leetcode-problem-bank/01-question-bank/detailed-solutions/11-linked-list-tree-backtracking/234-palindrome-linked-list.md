# 234. 回文链表

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/palindrome-linked-list/
- 题目 slug：`palindrome-linked-list`
- 来源专题：链表、树与回溯
- 来源分类路径：一、链表 / §1.6 快慢指针
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/palindrome-linked-list/solutions/2952645/o1-kong-jian-zuo-fa-xun-zhao-zhong-jian-rv0f3/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：递归 / 寻找中间节点+反转链表（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/palindrome-linked-list/solutions/2952645/o1-kong-jian-zuo-fa-xun-zhao-zhong-jian-rv0f3/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`o1-kong-jian-zuo-fa-xun-zhao-zhong-jian-rv0f3`
- topic id：`2952645`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:30:25 +0800

## 前言

回想一下，怎么判断一个字符串是不是回文串？我们可以从最左最右开始，比较第一个字母和最后一个字母是不是一样的，一样的话，就继续比较第二个字母和倒数第二个字母，依此类推。这个过程会从左到右遍历字符串，以及从右到左遍历字符串。

对于链表，如何从右到左遍历呢？

## 方法一：递归

递归的「归」的过程，就是从右到左遍历链表的过程。

下面这个递归函数可以从右到左打印链表的节点值：

```py
def f(node):
    if node is None:
        return
    f(node.next)
    print(node.val)  # 注：如果把这行代码移到 f(node.next) 的上面，就是从左到右打印

f(head)
```

在「归」的过程中，用另一个指针 $\textit{left}$ 从左到右遍历链表，就可以比较对称位置的值是否相等了。

> **注**：递归做法效率比较低，更快的做法见方法二。

```py [sol-Python3]
class Solution:
    def isPalindrome(self, head: Optional[ListNode]) -> bool:
        left = head

        def is_pal(right: Optional[ListNode]) -> bool:
            # 「递」，先把 right 移到链表末尾
            if right.next and not is_pal(right.next):
                return False
            # 「归」的过程就是在从右到左遍历链表
            nonlocal left
            if left.val != right.val:
                return False
            left = left.next  # left 往右走
            return True  # 归，right 会往左走

        return is_pal(head)
```

```java [sol-Java]
class Solution {
    private ListNode left;

    public boolean isPalindrome(ListNode head) {
        left = head;
        return isPal(head);
    }

    private boolean isPal(ListNode right) {
        // 「递」，先把 right 移到链表末尾
        if (right.next != null && !isPal(right.next)) {
            return false;
        }
        // 「归」的过程就是在从右到左遍历链表
        if (left.val != right.val) {
            return false;
        }
        left = left.next; // left 往右走
        return true; // 归，right 会往左走
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    bool isPalindrome(ListNode* head) {
        ListNode* left = head;

        // lambda 递归函数
        auto is_pal = [&](this auto&& is_pal, ListNode* right) -> bool {
            // 「递」，先把 right 移到链表末尾
            if (right->next && !is_pal(right->next)) {
                return false;
            }
            // 「归」的过程就是在从右到左遍历链表
            if (left->val != right->val) {
                return false;
            }
            left = left->next; // left 往右走
            return true; // 归，right 会往左走
        };

        return is_pal(head);
    }
};
```

```c [sol-C]
struct ListNode* left;

bool is_pal(struct ListNode* right) {
    // 「递」，先把 right 移到链表末尾
    if (right->next && !is_pal(right->next)) {
        return false;
    }
    // 「归」的过程就是在从右到左遍历链表
    if (left->val != right->val) {
        return false;
    }
    left = left->next; // left 往右走
    return true; // 归，right 会往左走
}

bool isPalindrome(struct ListNode* head) {
    left = head;
    return is_pal(head);
}
```

```go [sol-Go]
func isPalindrome(head *ListNode) bool {
    left := head

    var isPal func(*ListNode) bool
    isPal = func(right *ListNode) bool {
        // 「递」，先把 right 移到链表末尾
        if right.Next != nil && !isPal(right.Next) {
            return false
        }
        // 「归」的过程就是在从右到左遍历链表
        if left.Val != right.Val {
            return false
        }
        left = left.Next // left 往右走
        return true // 归，right 会往左走
    }

    return isPal(head)
}
```

```js [sol-JavaScript]
var isPalindrome = function(head) {
    let left = head;

    function isPal(right) {
        // 「递」，先把 right 移到链表末尾
        if (right.next && !isPal(right.next)) {
            return false;
        }
        // 「归」的过程就是在从右到左遍历链表
        if (left.val !== right.val) {
            return false;
        }
        left = left.next; // left 往右走
        return true; // 归，right 会往左走
    }

    return isPal(head);
};
```

```rust [sol-Rust]
impl Solution {
    fn is_pal(left: &mut &Option<Box<ListNode>>, right: &Option<Box<ListNode>>) -> bool {
        if let Some(r) = right {
            // 「递」，先把 right 移到链表末尾
            if !Self::is_pal(left, &r.next) {
                return false;
            }
            // 「归」的过程就是在从右到左遍历链表
            let l = left.as_ref().unwrap();
            if l.val != r.val {
                return false;
            }
            *left = &l.next; // left 往右走
        }
        true // 归，right 会往左走
    }

    pub fn is_palindrome(head: Option<Box<ListNode>>) -> bool {
        let mut left = &head;
        Self::is_pal(&mut left, &head)
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是链表的长度（节点个数）。
- 空间复杂度：$\mathcal{O}(n)$。递归需要 $\mathcal{O}(n)$ 的栈空间。

## 方法二：迭代

**前置题目**：

- [876. 链表的中间结点](https://leetcode.cn/problems/middle-of-the-linked-list/)，[视频讲解【基础算法精讲 07】](https://www.bilibili.com/video/BV1KG4y1G7cu/)
- [206. 反转链表](https://leetcode.cn/problems/reverse-linked-list/)，[视频讲解【基础算法精讲 06】](https://www.bilibili.com/video/BV1sd4y1x7KN/)

首先，找链表的中间节点：

- 如果链表有奇数个节点，找正中间的节点。![lc-midlist1.jpg](https://pic.leetcode.cn/1729048747-htsJVo-lc-midlist1.jpg)
- 如果链表有偶数个节点，找正中间右边的节点。![lc-midlist2.jpg](https://pic.leetcode.cn/1729048754-HLNfqE-lc-midlist2.jpg)

然后，**把中间节点到链表末尾反转**。如上图，反转后得到链表 $6\to 5\to 4$，其头节点记作 $\textit{head}_2$。这样我们就能从 $\textit{head}_2$ 开始，依次访问原链表的最后一个节点、倒数第二个节点、倒数第三个节点……

最后，同时遍历 $\textit{head}$ 和 $\textit{head}_2$ 这两个链表，每次循环判断 $\textit{head}.\textit{val}$ 是否等于 $\textit{head}_2.\textit{val}$，若不相等，则返回 $\texttt{false}$。循环直到 $\textit{head}_2$ 链表遍历结束。如果循环中没有返回 $\texttt{false}$，说明链表是回文的，返回 $\texttt{true}$。

⚠**注意**：第一张图中的 $2\to 3$，在反转链表后，并不会断开。第一张图反转链表后，我们得到了两条链表，一条是 $1\to 2\to 3$，另一条是 $5\to 4\to 3$。

⚠**注意**：第二张图中的 $3\to 4$，在反转链表后，并不会断开。第二张图反转链表后，我们得到了两条链表，一条是 $1\to 2\to 3\to 4$，另一条是 $6\to 5\to 4$。这意味着下面代码在写循环的时候，循环条件要判断 $\textit{head}_2$ 是否为空而不是 $\textit{head}$ 是否为空。如果判断 $\textit{head}$ 是否为空，会错误地多循环一次，导致访问 `head2.val` 出现空指针异常。

#### 答疑

**问**：为什么不能反转**整个**链表？

**答**：注意我们还要从 $\textit{head}$ 开始，从左到右遍历链表。如果反转整个链表，链表前半段的结构就被破坏了，无法从 $\textit{head}$ 开始访问后续节点。

下面的代码修改了输入的链表。复原输入的写法可以参考【Python3 写法二】。

```py [sol-Python3]
class Solution:
    # 876. 链表的中间结点
    def middleNode(self, head: Optional[ListNode]) -> Optional[ListNode]:
        slow = fast = head
        while fast and fast.next:
            slow = slow.next
            fast = fast.next.next
        return slow

    # 206. 反转链表
    def reverseList(self, head: Optional[ListNode]) -> Optional[ListNode]:
        pre, cur = None, head
        while cur:
            nxt = cur.next
            cur.next = pre
            pre = cur
            cur = nxt
        return pre

    def isPalindrome(self, head: Optional[ListNode]) -> bool:
        mid = self.middleNode(head)
        head2 = self.reverseList(mid)
        while head2:
            if head.val != head2.val:  # 不是回文链表
                return False
            head = head.next
            head2 = head2.next
        return True
```

```py [sol-Python3 写法二]
class Solution:
    # 876. 链表的中间结点
    def middleNode(self, head: Optional[ListNode]) -> Optional[ListNode]:
        slow = fast = head
        while fast and fast.next:
            slow = slow.next
            fast = fast.next.next
        return slow

    # 206. 反转链表
    def reverseList(self, head: Optional[ListNode]) -> Optional[ListNode]:
        pre, cur = None, head
        while cur:
            nxt = cur.next
            cur.next = pre
            pre = cur
            cur = nxt
        return pre

    def isPalindrome(self, head: Optional[ListNode]) -> bool:
        mid = self.middleNode(head)
        head2 = h2 = self.reverseList(mid)
        while head2:
            if head.val != head2.val:  # 不是回文链表
                self.reverseList(h2)  # 复原
                return False
            head = head.next
            head2 = head2.next
        self.reverseList(h2)  # 复原
        return True
```

```java [sol-Java]
class Solution {
    public boolean isPalindrome(ListNode head) {
        ListNode mid = middleNode(head);
        ListNode head2 = reverseList(mid);
        while (head2 != null) {
            if (head.val != head2.val) { // 不是回文链表
                return false;
            }
            head = head.next;
            head2 = head2.next;
        }
        return true;
    }

    // 876. 链表的中间结点
    private ListNode middleNode(ListNode head) {
        ListNode slow = head;
        ListNode fast = head;
        while (fast != null && fast.next != null) {
            slow = slow.next;
            fast = fast.next.next;
        }
        return slow;
    }

    // 206. 反转链表
    private ListNode reverseList(ListNode head) {
        ListNode pre = null;
        ListNode cur = head;
        while (cur != null) {
            ListNode nxt = cur.next;
            cur.next = pre;
            pre = cur;
            cur = nxt;
        }
        return pre;
    }
}
```

```cpp [sol-C++]
class Solution {
    // 876. 链表的中间结点
    ListNode* middleNode(ListNode* head) {
        ListNode* slow = head, *fast = head;
        while (fast && fast->next) {
            slow = slow->next;
            fast = fast->next->next;
        }
        return slow;
    }

    // 206. 反转链表
    ListNode* reverseList(ListNode* head) {
        ListNode* pre = nullptr, *cur = head;
        while (cur) {
            ListNode* nxt = cur->next;
            cur->next = pre;
            pre = cur;
            cur = nxt;
        }
        return pre;
    }

public:
    bool isPalindrome(ListNode* head) {
        ListNode* mid = middleNode(head);
        ListNode* head2 = reverseList(mid);
        while (head2) {
            if (head->val != head2->val) { // 不是回文链表
                return false;
            }
            head = head->next;
            head2 = head2->next;
        }
        return true;
    }
};
```

```c [sol-C]
// 876. 链表的中间结点
struct ListNode* middleNode(struct ListNode* head) {
    struct ListNode *slow = head, *fast = head;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    return slow;
}

// 206. 反转链表
struct ListNode* reverseList(struct ListNode* head) {
    struct ListNode *pre = NULL, *cur = head;
    while (cur) {
        struct ListNode* nxt = cur->next;
        cur->next = pre;
        pre = cur;
        cur = nxt;
    }
    return pre;
}

bool isPalindrome(struct ListNode* head) {
    struct ListNode* mid = middleNode(head);
    struct ListNode* head2 = reverseList(mid);
    while (head2) {
        if (head->val != head2->val) { // 不是回文链表
            return false;
        }
        head = head->next;
        head2 = head2->next;
    }
    return true;
}
```

```go [sol-Go]
// 876. 链表的中间结点
func middleNode(head *ListNode) *ListNode {
    slow, fast := head, head
    for fast != nil && fast.Next != nil {
        slow = slow.Next
        fast = fast.Next.Next
    }
    return slow
}

// 206. 反转链表
func reverseList(head *ListNode) *ListNode {
    var pre, cur *ListNode = nil, head
    for cur != nil {
        nxt := cur.Next
        cur.Next = pre
        pre = cur
        cur = nxt
    }
    return pre
}

func isPalindrome(head *ListNode) bool {
    mid := middleNode(head)
    head2 := reverseList(mid)
    for head2 != nil {
        if head.Val != head2.Val { // 不是回文链表
            return false
        }
        head = head.Next
        head2 = head2.Next
    }
    return true
}
```

```js [sol-JavaScript]
// 876. 链表的中间结点
function middleNode(head) {
    let slow = head, fast = head;
    while (fast !== null && fast.next !== null) {
        slow = slow.next;
        fast = fast.next.next;
    }
    return slow;
}

// 206. 反转链表
function reverseList(head) {
    let pre = null, cur = head;
    while (cur !== null) {
        const nxt = cur.next;
        cur.next = pre;
        pre = cur;
        cur = nxt;
    }
    return pre;
}

var isPalindrome = function(head) {
    const mid = middleNode(head);
    let head2 = reverseList(mid);
    while (head2 !== null) {
        if (head.val !== head2.val) { // 不是回文链表
            return false;
        }
        head = head.next;
        head2 = head2.next;
    }
    return true;
};
```

```rust [sol-Rust]
impl Solution {
    // 876. 链表的中间结点
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
        slow.take() // 避免 clone()
    }

    // 206. 反转链表
    fn reverse_list(head: Option<Box<ListNode>>) -> Option<Box<ListNode>> {
        let mut pre = None;
        let mut cur = head;
        while let Some(mut node) = cur {
            let nxt = node.next;
            node.next = pre;
            pre = Some(node);
            cur = nxt;
        }
        pre
    }

    pub fn is_palindrome(mut head: Option<Box<ListNode>>) -> bool {
        // 由于 Self::middle_node 把中间节点 take 出来了，所以反转后 head2 的长度 >= head 的长度
        let mid = Self::middle_node(&head);
        let mut head2 = Self::reverse_list(mid);
        while head.is_some() { // head 更短，应判断 head 是否为空
            if head.as_ref().unwrap().val != head2.as_ref().unwrap().val {
                return false;
            }
            head = head.unwrap().next;
            head2 = head2.unwrap().next;
        }
        true
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是链表的长度（节点个数）。
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
