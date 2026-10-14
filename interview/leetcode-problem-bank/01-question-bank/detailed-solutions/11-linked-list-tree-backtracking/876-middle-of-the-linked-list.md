# 876. 链表的中间结点

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/middle-of-the-linked-list/
- 题目 slug：`middle-of-the-linked-list`
- 来源专题：链表、树与回溯
- 来源分类路径：一、链表 / §1.6 快慢指针
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/middle-of-the-linked-list/solutions/1999265/mei-xiang-ming-bai-yi-ge-shi-pin-jiang-t-wzwm/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[龟兔赛跑（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/middle-of-the-linked-list/solutions/1999265/mei-xiang-ming-bai-yi-ge-shi-pin-jiang-t-wzwm/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`mei-xiang-ming-bai-yi-ge-shi-pin-jiang-t-wzwm`
- topic id：`1999265`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:30:25 +0800

龟兔赛跑。如果兔子速度恰好比乌龟速度快一倍，那么当兔子到达终点时，乌龟走了一半。

用两个指针模仿龟兔赛跑。都从 $\textit{head}$ 出发，快指针每次走两步，慢指针每次走一步。**当快指针移动到链表的最后一个节点，或者最后一个节点的下一个节点（空节点）时，停止**。

如果链表长度 $n$ 是奇数，那么快指针最终会停在最后一个节点，移动 $\left\lfloor\dfrac{n}{2}\right\rfloor$ 次，此时慢指针停在第 $1+\left\lfloor\dfrac{n}{2}\right\rfloor$ 个节点处，即链表的中间节点。如下图，$n=5$，慢指针停在第 $1+\left\lfloor\dfrac{n}{2}\right\rfloor=3$ 个节点处。

![lc876-1.jpg](https://pic.leetcode.cn/1780361500-uENvun-lc876-1.jpg)

如果链表长度 $n$ 是偶数，那么快指针最终会停在最后一个节点的下一个节点（空节点），移动 $\dfrac{n}{2}$ 次，此时慢指针停在第 $1+\dfrac{n}{2}$ 个节点处，即链表的中间节点。如下图，$n=6$，慢指针停在第 $1+\dfrac{n}{2}=4$ 个节点处。

![lc876-2.jpg](https://pic.leetcode.cn/1780361507-jwpXKJ-lc876-2.jpg)

本题视频讲解：[【基础算法精讲 07】](https://www.bilibili.com/video/BV1KG4y1G7cu/)。

```py [sol-Python3]
class Solution:
    def middleNode(self, head: Optional[ListNode]) -> Optional[ListNode]:
        slow = fast = head
        while fast and fast.next:
            slow = slow.next  # 乌龟走一步
            fast = fast.next.next  # 兔子走两步
        return slow  # 兔子到达终点时，乌龟走了一半，在中间位置
```

```java [sol-Java]
class Solution {
    public ListNode middleNode(ListNode head) {
        ListNode slow = head;
        ListNode fast = head;
        while (fast != null && fast.next != null) {
            slow = slow.next; // 乌龟走一步
            fast = fast.next.next; // 兔子走两步
        }
        return slow; // 兔子到达终点时，乌龟走了一半，在中间位置
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    ListNode* middleNode(ListNode* head) {
        ListNode* slow = head;
        ListNode* fast = head;
        while (fast && fast->next) {
            slow = slow->next; // 乌龟走一步
            fast = fast->next->next; // 兔子走两步
        }
        return slow; // 兔子到达终点时，乌龟走了一半，在中间位置
    }
};
```

```c [sol-C]
struct ListNode* middleNode(struct ListNode* head) {
    struct ListNode* slow = head;
    struct ListNode* fast = head;
    while (fast && fast->next) {
        slow = slow->next; // 乌龟走一步
        fast = fast->next->next; // 兔子走两步
    }
    return slow; // 兔子到达终点时，乌龟走了一半，在中间位置
}
```

```go [sol-Go]
func middleNode(head *ListNode) *ListNode {
    slow, fast := head, head
    for fast != nil && fast.Next != nil {
        slow = slow.Next      // 乌龟走一步
        fast = fast.Next.Next // 兔子走两步
    }
    return slow // 兔子到达终点时，乌龟走了一半，在中间位置
}
```

```js [sol-JS]
var middleNode = function(head) {
    let slow = head;
    let fast = head;
    while (fast && fast.next) {
        slow = slow.next; // 乌龟走一步
        fast = fast.next.next; // 兔子走两步
    }
    return slow; // 兔子到达终点时，乌龟走了一半，在中间位置
};
```

```rust [sol-Rust]
impl Solution {
    pub fn middle_node(head: Option<Box<ListNode>>) -> Option<Box<ListNode>> {
        let mut fast = &head;
        let mut slow = &head;
        while fast.is_some() && fast.as_ref()?.next.is_some() {
            slow = &slow.as_ref()?.next; // 乌龟走一步
            fast = &fast.as_ref()?.next.as_ref()?.next; // 兔子走两步
        }
        slow.clone() // 不 clone 的写法见【Rust 写法二】和【Rust 写法三】
    }
}
```

```rust [sol-Rust 写法二]
impl Solution {
    pub fn middle_node(head: Option<Box<ListNode>>) -> Option<Box<ListNode>> {
        let mut fast = &head;
        let mut slow = &head;
        while fast.is_some() && fast.as_ref()?.next.is_some() {
            slow = &slow.as_ref()?.next; // 乌龟走一步
            fast = &fast.as_ref()?.next.as_ref()?.next; // 兔子走两步
        }
        // 只读引用 -> 只读裸指针 -> 可变裸指针
        let mut slow = slow as *const Option<Box<ListNode>> as *mut Option<Box<ListNode>>;
        // 可变裸指针 -> 可变引用
        let slow = unsafe { &mut *slow };
        slow.take()
    }
}
```

```rust [sol-Rust 写法三]
impl Solution {
    pub fn middle_node(mut head: Option<Box<ListNode>>) -> Option<Box<ListNode>> {
        let mut fast = &head;
        let mut slow = &head;
        while fast.is_some() && fast.as_ref()?.next.is_some() {
            slow = &slow.as_ref()?.next; // 乌龟走一步
            fast = &fast.as_ref()?.next.as_ref()?.next; // 兔子走两步
        }
        // 把 slow 从 &Option<Box<ListNode>> 直接强转成 &mut Option<Box<ListNode>>
        #[allow(mutable_transmutes)]
        let slow: &mut Option<Box<ListNode>> = unsafe { std::mem::transmute(slow) };
        slow.take()
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是链表的长度。
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
11. [链表、树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)
