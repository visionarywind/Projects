# 92. 反转链表 II

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/reverse-linked-list-ii/
- 题目 slug：`reverse-linked-list-ii`
- 来源专题：链表、树与回溯
- 来源分类路径：一、链表 / §1.4 反转链表
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/reverse-linked-list-ii/solutions/1992226/you-xie-cuo-liao-yi-ge-shi-pin-jiang-tou-teqq/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【视频讲解】反转链表 O(1) 空间写法（Python/Java/C++/C/Go/JS）](https://leetcode.cn/problems/reverse-linked-list-ii/solutions/1992226/you-xie-cuo-liao-yi-ge-shi-pin-jiang-tou-teqq/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`you-xie-cuo-liao-yi-ge-shi-pin-jiang-tou-teqq`
- topic id：`1992226`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:30:25 +0800

**前置题目**：[206. 反转链表](https://leetcode.cn/problems/reverse-linked-list/)

**视频讲解**：请看 [【基础算法精讲 06】](https://www.bilibili.com/video/BV1sd4y1x7KN/)，制作不易，欢迎点赞~

⚠**注意**：$\textit{left}$ 和 $\textit{right}$ 是**位置范围**，不是元素值的范围。

```py [sol-Python3]
class Solution:
    def reverseBetween(self, head: Optional[ListNode], left: int, right: int) -> Optional[ListNode]:
        p0 = dummy = ListNode(next=head)
        # 把 p0 移到 left 的上一个节点
        for _ in range(left - 1):
            p0 = p0.next

        pre = None
        cur = p0.next
        for _ in range(right - left + 1):
            nxt = cur.next
            cur.next = pre  # 每次循环只修改一个 next，方便大家理解
            pre = cur
            cur = nxt

        # 见视频
        p0.next.next = cur
        p0.next = pre
        return dummy.next
```

```java [sol-Java]
class Solution {
    public ListNode reverseBetween(ListNode head, int left, int right) {
        ListNode dummy = new ListNode(0, head);
        ListNode p0 = dummy;
        // 把 p0 移到 left 的上一个节点
        for (int i = 0; i < left - 1; i++) {
            p0 = p0.next;
        }

        ListNode pre = null;
        ListNode cur = p0.next;
        for (int i = 0; i < right - left + 1; i++) {
            ListNode nxt = cur.next;
            cur.next = pre; // 每次循环只修改一个 next，方便大家理解
            pre = cur;
            cur = nxt;
        }

        // 见视频
        p0.next.next = cur;
        p0.next = pre;
        return dummy.next;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    ListNode* reverseBetween(ListNode* head, int left, int right) {
        ListNode dummy(0, head);
        ListNode* p0 = &dummy;
        // 把 p0 移到 left 的上一个节点
        for (int i = 0; i < left - 1; i++) {
            p0 = p0->next;
        }

        ListNode* pre = nullptr;
        ListNode* cur = p0->next;
        for (int i = 0; i < right - left + 1; i++) {
            ListNode* nxt = cur->next;
            cur->next = pre; // 每次循环只修改一个 next，方便大家理解
            pre = cur;
            cur = nxt;
        }

        // 见视频
        p0->next->next = cur;
        p0->next = pre;
        return dummy.next;
    }
};
```

```c [sol-C]
struct ListNode* reverseBetween(struct ListNode* head, int left, int right) {
    struct ListNode dummy = {0, head};
    struct ListNode* p0 = &dummy;
    // 把 p0 移到 left 的上一个节点
    for (int i = 0; i < left - 1; i++) {
        p0 = p0->next;
    }

    struct ListNode* pre = NULL;
    struct ListNode* cur = p0->next;
    for (int i = 0; i < right - left + 1; i++) {
        struct ListNode* nxt = cur->next;
        cur->next = pre; // 每次循环只修改一个 next，方便大家理解
        pre = cur;
        cur = nxt;
    }

    // 见视频
    p0->next->next = cur;
    p0->next = pre;
    return dummy.next;
}
```

```go [sol-Go]
func reverseBetween(head *ListNode, left, right int) *ListNode {
    dummy := ListNode{Next: head}
    p0 := &dummy
    // 把 p0 移到 left 的上一个节点
    for range left - 1 {
        p0 = p0.Next
    }

    var pre *ListNode
    cur := p0.Next
    for range right - left + 1 {
        nxt := cur.Next
        cur.Next = pre // 每次循环只修改一个 Next，方便大家理解
        pre = cur
        cur = nxt
    }

    // 见视频
    p0.Next.Next = cur
    p0.Next = pre
    return dummy.Next
}
```

```js [sol-JavaScript]
var reverseBetween = function(head, left, right) {
    const dummy = new ListNode(0, head);
    let p0 = dummy;
    // 把 p0 移到 left 的上一个节点
    for (let i = 0; i < left - 1; i++) {
        p0 = p0.next;
    }

    let pre = null;
    let cur = p0.next;
    for (let i = 0; i < right - left + 1; i++) {
        const nxt = cur.next;
        cur.next = pre; // 每次循环只修改一个 next，方便大家理解
        pre = cur;
        cur = nxt;
    }

    // 见视频
    p0.next.next = cur;
    p0.next = pre;
    return dummy.next;
};
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(\textit{right})$。
- 空间复杂度：$\mathcal{O}(1)$。

## 分类题单

[如何科学刷题？](https://leetcode.cn/discuss/post/3141566/ru-he-ke-xue-shua-ti-by-endlesscheng-q3yd/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/discuss/post/3578981/ti-dan-hua-dong-chuang-kou-ding-chang-bu-rzz7/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/discuss/post/3579164/ti-dan-er-fen-suan-fa-er-fen-da-an-zui-x-3rqn/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/discuss/post/3579480/ti-dan-dan-diao-zhan-ju-xing-xi-lie-zi-d-u4hk/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/discuss/post/3580195/fen-xiang-gun-ti-dan-wang-ge-tu-dfsbfszo-l3pa/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/discuss/post/3580371/fen-xiang-gun-ti-dan-wei-yun-suan-ji-chu-nth4/)
6. [图论算法（DFS/BFS/拓扑排序/基环树/最短路/最小生成树/网络流）](https://leetcode.cn/discuss/post/3581143/fen-xiang-gun-ti-dan-tu-lun-suan-fa-dfsb-qyux/)
7. [动态规划（入门/背包/划分/状态机/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/discuss/post/3581838/fen-xiang-gun-ti-dan-dong-tai-gui-hua-ru-007o/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/discuss/post/3583665/fen-xiang-gun-ti-dan-chang-yong-shu-ju-j-bvmv/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/discuss/post/3584388/fen-xiang-gun-ti-dan-shu-xue-suan-fa-shu-gcai/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/discuss/post/3091107/fen-xiang-gun-ti-dan-tan-xin-ji-ben-tan-k58yb/)
11. [链表、树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/discuss/post/3142882/fen-xiang-gun-ti-dan-lian-biao-er-cha-sh-6srp/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/discuss/post/3144832/fen-xiang-gun-ti-dan-zi-fu-chuan-kmpzhan-ugt4/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)
