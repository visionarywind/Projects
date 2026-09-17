# 138. 随机链表的复制

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/copy-list-with-random-pointer/
- 题目 slug：`copy-list-with-random-pointer`
- 来源专题：链表、树与回溯
- 来源分类路径：一、链表 / §1.11 其他
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/copy-list-with-random-pointer/solutions/2993775/bu-yong-ha-xi-biao-de-zuo-fa-pythonjavac-nzdo/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[不用哈希表，简洁写法（Python/Java/C++/C/Go/JS）](https://leetcode.cn/problems/copy-list-with-random-pointer/solutions/2993775/bu-yong-ha-xi-biao-de-zuo-fa-pythonjavac-nzdo/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`bu-yong-ha-xi-biao-de-zuo-fa-pythonjavac-nzdo`
- topic id：`2993775`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:30:25 +0800

## 题意

深拷贝一个链表，要求新链表中的每个节点都是新创建的，并且这些节点的 $\textit{random}$ 指针都指向新链表中的相应节点。

## 思路

如果没有 $\textit{random}$ 指针，只需在遍历链表的同时，依次复制每个节点（创建新节点并复制 $\textit{val}$），添加在新链表的末尾。

有 $\textit{random}$ 指针，问题就变得复杂了，我们需要知道 $\textit{random}$ 指向的那个节点，在新链表中是哪个节点。

所以必须记录原链表节点到新链表节点的映射（map）。这样可以通过原链表 $\textit{random}$ 指向的节点，知道新链表的 $\textit{random}$ 应该指向哪个节点。

难道要用哈希表吗？不需要，我们可以把新链表和旧链表「混在一起」。

例如链表 $1\to 2\to 3$，依次复制每个节点（创建新节点并复制 $\textit{val}$ 和 $\textit{next}$），把新节点直接插到原节点的后面，形成一个**交错链表**：

$$
1\to1'\to 2\to 2'\to 3\to 3'
$$

如此一来，**原链表节点的下一个节点，就是其对应的新链表节点了**！

然后遍历这个交错链表，假如节点 $1$ 的 $\textit{random}$ 指向节点 $3$，那么就把新节点 $1'$ 的 $\textit{random}$ 指向节点 $3$ 的下一个节点 $3'$，这样就完成了对 $\textit{random}$ 指针的复制。

最后，从交错链表中**分离**出 $1'\to 2'\to 3'$，即为深拷贝后的链表。做法类似 [328. 奇偶链表](https://leetcode.cn/problems/odd-even-linked-list/)。

⚠**注意**：不能只删除节点 $1,2,3$，因为题目要求原链表的 $\textit{next}$ 不能修改。（用 Python 的同学可以先看第一份代码，再看第二份代码）

```py [sol-Python3]
class Solution:
    def copyRandomList(self, head: 'Optional[Node]') -> 'Optional[Node]':
        # 复制每个节点，把新节点直接插到原节点的后面
        cur = head
        while cur:
            cur.next = Node(cur.val, cur.next)
            cur = cur.next.next

        # 遍历交错链表中的原链表节点
        cur = head
        while cur:
            if cur.random:
                # 要复制的 random 是 cur.random 的下一个节点
                cur.next.random = cur.random.next
            cur = cur.next.next

        # 删除交错链表中的原链表节点，剩下的节点即为新链表
        cur = dummy = Node(0, head)
        while cur.next:
            # 删除原链表的节点，即当前节点的下一个节点
            # 如果要恢复原链表，见另一份代码【Python3 写法二】
            cur.next = cur.next.next
            cur = cur.next

        return dummy.next
```

```py [sol-Python3 写法二]
class Solution:
    def copyRandomList(self, head: 'Optional[Node]') -> 'Optional[Node]':
        # 复制每个节点，把新节点直接插到原节点的后面
        cur = head
        while cur:
            cur.next = Node(cur.val, cur.next)
            cur = cur.next.next

        # 遍历交错链表中的原链表节点
        cur = head
        while cur:
            if cur.random:
                # 要复制的 random 是 cur.random 的下一个节点
                cur.next.random = cur.random.next
            cur = cur.next.next

        # 把交错链表分离成两个链表
        tail = dummy = Node(0, head)
        cur = head
        while cur:
            copy = cur.next  # 新节点
            tail.next = copy  # 把新节点插在 tail 的后面，构建新的链表
            cur.next = copy.next  # 恢复原节点的 next
            cur = cur.next
            tail = tail.next

        return dummy.next
```

```java [sol-Java]
class Solution {
    public Node copyRandomList(Node head) {
        // 复制每个节点，把新节点直接插到原节点的后面
        for (Node cur = head; cur != null; cur = cur.next.next) {
            cur.next = new Node(cur.val, cur.next);
        }

        // 遍历交错链表中的原链表节点
        for (Node cur = head; cur != null; cur = cur.next.next) {
            if (cur.random != null) {
                // 要复制的 random 是 cur.random 的下一个节点
                cur.next.random = cur.random.next;
            }
        }

        // 把交错链表分离成两个链表
        Node dummy = new Node(0);
        Node tail = dummy;
        for (Node cur = head; cur != null; cur = cur.next, tail = tail.next) {
            Node copy = cur.next; // 新节点
            tail.next = copy; // 把新节点插在 tail 的后面，构建新的链表
            cur.next = copy.next; // 恢复原节点的 next
        }

        return dummy.next;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    Node* copyRandomList(Node* head) {
        // 复制每个节点，把新节点直接插到原节点的后面
        for (Node* cur = head; cur; cur = cur->next->next) {
            cur->next = new Node(cur->val, cur->next, nullptr);
        }

        // 遍历交错链表中的原链表节点
        for (Node* cur = head; cur; cur = cur->next->next) {
            if (cur->random) {
                // 要复制的 random 是 cur->random 的下一个节点
                cur->next->random = cur->random->next;
            }
        }

        // 把交错链表分离成两个链表
        Node dummy(0);
        Node* tail = &dummy;
        for (Node* cur = head; cur; cur = cur->next, tail = tail->next) {
            Node* copy = cur->next; // 新节点
            tail->next = copy; // 把新节点插在 tail 的后面，构建新的链表
            cur->next = copy->next; // 恢复原节点的 next
        }

        return dummy.next;
    }
};
```

```c [sol-C]
struct Node* copyRandomList(struct Node* head) {
    // 复制每个节点，把新节点直接插到原节点的后面
    for (struct Node* cur = head; cur; cur = cur->next->next) {
        struct Node* copy = malloc(sizeof(struct Node));
        copy->val = cur->val;
        copy->next = cur->next;
        copy->random = NULL;
        cur->next = copy;
    }

    // 遍历交错链表中的原链表节点
    for (struct Node* cur = head; cur; cur = cur->next->next) {
        if (cur->random) {
            // 要复制的 random 是 cur->random 的下一个节点
            cur->next->random = cur->random->next;
        }
    }

    // 把交错链表分离成两个链表
    struct Node dummy;
    struct Node* tail = &dummy;
    for (struct Node* cur = head; cur; cur = cur->next, tail = tail->next) {
        struct Node* copy = cur->next; // 新节点
        tail->next = copy; // 把新节点插在 tail 的后面，构建新的链表
        cur->next = copy->next; // 恢复原节点的 next
    }

    return dummy.next;
}
```

```go [sol-Go]
func copyRandomList(head *Node) *Node {
    // 复制每个节点，把新节点直接插到原节点的后面
    for cur := head; cur != nil; cur = cur.Next.Next {
        cur.Next = &Node{Val: cur.Val, Next: cur.Next}
    }

    // 遍历交错链表中的原链表节点
    for cur := head; cur != nil; cur = cur.Next.Next {
        if cur.Random != nil {
            // 要复制的 random 是 cur.Random 的下一个节点
            cur.Next.Random = cur.Random.Next
        }
    }

    // 把交错链表分离成两个链表
    dummy := Node{}
    tail := &dummy
    for cur := head; cur != nil; cur, tail = cur.Next, tail.Next {
        clone := cur.Next     // 新节点
        tail.Next = clone     // 把新节点插在 tail 的后面，构建新的链表
        cur.Next = clone.Next // 恢复原节点的 next
    }

    return dummy.Next
}
```

```js [sol-JavaScript]
var copyRandomList = function(head) {
    // 复制每个节点，把新节点直接插到原节点的后面
    for (let cur = head; cur; cur = cur.next.next) {
        cur.next = new _Node(cur.val, cur.next, null);
    }

    // 遍历交错链表中的原链表节点
    for (let cur = head; cur; cur = cur.next.next) {
        if (cur.random) {
            // 要复制的 random 是 cur.random 的下一个节点
            cur.next.random = cur.random.next;
        }
    }

    // 把交错链表分离成两个链表
    const dummy = new _Node();
    let tail = dummy;
    for (let cur = head; cur; cur = cur.next, tail = tail.next) {
        const copy = cur.next; // 新节点
        tail.next = copy; // 把新节点插在 tail 的后面，构建新的链表
        cur.next = copy.next; // 恢复原节点的 next
    }

    return dummy.next;
};
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是链表的长度。
- 空间复杂度：$\mathcal{O}(1)$。返回值不计入。

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

本题来自 `一、链表 / §1.11 其他`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、链表 / §1.11 其他`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
