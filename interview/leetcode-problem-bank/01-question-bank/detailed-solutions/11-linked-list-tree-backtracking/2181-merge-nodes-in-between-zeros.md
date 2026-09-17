# 2181. 合并零之间的节点

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/merge-nodes-in-between-zeros/
- 题目 slug：`merge-nodes-in-between-zeros`
- 来源专题：链表、树与回溯
- 来源分类路径：一、链表 / §1.1 遍历链表
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/merge-nodes-in-between-zeros/solutions/1278727/jian-ji-xie-fa-by-endlesscheng-c4gf/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[原地做法，O(1) 空间（Python/Java/C++/C/Go/JS）](https://leetcode.cn/problems/merge-nodes-in-between-zeros/solutions/1278727/jian-ji-xie-fa-by-endlesscheng-c4gf/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`jian-ji-xie-fa-by-endlesscheng-c4gf`
- topic id：`1278727`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:22:48 +0800

![lc2181.png](https://pic.leetcode.cn/1725600294-hlvzrG-lc2181.png)

累加两个 $0$ 之间的节点值，加到哪里？

如上图，可以把 $3,1$ 的和填到第一个节点 $0$ 上，把 $4,5,2$ 的和填到第二个节点 $3$ 上。注意当我们遍历 $4,5,2$ 时，节点 $3$ 已经遍历过了，可以直接作为保存累加值的节点。

具体算法如下：

1. 把 $\textit{head}$ 当作答案链表的头节点。在一开始，$\textit{head}$ 也是答案链表的末尾节点 $\textit{tail}$。
2. 从 $\textit{head}.\textit{next}$ 开始遍历链表。
3. 如果当前节点的值不为 $0$，则把节点值加到 $\textit{tail}.\textit{val}$ 中。
4. 如果当前节点的值等于 $0$，则更新 $\textit{tail}$ 为 $\textit{tail}.\textit{next}$，然后把 $\textit{tail}.\textit{val}$ 置为 $0$。由于我们本质上是在合并节点值，$\textit{tail}.\textit{next}$ 这个节点是一定存在的。
5. 继续向后遍历链表。
6. 如果当前节点的下一个节点为空，说明我们遍历到了链表的最后一个节点 $0$，遍历结束。否则回到第 3 步。
7. 由于我们是在原链表上修改的，原链表的最后一个节点 $0$ 必然不在答案链表中，所以答案链表一定比原链表短。把 $\textit{tail}.\textit{next}$ 置为空，以确保答案链表不包含原链表中多余的节点。
8. 最后返回 $\textit{head}$，即答案链表的头节点。

```py [sol-Python3]
class Solution:
    def mergeNodes(self, head: Optional[ListNode]) -> Optional[ListNode]:
        tail = head
        cur = head.next
        while cur.next:
            if cur.val:
                tail.val += cur.val
            else:
                tail = tail.next
                tail.val = 0
            cur = cur.next
        tail.next = None
        return head
```

```java [sol-Java]
class Solution {
    public ListNode mergeNodes(ListNode head) {
        ListNode tail = head;
        for (ListNode cur = head.next; cur.next != null; cur = cur.next) {
            if (cur.val != 0) {
                tail.val += cur.val;
            } else {
                tail = tail.next;
                tail.val = 0;
            }
        }
        tail.next = null;
        return head;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    ListNode* mergeNodes(ListNode* head) {
        auto tail = head;
        for (auto cur = head->next; cur->next; cur = cur->next) {
            if (cur->val) {
                tail->val += cur->val;
            } else {
                tail = tail->next;
                tail->val = 0;
            }
        }
        // 注：这里没有 delete 剩余节点，可以自行补充
        tail->next = nullptr;
        return head;
    }
};
```

```c [sol-C]
struct ListNode* mergeNodes(struct ListNode* head) {
    struct ListNode* tail = head;
    for (struct ListNode* cur = head->next; cur->next; cur = cur->next) {
        if (cur->val) {
            tail->val += cur->val;
        } else {
            tail = tail->next;
            tail->val = 0;
        }
    }
    // 注：这里没有 free 剩余节点，可以自行补充
    tail->next = NULL;
    return head;
}
```

```go [sol-Go]
func mergeNodes(head *ListNode) *ListNode {
    tail := head
    for cur := head.Next; cur.Next != nil; cur = cur.Next {
        if cur.Val != 0 {
            tail.Val += cur.Val
        } else {
            tail = tail.Next
            tail.Val = 0
        }
    }
    tail.Next = nil
    return head
}
```

```js [sol-JavaScript]
var mergeNodes = function(head) {
    let tail = head;
    for (let cur = head.next; cur.next; cur = cur.next) {
        if (cur.val) {
            tail.val += cur.val;
        } else {
            tail = tail.next;
            tail.val = 0;
        }
    }
    tail.next = null;
    return head;
};
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是链表长度。
- 空间复杂度：$\mathcal{O}(1)$。没有创建新的节点。

更多相似题目，见下面的链表题单。

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

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `一、链表 / §1.1 遍历链表`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、链表 / §1.1 遍历链表`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
