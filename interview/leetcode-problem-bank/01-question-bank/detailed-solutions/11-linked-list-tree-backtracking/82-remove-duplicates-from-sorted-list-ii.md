# 82. 删除排序链表中的重复元素 II

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/remove-duplicates-from-sorted-list-ii/
- 题目 slug：`remove-duplicates-from-sorted-list-ii`
- 来源专题：链表、树与回溯
- 来源分类路径：一、链表 / §1.2 删除节点
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/remove-duplicates-from-sorted-list-ii/solutions/2004067/ru-he-qu-zhong-yi-ge-shi-pin-jiang-tou-p-2ddn/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【视频】如何去重？一个视频讲透！附题单！（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/remove-duplicates-from-sorted-list-ii/solutions/2004067/ru-he-qu-zhong-yi-ge-shi-pin-jiang-tou-p-2ddn/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`ru-he-qu-zhong-yi-ge-shi-pin-jiang-tou-p-2ddn`
- topic id：`2004067`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:30:25 +0800

## 视频讲解

请看[【基础算法精讲 08】](https://www.bilibili.com/video/BV1VP4y1Q71e/)，制作不易，欢迎点赞~

```py [sol-Python3]
class Solution:
    def deleteDuplicates(self, head: Optional[ListNode]) -> Optional[ListNode]:
        cur = dummy = ListNode(next=head)
        while cur.next and cur.next.next:
            val = cur.next.val
            if cur.next.next.val == val:  # 后两个节点值相同
                # 值等于 val 的节点全部删除
                while cur.next and cur.next.val == val:
                    cur.next = cur.next.next
            else:
                cur = cur.next
        return dummy.next
```

```java [sol-Java]
class Solution {
    public ListNode deleteDuplicates(ListNode head) {
        ListNode dummy = new ListNode(0, head);
        ListNode cur = dummy;
        while (cur.next != null && cur.next.next != null) {
            int val = cur.next.val;
            if (cur.next.next.val == val) { // 后两个节点值相同
                // 值等于 val 的节点全部删除
                while (cur.next != null && cur.next.val == val) {
                    cur.next = cur.next.next;
                }
            } else {
                cur = cur.next;
            }
        }
        return dummy.next;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    ListNode* deleteDuplicates(ListNode* head) {
        ListNode dummy(0, head);
        auto cur = &dummy;
        while (cur->next && cur->next->next) {
            int val = cur->next->val;
            if (cur->next->next->val == val) { // 后两个节点值相同
                // 值等于 val 的节点全部删除
                while (cur->next && cur->next->val == val) {
                    cur->next = cur->next->next;
                }
            } else {
                cur = cur->next;
            }
        }
        return dummy.next;
    }
};
```

```cpp [sol-C++ 回收内存]
class Solution {
public:
    ListNode* deleteDuplicates(ListNode* head) {
        ListNode dummy(0, head);
        auto cur = &dummy;
        while (cur->next && cur->next->next) {
            int val = cur->next->val;
            if (cur->next->next->val == val) { // 后两个节点值相同
                // 值等于 val 的节点全部删除
                while (cur->next && cur->next->val == val) {
                    auto nxt = cur->next;
                    cur->next = nxt->next;
                    delete nxt;
                }
            } else {
                cur = cur->next;
            }
        }
        return dummy.next;
    }
};
```

```c [sol-C]
struct ListNode* deleteDuplicates(struct ListNode* head) {
    struct ListNode dummy = {0, head};
    struct ListNode* cur = &dummy;
    while (cur->next && cur->next->next) {
        int val = cur->next->val;
        if (cur->next->next->val == val) { // 后两个节点值相同
            // 值等于 val 的节点全部删除
            while (cur->next && cur->next->val == val) {
                cur->next = cur->next->next;
            }
        } else {
            cur = cur->next;
        }
    }
    return dummy.next;
}
```

```go [sol-Go]
func deleteDuplicates(head *ListNode) *ListNode {
    dummy := &ListNode{Next: head}
    cur := dummy
    for cur.Next != nil && cur.Next.Next != nil {
        val := cur.Next.Val
        if cur.Next.Next.Val == val { // 后两个节点值相同
            // 值等于 val 的节点全部删除
            for cur.Next != nil && cur.Next.Val == val {
                cur.Next = cur.Next.Next
            }
        } else {
            cur = cur.Next
        }
    }
    return dummy.Next
}
```

```js [sol-JavaScript]
var deleteDuplicates = function(head) {
    const dummy = new ListNode(0, head);
    let cur = dummy;
    while (cur.next && cur.next.next) {
        let val = cur.next.val;
        if (cur.next.next.val === val) { // 后两个节点值相同
            // 值等于 val 的节点全部删除
            while (cur.next && cur.next.val === val) {
                cur.next = cur.next.next;
            }
        } else {
            cur = cur.next;
        }
    }
    return dummy.next;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn delete_duplicates(head: Option<Box<ListNode>>) -> Option<Box<ListNode>> {
        let mut dummy = ListNode { val: 0, next: head };
        let mut cur = &mut dummy;
        while let Some(ref nxt) = cur.next {
            if let Some(ref nxt_nxt) = nxt.next {
                let val = nxt.val;
                if nxt_nxt.val == val { // 后两个节点值相同
                    // 值等于 val 的节点全部删除
                    while cur.next.is_some() && cur.next.as_ref()?.val == val {
                        cur.next = cur.next.take()?.next;
                    }
                } else {
                    cur = cur.next.as_mut()?;
                }
            } else {
                break;
            }
        }
        dummy.next
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为链表的长度。
- 空间复杂度：$\mathcal{O}(1)$，仅用到若干额外变量。

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

本题来自 `一、链表 / §1.2 删除节点`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、链表 / §1.2 删除节点`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
