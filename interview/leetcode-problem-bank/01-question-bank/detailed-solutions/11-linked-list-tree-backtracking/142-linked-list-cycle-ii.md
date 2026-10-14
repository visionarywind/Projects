# 142. 环形链表 II

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/linked-list-cycle-ii/
- 题目 slug：`linked-list-cycle-ii`
- 来源专题：链表、树与回溯
- 来源分类路径：一、链表 / §1.6 快慢指针
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/linked-list-cycle-ii/solutions/1999271/mei-xiang-ming-bai-yi-ge-shi-pin-jiang-t-nvsq/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【图解】一张图秒懂环形链表 II，Floyd 判圈算法（Python/Java/C++/C/Go/JS）](https://leetcode.cn/problems/linked-list-cycle-ii/solutions/1999271/mei-xiang-ming-bai-yi-ge-shi-pin-jiang-t-nvsq/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`mei-xiang-ming-bai-yi-ge-shi-pin-jiang-t-nvsq`
- topic id：`1999271`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:30:25 +0800

请先完成上一题 [141. 环形链表](https://leetcode.cn/problems/linked-list-cycle/)。本题继续使用快慢指针的做法。

![图解环形链表](https://pic.leetcode.cn/1741414978-wPTZwJ-lc142-3-c.png)

**注 1**：$kc-a$ 是从**入环口**开始的步数。由于 $(kc-a) + a = kc$，所以从 $kc-a$ 开始，再走 $a$ 步，就恰好走了 $k$ 圈，回到入环口。

**注 2**：慢指针从相遇点开始，移动 $a$ 步后恰好走到入环口，但在这个过程中，可能会多次经过入环口。

**注 3**：这个算法叫做 **Floyd 判圈算法**。

[本题视频讲解【基础算法精讲 07】](https://www.bilibili.com/video/BV1KG4y1G7cu/)

```py [sol-Python3]
class Solution:
    def detectCycle(self, head: Optional[ListNode]) -> Optional[ListNode]:
        slow = fast = head
        while fast and fast.next:
            slow = slow.next
            fast = fast.next.next
            if fast is slow:  # 相遇
                while slow is not head:  # 再走 a 步
                    slow = slow.next
                    head = head.next
                return slow
        return None
```

```java [sol-Java]
class Solution {
    public ListNode detectCycle(ListNode head) {
        ListNode slow = head;
        ListNode fast = head;
        while (fast != null && fast.next != null) {
            slow = slow.next;
            fast = fast.next.next;
            if (fast == slow) { // 相遇
                while (slow != head) { // 再走 a 步
                    slow = slow.next;
                    head = head.next;
                }
                return slow;
            }
        }
        return null;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    ListNode* detectCycle(ListNode* head) {
        ListNode* slow = head;
        ListNode* fast = head;
        while (fast && fast->next) {
            slow = slow->next;
            fast = fast->next->next;
            if (fast == slow) { // 相遇
                while (slow != head) { // 再走 a 步
                    slow = slow->next;
                    head = head->next;
                }
                return slow;
            }
        }
        return nullptr;
    }
};
```

```c [sol-C]
struct ListNode* detectCycle(struct ListNode* head) {
    struct ListNode* slow = head;
    struct ListNode* fast = head;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
        if (fast == slow) { // 相遇
            while (slow != head) { // 再走 a 步
                slow = slow->next;
                head = head->next;
            }
            return slow;
        }
    }
    return NULL;
}
```

```go [sol-Go]
func detectCycle(head *ListNode) *ListNode {
    slow, fast := head, head
    for fast != nil && fast.Next != nil {
        slow = slow.Next
        fast = fast.Next.Next
        if fast == slow { // 相遇
            for slow != head { // 再走 a 步
                slow = slow.Next
                head = head.Next
            }
            return slow
        }
    }
    return nil
}
```

```js [sol-JavaScript]
var detectCycle = function(head) {
    let slow = head, fast = head;
    while (fast && fast.next) {
        slow = slow.next;
        fast = fast.next.next;
        if (fast === slow) { // 相遇
            while (slow !== head) { // 再走 a 步
                slow = slow.next;
                head = head.next;
            }
            return slow;
        }
    }
    return null;
};
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是链表的长度。理由见 [本题视频讲解【基础算法精讲 07】](https://www.bilibili.com/video/BV1KG4y1G7cu/)。
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
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)
